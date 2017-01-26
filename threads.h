//***************************************************************************
//  NARS2000 -- Threads Header
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

typedef struct tagCNT_THREAD        // Thread struct for CreateNewTabInThread
{
    HWND    hWndParent;             // 00:  Window handle of the parent
    HGLOBAL hGlbDPFE;               // 04:  Workspace DPFE global memory handle
    int     iTabIndex;              // 08:  Insert the new tab to the left of this one
    UBOOL   bExecLX;                // 0C:  TRUE iff execute []LX after successful load
    HANDLE  hThread;                // 10:  Handle to the current thread
    DWORD   dwThreadId;             // 14:  Thread Id
                                    // 18:  Length
} CNT_THREAD, *LPCNT_THREAD;


typedef struct tagIE_THREAD         // Thread struct for ImmExecStmtInThread
{
    HWND         hWndEC;            // 00:  Handle of Edit Ctrl window
    LPPERTABDATA lpMemPTD;          // 04:  Ptr to PerTabData global memory
    LPWCHAR      lpwszCompLine;     // 08:  Ptr to complete line
    APLNELM      aplNELM;           // 0C:  NELM of lpwszCompLine
    HGLOBAL      hGlbWFSO;          // 10:  WaitForSingleObject callback global memory handle
    UBOOL        bFreeLine,         // 14:  TRUE iff we should VirtualFree lpwszCompLine on completion
                 bActOnErrors;      // 18:  TRUE iff errors are acted upon
                                    // 1C:  Length
} IE_THREAD, *LPIE_THREAD;


typedef struct tagCDB_THREAD        // Thread struct for CreateDebuggerInThread
{
    LPPERTABDATA lpMemPTD;          // 00:  Ptr to PerTabData global memory
                                    // 04:  Length
} CDB_THREAD, *LPCDB_THREAD;


typedef struct tagCR_THREAD         // Thread struct for CreateResetInThread
{
    LPPERTABDATA lpMemPTD;          // 00:  Ptr to PerTabData global memory
    HANDLE       hThread;           // 04:  Handle to the current thread
    DWORD        dwThreadId;        // 08:  Thread Id
                                    // 0C:  Length
} CR_THREAD, *LPCR_THREAD;


//***************************************************************************
//  End of File: threads.h
//***************************************************************************
