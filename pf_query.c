//***************************************************************************
//  NARS2000 -- Primitive Function -- Query
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
#include <math.h>
#include "headers.h"
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>


#ifndef PROTO
PRIMSPEC PrimSpecQuery =
{
    // Monadic functions
    &PrimFnMon_EM_YY,
    &PrimSpecQueryStorageTypeMon,
    NULL,   // &PrimFnMonQueryAPA_EM, -- Can't happen w/Query

    NULL,   // &PrimFnMonQueryBisB, -- Can't happen w/Query
    NULL,   // &PrimFnMonQueryBisI, -- Can't happen w/Query
    NULL,   // &PrimFnMonQueryBisF, -- Can't happen w/Query

////               IisB,     // Handled via type promotion (to IisI)
    &PrimFnMonQueryIisI,
    &PrimFnMonQueryIisF,

////               FisB,     // Handled via type promotion (to FisI)
    NULL,   // &PrimFnMonQueryFisI, -- Can't happen w/Query
    NULL,   // &PrimFnMonQueryFisF, -- can't happen w/Query

    &PrimFnMonQueryRisR,

////               VisR,     // Handled via type promotion (to VisV)
    &PrimFnMonQueryVisV,

    // Dyadic functions
    NULL,   // &PrimFnDyd_EM, -- Dyadic Query not scalar
    NULL,   // &PrimSpecQueryStorageTypeDyd, -- Dyadic Query not scalar
    NULL,   // &PrimFnDydQueryAPA_EM, -- Dyadic Query not scalar

    NULL,   // &PrimFnDydQueryBisBvB, -- Dyadic Query not scalar
    NULL,   // &PrimFnDydQueryBisIvI, -- Dyadic Query not scalar
    NULL,   // &PrimFnDydQueryBisFvF, -- Dyadic Query not scalar
    NULL,   // &PrimFnDydQueryBisCvC, -- Dyadic Query not scalar

////                 IisBvB,    // Handled via type promotion (to IisIvI)
    NULL,   // &PrimFnDydQueryIisIvI,
    NULL,   // &PrimFnDydQueryIisFvF, -- Dyadic Query not scalar

////                 FisBvB,    // Handled via type promotion (to FisIvI)
    NULL,   // &PrimFnDydQueryFisIvI, -- Dyadic Query not scalar
    NULL,   // &PrimFnDydQueryFisFvF, -- Dyadic Query not scalar

    NULL,   // &PrimFnDydQueryBisRvR, -- Dyadic Query not scalar
    NULL,   // &PrimFnDydQueryRisRvR, -- Dyadic Query not scalar

    NULL,   // &PrimFnDydQueryBisVvV, -- Dyadic Query not scalar
////                 VisRvR     // Handled via type promotion (to VisVvV)
    NULL,   // &PrimFnDydQueryVisVvV, -- Dyadic Query not scalar
};

static LPPRIMSPEC lpPrimSpec = {&PrimSpecQuery};
#endif


//***************************************************************************
//  $PrimFnQuery_EM_YY
//
//  Primitive function for monadic and dyadic Query ("roll" and "deal")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnQuery_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnQuery_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_QUERY);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return (*lpPrimSpec->PrimFnMon_EM_YY) (            lptkFunc, lptkRhtArg, lptkAxis, lpPrimSpec);
    else
        return PrimFnDydQuery_EM_YY           (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnQuery_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnQuery_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic Query
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnQuery_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnQuery_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // If the left arg is not present, ...
    if (lptkLftArg EQ NULL)
        //***************************************************************
        // Called monadically
        //***************************************************************
        return PrimProtoFnScalar_EM_YY (lptkLftArg,         // Ptr to left arg token
                                        lptkFunc,           // Ptr to function token
                                        lptkRhtArg,         // Ptr to right arg token
                                        lptkAxis);          // Ptr to axis token (may be NULL)
    else
        //***************************************************************
        // Called dyadically
        //***************************************************************

        // Convert to a prototype
        return PrimProtoFnMixed_EM_YY (&PrimFnQuery_EM_YY,  // Ptr to primitive function routine
                                        lptkLftArg,         // Ptr to left arg token
                                        lptkFunc,           // Ptr to function token
                                        lptkRhtArg,         // Ptr to right arg token
                                        lptkAxis);          // Ptr to axis token (may be NULL)
} // End PrimProtoFnQuery_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimSpecQueryStorageTypeMon
//
//  Primitive monadic scalar function special handling:  Storage type
//***************************************************************************

