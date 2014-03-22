//***************************************************************************
//  NARS2000 -- Multi Precision Infinity Functions
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
#define MPIFNS
#include <windows.h>
#include <math.h>               // For fabs
#include "headers.h"

typedef mpir_ui (*MPZ_INVALID)  (enum MP_ENUM, mpz_t , mpz_t , mpz_t , mpz_t , mpir_ui, mpir_ui);
typedef mpir_ui (*MPQ_INVALID)  (enum MP_ENUM, mpq_t , mpq_t , mpq_t , mpq_t , mpir_ui         );
typedef mpir_ui (*MPFR_INVALID) (enum MP_ENUM, mpfr_t, mpfr_t, mpfr_t, mpfr_t, mpir_ui         );

MPZ_INVALID  gmpz_invalid  = &mpz_exit;
MPQ_INVALID  gmpq_invalid  = &mpq_exit;
MPFR_INVALID gmpfr_invalid = &mpfr_exit;

#define IsInfinity(a)       (!_finite (a) && !_isnan (a))

#define mpfr_clr_inf(a)     (a)


//***************************************************************************
//  Generic Functions
//***************************************************************************

//***************************************************************************
//  $mp_get_invalid_functions
//
//  Return the current invalid operation function(s)
//***************************************************************************

void mp_get_invalid_functions
    (mpir_ui (**mpz_invalid)  (enum tagMP_ENUM, mpz_t , mpz_t , mpz_t , mpz_t , mpir_ui, mpir_ui),
     mpir_ui (**mpq_invalid)  (enum tagMP_ENUM, mpq_t , mpq_t , mpq_t , mpq_t , mpir_ui         ),
     mpir_ui (**mpfr_invalid) (enum tagMP_ENUM, mpfr_t, mpfr_t, mpfr_t, mpfr_t, mpir_ui         ))

{
    if (mpz_invalid)
        *mpz_invalid  = gmpz_invalid;
    if (mpq_invalid)
        *mpq_invalid  = gmpq_invalid;
    if (mpfr_invalid)
        *mpfr_invalid = gmpfr_invalid;
} // End mp_get_invalid_functions


//***************************************************************************
//  $mp_set_invalid_functions
//
//  Save the current invalid operation function(s)
//***************************************************************************

void mp_set_invalid_functions
    (mpir_ui (*mpz_invalid)  (enum tagMP_ENUM, mpz_t , mpz_t , mpz_t , mpz_t , mpir_ui, mpir_ui),
     mpir_ui (*mpq_invalid)  (enum tagMP_ENUM, mpq_t , mpq_t , mpq_t , mpq_t , mpir_ui         ),
     mpir_ui (*mpfr_invalid) (enum tagMP_ENUM, mpfr_t, mpfr_t, mpfr_t, mpfr_t, mpir_ui         ))

{
    if (mpz_invalid)
        gmpz_invalid  = mpz_invalid;
    if (mpq_invalid)
        gmpq_invalid  = mpq_invalid;
    if (mpfr_invalid)
        gmpfr_invalid = mpfr_invalid;
} // End mp_set_invalid_functions


//***************************************************************************
//  $signumint
//
//  Return -1, 0, +1 depending upon the sign of a give integer
//***************************************************************************

int signumint
    (mp_size_t val)             // Source

{
    if (val < 0)
        return -1;
    else
       return (val > 0);
} // End signumint


//***************************************************************************
//  $signumflt
//
//  Return -1, 0, +1 depending upon the sign of a give float
//***************************************************************************

int signumflt
    (double val)

{
    if (val < 0)
        return -1;
    else
       return (val > 0);
} // End signumflt


//***************************************************************************
//  MPZ_ Functions
//***************************************************************************

//***************************************************************************
//  &mpz_exit
//***************************************************************************

mpir_ui mpz_exit
    (enum MP_ENUM mp_enum,
     mpz_t   rop,
     mpz_t   op1,
     mpz_t   op2,
     mpz_t   op3,
     mpir_ui a,
     mpir_ui b)

{
    exit (0);

    return 0;           // To keep the compiler happy
} // End mpz_exit


//***************************************************************************
//  $mpz_inf_p
//
//  Is the given value an infinity?
//***************************************************************************

int mpz_inf_p
    (mpz_t op)                  // Source

{
    return ((op->_mp_size EQ (mp_size_t) INT_MAX
          || op->_mp_size EQ (mp_size_t) INT_MIN)
         && op->_mp_alloc EQ 0
         && op->_mp_d     EQ NULL);
} // End mpz_inf_p


//***************************************************************************
//  $IsMpzNULL
//
//  Is the given value all zero?
//***************************************************************************

int IsMpzNULL
    (mpz_t op)                  // Source

{
    return (op->_mp_size  EQ 0
         && op->_mp_alloc EQ 0
         && op->_mp_d     EQ NULL);
} // End IsMpzNULL


//***************************************************************************
//  $mpz_set_inf
//
//  Set the argument to +/-infinity
//***************************************************************************

void mpz_set_inf
    (mpz_t rop,                 // Destination
     int   sgn)                 // Sign:  >= 0 for +Infinity, < 0 for -Infinity

{
    // If it's not infinite, ...
    if (!mpz_inf_p (rop))
        // Free the value as we'll use a special format
        mpz_clear (rop);

    // Set the numerator to a special format, properly signed
    rop->_mp_size  = (sgn >= 0) ? (mp_size_t) INT_MAX
                                : (mp_size_t) INT_MIN;
    rop->_mp_alloc = 0;
    rop->_mp_d     = NULL;
} // mpz_set_inf


//***************************************************************************
//  $mpz_clr_inf
//
//  Initialize op if it's an infinity
//***************************************************************************

mpz_ptr mpz_clr_inf
    (mpz_t op)                  // Source

{
    if (mpz_inf_p (op))
        mpz_init (op);

    return op;
} // End mpz_clr_inf


//***************************************************************************
//  $mpiz_set
//
//  Set the integer value from an integer
//***************************************************************************

void mpiz_set
    (mpz_t rop,                 // Destination
     mpz_t op)                  // Source

{
    if (mpz_inf_p (op))
        mpz_set_inf (rop, mpz_sgn (op));
    else
        mpz_set (mpz_clr_inf (rop), op);
} // End mpiz_set


//***************************************************************************
//  $mpiz_set_ui
//
//  Set the integer value from an mpir_ui
//***************************************************************************

void mpiz_set_ui
    (mpz_t   rop,               // Destination
     mpir_ui op)                // Source

{
    mpz_set_ui (mpz_clr_inf (rop), op);
} // End mpiz_set_ui


//***************************************************************************
//  $mpiz_set_si
//
//  Set the integer value from an mpir_si
//***************************************************************************

void mpiz_set_si
    (mpz_t   rop,               // Destination
     mpir_si op)                // Source

{
    mpz_set_si (mpz_clr_inf (rop), op);
} // End mpiz_set_si


//***************************************************************************
//  $mpiz_set_d
//
//  Set the integer value from a double
//***************************************************************************

void mpiz_set_d
    (mpz_t  rop,                // Destination
     double op)                 // Source

{
    if (IsInfinity (op))
        mpz_set_inf (rop, (op EQ fabs (op)) ? 1 : -1);
    else
        mpz_set_d (mpz_clr_inf (rop), op);
} // End mpiz_set_d


//***************************************************************************
//  $mpiz_set_q
//
//  Set the integer value from a rational
//***************************************************************************

void mpiz_set_q
    (mpz_t rop,                 // Destination
     mpq_t op)                  // Source

{
    if (mpq_inf_p (op))
        mpz_set_inf (rop, mpq_sgn (op));
    else
        mpz_set_q (mpz_clr_inf (rop), op);
} // End mpiz_set_q


//***************************************************************************
//  $mpiz_set_f
//
//  Set the integer value from a MPFR
//***************************************************************************

void mpiz_set_fr
    (mpz_t     rop,             // Destination
     mpfr_t     op,             // Source
     mpfr_rnd_t rnd)            // Rounding mode

{
    if (mpfr_inf_p (op))
        mpz_set_inf (rop, mpfr_sgn (op));
    else
        mpz_set_fr (mpz_clr_inf (rop), op, rnd);
} // End mpiz_set_fr


//***************************************************************************
//  $mpiz_set_str
//
//  Set rop from a string in a given base
//***************************************************************************

int mpiz_set_str
    (mpz_t rop,                 // Destination
     char *str,                 // Source
     int   base)                // Base

{
    char *p;

    // Skip over white space
    p = str;
    while (isspace (*p))
        p++;
    // If the input consists of "!" or "-!", ...
    if (strcmp (p, DEF_POSINFINITY_STR) EQ 0
     || strcmp (p, DEF_NEGINFINITY_STR) EQ 0)
    {
        // Set to the appropriate signed infinity
        mpz_set_inf (rop, p[0] EQ '-');

        return 0;
    } else
        return mpz_set_str (mpz_clr_inf (rop), str, base);
} // End mpiz_set_str


//***************************************************************************
//  $mpiz_init_set
//
//  Set the integer value from an integer
//***************************************************************************

void mpiz_init_set
    (mpz_t rop,                 // Destination
     mpz_t op)                  // Source

{
    if (mpz_inf_p (op))
    {
        mpz_init (rop);
        mpz_set_inf (rop, mpz_sgn (op));
    } else
        mpz_init_set (rop, op);
} // End mpiz_init_set


//***************************************************************************
//  $mpiz_init_set_d
//
//  Set the integer value from a double
//***************************************************************************

void mpiz_init_set_d
    (mpz_t  rop,                // Destination
     double op)                 // Source

{
    if (IsInfinity (op))
    {
        mpz_init (rop);
        mpz_set_inf (rop, (op EQ fabs (op)) ? 1 : -1);
    } else
        mpz_init_set_d (rop, op);
} // End mpiz_init_set_d


//***************************************************************************
//  $mpiz_get_ui
//
//  Return the arg as an unsigned integer
//***************************************************************************

mpir_ui mpiz_get_ui
    (mpz_t op)                  // Source

{
    if (mpz_inf_p (op))
        return (mpir_ui )0xFFFFFFFF;
    else
        return mpz_get_ui (mpz_clr_inf (op));
} // End mpiz_get_ui


//***************************************************************************
//  $mpiz_get_si
//
//  Return the arg as an signed integer
//***************************************************************************

mpir_si mpiz_get_si
    (mpz_t op)                  // Source

{
    if (mpz_inf_p (op))
        return (mpir_si) 0xFFFFFFFF;
    else
        return mpz_get_si (mpz_clr_inf (op));
} // End mpiz_get_si


//***************************************************************************
//  $mpiz_get_d
//
//  Return the arg as a double
//***************************************************************************

double mpiz_get_d
    (mpz_t op)                  // Source

{
    if (mpz_inf_p (op))
    {
        __int64 Int64;

        Int64 = (op > 0) ? POS_INFINITY
                         : NEG_INFINITY;
        return *(double *) &Int64;
    } else
        return mpz_get_d (mpz_clr_inf (op));
} // End mpiz_get_d


//***************************************************************************
//  $mpiz_get_d_2exp
//
//  Return the arg as a double
//***************************************************************************

double mpiz_get_d_2exp
    (mpir_si *exp,              // Ptr to exponent
     mpz_t    op)               // Source

{
    if (mpz_inf_p (op))
    {
        __int64 Int64;

        *exp = (mpir_si) 0xFFFFFFFF;

        Int64 = (op > 0) ? POS_INFINITY
                         : NEG_INFINITY;
        return *(double *) &Int64;
    } else
        return mpz_get_d_2exp (exp, mpz_clr_inf (op));
} // End mpiz_get_d_2exp


//***************************************************************************
//  $mpiz_get_str
//
//  Convert op to a string of digits in a given base
//***************************************************************************

char *mpiz_get_str
    (char *str,                 // Ptr to output string
     int   base,                // Number base
     mpz_t op)                  // Source

{
    if (mpz_inf_p (op))
    {
        __mpz_struct tmp = {0};
        char *p;

        // Initialize and set a temp to "1" or "-1"
        //   which is of sufficient size to store "!" or "-!"
        mpz_init_set_si (&tmp, mpz_sgn (op));

        // Convert to "1" or "-1" using the caller's <str>
        p = mpz_get_str (str, base, &tmp);

        // Change the "1" to the inifnity char
        p[p[0] EQ '-'] = DEF_POSINFINITY_CHAR;

        // We no longer need this storage
        Myz_clear (&tmp);

        return p;
    } else
        return mpz_get_str (str, base, mpz_clr_inf (op));
} // End mpiz_get_str


//***************************************************************************
//  $mpiz_add
//
//  Add two integers
//***************************************************************************

void mpiz_add
    (mpz_t rop,                 // Destination
     mpz_t op1,                 // Left arg
     mpz_t op2)                 // Right arg

{
    // Split cases based upon which (if any) arg is an infinity
    switch (2 * mpz_inf_p (op1) + mpz_inf_p (op2))
    {
        case 2 * 0 + 0:     // Neither arg is an infinity
            // Call the original function
            mpz_add (mpz_clr_inf (rop), op1, op2);

            break;

        case 2 * 0 + 1:     // Op2 only is an infinity
            // The result is the infinite argument (op2)
            mpz_set_inf (rop, mpz_sgn (op2));

            break;

        case 2 * 1 + 0:     // Op1 only is an infinity
            // The result is the infinite argument (op1)
            mpz_set_inf (rop, mpz_sgn (op1));

            break;

        case 2 * 1 + 1:     // Op1 and Op2 are infinities
            // If they are the same sign, ...
            if (mpz_sgn (op1) EQ mpz_sgn (op2))
                mpz_set_inf (rop, mpz_sgn (op1));
            else
                (*gmpz_invalid) (MP_ADD, rop, op1, op2, NULL, 0, 0);
            break;
    } // End SWITCH
} // End mpiz_add


//***************************************************************************
//  $mpiz_add_ui
//
//  Add an integer and an mpir_ui
//***************************************************************************

void mpiz_add_ui
    (mpz_t   rop,               // Destination
     mpz_t   op1,               // Left arg
     mpir_ui op2)               // Right arg

{
    if (mpz_inf_p (op1))
        mpz_set_inf (rop, mpz_sgn (op1));
    else
        mpz_add_ui (mpz_clr_inf (rop), op1, op2);
} // End mpiz_add_ui


//***************************************************************************
//  $mpiz_sub
//
//  Subtract two integers
//***************************************************************************

void mpiz_sub
    (mpz_t rop,                 // Destination
     mpz_t op1,                 // Left arg
     mpz_t op2)                 // Right arg

{
    // Split cases based upon which (if any) arg is an infinity
    switch (2 * mpz_inf_p (op1) + mpz_inf_p (op2))
    {
        case 2 * 0 + 0:     // Neither arg is an infinity
            // Call the original function
            mpz_sub (mpz_clr_inf (rop), op1, op2);

            break;

        case 2 * 0 + 1:     // Op2 only is an infinity
            // The result is theinfinite argument (op2)
            mpz_set_inf (rop, mpz_sgn (op2));

            break;

        case 2 * 1 + 0:     // Op1 only is an infinity
            // The result is theinfinite argument (op1)
            mpz_set_inf (rop, mpz_sgn (op1));

            break;

        case 2 * 1 + 1:     // Op1 and Op2 are infinities
            // If they are the opposite sign, ...
            if (mpz_sgn (op1) NE mpz_sgn (op2))
                mpz_set_inf (rop, mpz_sgn (op1));
            else
                (*gmpz_invalid) (MP_SUB, rop, op1, op2, NULL, 0, 0);
            break;
    } // End SWITCH
} // End mpiz_sub


