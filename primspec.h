//***************************************************************************
//  NARS2000 -- Primitive Function Special Header
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

// Define incomplete definition of a struc defined in full later
struct tagPRIMSPEC;

// Call this function for each monadic primitive function
typedef LPPL_YYSTYPE PRIMFN_MON
    (LPTOKEN lptkFunc,                      // Ptr to function token
     LPTOKEN lptkRhtArg,                    // Ptr to right arg token
     LPTOKEN lptkAxis,                      // Ptr to axis token (may be NULL)
     struct tagPRIMSPEC *lpPrimSpec);       // Ptr to local PRIMSPEC

typedef PRIMFN_MON *LPPRIMFN_MON;

// Call this function for each dyadic primitive function
typedef LPPL_YYSTYPE PRIMFN_DYD
    (LPTOKEN lptkLftArg,                    // Ptr to left arg token
     LPTOKEN lptkFunc,                      // Ptr to function token
     LPTOKEN lptkRhtArg,                    // Ptr to right arg token
     LPTOKEN lptkAxis,                      // Ptr to axis token (may be NULL)
     struct tagPRIMSPEC *lpPrimSpec);       // Ptr to local PRIMSPEC

typedef PRIMFN_DYD *LPPRIMFN_DYD;

typedef UBOOL PRIMFN_DYD_SNvSN
    (LPPL_YYSTYPE lpYYRes,                  // Ptr to the result

     LPTOKEN      lptkLftArg,               // Ptr to left arg token
     LPTOKEN      lptkFunc,                 // Ptr to function token
     LPTOKEN      lptkRhtArg,               // Ptr to right arg token

     HGLOBAL      hGlbLft,                  // Handle to left arg
     HGLOBAL      hGlbRht,                  // ...       right ...
     HGLOBAL     *lphGlbRes,                // Ptr to handle to result

     LPVOID       lpMemLft,                 // Points to Sig.nature
     LPVOID       lpMemRht,                 // ...

     LPAPLUINT    lpMemAxisHead,            // Ptr to axis values, fleshed out by CheckAxis_EM (may be NULL)
     LPAPLUINT    lpMemAxisTail,            // Ptr to grade up of AxisHead

     APLRANK      aplRankLft,               // Left arg rank
     APLRANK      aplRankRht,               // Right ...
     APLRANK      aplRankRes,               // Result ...

     APLSTYPE     aplTypeLft,               // Left arg storage type
     APLSTYPE     aplTypeRht,               // Right ...
     APLSTYPE     aplTypeRes,               // Result ...

     APLNELM      aplNELMLft,               // Left arg NELM
     APLNELM      aplNELMRht,               // Right ...
     APLNELM      aplNELMRes,               // Result ...
     APLNELM      aplNELMAxis,              // Axis ...

     UBOOL        bLftIdent,                // TRUE iff the function has a left identity element and the Axis tail is valid
     UBOOL        bRhtIdent,                // ...                         right ...

     struct tagPRIMSPEC *lpPrimSpec);       // Ptr to local PRIMSPEC

typedef PRIMFN_DYD_SNvSN *LPPRIMFN_DYD_SNvSN;

// Call this function to determine the storage type of the monadic result
typedef APLSTYPE STORAGE_TYPE_MON
        (APLNELM    aplNELMRht,             // Right arg NELM
         LPAPLSTYPE lpaplTypeRht,           // Ptr to right arg storage type
         LPTOKEN    lptkFunc);              // Ptr to function token

typedef STORAGE_TYPE_MON *LPSTORAGE_TYPE_MON;

// Call this function to determine the storage type of the dyadic result
typedef APLSTYPE STORAGE_TYPE_DYD
        (APLNELM    aplNELMLft,             // Left arg NELM
         LPAPLSTYPE lpaplTypeLft,           // Ptr to left arg storage type
         LPTOKEN    lptkFunc,               // Ptr to function token
         APLNELM    aplNELMRht,             // Right arg NELM
         LPAPLSTYPE lpaplTypeRht);          // Ptr to right arg storage type

typedef STORAGE_TYPE_DYD *LPSTORAGE_TYPE_DYD;

// Call this function if the monadic result is APA
typedef UBOOL APARESULT_MON
        (LPPL_YYSTYPE lpYYRes,              // Ptr to the result (may be NULL)

         LPTOKEN      lptkFunc,             // Ptr to function token

         HGLOBAL      hGlbRht,              // HGLOBAL of right arg
         HGLOBAL     *lphGlbRes,            // ...        result

         APLRANK      aplRankRht,           // Right arg rank

         struct tagPRIMSPEC *lpPrimSpec);   // Ptr to local PRIMSPEC