APLSTYPE PrimSpecQueryStorageTypeMon
    (APLNELM    aplNELMRht,
     LPAPLSTYPE lpaplTypeRht,
     LPTOKEN    lptkFunc)

{
    APLSTYPE aplTypeRes;

    // In case the right arg is an empty char,
    //   change its type to BOOL
    if (IsCharEmpty (*lpaplTypeRht, aplNELMRht))
        *lpaplTypeRht = ARRAY_BOOL;

    // Weed out chars & heteros
    if (IsSimpleCH (*lpaplTypeRht))
        return ARRAY_ERROR;

    // The storage type of the result is
    //   the same as that of the right arg
    //   except SimpleNum goes to INT
    if (IsSimpleNum (*lpaplTypeRht))
        aplTypeRes = ARRAY_INT;
    else
        aplTypeRes = *lpaplTypeRht;
    return aplTypeRes;
} // End PrimSpecQueryStorageTypeMon


//***************************************************************************
//  $PrimFnMonQueryIisI
//
//  Primitive scalar function monadic Query:  I {is} fn I
//
//  This algorithm was taken from
//    "How the Roll Function Works" by E. E. McDonnell,
//    APL Quote Quad, Vol. 8, Number 3, p. 42.
//***************************************************************************

APLINT PrimFnMonQueryIisI
    (APLINT     aplIntegerRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLBOOL      bQuadIO;       // []IO
    APLUINT      uQuadRL;       // []RL

    // Get the current value of []IO & []RL
    bQuadIO = GetQuadIO ();
    uQuadRL = lpPrimSpec->uQuadRL;

    // Check for DOMAIN ERROR
    if (aplIntegerRht < bQuadIO)
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    // Calculate the next QuadRL
    uQuadRL = NextQuadRL (uQuadRL);

    // Save uQuadRL back into lpPrimSpec
    lpPrimSpec->uQuadRL = uQuadRL;

    return bQuadIO + GetRandomNum (aplIntegerRht, uQuadRL, GetMemPTD ());
} // End PrimFnMonQueryIisI


//***************************************************************************
//  $PrimFnMonQueryIisF
//
//  Primitive scalar function monadic Query:  I {is} fn F
//
//  This algorithm was taken from
//    "How the Roll Function Works" by E. E. McDonnell,
//    APL Quote Quad, Vol. 8, Number 3, p. 42.
//***************************************************************************

APLINT PrimFnMonQueryIisF
    (APLFLOAT   aplFloatRht,
     LPPRIMSPEC lpPrimSpec)

{
    // Check for DOMAIN ERROR
    if (aplFloatRht NE floor (aplFloatRht))
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    return PrimFnMonQueryIisI ((APLINT) aplFloatRht, lpPrimSpec);
} // End PrimFnMonQueryIisF


//***************************************************************************
//  $NextQuadRL
//
//  Return the next value of []RL given the current one
//***************************************************************************

APLUINT NextQuadRL
    (APLUINT uQuadRL)           // Current value of []RL

{
    // Generate the next value of QuadRL
    // #define NextQuadRL(a)       (((a) * DEF_QUADRL_CWS) % QUADRL_MODULUS);
    APLUINT aplIntegerRes;      // The result
    mpz_t   mpzTmp1 = {0},      // Temp
            mpzTmp2 = {0};      // ...

    // Initialize the temps
    mpz_init (mpzTmp1);
    mpz_init (mpzTmp2);

    // Tmp1 = uQuadRL
    // Tmp2 = DEF_QUADRL_CWS
    mpz_set_ux (mpzTmp1, uQuadRL);
    mpz_set_ux (mpzTmp2, DEF_QUADRL_CWS);

    // Tmp1 = Tmp1 * Tmp2
    mpz_mul    (mpzTmp1, mpzTmp1, mpzTmp2);

    // Tmp2 = QUADRL_MODULUS
    mpz_set_ux (mpzTmp2, QUADRL_MODULUS);

    // Tmp1 = Tmp1 % Tmp2
    mpz_tdiv_r (mpzTmp1, mpzTmp1, mpzTmp2);

    // aplIntegerRht = Tmp1
    aplIntegerRes = mpz_get_sx (mpzTmp1);

    // We no longer need this storage
    Myz_clear (mpzTmp2);
    Myz_clear (mpzTmp1);

    return aplIntegerRes;
} // End NextQuadRL


