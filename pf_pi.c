//***************************************************************************
//  NARS2000 -- Primitive Function -- Pi
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
#include "common.h"
#include <ecm.h>


#define INIT_FACTOR_CNT     1000
#define INIT_FACTOR_INC     1000

typedef struct tagB1B2TABLE
{
    double B1,                      // Optimal B1
           B2;                      // Default B2
    int    N;                       // # expected curves
} B1B2TABLE, *LPB1B2TABLE;

B1B2TABLE dB1B2Table[] =            // Optimal values for B1, B2, and N for ECM factoring
                                    //  as found in ECM's README file, Table 1.
//    B1       B2   # curves         # digits in the expected factor
    {{11e3,   1.9e6,    74},        // 20
     { 5e4,   1.3e7,   221},        // 25
     {25e4,   1.3e8,   453},        // 30
     { 1e6,   1.0e9,   984},        // 35
     { 3e6,   5.7e9,  2541},        // 40
     {11e6,  3.5e10,  4949},        // 45
     {43e6,  2.4e11,  8266},        // 50
     {11e7,  7.8e11, 20158},        // 55
     {26e7,  3.2e12, 47173},        // 60
     {85e7,  1.6e13, 77666},        // 65
    };

#define B1B2TAB_MIN     20          // Minimum value for # digits in b1Table
#define B1B2TAB_MAX     65          // Maximum ...
#define B1B2TAB_INC      5          // Increment     ...


//***************************************************************************
//  $PrimFnPi_EM_YY
//
//  Primitive function for monadic and dyadic Pi ("Prime Factors" and "Number Theoretic")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnPi_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnPi_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // Ensure not an overflow function
    Assert (lptkFunc->tkData.tkChar EQ UTF16_PI);

    // Split cases based upon monadic or dyadic
    if (lptkLftArg EQ NULL)
        return PrimFnMonPi_EM_YY (            lptkFunc, lptkRhtArg, lptkAxis);
    else
        return PrimFnDydPi_EM_YY (lptkLftArg, lptkFunc, lptkRhtArg, lptkAxis);
} // End PrimFnPi_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoFnPi_EM_YY
//
//  Generate a prototype for the primitive functions monadic & dyadic Pi
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFnPi_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFnPi_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    // If the left arg is not present, ...
    if (lptkLftArg EQ NULL)
        //***************************************************************
        // Called monadically
        //***************************************************************
        return
          PrimProtoFnScalar_EM_YY (lptkLftArg,      // Ptr to left arg token
                                   lptkFunc,        // Ptr to function token
                                   lptkRhtArg,      // Ptr to right arg token
                                   lptkAxis);       // Ptr to axis token (may be NULL)
    else
        //***************************************************************
        // Called dyadically
        //***************************************************************

        // Convert to a prototype
        return
          PrimProtoFnMixed_EM_YY (&PrimFnPi_EM_YY,  // Ptr to primitive function routine
                                   lptkLftArg,      // Ptr to left arg token
                                   lptkFunc,        // Ptr to function token
                                   lptkRhtArg,      // Ptr to right arg token
                                   lptkAxis);       // Ptr to axis token (may be NULL)
} // End PrimProtoFnPi_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnMonPi_EM_YY
//
//  Primitive function for monadic Pi ("Prime Factors")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnMonPi_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnMonPi_EM_YY
    (LPTOKEN lptkFunc,                  // Ptr to function token
     LPTOKEN lptkRhtArg,                // Ptr to right arg token
     LPTOKEN lptkAxis)                  // Ptr to axis token (may be NULL)

{
    APLSTYPE          aplTypeRht,       // Right arg storage type
                      aplTypeRes;       // Result ...
    APLNELM           aplNELMRht,       // Right arg NELM
                      aplNELMRes = 0;   // Result    ...
    APLRANK           aplRankRht;       // Right arg rank
    HGLOBAL           hGlbRht,          // Right arg global memory handle
                      hGlbRes = NULL;   // Result global memory handle
    LPVOID            lpMemRes = NULL;  // Ptr to result global memory
    APLLONGEST        aplLongestRht;    // Right arg integer value
    APLINT            aplIntegerRht;    // Right arg integer value
    UBOOL             bRet;             // TRUE iff the result is valid
    APLUINT           ByteRes,          // # bytes in the result
                      uRes;             // Loop counter
    LPPL_YYSTYPE      lpYYRes = NULL;   // Ptr to result
    LPPLLOCALVARS     lpplLocalVars;    // Ptr to re-entrant vars
    LPUBOOL           lpbCtrlBreak;     // Ptr to Ctrl-Break flag
    APLMPI            aplMPIRht = {0},  // Right arg as MP integer
                      aplMPIRes = {0};  // Result    ...
    LPVOID            lpMemRht;         // Ptr to right arg global memory
    MEMTMP            memTmp = {0};     // Temporary memory for factors

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the attributes (Type, NELM, Rank)
    //   of the arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check for RIGHT RANK ERROR
    if (IsMultiRank (aplRankRht))
        goto RIGHT_RANK_EXIT;

    // Check for RIGHT LENGTH ERROR
    if (!IsSingleton (aplNELMRht))
        goto RIGHT_LENGTH_EXIT;

    // Get right arg's global ptr
    aplLongestRht = GetGlbPtrs (lptkRhtArg, &hGlbRht);

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            // Save in local var
            mpz_init_set_sx (&aplMPIRht, (APLINT) aplLongestRht);

            // Set the result storage type
            aplTypeRes = ARRAY_INT;

            break;

        case ARRAY_FLOAT:
            // Attempt to convert the float to an integer using System []CT
            aplIntegerRht = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestRht, &bRet);
            if (!bRet)
                goto RIGHT_DOMAIN_EXIT;

            // Save in local var
            mpz_init_set_sx (&aplMPIRht, aplIntegerRht);

            // Set the result storage type
            aplTypeRes = ARRAY_INT;

            break;

        case ARRAY_RAT:
            // Lock the memory to get a ptr to it
            lpMemRht = MyGlobalLockVar (hGlbRht);

            // Skip over the header and dimensions to the data
            lpMemRht = VarArrayDataFmBase (lpMemRht);

            // Save in local var
            mpz_init_set (&aplMPIRht, mpq_numref ((LPAPLRAT) lpMemRht));

            // Is it an integer?
            bRet = mpq_integer_p ((LPAPLRAT) lpMemRht);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbRht); lpMemRht = NULL;

            // If it's not an integer, ...
            if (!bRet)
                goto RIGHT_DOMAIN_EXIT;
            // Set the result storage type
            aplTypeRes = ARRAY_RAT;

            break;

        case ARRAY_VFP:
            // Lock the memory to get a ptr to it
            lpMemRht = MyGlobalLockVar (hGlbRht);

            // Skip over the header and dimensions to the data
            lpMemRht = VarArrayDataFmBase (lpMemRht);

            // Save in local var
            mpz_init_set_fr (&aplMPIRht, (LPAPLVFP) lpMemRht);

            // Is it an integer?
            bRet = mpfr_integer_p ((LPAPLVFP) lpMemRht);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbRht); lpMemRht = NULL;

            // If it's not an integer, ...
            if (!bRet)
                goto RIGHT_DOMAIN_EXIT;
            // Set the result storage type
            aplTypeRes = ARRAY_RAT;

            break;

        default:
            goto RIGHT_DOMAIN_EXIT;
    } // End SWITCH

    // Check the singleton arg
    if (mpz_sgn (&aplMPIRht) <= 0)
        goto RIGHT_DOMAIN_EXIT;

    // Allocate memory for the APLMPI factors
    memTmp.hGlbMem =
      DbgGlobalAlloc (GHND, INIT_FACTOR_CNT * sizeof (APLMPI));

    // Check for error
    if (memTmp.hGlbMem EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    memTmp.lpMemOrg =
    memTmp.lpMemNxt =
      MyGlobalLock000 (memTmp.hGlbMem);

    // Initialize the # allocated entries
    memTmp.uMaxEntry = INIT_FACTOR_CNT;

    // Factor the given number

    // Call common routine
    aplMPIRes =
      PrimFnPiCommon (&memTmp,              // Ptr to factor struc (may be NULL)
                      NUMTHEORY_FACTOR,     // Function index
                     &aplMPIRht,            // Value to factor
                      lpbCtrlBreak,         // Ptr to Ctrl-Break flag
                     &bRet);                // Ptr to TRUE iff the result is valid
    // Check for error
    if (!bRet)
    {
        if (mpz_cmp_si (&aplMPIRes, -1) EQ 0)
            goto RIGHT_NONCE_EXIT;
        else
            goto WSFULL_EXIT;
    } // End IF

    // Get the actual NELM
    aplNELMRes = memTmp.uNumEntry;

    Assert (aplNELMRes EQ mpz_get_sx (&aplMPIRes));

    // Check for Ctrl-Break
    if (CheckCtrlBreak (*lpbCtrlBreak))
        goto ERROR_EXIT;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRes, 1);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result.
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock000 (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Skip over the header to the dimension
    lpMemRes = VarArrayBaseToDim (lpMemRes);

    // Save and skip over the actual dimension
    *((LPAPLDIM) lpMemRes)++ = aplNELMRes;

    // Split cases based upon the result storage type
    switch (aplTypeRes)
    {
        case ARRAY_INT:
            // Copy the MPI data to the result as INT
            for (uRes = 0; uRes < aplNELMRes; uRes++)
                // Get the integer value
                *((LPAPLINT) lpMemRes)++ = mpz_get_sx (&memTmp.lpMemOrg[uRes]);
            break;

        case ARRAY_RAT:
            // Copy the MPI data to the result as RAT
            for (uRes = 0; uRes < aplNELMRes; uRes++)
                // Copy the MPI value as a RAT
                mpq_init_set_z (((LPAPLRAT) lpMemRes)++, &memTmp.lpMemOrg[uRes]);
            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Allocate a new YYRes;
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    goto NORMAL_EXIT;

RIGHT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RIGHT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RIGHT_NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

ERROR_EXIT:
    if (hGlbRes)
    {
        if (lpMemRes)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
        } // End IF

        // We no longer need this resource
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
    // Free the storage in memTmp.lpMemOrg (if any)
    for (uRes = 0; uRes < memTmp.uNumEntry; uRes++)
        Myz_clear (&memTmp.lpMemOrg[uRes]);

    // If we allocated memory for the factors, ...
    if (memTmp.lpMemOrg)
    {
        // We no longer need this ptr
        MyGlobalUnlock (memTmp.lpMemOrg); memTmp.lpMemOrg = memTmp.lpMemNxt = NULL;

        // We no longer need this storage
        DbgGlobalFree (memTmp.hGlbMem); memTmp.hGlbMem = NULL;
    } // End IF

    // We no longer need this storage
    Myz_clear (&aplMPIRes);
    Myz_clear (&aplMPIRht);

    return lpYYRes;
} // End PrimFnMonPi_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnDydPi_EM_YY
//
//  Primitive function for dyadic Pi ("Number Theoretic")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnDydPi_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFnDydPi_EM_YY
    (LPTOKEN lptkLftArg,                // Ptr to left arg token
     LPTOKEN lptkFunc,                  // Ptr to function token
     LPTOKEN lptkRhtArg,                // Ptr to right arg token
     LPTOKEN lptkAxis)                  // Ptr to axis token (may be NULL)

{
    APLSTYPE      aplTypeLft,           // Left arg storage type
                  aplTypeRht,           // Right ...
                  aplTypeRes;           // Result   ...
    APLNELM       aplNELMLft,           // Left arg NELM
                  aplNELMRht,           // Right ...
                  aplNELMRes = 0;       // Result   ...
    APLRANK       aplRankLft,           // Left arg rank
                  aplRankRht;           // Right ...
    HGLOBAL       hGlbLft = NULL,       // Left arg global memory handle
                  hGlbRht = NULL,       // Right ...
                  hGlbRes = NULL;       // Result   ...
    LPVOID        lpMemLft = NULL,      // Ptr to left arg global memory
                  lpMemRht = NULL,      // Ptr to right ...
                  lpMemRes = NULL;      // Ptr to result   ...
    LPAPLDIM      lpMemDimRht;          // Ptr to right arg dimensions
    LPPERTABDATA  lpMemPTD;             // Ptr to PerTabData global memory
    LPPLLOCALVARS lpplLocalVars;        // Ptr to re-entrant vars
    LPUBOOL       lpbCtrlBreak;         // Ptr to Ctrl-Break flag
    APLRAT        mpqRes = {0},         // The result
                  mpqTmp = {0};         // Temporary
    APLLONGEST    aplLongestLft,        // Left arg integer value
                  aplLongestRht;        // Right ...
    NUMTHEORY     fcnType;              // Left arg as an integer
    APLUINT       uCnt,                 // Loop counter
                  ByteRes;              // # bytes in the result
    UBOOL         bRet;                 // TRUE iff the result is valid
    APLMPI        aplMPIRht = {0},      // Right arg as MP integer
                  aplMPIRes = {0};      // Result    ...
    APLBOOL       bQuadIO;              // []IO
    LPPL_YYSTYPE  lpYYRes = NULL;       // Ptr to result
    EXCEPTION_CODES exceptionCode = EXCEPTION_SUCCESS;

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the thread's ptr to local vars
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    // Get the ptr to the Ctrl-Break flag
    lpbCtrlBreak = &lpplLocalVars->bCtrlBreak;

    // Get the attributes (Type, NELM, Rank)
    //   of the args
    AttrsOfToken (lptkLftArg, &aplTypeLft, &aplNELMLft, &aplRankLft, NULL);
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // Check the left arg
    if (!IsSingleton (aplNELMLft))
    {
        // If it's a matrix or higher, ...
        if (IsMultiRank (aplRankLft))
            goto LEFT_RANK_EXIT;
        else
            goto LEFT_LENGTH_EXIT;
    } // End IF

    // Get left/right arg's global ptr
    aplLongestLft = GetGlbPtrs_LOCK (lptkLftArg, &hGlbLft, &lpMemLft);
    aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

    // If it's a global, ...
    if (hGlbRht)
        // Skip over the header to the dimensions
        lpMemDimRht = VarArrayBaseToDim (lpMemRht);

    // If it's a global, ...
    if (hGlbLft)
        // Skip over the header and dimentions to the data
        lpMemLft = VarArrayDataFmBase (lpMemLft);
    else
        lpMemLft = &aplLongestLft;

    // If it's a global, ...
    if (hGlbRht)
        // Skip over the header and dimentions to the data
        lpMemRht = VarArrayDataFmBase (lpMemRht);
    else
        lpMemRht = &aplLongestRht;

    // Split cases based upon the left arg storage type
    switch (aplTypeLft)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            // Get the left arg as an integer
            fcnType = (APLINT) aplLongestLft;

            break;

        case ARRAY_FLOAT:
            // Attempt to convert the float to an integer using System []CT
            fcnType = FloatToAplint_SCT (*(LPAPLFLOAT) &aplLongestLft, &bRet);
            if (!bRet)
                goto LEFT_DOMAIN_EXIT;
            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            goto LEFT_DOMAIN_EXIT;

        case ARRAY_RAT:
            // Get the left arg as an integer
            fcnType = mpq_get_sx ((LPAPLRAT) lpMemLft, &bRet);

            if (!bRet)
                goto LEFT_DOMAIN_EXIT;
            break;

        case ARRAY_VFP:
            // Get the left arg as an integer
            fcnType = mpfr_get_sx ((LPAPLVFP) lpMemLft, &bRet);

            if (!bRet)
                goto LEFT_DOMAIN_EXIT;
            break;

        defstop
            break;
    } // End SWITCH

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
        case ARRAY_FLOAT:
            aplTypeRes = ARRAY_INT;

            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            goto RIGHT_DOMAIN_EXIT;

        case ARRAY_RAT:
        case ARRAY_VFP:
            aplTypeRes = ARRAY_RAT;

            break;

        defstop
            break;
    } // End SWITCH

    // Initialize the temps
    mpz_init (&aplMPIRht);
    mpq_init (&mpqRes);
    mpq_init (&mpqTmp);
