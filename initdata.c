//***************************************************************************
//  NARS2000 -- Initialization of Data
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


extern PRIMSPEC PrimSpecBar;
extern PRIMSPEC PrimSpecCircle;
extern PRIMSPEC PrimSpecCircleStar;
extern PRIMSPEC PrimSpecColonBar;
extern PRIMSPEC PrimSpecDownCaret;
extern PRIMSPEC PrimSpecDownCaretTilde;
extern PRIMSPEC PrimSpecDownStile;
extern PRIMSPEC PrimSpecEqual;
extern PRIMSPEC PrimSpecLeftCaret;
extern PRIMSPEC PrimSpecLeftCaretUnderbar;
extern PRIMSPEC PrimSpecNotEqual;
extern PRIMSPEC PrimSpecPlus;
extern PRIMSPEC PrimSpecQuoteDot;
extern PRIMSPEC PrimSpecQuery;
extern PRIMSPEC PrimSpecRightCaret;
extern PRIMSPEC PrimSpecRightCaretUnderbar;
extern PRIMSPEC PrimSpecRoot;
extern PRIMSPEC PrimSpecStar;
extern PRIMSPEC PrimSpecStile;
extern PRIMSPEC PrimSpecTilde;
extern PRIMSPEC PrimSpecTimes;
extern PRIMSPEC PrimSpecUpCaret;
extern PRIMSPEC PrimSpecUpCaretTilde;
extern PRIMSPEC PrimSpecUpStile;


// Monadic Operators TO DO                             Monadic          Dyadic
/////// PrimOpStileTilde_EM_YY                      // ERROR            Partition (*)

// Dyadic  Operators TO DO                             Monadic          Dyadic
/////// PrimOpDieresisDel_EM_YY                     // ERROR            Dual (*)
/////// PrimOpDieresisStar_EM_YY                    // ERROR            Power (*)

// (*) = Unfinished

// First coordinate functions handled by common function
#define PrimFnCircleBar_EM_YY   PrimFnCircleStile_EM_YY
#define PrimFnCommaBar_EM_YY    PrimFnComma_EM_YY
#define PrimFnSlashBar_EM_YY    PrimFnSlash_EM_YY
#define PrimFnSlopeBar_EM_YY    PrimFnSlope_EM_YY
#define PrimOpSlashBar_EM_YY    PrimOpSlash_EM_YY
#define PrimOpSlopeBar_EM_YY    PrimOpSlope_EM_YY

// This var is needed by the GSL routines.
double __infinity;


//***************************************************************************
//  $InitPrimTabs
//
//  Initialize various primitive function, operator,
//    prototype tables, and flags
//***************************************************************************

void InitPrimTabs
    (void)

{
    InitConstants ();           // Initialize various constants
    InitPrimFns ();             // ...        prim fcn/opr jump tables
    InitPrimProtoFns ();        // ...        prim fcn/opr prototype jump table
    InitPrimSpecs ();           // ...        table of PRIMSPECs
    InitPrimFlags ();           // ...        table of primitive fcn/opr flags
    InitIdentityElements ();    // ...        identity elements
    InitPrimeTabs ();           // ...        prime # tables
} // End InitPrimTabs


//***************************************************************************
//  $InitConstants
//
//  Initialize various constants
//***************************************************************************

void InitConstants
    (void)

{
    APLINT aplInteger;          // Temporary value

    // Create various floating point constants
    aplInteger = POS_INFINITY; fltPosInfinity = *(LPAPLFLOAT) &aplInteger;
                                  __infinity  = fltPosInfinity;
    aplInteger = NEG_INFINITY; fltNegInfinity = *(LPAPLFLOAT) &aplInteger;
    aplInteger = FLOAT2POW53;  Float2Pow53    = *(LPAPLFLOAT) &aplInteger;
    aplInteger = FLOATPI;      FloatPi        = *(LPAPLFLOAT) &aplInteger;
    aplInteger = FLOATGAMMA;   FloatGamma     = *(LPAPLFLOAT) &aplInteger;
    aplInteger = FLOATE;       FloatE         = *(LPAPLFLOAT) &aplInteger;

    // Get # ticks per second to be used as a conversion
    //   factor for QueryPerformanceCounter into seconds
    QueryPerformanceFrequency (&liTicksPerSec);
} // End InitConstants


//***************************************************************************
//  $InitGlbNumConstants
//
//  Initialize global numeric constants
//***************************************************************************

void InitGlbNumConstants
    (void)

{
    // Clear the MPFR cache constants
    mpfr_free_cache ();

    // Use our own memory management functions for MPIR/MPFR
    mp_set_memory_functions (mp_alloc, mp_realloc, mp_free);

    // Set the default precision for the following 64-bit VFP constants
    mpfr_set_default_prec (128);

    // Initialize the MPI, RAT, and VFP constants
    mpz_init_set_str  (&mpzMinInt  , "-9223372036854775808", 10);
    mpz_init_set_str  (&mpzMaxInt  ,  "9223372036854775807", 10);
    mpz_init_set_str  (&mpzMaxUInt , "18446744073709551615", 10);
    mpq_init_set_str  (&mpqMinInt  , "-9223372036854775808", 10);
    mpq_init_set_str  (&mpqMaxInt  ,  "9223372036854775807", 10);
    mpq_init_set_str  (&mpqMaxUInt , "18446744073709551615", 10);
    mpq_init_set_ui   (&mpqHalf    , 1, 2);
    mpq_init          (&mpqZero);
    mpfr_init_set_str (&mpfMinInt  , "-9223372036854775808", 10, MPFR_RNDN);
    mpfr_init_set_str (&mpfMaxInt  ,  "9223372036854775807", 10, MPFR_RNDN);
    mpfr_init_set_str (&mpfMaxUInt , "18446744073709551615", 10, MPFR_RNDN);
    mpfr_set_inf      (&mpfPosInfinity                     ,  1);
    mpfr_set_inf      (&mpfNegInfinity                     , -1);
    mpfr_init_set_d   (&mpfHalf    , 0.5                       , MPFR_RNDN);
    mpfr_init0        (&mpfZero);

    // Use our own invalid operation functions for MPIR/MPFR
    mp_set_invalid_functions (mpz_invalid, mpq_invalid, mpfr_invalid);
} // End InitGlbNumConstants


//***************************************************************************
//  $UninitGlbNumConstants
//
//  Uninitialize global numeric constants
//***************************************************************************

void UninitGlbNumConstants
    (void)

{
    // Uninitialize the MPI, RAT, and VFP constants
    Myf_clear (&mpfZero    );
    Myf_clear (&mpfHalf    );
    Myf_clear (&mpfNegInfinity );
    Myf_clear (&mpfPosInfinity );
    Myf_clear (&mpfMaxUInt );
    Myf_clear (&mpfMaxInt  );
    Myf_clear (&mpfMinInt  );
    Myq_clear (&mpqZero    );
    Myq_clear (&mpqHalf    );
    Myq_clear (&mpqMaxUInt );
    Myq_clear (&mpqMaxInt  );
    Myq_clear (&mpqMinInt  );
    Myz_clear (&mpzMaxUInt );
    Myz_clear (&mpzMaxInt  );
    Myz_clear (&mpzMinInt  );
} // End UninitGlbNumConstants


//***************************************************************************
//  $InitPTDVars
//
//  Initialize PerTabData vars
//***************************************************************************

void InitPTDVars
    (LPPERTABDATA lpMemPTD)             // Ptr to PerTabData global memory