//***************************************************************************
//  $GetRandomNum
//
//  Return a random number
//***************************************************************************

APLINT GetRandomNum
    (APLINT       aplIntegerRht,        // The integer arg
     APLUINT      uQuadRL,              // []RL
     LPPERTABDATA lpMemPTD)             // Ptr to PerTabData global memory

{
    APLINT   aplIntRes;                 // The integer result
    APLFLOAT aplFloatRht,               // The arg as a float
             aplFloatRes;               // The FP result
    mpz_t    mpzTmp1 = {0},             // Temp
             mpzTmp2 = {0};             // ...

    // Split cases based upon the Distribution Type
    switch (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_DT]->stData.stChar)
    {
        case L'r':
            // Initialize the temps
            mpz_init (mpzTmp1);
            mpz_init (mpzTmp2);

            // Tmp1 = uQuadRL
            // Tmp2 = aplIntegerRht
            mpz_set_ux (mpzTmp1, uQuadRL);
            mpz_set_sx (mpzTmp2, aplIntegerRht);

            // Tmp1 = Tmp1 * Tmp2
            mpz_mul    (mpzTmp1, mpzTmp1, mpzTmp2);

            // Tmp2 = QUADRL_MODULUS
            mpz_set_ux (mpzTmp2, QUADRL_MODULUS);

            // Tmp1 = Tmp1 / Tmp2
            mpz_tdiv_q (mpzTmp1, mpzTmp1, mpzTmp2);

            // aplIntegerRht = Tmp1
            aplIntegerRht = mpz_get_sx (mpzTmp1);

            Myz_clear (mpzTmp2);
            Myz_clear (mpzTmp1);

            return aplIntegerRht;

        case L'g':
            // If the rng has not been allocated as yet, ...
            if (lpMemPTD->gslRNG EQ NULL)
                // Allocate storage for a RNG of type
                // The algorithm chosen (minstd) is completely arbitrary
                lpMemPTD->gslRNG = gsl_rng_alloc (gsl_rng_minstd);

            // Convert the range to [0, aplIntegerRht)
            aplIntegerRht--;

#define SD      2       // Standard deviations
            // Generate a Gaussian-distributed pseudo-random number
            //   with a standard deviation of SD
            aplFloatRes =  gsl_ran_gaussian (lpMemPTD->gslRNG, SD);

            // Convert the arg to float
            aplFloatRht = (APLFLOAT) aplIntegerRht;

            // Cutoff values to +/-3 standard deviations [-3*SD, 3*SD]
            aplFloatRes = max (aplFloatRes, -3*SD);
            aplFloatRes = min (aplFloatRes,  3*SD);

            // Scale the range to +/-aplFloatRht / 2
            aplFloatRes *= aplFloatRht / (2 * 3 * SD);
#undef  SD
            // Shift the range to [0, aplFloatRht] = [0, aplIntegerRht)
            aplFloatRes += (aplFloatRht / 2);

            // Convert to integer
            return (APLINT) aplFloatRes;

        case L'p':
            // If the rng has not been allocated as yet, ...
            if (lpMemPTD->gslRNG EQ NULL)
                // Allocate storage for a RNG of type
                // The algorithm chosen (minstd) is completely arbitrary
                lpMemPTD->gslRNG = gsl_rng_alloc (gsl_rng_minstd);

            // Convert the range to [0, aplIntegerRht)
            aplIntegerRht--;

            // Convert the arg to float
            aplFloatRht = (APLFLOAT) aplIntegerRht;

            // Generate a Poisson-distributed pseudo-random number
            //   with a mean of aplFloatRht / 2
            aplIntRes = gsl_ran_poisson (lpMemPTD->gslRNG, aplFloatRht / 2);

            return aplIntRes;   // ***FIXME***

            // Truncate the high end at aplIntegerRht
            return min (aplIntRes, aplIntegerRht);

        defstop
            return -1;
    } // End SWITCH
} // End GetRandomNum


//***************************************************************************
//  $PrimFnMonQueryRisR
//
//  Primitive scalar function monadic Query:  R {is} fn R
//***************************************************************************

