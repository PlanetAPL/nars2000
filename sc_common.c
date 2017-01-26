//***************************************************************************
//  NARS2000 -- System Command Common Routines
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
#include <stdio.h>
#include <time.h>
#include "headers.h"


//***************************************************************************
//  $MakeWorkspaceNameCanonical
//
// Convert a workspace name into a canonical form (without WS_WKSEXT)
//***************************************************************************

void MakeWorkspaceNameCanonical
    (LPWCHAR lpwszOut,          // Output workspace name
     LPWCHAR lpwszInp,          // Input  ...
     LPWCHAR lpwszDefDir)       // Default drive and directory if no drive letter (may be NULL)

{
    UINT uLen;

    // If the incoming workspace name begins with a double-quote, skip over it
    if (lpwszInp[0] EQ WC_DQ)
        lpwszInp++;

    // Get the incoming workspace name string length
    uLen = lstrlenW (lpwszInp);

    // If the incoming workspace name ends with a double-quote, delete it
    if (uLen && lpwszInp[uLen - 1] EQ WC_DQ)
        lpwszInp[uLen - 1] = WC_EOS;

    // If the name doesn't begin with a drive letter and
    //   doesn't start at the root or a dot, prepend the
    //   default dir
    if (lpwszDefDir                 // Not NULL
     && lpwszInp[0] NE WC_EOS       // Non-empty,
     && ((lpwszInp[0] EQ L'.'
       && lpwszInp[1] EQ L'.')      // and up one dir
      || lpwszInp[0] NE L'.')       // or not current dir,
     && lpwszInp[0] NE WC_SLOPE     // and not root dir,
     && lpwszInp[1] NE L':')        // and no drive letter
    {
        strcpyW (lpwszOut, lpwszDefDir);

        // If the input doesn't already start with a backslash, ...
        if (lpwszInp[0] NE WC_SLOPE)
            AppendBackslash (lpwszOut);

        strcatW (lpwszOut, lpwszInp);
    } else
        strcpyW (lpwszOut, lpwszInp);

    // Get the outgoing workspace name string length
    uLen = lstrlenW (lpwszOut);

    // If the workspace name is long enough and
    //   ends with WSKEXT
    if (uLen >= WS_WKSEXT_LEN
     && lstrcmpiW (&lpwszOut[uLen - WS_WKSEXT_LEN], WS_WKSEXT) EQ 0)
        lpwszOut[uLen - WS_WKSEXT_LEN] = WC_EOS;
} // End MakeWorkspaceNameCanonical


//***************************************************************************
//  $AppendBackslash
//
//  Ensure the given string ends with a backslash
//***************************************************************************

void AppendBackslash
    (LPWCHAR lpwsz)

{
    UINT uLen;

    // Get the incoming string length
    uLen = lstrlenW (lpwsz);

    // If there's no trailing backslash, ...
    if (uLen && lpwsz[uLen - 1] NE WC_SLOPE)
        // Append one
        strcatW (lpwsz, WS_SLOPE);
} // End AppendBackslash


//***************************************************************************
//  $DisplayWorkspaceStamp
//
//  Display the workspace timestamp
//***************************************************************************

void DisplayWorkspaceStamp
    (LPDICTIONARY lpDict)               // Ptr to workspace dictionary