{
    // Free these vars unless already free
    Myf_clear        (&lpMemPTD->mpfrPi);
    Myf_clear        (&lpMemPTD->mpfrGamma);
    Myf_clear        (&lpMemPTD->mpfrE);

    // Create a local value for Pi
    mpfr_init0       (&lpMemPTD->mpfrPi);
    mpfr_const_pi    (&lpMemPTD->mpfrPi, MPFR_RNDN);

    // Create a local value for Gamma
    mpfr_init0       (&lpMemPTD->mpfrGamma);
    mpfr_const_euler (&lpMemPTD->mpfrGamma, MPFR_RNDN);

    // Create a local value for e
    mpfr_init_set_ui (&lpMemPTD->mpfrE,                1, MPFR_RNDN);
    mpfr_exp         (&lpMemPTD->mpfrE, &lpMemPTD->mpfrE, MPFR_RNDN);
} // InitPTDVars


//***************************************************************************
//  $InitVfpPrecision
//
//  Initalize the VFP default precision
//***************************************************************************

void InitVfpPrecision
    (APLUINT uDefPrec)          // Default VFP precision

{
    // Set the new default precision
    mpfr_set_default_prec ((mpfr_prec_t) uDefPrec);
} // End InitVfpPrecision


//***************************************************************************
//  $InitPrimFns
//
//  Initialize the primitive function and operator jump table
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- InitPrimFns"
#else
#define APPEND_NAME
#endif

void InitPrimFns
    (void)

{
    //***************************************************************
    //  Primitive Functions
    //  Primitive operators are handled case-by-case in
    //    ExecOp1_EM_YY and ExecOp2_EM_YY.
    //***************************************************************
                                                                                // Alt-'a' - alpha
    InitPrimFn (UTF16_UPTACK              , &PrimFnUpTack_EM_YY             );  // Alt-'b' - up tack
    InitPrimFn (UTF16_UPSHOE              , &PrimFnUpShoe_EM_YY             );  // Alt-'c' - up shoe
    InitPrimFn (UTF16_DOWNSTILE           , &PrimFnDownStile_EM_YY          );  // Alt-'d' - down stile
    InitPrimFn (UTF16_EPSILON             , &PrimFnEpsilon_EM_YY            );  // Alt-'e' - epsilon
////                                                                            // Alt-'f' - infinity
////                                                                            // Alt-'g' - del
////                                                                            // Alt-'h' - delta
    InitPrimFn (UTF16_IOTA                , &PrimFnIota_EM_YY               );  // Alt-'i' - iota
////                                                                            // Alt-'j' - jot (compose)
////                                                                            // Alt-'k' - single quote
////                                                                            // Alt-'l' - quad
////                                                                            // Alt-'m' - down-shoe-stile
    InitPrimFn (UTF16_DOWNTACK            , &PrimFnDownTack_EM_YY           );  // Alt-'n' - down tack
    InitPrimFn (UTF16_CIRCLE              , &PrimFnCircle_EM_YY             );  // Alt-'o' - circle
    InitPrimFn (UTF16_CIRCLE2             , &PrimFnCircle_EM_YY             );  // Circle2
    InitPrimFn (UTF16_PI                  , &PrimFnPi_EM_YY                 );  // Alt-'p' - pi
    InitPrimFn (UTF16_QUERY               , &PrimFnQuery_EM_YY              );  // Alt-'q' - question mark
    InitPrimFn (UTF16_RHO                 , &PrimFnRho_EM_YY                );  // Alt-'r' - rho
    InitPrimFn (UTF16_UPSTILE             , &PrimFnUpStile_EM_YY            );  // Alt-'s' - up stile
    InitPrimFn (UTF16_TILDE               , &PrimFnTilde_EM_YY              );  // Alt-'t' - tilde
    InitPrimFn (UTF16_TILDE2              , &PrimFnTilde_EM_YY              );  // Tilde2'
    InitPrimFn (UTF16_DOWNARROW           , &PrimFnDownArrow_EM_YY          );  // Alt-'u' - down arrow
    InitPrimFn (UTF16_DOWNSHOE            , &PrimFnDownShoe_EM_YY           );  // Alt-'v' - down shoe
                                                                                // Alt-'w' - omega
    InitPrimFn (UTF16_RIGHTSHOE           , &PrimFnRightShoe_EM_YY          );  // Alt-'x' - right shoe
    InitPrimFn (UTF16_UPARROW             , &PrimFnUpArrow_EM_YY            );  // Alt-'y' - up arrow
    InitPrimFn (UTF16_LEFTSHOE            , &PrimFnLeftShoe_EM_YY           );  // Alt-'z' - left shoe
    InitPrimFn (UTF16_EQUALUNDERBAR       , &PrimFnEqualUnderbar_EM_YY      );  // Alt-'!' - match
////                                                                            // Alt-'"' - (none)
    InitPrimFn (UTF16_DELSTILE            , &PrimFnDelStile_EM_YY           );  // Alt-'#' - grade-down
    InitPrimFn (UTF16_DELTASTILE          , &PrimFnDeltaStile_EM_YY         );  // Alt-'$' - grade-up
    InitPrimFn (UTF16_CIRCLESTILE         , &PrimFnCircleStile_EM_YY        );  // Alt-'%' - rotate
    InitPrimFn (UTF16_CIRCLESLOPE         , &PrimFnCircleSlope_EM_YY        );  // Alt-'^' - transpose
    InitPrimFn (UTF16_CIRCLEBAR           , &PrimFnCircleBar_EM_YY          );  // Alt-'&' - circle-bar
    InitPrimFn (UTF16_UPTACKJOT           , &PrimFnUpTackJot_EM_YY          );  // Alt-'\''- execute
    InitPrimFn (UTF16_DOWNCARETTILDE      , &PrimFnDownCaretTilde_EM_YY     );  // Alt-'(' - nor
    InitPrimFn (UTF16_NOR                 , &PrimFnDownCaretTilde_EM_YY     );  // Alt-'(' - nor
    InitPrimFn (UTF16_UPCARETTILDE        , &PrimFnUpCaretTilde_EM_YY       );  // Alt-')' - nand
    InitPrimFn (UTF16_NAND                , &PrimFnUpCaretTilde_EM_YY       );  // Alt-')' - nand
    InitPrimFn (UTF16_CIRCLESTAR          , &PrimFnCircleStar_EM_YY         );  // Alt-'*' - log
    InitPrimFn (UTF16_DOMINO              , &PrimFnDomino_EM_YY             );  // Alt-'+' - domino
////                                                                            // Alt-',' - lamp
    InitPrimFn (UTF16_TIMES               , &PrimFnTimes_EM_YY              );  // Alt-'-' - times
    InitPrimFn (UTF16_SLOPEBAR            , &PrimFnSlopeBar_EM_YY           );  // Alt-'.' - slope-bar
    InitPrimFn (UTF16_SLASHBAR            , &PrimFnSlashBar_EM_YY           );  // Alt-'/' - slash-bar
    InitPrimFn (UTF16_UPCARET             , &PrimFnUpCaret_EM_YY            );  // Alt-'0' - and (94??)
    InitPrimFn (UTF16_CIRCUMFLEX          , &PrimFnUpCaret_EM_YY            );  // '^'
////                                                                            // Alt-'1' - dieresis
////                                                                            // Alt-'2' - overbar
    InitPrimFn (UTF16_LEFTCARET           , &PrimFnLeftCaret_EM_YY          );  // Alt-'3' - less
    InitPrimFn (UTF16_LEFTCARETUNDERBAR   , &PrimFnLeftCaretUnderbar_EM_YY  );  // Alt-'4' - not more
    InitPrimFn (UTF16_LEFTCARETUNDERBAR2  , &PrimFnLeftCaretUnderbar_EM_YY  );  // Not more2
    InitPrimFn (UTF16_EQUAL               , &PrimFnEqual_EM_YY              );  // Alt-'5' - equal
    InitPrimFn (UTF16_RIGHTCARETUNDERBAR  , &PrimFnRightCaretUnderbar_EM_YY );  // Alt-'6' - not less
    InitPrimFn (UTF16_RIGHTCARETUNDERBAR2 , &PrimFnRightCaretUnderbar_EM_YY );  // Not less2
    InitPrimFn (UTF16_RIGHTCARET          , &PrimFnRightCaret_EM_YY         );  // Alt-'7' - more
    InitPrimFn (UTF16_NOTEQUAL            , &PrimFnNotEqual_EM_YY           );  // Alt-'8' - not equal
    InitPrimFn (UTF16_DOWNCARET           , &PrimFnDownCaret_EM_YY          );  // Alt-'9' - or
////                                                                            // Alt-':' - (none)
    InitPrimFn (UTF16_DOWNTACKJOT         , &PrimFnDownTackJot_EM_YY        );  // Alt-';' - format
////                                                                            // Alt-'<' - (none)
    InitPrimFn (UTF16_COLONBAR            , &PrimFnColonBar_EM_YY           );  // Alt-'=' - divide
////                                                                            // Alt-'>' - (none)
////                                                                            // Alt-'?' - circle-middle-dot
    InitPrimFn (UTF16_NOTEQUALUNDERBAR    , &PrimFnNotEqualUnderbar_EM_YY   );  // Alt-'@' - mismatch
////                                                                            // Alt-'A' - (none)
////                                                                            // Alt-'B' - (none)
////                                                                            // Alt-'C' - (none)
////                                                                            // Alt-'D' - (none)
    InitPrimFn (UTF16_EPSILONUNDERBAR     , &PrimFnEpsilonUnderbar_EM_YY    );  // Alt-'E' - epsilon-underbar
////                                                                            // Alt-'F' - (none)
////                                                                            // Alt-'G' - dieresis-del (dual)
////                                                                            // Alt-'H' - delta-underbar
    InitPrimFn (UTF16_IOTAUNDERBAR        , &PrimFnIotaUnderbar_EM_YY       );  // Alt-'I' - iota-underbar
////                                                                            // Alt-'J' - dieresis-jot (rank)
////                                                                            // Alt-'K' - (none)
    InitPrimFn (UTF16_SQUAD               , &PrimFnSquad_EM_YY              );  // Alt-'L' - squad
////                                                                            // Alt-'M' - stile-tilde (partition)
////                                                                            // Alt-'N' - dieresis-downtack (convolution)
////                                                                            // Alt-'O' - dieresis-circle (composition)
////                                                                            // Alt-'P' - dieresis-star (power)
////                                                                            // Alt-'Q' - (none)
    InitPrimFn (UTF16_ROOT                , &PrimFnRoot_EM_YY               );  // Alt-'R' - root
    InitPrimFn (UTF16_SECTION             , &PrimFnSection_EM_YY            );  // Alt-'S' - section (symmetric difference)
////                                                                            // Alt-'T' - dieresis-tilde (commute/duplicate)
////                                                                            // Alt-'U' - (none)
////                                                                            // Alt-'V' - (none)
////                                                                            // Alt-'W' - (none)
    InitPrimFn (UTF16_RIGHTSHOEUNDERBAR   , &PrimFnRightShoeUnderbar_EM_YY  );  // Alt-'X' - right shoe
////                                                                            // Alt-'Y' - (none)
    InitPrimFn (UTF16_LEFTSHOEUNDERBAR    , &PrimFnLeftShoeUnderbar_EM_YY   );  // Alt-'Z' - left shoe underbar
////                                                                            // Alt-'[' - left arrow
    InitPrimFn (UTF16_LEFTTACK            , &PrimFnLeftTack_EM_YY           );  // Alt-'\' - left tack
////                                                                            // Alt-']' - right arrow
////                                                                            // Alt-'_' - variant
////                                                                            // Alt-'`' - diamond
////                                                                            // Alt-'{' - quote-quad
    InitPrimFn (UTF16_RIGHTTACK           , &PrimFnRightTack_EM_YY          );  // Alt-'|' - right tack
////                                                                            // Alt-'}' - zilde
    InitPrimFn (UTF16_COMMABAR            , &PrimFnCommaBar_EM_YY           );  // Alt-'~' - comma-bar
    InitPrimFn (UTF16_BAR                 , &PrimFnBar_EM_YY                );  //     '-'
    InitPrimFn (UTF16_BAR2                , &PrimFnBar_EM_YY                );  //     '-'
    InitPrimFn (UTF16_DOT                 , &PrimFnDotDot_EM_YY             );  //     '.'
    InitPrimFn (UTF16_PLUS                , &PrimFnPlus_EM_YY               );  //     '+'
    InitPrimFn (UTF16_STILE               , &PrimFnStile_EM_YY              );  //     '|'
    InitPrimFn (UTF16_STILE2              , &PrimFnStile_EM_YY              );  //     '|'
    InitPrimFn (UTF16_COMMA               , &PrimFnComma_EM_YY              );  //     ','
    InitPrimFn (UTF16_QUOTEDOT            , &PrimFnQuoteDot_EM_YY           );  //     '!'
    InitPrimFn (UTF16_SLASH               , &PrimFnSlash_EM_YY              );  //     '/'
    InitPrimFn (UTF16_SLOPE               , &PrimFnSlope_EM_YY              );  //     '\'
    InitPrimFn (UTF16_STAR                , &PrimFnStar_EM_YY               );  //     '*'
    InitPrimFn (UTF16_STAR2               , &PrimFnStar_EM_YY               );  //     '*'
////                                                                            //         - dot (inner product)
////                                                                            //         - jotdot (outer product)
} // End InitPrimFns
#undef  APPEND_NAME


