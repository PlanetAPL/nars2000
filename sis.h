//***************************************************************************
//  NARS2000 -- State Indicator Stack Header
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

typedef enum tagRESET_FLAGS
{
    RESETFLAG_NONE = 0,             // 00:  No resetting
    RESETFLAG_ONE,                  // 01:  Reset one level  -- {goto}
    RESETFLAG_ONE_INIT,             // 02:  Reset one level  -- {goto} (initial case)
    RESETFLAG_ALL,                  // 03:  Reset all levels -- )RESET
    RESETFLAG_QUADERROR_INIT,       // 04:  Reset one level  -- []ERROR/[]ES (initialization)
    RESETFLAG_QUADERROR_EXEC,       // 05:  Reset one level  -- []ERROR/[]ES (execute []ELX)
    RESETFLAG_STOP,                 // 06:  Reset to next suspended or immexec level
                                    // 07-07:  Available entries (3 bits)
} RESET_FLAGS;

// User-defined function/operator header signature
#define SIS_HEADER_SIGNATURE   ' SIS'

typedef struct tagSIS_HEADER
{
    HEADER_SIGNATURE Sig;           // 00:  SIS header signature
    HANDLE           hSemaphore,    // 04:  Semaphore handle
                     hSigaphore;    // 08:  Semaphore handle to signal on exit (NULL if none)
    HGLOBAL          hGlbDfnHdr,    // 0C:  User-defined function/operator global memory handle (may be NULL if not fcn/op)
                     hGlbFcnName,   // 10:  Function name global memory handle (may be NULL if not fcn/op)
                     hGlbQuadEM;    // 14:  []EM global memory handle
    LPSYMENTRY       lpSymGlbGoto;  // 18:  []EC[2] entry for {goto} target as an LPSYMENTRY or HGLOBAL
    UINT             DfnType:4,     // 1C:  0000000F:  User-defined function/operator Type (see DFN_TYPES)
                     FcnValence:3,  //      00000070:  User-defined function/operator Valence (see FCN_VALENCES)
                     DfnAxis:1,     //      00000080:  User-defined function/operator accepts axis value
                     Suspended:1,   //      00000100:  Function is suspended
                     ResetFlag:3,   //      00000E00:  SI stack is resetting (see RESET_FLAGS)
                     PermFn:1,      //      00001000:  Permanent function (i.e. Magic Function/Operator)
                     Restartable:1, //      00002000:  This SI level is restartable
                     Unwind:1,      //      00004000:  Unwind this level for error message level
                     ItsEC:1,       //      00008000:  TRUE iff DFNTYPE_ERRCTRL and this level is []EC (not []EA)
                     bAFO:1,        //      00010000:  TRUE iff this level is an AFO
                     :15;           //      FFFE0000:  Available bits
    EVENT_TYPES      EventType;     // 20:  Event type (Major, Minor) (see EVENT_TYPES)
    UINT             CurLineNum,    // 24:  Current line # (origin-1)
                     NxtLineNum,    // 28:  Next    ...
                     NxtTknNum,     // 2C:  Next token #
                     numLabels,     // 30:  # line labels
                     numFcnLines,   // 34:  # lines in the function (not counting the header)
                     numSymEntries, // 38:  # LPSYMENTRYs localized on the stack
                     QQPromptLen,   // 3C:  Quote-Quad input prompt length
                     ErrorCode;     // 40:  Error code (see ERROR_CODES)
    struct tagSIS_HEADER
                    *lpSISErrCtrl,  // 44:  Ptr to controlling []EA/[]EC SIS header (NULL = none)
                    *lpSISPrv,      // 48:  Ptr to previous SIS header (NULL = none)
                    *lpSISNxt;      // 4C:  Ptr to next     ...         ...
    LPTOKEN          lptkFunc;      // 50:  Ptr to function token for Quote-Quad input
    struct tagFORSTMT
                    *lpForStmtBase, // 54:  Ptr to starting entry in FORSTMT stack
                    *lpForStmtNext; // 58:  Ptr to next available ...
                                    // 5C:  Length
                                    // 5C:  Array of LPSYMENTRYs (shadowed entry for results, args, labels, & locals)
} SIS_HEADER, *LPSIS_HEADER;


//***************************************************************************
//  End of File: sis.h
//***************************************************************************