{
    WCHAR      wszTimeStamp[16 + 1],    // Output save area for time stamp
               wszVersion[WS_VERLEN];   // ...                  version info
    LPWCHAR    lpwszProf;               // Ptr to profile string
    FILETIME   ftCreation,              // Function creation time in UTC
               ftLocalTime;             // ...                       localtime
    SYSTEMTIME systemTime;              // Current system (UTC) time

    // Get the version #
    lpwszProf =
      ProfileGetString (SECTNAME_GENERAL,   // Ptr to the section name
                        KEYNAME_VERSION,    // Ptr to the key name
                        L"",                // Ptr to the default value
                        lpDict);            // Ptr to workspace dictionary
    // Copy the string to a save area
    // DO NOT USE lstrcpyW as it doesn't trigger a visible Page Fault
    MyStrcpyW (wszVersion, sizeof (wszVersion), lpwszProf);

    // Get the current system (UTC) time
    GetSystemTime (&systemTime);

    // Convert system time to file time and save as creation time
    SystemTimeToFileTime (&systemTime, &ftCreation);

    // Format the creation time
    MySprintfW (wszTimeStamp,
                sizeof (wszTimeStamp),
                FMTSTR_DATETIME,
                ftCreation.dwHighDateTime,
                ftCreation.dwLowDateTime);
    // Read the creation time
    lpwszProf =
      ProfileGetString (SECTNAME_GENERAL,       // Ptr to the section name
                        KEYNAME_CREATIONTIME,   // Ptr to the key name
                        wszTimeStamp,           // Ptr to the default value
                        lpDict);                // Ptr to the file name
    // Convert the CreationTime string to time
    sscanfW (lpwszProf, SCANFSTR_TIMESTAMP, (LPAPLINT) &ftCreation);

    if (OptionFlags.bUseLocalTime)
        // Convert to local filetime
        FileTimeToLocalFileTime (&ftCreation, &ftLocalTime);
    else
        ftLocalTime = ftCreation;

    // Convert the creation time to system time so we can display it
    FileTimeToSystemTime (&ftLocalTime, &systemTime);

    // Display the "SAVED ..." message
    DisplaySavedMsg (systemTime, OptionFlags.bUseLocalTime, wszVersion);
} // End DisplayWorkspaceStamp


//***************************************************************************
//  DisplaySavedMsg
//
//  Display the "SAVED ..." message
//***************************************************************************

void DisplaySavedMsg
    (SYSTEMTIME systemTime,         // System time
     UBOOL      bUseLocalTime,      // TRUE iff we should use LocalTime
     LPWCHAR    lpwszVersion)       // Workspace version # (may be NULL)

{
#define TIMESTAMP_FMT L"SAVED MM/DD/YYYY hh:mm:ss (GMT) (ver 0.00)"

    // "+ 1" for the trailing zero
    WCHAR wszTemp[strcountof (TIMESTAMP_FMT) + 1];

    strcpyW (wszTemp, L"SAVED ");

    // Format it
    MySprintfW (&wszTemp[lstrlenW (wszTemp)],
                 sizeof (wszTemp) - (lstrlenW (wszTemp) * sizeof (wszTemp[0])),
                 DATETIME_FMT L"%s",
                 systemTime.wMonth,
                 systemTime.wDay,
                 systemTime.wYear,
                 systemTime.wHour,
                 systemTime.wMinute,
                 systemTime.wSecond,
                 bUseLocalTime ? L"" : L" (GMT)");
#ifdef DEBUG
    if (lpwszVersion NE NULL)
        // Append the workspace version
        MySprintfW (&wszTemp[lstrlenW (wszTemp)],
                     sizeof (wszTemp) - (lstrlenW (wszTemp) * sizeof (wszTemp[0])),
                    L" (ver %s)",
                     lpwszVersion);
#endif
    // Display it
    AppendLine (wszTemp, FALSE, TRUE);
} // End DisplaySavedMsg


//***************************************************************************
//  $SendMessageLastTab
//
//  Send a (constant) message to the previously outgoing tab
//***************************************************************************

void SendMessageLastTab
    (const LPWCHAR lpwErrMsg,           // Ptr to constant error message text
     LPPERTABDATA  lpMemPTD)            // Ptr to PerTabData global memory

{
    int iPrvTabIndex;                   // Index of previous tab

    // Get the index of the tab from which we were )LOADed
    iPrvTabIndex = TranslateTabIDToIndex (lpMemPTD->PrvTabID);

    if (iPrvTabIndex NE -1)
    {
        // Get the PerTabData global memory ptr for the preceding tab
        lpMemPTD = GetPerTabPtr (iPrvTabIndex); Assert (IsValidPtr (lpMemPTD, sizeof (lpMemPTD)));

        // Send this error message to the previous tab's SM
        SendMessageW (lpMemPTD->hWndSM, MYWM_ERRMSG, 0, (LPARAM) lpwErrMsg);
    } // End IF
} // End SendMessageLastTab


//***************************************************************************
//  $MakeWorkspaceBackup
//
//  Save a backup copy of the workspace we're about to load
//***************************************************************************