RESTART_RAT:
    //***************************************************************
    // Calculate space needed for the result
    //***************************************************************
    ByteRes = CalcArraySize (aplTypeRes, aplNELMRht, aplRankRht);

    //***************************************************************
    // Check for overflow
    //***************************************************************
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    //***************************************************************
    // Now we can allocate the storage for the result
    //***************************************************************
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock000 (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeRes;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRht;
    lpHeader->Rank       = aplRankRht;
#undef  lpHeader

    // Skip over the header to the dimensions
    lpMemRes = VarArrayBaseToDim (lpMemRes);

    // Fill in the result's dimensions
    for (uCnt = 0; uCnt < aplRankRht; uCnt++)
        *((LPAPLDIM) lpMemRes)++ = *lpMemDimRht++;

    // lpMemRes now points to the data

    __try
    {
    // Loop through the elements of the right arg
    for (uCnt = 0; uCnt < aplNELMRht; uCnt++)
    {
        // Split cases based upon the right arg storage type
        switch (aplTypeRht)
        {
            case ARRAY_BOOL:
            case ARRAY_INT:
            case ARRAY_APA:
                // Get the next element as an integer
                mpz_set_sx (&aplMPIRht, GetNextInteger (lpMemRht, aplTypeRht, uCnt));

                break;

            case ARRAY_FLOAT:
                // Convert from FLOAT to RAT
                mpq_set_d (&mpqTmp, ((LPAPLFLOAT) lpMemRht)[uCnt]);

                // Call common code to test for integer tolerance
                if (!PrimFnPiIntegerTolerance (&mpqRes, fcnType, &mpqTmp))
                    goto RIGHT_DOMAIN_EXIT;

                // Copy the integer as an MPI
                mpz_set (&aplMPIRht, mpq_numref (&mpqRes));

                break;

            case ARRAY_RAT:
                // Copy the RAT
                mpq_set   (&mpqTmp, &((LPAPLRAT) lpMemRht)[uCnt]);

                // Call common code to test for integer tolerance
                if (!PrimFnPiIntegerTolerance (&mpqRes, fcnType, &mpqTmp))
                    goto RIGHT_DOMAIN_EXIT;

                // Copy the integer as an MPI
                mpz_set (&aplMPIRht, mpq_numref (&mpqRes));

                break;

            case ARRAY_VFP:
                // Convert from VFP to RAT
                mpq_set_fr (&mpqTmp, &((LPAPLVFP) lpMemRht)[uCnt]);

                // Call common code to test for integer tolerance
                if (!PrimFnPiIntegerTolerance (&mpqRes, fcnType, &mpqTmp))
                    goto RIGHT_DOMAIN_EXIT;

                // Copy the integer as an MPI
                mpz_set (&aplMPIRht, mpq_numref (&mpqRes));

                break;

            defstop
                break;
        } // End SWITCH

        // Split cases based upon the left arg
        switch (fcnType)
        {
            case NUMTHEORY_DIVCNT:          //  0A:  Divisor function (count of divisors of N)
            case NUMTHEORY_DIVSUM:          //  0B:  Divisor function (sum of divisors of N)
            case NUMTHEORY_MOBIUS:          //  0C:  Mobius function
            case NUMTHEORY_TOTIENT:         //  0D:  Totient function (# positive integers coprime to N)
                // If the arg is <= 0, ...
                if (mpz_sgn (&aplMPIRht) <= 0)
                    goto RIGHT_DOMAIN_EXIT;

                // Call common routine
                aplMPIRes =
                  PrimFnPiCommon (NULL,             // Ptr to factor struc (may be NULL)
                                  fcnType,          // Function index
                                 &aplMPIRht,        // Value to factor
                                  lpbCtrlBreak,     // Ptr to Ctrl-Break flag
                                 &bRet);            // Ptr to TRUE iff the result is valid
                // Check for error
                if (!bRet)
                    goto RIGHT_NONCE_EXIT;
                break;

            case NUMTHEORY_ISPRIME:         //  00:  TRUE iff the given # is prime
                // If the arg is <= 0, ...
                if (mpz_sgn (&aplMPIRht) <= 0)
                    // Initialize to 0 (FALSE)
                    mpz_init (&aplMPIRes);
                else
                    aplMPIRes =
                      PrimFnPiIsPrime   (aplMPIRht, lpbCtrlBreak, lpMemPTD);
                break;

            case NUMTHEORY_NEXTPRIME:       //  01:  Next prime after a given #
                // If the arg is <= 0, ...
                if (mpz_sgn (&aplMPIRht) <= 0)
                    // Initialize to 2 (smallest prime)
                    mpz_init_set_ui (&aplMPIRes, 2);
                else
                    aplMPIRes =
                      PrimFnPiNextPrime (aplMPIRht, lpbCtrlBreak, lpMemPTD);
                break;

            case NUMTHEORY_PREVPRIME:       // -01:  Prev prime before a given #
                // If the arg is <= 0, ...
                if (mpz_sgn (&aplMPIRht) <= 0)
                    goto RIGHT_DOMAIN_EXIT;
                else
                    aplMPIRes =
                      PrimFnPiPrevPrime (aplMPIRht, lpbCtrlBreak, lpMemPTD);
                break;

            case NUMTHEORY_NTHPRIME:        // -02:  Nth prime
                // If the arg is < bQuadIO, ...
                if (mpz_cmp_ui (&aplMPIRht, bQuadIO) < 0)
                    goto RIGHT_DOMAIN_EXIT;
                else
                    aplMPIRes =
                      PrimFnPiNthPrime  (aplMPIRht, lpbCtrlBreak, lpMemPTD);
                break;

            case NUMTHEORY_NUMPRIMES:       //  02:  # primes <= a given #
                // If the arg is <= 1, ...
                if (mpz_cmp_ui (&aplMPIRht, 1) <= 0)
                    // Initialize to 0 (FALSE)
                    mpz_init (&aplMPIRes);
                else
                    aplMPIRes =
                      PrimFnPiNumPrimes (aplMPIRht, lpbCtrlBreak, lpMemPTD);
                break;

            default:
                goto LEFT_DOMAIN_EXIT;

                break;
        } // End SWITCH

        // Split cases based upon the result storage type
        switch (aplTypeRes)
        {
            case ARRAY_INT:
                // Save the new value in the result
                if (mpz_fits_sx_p (&aplMPIRes))
                    *((LPAPLINT) lpMemRes)++ = mpz_get_sx (&aplMPIRes);
                else
                {
                    // Promote the result to RAT
                    aplTypeRes = ARRAY_RAT;

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

                    // We no longer need this resource
                    FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;

                    goto RESTART_RAT;
                } // End IF/ELSE

                break;

            case ARRAY_RAT:
                // Save the new value in the result
                mpq_init_set_z (((LPAPLRAT) lpMemRes)++, &aplMPIRes);

                break;

            defstop
                break;
        } // End SWITCH

        // We no longer need this storage
        Myz_clear (&aplMPIRes);
    } // End FOR
    } __except (CheckException (GetExceptionInformation (), L"PrimFnDydPi_EM_YY"))
    {
        dprintfWL9 (L"!!Initiating Exception in " APPEND_NAME L" #1: %2d (%S#%d)", MyGetExceptionCode (), FNLN);

        // Save the exception code
        exceptionCode = MyGetExceptionCode ();

        // Split cases based upon the ExceptionCode
        switch (exceptionCode)
        {
            case EXCEPTION_DOMAIN_ERROR:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                goto RIGHT_DOMAIN_EXIT;

            case EXCEPTION_NONCE_ERROR:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                goto RIGHT_NONCE_EXIT;

            default:
                // Display message for unhandled exception
                DisplayException ();

                break;
        } // End SWITCH
    } // End __try/__except

    // Unlock the result global memory in case TypeDemote actually demotes
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Allocate a new YYRes;
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    // See if it fits into a lower (but not necessarily smaller) datatype
    TypeDemote (&lpYYRes->tkToken);

    goto NORMAL_EXIT;

LEFT_RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

LEFT_LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

LEFT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkLftArg);
    goto ERROR_EXIT;