APLRAT PrimFnMonQueryRisR
    (APLRAT     aplRatRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLBOOL bQuadIO;            // []IO
    APLRAT  mpqRes = {0};       // Result

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Check for DOMAIN ERROR
    if (mpq_cmp_ui (&aplRatRht, bQuadIO, 1) < 0)
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    // If the RAT is an integer,  ...
    if (mpq_integer_p (&aplRatRht))
    {
#ifdef DEBUG
        LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

        // Get ptr to PerTabData global memory
        lpMemPTD = GetMemPTD ();
#else
  #define lpMemPTD  GetMemPTD ()
#endif
        // Split cases based upon the Distribution Type
        switch (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_DT]->stData.stChar)
        {
            case L'r':
                // Initialize the result to 0/1
                mpq_init (&mpqRes);

                // Generate a uniformly-distributed random number in [0, Rht)
                mpz_urandomm (mpq_numref (&mpqRes),
                              lpMemPTD->randState,
                              mpq_numref (&aplRatRht));
                // Add in []IO
                mpz_add_ui (mpq_numref (&mpqRes), mpq_numref (&mpqRes), bQuadIO);

                break;

            case L'g':
            case L'p':
                RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

                break;

            defstop
                break;
        } // End SWITCH
    } else
        // Otherwise, it's an error
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    return mpqRes;
#ifndef DEBUG
  #undef  lpMemPTD
#endif
} // End PrimFnMonQueryRisR


//***************************************************************************
//  $PrimFnMonQueryVisV
//
//  Primitive scalar function monadic Query:  V {is} fn V
//***************************************************************************

APLVFP PrimFnMonQueryVisV
    (APLVFP     aplVfpRht,
     LPPRIMSPEC lpPrimSpec)

{
    APLBOOL      bQuadIO;           // []IO
    APLMPI       mpzRes = {0};      // Result as MPI
    APLVFP       mpfRes = {0};      // Result as VFP

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Check for DOMAIN ERROR
    if (mpfr_cmp_ui (&aplVfpRht, bQuadIO) < 0)
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    // If the VFP is an integer,  ...
    if (mpfr_integer_p (&aplVfpRht))
    {
#ifdef DEBUG
        LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

        // Get ptr to PerTabData global memory
        lpMemPTD = GetMemPTD ();
#else
  #define lpMemPTD  GetMemPTD ()
#endif
        // Split cases based upon the Distribution Type
        switch (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_DT]->stData.stChar)
        {
            case L'r':
                // Initialize the result to R
                mpz_init_set_fr (&mpzRes, &aplVfpRht);

                // Generate a uniformly-distributed random number in [0, Rht)
                mpz_urandomm (&mpzRes,
                              lpMemPTD->randState,
                              &mpzRes);
                // Add in []IO
                mpz_add_ui (&mpzRes, &mpzRes, bQuadIO);

                // Copy to the VFP result
                mpfr_init_set_z (&mpfRes, &mpzRes, MPFR_RNDN);

                // We no longer need this storage
                Myz_clear (&mpzRes);

                break;

            case L'g':
            case L'p':
                RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

                break;

            defstop
                break;
        } // End SWITCH
#ifndef DEBUG
  #undef  lpMemPTD
#endif
    } else
        // Otherwise, it's an error
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

    return mpfRes;
} // End PrimFnMonQueryVisV