typedef APARESULT_MON *LPAPARESULT_MON;

// Call this function if the dyadic result is APA
typedef UBOOL APARESULT_DYD
        (LPPL_YYSTYPE lpYYRes,              // Ptr to the result (may be NULL)

         LPTOKEN      lptkFunc,             // Ptr to function token

         HGLOBAL      hGlbLft,              // HGLOBAL of left arg (may be NULL if simple)
         HGLOBAL      hGlbRht,              // ...        right ...
         HGLOBAL     *lphGlbRes,            // ...        result

         APLRANK      aplRankLft,           // Left arg rank
         APLRANK      aplRankRht,           // Right ...

         APLNELM      aplNELMLft,           // Left arg NELM
         APLNELM      aplNELMRht,           // Right ...

         APLINT       aplInteger,           // The integer from the simple side
         struct tagPRIMSPEC *lpPrimSpec);   // Ptr to local PRIMSPEC

typedef APARESULT_DYD *LPAPARESULT_DYD;


typedef APLBOOL  BISB         (APLBOOL           , struct tagPRIMSPEC *lpPrimSpec);
typedef APLBOOL  BISI         (APLINT            , struct tagPRIMSPEC *lpPrimSpec);
typedef APLBOOL  BISF         (APLFLOAT          , struct tagPRIMSPEC *lpPrimSpec);
typedef APLBOOL  BISC         (APLCHAR           , struct tagPRIMSPEC *lpPrimSpec);
typedef APLINT   IISI         (APLINT            , struct tagPRIMSPEC *lpPrimSpec);
typedef APLINT   IISF         (APLFLOAT          , struct tagPRIMSPEC *lpPrimSpec);
typedef APLINT   IISC         (APLCHAR           , struct tagPRIMSPEC *lpPrimSpec);
typedef APLFLOAT FISI         (APLINT            , struct tagPRIMSPEC *lpPrimSpec);
typedef APLFLOAT FISF         (APLFLOAT          , struct tagPRIMSPEC *lpPrimSpec);
typedef APLFLOAT FISC         (APLCHAR           , struct tagPRIMSPEC *lpPrimSpec);

typedef APLRAT   RISR         (APLRAT            , struct tagPRIMSPEC *lpPrimSpec);

typedef APLVFP   VISV         (APLVFP            , struct tagPRIMSPEC *lpPrimSpec);

typedef APLB64   B64ISB64     (APLB64            , struct tagPRIMSPEC *lpPrimSpec);
typedef APLB32   B32ISB32     (APLB32            , struct tagPRIMSPEC *lpPrimSpec);
typedef APLB16   B16ISB16     (APLB16            , struct tagPRIMSPEC *lpPrimSpec);
typedef APLB08   B08ISB08     (APLB08            , struct tagPRIMSPEC *lpPrimSpec);

typedef APLBOOL  BISBVB       (APLBOOL , APLBOOL , struct tagPRIMSPEC *lpPrimSpec);
typedef APLBOOL  BISBVI       (APLBOOL , APLINT  , struct tagPRIMSPEC *lpPrimSpec);
typedef APLBOOL  BISBVF       (APLBOOL , APLFLOAT, struct tagPRIMSPEC *lpPrimSpec);
typedef APLBOOL  BISBVC       (APLBOOL , APLCHAR , struct tagPRIMSPEC *lpPrimSpec);

typedef APLBOOL  BISIVB       (APLINT  , APLBOOL , struct tagPRIMSPEC *lpPrimSpec);
typedef APLBOOL  BISIVI       (APLINT  , APLINT  , struct tagPRIMSPEC *lpPrimSpec);
typedef APLBOOL  BISIVF       (APLINT  , APLFLOAT, struct tagPRIMSPEC *lpPrimSpec);
typedef APLBOOL  BISIVC       (APLINT  , APLCHAR , struct tagPRIMSPEC *lpPrimSpec);

typedef APLBOOL  BISFVB       (APLFLOAT, APLBOOL , struct tagPRIMSPEC *lpPrimSpec);
typedef APLBOOL  BISFVI       (APLFLOAT, APLINT  , struct tagPRIMSPEC *lpPrimSpec);
typedef APLBOOL  BISFVF       (APLFLOAT, APLFLOAT, struct tagPRIMSPEC *lpPrimSpec);
typedef APLBOOL  BISFVC       (APLFLOAT, APLCHAR , struct tagPRIMSPEC *lpPrimSpec);