RIGHT_DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RIGHT_NONCE_EXIT:
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                               lptkRhtArg);
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

        // We no longer need this resource
        FreeResultGlobalIncompleteVar (hGlbRes); hGlbRes = NULL;
    } // End IF
NORMAL_EXIT:
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

    // We no longer need this storage
    Myq_clear (&mpqTmp);
    Myq_clear (&mpqRes);
    Myz_clear (&aplMPIRes);
    Myz_clear (&aplMPIRht);

    return lpYYRes;
} // End PrimFnDydPi_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $PrimFnPiIntegerTolerance
//
//  Determine if a RAT is within SYS_CT of an integer
//***************************************************************************

UBOOL PrimFnPiIntegerTolerance
    (LPAPLRAT  mpqRes,                  // Ptr to result
     NUMTHEORY fcnType,                 // Number theory function type
     LPAPLRAT  mpqTmp)                  // Ptr to incoming value

{
    // Allow Next Prime/Prev Prime/Num Primes right arg to be fractional
    if (fcnType EQ NUMTHEORY_NEXTPRIME
     || fcnType EQ NUMTHEORY_PREVPRIME
     || fcnType EQ NUMTHEORY_NUMPRIMES)
    {
        // Convert the RAT to an integer
        if (fcnType EQ NUMTHEORY_PREVPRIME)
            mpq_ceil  (mpqRes, mpqTmp);
        else
            mpq_floor (mpqRes, mpqTmp);
    } else
    {
        // See if this number is within SYS_CT of a rational integer

        // Get the ceiling
        mpq_ceil  (mpqRes, mpqTmp);

        // Compare against the original value
        if (mpq_cmp_ct (*mpqRes, *mpqTmp, SYS_CT) NE 0)
        {
            // Get the floor
            mpq_floor (mpqRes, mpqTmp);

            // Compare against the original value
            if (mpq_cmp_ct (*mpqRes, *mpqTmp, SYS_CT) NE 0)
                return FALSE;
        } // End IF/ELSE
    } // End IF/ELSE

    return TRUE;
} // End PrimFnPiIntegerTolerance


//***************************************************************************
//  $PrimFnPiCommon
//
//  Common primitive function for monadic/dyadic Pi ("Prime Factors")
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFnPiCommon"
#else
#define APPEND_NAME
#endif

APLMPI PrimFnPiCommon
    (LPMEMTMP     lpMemTmp,         // Ptr to factor struc (may be NULL)
     APLUINT      uFcnIndex,        // Function index (see enum NUMTHEORY)
     LPAPLMPI     mpzRht,           // Ptr to value to factor
     LPUBOOL      lpbCtrlBreak,     // Ptr to Ctrl-Break flag
     LPUBOOL      lpbRet)           // Ptr to TRUE iff the result is valid

{
    APLU3264     uRes,              // Loop counter
                 uPrime;            // The current prime
    UINT         uPrimeCnt;         // # occurrences of a specific prime
    APLNELM      aplNELMRes;        // Result NELM
    PROCESSPRIME procPrime;         // Struct for args to ProcessPrime
    APLMPI       aplTmp = {0},
                 aplPrime = {0};

    // Initialize the function vars
    procPrime.uFcnIndex    = uFcnIndex;
    procPrime.bSquareFree  = TRUE;
    mpz_init_set    (&procPrime.mpzTotient, mpzRht);
    mpz_init_set_ui (&procPrime.mpzDivisor, 1);
    procPrime.lpbCtrlBreak = lpbCtrlBreak;
    get_random_seeds (&procPrime.seed1, &procPrime.seed2);

    // Initialize the result
    *lpbRet = TRUE;

    // Initialize the temps
    mpz_init (&aplTmp);
    mpz_init (&aplPrime);

    // Run through the list of small primes
    // This handles all numbers < PRECOMPUTED_PRIME_NEXT2 = ~1e10 (~33 bits)
    for (aplNELMRes = uPrime = uRes = 0;
         mpz_cmp_ui (mpzRht, 1) NE 0
      && uRes < PRECOMPUTED_NUM_PRIMES;
         uRes++)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Initialize the prime
        uPrime += prime_delta[uRes];
        mpz_set_ui (&aplPrime, uPrime);

        // If it's divisible, ...
        if (mpz_divisible_p (mpzRht, &aplPrime))
        {
            // Remove all occurrences of this prime from mpzRht
            uPrimeCnt = (UINT) mpz_remove (mpzRht, mpzRht, &aplPrime);

            // Count it in
            aplNELMRes += uPrimeCnt;

            // Process this prime factor
            ProcessPrime (aplPrime, uPrimeCnt, &procPrime);

            // If we're saving the values, ...
            if (lpMemTmp)
            while (uPrimeCnt--)
            {
                // Resize the factor struc if needed
                if (!ResizeFactorStruc (lpMemTmp))
                    goto WSFULL_EXIT;

                // Save in the result
                mpz_init_set (lpMemTmp->lpMemNxt++, &aplPrime); lpMemTmp->uNumEntry++;
            } // End WHILE
        } // End IF
    } // End FOR

    // Check against the smallest # not factorable by trial division
    if (mpz_cmp_ui (mpzRht, 1) > 0
     && mpz_cmp_sx (mpzRht, PRECOMPUTED_PRIME_NEXT2) < 0)   // = 100003L*100003L
    {
         // Count it in
         aplNELMRes++;

        // Process this prime factor
        ProcessPrime (*mpzRht, 1, &procPrime);

        // If we're saving the values, ...
        if (lpMemTmp)
        {
            // Resize the factor struc if needed
            if (!ResizeFactorStruc (lpMemTmp))
                goto WSFULL_EXIT;

            // Save in the result
            mpz_init_set (lpMemTmp->lpMemNxt++, mpzRht); lpMemTmp->uNumEntry++;
        } // End IF
    } else
    // Loop until we have no more factors
    while (mpz_cmp_ui (mpzRht, 1) > 0)
    {
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;

        // Try to factor aplIntegerRht using various methods
        Myz_clear (&aplPrime);
        aplPrime = PrimeFactor (*mpzRht, &procPrime, lpbRet);

        // Check for error
        if (!*lpbRet)
            goto NONCE_EXIT;

        // If it's divisible, ...
        if (mpz_divisible_p (mpzRht, &aplPrime))
        {
            // Remove all occurrences of this prime from mpzRht
            uPrimeCnt = (UINT) mpz_remove (mpzRht, mpzRht, &aplPrime);

            // Count it in
            aplNELMRes += uPrimeCnt;

            // Process this prime factor
            ProcessPrime (aplPrime, uPrimeCnt, &procPrime);

            // If we're saving the values, ...
            if (lpMemTmp)
            while (uPrimeCnt--)
            {
                // Resize the factor struc if needed
                if (!ResizeFactorStruc (lpMemTmp))
                    goto WSFULL_EXIT;

                // Save in the result
                mpz_init_set (lpMemTmp->lpMemNxt++, &aplPrime); lpMemTmp->uNumEntry++;
            } // End WHILE
        } // End IF
    } // End WHILE

    // Split cases based upon the function index
    switch (uFcnIndex)
    {
        case NUMTHEORY_FACTOR:
            mpz_set_sx (&aplPrime, aplNELMRes);

            break;

        case NUMTHEORY_DIVCNT:          // 10 {pi} R
        case NUMTHEORY_DIVSUM:          // 11 {pi} R
            // Copy to result var
            mpz_set (&aplPrime, &procPrime.mpzDivisor);

            break;

        case NUMTHEORY_MOBIUS:          // 12 {pi} R
            // If the prime factors are all unique, ...
            if (procPrime.bSquareFree)
                // Return (-1) * (the parity of the # factors)
                mpz_set_sx (&aplPrime, (aplNELMRes & BIT0) ? -1 : 1);
            else
                // The prime factors are not all unique
                mpz_set_ui (&aplPrime, 0);
            break;

        case NUMTHEORY_TOTIENT:         // 13 {pi} R
            // Copy to result var
            mpz_set (&aplPrime, &procPrime.mpzTotient);

            break;

        case NUMTHEORY_ISPRIME:         //  0 {pi} R
        case NUMTHEORY_NEXTPRIME:       //  1 {pi} R
        case NUMTHEORY_PREVPRIME:       // -1 {pi} R
        case NUMTHEORY_NUMPRIMES:       //  2 {pi} R
        case NUMTHEORY_NTHPRIME:        // -2 {pi} R
        defstop
            break;
    } // End SWITCH

    goto NORMAL_EXIT;