//***************************************************************************
//  $mpiz_sub_ui
//
//  Subtract an integer and an mpir_ui
//***************************************************************************

void mpiz_sub_ui
    (mpz_t   rop,               // Destination
     mpz_t   op1,               // Left arg
     mpir_ui op2)               // Right arg

{
    if (mpz_inf_p (op1))
        mpz_set_inf (rop, mpz_sgn (op1));
    else
        mpz_sub_ui (mpz_clr_inf (rop), op1, op2);
} // End mpiz_sub_ui


//***************************************************************************
//  $mpiz_ui_sub
//
//  Subtract an mpir_ui and an integer
//***************************************************************************

void mpiz_ui_sub
    (mpz_t   rop,               // Destination
     mpir_ui op1,               // Left arg
     mpz_t   op2)               // Right arg

{
    if (mpz_inf_p (op2))
        mpz_set_inf (rop, -mpz_sgn (op2));
    else
        mpz_ui_sub (mpz_clr_inf (rop), op1, op2);
} // End mpiz_ui_sub


//***************************************************************************
//  $mpiz_mul
//
//  Multiply two integers
//***************************************************************************

void mpiz_mul
    (mpz_t rop,                 // Destination
     mpz_t op1,                 // Left arg
     mpz_t op2)                 // Right arg

{
    // Split cases based upon which (if any) arg is an infinity
    switch (2 * mpz_inf_p (op1) + mpz_inf_p (op2))
    {
        case 2 * 0 + 0:     // Neither arg is an infinity
            // Call the original function
            mpz_mul (mpz_clr_inf (rop), op1, op2);

            break;

        case 2 * 0 + 1:     // Op2 only is an infinity
            // The result is the infinite argument (op2)
            mpz_set_inf (rop, mpz_sgn (op2));

            break;

        case 2 * 1 + 0:     // Op1 only is an infinity
            // The result is the infinite argument (op1)
            mpz_set_inf (rop, mpz_sgn (op1));

            break;

        case 2 * 1 + 1:     // Op1 and Op2 are infinities
            mpz_set_inf (rop, (mpz_sgn (op1) EQ mpz_sgn (op2)) ? 1 : -1);

            break;
    } // End SWITCH
} // End mpiz_mul


//***************************************************************************
//  $mpiz_mul_si
//
//  Multiply an integer and an mpir_si
//***************************************************************************

void mpiz_mul_si
    (mpz_t   rop,               // Destination
     mpz_t   op1,               // Left arg
     mpir_si op2)               // Right arg

{
    if (mpz_inf_p (op1))
        mpz_set_inf (rop, signumint (mpz_sgn (op1)) EQ signumint (op2) ? 1 : -1);
    else
        mpz_mul_si (mpz_clr_inf (rop), op1, op2);
} // End mpiz_mul_si


//***************************************************************************
//  $mpiz_mul_ui
//
//  Multiply an integer and an mpir_ui
//***************************************************************************

void mpiz_mul_ui
    (mpz_t   rop,               // Destination
     mpz_t   op1,               // Left arg
     mpir_ui op2)               // Right arg

{
    if (mpz_inf_p (op1))
        mpz_set_inf (rop, mpz_sgn (op1));
    else
        mpz_mul_ui (mpz_clr_inf (rop), op1, op2);
} // End mpiz_mul_ui


//***************************************************************************
//  $mpiz_addmul
//
//  Multiply two integers and add into a third
//***************************************************************************

void mpiz_addmul
    (mpz_t rop,                 // Destination
     mpz_t op1,                 // Left arg
     mpz_t op2)                 // Right arg

{
    if (mpz_inf_p (rop)
     || mpz_inf_p (op1)
     || mpz_inf_p (op2))
    {
        MP_INT tmp = {0};

        mpz_init (&tmp);
        mpiz_mul (&tmp, op1, op2);
        mpiz_add (rop, rop, &tmp);
        mpz_clear (&tmp);
    } else
        mpz_addmul (mpz_clr_inf (rop), op1, op2);
} // End mpiz_addmul


//***************************************************************************
//  $mpiz_addmul_ui
//
//  Multiply two integers and add into a third
//***************************************************************************

void mpiz_addmul_ui
    (mpz_t   rop,               // Destination
     mpz_t   op1,               // Left arg
     mpir_ui op2)               // Right arg

{
    if (mpz_inf_p (rop)
     || mpz_inf_p (op1))
    {
        MP_INT tmp = {0};

        mpz_init (&tmp);
        mpiz_mul_ui (&tmp, op1, op2);
        mpiz_add (rop, rop, &tmp);
        mpz_clear (&tmp);
    } else
        mpz_addmul_ui (mpz_clr_inf (rop), op1, op2);
} // End mpiz_addmul_ui


//***************************************************************************
//  $mpiz_submul
//
//  Multiply two integers and subtract from a third
//***************************************************************************

void mpiz_submul
    (mpz_t rop,                 // Destination
     mpz_t op1,                 // Left arg
     mpz_t op2)                 // Right arg

{
    if (mpz_inf_p (rop)
     || mpz_inf_p (op1)
     || mpz_inf_p (op2))
    {
        MP_INT tmp = {0};

        mpz_init (&tmp);
        mpiz_mul (&tmp, op1, op2);
        mpiz_sub (rop, rop, &tmp);
        mpz_clear (&tmp);
    } else
        mpz_submul (mpz_clr_inf (rop), op1, op2);
} // End mpiz_submul


//***************************************************************************
//  $mpiz_submul_ui
//
//  Multiply two integers and subtract from a third
//***************************************************************************

void mpiz_submul_ui
    (mpz_t   rop,               // Destination
     mpz_t   op1,               // Left arg
     mpir_ui op2)               // Right arg

{
    if (mpz_inf_p (rop)
     || mpz_inf_p (op1))
    {
        MP_INT tmp = {0};

        mpz_init (&tmp);
        mpiz_mul_ui (&tmp, op1, op2);
        mpiz_sub (rop, rop, &tmp);
        mpz_clear (&tmp);
    } else
        mpz_submul_ui (mpz_clr_inf (rop), op1, op2);
} // End mpiz_submul_ui


//***************************************************************************
//  $mpiz_mul_2exp
//
//  Multiply an integer by 2^ mp_bitcnt_t
//***************************************************************************

void mpiz_mul_2exp
    (mpz_t       rop,           // Destination
     mpz_t       op1,           // Left arg
     mp_bitcnt_t op2)           // Right arg

{
    if (mpz_inf_p (op1))
        mpz_set_inf (rop, mpz_sgn (op1));
    else
        mpz_mul_2exp (mpz_clr_inf (rop), op1, op2);
} // End mpiz_mul_2exp


//***************************************************************************
//  $mpiz_neg
//
//  Set rop to the negative of op
//***************************************************************************

void mpiz_neg
    (mpz_t rop,                 // Destination
     mpz_t op)                  // Source

{
    // If the op is infinite, ...
    if (mpz_inf_p (op))
        // Set the result to the infinity with the opposite sign as op
        mpz_set_inf (rop, -mpz_sgn (op));
    else
        // Otherwise, call the original function
        mpz_neg (mpz_clr_inf (rop), op);
} // End mpiz_neg


//***************************************************************************
//  $mpiz_abs
//
//  Set rop to the absolute value of op
//***************************************************************************

void mpiz_abs
    (mpz_t rop,                 // Destination
     mpz_t op)                  // Source

{
    if (mpz_inf_p (op))
        mpz_set_inf (rop, 1);
    else
        mpz_abs (mpz_clr_inf (rop), op);
} // End mpiz_abs


//***************************************************************************
//  $mpiz_cdiv_q
//
//  Divide N by D and return the quotient
//***************************************************************************

void mpiz_cdiv_q
    (mpz_t q,                   // Destination (quotient)
     mpz_t n,                   // Left arg (numerator)
     mpz_t d)                   // Right arg (denominator)

{
    // Check for special cases
    if (mpz_cmp_ui (d, 0) EQ 0)
        // Divide by zero returns infinity with N's sign
        mpz_set_inf (q, mpz_sgn (n));
    else
    if (mpz_inf_p (n)
     && !mpz_inf_p (d))
        // Divide into infinity returns infinity with NxD's sign
        mpz_set_inf (q, mpz_sgn (n) * mpz_sgn (d));
    else
    if (mpz_inf_p (n)
     && mpz_inf_p (d))
        // Infinity / infinity is undefined
        (*gmpz_invalid) (MP_CDIV_Q, q, n, d, NULL, 0, 0);
    else
        mpz_cdiv_q (mpz_clr_inf (q), n, d);
} // End mpiz_cdiv_q


//***************************************************************************
//  $mpiz_cdiv_r
//
//  Divide N by D and return the remainder
//***************************************************************************

void mpiz_cdiv_r
    (mpz_t r,                   // Destination (remainder)
     mpz_t n,                   // Left arg (numerator)
     mpz_t d)                   // Right arg (denominator)

{
    // Check for special cases
    if (mpz_cmp_ui (d, 0) EQ 0
     || mpz_inf_p (n)
     || mpz_inf_p (d))
        // Remainder is undefined for infinite result
        (*gmpz_invalid) (MP_CDIV_Q, r, n, d, NULL, 0, 0);
    else
        mpz_cdiv_r (r, n, d);
} // End mpiz_cdiv_r


//***************************************************************************
//  $mpiz_cdiv_qr
//
//  Divide N by D and return the quotient and remainder
//***************************************************************************

void mpiz_cdiv_qr
    (mpz_t q,                   // Destination (quotient)
     mpz_t r,                   // Destination (remainder)
     mpz_t n,                   // Left arg (numerator)
     mpz_t d)                   // Right arg (denominator)

{
    // Check for special cases
    if (mpz_cmp_ui (d, 0) EQ 0
     || mpz_inf_p (n)
     || mpz_inf_p (d))
        // Remainder is undefined for infinite result
        (*gmpz_invalid) (MP_CDIV_QR, q, r, n, d, 0, 0);
    else
        mpz_cdiv_qr (mpz_clr_inf (q), r, n, d);
} // End mpiz_cdiv_qr


//***************************************************************************
//  $mpiz_cdiv_q_ui
//
//  Divide N by D and return the quotient
//***************************************************************************

mpir_ui mpiz_cdiv_q_ui
    (mpz_t   q,                 // Destination (quotient)
     mpz_t   n,                 // Left arg (numerator)
     mpir_ui d)                 // Right arg (denominator)

{
    // Check for special cases
    if (d EQ 0)
    {
        // Divide by zero returns infinity with N's sign
        mpz_set_inf (q, mpz_sgn (n));

        return 0;
    } else
    if (mpz_inf_p (n))
    {
        // Divide into infinity returns infinity with NxD's sign
        mpz_set_inf (q, mpz_sgn (n) * signumint (d));

        return 0;
    } else
        return mpz_cdiv_q_ui (mpz_clr_inf (q), n, d);
} // End mpiz_cdiv_q_ui


//***************************************************************************
//  $mpiz_cdiv_r_ui
//
//  Divide N by D and return the remainder
//***************************************************************************

mpir_ui mpiz_cdiv_r_ui
    (mpz_t   r,                 // Destination (remainder)
     mpz_t   n,                 // Left arg (numerator)
     mpir_ui d)                 // Right arg (denominator)

{
    // Check for special cases
    if (d EQ 0
     || mpz_inf_p (n))
        // Remainder is undefined for infinite result
        return (*gmpz_invalid) (MP_CDIV_R_UI, r, n, NULL, NULL, 0, 0);
    else
        return mpz_cdiv_r_ui (mpz_clr_inf (r), n, d);
} // End mpiz_cdiv_r_ui


//***************************************************************************
//  $mpiz_cdiv_qr_ui
//
//  Divide N by D and return the quotient and remainder
//***************************************************************************

mpir_ui mpiz_cdiv_qr_ui
    (mpz_t   q,                 // Destination (quotient)
     mpz_t   r,                 // Destination (remainder)
     mpz_t   n,                 // Left arg (numerator)
     mpir_ui d)                 // Right arg (denominator)

{
    // Check for special cases
    if (d EQ 0
     || mpz_inf_p (n))
        // Remainder is undefined for infinite result
        return (*gmpz_invalid) (MP_CDIV_QR_UI, q, r, n, NULL, d, 0);
    else
        return mpz_cdiv_qr_ui (mpz_clr_inf (q), r, n, d);
} // End mpiz_cdiv_qr_ui


//***************************************************************************
//  $mpiz_cdiv_ui
//
//  Divide N by D and return the remainder
//***************************************************************************

mpir_ui mpiz_cdiv_ui
    (mpz_t   n,                 // Left arg (numerator)
     mpir_ui d)                 // Right arg (denominator)

{
    // Check for special cases
    if (d EQ 0
     || mpz_inf_p (n))
        // mpir_ui is too small for infinite result
        return (*gmpz_invalid) (MP_CDIV_QR_UI, n, NULL, NULL, NULL, d, 0);
    else
        return mpz_cdiv_ui (mpz_clr_inf (n), d);
} // End mpiz_cdiv_ui


//***************************************************************************
//  $mpiz_cdiv_q_2exp
//
//  Divide N by 2^B and return the quotient
//***************************************************************************

void mpiz_cdiv_q_2exp
    (mpz_t   q,                 // Destination (quotient)
     mpz_t   n,                 // Left qrg (numerator)
     mpir_ui b)                 // Right arg (denominator)

{
    // Check for special cases
    if (mpz_inf_p (n))
        // Result is infinity with N's sign
        mpz_set_inf (q, mpz_sgn (n));
    else
        mpz_cdiv_q_2exp (mpz_clr_inf (q), n, b);
} // End mpiz_cdiv_q_2exp


//***************************************************************************
//  $mpiz_cdiv_r_2exp
//
//  Divide N by 2^B and return the remainder
//***************************************************************************

void mpiz_cdiv_r_2exp
    (mpz_t   r,                 // Destination (remainder)
     mpz_t   n,                 // Left qrg (numerator)
     mpir_ui b)                 // Right arg (denominator)

{
    // Check for special cases
    if (mpz_inf_p (n))
        // Remainder is undefined for infinite result
        (*gmpz_invalid) (MP_CDIV_R_2EXP, r, n, NULL, NULL, b, 0);
    else
        mpz_cdiv_r_2exp (mpz_clr_inf (r), n, b);
} // End mpiz_cdiv_r_2exp


//***************************************************************************
//  $mpiz_fdiv_q
//
//  Divide N by D and return the quotient
//***************************************************************************

void mpiz_fdiv_q
    (mpz_t q,                   // Destination (quotient)
     mpz_t n,                   // Left arg (numerator)
     mpz_t d)                   // Right arg (denominator)