//***************************************************************************
//  $InitPrimFn
//
//  Initialize a single primitive function/operator
//***************************************************************************

void InitPrimFn
    (WCHAR     wchFn,
     LPPRIMFNS lpPrimFn)

{
    if (PrimFnsTab[PRIMTAB_MASK & wchFn])
        DbgStop ();         // We should never get here
    else
        PrimFnsTab[PRIMTAB_MASK & wchFn] = lpPrimFn;
} // End InitPrimFn


//***************************************************************************
//  $PrimFn_EM
//
//  Default function for symbols for which there is no corresponding
//    primitive function/operator
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimFn_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimFn_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End PrimFn_EM
#undef  APPEND_NAME

// Monadic operators TO DO
#define PrimProtoOpStileTilde_EM_YY             PrimProtoOp_EM

// Dyadic operators TO DO
#define PrimProtoOpDieresisDel_EM_YY            PrimProtoOp_EM
#define PrimProtoOpDieresisStar_EM_YY           PrimProtoOp_EM


// Primitive scalar functions DONE
#define PrimProtoFnBar_EM_YY                    PrimProtoFnScalar_EM_YY
#define PrimProtoFnCircle_EM_YY                 PrimProtoFnScalar_EM_YY
#define PrimProtoFnCircleStar_EM_YY             PrimProtoFnScalar_EM_YY
#define PrimProtoFnColonBar_EM_YY               PrimProtoFnScalar_EM_YY
#define PrimProtoFnDownCaret_EM_YY              PrimProtoFnScalar_EM_YY
#define PrimProtoFnDownCaretTilde_EM_YY         PrimProtoFnScalar_EM_YY
#define PrimProtoFnDownStile_EM_YY              PrimProtoFnScalar_EM_YY
#define PrimProtoFnEqual_EM_YY                  PrimProtoFnScalar_EM_YY
#define PrimProtoFnLeftCaret_EM_YY              PrimProtoFnScalar_EM_YY
#define PrimProtoFnLeftCaretUnderbar_EM_YY      PrimProtoFnScalar_EM_YY
#define PrimProtoFnNotEqual_EM_YY               PrimProtoFnScalar_EM_YY
#define PrimProtoFnPlus_EM_YY                   PrimProtoFnScalar_EM_YY
#define PrimProtoFnQuoteDot_EM_YY               PrimProtoFnScalar_EM_YY
#define PrimProtoFnRightCaret_EM_YY             PrimProtoFnScalar_EM_YY
#define PrimProtoFnRightCaretUnderbar_EM_YY     PrimProtoFnScalar_EM_YY
#define PrimProtoFnRoot_EM_YY                   PrimProtoFnScalar_EM_YY
#define PrimProtoFnStar_EM_YY                   PrimProtoFnScalar_EM_YY
#define PrimProtoFnStile_EM_YY                  PrimProtoFnScalar_EM_YY
#define PrimProtoFnTimes_EM_YY                  PrimProtoFnScalar_EM_YY
#define PrimProtoFnUpCaret_EM_YY                PrimProtoFnScalar_EM_YY
#define PrimProtoFnUpCaretTilde_EM_YY           PrimProtoFnScalar_EM_YY
#define PrimProtoFnUpStile_EM_YY                PrimProtoFnScalar_EM_YY