typedef APLBOOL  BISCVB       (APLCHAR , APLBOOL , struct tagPRIMSPEC *lpPrimSpec);
typedef APLBOOL  BISCVI       (APLCHAR , APLINT  , struct tagPRIMSPEC *lpPrimSpec);
typedef APLBOOL  BISCVF       (APLCHAR , APLFLOAT, struct tagPRIMSPEC *lpPrimSpec);
typedef APLBOOL  BISCVC       (APLCHAR , APLCHAR , struct tagPRIMSPEC *lpPrimSpec);

typedef APLINT   IISBVB       (APLBOOL , APLBOOL , struct tagPRIMSPEC *lpPrimSpec);
typedef APLINT   IISBVI       (APLBOOL , APLINT  , struct tagPRIMSPEC *lpPrimSpec);
typedef APLINT   IISBVF       (APLBOOL , APLFLOAT, struct tagPRIMSPEC *lpPrimSpec);
typedef APLINT   IISBVC       (APLBOOL , APLCHAR , struct tagPRIMSPEC *lpPrimSpec);

typedef APLINT   IISIVB       (APLINT  , APLBOOL , struct tagPRIMSPEC *lpPrimSpec);
typedef APLINT   IISIVI       (APLINT  , APLINT  , struct tagPRIMSPEC *lpPrimSpec);
typedef APLINT   IISIVF       (APLINT  , APLFLOAT, struct tagPRIMSPEC *lpPrimSpec);
typedef APLINT   IISIVC       (APLINT  , APLCHAR , struct tagPRIMSPEC *lpPrimSpec);

typedef APLINT   IISFVB       (APLFLOAT, APLBOOL , struct tagPRIMSPEC *lpPrimSpec);
typedef APLINT   IISFVI       (APLFLOAT, APLINT  , struct tagPRIMSPEC *lpPrimSpec);
typedef APLINT   IISFVF       (APLFLOAT, APLFLOAT, struct tagPRIMSPEC *lpPrimSpec);
typedef APLINT   IISFVC       (APLFLOAT, APLCHAR , struct tagPRIMSPEC *lpPrimSpec);

typedef APLINT   IISCVB       (APLCHAR , APLBOOL , struct tagPRIMSPEC *lpPrimSpec);
typedef APLINT   IISCVI       (APLCHAR , APLINT  , struct tagPRIMSPEC *lpPrimSpec);
typedef APLINT   IISCVF       (APLCHAR , APLFLOAT, struct tagPRIMSPEC *lpPrimSpec);
typedef APLINT   IISCVC       (APLCHAR , APLCHAR , struct tagPRIMSPEC *lpPrimSpec);

typedef APLFLOAT FISBVB       (APLBOOL , APLBOOL , struct tagPRIMSPEC *lpPrimSpec);
typedef APLFLOAT FISBVI       (APLBOOL , APLINT  , struct tagPRIMSPEC *lpPrimSpec);
typedef APLFLOAT FISBVF       (APLBOOL , APLFLOAT, struct tagPRIMSPEC *lpPrimSpec);
typedef APLFLOAT FISBVC       (APLBOOL , APLCHAR , struct tagPRIMSPEC *lpPrimSpec);

typedef APLFLOAT FISIVB       (APLINT  , APLBOOL , struct tagPRIMSPEC *lpPrimSpec);
typedef APLFLOAT FISIVI       (APLINT  , APLINT  , struct tagPRIMSPEC *lpPrimSpec);
typedef APLFLOAT FISIVF       (APLINT  , APLFLOAT, struct tagPRIMSPEC *lpPrimSpec);
typedef APLFLOAT FISIVC       (APLINT  , APLCHAR , struct tagPRIMSPEC *lpPrimSpec);

typedef APLFLOAT FISFVB       (APLFLOAT, APLBOOL , struct tagPRIMSPEC *lpPrimSpec);
typedef APLFLOAT FISFVI       (APLFLOAT, APLINT  , struct tagPRIMSPEC *lpPrimSpec);
typedef APLFLOAT FISFVF       (APLFLOAT, APLFLOAT, struct tagPRIMSPEC *lpPrimSpec);
typedef APLFLOAT FISFVC       (APLFLOAT, APLCHAR , struct tagPRIMSPEC *lpPrimSpec);

typedef APLFLOAT FISCVB       (APLCHAR , APLBOOL , struct tagPRIMSPEC *lpPrimSpec);
typedef APLFLOAT FISCVI       (APLCHAR , APLINT  , struct tagPRIMSPEC *lpPrimSpec);
typedef APLFLOAT FISCVF       (APLCHAR , APLFLOAT, struct tagPRIMSPEC *lpPrimSpec);
typedef APLFLOAT FISCVC       (APLCHAR , APLCHAR , struct tagPRIMSPEC *lpPrimSpec);