//***************************************************************************
//  $PrimFnDydQuery_EM_YY
//
//  Primitive function for dyadic Query ("deal")
//
//  This algorithm was taken from
//    "APLGOL, an Experimental Structured Programming Language" by R. A. Kelley,
//    IBM Journal of Research and Development, January 1973, pp. 69-73.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydQuery_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydQuery_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    APLSTYPE      aplTypeLft,       // Left arg storage type
                  aplTypeRht;       // Right ...
    APLNELM       aplNELMLft,       // Left arg NELM
                  aplNELMRht;       // Right ...
    APLRANK       aplRankLft,       // Left arg rank
                  aplRankRht;       // Right ...
    HGLOBAL       hGlbLft = NULL,   // Left arg global memory handle
                  hGlbRht = NULL,   // Right ...
                  hGlbRes = NULL,   // Result   ...
                  lpSymGlbLft,      // Ptr to left arg as global number
                  lpSymGlbRht;      // ...    right ...
    LPVOID        lpMemLft = NULL,  // Ptr to left arg global memory
                  lpMemRht = NULL;  // Ptr to right ...
    LPAPLINT      lpMemRes = NULL;  // Ptr to result   ...
    APLINT        aplIntegerLft,    // Left arg temporary integer
                  aplIntegerRht;    // Right ...
    APLFLOAT      aplFloatLft,      // Left arg temporary float
                  aplFloatRht;      // Right ...
    APLUINT       ByteRes;          // # bytes in the result
    APLUINT       uQuadRL;          // []RL
    APLINT        uLft,             // Left arg loop counter
                  uRht,             // Right ...
                  uTmp,             // Temporary ...
                  uSub;             // Subarray  ...
    UBOOL         bRet;             // TRUE iff result is valid
    LPPL_YYSTYPE  lpYYRes = NULL;   // Ptr to the result
    APLBOOL       bQuadIO;          // []IO
    LPPERTABDATA  lpMemPTD;         // Ptr to PerTabData global memory
    LPPLLOCALVARS lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;     // Ptr to Ctrl-Break flag

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // If the right arg is a list, ...
    if (IsTknParList (lptkRhtArg))
        return PrimFnSyntaxError_EM (lptkFunc APPEND_NAME_ARG);

    //***************************************************************
    // This function is not sensitive to the axis operator,
    //   so signal a syntax error if present
    //***************************************************************
    if (lptkAxis NE NULL)
        goto AXIS_SYNTAX_EXIT;

    // Get the current value of []IO & []RL
    bQuadIO = GetQuadIO ();
    uQuadRL = GetQuadRL ();

    // Get the attributes (Type, NELM, and Rank) of the left & right args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Get left and right arg's global ptrs
    GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // Check for LEFT/RIGHT RANK ERRORs
    if (IsMultiRank (aplRankLft)
     || IsMultiRank (aplRankRht))
        goto RANK_EXIT;

    // Check for LEFT/RIGHT LENGTH ERRORs
    if (!IsSingleton (aplNELMLft)
     || !IsSingleton (aplNELMRht))
        goto LENGTH_EXIT;

    // Get the respective first values
    GetFirstValueToken (lptkLftArg,     // Ptr to left arg token
                       &aplIntegerLft,  // Ptr to integer result
                       &aplFloatLft,    // Ptr to float ...
                        NULL,           // Ptr to WCHAR ...
                        NULL,           // Ptr to longest ...
                       &lpSymGlbLft,    // Ptr to lpSym/Glb ...
                        NULL,           // Ptr to ...immediate type ...
                        NULL);          // Ptr to array type ...
    GetFirstValueToken (lptkRhtArg,     // Ptr to right arg token
                       &aplIntegerRht,  // Ptr to integer result
                       &aplFloatRht,    // Ptr to float ...
                        NULL,           // Ptr to WCHAR ...
                        NULL,           // Ptr to longest ...
                       &lpSymGlbRht,    // Ptr to lpSym/Glb ...
                        NULL,           // Ptr to ...immediate type ...
                        NULL);          // Ptr to array type ...
    // Check for LEFT/RIGHT DOMAIN ERRORs
    bRet = ((!IsSimpleChar (aplTypeLft))
         && (!IsSimpleChar (aplTypeRht)));
    if (bRet && IsSimpleFlt (aplTypeLft))
        // Attempt to convert the float to an integer using System []CT
        aplIntegerLft = FloatToAplint_SCT (aplFloatLft, &bRet);
    if (bRet && IsSimpleFlt (aplTypeRht))
        // Attempt to convert the float to an integer using System []CT
        aplIntegerRht = FloatToAplint_SCT (aplFloatRht, &bRet);
    if (bRet && IsRat (aplTypeLft))
        // Attempt to convert the RAT to an integer using System []CT
        aplIntegerLft = mpq_get_sctsx ((LPAPLRAT) lpSymGlbLft, &bRet);
    if (bRet && IsRat (aplTypeRht))
        // Attempt to convert the RAT to an integer using System []CT
        aplIntegerRht = mpq_get_sctsx ((LPAPLRAT) lpSymGlbRht, &bRet);
    if (bRet && IsVfp (aplTypeLft))
        // Attempt to convert the VFP to an integer using System []CT
        aplIntegerLft = mpfr_get_sctsx ((LPAPLVFP) lpSymGlbLft, &bRet);
    if (bRet && IsVfp (aplTypeRht))
        // Attempt to convert the VFP to an integer using System []CT
        aplIntegerRht = mpfr_get_sctsx ((LPAPLVFP) lpSymGlbRht, &bRet);

    if (!bRet
     || aplIntegerLft < 0
     || aplIntegerRht < 0
     || aplIntegerLft > aplIntegerRht)
        goto DOMAIN_EXIT;

    // Calculate space needed for the result
    // If we're using the brute force algorithm,
    //   allocate only what is needed
    if (aplIntegerLft < (aplIntegerRht / 16))
        ByteRes = CalcArraySize (ARRAY_INT, aplIntegerLft, 1);
    else
    // Otherwise, allocate space for aplIntegerRht integers,
    //   and downsize it at the end.
        ByteRes = CalcArraySize (ARRAY_INT, aplIntegerRht, 1);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock000 (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_INT;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplIntegerLft;
    lpHeader->Rank       = 1;

    // Mark the result as a permutation vector if the
    //   left and right args are the same.
    if (aplIntegerLft EQ aplIntegerRht)
    {
        lpHeader->PV0 = (bQuadIO EQ 0);
        lpHeader->PV1 = (bQuadIO EQ 1);
    } // End IF
#undef  lpHeader

    // Skip over the header to the dimension
    *VarArrayBaseToDim (lpMemRes) = aplIntegerLft;

    // Skip over the header and dimension to the data
    lpMemRes = VarArrayDataFmBase (lpMemRes);

    // We make roll atomic by not saving any intermediate values
    //   into lpMemPTD->lpSymQuadRL.

    // Split cases based upon the ratio of the left to right arg
    if (aplIntegerLft < (aplIntegerRht / 16))
    {
        // The short case
        for (uLft = 0; uLft < aplIntegerLft; uLft++)
        {
            // Check for Ctrl-Break
            if (CheckCtrlBreak (*lpbCtrlBreak))
                goto ERROR_EXIT;

            // Calculate the next QuadRL
            uQuadRL = NextQuadRL (uQuadRL);

            // Get a random number using the appropriate algorithm
            uTmp = bQuadIO + GetRandomNum (aplIntegerRht, uQuadRL, lpMemPTD);

            // Search for it in the existing set
            for (uSub = 0; uSub < uLft; uSub++)
            if (uTmp EQ lpMemRes[uSub])
                break;

            // If not found, save it in the result
            if (uSub EQ uLft)
                lpMemRes[uLft] = uTmp;
            else
                uLft--;
        } // End FOR
    } else
    {
        // The long case

        // Fill the result with {iota}aplIntegerRht
        for (uRht = 0; uRht < aplIntegerRht; uRht++)    // Z {is} {iota} R
            lpMemRes[uRht] = uRht + bQuadIO;

        // Loop through the elements
        for (uLft = 0; uLft < aplIntegerLft; uLft++)    // :for I :in ({iota} L) - {quad}IO
        {
            // Check for Ctrl-Break
            if (CheckCtrlBreak (*lpbCtrlBreak))
                goto ERROR_EXIT;
                                                        // J {is} I + (?R - I) - {quad}IO
            // Calculate the next QuadRL
            uQuadRL = NextQuadRL (uQuadRL);

            // Get a random number using the appropriate algorithm
            uRht = uLft + GetRandomNum (aplIntegerRht - uLft, uQuadRL, lpMemPTD);

            // Swap the two values
            uTmp           = lpMemRes[uLft];            // Z[{quad}IO + I J] {is} Z[{quad}IO + J I]
            lpMemRes[uLft] = lpMemRes[uRht];
            lpMemRes[uRht] = uTmp;
        } // End IF                                     // :endfor

        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

        // Reallocate the memory down to the value of the left arg
        hGlbRes =
          MyGlobalReAlloc (hGlbRes,
                           MyGlobalSize (hGlbRes) - (APLU3264) (aplIntegerRht - aplIntegerLft) * sizeof (APLINT),
                           GHND);
    } // End IF/ELSE

    // Allocate a new YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // Save the value of uQuadRL as the new []RL
    SetQuadRL (uQuadRL);

    goto NORMAL_EXIT;

AXIS_SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkAxis);
    goto ERROR_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes)
    {
        if (lpMemRes)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
        } // End IF

        // We no longer need this storage
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    if (hGlbRes && lpMemRes)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
    } // End IF

    if (hGlbLft && lpMemLft)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLft); lpMemLft = NULL;
    } // End IF

    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return lpYYRes;
} // End PrimFnDydQuery_EM_YY


//***************************************************************************
//  End of File: pf_query.c
//***************************************************************************