NONCE_EXIT:
    mpz_set_sx (&aplPrime, -1);

    goto ERROR_EXIT;

WSFULL_EXIT:
    mpz_set_sx (&aplPrime, -2);

    goto ERROR_EXIT;

ERROR_EXIT:
    // If we're saving the values, ...
    if (lpMemTmp)
    for (uRes = 0; uRes < lpMemTmp->uNumEntry; uRes++)
        Myz_clear (&lpMemTmp->lpMemOrg[uRes]);
NORMAL_EXIT:
    // We no longer need this storage
    Myz_clear (&aplTmp);
    Myz_clear (&procPrime.mpzDivisor);
    Myz_clear (&procPrime.mpzTotient);

    return aplPrime;
} // End PrimFnPiCommon
#undef  APPEND_NAME


//***************************************************************************
//  $ResizeFactorStruc
//
//  Resize the factor memory struc if needed
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ResizeFactorStruc"
#else
#define APPEND_NAME
#endif

UBOOL ResizeFactorStruc
    (LPMEMTMP lpMemTmp)

{
    HGLOBAL  hGlbMem;
    APLU3264 uLen;
    LPAPLMPI lpMemNew;

    // If we're at the current maximum, ...
    if (lpMemTmp->uNumEntry EQ lpMemTmp->uMaxEntry)
    {
        // Unlock so we may reallocate it
        MyGlobalUnlock (lpMemTmp->hGlbMem); lpMemTmp->lpMemOrg = lpMemTmp->lpMemNxt = NULL;

        // Calculate the new length
        uLen = (lpMemTmp->uMaxEntry + INIT_FACTOR_INC) * sizeof (APLMPI);

        // Reallocate up the struc
        //   moving the old data to the new location, and
        //   freeing the old global memory
        hGlbMem =
          MyGlobalReAlloc (lpMemTmp->hGlbMem,
                           uLen,
                           GHND);
        if (hGlbMem)
            // Lock the memory to get a ptr to it
            lpMemNew = MyGlobalLockInt (hGlbMem);
        else
        {
            // Attempt to allocate a new struc
            hGlbMem =
              DbgGlobalAlloc (GHND, uLen);
            if (hGlbMem)
            {
                // Lock the old memory to get a ptr to it
                lpMemTmp->lpMemOrg = MyGlobalLock000 (lpMemTmp->hGlbMem);

                // Lock the new memory to get a ptr to it
                lpMemNew = MyGlobalLock000 (hGlbMem);

                // Copy the factors in the old memory to the new memory
                CopyMemory (lpMemNew,
                            lpMemTmp->lpMemOrg,
                            lpMemTmp->uMaxEntry * sizeof (APLMPI));
                // We no longer need this ptr
                MyGlobalUnlock (lpMemTmp->hGlbMem); lpMemTmp->lpMemOrg = NULL;

                // We no longer need this storage
                DbgGlobalFree (lpMemTmp->hGlbMem); lpMemTmp->hGlbMem = NULL;
            } else
                return FALSE;
        } // End IF/ELSE
    } else
        return TRUE;

    // Save the (possibly new) global memory handle
    lpMemTmp->hGlbMem = hGlbMem;

    // Point to the (possibly new) starting entry
    lpMemTmp->lpMemOrg = lpMemNew;

    // Point to the current entry
    lpMemTmp->lpMemNxt = lpMemTmp->lpMemOrg + lpMemTmp->uNumEntry;

    // Count in the new maximum
    lpMemTmp->uMaxEntry += INIT_FACTOR_INC;

    return TRUE;
} // End ResizeFactorStruc
#undef  APPEND_NAME


//***************************************************************************
//  $PrimeFactor
//
//  Return either a prime factor of a given #
//    or the number itself if it's prime
//***************************************************************************

APLMPI PrimeFactor
    (APLMPI         mpzNumber,          // The number to factor
     LPPROCESSPRIME lpProcPrime,        // Ptr to PROCESSPRIME struc
     LPUBOOL        lpbRet)             // Ptr to TRUE iff the result is valid

{
    APLMPI       mpzFactor1 = {0},      // Factor #1
                 mpzFactor2 = {0},      // ...     2
                 mpzRes     = {0};      // Result
    APLUINT      uVal,                  // Temporary unsigned value
                 digs,                  // # digits in the value
                 bits;                  // # bits in the value
    double       B1,                    // Optimal value for B1 for a given # for ECM factoring
                 B2;                    // Default ...       B2
    int          N,                     // # expected curves for B1, B2, D
                 iCnt,                  // Loop counter
                 iVal;                  // Temporary signed value
    ecm_params   ecmParams;             // ECM parameters
    UINT         uEcmInit = 0;          // # times ecmParams have been initialized
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
#ifdef DEBUG
    char         szTemp1[1024],         // Temporary output save area
                 szTemp2[1024];         // ...
    LPCHAR       szMethods[] = {"ECM", "p-1", "p+1"};
#endif

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the # bits in the incoming number
    bits = mpz_sizeinbase (&mpzNumber, 2);

    // Initialize the result & temps
    mpz_init_set (&mpzRes, &mpzNumber);
    mpz_init     (&mpzFactor1);
    mpz_init     (&mpzFactor2);

    if (bits <= (SMALL_COMPOSITE_CUTOFF_BITS + 15))     // = 85 + 15 = 100
    {
        if (bits <= PRECOMPUTED_PRIME_NEXT2_LOG2)       // = 33
            goto NORMAL_EXIT;
        else
        if (bits <= 60)
        {
            uVal = MySqufof (&mpzRes, lpProcPrime->lpbCtrlBreak);

            // Check for Ctrl-Break
            if (CheckCtrlBreak (*lpProcPrime->lpbCtrlBreak))
                goto BREAK_EXIT;

            switch (uVal)
            {
                case 0:
                case 1:
                    // Try Pollard's Rho
                    if (MyRho (&mpzRes, &mpzFactor1, lpProcPrime))
                    {
                        dprintfWL0 (L"MyRho:     %S", mpz_get_str (szTemp1, 10, &mpzFactor1));

                        Myz_clear (&mpzRes);
                        mpzRes = PrimeFactor (mpzFactor1, lpProcPrime, lpbRet);

                        // Check for error
                        if (!*lpbRet)
                            goto NONCE_EXIT;

                        goto NORMAL_EXIT;
                    } else
                    {
                        uVal = MyTinyqs (&mpzRes, &mpzFactor1, &mpzFactor2, lpProcPrime->lpbCtrlBreak);

                        if (mpz_cmp_ui (&mpzFactor1, 1) EQ 0)
                            uVal = 0;
                    } // End IF

                    if (uVal)
                    {
                        dprintfWL0 (L"MyTinyqs:  %S %S", mpz_get_str (szTemp1, 10, &mpzFactor1), mpz_get_str (szTemp2, 10, &mpzFactor2));
                    } // End IF -- MUST use braces as dprintfWLx is empty for non-DEBUG

                    // Check for Ctrl-Break
                    if (CheckCtrlBreak (*lpProcPrime->lpbCtrlBreak))
                        goto BREAK_EXIT;

                    break;

                default:
                    dprintfWL0 (L"MySqufof:  %I64u", uVal);

                    // If the value is within trial division, ...
                    if (uVal < PRECOMPUTED_PRIME_NEXT2)
                    {
                        // It's a prime:  return it
                        mpz_set_sx (&mpzRes, uVal);

                        goto NORMAL_EXIT;
                    } // End IF

                    // Otherwise, it may be composite:  try to factor it
                    mpz_set_sx (&mpzFactor1, uVal);

                    Myz_clear (&mpzRes);
                    mpzRes = PrimeFactor (mpzFactor1, lpProcPrime, lpbRet);

                    // Check for error
                    if (!*lpbRet)
                        goto NONCE_EXIT;

                    goto NORMAL_EXIT;
            } // End SWITCH
        } else
        {
            uVal = MyTinyqs (&mpzRes, &mpzFactor1, &mpzFactor2, lpProcPrime->lpbCtrlBreak);

            if (mpz_cmp_ui (&mpzFactor1, 1) EQ 0)
                uVal = 0;
            if (uVal)
            {
                dprintfWL0 (L"MyTinyqs:  %S %S", mpz_get_str (szTemp1, 10, &mpzFactor1), mpz_get_str (szTemp2, 10, &mpzFactor2));
            } // End IF -- MUST use braces as dprintfWLx is empty for non-DEBUG
        } // End IF

        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpProcPrime->lpbCtrlBreak))
            goto BREAK_EXIT;

        if (uVal)
        {
            if (IsMpz1 (&mpzFactor1))
                mpz_set (&mpzRes, &mpzFactor2);
            else
            if (IsMpz1 (&mpzFactor2))
                mpz_set (&mpzRes, &mpzFactor1);
            else
            {
                Myz_clear (&mpzRes);

                // Try again with the smaller # so as to try to return factors in ascending order
                mpzRes = PrimeFactor ((mpz_cmp (&mpzFactor1, &mpzFactor2) < 0) ? mpzFactor1 : mpzFactor2, lpProcPrime, lpbRet);

                // Check for error
                if (!*lpbRet)
                    goto NONCE_EXIT;
            } // End IF/ELSE/...

            goto NORMAL_EXIT;
        } // End IF
    } // End IF

    // Izit a prime?
    if (mpz_likely_prime_p (&mpzRes, lpMemPTD->randState, PRECOMPUTED_PRIME_MAX))
    {
        dprintfWL0 (L"prime?:  %S", mpz_get_str (szTemp1, 10, &mpzRes));

        goto NORMAL_EXIT;
    } // End IF