{
    // Check for special cases
    if (mpz_cmp_ui (d, 0) EQ 0)
        // Divide by zero returns infinity with N's sign
        mpz_set_inf (q, mpz_sgn (n));
    else
    if (mpz_inf_p (n)
     && !mpz_inf_p (d))
        // Divide into infinity returns infinity with NxD's sign
        mpz_set_inf (q, mpz_sgn (n) * mpz_sgn (d));
    else
    if (mpz_inf_p (n)
     && mpz_inf_p (d))
        // Infinity / infinity is undefined
        (*gmpz_invalid) (MP_FDIV_Q, q, n, d, NULL, 0, 0);
    else
        mpz_fdiv_q (mpz_clr_inf (q), n, d);
} // End mpiz_fdiv_q


//***************************************************************************
//  $mpiz_fdiv_r
//
//  Divide N by D and return the remainder
//***************************************************************************

void mpiz_fdiv_r
    (mpz_t r,                   // Destination (remainder)
     mpz_t n,                   // Left arg (numerator)
     mpz_t d)                   // Right arg (denominator)

{
    // Check for special cases
    if (mpz_cmp_ui (d, 0) EQ 0
     || mpz_inf_p (n)
     || mpz_inf_p (d))
        // Remainder is undefined for infinite result
        (*gmpz_invalid) (MP_FDIV_Q, r, n, d, NULL, 0, 0);
    else
        mpz_fdiv_r (mpz_clr_inf (r), n, d);
} // End mpiz_fdiv_r


//***************************************************************************
//  $mpiz_fdiv_qr
//
//  Divide N by D and return the quotient and remainder
//***************************************************************************

void mpiz_fdiv_qr
    (mpz_t q,                   // Destination (quotient)
     mpz_t r,                   // Destination (remainder)
     mpz_t n,                   // Left arg (numerator)
     mpz_t d)                   // Right arg (denominator)

{
    // Check for special cases
    if (mpz_cmp_ui (d, 0) EQ 0
     || mpz_inf_p (n)
     || mpz_inf_p (d))
        // Remainder is undefined for infinite result
        (*gmpz_invalid) (MP_FDIV_QR, q, r, n, d, 0, 0);
    else
        mpz_fdiv_qr (mpz_clr_inf (q), r, n, d);
} // End mpiz_fdiv_qr


//***************************************************************************
//  $mpiz_fdiv_q_ui
//
//  Divide N by D and return the quotient
//***************************************************************************

mpir_ui mpiz_fdiv_q_ui
    (mpz_t   q,                 // Destination (quotient)
     mpz_t   n,                 // Left arg (numerator)
     mpir_ui d)                 // Right arg (denominator)

{
    // Check for special cases
    if (d EQ 0)
    {
        // Divide by zero returns infinity with N's sign
        mpz_set_inf (q, mpz_sgn (n));

        return 0;
    } else
    if (mpz_inf_p (n))
    {
        // Divide into infinity returns infinity with NxD's sign
        mpz_set_inf (q, mpz_sgn (n) * signumint (d));

        return 0;
    } else
        return mpz_fdiv_q_ui (mpz_clr_inf (q), n, d);
} // End mpiz_fdiv_q_ui


//***************************************************************************
//  $mpiz_fdiv_r_ui
//
//  Divide N by D and return the remainder
//***************************************************************************

mpir_ui mpiz_fdiv_r_ui
    (mpz_t   r,                 // Destination (remainder)
     mpz_t   n,                 // Left arg (numerator)
     mpir_ui d)                 // Right arg (denominator)

{
    // Check for special cases
    if (d EQ 0
     || mpz_inf_p (n))
        // Remainder is undefined for infinite result
        return (*gmpz_invalid) (MP_FDIV_R_UI, r, n, NULL, NULL, 0, 0);
    else
        return mpz_fdiv_r_ui (mpz_clr_inf (r), n, d);
} // End mpiz_fdiv_r_ui


//***************************************************************************
//  $mpiz_fdiv_qr_ui
//
//  Divide N by D and return the quotient and remainder
//***************************************************************************

mpir_ui mpiz_fdiv_qr_ui
    (mpz_t   q,                 // Destination (quotient)
     mpz_t   r,                 // Destination (remainder)
     mpz_t   n,                 // Left arg (numerator)
     mpir_ui d)                 // Right arg (denominator)

{
    // Check for special cases
    if (d EQ 0
     || mpz_inf_p (n))
        // Remainder is undefined for infinite result
        return (*gmpz_invalid) (MP_FDIV_QR_UI, q, r, n, NULL, d, 0);
    else
        return mpz_fdiv_qr_ui (mpz_clr_inf (q), r, n, d);
} // End mpiz_fdiv_qr_ui


//***************************************************************************
//  $mpiz_fdiv_ui
//
//  Divide N by D and return the remainder
//***************************************************************************

mpir_ui mpiz_fdiv_ui
    (mpz_t   n,                 // Left arg (numerator)
     mpir_ui d)                 // Right arg (denominator)

{
    // Check for special cases
    if (d EQ 0
     || mpz_inf_p (n))
        // mpir_ui is too small for infinite result
        return (*gmpz_invalid) (MP_FDIV_QR_UI, n, NULL, NULL, NULL, d, 0);
    else
        return mpz_fdiv_ui (n, d);
} // End mpiz_fdiv_ui


//***************************************************************************
//  $mpiz_fdiv_q_2exp
//
//  Divide N by 2^B and return the quotient
//***************************************************************************

void mpiz_fdiv_q_2exp
    (mpz_t   q,                 // Destination (quotient)
     mpz_t   n,                 // Left qrg (numerator)
     mpir_ui b)                 // Right arg (denominator)

{
    // Check for special cases
    if (mpz_inf_p (n))
        // Result is infinity with N's sign
        mpz_set_inf (q, mpz_sgn (n));
    else
        mpz_fdiv_q_2exp (mpz_clr_inf (q), n, b);
} // End mpiz_fdiv_q_2exp


//***************************************************************************
//  $mpiz_fdiv_r_2exp
//
//  Divide N by 2^B and return the remainder
//***************************************************************************

void mpiz_fdiv_r_2exp
    (mpz_t   r,                 // Destination (remainder)
     mpz_t   n,                 // Left qrg (numerator)
     mpir_ui b)                 // Right arg (denominator)

{
    // Check for special cases
    if (mpz_inf_p (n))
        // Remainder is undefined for infinite result
        (*gmpz_invalid) (MP_FDIV_R_2EXP, r, n, NULL, NULL, b, 0);
    else
        mpz_fdiv_r_2exp (mpz_clr_inf (r), n, b);
} // End mpiz_fdiv_r_2exp


//***************************************************************************
//  $mpiz_tdiv_q
//
//  Divide N by D and return the quotient
//***************************************************************************

void mpiz_tdiv_q
    (mpz_t q,                   // Destination (quotient)
     mpz_t n,                   // Left arg (numerator)
     mpz_t d)                   // Right arg (denominator)

{
    // Check for special cases
    if (mpz_cmp_ui (d, 0) EQ 0)
        // Divide by zero returns infinity with N's sign
        mpz_set_inf (q, mpz_sgn (n));
    else
    if (mpz_inf_p (n)
     && !mpz_inf_p (d))
        // Divide into infinity returns infinity with NxD's sign
        mpz_set_inf (q, mpz_sgn (n) * mpz_sgn (d));
    else
    if (mpz_inf_p (n)
     && mpz_inf_p (d))
        // Infinity / infinity is undefined
        (*gmpz_invalid) (MP_TDIV_Q, q, n, d, NULL, 0, 0);
    else
        mpz_tdiv_q (mpz_clr_inf (q), n, d);
} // End mpiz_tdiv_q


//***************************************************************************
//  $mpiz_tdiv_r
//
//  Divide N by D and return the remainder
//***************************************************************************

void mpiz_tdiv_r
    (mpz_t r,                   // Destination (remainder)
     mpz_t n,                   // Left arg (numerator)
     mpz_t d)                   // Right arg (denominator)

{
    // Check for special cases
    if (mpz_cmp_ui (d, 0) EQ 0
     || mpz_inf_p (n)
     || mpz_inf_p (d))
        // Remainder is undefined for infinite result
        (*gmpz_invalid) (MP_TDIV_Q, r, n, d, NULL, 0, 0);
    else
        mpz_tdiv_r (mpz_clr_inf (r), n, d);
} // End mpiz_tdiv_r


//***************************************************************************
//  $mpiz_tdiv_qr
//
//  Divide N by D and return the quotient and remainder
//***************************************************************************

void mpiz_tdiv_qr
    (mpz_t q,                   // Destination (quotient)
     mpz_t r,                   // Destination (remainder)
     mpz_t n,                   // Left arg (numerator)
     mpz_t d)                   // Right arg (denominator)

{
    // Check for special cases
    if (mpz_cmp_ui (d, 0) EQ 0
     || mpz_inf_p (n)
     || mpz_inf_p (d))
        // Remainder is undefined for infinite result
        (*gmpz_invalid) (MP_TDIV_QR, q, r, n, d, 0, 0);
    else
        mpz_tdiv_qr (mpz_clr_inf (q), r, n, d);
} // End mpiz_tdiv_qr


//***************************************************************************
//  $mpiz_tdiv_q_ui
//
//  Divide N by D and return the quotient
//***************************************************************************

mpir_ui mpiz_tdiv_q_ui
    (mpz_t   q,                 // Destination (quotient)
     mpz_t   n,                 // Left arg (numerator)
     mpir_ui d)                 // Right arg (denominator)

{
    // Check for special cases
    if (d EQ 0)
    {
        // Divide by zero returns infinity with N's sign
        mpz_set_inf (q, mpz_sgn (n));

        return 0;
    } else
    if (mpz_inf_p (n))
    {
        // Divide into infinity returns infinity with NxD's sign
        mpz_set_inf (q, mpz_sgn (n) * signumint (d));

        return 0;
    } else
        return mpz_tdiv_q_ui (mpz_clr_inf (q), n, d);
} // End mpiz_tdiv_q_ui


//***************************************************************************
//  $mpiz_tdiv_r_ui
//
//  Divide N by D and return the remainder
//***************************************************************************

mpir_ui mpiz_tdiv_r_ui
    (mpz_t   r,                 // Destination (remainder)
     mpz_t   n,                 // Left arg (numerator)
     mpir_ui d)                 // Right arg (denominator)

{
    // Check for special cases
    if (d EQ 0
     || mpz_inf_p (n))
        // Remainder is undefined for infinite result
        return (*gmpz_invalid) (MP_TDIV_R_UI, r, n, NULL, NULL, 0, 0);
    else
        return mpz_tdiv_r_ui (mpz_clr_inf (r), n, d);
} // End mpiz_tdiv_r_ui


//***************************************************************************
//  $mpiz_tdiv_qr_ui
//
//  Divide N by D and return the quotient and remainder
//***************************************************************************

mpir_ui mpiz_tdiv_qr_ui
    (mpz_t   q,                 // Destination (quotient)
     mpz_t   r,                 // Destination (remainder)
     mpz_t   n,                 // Left arg (numerator)
     mpir_ui d)                 // Right arg (denominator)

{
    // Check for special cases
    if (d EQ 0
     || mpz_inf_p (n))
        // Remainder is undefined for infinite result
        return (*gmpz_invalid) (MP_TDIV_QR_UI, q, r, n, NULL, d, 0);
    else
        return mpz_tdiv_qr_ui (mpz_clr_inf (q), r, n, d);
} // End mpiz_tdiv_qr_ui


//***************************************************************************
//  $mpiz_tdiv_ui
//
//  Divide N by D and return the remainder
//***************************************************************************

mpir_ui mpiz_tdiv_ui
    (mpz_t   n,                 // Left arg (numerator)
     mpir_ui d)                 // Right arg (denominator)

{
    // Check for special cases
    if (d EQ 0
     || mpz_inf_p (n))
        // mpir_ui is too small for infinite result
        return (*gmpz_invalid) (MP_TDIV_QR_UI, n, NULL, NULL, NULL, d, 0);
    else
        return mpz_tdiv_ui (n, d);
} // End mpiz_tdiv_ui


//***************************************************************************
//  $mpiz_tdiv_q_2exp
//
//  Divide N by 2^B and return the quotient
//***************************************************************************

void mpiz_tdiv_q_2exp
    (mpz_t   q,                 // Destination (quotient)
     mpz_t   n,                 // Left qrg (numerator)
     mpir_ui b)                 // Right arg (denominator)

{
    // Check for special cases
    if (mpz_inf_p (n))
        // Result is infinity with N's sign
        mpz_set_inf (q, mpz_sgn (n));
    else
        mpz_tdiv_q_2exp (mpz_clr_inf (q), n, b);
} // End mpiz_tdiv_q_2exp


//***************************************************************************
//  $mpiz_tdiv_r_2exp
//
//  Divide N by 2^B and return the remainder
//***************************************************************************

void mpiz_tdiv_r_2exp
    (mpz_t   r,                 // Destination (remainder)
     mpz_t   n,                 // Left qrg (numerator)
     mpir_ui b)                 // Right arg (denominator)

{
    // Check for special cases
    if (mpz_inf_p (n))
        // Remainder is undefined for infinite result
        (*gmpz_invalid) (MP_TDIV_R_2EXP, r, n, NULL, NULL, b, 0);
    else
        mpz_tdiv_r_2exp (mpz_clr_inf (r), n, b);
} // End mpiz_tdiv_r_2exp


//***************************************************************************
//  $mpiz_mod
//
//  Set the result to N mod D
//***************************************************************************

void mpiz_mod
    (mpz_t r,                   // The result
     mpz_t n,                   // The argument
     mpz_t d)                   // The modulus

{
    // Handle zero modulus or argument
    if (mpz_sgn (n) EQ 0
     || mpz_sgn (d) EQ 0)
        // Initialize the result to zero
        mpz_init (r);
    else
    // Check for indeterminate cases
    // If the argument is negative infinity,...
    if (mpz_inf_p (n) && mpz_sgn (n) < 0)
    {
        // If the modulus is positive, ...
        if (mpz_sgn (d) > 0)
            (*gmpz_invalid) (MP_PosMODNi, r, n, d, NULL, 0, 0);
        else
        // If the modulus is negative, ...
        if (mpz_sgn (d) < 0)
            (*gmpz_invalid) (MP_NegMODNi, r, n, d, NULL, 0, 0);
    } else
    // If the argument is positive infinity,...
    if (mpz_inf_p (n) && mpz_sgn (n) > 0)
    {
        // If the modulus is positive, ...
        if (mpz_sgn (d) > 0)
            (*gmpz_invalid) (MP_PosMODPi, r, n, d, NULL, 0, 0);
        else
        // If the modulus is negative, ...
        if (mpz_sgn (d) < 0)
            (*gmpz_invalid) (MP_NegMODPi, r, n, d, NULL, 0, 0);
    } else
    // If the modulus is negative infinity,...
    if (mpz_inf_p (d) && mpz_sgn (d) < 0)
    {
        // If the argument is positive, ...
        if (mpz_sgn (n) > 0)
            (*gmpz_invalid) (MP_NiMODPos, r, n, d, NULL, 0, 0);
        else
        // If the argument is negative, ...
        if (mpz_sgn (n) < 0)
            (*gmpz_invalid) (MP_NiMODNeg, r, n, d, NULL, 0, 0);
    } else
    // If the modulus is positive infinity,...
    if (mpz_inf_p (d) && mpz_sgn (d) > 0)
    {
        // If the argument is positive, ...
        if (mpz_sgn (n) > 0)
            (*gmpz_invalid) (MP_PiMODPos, r, n, d, NULL, 0, 0);
        else
        // If the argument is negative, ...
        if (mpz_sgn (n) < 0)
            (*gmpz_invalid) (MP_PiMODNeg, r, n, d, NULL, 0, 0);
    } else
        mpz_mod (r, n, d);
} // End mpiz_mod