void MakeWorkspaceBackup
    (LPWCHAR lpwszDPFE,                 // Drive, Path, Filename, Ext of the workspace (with WS_WKSEXT)
     LPWCHAR lpwExtType)                // LOADBAK_EXT or SAVEBAK_EXT

{
    WCHAR   wszTemp[_MAX_PATH];         // Save area for workspace backup DPFE
    APLUINT uLen;                       // Temporary length
    FILE   *fStream;                    // Ptr to file stream

    // The workspace name is of the form d:\path\to\workspace\filename.ws.nars
    // The backup    ...                 d:\path\to\workspace\filename.load.bak.ws.nars   or
    //                                   d:\path\to\workspace\filename.save.bak.ws.nars

    // Attempt to open the file
    fStream = fopenW (lpwszDPFE, L"r");

    // If the workspace doesn't exist, ...
    if (fStream EQ NULL)
        // No need to make a backup
        return;

    // We no longer need this resource
    fclose (fStream); fStream = NULL;

    // Copy the original name
    strcpyW (wszTemp, lpwszDPFE);

    // Get the entire length less WS_WKSEXT
    uLen = lstrlenW (wszTemp) - WS_WKSEXT_LEN;

    // Append new extensions
    strcpyW (&wszTemp[uLen], lpwExtType);

    // Copy the workspace to its backup
    if (!CopyFileW (lpwszDPFE,      // Source file (must exist)
                    wszTemp,        // Destination file (need not exist)
                    FALSE))         // TRUE iff Fail if dest file already exists
        FormatSystemErrorMessage (L"WORKSPACE BACKUP NOT COMPLETE");
} // End MakeWorkspaceBackup


//***************************************************************************
//  $SaveNewWsid_EM
//
//  Save a new []WSID
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SaveNewWsid_EM"
#else
#define APPEND_NAME
#endif

UBOOL SaveNewWsid_EM
    (LPAPLCHAR lpMemSaveWSID)           // Ptr to []WSID to save (includes WS_WKSEXT)

{
    HGLOBAL      hGlbWSID;              // []WSID global memory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    int          iLen,                  // Length of []WSID (with WS_WKSEXT)
                 iLen2;                 // ...              (w/o  ...)
    APLUINT      ByteWSID;              // # bytes in the []WSID
    LPAPLCHAR    lpMemNewWSID;          // Ptr to new []WSID global memory
    UBOOL        bRet = FALSE;          // TRUE iff result is valid

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get the length of the []WSID (including WS_WKSEXT)
    iLen = lstrlenW (lpMemSaveWSID);

    // If the []WSID is non-empty, ...
    if (iLen)
    {
        // Omit the trailing WS_WKSEXT
        iLen2 = iLen - WS_WKSEXT_LEN;
        Assert (lpMemSaveWSID[iLen2] EQ L'.');
        lpMemSaveWSID[iLen2] = WC_EOS;

        // Save the WSID in the list of recent files
        SaveRecentWSID (lpMemSaveWSID);

        // Calculate space needed for the new WSID
        ByteWSID = CalcArraySize (ARRAY_CHAR, iLen2, 1);

        // Check for overflow
        if (ByteWSID NE (APLU3264) ByteWSID)
            goto WSFULL_EXIT;

        // Allocate space for the new WSID
        hGlbWSID = DbgGlobalAlloc (GHND, (APLU3264) ByteWSID);
        if (hGlbWSID EQ NULL)
            goto WSFULL_EXIT;

        // Lock the memory to get a ptr to it
        lpMemNewWSID = MyGlobalLock000 (hGlbWSID);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemNewWSID)
        // Fill in the header
        lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
        lpHeader->ArrType    = ARRAY_CHAR;
////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////lpHeader->SysVar     = FALSE;       // Already zero from GHND
        lpHeader->RefCnt     = 1;
        lpHeader->NELM       = iLen2;
        lpHeader->Rank       = 1;
#undef  lpHeader

        // Fill in the dimension
        *VarArrayBaseToDim (lpMemNewWSID) = iLen2;

        // Skip over the header and dimensions to the data
        lpMemNewWSID = VarArrayDataFmBase (lpMemNewWSID);

        // Copy data to the new []WSID
        CopyMemoryW (lpMemNewWSID, lpMemSaveWSID, iLen2);

        // We no longer need this ptr
        MyGlobalUnlock (hGlbWSID); lpMemNewWSID = NULL;
    } else
        hGlbWSID = hGlbV0Char;

    // Free the old []WSID
    FreeResultGlobalVar (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_WSID]->stData.stGlbData); lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_WSID]->stData.stGlbData = NULL;

    // Save the new []WSID
    lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_WSID]->stData.stGlbData = MakePtrTypeGlb (hGlbWSID);

    // Tell the Tab Ctrl about the new workspace name
    NewTabName (NULL);

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