#ifdef MPQS_ENABLED
    if (MyFactor_mpqs (&mpzRes))
        goto NORMAL_EXIT;
#endif

    // Get the # digits in the incoming number
    digs = mpz_sizeinbase (&mpzRes, 10);

    // Divide by 2 to get the # digits in the factor we're looking for
    digs /= 2;

    // Convert # digits to nearest multiple of B1B2TAB_INC
    digs = B1B2TAB_INC * ((digs + B1B2TAB_INC / 2) / B1B2TAB_INC);

    // If the value is below the minimum of B1B2TAB_MIN, raise it to B1B2TAB_MIN
    if (digs < B1B2TAB_MIN)
        digs = B1B2TAB_MIN;
    else
    // If the value is above the maximum of B1TAB_MAX, lower it to B1TAB_MAX
    if (digs > B1B2TAB_MAX)
        digs = B1B2TAB_MAX;

    // Calculate the appropriate values of B1, B2, & N for this number
    B1 = dB1B2Table[(digs - B1B2TAB_MIN) / B1B2TAB_INC].B1;
    B2 = dB1B2Table[(digs - B1B2TAB_MIN) / B1B2TAB_INC].B2;
    N  = dB1B2Table[(digs - B1B2TAB_MIN) / B1B2TAB_INC].N ;
#ifdef DEBUG
#define DPRINTF(dB1,dB2,N)                                                                              \
    {                                                                                                   \
        WCHAR wszTemp[1024];                                                                            \
                                                                                                        \
        swprintf (wszTemp,                                                                              \
                 countof (wszTemp),                                                                     \
                 L"B1 = %.20g, B2 = %.20g, N = %d, Bits = %I64d, D = %I64d",                            \
                 dB1, dB2, N, bits, digs);                                                              \
        DbgMsgW (wszTemp);                                                                              \
    }
#else
#define DPRINTF
#endif

#define TryECM(Method,dB1,dB2,iCnt,N)                                                                           \
    ecmParams->method    = Method;                                                                              \
    mpz_set_d (ecmParams->B2, dB2);                                                                             \
                                                                                                                \
    dprintfWL0 (L"Trying %S %u of %u", szMethods[Method], iCnt, N);                                             \
    DPRINTF (dB1, dB2, N);                                                                                      \
                                                                                                                \
    /* Try ECM */                                                                                               \
    iVal = ecm_factor (&mpzFactor1, &mpzRes, dB1, ecmParams);                                                   \
                                                                                                                \
    /* Check for Ctrl-Break */                                                                                  \
    if (CheckCtrlBreak (*lpProcPrime->lpbCtrlBreak))                                                            \
        goto BREAK_EXIT;                                                                                        \
                                                                                                                \
    /* If there was an error, ... */                                                                            \
    if (ECM_ERROR_P (iVal))                                                                                     \
        goto ERROR_EXIT;                                                                                        \
    /* If it succeeded, ... */                                                                                  \
    if (ECM_FACTOR_FOUND_P (iVal))                                                                              \
    {                                                                                                           \
        /* Is this factor prime? */                                                                             \
        if (mpz_likely_prime_p (&mpzFactor1, lpMemPTD->randState, PRECOMPUTED_PRIME_MAX))                       \
            /* Copy the new factor to the result */                                                             \
            mpz_set (&mpzRes, &mpzFactor1);                                                                     \
        else                                                                                                    \
        {                                                                                                       \
            /* Try again */                                                                                     \
            Myz_clear (&mpzRes);                                                                                \
            mpzRes = PrimeFactor (mpzFactor1, lpProcPrime, lpbRet);                                             \
        } /* End IF/ELSE */                                                                                     \
                                                                                                                \
        goto NORMAL_EXIT;                                                                                       \
    } /* End IF */                                                                                              \
                                                                                                                \
    if (mpz_likely_prime_p (&mpzRes, lpMemPTD->randState, PRECOMPUTED_PRIME_MAX))                               \
    {                                                                                                           \
        dprintfWL0 (L"prime?:  %S", mpz_get_str (szTemp1, 10, &mpzRes));                                        \
                                                                                                                \
        goto NORMAL_EXIT;                                                                                       \
    } /* End IF */

    /* Initialize ecmParams */
    ecm_init (ecmParams); uEcmInit++;
    ecmParams->stop_asap = &StopASAP;
    ecmParams->B1done    = PRECOMPUTED_PRIME_MAX;

    // Try P-1 once with 10*B1 and B2
    TryECM (ECM_PM1, 10*B1, B2, 0, 0);

    // Try P+1 three times with 5*B1 and B2
    TryECM (ECM_PP1,  5*B1, B2, 0, 3);
    TryECM (ECM_PP1,  5*B1, B2, 1, 3);
    TryECM (ECM_PP1,  5*B1, B2, 2, 3);

    // LIMIT N to 2 ***FIXME***
    N = min (N, 2);

    // Try ECM N(B1, B2, D) times
    for (iCnt = 0; iCnt < N; iCnt++)
    {
        TryECM (ECM_ECM, B1, B2, iCnt, N);
    } // End FOR

    /* Uninitialize ecmParams */
    ecm_clear (ecmParams); uEcmInit--;
NONCE_EXIT:
ERROR_EXIT:
    // Mark as a NONCE ERROR
    *lpbRet = FALSE;
BREAK_EXIT:
NORMAL_EXIT:
    // If we initialized 'em, ...
    while (uEcmInit--)
        // Clear the ECM params
        ecm_clear (ecmParams);

    Myz_clear (&mpzFactor1);
    Myz_clear (&mpzFactor2);

    return mpzRes;
} // End PrimeFactor


//***************************************************************************
//  $StopASAP
//
//  Check to see fi we should stop ASAP
//***************************************************************************

int StopASAP
    (void)

{
    LPPLLOCALVARS lpplLocalVars;

    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    return lpplLocalVars->bCtrlBreak;
} // End StopASAP


//***************************************************************************
//  $ProcessPrime
//
//  Process a prime according to the function index
//***************************************************************************

void ProcessPrime
    (APLMPI         aplMPIRht,      // The prime to process
     UINT           uPrimeCnt,      // The prime exponent
     LPPROCESSPRIME lpProcPrime)    // Ptr to PROCESSPRIME struc

{
    APLMPI  mpzTmp = {0};           // Temporary

    // Initialize the temp
    mpz_init (&mpzTmp);

    // Split cases based upon the function index
    switch (lpProcPrime->uFcnIndex)
    {
        case NUMTHEORY_FACTOR:      //    {pi} R
            break;

        case NUMTHEORY_MOBIUS:      // 12 {pi} R
            // Accumulate the square-free state
            lpProcPrime->bSquareFree &= (uPrimeCnt <= 1);

            break;

        case NUMTHEORY_TOTIENT:     // 13 {pi} R
            // If this prime divides N, ...
            if (uPrimeCnt)
            {
////////////////lpProcPrime->uTotient -= lpProcPrime->uTotient / uPrime;
                mpz_divexact (&mpzTmp, &lpProcPrime->mpzTotient, &aplMPIRht);
                mpz_sub      (&lpProcPrime->mpzTotient, &lpProcPrime->mpzTotient, &mpzTmp);
            } // End IF

            break;

        case NUMTHEORY_DIVCNT:      // 10 {pi} R
            // If this prime divides N, ...
            if (uPrimeCnt)
            {
////////////////lpProcPrime->uDivisor *= (uPrimeCnt + 1);
                mpz_set_sx (&mpzTmp, uPrimeCnt + 1);
                mpz_mul (&lpProcPrime->mpzDivisor, &lpProcPrime->mpzDivisor, &mpzTmp);
            } // End IF

            break;

        case NUMTHEORY_DIVSUM:      // 11 {pi} R
            // If this prime divides N, ...
            if (uPrimeCnt)
            {
                APLMPI mpzTmp2 = {0};

                // Initialize the temp to 0
                mpz_init (&mpzTmp2);

                // Tmp2 = p - 1
                mpz_sub_ui (&mpzTmp2, &aplMPIRht, 1);

                // Tmp = exp (p, uPrimeCnt + 1)
                mpz_pow_ui (&mpzTmp, &aplMPIRht, uPrimeCnt + 1);

                // Tmp = Tmp - 1
                mpz_sub_ui (&mpzTmp, &mpzTmp, 1);

                // Tmp = Tmp / (p - 1)
                mpz_div    (&mpzTmp, &mpzTmp, &mpzTmp2);

////////////////lpProcPrime->uDivisor *= uAccum;
                mpz_mul (&lpProcPrime->mpzDivisor, &lpProcPrime->mpzDivisor, &mpzTmp);

                // We no longer need this storage
                Myz_clear (&mpzTmp2);
            } // End IF

            break;

        case NUMTHEORY_NTHPRIME:    // -2 {pi} R
        case NUMTHEORY_PREVPRIME:   // -1 {pi} R
        case NUMTHEORY_ISPRIME:     //  0 {pi} R
        case NUMTHEORY_NEXTPRIME:   //  1 {pi} R
        case NUMTHEORY_NUMPRIMES:   //  2 {pi} R
        defstop
            break;
    } // End SWITCH

    // We no longer need this storage
    Myz_clear (&mpzTmp);
} // End ProcessPrime


//***************************************************************************
//  $PrimFnPiIsPrime        0 {pi} R
//
//  Return TRUE or FALSE for whether or not a given # is a prime
//***************************************************************************

APLMPI PrimFnPiIsPrime
    (APLMPI        aplMPIArg,       // The number to check
     LPUBOOL       lpbCtrlBreak,    // Ptr to Ctrl-Break flag
     LPPERTABDATA  lpMemPTD)        // Ptr to PerTabData global memory

{
    APLMPI mpzRes = {0};            // The result

    // Initialize the result to 0
    mpz_init (&mpzRes);

    if (mpz_cmp_ui (&aplMPIArg, 1) > 0)
        mpz_set_ui (&mpzRes, mpz_likely_prime_p (&aplMPIArg, lpMemPTD->randState, 0));

    return mpzRes;
} // End PrimFnPiIsPrime


//***************************************************************************
//  $PrimFnPiNextPrime      1 {pi} R
//
//  Return the next prime after than a given #
//***************************************************************************

APLMPI PrimFnPiNextPrime
    (APLMPI        aplMPIArg,       // The number to check
     LPUBOOL       lpbCtrlBreak,    // Ptr to Ctrl-Break flag
     LPPERTABDATA  lpMemPTD)        // Ptr to PerTabData global memory