//***************************************************************************
//  $mpiz_mod_ui
//
//  Set the result to N mod D
//***************************************************************************

mpir_ui mpiz_mod_ui
    (mpz_t   r,                 // The result
     mpz_t   n,                 // The argument
     mpir_ui d)                 // The modulus

{
    // Handle zero modulus or argument
    if (mpz_sgn (n) EQ 0
     ||          d  EQ 0)
    {
        // Initialize the result to zero
        mpz_init (r);

        return 0;       // ***FIXME***
    } else
    // Check for indeterminate cases
    // If the argument is negative infinity,...
    if (mpz_inf_p (n) && mpz_sgn (n) < 0)
        return (*gmpz_invalid) (MP_MODUINi, r, n, NULL, NULL, d, 0);
    else
    // If the argument is positive infinity,...
    if (mpz_inf_p (n) && mpz_sgn (n) > 0)
        return (*gmpz_invalid) (MP_MODUIPi, r, n, NULL, NULL, d, 0);
    else
        return mpz_mod_ui (r, n, d);
} // End mpiz_mod


//***************************************************************************
//  $mpiz_divexact
//
//  Set the result to n/d
//***************************************************************************

void mpiz_divexact
    (mpz_t q,                   // Quotient
     mpz_t n,                   // Numerator
     mpz_t d)                   // Divisor

{
    mpz_divexact (q, n, d);     // ***FIXME***
} // End mpiz_divexact


//***************************************************************************
//  $mpiz_divexact_ui
//
//  Set the result to n/d
//***************************************************************************

void mpiz_divexact_ui
    (mpz_t   q,                 // Quotient
     mpz_t   n,                 // Numerator
     mpir_ui d)                 // Divisor

{
    mpz_divexact_ui (q, n, d);  // ***FIXME***
} // End mpiz_divexact_ui


//***************************************************************************
//  $mpiz_divisible_p
//
//  Return non-zero if N is evenly divisible by D
//***************************************************************************

int mpiz_divisible_p
    (mpz_t n,                   // Numerator
     mpz_t d)                   // Denominator

{
    if (mpz_inf_p (n)
     || mpz_inf_p (d))
        return (int) (*gmpz_invalid) (MP_DIVISIBLE_P, n, d, NULL, NULL, 0, 0);
    else
        return mpz_divisible_p (n, d);
} // End mpiz_divisible_p


//***************************************************************************
//  $mpiz_divisible_ui_p
//
//  Return non-zero if N is evenly divisible by D
//***************************************************************************

int mpiz_divisible_ui_p
    (mpz_t   n,                 // Numerator
     mpir_ui d)                 // Denominator

{
    if (mpz_inf_p (n))
        return (int) (*gmpz_invalid) (MP_DIVISIBLE_UI_P, n, NULL, NULL, NULL, 0, 0);
    else
        return mpz_divisible_ui_p (n, d);
} // End mpiz_divisible_ui_p


//***************************************************************************
//  $mpiz_divisible_2exp_p
//
//  Return non-zero if N is evenly divisible by 2^D
//***************************************************************************

int mpiz_divisible_2exp_p
    (mpz_t       n,             // Numerator
     mp_bitcnt_t d)             // Denominator

{
    if (mpz_inf_p (n))
        return (int) (*gmpz_invalid) (MP_DIVISIBLE_2EXP_P, n, NULL, NULL, NULL, 0, 0);
    else
        return mpz_divisible_2exp_p (n, d);
} // End mpiz_divisible_2exp_p


//***************************************************************************
//  $mpiz_pow_ui
//
//  Set rop to base^exp
//***************************************************************************

void mpiz_pow_ui
    (mpz_t   rop,               // Destination
     mpz_t   base,              // Left arg
     mpir_ui exp)               // Right arg

{
    MP_INT mpzRes = {0};

    // Check for special cases
    if (IsMpz0 (base) && exp EQ 0           //  0  * 0
     || mpz_inf_p (base) && exp EQ 0)   // Inf * 0
        (*gmpz_invalid) (MP_POW_UI, rop, base, NULL, NULL, exp, 0);
    else
    if (mpz_inf_p (base))
        mpz_set_inf (rop, ((exp & 1) && mpz_sgn (base) < 0) ? -1 : 1);
    else
        mpz_pow_ui (mpz_clr_inf (rop), base, exp);
} // End mpiz_pow_ui


//***************************************************************************
//  $mpiz_ui_pow_ui
//
//  Set rop to base^exp
//***************************************************************************

void mpiz_ui_pow_ui
    (mpz_t   rop,               // Destination
     mpir_ui base,              // Left arg
     mpir_ui exp)               // Right arg

{
    // Check for special cases
    if (base EQ 0 && exp EQ 0)          //  0  * 0
        (*gmpz_invalid) (MP_UI_POW_UI, rop, NULL, NULL, NULL, base, exp);
    else
        mpz_ui_pow_ui (mpz_clr_inf (rop), base, exp);
} // End mpiz_ui_pow_ui


//***************************************************************************
//  $mpiz_sqrt
//
//  Set result to floor (sqrt (op))
//***************************************************************************

void mpiz_sqrt
    (mpz_t rop,                 // Destination
     mpz_t op)                  // Source

{
    if (mpz_inf_p (op))
    {
        if (mpz_sgn (op) < 0)
            (*gmpz_invalid) (MP_SQRT, rop, op, NULL, NULL, 0, 0);
        else
            mpz_set_inf (rop, 1);
    } else
        mpz_sqrt (rop, op);
} // End mpiz_sqrt


//***************************************************************************
//  $mpiz_likely_prime_p
//
//  Determine if the arg is likely a prime
//***************************************************************************

int mpiz_likely_prime_p
    (mpz_t           rop,       // Argument
     gmp_randstate_t state,     // Random state
     mpir_ui         div)       // Largest excluded prime divisor (0 = none)

{
    return mpz_likely_prime_p (rop, state, div);    // ***FIXME***
} // End mpiz_likely_prime_p


//***************************************************************************
//  $mpiz_probable_prime_p
//
//  Determine if the arg is probably a prime
//***************************************************************************

int mpiz_probable_prime_p
    (mpz_t           rop,       // Argument
     gmp_randstate_t state,     // Random state
     int             prob,      // Error chance is 1 in 2^prob
     mpir_ui         div)       // Largest excluded prime divisor (0 = none)

{
    return mpz_probable_prime_p (rop, state, prob, div);    // ***FIXME***
} // End mpiz_probable_prime_p


//***************************************************************************
//  $mpiz_nextprime
//
//  Set result to next prime greater than op
//***************************************************************************

void mpiz_nextprime
    (mpz_t rop,                 // Destination
     mpz_t op)                  // Argument

{
    mpz_nextprime (rop, op);    // ***FIXME***
} // End mpiz_nextprime


//***************************************************************************
//  $mpiz_gcd
//
//  Set rop to the absolute value of the gcd of op1 and op2
//***************************************************************************

void mpiz_gcd
    (mpz_t rop,                 // Destination
     mpz_t op1,                 // Left arg
     mpz_t op2)                 // Right arg

{
    // Split cases based upon which (if any) arg is an infinity
    switch (2 * mpz_inf_p (op1) + mpz_inf_p (op2))
    {
        case 2 * 0 + 0:     // Neither arg is an infinity
            // Call the original function
            mpz_gcd (mpz_clr_inf (rop), op1, op2);

            break;

        case 2 * 0 + 1:     // Op2 only is an infinity
        case 2 * 1 + 0:     // Op1 only is an infinity
        case 2 * 1 + 1:     // Op1 and Op2 are infinities
            (*gmpz_invalid) (MP_GCD, rop, op1, op2, NULL, 0, 0);

            break;

        default:
            break;
    } // End SWITCH
} // End mpiz_gcd


//***************************************************************************
//  $mpiz_lcm
//
//  Set rop to the absolute value of the lcm of op1 and op2
//***************************************************************************

void mpiz_lcm
    (mpz_t rop,                 // Destination
     mpz_t op1,                 // Left arg
     mpz_t op2)                 // Right arg

{
    // Split cases based upon which (if any) arg is an infinity
    switch (2 * mpz_inf_p (op1) + mpz_inf_p (op2))
    {
        case 2 * 0 + 0:     // Neither arg is an infinity
            // Call the original function
            mpz_lcm (mpz_clr_inf (rop), op1, op2);

            break;

        case 2 * 0 + 1:     // Op2 only is an infinity
        case 2 * 1 + 0:     // Op1 only is an infinity
        case 2 * 1 + 1:     // Op1 and Op2 are infinities
            (*gmpz_invalid) (MP_LCM, rop, op1, op2, NULL, 0, 0);

            break;

        default:
            break;
    } // End SWITCH
} // End mpiz_lcm


//***************************************************************************
//  $mpiz_remove
//
//  Remove all occurrences of the factor f from op
//    and store the result in rop, returning the #
//    occurrecnes in the result.
//***************************************************************************

mp_bitcnt_t mpiz_remove
    (mpz_t rop,                 // Destination
     mpz_t op,                  // Argument
     mpz_t f)                   // Factor to remove

{
    return mpz_remove (rop, op, f);     // ***FINISHME***
} // End mpiz_remove


//***************************************************************************
//  $mpiz_cmp
//
//  Compare op1 and op2, returning
//    positive if op1 >  op2,
//    zero     if op1 == op2
//    negative if op1 <  op2
//***************************************************************************

int mpiz_cmp
    (mpz_t op1,                 // Left arg
     mpz_t op2)                 // Right arg

{
    // Split cases based upon which (if any) arg is an infinity
    switch (2 * mpz_inf_p (op1) + mpz_inf_p (op2))
    {
        case 2 * 0 + 0:     // Neither arg is an infinity
            // Call the original function
            return mpz_cmp (op1, op2);

        case 2 * 0 + 1:     // Op2 only is an infinity
            // The result is opposite sign of the infinite argument (op2)
            return -mpz_sgn (op2);

        case 2 * 1 + 0:     // Op1 only is an infinity
            // The result is same sign as the infinite argument (op1)
            return  mpz_sgn (op1);

        case 2 * 1 + 1:     // Op1 and Op2 are infinities
            // The result is 0 if op1 and op2 are the same sign,
            //   +1 if op2 is negative and -1 if op1 is negative
            return signumint (mpz_sgn (op1) - mpz_sgn (op2));

        default:
            return 0;       // To keep the compiler happy
    } // End SWITCH
} // End mpiz_cmp


//***************************************************************************
//  $mpiz_cmp_d
//
//  Compare op1 and op2, returning
//    positive if op1 >  op2,
//    zero     if op1 == op2
//    negative if op1 <  op2
//***************************************************************************

int mpiz_cmp_d
    (mpz_t  op1,                // Left arg
     double op2)                // Right arg

{
    // Split cases based upon which (if any) arg is an infinity
    switch (2 * mpz_inf_p (op1) + IsInfinity (op2))
    {
        case 2 * 0 + 0:     // Neither arg is an infinity
            // Call the original function
            return mpz_cmp_d (op1, op2);

        case 2 * 0 + 1:     // Op2 only is an infinity
            // The result is the opposite sign of the infinite argument (op2)
            return -signumflt (op2);

        case 2 * 1 + 0:     // Op1 only is an infinity
            // The result is the same sign as the infinite argument (op1)
            return  mpz_sgn (op1);

        case 2 * 1 + 1:     // Op1 and Op2 are infinities
            // The result is 0 if op1 and op2 are the same sign,
            //   +1 if op2 is negative and -1 if op1 is negative
            return signumint ((mpz_sgn (op1) - signumflt (op2)));

        default:
            return 0;       // To keep the compiler happy
    } // End SWITCH
} // End mpiz_cmp_d


//***************************************************************************
//  $mpiz_cmp_ui
//
//  Compare op1 and op2, returning
//    positive if op1 >  op2,
//    zero     if op1 == op2
//    negative if op1 <  op2
//***************************************************************************

int mpiz_cmp_ui
    (mpz_t   op1,               // Left arg
     mpir_ui op2)               // Right arg

{
    if (mpz_inf_p (op1))
        return mpz_sgn (op1);
    else
        return mpz_cmp_ui (op1, op2);
} // End mpiz_cmp_ui


//***************************************************************************
//  $mpiz_cmp_si
//
//  Compare op1 and op1, returning
//    positive if op1 >  op2,
//    zero     if op1 == op2
//    negative if op1 <  op2
//***************************************************************************

int mpiz_cmp_si
    (mpz_t   op1,               // Left arg
     mpir_si op2)               // Right arg

{
    if (mpz_inf_p (op1))
        return mpz_sgn (op1);
    else
        return mpz_cmp_si (op1, op2);
} // End mpiz_cmp_si


//***************************************************************************
//  $mpiz_cmpabs
//
//  Compare the absolute values of op1 and op2, returning
//    positive if abs (op1) >  abs (op2)
//    zero     if abs (op1) == abs (op2)
//    negative if abs (op1) <  abs (op2)
//***************************************************************************

int mpiz_cmpabs
    (mpz_t op1,                 // Left arg
     mpz_t op2)                 // Right arg

{
    // Split cases based upon which (if any) arg is an infinity
    switch (2 * mpz_inf_p (op1) + mpz_inf_p (op2))
    {
        case 2 * 0 + 0:     // Neither arg is an infinity
            // Call the original function
            return mpz_cmpabs (op1, op2);

        case 2 * 0 + 1:     // Op2 only is an infinity
            return -1;

        case 2 * 1 + 0:     // Op1 only is an infinity
            return  1;

        case 2 * 1 + 1:     // Op1 and Op2 are infinities
            return  0;

        default:
            return 0;       // To keep the compiler happy
    } // End SWITCH
} // End mpiz_cmpabs


//***************************************************************************
//  $mpiz_cmpabs_d
//
//  Compare absolute values of op1 and op2, returning
//    positive if abs (op1) >  abs (op2)
//    zero     if abs (op1) == abs (op2)
//    negative if abs (op1) <  abs (op2)
//***************************************************************************

int mpiz_cmpabs_d
    (mpz_t  op1,                // Left arg
     double op2)                // Right arg

{
    // Split cases based upon which (if any) arg is an infinity
    switch (2 * mpz_inf_p (op1) + IsInfinity (op2))
    {
        case 2 * 0 + 0:     // Neither arg is an infinity
            // Call the original function
            return mpz_cmpabs_d (op1, op2);

        case 2 * 0 + 1:     // Op2 only is an infinity
            return -1;

        case 2 * 1 + 0:     // Op1 only is an infinity
            return  1;

        case 2 * 1 + 1:     // Op1 and Op2 are infinities
            return 0;

        default:
            return 0;       // To keep the compiler happy
    } // End SWITCH
} // End mpiz_cmpabs_d