WSFULL_EXIT:
    // Display the error message
    ReplaceLastLineCRPmt (ERRMSG_NOT_SAVED_WS_FULL APPEND_NAME);

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (iLen)
        // Restore zapped char
        lpMemSaveWSID[iLen2] = L'.';

    return bRet;
} // End SaveNewWsid_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ShortenWSID
//
//  Shorten a WSID if it's in the default workspace dir
//***************************************************************************

LPWCHAR ShortenWSID
    (LPWCHAR lpMemWSID)

{
    UINT uLen;

    // Get the length of the default workspace dir
    uLen = lstrlenW (lpwszWorkDir);

    // If the workspace is saved into the default location,
    //   omit the leading portion of the text
    if (strncmpW (lpwszWorkDir, lpMemWSID, uLen) EQ 0)
        return &lpMemWSID[uLen];
    else
        return lpMemWSID;
} // End ShortenWSID


//***************************************************************************
//  $SaveRecentWSID
//
//  Save a WSID in the list of recent files
//***************************************************************************

void SaveRecentWSID
    (LPAPLCHAR lpMemWSID)               // Ptr to []WSID to save (excludes WS_WKSEXT)

{
    UINT  uCnt;                             // Loop counter
    WCHAR (*lpwszRecentFiles)[][_MAX_PATH]; // Ptr to list of recent files

    // Lock the memory to get a ptr to it
    lpwszRecentFiles = MyGlobalLock (hGlbRecentFiles);

    // Check for duplicates
    for (uCnt = 0; uCnt < uNumRecentFiles; uCnt++)
    {
        if (lstrcmpW (lpMemWSID, (*lpwszRecentFiles)[uCnt]) EQ 0)
        {
            // It's in our list, so move it from position uCnt to 0
            if (uCnt NE 0)
            {
                // Move entries from [0, uCnt - 1] to [1, uCnt]
                MoveMemoryW ((*lpwszRecentFiles)[1],
                             (*lpwszRecentFiles)[0],
                             uCnt * _MAX_PATH);
                // Copy the new file to the top
                strcpyW ((*lpwszRecentFiles)[0], lpMemWSID);
            } // End IF

            break;
        } // End IF
    } // End FOR

    // If it's not in our list, ...
    if (uCnt EQ uNumRecentFiles)
    {
        // If there's room at the end of the list, ...
        if (uNumRecentFiles < DEF_RECENTFILES)
            // Append it to the end and
            //   increase the # Recent Files
            strcpyW ((*lpwszRecentFiles)[uNumRecentFiles++], lpMemWSID);
        else
        {
            // Move entries from [0, DEF_RECENTFILES - 2] to [1, DEF_RECENTFILES - 1]
            MoveMemoryW ((*lpwszRecentFiles)[1],
                         (*lpwszRecentFiles)[0],
                         (DEF_RECENTFILES - 1) * _MAX_PATH);
            // Copy the new file to the top
            strcpyW ((*lpwszRecentFiles)[0], lpMemWSID);
        } // End IF/ELSE
    } // End IF

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRecentFiles); lpwszRecentFiles = NULL;
} // End SaveRecentWSID


//***************************************************************************
//  $CheckCommandLine
//
//  Check a system command line for switches
//***************************************************************************

UBOOL CheckCommandLine
    (LPWCHAR   lpwszTail,               // Ptr to command line (after the command itself)
     LPUINT    lpuArgCnt,               // Ptr to # args
     LPWCHAR **lplplpwszArgs,           // Ptr to ptr to ptr to args
     LPWCHAR  *lplpwszLeadRange,        // Ptr to ptr to leading range
     LPWCHAR  *lplpwszTailRange,        // ...           trailing range
                                        //              (may point to NULL if no separator)
     UBOOL     bLibCmd)                 // TRUE iff )LIB