{
    APLMPI mpzRes = {0};            // The result

    // Initialize the result to 0
    mpz_init (&mpzRes);

    // Handle special cases
    if (mpz_cmp_ui (&aplMPIArg, 2) <= 0)
    {
        mpz_set    (&mpzRes, &aplMPIArg);
        mpz_add_ui (&mpzRes, &mpzRes, 1);
    } else
        // Calculate the next likely prime
        mpz_next_prime (&mpzRes, &aplMPIArg, lpbCtrlBreak, lpMemPTD);

    return mpzRes;
} // End PrimFnPiNextPrime


//***************************************************************************
//  $PrimFnPiPrevPrime      -1 {pi} R
//
//  Return the Prev prime before a given #
//***************************************************************************

APLMPI PrimFnPiPrevPrime
    (APLMPI        aplMPIArg,       // The number to check
     LPUBOOL       lpbCtrlBreak,    // Ptr to Ctrl-Break flag
     LPPERTABDATA  lpMemPTD)        // Ptr to PerTabData global memory

{
    APLMPI mpzRes = {0};            // The result

    // Handle special cases
    if (mpz_cmp_ui (&aplMPIArg, 2) <= 0)
    {
        RaiseException (EXCEPTION_DOMAIN_ERROR, 0, 0, NULL);

        return mpzRes;
    } // End IF

    // Initialise the result to 0
    mpz_init (&mpzRes);

    // Handle special cases
    if (mpz_cmp_ui (&aplMPIArg, 3) EQ 0)
        mpz_set_sx (&mpzRes, 2);
    else
        // Calculate the previous likely prime
        mpz_prev_prime (&mpzRes, &aplMPIArg, lpbCtrlBreak, lpMemPTD);

    return mpzRes;
} // End PrimFnPiPrevPrime


//***************************************************************************
//  $NthPrime               -2 {pi} R   subroutine
//
//  Calculate the Nth prime using NthPrimeTab, etc.
//
//  This code was influenced by http://dfns.dyalog.com/c_pco.htm from Roger Hui.
//***************************************************************************

UBOOL NthPrime
    (LPAPLMPI     mpzRes,           // The result
     APLMPI       mpzArg,           // The number to check (origin-0)
     LPUBOOL      lpbCtrlBreak,     // Ptr to Ctrl-Break flag
     LPPERTABDATA lpMemPTD,         // Ptr to PerTabData global memory
     UBOOL        bQuadIO,          // []IO
     TABSTATE     tabState,         // Table to use for the value
     APLU3264     uCnt)             // The index into NthPowerTab/NthPrimeTab depending upon tabState

{
    UBOOL  bRet = FALSE,            // TRUE iff the result is valid or Ctrl-Break pressed
           bDir;                    // TRUE iff we're searching forwards
    APLMPI mpzTmp = {0},            // Temporary MPIs
           mpzDif = {0},            // ...
           mpzLow = {0},            // Lower limit endpoint
           mpzUpp = {0};            // Upper ...

    // Initialize the temps
    mpz_init (&mpzTmp);
    mpz_init (&mpzDif);
    mpz_init (&mpzLow);
    mpz_init (&mpzUpp);

    // Split cases based upon the table state
    switch (tabState)
    {
        case TABSTATE_NTHPOWERTAB:
            // Copy as the lower power of ten
            mpz_ui_pow_ui (&mpzLow, 10, uCnt    );

            // Copy as the upper power of ten
            mpz_ui_pow_ui (&mpzUpp, 10, uCnt + 1);

            break;

        case TABSTATE_NTHPRIMETAB:
            // Copy as the lower multiple of NthPrimeInc entry
            mpz_set_ui (&mpzLow,  uCnt      * NthPrimeInc);

            // Copy as the upper multiple of NthPrimeInc entry
            mpz_set_ui (&mpzUpp, (uCnt + 1) * NthPrimeInc);

            break;

        case TABSTATE_LASTVALUE:
            // Copy as the lower multiple of NthPrimeInc entry
            mpz_set_sx (&mpzLow, NthPrimeStr.aplNthCnt);

////////////// Copy as the upper multiple of NthPrimeInc entry
////////////mpz_set_sx (&mpzUpp, NthPrimeStr.aplNthCnt + NthPrimeInc);

            break;

        defstop
            break;
    } // End SWITCH

    // If we're not using the Last Value, ...
    if (tabState NE TABSTATE_LASTVALUE)
    {
        // Calculate the midpoint of the lower and upper endpoints
        mpz_add    (&mpzDif, &mpzUpp, &mpzLow);
        mpz_div_ui (&mpzDif, &mpzDif, 2);
    } // End IF

    // Split cases based upon the table state
    switch (tabState)
    {
        case TABSTATE_NTHPOWERTAB:
            mpz_set     ( mpzRes, &NthPowerTab[uCnt    ].aplMPI);
            mpz_set     (&mpzTmp, &NthPowerTab[uCnt + 1].aplMPI);

            break;

        case TABSTATE_NTHPRIMETAB:
            mpz_set_sx  ( mpzRes, NthPrimeTab[uCnt    ]);
            mpz_set_sx  (&mpzTmp, NthPrimeTab[uCnt + 1]);

            break;

        case TABSTATE_LASTVALUE:
            mpz_set_sx  ( mpzRes, NthPrimeStr.aplNthVal);
            mpz_set_sx  (&mpzTmp, -1);      // Always at endpoint for LastValue

            break;

        defstop
            break;
    } // End SWITCH

    // Determine whether to search forward from mpzRes,
    //   or backwards from mpzTmp.
    if (mpz_cmp_si (&mpzTmp, -1) NE 0)
        // Calculate the direction (1 = forward, 0 = backward)
        bDir = mpz_cmp (&mpzDif, &mpzArg) >= 0;
    else
        // We're at the endpoint -- go forwards
        bDir = TRUE;

    // If we're searching forwards, ...
    if (bDir)
    {
        // Search forwards
        while (mpz_cmp (&mpzArg, &mpzLow) > 0)
        {
            // Check for Ctrl-Break
            if (CheckCtrlBreak (*lpbCtrlBreak))
                goto ERROR_EXIT;

            // Increment the count
            mpz_add_ui (&mpzLow, &mpzLow, 1);

            // Calculate the next prime
            if (!mpz_next_prime (mpzRes, mpzRes, lpbCtrlBreak, lpMemPTD))
                goto ERROR_EXIT;
        } // End WHILE
    } else
    {
        // Copy the upper endpoint prime
        mpz_set (mpzRes, &mpzTmp);

        // Search backwards
        while (mpz_cmp (&mpzArg, &mpzUpp) < 0)
        {
            // Check for Ctrl-Break
            if (CheckCtrlBreak (*lpbCtrlBreak))
                goto ERROR_EXIT;

            // Decrement the count
            mpz_sub_ui (&mpzUpp, &mpzUpp, 1);

            // Calculate the previous prime
            if (!mpz_prev_prime (mpzRes, mpzRes, lpbCtrlBreak, lpMemPTD))
                goto ERROR_EXIT;
        } // End WHILE
    } // End IF/ELSE

    // Mark as valid result
    bRet = TRUE;

    goto NORMAL_EXIT;

ERROR_EXIT:
    // Mark as Ctrl-Break pressed
    bRet = TRUE;
NORMAL_EXIT:
    // We no longer need this storage
    Myz_clear (&mpzUpp);
    Myz_clear (&mpzLow);
    Myz_clear (&mpzDif);
    Myz_clear (&mpzTmp);

    return bRet;
} // End NthPrime


//***************************************************************************
//  $PrimFnPiNthPrime       -2 {pi} R
//
//  Return the Nth prime
//***************************************************************************

APLMPI PrimFnPiNthPrime
    (APLMPI        aplMPIArg,       // The N in "Nth prime to find"
     LPUBOOL       lpbCtrlBreak,    // Ptr to Ctrl-Break flag
     LPPERTABDATA  lpMemPTD)        // Ptr to PerTabData global memory

{
    APLMPI   mpzRes = {0},          // The result
             mpzArg = {0},          // Temp arg
             mpzInd = {0};          // ...
    APLBOOL  bQuadIO;               // []IO
    APLU3264 uCnt;                  // Loop counter

    // Get the current value of []IO
    bQuadIO = GetQuadIO ();

    // Initialize the result to 0
    mpz_init (&mpzRes);
    mpz_init (&mpzArg);
    mpz_init (&mpzInd);

    // Convert to origin-0
    mpz_sub_ui (&mpzArg, &aplMPIArg, bQuadIO);

    // Calculate the table index
    mpz_div_ui (&mpzInd, &mpzArg, NthPrimeInc);

    // If the argument is within our table range, ...
    if (mpz_cmp_ui (&mpzInd, NthPrimeCnt) < 0)
    {
        // Get the table index
        uCnt = mpz_get_ui (&mpzInd);

        if (NthPrime (&mpzRes, mpzArg, lpbCtrlBreak, lpMemPTD, bQuadIO, TABSTATE_NTHPRIMETAB, uCnt))
            goto NORMAL_EXIT2;
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;
    } // End IF

    // Save the lower and upper limits of the last value
    mpz_set_sx (&mpzRes, NthPrimeStr.aplNthCnt              );
    mpz_set_sx (&mpzInd, NthPrimeStr.aplNthCnt + NthPrimeInc);

    // If the arg is just beyond the last value, ...
    if (NthPrimeStr.aplNthVal NE 0
     && mpz_cmp (&mpzRes, &mpzArg) <= 0
     && mpz_cmp (&mpzArg, &mpzInd) <= 0)
    {
        if (NthPrime (&mpzRes, mpzArg, lpbCtrlBreak, lpMemPTD, bQuadIO, TABSTATE_LASTVALUE, -1))
            goto NORMAL_EXIT;
        // Check for Ctrl-Break
        if (CheckCtrlBreak (*lpbCtrlBreak))
            goto ERROR_EXIT;
    } else
    {
        // Set to last + 1 index in NthPrimeTab
        mpz_set_sx (&mpzRes, NthPrimeCnt * NthPrimeInc);

        // Check to see if the value is beyond our NthPrimeTab
        if (mpz_cmp (&mpzArg, &mpzRes) <= 0)
        {
            // Set the Nth Prime Count & Value
            NthPrimeStr.aplNthCnt = (NthPrimeCnt - 1) * NthPrimeInc;
            NthPrimeStr.aplNthVal = NthPrimeTab[NthPrimeCnt - 1];

            if (NthPrime (&mpzRes, mpzArg, lpbCtrlBreak, lpMemPTD, bQuadIO, TABSTATE_LASTVALUE, -1))
                goto NORMAL_EXIT;
            // Check for Ctrl-Break
            if (CheckCtrlBreak (*lpbCtrlBreak))
                goto ERROR_EXIT;
        } else
        {
            APLVFP mpfRes = {0};

            // Initialize the result
            mpfr_init0 (&mpfRes);
            mpfr_set_z (&mpfRes, &mpzArg, MPFR_RNDN);

            // Calculate the log10 of mpzArg
            mpfr_log10 (&mpfRes, &mpfRes, MPFR_RNDN);
            mpfr_floor (&mpfRes, &mpfRes);

            // Convert the data to an integer
            uCnt = mpfr_get_si (&mpfRes, MPFR_RNDN);

            // We no longer need this storage
            mpfr_clear (&mpfRes);

            // Call common routine
            if (NthPrime (&mpzRes, mpzArg, lpbCtrlBreak, lpMemPTD, bQuadIO, TABSTATE_NTHPOWERTAB, uCnt))
                goto NORMAL_EXIT;
        } // End IF/ELSE
    } // End IF/ELSE

    // We no longer need this storage
    Myz_clear (&mpzRes);

    RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

NORMAL_EXIT:
    // Save as the new last value
    NthPrimeStr.aplNthCnt = mpz_get_sx (&aplMPIArg);
    NthPrimeStr.aplNthVal = mpz_get_sx (&mpzRes);
NORMAL_EXIT2:
ERROR_EXIT:
    // We no longer need this storage
    Myz_clear (&mpzInd);
    Myz_clear (&mpzArg);

    return mpzRes;
} // End PrimFnPiNthPrime