//***************************************************************************
//  $mpiz_cmpabs_ui
//
//  Compare absolute values of op1 and op2, returning
//    positive if abs (op1) >  abs (op2)
//    zero     if abs (op1) == abs (op2)
//    negative if abs (op1) <  abs (op2)
//***************************************************************************

int mpiz_cmpabs_ui
    (mpz_t   op1,               // Left arg
     mpir_ui op2)               // Right arg

{
    if (mpz_inf_p (op1))
        return 1;
    else
        return mpz_cmpabs_ui (op1, op2);
} // End mpiz_cmpabs_ui


//***************************************************************************
//  $mpiz_and
//
//  Set rop to op1 bitwise-and op2
//***************************************************************************

void mpiz_and
    (mpz_t rop,                 // Destination
     mpz_t op1,                 // Left arg
     mpz_t op2)                 // Right arg

{
    if (mpz_inf_p (op1)
     || mpz_inf_p (op2))
        (*gmpz_invalid) (MP_AND, rop, op1, op2, NULL, 0, 0);
    else
        mpz_and (mpz_clr_inf (rop), op1, op2);
} // End mpiz_and


//***************************************************************************
//  $mpiz_ior
//
//  Set rop to op1 inclusive-or op2
//***************************************************************************

void mpiz_ior
    (mpz_t rop,                 // Destination
     mpz_t op1,                 // Left arg
     mpz_t op2)                 // Right arg

{
    if (mpz_inf_p (op1)
     || mpz_inf_p (op2))
        (*gmpz_invalid) (MP_IOR, rop, op1, op2, NULL, 0, 0);
    else
        mpz_ior (mpz_clr_inf (rop), op1, op2);
} // End mpiz_ior


//***************************************************************************
//  $mpiz_xor
//
//  Set rop to op1 exclusive-or op2
//***************************************************************************

void mpiz_xor
    (mpz_t rop,                 // Destination
     mpz_t op1,                 // Left arg
     mpz_t op2)                 // Right arg

{
    if (mpz_inf_p (op1)
     || mpz_inf_p (op2))
        (*gmpz_invalid) (MP_XOR, rop, op1, op2, NULL, 0, 0);
    else
        mpz_xor (mpz_clr_inf (rop), op1, op2);
} // End mpiz_xor


//***************************************************************************
//  $mpiz_scan0
//
//  Scan op starting from starting_bit for the first 0
//***************************************************************************

mp_bitcnt_t mpiz_scan0
    (mpz_t op,
     mp_bitcnt_t starting_bit)

{
    return mpz_scan0 (op, starting_bit);
} // End mpiz_scan0


//***************************************************************************
//  $mpiz_scan1
//
//  Scan op starting from starting_bit for the first 1
//***************************************************************************

mp_bitcnt_t mpiz_scan1
    (mpz_t op,
     mp_bitcnt_t starting_bit)

{
    return mpz_scan1 (op, starting_bit);
} // End mpiz_scan1


//***************************************************************************
//  $mpiz_com
//
//  Set rop to one's complement of op1
//***************************************************************************

void mpiz_com
    (mpz_t rop,                 // Destination
     mpz_t op1)                 // Left arg

{
    if (mpz_inf_p (op1))
        (*gmpz_invalid) (MP_COM, rop, op1, NULL, NULL, 0, 0);
    else
        mpz_com (mpz_clr_inf (rop), op1);
} // End mpiz_com


//***************************************************************************
//  $mpiz_fits_ulong_p
//
//  Return non-zero if op fits in an unsigned long int
//***************************************************************************

int mpiz_fits_ulong_p
    (mpz_t op)                  // Source

{
    if (mpz_inf_p (op))
        return 0;
    else
        return mpz_fits_ulong_p (op);
} // End mpiz_fits_ulong_p


//***************************************************************************
//  $mpiz_fits_slong_p
//
//  Return non-zero if op fits in a signed long int
//***************************************************************************

int mpiz_fits_slong_p
    (mpz_t op)                  // Source

{
    if (mpz_inf_p (op))
        return 0;
    else
        return mpz_fits_slong_p (op);
} // End mpiz_fits_slong_p


//***************************************************************************
//  $mpiz_fits_uint_p
//
//  Return non-zero if op fits in an unsigned int
//***************************************************************************

int mpiz_fits_uint_p
    (mpz_t op)                  // Source

{
    if (mpz_inf_p (op))
        return 0;
    else
        return mpz_fits_uint_p (op);
} // End mpiz_fits_uint_p


//***************************************************************************
//  $mpiz_fits_sint_p
//
//  Return non-zero if op fits in a signed int
//***************************************************************************

int mpiz_fits_sint_p
    (mpz_t op)                  // Source

{
    if (mpz_inf_p (op))
        return 0;
    else
        return mpz_fits_sint_p (op);
} // End mpiz_fits_sint_p


//***************************************************************************
//  $mpiz_fits_ushort_p
//
//  Return non-zero if op fits in an unsigned short
//***************************************************************************

int mpiz_fits_ushort_p
    (mpz_t op)                  // Source

{
    if (mpz_inf_p (op))
        return 0;
    else
        return mpz_fits_ushort_p (op);
} // End mpiz_fits_ushort_p


//***************************************************************************
//  $mpiz_fits_sshort_p
//
//  Return non-zero if op fits in a signed short
//***************************************************************************

int mpiz_fits_sshort_p
    (mpz_t op)                  // Source

{
    if (mpz_inf_p (op))
        return 0;
    else
        return mpz_fits_sshort_p (op);
} // End mpiz_fits_sshort_p


//***************************************************************************
//  $mpiz_odd_p
//
//  Return non-zero if op is odd
//***************************************************************************

int mpiz_odd_p
    (mpz_t op)                  // Source

{
    if (mpz_inf_p (op))
        return 0;
    else
        return mpz_odd_p (op);
} // End mpiz_odd_p


//***************************************************************************
//  $mpiz_odd_p
//
//  Return non-zero if op is even
//***************************************************************************

int mpiz_even_p
    (mpz_t op)                  // Source

{
    if (mpz_inf_p (op))
        return 0;
    else
        return mpz_odd_p (op);
} // End mpiz_odd_p


//***************************************************************************
//  $mpiz_sizeinbase
//
//  Return the size of op measured in the number of digits in the given base
//***************************************************************************

size_t mpiz_sizeinbase
    (mpz_t op,                  // Source
     int   base)                // Base

{
    return mpz_sizeinbase (mpz_clr_inf (op), base);
} // End mpiz_sizeinbase


//***************************************************************************
//  $_mpiz_realloc
//
//  Change the space for op
//***************************************************************************

void *_mpiz_realloc
    (mpz_t     op,              // Source
     mp_size_t new_alloc)       // New allocation

{
    if (mpz_inf_p (op))
        return NULL;
    else
        return _mpz_realloc (op, new_alloc);
} // End _mpiz_realloc


//***************************************************************************
//  $mpiz_getlimbn
//
//  Return limb n from op
//***************************************************************************

mp_limb_t mpiz_getlimbn
    (mpz_t     op,              // Source
     mp_size_t n)               // Limb #

{
    if (mpz_inf_p (op))
        return 0;
    else
        return mpz_getlimbn (op, n);
} // End mpiz_getlimbn


//***************************************************************************
//  $mpiz_size
//
//  Return size of op in limbs
//***************************************************************************

size_t mpiz_size
    (mpz_t op)                  // Source

{
    if (mpz_inf_p (op))
        return 0;
    else
        return mpz_size (op);
} // End mpiz_size


//***************************************************************************
//  MPQ_ Functions
//***************************************************************************

//***************************************************************************
//  &mpq_exit
//***************************************************************************

mpir_ui mpq_exit
    (enum MP_ENUM mp_enum,
     mpq_t   rop,
     mpq_t   op1,
     mpq_t   op2,
     mpq_t   op3,
     mpir_ui d)

{
    exit (0);

    return 0;               // To keep the compiler happy
} // End mpq_exit


//***************************************************************************
//  $mpq_inf_p
//
//  Is the given value an infinity
//***************************************************************************

int mpq_inf_p
    (mpq_t op)                  // Source

{
    return (mpz_inf_p (mpq_numref (op))
         && IsMpzNULL (mpq_denref (op)));
} // End mpq_inf_p


//***************************************************************************
//  $IsMpqPosInfinity
//
//  Is the given value +infinity
//***************************************************************************

int IsMpqPosInfinity
    (mpq_t op)                  // Source

{
    return (mpq_inf_p (op)
         && mpq_sgn (op) > 0);
} // End IsMpqPosInfinity


//***************************************************************************
//  $IsMpqNegInfinity
//
//  Is the given value -infinity
//***************************************************************************

int IsMpqNegInfinity
    (mpq_t op)                  // Source

{
    return (mpq_inf_p (op)
         && mpq_sgn (op) < 0);
} // End IsMpqNegInfinity


//***************************************************************************
//  $IsMpqNULL
//
//  Is the given value all zero?
//***************************************************************************

int IsMpqNULL
    (mpq_t op)                  // Source

{
    return (IsMpzNULL (mpq_numref (op))
         && IsMpzNULL (mpq_denref (op)));
} // End IsMpqNULL


//***************************************************************************
//  $mpq_set_infsub
//
//  Set the argument to +/-infinity
//***************************************************************************

void mpq_set_infsub
    (mpq_t rop,                 // Destination
     int   sgn)                 // Sign:  >= 0 for +Infinity, < 0 for -Infinity

{
    // Set the numerator to a special format, properly signed
    rop->_mp_num._mp_size  = (sgn >= 0) ? (mp_size_t) INT_MAX
                                        : (mp_size_t) INT_MIN;
    rop->_mp_num._mp_alloc = 0;
    rop->_mp_num._mp_d     = NULL;

    // Set the denominator to all 0s
    rop->_mp_den._mp_size  = 0;
    rop->_mp_den._mp_alloc = 0;
    rop->_mp_den._mp_d     = NULL;
} // mpq_set_infsub


//***************************************************************************
//  $mpq_set_inf
//
//  Set the argument to +/-infinity
//***************************************************************************

void mpq_set_inf
    (mpq_t rop,                 // Destination
     int   sgn)                 // Sign:  >= 0 for +Infinity, < 0 for -Infinity

{
    // If it's not infinite, ...
    if (!mpq_inf_p (rop))
        // Free the arg as we'll use a special format
        Myq_clear (rop);

    mpq_set_infsub (rop, sgn);
} // mpq_set_inf


//***************************************************************************
//  $mpq_clr_inf
//
//  Initialize op if it's an infinity
//***************************************************************************

mpq_ptr mpq_clr_inf
    (mpq_t op)                  // Source

{
    if (mpq_inf_p (op))
        mpq_init (op);

    return (op);
} // End mpq_clr_inf


//***************************************************************************
//  $mpiq_canonicalize
//
//  Remove any common factors
//***************************************************************************

void mpiq_canonicalize
    (mpq_t op)

{
    if (!mpq_inf_p (op))
        mpq_canonicalize (op);
} // End mpiq_canonicalize


//***************************************************************************
//  $mpiq_set
//
//  Save a rational in another rational
//***************************************************************************

void mpiq_set
    (mpq_t rop,                 // Destination
     mpq_t op)                  // Source

{
    if (mpq_inf_p (op))
        mpq_set_inf (rop, mpq_sgn (op));
    else
        mpq_set (mpq_clr_inf (rop), op);
} // End mpiq_set


//***************************************************************************
//  $mpiq_set_z
//
//  Save an MP_INT in a rational
//***************************************************************************

void mpiq_set_z
    (mpq_t rop,                 // Destination
     mpz_t op)                  // Source

{
    if (mpz_inf_p (op))
        mpq_set_inf (rop, mpz_sgn (op));
    else
        mpq_set_z (mpq_clr_inf (rop), op);
} // End mpiq_set_z


//***************************************************************************
//  $mpiq_set_ui
//
//  Save an unsigned op1/op2 as a rational
//***************************************************************************

void mpiq_set_ui
    (mpq_t   rop,               // Destination
     mpir_ui op1,               // Source numerator
     mpir_ui op2)               // Source denominator

{
    if (op2 EQ 0)
        mpq_set_inf (rop, 1);
    else
        mpq_set_ui (mpq_clr_inf (rop), op1, op2);
} // End mpiq_set_ui


//***************************************************************************
//  $mpiq_set_si
//
//  Save a signed op1/op2 as a rational
//***************************************************************************

void mpiq_set_si
    (mpq_t   rop,               // Destination
     mpir_si op1,               // Source numerator
     mpir_ui op2)               // Source denominator

{
    if (op2 EQ 0)
        mpq_set_inf (rop, signumint (op1));
    else
        mpq_set_si (mpq_clr_inf (rop), op1, op2);
} // End mpiq_set_si


//***************************************************************************
//  $mpiq_set_str
//
//  Set rop from a string in a given base
//***************************************************************************

int mpiq_set_str
    (mpq_t rop,                 // Destination
     char *str,                 // Source
     int   base)                // Base

{
    char *p, *q;

    // Skip over white space
    p = str;
    while (isspace (*p))
        p++;
    // Find the numerator/denominator separator (if any)
    q = strchr (p, '/');
    if (q
     && mpz_set_str (mpq_denref (rop), &q[1], base) EQ 0
     && mpz_cmp_ui  (mpq_denref (rop), 0) EQ 0)
    {
        // Set to the appropriate signed infinity
        mpq_set_inf (rop, p[0] EQ '-');

        return 0;
    } else
    // If the input consists of "!" or "-!", ...
    if (strcmp (p, DEF_POSINFINITY_STR) EQ 0
     || strcmp (p, DEF_NEGINFINITY_STR) EQ 0)
    {
        // Set to the appropriate signed infinity
        mpq_set_inf (rop, p[0] EQ '-');

        return 0;
    } else
        return mpq_set_str (mpq_clr_inf (rop), str, base);
} // End mpiq_set_str


//***************************************************************************
//  $mpiq_get_d
//
//  Return the arg as a double
//***************************************************************************

double mpiq_get_d
    (mpq_t op)                  // Source

{
    if (mpq_inf_p (op))
    {
        __int64 Int64;

        Int64 = (op > 0) ? POS_INFINITY
                         : NEG_INFINITY;
        return *(double *) &Int64;
    } else
        return mpq_get_d (op);
} // End mpiq_get_d


//***************************************************************************
//  $mpiq_set_d
//
//  Set the rational value from a double
//***************************************************************************

void mpiq_set_d
    (mpq_t  rop,                // Destination
     double op)                 // Source

{
    if (IsInfinity (op))
        mpq_set_inf (rop, (op EQ fabs (op)) ? 1 : -1);
    else
        mpq_set_d (mpq_clr_inf (rop), op);
} // End mpiq_set_d


//***************************************************************************
//  $mpiq_set_fr
//
//  Set the rational value from a MPFR
//***************************************************************************

void mpiq_set_fr
    (mpq_t  rop,                // Destination
     mpfr_t op)                 // Source

