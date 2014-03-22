//***************************************************************************
//  NARS2000 -- System Command:  )WSID
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

#define STRICT
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "headers.h"


//***************************************************************************
//  $CmdWsid_EM
//
//  Execute the system command:  )WSID [wsid]
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CmdWsid_EM"
#else
#define APPEND_NAME
#endif

UBOOL CmdWsid_EM
    (LPWCHAR lpwszTail)                 // Ptr to command line tail

{
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    APLNELM      aplNELMWSID;           // []WSID NELM
    APLRANK      aplRankWSID;           // ...    rank
    LPAPLCHAR    lpMemWSID;             // Ptr to old []WSID global memory
    WCHAR        wszTailDPFE[_MAX_PATH];// Save area for canonical form of given ws name
    LPWCHAR      lpw;                   // Temporary ptr
    APLUINT      ByteWSID;              // # bytes in the []WSID
    HGLOBAL      hGlbWSID;              // []WSID global memory handle
    UINT         uLen;                  // Length of given WSID
    UBOOL        bRet = FALSE;          // TRUE iff result is valid

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Lock the memory to get a ptr to it
    lpMemWSID = MyGlobalLock (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_WSID]->stData.stGlbData);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemWSID)
    // Get the NELM and Rank
    aplNELMWSID = lpHeader->NELM;
    aplRankWSID = lpHeader->Rank;
#undef  lpHeader

    // Skip to the next blank
    lpw = SkipToCharDQW (lpwszTail, L' ');

    // Zap it in case there are trailing blanks
    *lpw = WC_EOS;

    // If there's a given WSID, set the WSID
    if (*lpwszTail)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_WSID]->stData.stGlbData); lpMemWSID = NULL;

        // Convert the given workspace name into a canonical form
        MakeWorkspaceNameCanonical (wszTailDPFE, lpwszTail, lpwszWorkDir);

        // Get the WSID length
        uLen = lstrlenW (wszTailDPFE);

        // Calculate space needed for the WSID
        ByteWSID = CalcArraySize (ARRAY_CHAR, uLen, 1);

        if (ByteWSID NE (APLU3264) ByteWSID)
            goto WSFULL_EXIT;

        // Allocate space for the WSID
        hGlbWSID = DbgGlobalAlloc (GHND, (APLU3264) ByteWSID);
        if (!hGlbWSID)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemWSID = MyGlobalLock (hGlbWSID);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemWSID)
        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = ARRAY_CHAR;
////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////lpHeader->SysVar     = FALSE;       // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = uLen;
        lpHeader->Rank       = 1;
#undef  lpHeader

        // Fill in the dimension
        *VarArrayBaseToDim (lpMemWSID) = uLen;

        // Skip over the header and dimensions to the data
        lpMemWSID = VarArrayBaseToData (lpMemWSID, 1);

        // Copy data to the []WSID
        CopyMemoryW (lpMemWSID, wszTailDPFE, uLen);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbWSID); lpMemWSID = NULL;

        // Lock the memory to get a ptr to it
        lpMemWSID = MyGlobalLock (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_WSID]->stData.stGlbData);

        // Skip over the header and dimensions to the data
        lpMemWSID = VarArrayBaseToData (lpMemWSID, aplRankWSID);

        // Display the old WSID
        AppendLine (L"WAS ", FALSE, FALSE);

        // If the old []WSID is empty, ...
        if (IsEmpty (aplNELMWSID))
            AppendLine (L"CLEAR WS", FALSE, TRUE);
        else
            // Because the global memory has a zero terminator,
            //   we don't need to copy the data to a temporary location and then
            //   append a zero terminator
            AppendLine (ShortenWSID (lpMemWSID), FALSE, TRUE);

        // We no longer need this ptr
        MyGlobalUnlock (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_WSID]->stData.stGlbData); lpMemWSID = NULL;

        // Free the old []WSID
        FreeResultGlobalVar (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_WSID]->stData.stGlbData); lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_WSID]->stData.stGlbData = NULL;

        // Save the new []WSID
        lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_WSID]->stData.stGlbData = MakePtrTypeGlb (hGlbWSID);

        // Tell the Tab Ctrl about the new workspace name
        NewTabName (NULL);
    } else
    // Display the current WSID
    {
        AppendLine (L"IS ", FALSE, FALSE);

        // If current []WSID is empty, ...
        if (IsEmpty (aplNELMWSID))
            AppendLine (L"CLEAR WS", FALSE, TRUE);
        else
        {
            // Skip over the header and dimensions to the data
            lpMemWSID = VarArrayBaseToData (lpMemWSID, aplRankWSID);

            // Because the global memory has a zero terminator,
            //   we don't need to copy the data to a temporary location and then
            //   append a zero terminator
            AppendLine (ShortenWSID (lpMemWSID), FALSE, TRUE);
        } // End IF/ELSE

        // We no longer need this ptr
        MyGlobalUnlock (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_WSID]->stData.stGlbData); lpMemWSID = NULL;
    } // End IF/ELSE

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    ReplaceLastLineCRPmt (ERRMSG_WS_FULL APPEND_NAME);

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    return bRet;
} // End CmdWsid_EM
#undef  APPEND_NAME


//***************************************************************************
//  End of File: sc_wsid.c
//***************************************************************************