{
    LPWCHAR lpwszSwitch = NULL,         // Ptr to switch
            lpw;                        // Temporary ptr
    UINT    uCnt;                       // Loop counter

    // If there's a command tail, ...
    if (lpwszTail[0])
        // Convert the argument line to argc/argv format
        *lplplpwszArgs =
          CommandLineToArgvW (lpwszTail, lpuArgCnt);
    else
        *lpuArgCnt = 0;

    // If it's the )LIB command,
    if (bLibCmd)
    {
        // Check for LIBCMD_SWITCH parameters
        for (uCnt = 0; uCnt < *lpuArgCnt; uCnt++)
        if ((*lplplpwszArgs)[uCnt][0] EQ LIBCMD_SWITCH)
            // Skip over the leading slash
            lpwszSwitch = &(*lplplpwszArgs)[uCnt][1];
        // Check for too much on the line
        switch (*lpuArgCnt)
        {
            case 0:             // No args
            case 1:             // Dir or switch
                break;

            case 2:             // Dir and switch
                // If there's a switch specified, ...
                if (lpwszSwitch)
                    break;

                // Fall through to error code

            default:
                IncorrectCommand ();

                return FALSE;
        } // End SWITCH
    } else
    {
        if (*lpuArgCnt)
            lpwszSwitch = (*lplplpwszArgs)[0];

        // Check for too much on the line
        if (*lpuArgCnt > 1)
        {
            IncorrectCommand ();

            return FALSE;
        } // End IF
    } // End IF/ELSE

    // If there's a switch, ...
    if (lpwszSwitch)
    {
        // Set a ptr to the first part
        *lplpwszLeadRange = lpwszSwitch;

        // Look for a separator
        lpw = strchrW (lpwszSwitch, L'-');

        // If it's present, ...
        if (lpw)
        {
            // Zap it, terminating the first range
            *lpw = WC_EOS;

            // Set a ptr to the second part
            *lplpwszTailRange = &lpw[1];
        } else
            *lplpwszTailRange = NULL;
    } else
        *lplpwszLeadRange = *lplpwszTailRange = L"";

    return TRUE;
} // End CheckCommandLine


//***************************************************************************
//  $SplitSwitches
//
//  Split out switches of the form -name=value
//***************************************************************************

LPWCHAR SplitSwitches
    (LPWCHAR lpwszTail,         // Ptr to tail of the command line
                                //   (everything after the command name)
     LPUINT  lpuInt)            // Ptr to # switches on the command line

{
    LPWCHAR lpwszCmd = NULL,    // Ptr to 1st switch (if any)
            lpwszNxt;           // Temporary ptr
    WCHAR   wcTmp;              // ...       char

    // Initialize the switch count
    *lpuInt = 0;

    // Skip over the Drive:\Path\filename.ext
    lpwszCmd = SkipToCharDQW (lpwszTail, L' ');
    wcTmp = *lpwszCmd; *lpwszCmd++ = WC_EOS; lpwszNxt = lpwszCmd;

    if (wcTmp)
    while (TRUE)
    {
        // Move the switches down over any leading blanks
        while (IsWhiteW (lpwszNxt[0]))
            MoveMemory (lpwszNxt, &lpwszNxt[1], (1 + lstrlenW (lpwszNxt)) * sizeof (lpwszNxt[0]));

        // Count the # switches
        if (lpwszNxt[0] EQ L'-'
         || lpwszNxt[0] EQ L'/')
            // Count in another switch
            (*lpuInt)++;
        else
            break;

        // Skip over the switch
        lpwszNxt = SkipToCharDQW (lpwszNxt, L' ');

        // Terminate the switch and skip over it
        wcTmp = *lpwszNxt; *lpwszNxt++ = WC_EOS;
        if (wcTmp EQ WC_EOS)
            break;
    } // End IF/WHILE

    return lpwszCmd;
} // End SplitSwitches


//***************************************************************************
//  End of File: sc_common.c
//***************************************************************************