{
    if (mpfr_inf_p (op))
        mpq_set_inf (rop, mpfr_sgn (op));
    else
    {
        mpf_t mpfTmp = {0};

        // Initialize the temp
        mpf_init (mpfTmp);

        // Convert from MPFR to MPF
        mpfr_get_f (mpfTmp, op, MPFR_RNDN);

        // Convert from MPF to MPQ
        mpq_set_f (mpq_clr_inf (rop), mpfTmp);

        // We no longer need this storage
        mpf_clear (mpfTmp);
    } // End IF/ELSE
} // End mpiq_set_fr


//***************************************************************************
//  $mpiq_get_str
//
//  Convert op to a string of digits in a given base
//***************************************************************************

char *mpiq_get_str
    (char *str,                 // Ptr to output string
     int   base,                // Number base
     mpq_t op)                  // Source

{
    if (mpq_inf_p (op))
    {
        __mpq_struct tmp = {0};
        char *p;

        // Initialize and set a temp to "1" or "-1"
        //   which is of sufficient size to store "!" or "-!"
        mpq_init_set_si (&tmp, mpq_sgn (op), 1);

        // Convert to "1" or "-1" using the caller's <str>
        p = mpq_get_str (str, base, &tmp);

        // Change the "1" to the inifnity char
        p[p[0] EQ '-'] = DEF_POSINFINITY_CHAR;

        // We no longer need this storage
        Myq_clear (&tmp);

        return p;
    } else
        return mpq_get_str (str, base, op);
} // End mpiq_get_str


//***************************************************************************
//  $mpiq_add
//
//  Add two integers
//***************************************************************************

void mpiq_add
    (mpq_t rop,                 // Destination
     mpq_t op1,                 // Left arg
     mpq_t op2)                 // Right arg

{
    // Split cases based upon which (if any) arg is an infinity
    switch (2 * mpq_inf_p (op1) + mpq_inf_p (op2))
    {
        case 2 * 0 + 0:     // Neither arg is an infinity
            // Call the original function
            mpq_add (mpq_clr_inf (rop), op1, op2);

            break;

        case 2 * 0 + 1:     // Op2 only is an infinity
            // The result is the infinite argument (op2)
            mpq_set_inf (rop, mpq_sgn (op2));

            break;

        case 2 * 1 + 0:     // Op1 only is an infinity
            // The result is the infinite argument (op1)
            mpq_set_inf (rop, mpq_sgn (op1));

            break;

        case 2 * 1 + 1:     // Op1 and Op2 are infinities
            // If they are the same sign, ...
            if (mpq_sgn (op1) EQ mpq_sgn (op2))
                // The result is the infinite argument (op1 or op2)
                mpq_set_inf (rop, mpq_sgn (op1));
            else
                (*gmpq_invalid) (MP_ADD, rop, op1, op2, NULL, 0);
            break;
    } // End SWITCH
} // End mpiq_add


//***************************************************************************
//  $mpiq_sub
//
//  Subtract two integers
//***************************************************************************

void mpiq_sub
    (mpq_t rop,                 // Destination
     mpq_t op1,                 // Left arg
     mpq_t op2)                 // Right arg

{
    // Split cases based upon which (if any) arg is an infinity
    switch (2 * mpq_inf_p (op1) + mpq_inf_p (op2))
    {
        case 2 * 0 + 0:     // Neither arg is an infinity
            // Call the original function
            mpq_sub (mpq_clr_inf (rop), op1, op2);

            break;

        case 2 * 0 + 1:     // Op2 only is an infinity
            // The result is the infinite argument (op2)
            mpq_set_inf (rop, mpq_sgn (op2));

            break;

        case 2 * 1 + 0:     // Op1 only is an infinity
            // The result is the infinite argument (op1)
            mpq_set_inf (rop, mpq_sgn (op1));

            break;

        case 2 * 1 + 1:     // Op1 and Op2 are infinities
            // If they are the opposite sign, ...
            if (mpq_sgn (op1) NE mpq_sgn (op2))
                // The result is the infinite argument (op1 or op2)
                mpq_set_inf (rop, mpq_sgn (op1));
            else
                (*gmpq_invalid) (MP_SUB, rop, op1, op2, NULL, 0);
            break;
    } // End SWITCH
} // End mpiq_sub


//***************************************************************************
//  $mpiq_mul
//
//  Multiply two integers
//***************************************************************************

void mpiq_mul
    (mpq_t rop,                 // Destination
     mpq_t op1,                 // Left arg
     mpq_t op2)                 // Right arg

{
    // Split cases based upon which (if any) arg is an infinity
    switch (2 * mpq_inf_p (op1) + mpq_inf_p (op2))
    {
        case 2 * 0 + 0:     // Neither arg is an infinity
            // call the original function
            mpq_mul (mpq_clr_inf (rop), op1, op2);

            break;

        case 2 * 0 + 1:     // Op2 only is an infinity
            // The result is the infinite argument (op2)
            mpq_set_inf (rop, mpq_sgn (op2));

            break;

        case 2 * 1 + 0:     // Op1 only is an infinity
            // The result is the infinite argument (op1)
            mpq_set_inf (rop, mpq_sgn (op1));

            break;

        case 2 * 1 + 1:     // Op1 and Op2 are infinities
            // The result is the infinite argument with the product of the signs
            mpq_set_inf (rop, mpq_sgn (op1) * mpq_sgn (op2));

            break;
    } // End SWITCH
} // End mpiq_mul


//***************************************************************************
//  $mpiq_mul_2exp
//
//  Multiply a rational by 2^ mp_bitcnt_t
//***************************************************************************

void mpiq_mul_2exp
    (mpq_t       rop,           // Destination
     mpq_t       op1,           // Left arg
     mp_bitcnt_t op2)           // Right arg

{
    if (mpq_inf_p (op1))
        mpq_set_inf (rop, mpq_sgn (op1));
    else
        mpq_mul_2exp (mpq_clr_inf (rop), op1, op2);
} // End mpiq_mul_2exp


//***************************************************************************
//  $mpiq_div
//
//  Divide two MPFs
//***************************************************************************

void mpiq_div
    (mpq_t rop,                 // Destination
     mpq_t op1,                 // Left arg
     mpq_t op2)                 // Right arg

{
    // Check for special cases
    if (!mpq_inf_p (op1)
     && mpq_cmp_ui (op2, 0, 1) EQ 0)
        // Divide by zero returns infinity with op1's sign
        mpq_set_inf (rop, mpq_sgn (op1));
    else
    if (mpq_inf_p (op1)
     && !mpq_inf_p (op2))
        // Divide into infinity returns infinity with op1xop2's sign
        mpq_set_inf (rop, mpq_sgn (op1) * mpq_sgn (op2));
    else
    if (!mpq_inf_p (op1)
     && mpq_inf_p (op2))
        // N / _ is 0
        mpq_set_ui (rop, 0, 1);
    else
    if (mpq_inf_p (op1)
     && mpq_inf_p (op2))
        // Infinity / infinity is undefined
        (*gmpq_invalid) (MP_DIV, rop, op1, op2, NULL, 0);
    else
        mpq_div (mpq_clr_inf (rop), op1, op2);
} // End mpiq_div


//***************************************************************************
//  $mpiq_div_2exp
//
//  Divide a rational and 2^ mp_bitcnt_t
//***************************************************************************

void mpiq_div_2exp
    (mpq_t       rop,           // Destination
     mpq_t       op1,           // Left arg
     mp_bitcnt_t op2)           // Right arg

{
    if (mpq_inf_p (op1))
        mpq_set_inf (rop, mpq_sgn (op1));
    else
        mpq_div_2exp (mpq_clr_inf (rop), op1, op2);
} // End mpiq_div_2exp


//***************************************************************************
//  $mpiq_neg
//
//  Set rop to the negative of op
//***************************************************************************

void mpiq_neg
    (mpq_t rop,                 // Destination
     mpq_t op)                  // Source

{
    if (mpq_inf_p (op))
        mpq_set_inf (rop, -mpq_sgn (op));
    else
        mpq_neg (mpq_clr_inf (rop), op);
} // End mpiq_neg


//***************************************************************************
//  $mpiq_abs
//
//  Set rop to the absolute value of op
//***************************************************************************

void mpiq_abs
    (mpq_t rop,                 // Destination
     mpq_t op)                  // Source

{
    if (mpq_inf_p (op))
        mpq_set_inf (rop, 1);
    else
        mpq_abs (mpq_clr_inf (rop), op);
} // End mpiq_abs


//***************************************************************************
//  $mpiq_inv
//
//  Set rop to 1 / op
//***************************************************************************

void mpiq_inv
    (mpq_t rop,                 // Destination
     mpq_t op)                  // Source

{
    if (mpq_inf_p (op))
        mpq_set_ui (rop, 0, 1);
    else
        mpq_inv (mpq_clr_inf (rop), op);
} // End mpiq_inv


//***************************************************************************
//  $mpiq_cmp
//
//  Compare op1 and op1, returning
//    positive if op1 >  op2,
//    zero     if op1 == op2
//    negative if op1 <  op2
//***************************************************************************

int mpiq_cmp
    (mpq_t op1,                 // Left arg
     mpq_t op2)                 // Right arg

{
    // Split cases based upon which (if any) arg is an infinity
    switch (2 * mpq_inf_p (op1) + mpq_inf_p (op2))
    {
        case 2 * 0 + 0:     // Neither arg is an infinity
            // Call the original function
            return mpq_cmp (op1, op2);

        case 2 * 0 + 1:     // Op2 only is an infinity
            // The result is the sign of the infinite argument (op2)
            return -mpq_sgn (op2);

        case 2 * 1 + 0:     // Op1 only is an infinity
            // The result is the sign of the infinite argument (op1)
            return  mpq_sgn (op1);

        case 2 * 1 + 1:     // Op1 and Op2 are infinities
            // The result is difference between the left and right argument signs
            return signumint (mpq_sgn (op1) - mpq_sgn (op2));

        default:
            return 0;       // To keep the compiler happy
    } // End SWITCH
} // End mpiq_cmp


//***************************************************************************
//  $mpiq_cmp_ui
//
//  Compare op1 and num / den, returning
//    positive if op1 >  num / den
//    zero     if op1 == num / den
//    negative if op1 <  num / den
//***************************************************************************

int mpiq_cmp_ui
    (mpq_t   op1,               // Source
     mpir_ui num,               // Numerator
     mpir_ui den)               // Denominator

{
    if (den EQ 0)
        return mpiq_cmp (op1, &mpqPosInfinity);
    else
    if (mpq_inf_p (op1))
        return mpq_sgn (op1);
    else
        return mpq_cmp_ui (op1, num, den);
} // End mpiq_cmp_ui


//***************************************************************************
//  $mpiq_cmp_si
//
//  Compare op1 and num / den, returning
//    positive if op1 >  op2,
//    zero     if op1 == op2
//    negative if op1 <  op2
//***************************************************************************

int mpiq_cmp_si
    (mpq_t   op1,               // Source
     mpir_si num,               // Numerator
     mpir_ui den)               // Denominator

{
    if (den EQ 0)
        return mpiq_cmp (op1, (num > 0) ? &mpqPosInfinity : &mpqNegInfinity);
    else
    if (mpq_inf_p (op1))
        return mpq_sgn (op1);
    else
        return mpq_cmp_si (op1, num, den);
} // End mpiq_cmp_si


//***************************************************************************
//  $mpiq_equal
//
//  Compare op1 and op1, returning
//    non-zero if op1 == op2,
//    zero     otherwise
//***************************************************************************

int mpiq_equal
    (mpq_t op1,                 // Left arg
     mpq_t op2)                 // Right arg

{
    // Split cases based upon which (if any) arg is an infinity
    switch (2 * mpq_inf_p (op1) + mpq_inf_p (op2))
    {
        case 2 * 0 + 0:     // Neither arg is an infinity
            // Call the original function
            return mpq_equal (op1, op2);

        case 2 * 0 + 1:     // Op2 only is an infinity
        case 2 * 1 + 0:     // Op1 only is an infinity
            return 0;

        case 2 * 1 + 1:     // Op1 and Op2 are infinities
            return signumint (mpq_sgn (op1) - mpq_sgn (op2));

        default:
            return 0;       // To keep the compiler happy
    } // End SWITCH
} // End mpiq_equal


//***************************************************************************
//  MPFR_ Functions
//***************************************************************************

//***************************************************************************
//  $mpfr_exit
//***************************************************************************

mpir_ui mpfr_exit
    (enum MP_ENUM mp_enum,
     mpfr_t  rop,
     mpfr_t  op1,
     mpfr_t  op2,
     mpfr_t  op3,
     mpir_ui d)

{
    exit (0);

    return 0;                   // To keep the compiler happy
} // End mpfr_exit


//***************************************************************************
//  $IsMpfNULL
//
//  Is the given value all zero?
//***************************************************************************

int IsMpfNULL
    (mpfr_t op)                 // Source

{
    return (op->_mpfr_prec EQ 0
         && op->_mpfr_exp  EQ 0
         && op->_mpfr_d    EQ NULL);
} // End IsMpfNULL


//***************************************************************************
//  $IsMpfPosInfinity
//
//  Is the given value +infinity
//***************************************************************************

int IsMpfPosInfinity
    (mpfr_t op)                 // Source

{
    return (mpfr_inf_p (op)
         && mpfr_sgn (op) > 0);
} // End IsMpfPosInfinity


//***************************************************************************
//  $IsMpfNegInfinity
//
//  Is the given value -infinity
//***************************************************************************

int IsMpfNegInfinity
    (mpfr_t op)                 // Source

{
    return (mpfr_inf_p (op)
         && mpfr_sgn (op) < 0);
} // End IsMpfNegInfinity


//***************************************************************************
//  $mpifr_copy
//
//  Copy a MPFR from a MPFR
//***************************************************************************

void mpifr_copy
    (mpfr_t rop,                // Destination
     mpfr_t op)                 // Source

{
    if (mpfr_inf_p (op))
        mpfr_set_inf (rop, mpfr_sgn (op));
    else
        mpfr_copy (mpfr_clr_inf (rop), op);
} // End mpifr_copy


//***************************************************************************
//  $mpifr_set_d
//
//  Set a MPFR from a double
//***************************************************************************

void mpifr_set_d
    (mpfr_t     rop,            // Destination
     double     op,             // Source
     mpfr_rnd_t rnd)            // Rounding mode

{
    if (IsInfinity (op))
        mpfr_set_inf (rop, (op EQ fabs (op)) ? 1 : -1);
    else
        mpfr_set_d (mpfr_clr_inf (rop), op, rnd);
} // End mpifr_set_d


//***************************************************************************
//  $mpifr_set_z
//
//  Set a MPFR from a API
//***************************************************************************

void mpifr_set_z
    (mpfr_t     rop,            // Destination
     mpz_t      op,             // Source
     mpfr_rnd_t rnd)            // Rounding mode

{
    if (mpz_inf_p (op))
        mpfr_set_inf (rop, mpz_sgn (op));
    else
        mpfr_set_z (mpfr_clr_inf (rop), op, rnd);
} // End mpifr_set_qz


//***************************************************************************
//  $mpifr_set_q
//
//  Set a MPFR from a rational
//***************************************************************************

void mpifr_set_q
    (mpfr_t     rop,            // Destination
     mpq_t      op,             // Source
     mpfr_rnd_t rnd)            // Rounding mode

