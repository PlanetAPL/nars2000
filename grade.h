//***************************************************************************
//  NARS2000 -- Up/down Grade header
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2011 Sudley Place Software

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


typedef struct tagTT_HANDLES        // Translate Table handles & ptrs
{
    HGLOBAL   hGlbTT;               // TT global memory handle
    LPAPLCHAR lpMemTT;              // Ptr to TT global memory
} TT_HANDLES, *LPTT_HANDLES;


typedef struct tagGRADE_DATA
{
    APLSTYPE     aplTypeRht;        // Right arg storage type
    APLUINT      aplNELMRest,       // Product of right arg dims after the first
                 aplNELMRht;        // Right arg NELM
    APLINT       apaOffRht,         // Right arg APA offset
                 apaMulRht;         // Right arg APA multiplier
    int          iMul;              // Multiplier for GradeUp vs. GradeDown
                                    //   1 for GradeUp, -1 for GradeDown
    APLRANK      aplRankLft;        // Left arg rank
    LPTT_HANDLES lpMemTTHandles;    // Ptr to TT Handles global memory
    UBOOL        PV0:1,             // Right arg is a Perumtation Vector, origin-0
                 PV1:1;             // ...                                       1
    LPUBOOL      lpbCtrlBreak;      // Ptr to Ctrl-Break flag
} GRADE_DATA, *LPGRADE_DATA;


typedef struct tagPDIE
{
    APLUINT Index:63,               // Grade-up index
            Used :1;                // TRUE iff it's been used
} PDIE, *LPPDIE;

typedef void (*LPMEOCOM) (APLSTYPE,         // Left arg storage type
                          LPVOID,           // Ptr to left arg global memory
                          LPPDIE,           // Ptr to left arg grade-up global memory
                          APLINT,           // Loop counter
                          APLSTYPE,         // Right arg storage type
                          APLNELM,          // Right arg NELM
                          LPVOID,           // Ptr to right arg global memory
                          LPPDIE,           // Ptr to right arg grade-up global memory
                          APLFLOAT,         // []CT
                          LPAPLBOOL);       // Ptr to result global memory

typedef void (*LPMIOCOM) (APLSTYPE,         // Left arg storage type
                          APLNELM,          // Left arg NELM
                          LPVOID,           // Ptr to left arg global memory
                          LPPDIE,           // Ptr to left arg grade-up global memory
                          APLSTYPE,         // Right arg storage type
                          LPVOID,           // Ptr to right arg global memory
                          LPPDIE,           // Ptr to right arg grade-up global memory
                          APLINT,           // Loop counter
                          UBOOL,            // []IO
                          APLFLOAT,         // []CT
                          APLUINT,          // Not found value
                          LPAPLUINT);       // Ptr to result global memory

typedef APLBOOL (*LPMMCOM) (APLSTYPE ,      // Left arg storage type
                            APLNELM  ,      // Left arg NELM
                            LPVOID   ,      // Ptr to left arg global memory
                            LPPDIE   ,      // Ptr to left arg grade-up global memory
                            APLSTYPE ,      // Right arg storage type
                            LPVOID   ,      // Ptr to right arg global memory
                            LPPDIE   ,      // Ptr to right arg grade-up global memory
                            LPUBOOL  );     // Ptr to Ctrl-Break flag


//***************************************************************************
//  End of File: grade.h
//***************************************************************************