// First coordinate functions handled by common function
#define PrimProtoFnCircleBar_EM_YY   PrimProtoFnCircleStile_EM_YY
#define PrimProtoFnCommaBar_EM_YY    PrimProtoFnComma_EM_YY
#define PrimProtoFnSlashBar_EM_YY    PrimProtoFnSlash_EM_YY
#define PrimProtoFnSlopeBar_EM_YY    PrimProtoFnSlope_EM_YY
#define PrimProtoOpSlashBar_EM_YY    PrimProtoOpSlash_EM_YY
#define PrimProtoOpSlopeBar_EM_YY    PrimProtoOpSlope_EM_YY


//***************************************************************************
//  $InitPrimProtoFns
//
//  Initialize the primitive function and operator prototype jump table
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- InitPrimProtoFns"
#else
#define APPEND_NAME
#endif

void InitPrimProtoFns
    (void)

{
    //***************************************************************
    //  Primitive Functions & operators
    //***************************************************************

                                                                                            // Alt-'a' - alpha
    InitPrimProtoFn (UTF16_UPTACK              , &PrimProtoFnUpTack_EM_YY             );    // Alt-'b' - up tack
    InitPrimProtoFn (UTF16_UPSHOE              , &PrimProtoFnUpShoe_EM_YY             );    // Alt-'c' - up shoe
    InitPrimProtoFn (UTF16_DOWNSTILE           , &PrimProtoFnDownStile_EM_YY          );    // Alt-'d' - down stile
    InitPrimProtoFn (UTF16_EPSILON             , &PrimProtoFnEpsilon_EM_YY            );    // Alt-'e' - epsilon
////                                                                                        // Alt-'f' - infinity
////                                                                                        // Alt-'g' - del
////                                                                                        // Alt-'h' - delta
    InitPrimProtoFn (UTF16_IOTA                , &PrimProtoFnIota_EM_YY               );    // Alt-'i' - iota
    InitPrimProtoOp (UTF16_JOT                 , &PrimProtoOpJot_EM_YY                );    // Alt-'j' - jot (compose)
    InitPrimProtoOp (UTF16_JOT2                , &PrimProtoOpJot_EM_YY                );    // Jot2
////                                                                                        // Alt-'k' - single quote
////                                                                                        // Alt-'l' - quad
    InitPrimProtoOp (UTF16_DOWNSHOESTILE       , &PrimProtoOpDownShoeStile_EM_YY      );    // Alt-'m' - down-shoe-stile
    InitPrimProtoFn (UTF16_DOWNTACK            , &PrimProtoFnDownTack_EM_YY           );    // Alt-'n' - down tack
    InitPrimProtoFn (UTF16_CIRCLE              , &PrimProtoFnCircle_EM_YY             );    // Alt-'o' - circle
    InitPrimProtoFn (UTF16_CIRCLE2             , &PrimProtoFnCircle_EM_YY             );    // Circle2
    InitPrimProtoFn (UTF16_PI                  , &PrimProtoFnPi_EM_YY                 );    // Alt-'p' - pi
    InitPrimProtoFn (UTF16_QUERY               , &PrimProtoFnQuery_EM_YY              );    // Alt-'q' - question mark
    InitPrimProtoFn (UTF16_RHO                 , &PrimProtoFnRho_EM_YY                );    // Alt-'r' - rho
    InitPrimProtoFn (UTF16_UPSTILE             , &PrimProtoFnUpStile_EM_YY            );    // Alt-'s' - up stile
    InitPrimProtoFn (UTF16_TILDE               , &PrimProtoFnTilde_EM_YY              );    // Alt-'t' - tilde
    InitPrimProtoFn (UTF16_TILDE2              , &PrimProtoFnTilde_EM_YY              );    // Tilde2
    InitPrimProtoFn (UTF16_DOWNARROW           , &PrimProtoFnDownArrow_EM_YY          );    // Alt-'u' - down arrow
    InitPrimProtoFn (UTF16_DOWNSHOE            , &PrimProtoFnDownShoe_EM_YY           );    // Alt-'v' - down shoe
                                                                                            // Alt-'w' - omega
    InitPrimProtoFn (UTF16_RIGHTSHOE           , &PrimProtoFnRightShoe_EM_YY          );    // Alt-'x' - right shoe
    InitPrimProtoFn (UTF16_UPARROW             , &PrimProtoFnUpArrow_EM_YY            );    // Alt-'y' - up arrow
    InitPrimProtoFn (UTF16_LEFTSHOE            , &PrimProtoFnLeftShoe_EM_YY           );    // Alt-'z' - left shoe
    InitPrimProtoFn (UTF16_EQUALUNDERBAR       , &PrimProtoFnEqualUnderbar_EM_YY      );    // Alt-'!' - match
////                                                                                        // Alt-'"' - (none)
    InitPrimProtoFn (UTF16_DELSTILE            , &PrimProtoFnDelStile_EM_YY           );    // Alt-'#' - grade-down
    InitPrimProtoFn (UTF16_DELTASTILE          , &PrimProtoFnDeltaStile_EM_YY         );    // Alt-'$' - grade-up
    InitPrimProtoFn (UTF16_CIRCLESTILE         , &PrimProtoFnCircleStile_EM_YY        );    // Alt-'%' - rotate
    InitPrimProtoFn (UTF16_CIRCLESLOPE         , &PrimProtoFnCircleSlope_EM_YY        );    // Alt-'^' - transpose
    InitPrimProtoFn (UTF16_CIRCLEBAR           , &PrimProtoFnCircleBar_EM_YY          );    // Alt-'&' - circle-bar
    InitPrimProtoFn (UTF16_UPTACKJOT           , &PrimProtoFnUpTackJot_EM_YY          );    // Alt-'\''- execute
    InitPrimProtoFn (UTF16_DOWNCARETTILDE      , &PrimProtoFnDownCaretTilde_EM_YY     );    // Alt-'(' - nor
    InitPrimProtoFn (UTF16_NOR                 , &PrimProtoFnDownCaretTilde_EM_YY     );    // Alt-'(' - nor
    InitPrimProtoFn (UTF16_UPCARETTILDE        , &PrimProtoFnUpCaretTilde_EM_YY       );    // Alt-')' - nand
    InitPrimProtoFn (UTF16_NAND                , &PrimProtoFnUpCaretTilde_EM_YY       );    // Alt-')' - nand
    InitPrimProtoFn (UTF16_CIRCLESTAR          , &PrimProtoFnCircleStar_EM_YY         );    // Alt-'*' - log
    InitPrimProtoFn (UTF16_DOMINO              , &PrimProtoFnDomino_EM_YY             );    // Alt-'+' - domino
////                                                                                        // Alt-',' - lamp
    InitPrimProtoFn (UTF16_TIMES               , &PrimProtoFnTimes_EM_YY              );    // Alt-'-' - times
    InitPrimProtoFn (UTF16_SLOPEBAR            , &PrimProtoFnSlopeBar_EM_YY           );    // Alt-'.' - slope-bar as Function
    InitPrimProtoOp (INDEX_OPSLOPEBAR          , &PrimProtoOpSlopeBar_EM_YY           );    // Alt-'.' - ...          Operator
    InitPrimProtoFn (UTF16_SLASHBAR            , &PrimProtoFnSlashBar_EM_YY           );    // Alt-'/' - slash-bar as Function
    InitPrimProtoOp (INDEX_OPSLASHBAR          , &PrimProtoOpSlashBar_EM_YY           );    // Alt-'/' - ...          Operator
    InitPrimProtoFn (UTF16_UPCARET             , &PrimProtoFnUpCaret_EM_YY            );    // Alt-'0' - and (94??)
    InitPrimProtoFn (UTF16_CIRCUMFLEX          , &PrimProtoFnUpCaret_EM_YY            );    //         -
    InitPrimProtoOp (UTF16_DIERESIS            , &PrimProtoOpDieresis_EM_YY           );    // Alt-'1' - dieresis
////                                                                                        // Alt-'2' - overbar
    InitPrimProtoFn (UTF16_LEFTCARET           , &PrimProtoFnLeftCaret_EM_YY          );    // Alt-'3' - less
    InitPrimProtoFn (UTF16_LEFTCARETUNDERBAR   , &PrimProtoFnLeftCaretUnderbar_EM_YY  );    // Alt-'4' - not more
    InitPrimProtoFn (UTF16_LEFTCARETUNDERBAR2  , &PrimProtoFnLeftCaretUnderbar_EM_YY  );    // Not more2
    InitPrimProtoFn (UTF16_EQUAL               , &PrimProtoFnEqual_EM_YY              );    // Alt-'5' - equal
    InitPrimProtoFn (UTF16_RIGHTCARETUNDERBAR  , &PrimProtoFnRightCaretUnderbar_EM_YY );    // Alt-'6' - not less
    InitPrimProtoFn (UTF16_RIGHTCARETUNDERBAR2 , &PrimProtoFnRightCaretUnderbar_EM_YY );    // Not less2
    InitPrimProtoFn (UTF16_RIGHTCARET          , &PrimProtoFnRightCaret_EM_YY         );    // Alt-'7' - more
    InitPrimProtoFn (UTF16_NOTEQUAL            , &PrimProtoFnNotEqual_EM_YY           );    // Alt-'8' - not equal
    InitPrimProtoFn (UTF16_DOWNCARET           , &PrimProtoFnDownCaret_EM_YY          );    // Alt-'9' - or
////                                                                                        // Alt-':' - (none)
    InitPrimProtoFn (UTF16_DOWNTACKJOT         , &PrimProtoFnDownTackJot_EM_YY        );    // Alt-';' - format
////                                                                                        // Alt-'<' - (none)
    InitPrimProtoFn (UTF16_COLONBAR            , &PrimProtoFnColonBar_EM_YY           );    // Alt-'=' - divide
////                                                                                        // Alt-'>' - (none)
    InitPrimProtoOp (UTF16_CIRCLEMIDDLEDOT     , &PrimProtoOpCircleMiddleDot_EM_YY    );    // Alt-'?' - circle-middle-dot
    InitPrimProtoFn (UTF16_NOTEQUALUNDERBAR    , &PrimProtoFnNotEqualUnderbar_EM_YY   );    // Alt-'@' - mismatch
////                                                                                        // Alt-'A' - (none)
////                                                                                        // Alt-'B' - (none)
////                                                                                        // Alt-'C' - (none)
////                                                                                        // Alt-'D' - (none)
    InitPrimProtoFn (UTF16_EPSILONUNDERBAR     , &PrimProtoFnEpsilonUnderbar_EM_YY    );    // Alt-'E' - epsilon-underbar
////                                                                                        // Alt-'F' - (none)
    InitPrimProtoOp (UTF16_DIERESISDEL         , &PrimProtoOpDieresisDel_EM_YY        );    // Alt-'G' - dieresis-del (dual)
////                                                                                        // Alt-'H' - delta-underbar
    InitPrimProtoFn (UTF16_IOTAUNDERBAR        , &PrimProtoFnIotaUnderbar_EM_YY       );    // Alt-'I' - iota-underbar
    InitPrimProtoOp (UTF16_DIERESISJOT         , &PrimProtoOpDieresisJot_EM_YY        );    // Alt-'J' - dieresis-jot (rank)
////                                                                                        // Alt-'K' - (none)
    InitPrimProtoFn (UTF16_SQUAD               , &PrimProtoFnSquad_EM_YY              );    // Alt-'L' - squad
    InitPrimProtoOp (UTF16_STILETILDE          , &PrimProtoOpStileTilde_EM_YY         );    // Alt-'M' - stile-tilde (partition)
    InitPrimProtoOp (UTF16_DIERESISDOWNTACK    , &PrimProtoOpDieresisDownTack_EM_YY   );    // Alt-'N' - dieresis-downtack (convolution)
    InitPrimProtoOp (UTF16_DIERESISCIRCLE      , &PrimProtoOpDieresisCircle_EM_YY     );    // Alt-'O' - dieresis-circle (composition)
    InitPrimProtoOp (UTF16_DIERESISSTAR        , &PrimProtoOpDieresisStar_EM_YY       );    // Alt-'P' - dieresis-star (power)
////                                                                                        // Alt-'Q' - (none)
    InitPrimProtoFn (UTF16_ROOT                , &PrimProtoFnRoot_EM_YY               );    // Alt-'R' - root
    InitPrimProtoFn (UTF16_SECTION             , &PrimProtoFnSection_EM_YY            );    // Alt-'S' - section (multiset symmetric difference)
    InitPrimProtoOp (UTF16_DIERESISTILDE       , &PrimProtoOpDieresisTilde_EM_YY      );    // Alt-'T' - dieresis-tilde (commute/duplicate)
////                                                                                        // Alt-'U' - (none)
////                                                                                        // Alt-'V' - (none)
////                                                                                        // Alt-'W' - (none)
    InitPrimProtoFn (UTF16_RIGHTSHOEUNDERBAR   , &PrimProtoFnRightShoeUnderbar_EM_YY  );    // Alt-'X' - right shoe underbar
////                                                                                        // Alt-'Y' - (none)
    InitPrimProtoFn (UTF16_LEFTSHOEUNDERBAR    , &PrimProtoFnLeftShoeUnderbar_EM_YY   );    // Alt-'Z' - left shoe underbar
////                                                                                        // Alt-'[' - left arrow
    InitPrimProtoFn (UTF16_LEFTTACK            , &PrimProtoFnLeftTack_EM_YY           );    // Alt-'\' - left tack
////                                                                                        // Alt-']' - right arrow
    InitPrimProtoOp (UTF16_VARIANT             , &PrimProtoOpVariant_EM_YY            );    // Alt-'_' - variant
////                                                                                        // Alt-'`' - diamond
////                                                                                        // Alt-'{' - quote-quad
    InitPrimProtoFn (UTF16_RIGHTTACK           , &PrimProtoFnRightTack_EM_YY          );    // Alt-'|' - right tack
////                                                                                        // Alt-'}' - zilde
    InitPrimProtoFn (UTF16_COMMABAR            , &PrimProtoFnCommaBar_EM_YY           );    // Alt-'~' - comma-bar
    InitPrimProtoFn (UTF16_BAR                 , &PrimProtoFnBar_EM_YY                );    //         -
    InitPrimProtoFn (UTF16_BAR2                , &PrimProtoFnBar_EM_YY                );    //         -
    InitPrimProtoFn (UTF16_PLUS                , &PrimProtoFnPlus_EM_YY               );    //         -
    InitPrimProtoFn (UTF16_DOT                 , &PrimProtoFnDotDot_EM_YY             );    //         -
    InitPrimProtoFn (UTF16_STILE               , &PrimProtoFnStile_EM_YY              );    //         -
    InitPrimProtoFn (UTF16_STILE2              , &PrimProtoFnStile_EM_YY              );    //         -
    InitPrimProtoFn (UTF16_COMMA               , &PrimProtoFnComma_EM_YY              );    //         -
    InitPrimProtoFn (UTF16_QUOTEDOT            , &PrimProtoFnQuoteDot_EM_YY           );    //         -
    InitPrimProtoFn (UTF16_STAR                , &PrimProtoFnStar_EM_YY               );    //         -
    InitPrimProtoFn (UTF16_STAR2               , &PrimProtoFnStar_EM_YY               );    //         -
    InitPrimProtoFn (UTF16_SLASH               , &PrimProtoFnSlash_EM_YY              );    //         - slash as Function
    InitPrimProtoOp (INDEX_OPSLASH             , &PrimProtoOpSlash_EM_YY              );    //         - ...      Operator
    InitPrimProtoFn (UTF16_SLOPE               , &PrimProtoFnSlope_EM_YY              );    //         - slope as Function
    InitPrimProtoOp (INDEX_OPSLOPE             , &PrimProtoOpSlope_EM_YY              );    //         - ...      Operator
    InitPrimProtoOp (UTF16_DOT                 , &PrimProtoOpDot_EM_YY                );    //         - dot (inner product)
    InitPrimProtoOp (INDEX_JOTDOT              , &PrimProtoOpJotDot_EM_YY             );    //         - jotdot (outer product)
} // End InitPrimProtoFns
#undef  APPEND_NAME