{
    if (mpq_inf_p (op))
        mpfr_set_inf (rop, mpq_sgn (op));
    else
        mpfr_set_q (mpfr_clr_inf (rop), op, rnd);
} // End mpifr_set_q


//***************************************************************************
//  $mpifr_set_str
//
//  Set rop from a string in a given base
//***************************************************************************

int mpifr_set_str
    (mpfr_t     rop,            // Destination
     char      *str,            // Source
     int        base,           // Base
     mpfr_rnd_t rnd)            // Rounding mode

{
    char *p;

    // Skip over white space
    p = str;
    while (isspace (*p))
        p++;
    // If the input consists of "!" or "-!", ...
    if (strcmp (p, DEF_POSINFINITY_STR) EQ 0
     || strcmp (p, DEF_NEGINFINITY_STR) EQ 0)
    {
        // Set to the appropriate signed infinity
        mpfr_set_inf (rop, p[0] EQ '-');

        return 0;
    } else
        return mpfr_set_str (mpfr_clr_inf (rop), str, base, rnd);
} // End mpifr_set_str


//***************************************************************************
//  $mpifr_init_copy
//
//  Set a MPFR from a MPFR
//***************************************************************************

void mpifr_init_copy
    (mpfr_t rop,                // Destination
     mpfr_t op)                 // Source

{
    if (mpfr_inf_p (op))
        mpfr_set_inf (rop, mpfr_sgn (op));
    else
        mpfr_init_copy (rop, op);
} // End mpifr_init_copy


//***************************************************************************
//  $mpifr_init_set_d
//
//  Set a MPFR from a double
//***************************************************************************

void mpifr_init_set_d
    (mpfr_t     rop,            // Destination
     double     op,             // Source
     mpfr_rnd_t rnd)            // Rounding mode

{
    if (IsInfinity (op))
        mpfr_set_inf (rop, (op EQ fabs (op)) ? 1 : -1);
    else
        mpfr_init_set_d (rop, op, rnd);
} // End mpifr_init_set_d


//***************************************************************************
//  $mpifr_init_set_str
//
//  Set rop from a string in a given base
//***************************************************************************

int mpifr_init_set_str
    (mpfr_t     rop,            // Destination
     char      *str,            // Source
     int        base,           // Base
     mpfr_rnd_t rnd)            // Rounding mode

{
    mpfr_init0 (rop);
    return mpfr_set_str (rop, str, base, rnd);
} // End mpifr_init_set_str


//***************************************************************************
//  $mpifr_get_d
//
//  Convert op to a double
//***************************************************************************

double mpifr_get_d
    (mpfr_t     op,
     mpfr_rnd_t rnd)

{
    if (mpfr_inf_p (op))
        return ((mpfr_sgn (op) > 0) ? PosInfinity : NegInfinity);
    else
        return mpfr_get_d (op, rnd);
} // End mpifr_get_d


//***************************************************************************
//  $mpifr_get_d_2exp
//
//  Convert op to a double
//***************************************************************************

double mpifr_get_d_2exp
    (signed long int *exp,
     mpfr_t           op,
     mpfr_rnd_t       rnd)

{
    if (mpfr_inf_p (op))
    {
        *exp = 0;
        return ((mpfr_sgn (op) > 0) ? PosInfinity : NegInfinity);
    } else
        return mpfr_get_d_2exp (exp, op, rnd);
} // End mpifr_get_d_2exp


//***************************************************************************
//  $mpifr_get_ui
//
//  Return the arg as an unsigned integer
//***************************************************************************

unsigned long int mpifr_get_ui
    (mpfr_t     op,             // Source
     mpfr_rnd_t rnd)            // Rounding mode

{
    if (mpfr_inf_p (op))
        return (unsigned long int) 0xFFFFFFFF;
    else
        return mpfr_get_ui (op, rnd);
} // End mpifr_get_ui


//***************************************************************************
//  $mpifr_get_si
//
//  Return the arg as an signed integer
//***************************************************************************

signed long int mpifr_get_si
    (mpfr_t     op,             // Source
     mpfr_rnd_t rnd)            // Rounding mode

{
    if (mpfr_inf_p (op))
        return (signed long int) 0xFFFFFFFF;
    else
        return mpfr_get_si (op, rnd);
} // End mpifr_get_si


//***************************************************************************
//  $mpifr_get_str
//
//  Convert op to a string of digits in a given base
//***************************************************************************

char *mpifr_get_str
    (char      *str,
     mp_exp_t  *expptr,
     int        base,
     size_t     n_digits,
     mpfr_t     op,
     mpfr_rnd_t rnd)            // Rounding mode

{
    if (mpfr_inf_p (op))
    {
        __mpfr_struct tmp = {0};
        char *p;

        // Initialize and set a temp to "1" or "-1"
        //   which is of sufficient size to store "!" or "-!"
        mpfr_init_set_si (&tmp, mpfr_sgn (op), rnd);

        // Convert to "1" or "-1" using the caller's <str>
        p = mpfr_get_str (str, expptr, base, n_digits, &tmp, rnd);

        // Change the "1" to the inifnity char
        p[p[0] EQ '-'] = DEF_POSINFINITY_CHAR;

        // We no longer need this storage
        Myf_clear (&tmp);

        return p;
    } else
        return mpfr_get_str (str, expptr, base, n_digits, op, rnd);
} // End mpifr_get_str


//***************************************************************************
//  $mpifr_get_z
//
//  Return the arg as an MPINT
//***************************************************************************

int mpifr_get_z
    (mpz_t      rop,            // Destination
     mpfr_t     op,             // Source
     mpfr_rnd_t rnd)            // Rounding mode

{
    if (mpfr_inf_p (op))
    {
        mpz_set_inf (rop, mpfr_sgn (op));

        return 0;
    } else
        return mpfr_get_z (mpz_clr_inf (rop), op, rnd);
} // End mpifr_get_z


//***************************************************************************
//  $mpifr_add
//
//  Add two MPFs
//***************************************************************************

void mpifr_add
    (mpfr_t     rop,            // Destination
     mpfr_t     op1,            // Left arg
     mpfr_t     op2,            // Right arg
     mpfr_rnd_t rnd)            // Rounding mode

{
    // Split cases based upon which (if any) arg is an infinity
    switch (2 * mpfr_inf_p (op1) + mpfr_inf_p (op2))
    {
        case 2 * 0 + 0:     // Neither arg is an infinity
            // Call the original function
            mpfr_add (mpfr_clr_inf (rop), op1, op2, rnd);

            break;

        case 2 * 0 + 1:     // Op2 only is an infinity
            // The result is the infinite argument (op2)
            mpfr_set_inf (rop, mpfr_sgn (op2));

            break;

        case 2 * 1 + 0:     // Op1 only is an infinity
            // The result is the infinite argument (op1)
            mpfr_set_inf (rop, mpfr_sgn (op1));

            break;

        case 2 * 1 + 1:     // Op1 and Op2 are infinities
            // If they are the same sign, ...
            if (mpfr_sgn (op1) EQ mpfr_sgn (op2))
                // The result is the infinite argument (op1 or op2)
                mpfr_set_inf (rop, mpfr_sgn (op1));
            else
                (*gmpfr_invalid) (MP_ADD, rop, op1, op2, NULL, 0);
            break;
    } // End SWITCH
} // End mpifr_add


//***************************************************************************
//  $mpifr_add_ui
//
//  Add an MPFR and an unsigned long int
//***************************************************************************

void mpifr_add_ui
    (mpfr_t            rop,     // Destination
     mpfr_t            op1,     // Left arg
     unsigned long int op2,     // Right arg
     mpfr_rnd_t        rnd)     // Rounding mode

{
    if (mpfr_inf_p (op1))
        mpfr_set_inf (rop, mpfr_sgn (op1));
    else
        mpfr_add_ui (mpfr_clr_inf (rop), op1, op2, rnd);
} // End mpifr_add_ui


//***************************************************************************
//  $mpifr_sub
//
//  Subtract two MPFs
//***************************************************************************

void mpifr_sub
    (mpfr_t     rop,            // Destination
     mpfr_t     op1,            // Left arg
     mpfr_t     op2,            // Right arg
     mpfr_rnd_t rnd)            // Rounding mode

{
    // Split cases based upon which (if any) arg is an infinity
    switch (2 * mpfr_inf_p (op1) + mpfr_inf_p (op2))
    {
        case 2 * 0 + 0:     // Neither arg is an infinity
            // Call the original function
            mpfr_sub (mpfr_clr_inf (rop), op1, op2, rnd);

            break;

        case 2 * 0 + 1:     // Op2 only is an infinity
            // The result is the infinite argument (op2)
            mpfr_set_inf (rop, mpfr_sgn (op2));

            break;

        case 2 * 1 + 0:     // Op1 only is an infinity
            // The result is the infinite argument (op1)
            mpfr_set_inf (rop, mpfr_sgn (op1));

            break;

        case 2 * 1 + 1:     // Op1 and Op2 are infinities
            // If they are the opposite sign, ...
            if (mpfr_sgn (op1) NE mpfr_sgn (op2))
                // The result is the infinite argument (op1 or op2)
                mpfr_set_inf (rop, mpfr_sgn (op1));
            else
                (*gmpfr_invalid) (MP_SUB, rop, op1, op2, NULL, 0);
            break;
    } // End SWITCH
} // End mpifr_sub


//***************************************************************************
//  $mpifr_ui_sub
//
//  Subtract an unsigned long int and a MPFR
//***************************************************************************

void mpifr_ui_sub
    (mpfr_t            rop,     // Destination
     unsigned long int op1,     // Left arg
     mpfr_t            op2,     // Right arg
     mpfr_rnd_t        rnd)     // Rounding mode

{
    if (mpfr_inf_p (op2))
        mpfr_set_inf (rop, -mpfr_sgn (op2));
    else
        mpfr_ui_sub (mpfr_clr_inf (rop), op1, op2, rnd);
} // End mpifr_ui_sub


//***************************************************************************
//  $mpifr_sub_ui
//
//  Subtract a MPFR and an unsigned long int
//***************************************************************************

void mpifr_sub_ui
    (mpfr_t            rop,     // Destination
     mpfr_t            op1,     // Left arg
     unsigned long int op2,     // Right arg
     mpfr_rnd_t        rnd)     // Rounding mode

{
    if (mpfr_inf_p (op1))
        mpfr_set_inf (rop, mpfr_sgn (op1));
    else
        mpfr_sub_ui (mpfr_clr_inf (rop), op1, op2, rnd);
} // End mpifr_sub_ui


//***************************************************************************
//  $mpifr_mul
//
//  Multiply two MPFs
//***************************************************************************

void mpifr_mul
    (mpfr_t     rop,            // Destination
     mpfr_t     op1,            // Left arg
     mpfr_t     op2,            // Right arg
     mpfr_rnd_t rnd)            // Rounding mode

{
    // Split cases based upon which (if any) arg is an infinity
    switch (2 * mpfr_inf_p (op1) + mpfr_inf_p (op2))
    {
        case 2 * 0 + 0:     // Neither arg is an infinity
            // Call the original function
            mpfr_mul (mpfr_clr_inf (rop), op1, op2, rnd);

            break;

        case 2 * 0 + 1:     // Op2 only is an infinity
            // The result is the infinite argument (op2)
            mpfr_set_inf (rop, mpfr_sgn (op2));

            break;

        case 2 * 1 + 0:     // Op1 only is an infinity
            // The result is the infinite argument (op1)
            mpfr_set_inf (rop, mpfr_sgn (op1));

            break;

        case 2 * 1 + 1:     // Op1 and Op2 are infinities
            // The result is the infinite argument with the product of the signs
            mpfr_set_inf (rop, mpfr_sgn (op1) * mpfr_sgn (op2));

            break;
    } // End SWITCH
} // End mpifr_mul


//***************************************************************************
//  $mpifr_mul_ui
//
//  Multiply a MPFR and an unsigned long int
//***************************************************************************

void mpifr_mul_ui
    (mpfr_t          rop,       // Destination
     mpfr_t          op1,       // Left arg
     signed long int op2,       // Right arg
     mpfr_rnd_t      rnd)       // Rounding mode

{
    if (mpfr_inf_p (op1))
        mpfr_set_inf (rop, mpfr_sgn (op1));
    else
        // Call the original function
        mpfr_mul_ui (mpfr_clr_inf (rop), op1, op2, rnd);
} // End mpifr_mul_ui


//***************************************************************************
//  $mpifr_div
//
//  Divide two MPFs
//***************************************************************************

void mpifr_div
    (mpfr_t     rop,            // Destination
     mpfr_t     op1,            // Left arg
     mpfr_t     op2,            // Right arg
     mpfr_rnd_t rnd)            // Rounding mode

{
    // Check for special cases
    if (!mpfr_inf_p (op2)
     && mpfr_cmp_ui (op2, 0) EQ 0)
        // Divide by zero returns infinity with op1's sign
        mpfr_set_inf (rop, mpfr_sgn (op1));
    else
    if (mpfr_inf_p (op1)
     && !mpfr_inf_p (op2))
        // Divide into infinity returns infinity with op1xop2's sign
        mpfr_set_inf (rop, mpfr_sgn (op1) * mpfr_sgn (op2));
    else
    if (!mpfr_inf_p (op1)
     && mpfr_inf_p (op2))
        // N / _ is 0
        mpfr_set_ui (rop, 0, rnd);
    else
    if (mpfr_inf_p (op1)
     && mpfr_inf_p (op2))
        // Infinity / infinity is undefined
        (*gmpfr_invalid) (MP_DIV, rop, op1, op2, NULL, 0);
    else
        // Call the original function
        mpfr_div (mpfr_clr_inf (rop), op1, op2, rnd);
} // End mpifr_div


//***************************************************************************
//  $mpifr_ui_div
//
//  Divide an unsigned long int and a MPFR
//***************************************************************************

void mpifr_ui_div
    (mpfr_t            rop,     // Destination
     unsigned long int op1,     // Left arg
     mpfr_t            op2,     // Right arg
     mpfr_rnd_t        rnd)     // Rounding mode

{
    // Check for special cases
    if (mpfr_inf_p (op2))
        // Divide infinity into finite returns zero
        mpfr_set_ui (mpfr_clr_inf (rop), 0, rnd);
    else
        // Call the original function
        mpfr_ui_div (mpfr_clr_inf (rop), op1, op2, rnd);
} // End mpifr_ui_div


//***************************************************************************
//  $mpifr_div_ui
//
//  Divide a MPFR and an unsigned long int
//***************************************************************************

void mpifr_div_ui
    (mpfr_t            rop,     // Destination
     mpfr_t            op1,     // Left arg
     unsigned long int op2,     // Right arg
     mpfr_rnd_t        rnd)     // Rounding mode

{
    // Check for special cases
    if (op2 EQ 0)
        // Divide by zero returns infinity with op1's sign
        mpfr_set_inf (rop, mpfr_sgn (op1));
    else
    if (mpfr_inf_p (op1))
        // Divide into infinity returns infinity with op1xop2's sign
        mpfr_set_inf (rop, mpfr_sgn (op1) * signumint (op2));
    else
        mpfr_div_ui (mpfr_clr_inf (rop), op1, op2, rnd);
} // End mpifr_div_ui