typedef APLBOOL  BISRVR       (APLRAT  , APLRAT  , struct tagPRIMSPEC *lpPrimSpec);
typedef APLRAT   RISRVR       (APLRAT  , APLRAT  , struct tagPRIMSPEC *lpPrimSpec);

typedef APLBOOL  BISVVV       (APLVFP  , APLVFP  , struct tagPRIMSPEC *lpPrimSpec);
typedef APLVFP   VISVVV       (APLVFP  , APLVFP  , struct tagPRIMSPEC *lpPrimSpec);

typedef APLB64   B64ISB64VB64 (APLB64  , APLB64  , struct tagPRIMSPEC *lpPrimSpec);
typedef APLB32   B32ISB32VB32 (APLB32  , APLB32  , struct tagPRIMSPEC *lpPrimSpec);
typedef APLB16   B16ISB16VB16 (APLB16  , APLB16  , struct tagPRIMSPEC *lpPrimSpec);
typedef APLB08   B08ISB08VB08 (APLB08  , APLB08  , struct tagPRIMSPEC *lpPrimSpec);

// General structure passed to all primitive functions
typedef struct tagPRIMSPEC
{
    // Monadic functions
    LPPRIMFN_MON        PrimFnMon_EM_YY;    // Ptr to monadic primitive function
    LPSTORAGE_TYPE_MON  StorageTypeMon;     // ...            storage type ...
    LPAPARESULT_MON     ApaResultMon_EM;    // ...            APA result ...

    BISB               *BisB;               // Monadic B {is} B
    BISI               *BisI;               // ...            I
    BISF               *BisF;               // ...            F

////IISB               *IisB;               // Handled via type promotion (to IisI)
    IISI               *IisI;               // Monadic I {is} I
    IISF               *IisF;               // ...            F

////FISB               *FisB;               // Handled via type promotion (to FisF)
    FISI               *FisI;               // Monadic F {is} I
    FISF               *FisF;               // ...            F

    RISR               *RisR;               // Monadic R {is} R

////VISR               *VisR;               // Handled via type promotion (to VisV)
    VISV               *VisV;               // Monadic V {is} V

    // Dyadic functions
    LPPRIMFN_DYD        PrimFnDyd_EM_YY;    // Ptr to dyadic primitive function
    LPSTORAGE_TYPE_DYD  StorageTypeDyd;     // ...           storage type ...
    LPAPARESULT_DYD     ApaResultDyd_EM;    // ...           APA result ...

    BISBVB             *BisBvB;             // Dyadic B {is} B vs B
    BISIVI             *BisIvI;             // ...           I vs I
    BISFVF             *BisFvF;             // ...           F vs F
    BISCVC             *BisCvC;             // ...           C vs C

////IISBVB             *IisBvB;             // Handled via type promotion (to IisIvI)
    IISIVI             *IisIvI;             // Dyadic I {is} I vs I
    IISFVF             *IisFvF;             // ...           F vs F

////FISBVB             *FisBvB;             // Handled via type promotion (to FisFvF)
    FISIVI             *FisIvI;             // Dyadic F {is} I vs I
    FISFVF             *FisFvF;             // ...           F vs F

    BISRVR             *BisRvR;             // Dyadic B {is} R vs R
    RISRVR             *RisRvR;             // Dyadic R {is} R vs R

    BISVVV             *BisVvV;             // Dyadic B {is} V vs V
////VISRVV             *VisRvR;             // Handled via type promotion (to VisVvV)
    VISVVV             *VisVvV;             // Dyadic V {is} V vs V

    B64ISB64           *B64isB64;           // Monadic B64 {is} B64
    B32ISB32           *B32isB32;           // Monadic B32 {is} B32
    B16ISB16           *B16isB16;           // Monadic B16 {is} B16
    B08ISB08           *B08isB08;           // Monadic B08 {is} B08

    B64ISB64VB64       *B64isB64vB64;       // Dyadic B64 {is} B64 v B64
    B32ISB32VB32       *B32isB32vB32;       // Dyadic B32 {is} B32 v B32
    B16ISB16VB16       *B16isB16vB16;       // Dyadic B16 {is} B16 v B16
    B08ISB08VB08       *B08isB08vB08;       // Dyadic B08 {is} B08 v B08

    APLUINT             QuadRL;             // []RL for atomicity
} PRIMSPEC, *LPPRIMSPEC;


//***************************************************************************
//  End of File: primspec.h
//***************************************************************************