//***************************************************************************
//  $InitPrimProtoFn
//
//  Initialize a single primitive function prototype
//***************************************************************************

void InitPrimProtoFn
    (WCHAR     wchFn,
     LPPRIMFNS lpPrimFn)

{
    if (PrimProtoFnsTab[PRIMTAB_MASK & wchFn])
        DbgStop ();         // We should never get here
    else
        PrimProtoFnsTab[PRIMTAB_MASK & wchFn] = lpPrimFn;
} // End InitPrimProtoFn


//***************************************************************************
//  $InitPrimProtoOp
//
//  Initialize a single primitive operator prototype
//***************************************************************************

void InitPrimProtoOp
    (WCHAR     wchFn,
     LPPRIMOPS lpPrimOp)

{
    if (PrimProtoOpsTab[PRIMTAB_MASK & wchFn])
        DbgStop ();         // We should never get here
    else
        PrimProtoOpsTab[PRIMTAB_MASK & wchFn] = lpPrimOp;
} // End InitPrimProtoOp


//***************************************************************************
//  $PrimProtoFn_EM
//
//  Default function for symbols for which there is no corresponding
//    primitive function/operator prototype
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoFn_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoFn_EM
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (may be NULL if monadic)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    return PrimFnNonceError_EM (lptkFunc APPEND_NAME_ARG);
} // End PrimProtoFn_EM
#undef  APPEND_NAME