//***************************************************************************
//  $mpifr_sqrt
//
//  Set rop to the square root of op
//***************************************************************************

void mpifr_sqrt
    (mpfr_t     rop,            // Destination
     mpfr_t     op,             // Source
     mpfr_rnd_t rnd)            // Rounding mode

{
    if (mpfr_inf_p (op))
    {
        if (mpfr_sgn (op) > 0)
            mpfr_set_inf (rop, -mpfr_sgn (op));
        else
            (*gmpfr_invalid) (MP_SQRT, rop, op, NULL, NULL, 0);
     }else
        mpfr_sqrt (mpfr_clr_inf (rop), op, rnd);
} // End mpifr_sqrt


//***************************************************************************
//  $mpifr_neg
//
//  Set rop to the negative of op
//***************************************************************************

void mpifr_neg
    (mpfr_t     rop,            // Destination
     mpfr_t     op,             // Source
     mpfr_rnd_t rnd)            // Rounding mode

{
    if (mpfr_inf_p (op))
        mpfr_set_inf (rop, -mpfr_sgn (op));
    else
        mpfr_neg0 (mpfr_clr_inf (rop), op, rnd);
} // End mpifr_neg


//***************************************************************************
//  $mpifr_abs
//
//  Set rop to the absolute value of op
//***************************************************************************

void mpifr_abs
    (mpfr_t     rop,            // Destination
     mpfr_t     op,             // Source
     mpfr_rnd_t rnd)            // Rounding mode

{
    if (mpfr_inf_p (op))
        mpfr_set_inf (rop, 1);
    else
        mpfr_abs (mpfr_clr_inf (rop), op, rnd);
} // End mpifr_abs


//***************************************************************************
//  $mpifr_mul_2exp
//
//  Multiply a MPFR and 2^unsigned long int
//***************************************************************************

void mpifr_mul_2exp
    (mpfr_t      rop,           // Destination
     mpfr_t      op1,           // Left arg
     mp_bitcnt_t op2,           // Right arg
     mpfr_rnd_t  rnd)           // Rounding mode

{
    if (mpfr_inf_p (op1))
        mpfr_set_inf (rop, mpfr_sgn (op1));
    else
        mpfr_mul_2exp (mpfr_clr_inf (rop), op1, (unsigned long) op2, rnd);
} // End mpifr_mul_2exp


//***************************************************************************
//  $mpifr_div_2exp
//
//  Divide a MPFR and 2^unsigned long int
//***************************************************************************

void mpifr_div_2exp
    (mpfr_t      rop,           // Destination
     mpfr_t      op1,           // Left arg
     mp_bitcnt_t op2,           // Right arg
     mpfr_rnd_t  rnd)           // Rounding mode

{
    if (mpfr_inf_p (op1))
        mpfr_set_inf (rop, mpfr_sgn (op1));
    else
        mpfr_div_2exp (mpfr_clr_inf (rop), op1, (unsigned long) op2, rnd);
} // End mpifr_div_2exp


//***************************************************************************
//  $mpifr_cmp
//
//  Compare op1 and op1, returning
//    positive if op1 >  op2,
//    zero     if op1 == op2
//    negative if op1 <  op2
//***************************************************************************

int mpifr_cmp
    (mpfr_t op1,                // Left arg
     mpfr_t op2)                // Right arg

{
    // Split cases based upon which (if any) arg is an infinity
    switch (2 * mpfr_inf_p (op1) + mpfr_inf_p (op2))
    {
        case 2 * 0 + 0:     // Neither arg is an infinity
            // Call the original function
            return mpfr_cmp (op1, op2);

        case 2 * 0 + 1:     // Op2 only is an infinity
            // The result is the opposite sign of the infinite argument (op2)
            return -mpfr_sgn (op2);

        case 2 * 1 + 0:     // Op1 only is an infinity
            // The result is the same sign as the infinite argument (op1)
            return  mpfr_sgn (op1);

        case 2 * 1 + 1:     // Op1 and Op2 are infinities
            // The result is the difference between the argument's signs
            return signumint (mpfr_sgn (op1) - mpfr_sgn (op2));

        default:
            return 0;       // To keep the compiler happy
    } // End SWITCH
} // End mpifr_cmp


//***************************************************************************
//  $mpifr_cmp_ui
//
//  Compare op1 and op1, returning
//    positive if op1 >  op2,
//    zero     if op1 == op2
//    negative if op1 <  op2
//***************************************************************************

int mpifr_cmp_ui
    (mpfr_t            op1,     // Left arg
     unsigned long int op2)     // Right arg

{
    if (mpfr_inf_p (op1))
        return mpfr_sgn (op1);
    else
        return mpfr_cmp_ui (op1, op2);
} // End mpifr_cmp_ui


//***************************************************************************
//  $mpifr_cmp_si
//
//  Compare op1 and op1, returning
//    positive if op1 >  op2,
//    zero     if op1 == op2
//    negative if op1 <  op2
//***************************************************************************

int mpifr_cmp_si
    (mpfr_t          op1,       // Left arg
     signed long int op2)       // Right arg

{
    if (mpfr_inf_p (op1))
        return mpfr_sgn (op1);
    else
        return mpfr_cmp_si (op1, op2);
} // End mpifr_cmp_si


//***************************************************************************
//  $mpifr_cmp_d
//
//  Compare op1 and op1, returning
//    positive if op1 >  op2,
//    zero     if op1 == op2
//    negative if op1 <  op2
//***************************************************************************

int mpifr_cmp_d
    (mpfr_t op1,                // Left arg
     double op2)                // Right arg

{
    if (mpfr_inf_p (op1))
        return mpfr_sgn (op1);
    else
        return mpfr_cmp_d (op1, op2);
} // End mpifr_cmp_d


//***************************************************************************
//  $mpifr_eq
//
//  Return non-zero if the first op3 bits of op1 and op2 are equal,
//    zero otherwise.
//***************************************************************************

int mpifr_eq
    (mpfr_t            op1,     // Left arg
     mpfr_t            op2,     // Right arg
     unsigned long int op3)     // Bit count

{
    // Split cases based upon which (if any) arg is an infinity
    switch (2 * mpfr_inf_p (op1) + mpfr_inf_p (op2))
    {
        case 2 * 0 + 0:     // Neither arg is an infinity
            // Call the original function
            return mpfr_eq (op1, op2, op3);

        case 2 * 0 + 1:     // Op2 only is an infinity
        case 2 * 1 + 0:     // Op1 only is an infinity
            return 0;

        case 2 * 1 + 1:     // Op1 and Op2 are infinities
            // If they are the same sign, ...
            return (mpfr_sgn (op1) EQ mpfr_sgn (op2));

        default:
            return 0;       // To keep the compiler happy
    } // End SWITCH
} // End mpifr_eq


//***************************************************************************
//  $mpifr_reldiff
//
//  Return (abs (op1 - op2)) / op1
//***************************************************************************

void mpifr_reldiff
    (mpfr_t     rop,            // Destination
     mpfr_t     op1,            // Left arg
     mpfr_t     op2,            // Right arg
     mpfr_rnd_t rnd)            // Rounding mode

{
    UBOOL bZ1,
          bZ2;

    // Split cases based upon which (if any) arg is an infinity
    switch (2 * mpfr_inf_p (op1) + mpfr_inf_p (op2))
    {
        case 2 * 0 + 0:     // Neither arg is an infinity
            bZ1 = mpfr_zero_p (op1);
            bZ2 = mpfr_zero_p (op2);

            if (bZ1 && bZ2)
                mpfr_set_ui (mpfr_clr_inf (rop), 0, MPFR_RNDN);
            else
            if (bZ1 || bZ2)
                mpfr_set_ui (mpfr_clr_inf (rop), 1, MPFR_RNDN);
            else
                // Call the original function
                mpfr_reldiff (mpfr_clr_inf (rop), op1, op2, rnd);

            break;

        case 2 * 0 + 1:     // Op2 only is an infinity
            // The result is an infinity with the sign of finite argument (op1)
            mpfr_set_inf (rop, mpfr_sgn (op1));

            break;

        case 2 * 1 + 0:     // Op1 only is an infinity
        case 2 * 1 + 1:     // Op1 and Op2 are infinities
            (*gmpfr_invalid) (MP_RELDIFF, rop, op1, op2, NULL, 0);

            break;
    } // End SWITCH
} // End mpifr_reldiff


//***************************************************************************
//  $mpifr_ceil
//
//  Calculate the ceiling of op
//***************************************************************************

void mpifr_ceil
    (mpfr_t rop,                // Destination
     mpfr_t op)                 // Source

{
    if (mpfr_inf_p (op))
        mpfr_set_inf (rop, mpfr_sgn (op));
    else
        mpfr_ceil (mpfr_clr_inf (rop), op);
} // End mpifr_ceil


//***************************************************************************
//  $mpifr_floor
//
//  Calculate the floor of op
//***************************************************************************

void mpifr_floor
    (mpfr_t rop,                // Destination
     mpfr_t op)                 // Source

{
    if (mpfr_inf_p (op))
        mpfr_set_inf (rop, mpfr_sgn (op));
    else
        mpfr_floor (mpfr_clr_inf (rop), op);
} // End mpifr_floor


//***************************************************************************
//  $mpifr_trunc
//
//  Calculate the truncation of op
//***************************************************************************

void mpifr_trunc
    (mpfr_t rop,                // Destination
     mpfr_t op)                 // Source

{
    if (mpfr_inf_p (op))
        mpfr_set_inf (rop, mpfr_sgn (op));
    else
        mpfr_trunc (mpfr_clr_inf (rop), op);
} // End mpifr_trunc


//***************************************************************************
//  $mpifr_integer_p
//
//  Return non-zero if op is an integer
//***************************************************************************

int mpifr_integer_p
    (mpfr_t op)                 // Source

{
    if (mpfr_inf_p (op))
        return 1;
    else
        return mpfr_integer_p (op);
} // End mpifr_integer_p


//***************************************************************************
//  $mpifr_fits_ulong_p
//
//  Return non-zero if op fits in an unsigned long int
//***************************************************************************

int mpifr_fits_ulong_p
    (mpfr_t     op,             // Source
     mpfr_rnd_t rnd)            // Rounding mode

{
    if (mpfr_inf_p (op))
        return 0;
    else
        return mpfr_fits_ulong_p (op, rnd);
} // End mpifr_fits_ulong_p


//***************************************************************************
//  $mpifr_fits_slong_p
//
//  Return non-zero if op fits in a signed long int
//***************************************************************************

int mpifr_fits_slong_p
    (mpfr_t     op,             // Source
     mpfr_rnd_t rnd)            // Rounding mode

{
    if (mpfr_inf_p (op))
        return 0;
    else
        return mpfr_fits_slong_p (op, rnd);
} // End mpifr_fits_slong_p


//***************************************************************************
//  $mpifr_fits_uint_p
//
//  Return non-zero if op fits in an unsigned int
//***************************************************************************

int mpifr_fits_uint_p
    (mpfr_t     op,             // Source
     mpfr_rnd_t rnd)            // Rounding mode

{
    if (mpfr_inf_p (op))
        return 0;
    else
        return mpfr_fits_uint_p (op, rnd);
} // End mpifr_fits_uint_p


//***************************************************************************
//  $mpifr_fits_sint_p
//
//  Return non-zero if op fits in a signed int
//***************************************************************************

int mpifr_fits_sint_p
    (mpfr_t     op,             // Source
     mpfr_rnd_t rnd)            // Rounding mode

{
    if (mpfr_inf_p (op))
        return 0;
    else
        return mpfr_fits_sint_p (op, rnd);
} // End mpifr_fits_sint_p


//***************************************************************************
//  $mpifr_fits_ushort_p
//
//  Return non-zero if op fits in an unsigned short
//***************************************************************************

int mpifr_fits_ushort_p
    (mpfr_t     op,             // Source
     mpfr_rnd_t rnd)            // Rounding mode

{
    if (mpfr_inf_p (op))
        return 0;
    else
        return mpfr_fits_ushort_p (op, rnd);
} // End mpifr_fits_ushort_p


//***************************************************************************
//  $mpifr_fits_sshort_p
//
//  Return non-zero if op fits in a signed short
//***************************************************************************

int mpifr_fits_sshort_p
    (mpfr_t     op,             // Source
     mpfr_rnd_t rnd)            // Rounding mode

{
    if (mpfr_inf_p (op))
        return 0;
    else
        return mpfr_fits_sshort_p (op, rnd);
} // End mpifr_fits_sshort_p


//***************************************************************************
//  $mpifr_pow_ui
//
//  Set rop to op1^op2
//***************************************************************************

void mpifr_pow_ui
    (mpfr_t            rop,     // Destination
     mpfr_t            op1,     // Left arg
     unsigned long int op2,     // Right arg
     mpfr_rnd_t        rnd)     // Rounding mode

{
    if (mpfr_inf_p (op1))
        mpfr_set_inf (rop, (op2 & 1) ? mpfr_sgn (op1) : 1);
    else
        mpfr_pow_ui (mpfr_clr_inf (rop), op1, op2, rnd);
} // End mpifr_pow_ui


//***************************************************************************
//  $mpifr_pow_z
//
//  Set rop to op1^op2
//***************************************************************************

void mpifr_pow_z
    (mpfr_t            rop,     // Destination
     mpfr_t            op1,     // Left arg
     mpz_t             op2,     // Right arg
     mpfr_rnd_t        rnd)     // Rounding mode

{
    if (mpfr_inf_p (op1))
    {
        // If op2 is -infinity, ...
        if (IsMpzInfinity (op2) && mpz_sgn (op2) < 0)
            mpfr_set_ui (mpfr_clr_inf (rop), 0, MPFR_RNDN);
        else
            mpfr_set_inf (rop, mpz_odd_p (op2) ? mpfr_sgn (op1) : 1);
    } else
    // If op2 is infinity, ...
    if (IsMpzInfinity (op2))
    {
        // The caller has already handled
        //    0   *   R   for any R
        //    L   *   _   for  L <= -1
        //    L   *  -_   for -1 <=  L < 0

        // Split cases based upon the range of op1

        // If op2 is +{infinity}, ...
        if (mpz_sgn (op2) > 0)
        {
            // Izit in (-1, 1) , ...
            if (mpfr_cmp_si (op1,  1) < 0
             && mpfr_cmp_si (op1, -1) > 0)
                // Result is 0
                mpfr_set_ui (mpfr_clr_inf (rop), 0, MPFR_RNDN);
            else
                // Result is {infinity}
                mpfr_set_inf (mpfr_clr_inf (rop), 1);
        } else
            // Izit < -1 or > 1, ...
            if (mpfr_cmp_si (op1, -1) < 0
             || mpfr_cmp_si (op1,  1) > 0)
                // Result is 0
                mpfr_set_ui (mpfr_clr_inf (rop), 0, MPFR_RNDN);
            else
                // Result is {infinity}
                mpfr_set_inf (mpfr_clr_inf (rop), 1);
    } else
        mpfr_pow_z (mpfr_clr_inf (rop), op1, op2, rnd);
} // End mpifr_pow_z


//***************************************************************************
//  End of File: mpifns.c
//***************************************************************************