//***************************************************************************
//  $NumPrimes              2 {pi} R    subroutine
//
//  Calculate the # primes <= mpzArg using NthPrimeTab, etc.
//
//  This code was influenced by http://dfns.dyalog.com/c_pco.htm from Roger Hui.
//***************************************************************************

UBOOL NumPrimes
    (LPAPLMPI     mpzRes,           // The result
     APLMPI       mpzArg,           // The number to check
     LPUBOOL      lpbCtrlBreak,     // Ptr to Ctrl-Break flag
     LPPERTABDATA lpMemPTD,         // Ptr to PerTabData global memory
     TABSTATE     tabState,         // Table to use for the value
     APLI3264     iCnt)             // The index into NthPowerTab/NthPrimeTab depending upon tabState

{
    UBOOL  bRet = FALSE,            // TRUE iff the result is valid or Ctrl-Break pressed
           bDir;                    // TRUE iff we're searching forwards
    APLMPI mpzTmp = {0},            // Temporary MPIs
           mpzDif = {0},            // ...
           mpzLow = {0},            // ...
           mpzUpp = {0};            // ...
    APLINT iMid,                    // Loop counter
           iMin,                    // Minimum index
           iMax;                    // Maximum ...

    // Initialize the temps
    mpz_init (&mpzTmp);
    mpz_init (&mpzDif);
    mpz_init (&mpzLow);
    mpz_init (&mpzUpp);

    // Calculate the upper and lower bounds of the search

    // Split cases based upon the table state
    switch (tabState)
    {
        case TABSTATE_NTHPOWERTAB:
            // Copy as the lower NthPowerTab entry
            mpz_set    (&mpzLow, &NthPowerTab[iCnt    ].aplMPI);

            // Copy as the upper NthPowerTab entry
            mpz_set    (&mpzUpp, &NthPowerTab[iCnt + 1].aplMPI);

            break;

        case TABSTATE_NTHPRIMETAB:
            // Copy as the lower NthPrimeTab entry
            mpz_set_sx (&mpzLow, NthPrimeTab[iCnt    ]);

            // Copy as the upper NthPrimeTab entry
            mpz_set_sx (&mpzUpp, NthPrimeTab[iCnt + 1]);

            break;

        case TABSTATE_LASTVALUE:
            // Copy as lower bound
            mpz_set_sx (&mpzLow, NthPrimeStr.aplNthVal);

            // Mark as no upper bound
            mpz_set_sx (&mpzUpp, -1);       // Always endpoint for LastValue

            break;

        defstop
            break;
    } // End SWITCH

    // If we're not using the Last Value, ...
    if (tabState NE TABSTATE_LASTVALUE
     && mpz_cmp_si (&mpzUpp, -1) NE 0)
    {
        Assert (mpz_cmp (&mpzLow, &mpzArg)          <= 0
             && mpz_cmp (         &mpzArg, &mpzUpp) <= 0);

        // Calculate the midpoint of the lower and upper endpoints
        mpz_add    (&mpzDif, &mpzUpp, &mpzLow);
        mpz_div_ui (&mpzDif, &mpzDif, 2);
    } // End IF

    // Split cases based upon the table state
    switch (tabState)
    {
        case TABSTATE_NTHPOWERTAB:
            // Copy as lower power of ten
            mpz_ui_pow_ui (&mpzTmp, 10, iCnt    );
            iMin = mpz_get_sx (&mpzTmp);

            // Copy as the upper power of ten
            mpz_ui_pow_ui (&mpzTmp, 10, iCnt + 1);
            iMax = mpz_get_sx (&mpzTmp);

            break;

        case TABSTATE_NTHPRIMETAB:
            // Set the lower & upper bound offsets
            iMin =  iCnt      * NthPrimeInc;
            iMax = (iCnt + 1) * NthPrimeInc;

            break;

        case TABSTATE_LASTVALUE:
            // Set the lower bound offset
            iMin = NthPrimeStr.aplNthCnt;

            break;

        defstop
            break;
    } // End SWITCH

    // Determine whether to search forwards from the lower bound
    //   or backwards from the upper bound
    if (mpz_cmp_si (&mpzUpp, -1) NE 0)
        // Calculate the direction (1 = forward, 0 = backward)
        bDir = mpz_cmp (&mpzDif, &mpzArg) >= 0;
    else
        // We're at the endpoint -- go forwards
        bDir = TRUE;

    // If the incoming value is not a prime, ...
    if (!mpz_likely_prime_p (&mpzArg, lpMemPTD->randState, 0))
        // Set it to the previous prime so we're always comparing primes
        mpz_prev_prime (&mpzDif, &mpzArg, lpbCtrlBreak, lpMemPTD);
    else
        mpz_set        (&mpzDif, &mpzArg);

    // If we're searching forwards, ...
    if (bDir)
    {
        // Set the lower bound offset
        iMid = iMin;

        // Search forwards
        while (mpz_cmp (&mpzDif, &mpzLow) > 0)
        {
            // Check for Ctrl-Break
            if (CheckCtrlBreak (*lpbCtrlBreak))
                goto ERROR_EXIT;

            // Increment the count
            iMid++;

            // Calculate the next prime
            if (!mpz_next_prime (&mpzLow, &mpzLow, lpbCtrlBreak, lpMemPTD))
                goto ERROR_EXIT;
        } // End WHILE
    } else
    {
        // Set the upper bound offset
        iMid = iMax;

        // Search backwards
        while (mpz_cmp (&mpzDif, &mpzUpp) < 0)
        {
            // Check for Ctrl-Break
            if (CheckCtrlBreak (*lpbCtrlBreak))
                goto ERROR_EXIT;

            // Decrement the count
            iMid--;

            // Calculate the previous prime
            if (!mpz_prev_prime (&mpzUpp, &mpzUpp, lpbCtrlBreak, lpMemPTD))
                goto ERROR_EXIT;
        } // End WHILE
    } // End IF/ELSE

    // Set the result
    mpz_set_sx (mpzRes, iMid + 1);

    // Mark as valid result
    bRet = TRUE;

    goto NORMAL_EXIT;

ERROR_EXIT:
    // Mark as Ctrl-Break pressed
    bRet = TRUE;
NORMAL_EXIT:
    // We no longer need this storage
    Myz_clear (&mpzUpp);
    Myz_clear (&mpzLow);
    Myz_clear (&mpzDif);
    Myz_clear (&mpzTmp);

    return bRet;
} // End NumPrimes


//***************************************************************************
//  $PrimFnPiNumPrimes      2 {pi} R
//
//  Return # primes <= a given #
//***************************************************************************

APLMPI PrimFnPiNumPrimes
    (APLMPI        aplMPIArg,       // The N in "# of primes <= N)
     LPUBOOL       lpbCtrlBreak,    // Ptr to Ctrl-Break flag
     LPPERTABDATA  lpMemPTD)        // Ptr to PerTabData global memory

{
    APLMPI   mpzRes = {0};          // The result
    UBOOL    bRet = FALSE;          // TRUE iff the result is valid
    int      iCmp;                  // Comparison result
    APLI3264 iMid,                  // Loop counter
             iMin,                  // Minimum index
             iMax;                  // Maximum ...
    UINT     uCnt;                  // Loop counter

    // Initialize the result to the last table entry
    mpz_init_set_sx (&mpzRes, NthPrimeTab[NthPrimeCnt - 1]);

    // If the arg is in range of the NthPrimeTab table, ...
    if (mpz_cmp (&aplMPIArg, &mpzRes) <= 0)
    {
        // Initialize the NthPrimeTab minimum and maximum indices
        iMin = 0;
        iMax = NthPrimeCnt - 1;

        // Lookup this value in the NthPrimeTab (binary search)
        while ((!bRet) && iMin <= iMax)
        {
            // Set the current index
            iMid = (iMin + iMax) / 2;

            mpz_set_sx (&mpzRes, NthPrimeTab[iMid]);

            // Check for a match
            switch (signumint (mpz_cmp (&aplMPIArg, &mpzRes)))
            {
                case -1:
                    iMax = iMid - 1;

                    break;

                case  1:
                    iMin = iMid + 1;

                    break;

                case  0:
                    // We found a match
                    bRet = TRUE;

                    break;

                defstop
                    break;
            } // End SWITCH
        } // End WHILE

        // If it's an exact match, ...
        if (bRet)
        {
            // Set the result
            mpz_set_sx (&mpzRes, iMid * NthPrimeInc + 1);

            goto NORMAL_EXIT;
        } else
        {
            // The arg is between NthPrimeTab[iMax] and NthPrimeTab[iMin]
            Assert (iMax EQ (iMin - 1));

            if (NumPrimes (&mpzRes, aplMPIArg, lpbCtrlBreak, lpMemPTD, TABSTATE_NTHPRIMETAB, iMax))
                goto NORMAL_EXIT;
        } // End IF/ELSE
    } else
    {
        // Find the corresponding entry in NthPowerTab
        for (uCnt = 0; uCnt < NthPowerCnt; uCnt++)
        if ((iCmp = mpz_cmp (&aplMPIArg, &NthPowerTab[uCnt].aplMPI)) <= 0)
        {
            // Save the current value
            mpz_set (&mpzRes, &NthPowerTab[uCnt].aplMPI);

            if (NumPrimes (&mpzRes, aplMPIArg, lpbCtrlBreak, lpMemPTD, TABSTATE_NTHPOWERTAB, uCnt - (iCmp NE 0)))
                goto NORMAL_EXIT;
            break;
        } // End FOR/IF

        // Search forwards for the answer
        if (NumPrimes (&mpzRes, aplMPIArg, lpbCtrlBreak, lpMemPTD, TABSTATE_NTHPOWERTAB, uCnt - 2))
            goto NORMAL_EXIT;
    } // End IF/ELSE

    // We no longer need this storage
    Myz_clear (&mpzRes);

    RaiseException (EXCEPTION_NONCE_ERROR, 0, 0, NULL);

NORMAL_EXIT:
    return mpzRes;
} // End PrimFnPiNumPrimes


