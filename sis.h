//***************************************************************************
//  NARS2000 -- State Indicator Stack Header
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
                     bSuspended:1,  //      00000100:  TRUE iff the function is suspended
                     ResetFlag:3,   //      00000E00:  SI stack is resetting (see RESET_FLAGS)
                     bRestartable:1,//      00001000:  TRUE iff this SI level is restartable
                     bUnwind:1,     //      00002000:  TRUE iff unwind this level for error message level
                     bItsEC:1,      //      00004000:  TRUE iff DFNTYPE_ERRCTRL and this level is []EC (not []EA)
                     bAFO:1,        //      00008000:  TRUE iff this level is an AFO
                     bMFO:1,        //      00010000:  TRUE iff this level is an MFO
                     bLclRL:1,      //      00020000:  TRUE iff []RL is localized in this AFO
                     bAfoValue:1,   //      00040000:  TRUE iff an AFO returned a value
                     :13;           //      FFF80000:  Available bits
    EVENT_TYPES      EventType;     // 20:  Event type (Major, Minor) (see EVENT_TYPES)
    UINT             CurLineNum,    // 24:  Current line # (origin-1)
                     NxtLineNum,    // 28:  Next    ...
                     NxtTknNum,     // 2C:  Next token #
                     numLabels,     // 30:  # line labels
                     numFcnLines,   // 34:  # lines in the function (not counting the header)
                     numSymEntries, // 38:  # LPSYMENTRYs localized on the stack
                     QQPromptLen,   // 3C:  Quote-Quad input prompt length
                     ErrorCode;     // 40:  Error code (see ERROR_CODES)
    int              iCharIndex;    // 44:  Caret position of current function
    struct tagSIS_HEADER
                    *lpSISErrCtrl,  // 48:  Ptr to controlling []EA/[]EC SIS header (NULL = none)
                    *lpSISPrv,      // 4C:  Ptr to previous SIS header (NULL = none)
                    *lpSISNxt;      // 50:  Ptr to next     ...         ...
    LPTOKEN          lptkFunc;      // 54:  Ptr to function token for Quote-Quad input
    struct tagFORSTMT
                    *lpForStmtBase, // 58:  Ptr to starting entry in FORSTMT stack
                    *lpForStmtNext; // 5C:  Ptr to next available ...
    LPPLLOCALVARS    lpplLocalVars; // 60:  Ptr to this level's plLocalVars
    LPHSHTABSTR      lphtsPrv;      // 64:  Ptr to previous HSHTABSTR (may be NULL)
                                    // 68:  Length
                                    // 68:  Array of LPSYMENTRYs (shadowed entry for results, args, labels, & locals)
} SIS_HEADER, *LPSIS_HEADER;


//***************************************************************************
//  End of File: sis.h
//***************************************************************************