//***************************************************************************
//  $PrimProtoOp_EM
//
//  Default function for symbols for which there is no corresponding
//    primitive operator prototype
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- PrimProtoOp_EM"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE PrimProtoOp_EM
    (LPTOKEN      lptkLftArg,           // Ptr to left arg token (may be NULL if monadic)
     LPPL_YYSTYPE lpYYFcnStrOpr,        // Ptr to function token
     LPTOKEN      lptkRhtArg,           // Ptr to right arg token
     LPTOKEN      lptkAxis)             // Ptr to axis token (always NULL)

{
    ErrorMessageIndirectToken (ERRMSG_NONCE_ERROR APPEND_NAME,
                              &lpYYFcnStrOpr->tkToken);
    return NULL;
} // End PrimProtoOp_EM
#undef  APPEND_NAME


//***************************************************************************
//  $InitPrimSpecs
//
//  Initialize the table of PRIMSPECs
//***************************************************************************

void InitPrimSpecs
    (void)

{
    // Initialize the table of PRIMSPECs
    Init1PrimSpec (UTF16_BAR                 , &PrimSpecBar                );
    Init1PrimSpec (UTF16_BAR2                , &PrimSpecBar                );
    Init1PrimSpec (UTF16_CIRCLE              , &PrimSpecCircle             );
    Init1PrimSpec (UTF16_CIRCLE2             , &PrimSpecCircle             );
    Init1PrimSpec (UTF16_CIRCLESTAR          , &PrimSpecCircleStar         );
    Init1PrimSpec (UTF16_COLONBAR            , &PrimSpecColonBar           );
    Init1PrimSpec (UTF16_DOWNCARET           , &PrimSpecDownCaret          );
    Init1PrimSpec (UTF16_DOWNCARETTILDE      , &PrimSpecDownCaretTilde     );
    Init1PrimSpec (UTF16_DOWNSTILE           , &PrimSpecDownStile          );
    Init1PrimSpec (UTF16_EQUAL               , &PrimSpecEqual              );
    Init1PrimSpec (UTF16_LEFTCARET           , &PrimSpecLeftCaret          );
    Init1PrimSpec (UTF16_LEFTCARETUNDERBAR   , &PrimSpecLeftCaretUnderbar  );
    Init1PrimSpec (UTF16_LEFTCARETUNDERBAR2  , &PrimSpecLeftCaretUnderbar  );
    Init1PrimSpec (UTF16_NAND                , &PrimSpecUpCaretTilde       );
    Init1PrimSpec (UTF16_NOR                 , &PrimSpecDownCaretTilde     );
    Init1PrimSpec (UTF16_NOTEQUAL            , &PrimSpecNotEqual           );
    Init1PrimSpec (UTF16_PLUS                , &PrimSpecPlus               );
    Init1PrimSpec (UTF16_QUOTEDOT            , &PrimSpecQuoteDot           );
    Init1PrimSpec (UTF16_QUERY               , &PrimSpecQuery              );
    Init1PrimSpec (UTF16_RIGHTCARET          , &PrimSpecRightCaret         );
    Init1PrimSpec (UTF16_RIGHTCARETUNDERBAR  , &PrimSpecRightCaretUnderbar );
    Init1PrimSpec (UTF16_RIGHTCARETUNDERBAR2 , &PrimSpecRightCaretUnderbar );
    Init1PrimSpec (UTF16_ROOT                , &PrimSpecRoot               );
    Init1PrimSpec (UTF16_STAR                , &PrimSpecStar               );
    Init1PrimSpec (UTF16_STAR2               , &PrimSpecStar               );
    Init1PrimSpec (UTF16_STILE               , &PrimSpecStile              );
    Init1PrimSpec (UTF16_STILE2              , &PrimSpecStile              );
    Init1PrimSpec (UTF16_TILDE               , &PrimSpecTilde              );
    Init1PrimSpec (UTF16_TILDE2              , &PrimSpecTilde              );
    Init1PrimSpec (UTF16_TIMES               , &PrimSpecTimes              );
    Init1PrimSpec (UTF16_UPCARET             , &PrimSpecUpCaret            );
    Init1PrimSpec (UTF16_CIRCUMFLEX          , &PrimSpecUpCaret            );
    Init1PrimSpec (UTF16_UPCARETTILDE        , &PrimSpecUpCaretTilde       );
    Init1PrimSpec (UTF16_UPSTILE             , &PrimSpecUpStile            );
} // End InitPrimSpecs


//***************************************************************************
//  $Init1PrimSpec
//
//  Initialize a single lpPrimSpec
//***************************************************************************

void Init1PrimSpec
    (WCHAR      wchFn,
     LPPRIMSPEC lpPrimSpec)

{
    if (PrimSpecTab[PRIMTAB_MASK & wchFn])
        DbgStop ();         // We should never get here
    else
        PrimSpecTab[PRIMTAB_MASK & wchFn] = lpPrimSpec;
} // End Init1PrimSpec


//***************************************************************************
//  $InitPrimFlags
//
//  Initialize the table of primitive function/operator flags
//***************************************************************************

void InitPrimFlags
    (void)