//***************************************************************************
//  $MyTinyqs
//***************************************************************************

uint32 MyTinyqs
    (LPAPLMPI n,
     LPAPLMPI factor1,
     LPAPLMPI factor2,
     LPUBOOL  lpbCtrlBreak)

{
    mp_t   N,                       // Temporary vars
           Factor1,                 // ...
           Factor2;                 // ...
    uint32 uRes;                    // The result
    size_t nWords;

    // Initialize the mp_t vars
    mp_clear (&N);
    mp_clear (&Factor1);
    mp_clear (&Factor2);

    // Convert the APLMPI args to mp_t args
    mpz_export (&N.val[0],                  // rop:     Output save area
                &nWords,                    // countp:  # words written to output save area
                 -1,                        // order:   Least significant word first
                 sizeof (N.val[0]),         // size:    Size of each word in the output save area
                 -1,                        // endian:  Little-endian
                  0,                        // nails:   # nails
                  n);                       // op:      Ptr to input
    N.nwords = (uint32) nWords;

    mpz_export (&Factor1.val[0],            // rop:     Output save area
                &nWords,                    // countp:  # words written to output save area
                 -1,                        // order:   Least significant word first
                 sizeof (Factor1.val[0]),   // size:    Size of each word in the output save area
                 -1,                        // endian:  Little-endian
                  0,                        // nails:   # nails
                  factor1);                 // op:      Ptr to input
    Factor1.nwords = (uint32) nWords;

    mpz_export (&Factor2.val[0],            // rop:     Output save area
                &nWords,                    // countp:  # words written to output save area
                 -1,                        // order:   Least significant word first
                 sizeof (Factor2.val[0]),   // size:    Size of each word in the output save area
                 -1,                        // endian:  Little-endian
                  0,                        // nails:   # nails
                  factor2);                 // op:      Ptr to input
    Factor2.nwords = (uint32) nWords;

    // Call the original handler
    uRes = tinyqs (&N, &Factor1, &Factor2, lpbCtrlBreak);

    // Convert the mp_t args to APLMPI
    mpz_import (n,                          // rop:     Output save area
                N.nwords,                   // count:   # words in the input data
                -1,                         // order:   Least significant word first
                sizeof (N.val[0]),          // size:    Size of each word in the input
                -1,                         // endian:  Little-endian
                 0,                         // nails:   # nails
                &N.val[0]);                 // op:      Ptr to input
    mpz_import (factor1,                    // rop:     Output save area
                Factor1.nwords,             // count:   # words in the input data
                -1,                         // order:   Least significant word first
                sizeof (Factor1.val[0]),    // size:    Size of each word in the input
                -1,                         // endian:  Little-endian
                 0,                         // nails:   # nails
                &Factor1.val[0]);           // op:      Ptr to input
    mpz_import (factor2,                    // rop:     Output save area
                Factor2.nwords,             // count:   # words in the input data
                -1,                         // order:   Least significant word first
                sizeof (Factor2.val[0]),    // size:    Size of each word in the input
                -1,                         // endian:  Little-endian
                 0,                         // nails:   # nails
                &Factor2.val[0]);           // op:      Ptr to input
    return uRes;
} // End MyTinyqs


//***************************************************************************
//  $MySqufof
//***************************************************************************

uint32 MySqufof
    (LPAPLMPI n,
     LPUBOOL  lpbCtrlBreak)

{
    uint32 uRes;                    // The result
    mp_t   N;                       // Temporary var
    size_t nWords;

    // Initialize the mp_t var
    mp_clear (&N);

    // Convert the APLMPI arg to mp_t arg
    mpz_export (&N.val[0],                  // rop:     Output save area
                &nWords,                    // countp:  # words written to output save area
                 -1,                        // order:   Least significant word first
                 sizeof (N.val[0]),         // size:    Size of each word in the output save area
                 -1,                        // endian:  Little-endian
                  0,                        // nails:   # nails
                  n);                       // op:      Ptr to input
    N.nwords = (uint32) nWords;

    // Call the original handler
    uRes = squfof (&N, lpbCtrlBreak);

    // Convert the mp_t args to APLMPI
    mpz_import (n,                          // rop:     Output save area
                N.nwords,                   // count:   # words in the input data
                -1,                         // order:   Least significant word first
                sizeof (N.val[0]),          // size:    Size of each word in the input
                -1,                         // endian:  Little-endian
                 0,                         // nails:   # nails
                &N.val[0]);                 // op:      Ptr to input
    return uRes;
} // End MySqufof


//***************************************************************************
//  $MyRho
//***************************************************************************

uint32 MyRho
    (LPAPLMPI n,
     LPAPLMPI reduced_n,
     LPPROCESSPRIME lpProcPrime)

{
    uint32 uRes;                    // The result
    mp_t   N,                       // Temporary vars
           Reduced_N;               // ...
    size_t nWords;

    // Initialize the mp_t vars
    mp_clear (&N);
    mp_clear (&Reduced_N);

    // Convert the APLMPI args to mp_t args
    mpz_export (&N.val[0],                  // rop:     Output save area
                &nWords,                    // countp:  # words written to output save area
                 -1,                        // order:   Least significant word first
                 sizeof (N.val[0]),         // size:    Size of each word in the output save area
                 -1,                        // endian:  Little-endian
                  0,                        // nails:   # nails
                  n);                       // op:      Ptr to input
    N.nwords = (uint32) nWords;

    mpz_export (&Reduced_N.val[0],          // rop:     Output save area
                &nWords,                    // countp:  # words written to output save area
                 -1,                        // order:   Least significant word first
                 sizeof (Reduced_N.val[0]), // size:    Size of each word in the output save area
                 -1,                        // endian:  Little-endian
                  0,                        // nails:   # nails
                  reduced_n);               // op:      Ptr to input
    Reduced_N.nwords = (uint32) nWords;

    // Call the original handler
    uRes = rho (&N, &Reduced_N, lpProcPrime);

    // Convert the mp_t args to APLMPI
    mpz_import (n,                          // rop:     Output save area
                N.nwords,                   // count:   # words in the input data
                -1,                         // order:   Least significant word first
                sizeof (N.val[0]),          // size:    Size of each word in the input
                -1,                         // endian:  Little-endian
                 0,                         // nails:   # nails
                &N.val[0]);                 // op:      Ptr to input
    mpz_import (reduced_n,                  // rop:     Output save area
                Reduced_N.nwords,           // count:   # words in the input data
                -1,                         // order:   Least significant word first
                sizeof (Reduced_N.val[0]),  // size:    Size of each word in the input
                -1,                         // endian:  Little-endian
                 0,                         // nails:   # nails
                &Reduced_N.val[0]);         // op:      Ptr to input
    return uRes;
} // End MyRho


#ifdef MPQS_ENABLED
//***************************************************************************
//  $MyFactor_mpqs
//***************************************************************************

UBOOL MyFactor_mpqs
    (LPAPLMPI n)

{
    factor_list_t factor_list;
    msieve_obj    obj;
    mp_t          N;                        // Temporary var
    UBOOL         bRet;                     // TRUE iff the result is valid

    size_t nWords;

    // Initialize the mp_t var
    mp_clear (&N);

    // Convert the APLMPI arg to mp_t arg
    mpz_export (&N.val[0],                  // rop:     Output save area
                &nWords,                    // countp:  # words written to output save area
                 -1,                        // order:   Least significant word first
                 sizeof (N.val[0]),         // size:    Size of each word in the output save area
                 -1,                        // endian:  Little-endian
                  0,                        // nails:   # nails
                  n);                       // op:      Ptr to input
    N.nwords = (uint32) nWords;

    bRet = factor_mpqs (&obj, &N, &factor_list);

    // Convert the mp_t arg to APLMPI
    mpz_import (n,                          // rop:     Output save area
                N.nwords,                   // count:   # words in the input data
                -1,                         // order:   Least significant word first
                sizeof (N.val[0]),          // size:    Size of each word in the input
                -1,                         // endian:  Little-endian
                 0,                         // nails:   # nails
                &N.val[0]);                 // op:      Ptr to input
    return bRet;
} // End MyFactor_mpqs
#endif


//***************************************************************************
//  $get_random_seeds
//***************************************************************************

void get_random_seeds
    (UINT *seed1,
     UINT *seed2)

{
    uint32 tmp_seed1, tmp_seed2;
    uint64 high_res_time;

    /* In a multithreaded program, every msieve object
       should have two unique, non-correlated seeds
       chosen for it */

    /* <Shrug> For everyone else, sample the current time,
       the high-res timer (hopefully not correlated to the
       current time), and the process ID. Multithreaded
       applications should fold in the thread ID too */

    high_res_time = read_clock ();
    tmp_seed1 = ((uint32) (high_res_time >> 32) ^
                 (uint32) time (NULL)) *
                 (uint32) getpid ();
    tmp_seed2 = (uint32) high_res_time;

    /* The final seeds are the result of a multiplicative
       hash of the initial seeds */

    *seed1 = tmp_seed1 * ((uint32)40499 * 65543);
    *seed2 = tmp_seed2 * ((uint32)40499 * 65543);
} // End get_random_seeds


//***************************************************************************
//  $read_clock
//***************************************************************************

uint64 read_clock
    (void)

{
    LARGE_INTEGER ret;
    QueryPerformanceCounter (&ret);
    return ret.QuadPart;
} // End read_clock


//***************************************************************************
//  End of File: pf_pi.c
//***************************************************************************