{
    UINT      uCnt;                 // Loop counter
    PRIMFLAGS PrimFlag,             // Temporary save area
              PrimFlags0 = {0};     // All zero PrimFlags for global R/O use
    APLU3264  PF_FB,                // Fast Boolean reduction and/or scan
              PF_AN,                // Function is associative on all numbers
              PF_AB,                // ...         associative on Booleans only
              PF_AL,                // ...         alternating
              PF_MS,                // ...         monadic scalar
              PF_DS,                // ...         dyadic  ...
              PF_ID,                // ...      has an identity element
              PF_LI,                // ...          a left identity element
              PF_RI;                // ...            right ...

    // Initialize the array
    for (uCnt = 0; uCnt < countof (PrimFlags); uCnt++)
        PrimFlags[uCnt] = PrimFlags0;

    // Get the bits as masks
    *((APLU3264 *) &PrimFlag) = 0; PrimFlag.FastBool  = TRUE; PF_FB = *(APLU3264 *) &PrimFlag;
    *((APLU3264 *) &PrimFlag) = 0; PrimFlag.AssocBool = TRUE; PF_AB = *(APLU3264 *) &PrimFlag;
    *((APLU3264 *) &PrimFlag) = 0; PrimFlag.AssocNumb = TRUE; PF_AN = *(APLU3264 *) &PrimFlag;
    *((APLU3264 *) &PrimFlag) = 0; PrimFlag.Alter     = TRUE; PF_AL = *(APLU3264 *) &PrimFlag;
    *((APLU3264 *) &PrimFlag) = 0; PrimFlag.MonScalar = TRUE; PF_MS = *(APLU3264 *) &PrimFlag;
    *((APLU3264 *) &PrimFlag) = 0; PrimFlag.DydScalar = TRUE; PF_DS = *(APLU3264 *) &PrimFlag;
    *((APLU3264 *) &PrimFlag) = 0; PrimFlag.IdentElem = TRUE; PF_ID = *(APLU3264 *) &PrimFlag;
    *((APLU3264 *) &PrimFlag) = 0; PrimFlag.bLftIdent = TRUE; PF_LI = *(APLU3264 *) &PrimFlag;
    *((APLU3264 *) &PrimFlag) = 0; PrimFlag.bRhtIdent = TRUE; PF_RI = *(APLU3264 *) &PrimFlag;

    Init1PrimFlag (UTF16_BAR                 , 0                     | PF_AL | PF_MS | PF_DS | PF_ID |         PF_RI | PF_INDEX_MINUS     , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_BAR2                , 0                     | PF_AL | PF_MS | PF_DS | PF_ID |         PF_RI | PF_INDEX_MINUS     , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_CIRCLE              , 0                             | PF_MS | PF_DS                                              , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_CIRCLE2             , 0                             | PF_MS | PF_DS                                              , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_CIRCLEBAR           , 0                                             | PF_ID | PF_LI                              , (LPPRIMOPS) PrimIdentFnCircleStile_EM_YY      );
    Init1PrimFlag (UTF16_CIRCLEMIDDLEDOT     , 0                                                                                          ,             NULL ); // PrimIdentOpCircleMiddleDot_EM_YY  );
    Init1PrimFlag (UTF16_CIRCLESLOPE         , 0                                             | PF_ID | PF_LI                              , (LPPRIMOPS) PrimIdentFnCircleSlope_EM_YY      );
    Init1PrimFlag (UTF16_CIRCLESTILE         , 0                                             | PF_ID | PF_LI                              , (LPPRIMOPS) PrimIdentFnCircleStile_EM_YY      );
    Init1PrimFlag (UTF16_CIRCLESTAR          , 0                             | PF_MS | PF_DS                                              , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_COLONBAR            , 0                     | PF_AL | PF_MS | PF_DS | PF_ID         | PF_RI | PF_INDEX_DIVIDE    , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_COMMA               , 0                                             | PF_ID | PF_LI | PF_RI                      , (LPPRIMOPS) PrimIdentFnComma_EM_YY            );
    Init1PrimFlag (UTF16_COMMABAR            , 0                                             | PF_ID | PF_LI | PF_RI                      , (LPPRIMOPS) PrimIdentFnComma_EM_YY            );
    Init1PrimFlag (UTF16_DIERESIS            , 0                                                                                          ,             PrimIdentOpDieresis_EM_YY         );
    Init1PrimFlag (UTF16_DIERESISCIRCLE      , 0                                                                                          ,             NULL ); // PrimIdentOpDieresisCircle_EM_YY   );
    Init1PrimFlag (UTF16_DIERESISDEL         , 0                                                                                          ,             NULL ); // PrimIdentOpDieresisDel_EM_YY      );
    Init1PrimFlag (UTF16_DIERESISDOWNTACK    , 0                                             | PF_ID | PF_LI | PF_RI                      , (LPPRIMOPS) PrimIdentOpDieresisDownTack_EM_YY );
    Init1PrimFlag (UTF16_DIERESISJOT         , 0                                                                                          ,             NULL ); // PrimIdentOpDieresisJot_EM_YY      );
    Init1PrimFlag (UTF16_DIERESISSTAR        , 0                                                                                          ,             NULL ); // PrimIdentOpDieresisStar_EM_YY     );
    Init1PrimFlag (UTF16_DIERESISTILDE       , 0                                                                                          ,             PrimIdentOpDieresisTilde_EM_YY    );
    Init1PrimFlag (UTF16_DOMINO              , 0                                             | PF_ID         | PF_RI                      , (LPPRIMOPS) PrimIdentFnDomino_EM_YY           );
    Init1PrimFlag (UTF16_DOT                 , 0                                                                                          ,             PrimIdentOpDot_EM_YY              );
    Init1PrimFlag (UTF16_DOWNARROW           , 0                                             | PF_ID | PF_LI                              , (LPPRIMOPS) PrimIdentFnDownArrow_EM_YY        );
    Init1PrimFlag (UTF16_DOWNCARET           , PF_FB | PF_AB                 | PF_MS | PF_DS | PF_ID | PF_LI | PF_RI | PF_INDEX_OR        , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_DOWNCARETTILDE      , PF_FB                         | PF_MS | PF_DS                         | PF_INDEX_NOR       , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_DOWNSHOE            , 0                                             | PF_ID | PF_LI | PF_RI                      , (LPPRIMOPS) PrimIdentFnDownShoe_EM_YY         );
    Init1PrimFlag (UTF16_DOWNSHOESTILE       , 0                                                                                          ,             NULL ); // PrimIdentFnDownShoeStile_EM_YY    );
    Init1PrimFlag (UTF16_DOWNSTILE           , PF_FB | PF_AB | PF_AN         | PF_MS | PF_DS | PF_ID | PF_LI | PF_RI | PF_INDEX_MIN       , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_DOWNTACK            , 0                                             | PF_ID | PF_LI                              , (LPPRIMOPS) PrimIdentFnDownTack_EM_YY         );
    Init1PrimFlag (UTF16_EQUAL               , PF_FB | PF_AB                 | PF_MS | PF_DS | PF_ID | PF_LI | PF_RI | PF_INDEX_EQUAL     , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (INDEX_JOTDOT              , 0                                                                                          ,             PrimIdentOpJotDot_EM_YY           );
    Init1PrimFlag (UTF16_LEFTCARET           , PF_FB                         | PF_MS | PF_DS | PF_ID | PF_LI         | PF_INDEX_LESS      , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_LEFTCARETUNDERBAR   , PF_FB                         | PF_MS | PF_DS | PF_ID | PF_LI         | PF_INDEX_LESSEQ    , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_LEFTCARETUNDERBAR2  , PF_FB                         | PF_MS | PF_DS | PF_ID | PF_LI         | PF_INDEX_LESSEQ    , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_NAND                , PF_FB                         | PF_MS | PF_DS                         | PF_INDEX_NAND      , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_NOR                 , PF_FB                         | PF_MS | PF_DS                         | PF_INDEX_NOR       , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_NOTEQUAL            , PF_FB | PF_AB                 | PF_MS | PF_DS | PF_ID | PF_LI | PF_RI | PF_INDEX_NOTEQUAL  , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_PLUS                , 0     | PF_AB | PF_AN         | PF_MS | PF_DS | PF_ID | PF_LI | PF_RI | PF_INDEX_PLUS      , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_QUOTEDOT            , PF_FB                         | PF_MS | PF_DS | PF_ID | PF_LI         | PF_INDEX_LESSEQ    , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_QUERY               , 0                             | PF_MS                                                      , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_RHO                 , 0                                             | PF_ID | PF_LI                              , (LPPRIMOPS) PrimIdentFnRho_EM_YY              );
    Init1PrimFlag (UTF16_RIGHTCARET          , PF_FB                         | PF_MS | PF_DS | PF_ID         | PF_RI | PF_INDEX_MORE      , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_RIGHTCARETUNDERBAR  , PF_FB                         | PF_MS | PF_DS | PF_ID         | PF_RI | PF_INDEX_MOREEQ    , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_RIGHTCARETUNDERBAR2 , PF_FB                         | PF_MS | PF_DS | PF_ID         | PF_RI | PF_INDEX_MOREEQ    , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_RIGHTSHOE           , 0                                             | PF_ID | PF_LI                              , (LPPRIMOPS) PrimIdentFnRightShoe_EM_YY        );
    Init1PrimFlag (UTF16_ROOT                , 0                             | PF_MS | PF_DS | PF_ID | PF_LI         | PF_INDEX_ROOT      , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_SECTION             , 0                                             | PF_ID         | PF_RI                      , (LPPRIMOPS) PrimIdentFnSection_EM_YY          );
    Init1PrimFlag (UTF16_SLASH               , 0                                                                                          ,             NULL ); // PrimIdentOpSlash_EM_YY            );
    Init1PrimFlag (UTF16_SLASHBAR            , 0                                                                                          ,             NULL ); // PrimIdentOpSlash_EM_YY            );
    Init1PrimFlag (UTF16_SLOPE               , 0                                                                                          ,             NULL ); // PrimIdentOpSlope_EM_YY            );
    Init1PrimFlag (UTF16_SLOPEBAR            , 0                                                                                          ,             NULL ); // PrimIdentOpSlope_EM_YY            );
    Init1PrimFlag (UTF16_SQUAD               , 0                                             | PF_ID | PF_LI                              , (LPPRIMOPS) PrimIdentFnSquad_EM_YY            );
    Init1PrimFlag (UTF16_STAR                , PF_FB                         | PF_MS | PF_DS | PF_ID         | PF_RI | PF_INDEX_MOREEQ    , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_STAR2               , PF_FB                         | PF_MS | PF_DS | PF_ID         | PF_RI | PF_INDEX_MOREEQ    , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_STILE               , PF_FB                         | PF_MS | PF_DS | PF_ID | PF_LI         | PF_INDEX_LESS      , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_STILE2              , PF_FB                         | PF_MS | PF_DS | PF_ID | PF_LI         | PF_INDEX_LESS      , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_STILETILDE          , 0                                                                                          ,             NULL ); // PrimIdentOpStileTilde_EM_YY       );
    Init1PrimFlag (UTF16_TILDE               , 0                             | PF_MS         | PF_ID         | PF_RI                      , (LPPRIMOPS) PrimIdentFnTilde_EM_YY            );
    Init1PrimFlag (UTF16_TILDE2              , 0                             | PF_MS         | PF_ID         | PF_RI                      , (LPPRIMOPS) PrimIdentFnTilde_EM_YY            );
    Init1PrimFlag (UTF16_TIMES               , PF_FB | PF_AB | PF_AN         | PF_MS | PF_DS | PF_ID | PF_LI | PF_RI | PF_INDEX_AND       , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_UPARROW             , 0                                             | PF_ID | PF_LI                              , (LPPRIMOPS) PrimIdentFnUpArrow_EM_YY          );
    Init1PrimFlag (UTF16_UPCARET             , PF_FB | PF_AB                 | PF_MS | PF_DS | PF_ID | PF_LI | PF_RI | PF_INDEX_AND       , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_CIRCUMFLEX          , PF_FB | PF_AB                 | PF_MS | PF_DS | PF_ID | PF_LI | PF_RI | PF_INDEX_AND       , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_UPCARETTILDE        , PF_FB                         | PF_MS | PF_DS                         | PF_INDEX_NAND      , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_UPSTILE             , PF_FB | PF_AB | PF_AN         | PF_MS | PF_DS | PF_ID | PF_LI | PF_RI | PF_INDEX_MAX       , (LPPRIMOPS) PrimIdentFnScalar_EM_YY           );
    Init1PrimFlag (UTF16_VARIANT             , 0                                                                                          ,             PrimIdentOpVariant_EM_YY          );
} // End InitPrimFlags


//***************************************************************************
//  $Init1PrimFlag
//
//  Initialize a single primitive flag
//***************************************************************************

void Init1PrimFlag
    (WCHAR     wchFn,
     APLU3264  uFlag,
     LPPRIMOPS lpPrimOps)

{
    *((APLU3264 *) &PrimFlags[PRIMTAB_MASK & wchFn]) |= uFlag;
                    PrimFlags[PRIMTAB_MASK & wchFn].lpPrimOps = lpPrimOps;
} // End Init1PrimFlag


//***************************************************************************
//  $InitIdentityElements
//
//  Initialize the table of primitive scalar function identity elements
//***************************************************************************

void InitIdentityElements
    (void)

{
    Init1IdentityElement (PF_INDEX_PLUS    , 0.0);
    Init1IdentityElement (PF_INDEX_MINUS   , 0.0);
    Init1IdentityElement (PF_INDEX_DIVIDE  , 1.0);

    Init1IdentityElement (PF_INDEX_MIN     , fltPosInfinity);
    Init1IdentityElement (PF_INDEX_MAX     , fltNegInfinity);

    Init1IdentityElement (PF_INDEX_AND     , 1.0);
    Init1IdentityElement (PF_INDEX_OR      , 0.0);

    Init1IdentityElement (PF_INDEX_LESS    , 0.0);
    Init1IdentityElement (PF_INDEX_LESSEQ  , 1.0);
    Init1IdentityElement (PF_INDEX_EQUAL   , 1.0);
    Init1IdentityElement (PF_INDEX_MOREEQ  , 1.0);
    Init1IdentityElement (PF_INDEX_MORE    , 0.0);

    Init1IdentityElement (PF_INDEX_ROOT    , 1.0);

    Init1IdentityElement (PF_INDEX_NOTEQUAL, 0.0);
} // End InitIdentityElements


//***************************************************************************
//  $Init1IdentityElement
//
//  Initialize an identity element
//***************************************************************************

void Init1IdentityElement
    (UINT     uIndex,
     APLFLOAT aplFloat)

{
    // If the identity element is Boolean, ...
    PrimIdent[uIndex].IsBool     = (aplFloat EQ 0.0) || (aplFloat EQ 1.0);
    PrimIdent[uIndex].bIdentElem = (aplFloat EQ 1.0);

    // Save if it's float
    PrimIdent[uIndex].fIdentElem = aplFloat;
} // End Init1IdentityElement


//***************************************************************************
//  $InitPrimeTabs
//
//  Initialize prime # tables
//***************************************************************************

void InitPrimeTabs
    (void)

{
    UINT uCnt;

    // Convert the chars in NthPowerCharTab to
    //   APLMPIs in NthPowerTab
    for (uCnt = 0; uCnt < NthPowerCnt; uCnt++)
        mpz_init_set_str (&NthPowerTab[uCnt].aplMPI, NthPowerTab[uCnt].lpChar, 10);

    // Initialize the NthPrimeStr values to the last entry in the NthPrimeTab
    NthPrimeStr.aplNthVal = NthPrimeTab[NthPrimeCnt - 1];
    NthPrimeStr.aplNthCnt = (NthPrimeCnt - 1) * NthPrimeInc;
} // End InitPrimeTabs


//***************************************************************************
//  End of File: initdata.c
//***************************************************************************
