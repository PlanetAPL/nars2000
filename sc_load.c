//***************************************************************************
//  NARS2000 -- System Command:  )LOAD
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
#ifndef _WIN64
  #define _USE_32BIT_TIME_T
#endif
#include <io.h>
#include <string.h>             // For wcsspnp
#define _NO_INLINING
#include <tchar.h>
#include "headers.h"
#include "debug.h"              // For xxx_TEMP_OPEN macros


// System Time Stamp for "1 CLEANSPACE"
// SAVED 1966-11-27 18.53.59 (GMT-4)
SYSTEMTIME cleanspaceTime = {1966,      // Year
                               11,      // Month           (1=Jan)
                                0,      // Day of the week (0=Sun)
                               27,      // Day             (1-31)
                               22,      // Hour            (0-23)
                               53,      // Minute          (0-59)
                               59,      // Second          (0-59)
                                0};     // Millisecond     (0-999)


//***************************************************************************
//  $CmdLoad_EM
//
//  Execute the system command:  )LOAD wsid
//***************************************************************************

UBOOL CmdLoad_EM
    (LPWCHAR lpwszTail)                 // Ptr to command line tail

{
    return CmdLoadCom_EM (lpwszTail, TRUE);
} // End CmdLoad_EM


//***************************************************************************
//  $CmdXload_EM
//
//  Execute the system command:  )XLOAD wsid
//***************************************************************************

UBOOL CmdXload_EM
    (LPWCHAR lpwszTail)                 // Ptr to command line tail

{
    return CmdLoadCom_EM (lpwszTail, FALSE);
} // End CmdXload_EM


//***************************************************************************
//  $CmdLoadCom_EM
//
//  Execute the system command:  )LOAD or )XLOAD wsid
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CmdLoadCom_EM"
#else
#define APPEND_NAME
#endif

UBOOL CmdLoadCom_EM
    (LPWCHAR lpwszTail,                     // Ptr to command line tail
     UBOOL   bExecLX)                       // TRUE iff execute []LX after successful load

{
    LPPERTABDATA  lpMemPTD;                 // Ptr to PerTabData global memory
    WCHAR         wszTailDPFE[_MAX_PATH];   // Save area for canonical form of given ws name
    LPWCHAR       lpw,                      // Temporary ptr
                  lpwszFormat;                  // Ptr to temporary storage
    int           iTabIndex;                // Tab index
    UBOOL         bRet = FALSE;             // TRUE iff the result is valid
    UINT          uCnt;                     // Loop counter
    LPWSZLIBDIRS  lpwszLibDirs = NULL;      // Ptr to LibDirs
    WCHAR         wszDrive[_MAX_DRIVE],     // Save area for "D:"
                  wszDir  [_MAX_DIR];       // ...           "\path\to\dir"
//////////////////wszFname[_MAX_FNAME],     // ...           "filename"
//////////////////wszExt  [_MAX_EXT];       // ...           ".ext"

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get ptr to temporary storage
    lpwszFormat = lpMemPTD->lpwszFormat;

    // Check for "1 CLEANSPACE"
    if (lstrcmpiW (lpwszTail, L"1 CLEANSPACE") NE 0)
    {
        // Skip to the next blank
        lpw = SkipToCharDQW (lpwszTail, L' ');

        // Zap it in case there are trailing blanks
        *lpw = WC_EOS;

        // If there's no WSID, that's an error
        if (lpwszTail[0] EQ WC_EOS)
        {
            IncorrectCommand ();

            return FALSE;
        } // End IF

        // Split out the drive and path from the module filename
        _wsplitpath (lpwszTail, wszDrive, wszDir, NULL, NULL);

        // Lock the memory to get a ptr to it
        lpwszLibDirs = MyGlobalLock (hGlbLibDirs);

        // If the command tail starts with a drive letter, or a backslash, ...
        if (wszDrive[0]
         || wszDir[0] EQ WC_SLOPE)
        {
            // Process this directory
            bRet =
              CmdLoadProcess (&wszTailDPFE[0],      // Ptr to save area for canonical form of given ws name
                               lpwszTail,           // Ptr to command line tail
                               L"");                // Ptr to directory
        } else
        // Loop through the search dirs
        for (uCnt = 0; !bRet && uCnt < uNumLibDirs; uCnt++)
        {
            // Process this directory
            bRet =
              CmdLoadProcess (&wszTailDPFE[0],      // Ptr to save area for canonical form of given ws name
                               lpwszTail,           // Ptr to command line tail
                               lpwszLibDirs[uCnt]); // Ptr to directory
        } // End FOR

        // If we failed, ...
        if (!bRet)
            goto WSNOTFOUND_EXIT;
    } else
        // Copy the name to the expected var
        MyStrcpyW (wszTailDPFE, sizeof (wszTailDPFE), lpwszTail);

    // Get the tab index from which this command was issued
    iTabIndex = TranslateTabIDToIndex (lpMemPTD->CurTabID);

    bRet =
      CreateNewTab (hWndMF,             // Parent window handle
                    wszTailDPFE,        // Drive, Path, Filename, Ext of the workspace
                    iTabIndex + 1,      // Insert new tab to the left of this one
                    bExecLX);           // TRUE iff execute []LX after successful load
WSNOTFOUND_EXIT:
    // If we locked it, ...
    if (lpwszLibDirs NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbLibDirs); lpwszLibDirs = NULL;
    } // End IF

    if (!bRet)
        ReplaceLastLineCRPmt (ERRMSG_WS_NOT_FOUND APPEND_NAME);

    return FALSE;
} // End CmdLoadCom_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CmdLoadProcess
//
//  Process a )LOAD command
//***************************************************************************

UBOOL CmdLoadProcess
    (LPWCHAR lpwszTailDPFE,                 // Ptr to save area for canonical form of given ws name
     LPWCHAR lpwszTail,                     // Ptr to command line tail
     LPWCHAR lpwszDir)                      // Ptr to directory

{
    FILE *fStream;                          // Ptr to file stream for the plain text workspace file

    // Convert the given workspace name into a canonical form (without WS_WKSEXT)
    MakeWorkspaceNameCanonical (lpwszTailDPFE, lpwszTail, lpwszDir);

    // Append the common workspace extension
    strcatW (lpwszTailDPFE, WS_WKSEXT);

    // Handle WS NOT FOUND messages here
    // Attempt to open the workspace
    fStream = fopenW (lpwszTailDPFE, L"r");

    // If the workspace doesn't exist, ...
    if (fStream EQ NULL)
        return FALSE;

    // We no longer need this resource
    fclose (fStream); fStream = NULL;

    return TRUE;
} // End CmdLoadProcess


//***************************************************************************
//  $LoadWorkspace_EM
//
//  Load a workspace
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- LoadWorkspace_EM"
#else
#define APPEND_NAME
#endif

UBOOL LoadWorkspace_EM
    (HGLOBAL hGlbDPFE,                  // Workspace DPFE global memory handle (NULL = CLEAR WS)
     HWND    hWndEC)                    // Edit Ctrl window handle

{
    LPWCHAR      lpwszDPFE = NULL,      // Drive, Path, Filename, Ext of the workspace (with WS_WKSEXT)
                 lpwErrMsg;             // Ptr to (constant) error message text
    FILE        *fStream;               // Ptr to file stream for the plain text workspace file
    UINT         uSymVar,               // Var section counter
                 uSymFcn,               // Fcn/Opr ...
                 uGlbCnt,               // [Globals] count
                 uSILevel,              // [General] SILevel value
                 uSID,                  // Loop counter
                 uCnt;                  // Loop counter
    WCHAR        wszCount[8],           // Save area for formatted uSymVar/Fcn counter
                 wszVersion[WS_VERLEN], // ...                     version info
                 wszSectName[15];       // ...                     section name (e.g., [Vars.nnn])
    UBOOL        bRet = FALSE,          // TRUE iff the result is valid
                 bImmed,                // TRUE iff the result of ParseSavedWsVar_EM is immediate
                 bExecLX,               // TRUE iff execute []LX after successful load
                 bSuspended;            // TRUE iff the function is suspended
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    APLSTYPE     aplTypeObj;            // Object storage type
    STFLAGS      stFlags = {0};         // SymTab flags
    LPSYMENTRY   lpSymEntry;            // Ptr to STE for HGLOBAL
    LPWCHAR      lpwCharEnd;            // Temporary ptr
    APLLONGEST   aplLongestObj;         // Object immediate value
    LPAPLLONGEST lpaplLongestObj;       // Ptr to ...
    LPSYMENTRY   lpSymLink = NULL;      // Ptr to anchor of SYMENTRY links for [Globals] values
                                        //   so we may delete them easily
    struct _wfinddata_t
                 ws_file;               // Workspace filename struc
    APLI3264     hFile;                 // File handle
    WCHAR        wszDir  [_MAX_DIR],
                 wszDrive[_MAX_DRIVE],
                 wszDPFE [_MAX_PATH] = {WC_EOS},
                 wszDPFE2[_MAX_PATH];
    LPDICTIONARY lpDict = NULL;         // Ptr to workspace dictionary
    LPWCHAR      lpwszProf;             // Ptr to profile string
    VARS_TEMP_OPEN

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Save the bExecLX flag
    bExecLX = lpMemPTD->bExecLX;

    // Check for CLEAR WS
    if (hGlbDPFE EQ NULL)
    {
        // Set to an empty vector which also looks like a CLEAR WS
        lpwszDPFE = L"";

        goto WSID_EXIT;
    } // End IF

    // Lock the memory to get a ptr to it
    lpwszDPFE = MyGlobalLockWsz (hGlbDPFE);

    // Check for CLEAR WS
    if (lpwszDPFE[0] EQ WC_EOS)
        goto WSID_EXIT;

    // Check for "1 CLEANSPACE"
    if (lstrcmpiW (lpwszDPFE, L"1 CLEANSPACE") EQ 0)
    {
        // Display the workspace timestamp
        DisplaySavedMsg (cleanspaceTime, FALSE, NULL);

        goto WSID_EXIT;
    } // End IF

    // Attempt to open the workspace
    fStream = fopenW (lpwszDPFE, L"r");

    // If the workspace doesn't exist, ...
    if (fStream EQ NULL)
        goto WSNOTFOUND_EXIT;

    // We no longer need this resource
    fclose (fStream); fStream = NULL;

    // Find the actual (case-sensitive) filename.ext of this file
    hFile = _wfindfirst (lpwszDPFE, &ws_file);
    Assert (hFile NE -1L);
    _findclose (hFile); hFile = 0;

    // Split out the components of the original DPFE
    _wsplitpath (lpwszDPFE, wszDrive, wszDir, NULL, NULL);

    // Recombine using the actual (case-sensitive) workspace filename.ext
    _wmakepath (wszDPFE, wszDrive, wszDir, ws_file.name, NULL);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbDPFE); lpwszDPFE = NULL;

    // Make a backup copy of the workspace
    if (OptionFlags.bBackupOnLoad)
        MakeWorkspaceBackup (wszDPFE, LOADBAK_EXT);

    // Initialize the iniparser
    lpDict = ProfileLoad_EM (wszDPFE, &lpwErrMsg);
    if (lpDict EQ NULL)
        goto ERRMSG_EXIT;

    // Get the version #
    lpwszProf =
      ProfileGetString (SECTNAME_GENERAL,   // Ptr to the section name
                        KEYNAME_VERSION,    // Ptr to the key name
                        L"",                // Ptr to the default value
                        lpDict);            // Ptr to workspace dictionary
    // Copy the string to a save area
    // DO NOT USE lstrcpyW as it doesn't trigger a visible Page Fault
    MyStrcpyW (wszVersion, sizeof (wszVersion), lpwszProf);

    // Compare the version #s
    if (lstrcmpW (wszVersion, WS_VERSTR) > 0)
    {
        WCHAR wszTemp[1024];

        // Format the error message text
        MySprintfW (wszTemp,
                    sizeof (wszTemp),
                   L"The version of this workspace (%s) is later than the interpreter expects (%s)."
                   L"  Please try loading the workspace with a later version of the interpreter.",
                    wszVersion,
                    WS_VERSTR);
        // Tell the user the bad news
        MessageBoxW (hWndMF,
                     wszTemp,
                     WS_APPNAME,
                     MB_OK | MB_ICONSTOP);
        goto ERROR_EXIT;
    } // End IF

    // Get the SI Level
    uSILevel =
      ProfileGetInt (SECTNAME_GENERAL,      // Ptr to the section name
                     KEYNAME_SILEVEL,       // Ptr to the key name
                     0,                     // Default value if not found
                     lpDict);               // Ptr to workspace dictionary
    __try
    {
        // Mark as suspended to handle the initial case
        bSuspended = TRUE;

        // Loop through the SI levels
        for (uSID = 0; uSID <= uSILevel; uSID++)
        {
            //***************************************************************
            // If this isn't the first SI level, create a new SIS struc
            //***************************************************************
            if (0 < uSID)
            {
                LPWCHAR      lpwFcnName,            // Ptr to function name
                             lpwFcnLine;            // Ptr to function line #
                UINT         uLineNum;              // Function line #
                UBOOL        bSuspended;            // TRUE iff the function is suspended
                LPSYMENTRY   lpSymEntry;            // Ptr to function SYMENTRY
                HGLOBAL      hGlbDfnHdr;            // Defined function global memory handle
                LPDFN_HEADER lpMemDfnHdr;           // Ptr to user-defined function/operator header

                // If there are more SI levels and the previous level was suspended,
                //   create a new SIS struc for immediate execution mode
                if (uSID < uSILevel
                 && bSuspended)
                    // Fill in the SIS header for Immediate Execution Mode
                    FillSISNxt (lpMemPTD,               // Ptr to PerTabData global memory
                                NULL,                   // Semaphore handle
                                DFNTYPE_IMM,            // DfnType
                                FCNVALENCE_IMM,         // FcnValence
                                FALSE,                  // Suspended
                                FALSE,                  // Restartable
                                TRUE);                  // LinkIntoChain
                // Format the counter
                MySprintfW (wszCount,
                            sizeof (wszCount),
                           L"%d",
                            uSID - 1);
                // Read in the SI line
                lpwFcnName =
                  ProfileGetString (SECTNAME_SI,        // Ptr to the section name
                                    wszCount,           // Ptr to the key name
                                    L"",                // Ptr to the default value
                                    lpDict);            // Ptr to workspace dictionary
                // Convert the {name}s and other chars to UTF16_xxx
                (void) ConvertNameInPlace (lpwFcnName);

                // Find the trailing marker of the function name
                lpwFcnLine = strchrW (lpwFcnName, L'[');

                // Check for Quad suspension first
                if ((lpwFcnName[0] NE WC_EOS)
                 && ((lstrcmpW (lpwFcnName, WS_UTF16_QUAD ) EQ 0)
                  || (lstrcmpW (lpwFcnName, WS_UTF16_QUAD2) EQ 0)))
                {
                    // Fill in the SIS header for Quad Input Mode
                    FillSISNxt (lpMemPTD,               // Ptr to PerTabData global memory
                                NULL,                   // Semaphore handle
                                DFNTYPE_QUAD,           // DfnType
                                FCNVALENCE_NIL,         // FcnValence
                                TRUE,                   // Suspended
                                FALSE,                  // Restartable
                                TRUE);                  // LinkIntoChain
                    // If this is the last SI level, mark as not executing []LX
                    if (uSID EQ (uSILevel - 1))
                        lpMemPTD->bExecLX = FALSE;
                } else
                // If there's no left bracket, this SI level is uninteresting
                if (lpwFcnLine NE NULL)
                {
                    // Zap the trailing marker
                    *lpwFcnLine++ = WC_EOS;

                    // Get the function line #
                    sscanfW (lpwFcnLine,
                             L"%u",
                            &uLineNum);
                    // Set the flags for what we're looking up
                    ZeroMemory (&stFlags, sizeof (stFlags));
                    stFlags.Inuse   = TRUE;
                    stFlags.ObjName = OBJNAME_USR;

                    // Look up the function name
                    lpSymEntry =
                      SymTabLookupName (lpwFcnName, &stFlags);

////////////////////if (lpSymEntry EQ NULL)            // ***FIXME***

                    // Get a ptr to the function header
                    hGlbDfnHdr = lpSymEntry->stData.stGlbData;

                    // Lock the memory to get a ptr to it
                    lpMemDfnHdr = MyGlobalLockDfn (hGlbDfnHdr);

                    // Get suspended state
                    bSuspended = strchrW (lpwFcnLine, L'*') NE NULL;

                    // Fill in the SIS header for a User-Defined Function/Operator
                    FillSISNxt (lpMemPTD,                   // Ptr to PerTabData global memory
                                NULL,                       // Semaphore handle (Filled in by ExecuteFunction_EM_YY)
                                lpMemDfnHdr->DfnType,       // DfnType
                                lpMemDfnHdr->FcnValence,    // FcnValence
                                bSuspended,                 // TRUE iff suspended
                                FALSE,                      // Restartable
                                FALSE);                     // LinkIntoChain
                    // Fill in the non-default SIS header entries
                    lpMemPTD->lpSISNxt->hGlbDfnHdr   = hGlbDfnHdr;
                    lpMemPTD->lpSISNxt->hGlbFcnName  = lpMemDfnHdr->steFcnName->stHshEntry->htGlbName;
                    lpMemPTD->lpSISNxt->DfnAxis      = lpMemDfnHdr->DfnAxis;
                    lpMemPTD->lpSISNxt->bAFO         = lpMemDfnHdr->bAFO;
                    lpMemPTD->lpSISNxt->bMFO         = lpMemDfnHdr->bMFO;
                    lpMemPTD->lpSISNxt->CurLineNum   = uLineNum;
                    lpMemPTD->lpSISNxt->NxtLineNum   = uLineNum + 1;
                    lpMemPTD->lpSISNxt->numLabels    = lpMemDfnHdr->numLblLines;
                    lpMemPTD->lpSISNxt->numFcnLines  = lpMemDfnHdr->numFcnLines;
////////////////////lpMemPTD->lpSISNxt->lpSISNxt     =              // Filled in by LocalizeAll

                    dprintfWL9 (L"~~Localize:    %p (%s)", lpMemPTD->lpSISNxt, L"LoadWorkspace_EM");

                    lpMemPTD->lpSISCur               = lpMemPTD->lpSISNxt;

                    // Localize all arguments, results, and locals
                    LocalizeAll (lpMemPTD, lpMemDfnHdr);

                    // We no longer need this ptr
                    MyGlobalUnlock (hGlbDfnHdr); lpMemDfnHdr = NULL;
                } // End IF
            } // End IF

            //***************************************************************
            // Load the variables at this SI level
            //***************************************************************

            // Format the section name
            MySprintfW (wszSectName,
                        sizeof (wszSectName),
                        SECTNAME_VARS L".%d",
                        uSID);
            // Get the [Vars.nnn] count
            uSymVar =
              ProfileGetInt (wszSectName,   // Ptr to the section name
                             KEYNAME_COUNT, // Ptr to the key name
                             0,             // Default value if not found
                             lpDict);       // Ptr to workspace dictionary
            // Loop through the [Vars.sss] section where sss is the SI level
            for (uCnt = 0; uCnt < uSymVar; uCnt++)
            {
                LPWCHAR  lpwSrc,            // Ptr to incoming data
                         lpwSrcStart;       // Ptr to starting point
                APLI3264 iMaxSize;          // Maximum size of lpwSrc

                // Save ptr & maximum size
                lpwSrc   = lpMemPTD->lpwszTemp;
                CHECK_TEMP_OPEN
                iMaxSize = lpMemPTD->iTempMaxSize;

                // Save the starting point
                lpwSrcStart = lpwSrc;

                // Format the counter
                MySprintfW (wszCount,
                            sizeof (wszCount),
                           L"%d",
                            uCnt);
                // Read the next string
                lpwszProf =
                  ProfileGetString (wszSectName,    // Ptr to the section name
                                    wszCount,       // Ptr to the key name
                                    L"",            // Ptr to the default value
                                    lpDict);        // Ptr to workspace dictionary
                // Check for empty or missing counter
                if (*lpwszProf EQ WC_EOS)
                {
                    EXIT_TEMP_OPEN

                    continue;
                } // End IF

                // Copy the string to a save area
                // DO NOT USE lstrcpyW as it doesn't trigger a visible Page Fault
                strcpyW (lpwSrc, lpwszProf);

                // Look for the name separator (L'=')
                lpwCharEnd = strchrW (lpwSrc, L'=');
                Assert (lpwCharEnd NE NULL);

                // Zap to form zero-terminated name
                *lpwCharEnd = WC_EOS;

                // Convert the {name}s and other chars to UTF16_xxx
                lpwSrc = ConvertNameInPlace (lpwSrc);

                // Skip over the zapped L'='
                lpwSrc++;

                // Set the flags for what we're looking up/appending
                ZeroMemory (&stFlags, sizeof (stFlags));
                stFlags.Inuse   = TRUE;

                if (IsSysName (lpwSrcStart))
                    // Tell 'em we're looking for system names
                    stFlags.ObjName = OBJNAME_SYS;
                else
                    // Tell 'em we're looking for user names
                    stFlags.ObjName = OBJNAME_USR;

                // Lookup the name in the symbol table
                lpSymEntry =
                  SymTabLookupName (lpwSrcStart, &stFlags);

                // If the name is not found and is a system name, ...
                if (lpSymEntry EQ NULL
                 && stFlags.ObjName EQ OBJNAME_SYS)
                {
                    // Append the name as new to get a new LPSYMENTRY
                    lpSymEntry = SymTabAppendNewName_EM (lpwSrcStart, &stFlags);
                    if (lpSymEntry EQ NULL)
                        goto ETO_ERROR_EXIT;

                    // Set the common values
////////////////////lpSymEntry->stFlags.Imm        =                // Already set from stFlags
////////////////////lpSymEntry->stFlags.Value      = FALSE;         // ...
////////////////////lpSymEntry->stFlags.ObjName    = OBJNAME_SYS;   // ...
                    lpSymEntry->stFlags.stNameType = NAMETYPE_UNK;
////////////////////lpSymEntry->stData.stGlbData   = NULL;          // Already set from stFlags
                } else
                {
                    // If the name is not found, it must be a user name
                    Assert ((stFlags.ObjName EQ OBJNAME_USR) || lpSymEntry NE NULL);

                    // If the name is not found, append it as a user name
                    if (lpSymEntry EQ NULL)
                    {
                        // Append the name to get a new LPSYMENTRY
                        lpSymEntry = SymTabAppendName_EM (lpwSrcStart, &stFlags);
                        if (lpSymEntry EQ NULL)
                            goto ETO_ERROR_EXIT;

                        // Mark the SYMENTRY as immediate so we don't free the
                        //   (non-existant) stGlbData
                        // Set other flags as appropriate
                        lpSymEntry->stFlags.Imm        = TRUE;
                    } // End IF

                    // If the entry has not already been loaded or is a system name, ...
                    // This can happen if a previous Function Array included it as a global (:nnn).
                    if (!lpSymEntry->stFlags.Value
                     || lpSymEntry->stFlags.ObjName EQ OBJNAME_SYS)
                    {
                        // Clear so we save a clean value
                        aplLongestObj = 0;

                        // Set this value as lpaplLongestObj is incremented by ParseSavedWsVar_EM
                        lpaplLongestObj = &aplLongestObj;

                        // Parse the value into aplLongestObj and aplTypeObj
                        lpwSrc =
                          ParseSavedWsVar_EM (lpwSrc,           // Ptr to input buffer
                                               iMaxSize - (APLU3264) ((LPBYTE) lpwSrc - (LPBYTE) lpwSrcStart),  // Maximum size of lpwSrc
                                             &lpaplLongestObj,  // Ptr to ptr to output element
                                             &aplTypeObj,       // Ptr to storage type (may be NULL)
                                              lpSymEntry,       // Ptr to SYMENTRY of the source (may be NULL)
                                             &bImmed,           // Ptr to immediate flag (TRUE iff result is immediate) (may be NULL)
                                              FALSE,            // TRUE iff to save SymTabAppend values, FALSE to save values directly
                                              FALSE,            // TRUE iff this is called from )COPY
                                              hWndEC,           // Edit Ctrl window handle
                                             &lpSymLink,        // Ptr to ptr to SYMENTRY link
                                              wszVersion,       // Workspace version text
                                              lpDict,           // Ptr to workspace dictionary
                                             &lpwErrMsg);       // Ptr to ptr to (constant error message text)
                        if (lpwSrc EQ NULL)
                            goto ETO_ERRMSG_EXIT;

                        // Out with the old
                        // Release the current value of the STE
                        //   if it's a global and has a value
                        if (!lpSymEntry->stFlags.Imm
                         &&  lpSymEntry->stFlags.Value)
                        {
                            FreeResultGlobalVar (lpSymEntry->stData.stGlbData); lpSymEntry->stData.stGlbData = NULL;
                        } // End IF

                        // In with the new
                        if (bImmed)
                            lpSymEntry->stFlags.ImmType  = TranslateArrayTypeToImmType (aplTypeObj);
                        else
                            lpSymEntry->stFlags.ImmType  = IMMTYPE_ERROR;
                        // Set the common values
                        lpSymEntry->stFlags.Imm        = bImmed;
                        lpSymEntry->stFlags.Value      = TRUE;
                        lpSymEntry->stFlags.ObjName    = (IsSysName (lpwSrcStart) ? OBJNAME_SYS : OBJNAME_USR);
                        lpSymEntry->stFlags.stNameType = NAMETYPE_VAR;
                        lpSymEntry->stData.stLongest   = aplLongestObj;

                        // If this is a sys var, ...
                        if (lpSymEntry->stFlags.ObjName EQ OBJNAME_SYS)
                        {
                            TOKEN        tkNam = {0};
                            static WCHAR wszTemp[512];
                            LPWCHAR      lpMemName;

                            // Fill in the name token
                            tkNam.tkFlags.TknType   = TKT_VARNAMED;
////////////////////////////tkNam.tkFlags.ImmType   = IMMTYPE_ERROR;    // Unused
////////////////////////////tkNam.tkFlags.NoDisplay = FALSE;            // Already zero from = {0}
////////////////////////////tkNam.>tkCharIndex      =                   // Unused
                            tkNam.tkData.tkSym      = lpSymEntry;

                            // If the target is a user-defined function/operator system label, ...
                            if (tkNam.tkData.tkSym->stFlags.DfnSysLabel)
                            {
                                // Lock the memory to get a ptr to it
                                lpMemName = MyGlobalLockWsz (lpSymEntry->stHshEntry->htGlbName);

                                // Format the error message text
                                MySprintfW (wszTemp,
                                            sizeof (wszTemp),
                                           L"Error Assigning to SysLbl:  %s",
                                            lpMemName);
                                // We no longer need this ptr
                                MyGlobalUnlock (lpSymEntry->stHshEntry->htGlbName); lpMemName = NULL;

                                // Set the error message text
                                lpwErrMsg = &wszTemp[0];

                                goto ETO_ERRMSG_EXIT;
                            } // End IF

                            // If it's not []DM, ...
                            if (!IsSymSysName (lpSymEntry, $QUAD_DM))
                            {
                                // Validate the value
                                bRet = (*aSysVarValidSet[tkNam.tkData.tkSym->stFlags.SysVarValid]) (&tkNam, &tkNam);

                                // If it validated, ...
                                if (bRet)
                                    // Execute the post-validation function
                                    (*aSysVarValidPost[tkNam.tkData.tkSym->stFlags.SysVarValid]) (&tkNam);
                                else
                                {
                                    // Lock the memory to get a ptr to it
                                    lpMemName = MyGlobalLockWsz (lpSymEntry->stHshEntry->htGlbName);

                                    // Format the error message text
                                    MySprintfW (wszTemp,
                                                sizeof (wszTemp),
                                               L"Error validating SysVar:  %s",
                                                lpMemName);
                                    // We no longer need this ptr
                                    MyGlobalUnlock (lpSymEntry->stHshEntry->htGlbName); lpMemName = NULL;

                                    // Set the error message text
                                    lpwErrMsg = &wszTemp[0];

                                    goto ETO_ERRMSG_EXIT;
                                } // End IF
                            } // End IF
                        } // End IF
                    } // End IF
                } // End IF/ELSE

                // Restore the original value
                *lpwCharEnd = L'=';

                EXIT_TEMP_OPEN
            } // End FOR

            //***************************************************************
            // Load the functions at this SI level
            //***************************************************************

            // Format the section name
            MySprintfW (wszSectName,
                        sizeof (wszSectName),
                        SECTNAME_FCNS L".%d",
                        uSID);
            // Get the [Fcns.nnn] count
            uSymFcn =
              ProfileGetInt (wszSectName,   // Ptr to the section name
                             KEYNAME_COUNT, // Ptr to the key name
                             0,             // Default value if not found
                             lpDict);       // Ptr to workspace dictionary
            // Loop through the [Fcns.sss] section where sss is the SI level
            for (uCnt = 0; uCnt < uSymFcn; uCnt++)
            {
                LPWCHAR    lpwSrc,          // Ptr to incoming data
                           lpwSrcStart;     // Ptr to starting point
                APLI3264   iMaxSize;        // Maximum size of lpwSrc
                NAME_TYPES nameType;

                // Save ptr & maximum size
                lpwSrc   = lpMemPTD->lpwszTemp;
                CHECK_TEMP_OPEN
                iMaxSize = lpMemPTD->iTempMaxSize;

                // Save the starting point
                lpwSrcStart = lpwSrc;

                // Format the counter
                MySprintfW (wszCount,
                            sizeof (wszCount),
                           L"%d",
                            uCnt);
                // Read the next string
                lpwszProf =
                  ProfileGetString (wszSectName,    // Ptr to the section name
                                    wszCount,       // Ptr to the key name
                                    L"",            // Ptr to the default value
                                    lpDict);        // Ptr to workspace dictionary
                // Check for empty or missing counter
                if (*lpwszProf EQ WC_EOS)
                {
                    EXIT_TEMP_OPEN

                    continue;
                } // End IF

                // Copy the string to a save area
                // DO NOT USE lstrcpyW as it doesn't trigger a visible Page Fault
                strcpyW (lpwSrc, lpwszProf);

                // Look for the name separator (L'=')
                lpwCharEnd = strchrW (lpwSrc, L'=');
                Assert (lpwCharEnd NE NULL);

                // Zap to form zero-terminated name
                *lpwCharEnd = WC_EOS;

                // Convert the {name}s and other chars to UTF16_xxx
                lpwSrc = ConvertNameInPlace (lpwSrc);

                // Skip over the zapped L'='
                lpwSrc++;

                // Parse the name type (2 = FN0, 3 = FN12, 4 = OP1, 5 = OP2, 6 = OP3)
                nameType = *lpwSrc++ - '0';

                Assert (*lpwSrc EQ L'='); lpwSrc++;

                // Set the flags for what we're looking up
                ZeroMemory (&stFlags, sizeof (stFlags));
                stFlags.Inuse   = TRUE;
                stFlags.ObjName = OBJNAME_USR;

                // Lookup the name in the symbol table
                lpSymEntry =
                  SymTabLookupName (lpwSrcStart, &stFlags);

                // If the name is not found, it must be a user name
                Assert ((stFlags.ObjName EQ OBJNAME_USR) || lpSymEntry NE NULL);

                // If the name is not found, append it as a user name
                if (lpSymEntry EQ NULL)
                {
                    // Append the name to get a new LPSYMENTRY
                    lpSymEntry = SymTabAppendName_EM (lpwSrcStart, &stFlags);
                    if (lpSymEntry EQ NULL)
                        goto ETO_ERROR_EXIT;

                    // Set stFlags as appropriate
                    lpSymEntry->stFlags.ObjName    = OBJNAME_USR;
////////////////////lpSymEntry->stFlags.stNameType = nameType;          // Set in ParseSavedWsFcn_EM
                } // End IF

                // If the entry has already been loaded and isn't a system name, ...
                // This can happen if a previous Function Array included it as a global (:nnn).
                if (!lpSymEntry->stFlags.Value
                 || lpSymEntry->stFlags.ObjName EQ OBJNAME_SYS)
                {
                    LPWCHAR lpwszOrigTemp;                      // Original lpMemPTD->lpwszTemp

                    // Protect the text
                    lpwszOrigTemp = lpMemPTD->lpwszTemp;
                    lpMemPTD->lpwszTemp += lstrlenW (lpMemPTD->lpwszTemp);

                    EXIT_TEMP_OPEN

                    // Parse the line into lpSymEntry->stData
                    bRet = ParseSavedWsFcn_EM (lpwSrc,          // Ptr to input buffer
                                               iMaxSize - (APLU3264) ((LPBYTE) lpwSrc - (LPBYTE) lpwSrcStart),  // Maximum size of lpwSrc
                                               lpSymEntry,      // Ptr to STE for the object
                                               nameType,        // Function name type (see NAME_TYPES)
                                               hWndEC,          // Edit Ctrl window handle
                                              &lpSymLink,       // Ptr to ptr to SYMENTRY link
                                               wszVersion,      // Workspace version text
                                               lpDict,          // Ptr to workspace dictionary
                                              &lpwErrMsg);      // Ptr to ptr to (constant) error message text
                    // Restore the original ptr
                    lpMemPTD->lpwszTemp = lpwszOrigTemp;

                    CHECK_TEMP_OPEN

                    if (!bRet)
                        goto ETO_ERRMSG_EXIT;
                } // End IF

                EXIT_TEMP_OPEN
            } // End FOR
        } // End FOR
    } __except (CheckVirtAlloc (GetExceptionInformation (),
                                L"LoadWorkspace_EM"))
    {
        // Display message for unhandled exception
        DisplayException ();
    } // End __try/__except

    // Get [Globals] count
    uGlbCnt =
      ProfileGetInt (SECTNAME_GLOBALS,  // Ptr to the section name
                     KEYNAME_COUNT,     // Ptr to the key name
                     0,                 // Default value if not found
                     lpDict);           // Ptr to workspace dictionary
    // Delete the symbol table entries for vars/fcns we allocated of the form FMTSTR_GLBCNT
    DeleteGlobalLinks (lpSymLink);

    // Convert the given workspace name into a canonical form (without WS_WKSEXT)
    MakeWorkspaceNameCanonical (wszDPFE2, wszDPFE, NULL);

    // Display the indent
    AppendLine (wszIndent, FALSE, FALSE);

    // If we are loaded via )XLOAD, ...
    if (bExecLX)
        // Display the )LOAD command
        AppendLine (L")LOAD ", FALSE, FALSE);
    else
        // Display the )XLOAD command
        AppendLine (L")XLOAD ", FALSE, FALSE);

    // Display the workspace name
    AppendLine (wszDPFE2, FALSE, TRUE);

    // Display the workspace timestamp
    DisplayWorkspaceStamp (lpDict);
WSID_EXIT:
    // Save wszDPFE as the value of the new []WSID
    bRet = SaveNewWsid_EM (wszDPFE);

    goto NORMAL_EXIT;

WSNOTFOUND_EXIT:
    // Send this (constant) error message to the previously outgoing tab
    SendMessageLastTab (ERRMSG_WS_NOT_FOUND APPEND_NAME, lpMemPTD);

    goto ERROR_EXIT;

ETO_ERRMSG_EXIT:
    EXIT_TEMP_OPEN
ERRMSG_EXIT:
    // Send this (constant) error message to the previously outgoing tab
    SendMessageLastTab (lpwErrMsg, lpMemPTD);

    goto ERROR_EXIT;

ETO_ERROR_EXIT:
    EXIT_TEMP_OPEN
ERROR_EXIT:
NORMAL_EXIT:
    if (hGlbDPFE NE NULL && lpwszDPFE NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbDPFE); lpwszDPFE = NULL;
    } // End IF

    // If there's a dictionary, ...
    if (lpDict NE NULL)
    {
        // Free the dictionary
        ProfileUnload (lpDict); lpDict = NULL;
    } // End IF

    return bRet;
} // End LoadWorkspace_EM
#undef  APPEND_NAME


//***************************************************************************
//  $IsSymSysName
//
//  Is this SYMENTRY that of a particular SysName?
//***************************************************************************

UBOOL IsSymSysName
    (LPSYMENTRY lpSymEntry,
     LPWCHAR    wszSysName)

{
    LPWCHAR lpwGlbName;                 // Ptr to SymEntry's name's global memory
    UBOOL   bRet;                       // TRUE iff the result is valid

    // Lock the memory to get a ptr to it
    lpwGlbName = MyGlobalLockWsz (lpSymEntry->stHshEntry->htGlbName);

    // Compare the names
    bRet = lstrcmpiW (lpwGlbName, wszSysName) EQ 0;

    // We no longer need this ptr
    MyGlobalUnlock (lpSymEntry->stHshEntry->htGlbName); lpwGlbName = NULL;

    return bRet;
} // End IsSymSysname


//***************************************************************************
//  $ParseSavedWsFcn_EM
//
//  Parse a value of the form FMTSTR_GLBCNT or {name}...
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ParseSavedWsFcn_EM"
#else
#define APPEND_NAME
#endif

UBOOL ParseSavedWsFcn_EM
    (LPWCHAR       lpwSrc,              // Ptr to input buffer
     APLI3264      iMaxSize,            // Maximum size of lpwSrc
     LPSYMENTRY    lpSymObj,            // Ptr to STE for the object
     NAME_TYPES    nameType,            // Function name type (see NAME_TYPES)
     HWND          hWndEC,              // Edit Ctrl window handle
     LPSYMENTRY   *lplpSymLink,         // Ptr to ptr to SYMENTRY link
     LPWCHAR       lpwszVersion,        // Ptr to workspace version text
     LPDICTIONARY  lpDict,              // Ptr to workspace dictionary
     LPWCHAR      *lplpwErrMsg)         // Ptr to ptr to (constant) error message text

{
    WCHAR wcTmp;                        // Temporary char

    // Mark as a function/operator
    lpSymObj->stFlags.stNameType = nameType;

    if (*lpwSrc EQ FMTCHR_LEAD)
    {
        LPSYMENTRY lpSymEntry;          // Ptr to STE for HGLOBAL
        HGLOBAL    hGlbObj,             // Object global memory handle
                   hGlbOld;             // Old    ...
        LPWCHAR    lpwCharEnd,          // Temporary ptr
                   lpwDataEnd;          // ...
        STFLAGS    stFlags = {0};       // SymTab flags

        // Tell 'em we're looking for )LOAD objects
        stFlags.Inuse   = TRUE;
        stFlags.ObjName = OBJNAME_LOD;

        // Copy the old value
        hGlbOld = lpSymObj->stData.stGlbData;

        // If the old value is valid, ...
        if (hGlbOld NE NULL)
            // Increment its reference count to keep it around
            //   in case it gets freed by LoadWorkspaceGlobal_EM
            DbgIncrRefCntDir_PTB (hGlbOld); // EXAMPLE:  )copy tests\64bit lx

        // Find the trailing L' '
        lpwCharEnd = SkipToCharW (lpwSrc, L' ');
        lpwDataEnd = &lpwSrc[lstrlenW (lpwSrc)] + 1;

        // Save old next char, zap to form zero-terminated name
        wcTmp = *lpwCharEnd; *lpwCharEnd = WC_EOS;

        // Get the matching HGLOBAL
        lpSymEntry =
          SymTabLookupName (lpwSrc, &stFlags);

        // If it's not found or has no value, load it from the [Globals] section
        if (lpSymEntry EQ NULL || !lpSymEntry->stFlags.Value)
            hGlbObj =
              LoadWorkspaceGlobal_EM (lpwSrc,           // Ptr to keyname (FMTSTR_GLBCNT)
                                      lpwDataEnd,       // Ptr to next available byte
                                      iMaxSize - (APLU3264) ((LPBYTE) lpwDataEnd - (LPBYTE) lpwSrc),  // Maximum size of lpwSrc
                                      hWndEC,           // Edit Ctrl window handle
                                      NULL,             // Ptr to SYMENTRY of the source (may be NULL)
                                      lplpSymLink,      // Ptr to ptr to SYMENTRY link
                                      lpwszVersion,     // Ptr to workspace version text
                                      lpDict,           // Ptr to workspace dictionary
                                      lplpwErrMsg);     // Ptr to ptr to (constant) error message text
        else
            hGlbObj = lpSymEntry->stData.stGlbData;

        if (hGlbObj EQ NULL)
            goto CORRUPTWS_EXIT;

        // Restore the original value
        *lpwCharEnd = wcTmp;

        // Save in the result
        lpSymObj->stData.stGlbData = hGlbObj;

        // Increment the reference count
        DbgIncrRefCntDir_PTB (hGlbObj);

        // Transfer the STE values to the new STE

        // Split cases based upon the signature
        switch (GetSignatureGlb_PTB (hGlbObj))
        {
            LPDFN_HEADER lpMemDfnHdr;       // Ptr to user-defined function/operator header

            case DFN_HEADER_SIGNATURE:
                // Lock the memory to get a ptr to it
                lpMemDfnHdr = MyGlobalLockDfn (hGlbObj);

                // Mark as valued and user-defined function/operator
                lpSymObj->stFlags.Value  =
                lpSymObj->stFlags.UsrDfn = TRUE;

                // Copy the "Accepts Axis Operator" flag
                lpSymObj->stFlags.DfnAxis = lpMemDfnHdr->DfnAxis;

                // We no longer need this ptr
                MyGlobalUnlock (hGlbObj); lpMemDfnHdr = NULL;

                break;

            case FCNARRAY_HEADER_SIGNATURE:
                // Mark as valued
                lpSymObj->stFlags.Value  = TRUE;

                break;

            defstop
                break;
        } // End SWITCH

        // If there's an old value, ...
        if (hGlbOld NE NULL)
        {
            LPDFN_HEADER lpMemDfnHdr;   // Ptr to DFN_HEADER global memory

            // Lock the memory to get a ptr to it
            lpMemDfnHdr = MyGlobalLockDfn (hGlbOld);

            // Save the STE flags as the STE is still active
            lpMemDfnHdr->SaveSTEFlags = TRUE;

            // We no longer need this ptr
            MyGlobalUnlock (hGlbOld); lpMemDfnHdr = NULL;

            // Free the old value
            FreeResultGlobalDFLV (hGlbOld); hGlbOld = NULL;
        } // End IF
    } else
    {
        // Convert the single {name} or other char to UTF16_xxx
        if (L'{' EQ  *lpwSrc)
        {
            // Get the next char
            // Because we created this name, we can expect it to be found
            //   and so don't need to check for zero result
            wcTmp = SymbolNameToChar (lpwSrc);

            // Skip to the next field
            lpwSrc = SkipPastCharW (lpwSrc, L'}');
        } else
            wcTmp = *lpwSrc++;

        // Mark as immediate
        lpSymObj->stFlags.Imm = TRUE;

        // Mark as immediate type
        lpSymObj->stFlags.ImmType = TranslateNameTypeToImmType (nameType);

        // Save in the result
        lpSymObj->stData.stChar = wcTmp;
    } // End IF

    return TRUE;
CORRUPTWS_EXIT:
    *lplpwErrMsg = ERRMSG_CORRUPT_WS APPEND_NAME;

    return FALSE;
} // End ParseSavedWsFcn_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ParseSavedWsVar_EM
//
//  Parse a value of the form FMTSTR_GLBCNT or T N R S V
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ParseSavedWsVar_EM"
#else
#define APPEND_NAME
#endif

LPWCHAR ParseSavedWsVar_EM
    (LPWCHAR       lpwSrc,              // Ptr to input buffer
     APLI3264      iMaxSize,            // Maximum size of lpwSrc
     LPVOID       *lplpMemObj,          // Ptr to ptr to output element
     LPAPLSTYPE    lpaplTypeObj,        // Ptr to storage type (may be NULL)
     LPSYMENTRY    lpSymEntrySrc,       // Ptr to SYMENTRY of the source (may be NULL)
     LPUBOOL       lpbImmed,            // Ptr to immediate flag (TRUE iff result is immediate) (may be NULL)
     UBOOL         bSymTab,             // TRUE iff to save SymTabAppend values, FALSE to save values directly
     UBOOL         bCopyCmd,            // TRUE iff this is called from )COPY
     HWND          hWndEC,              // Edit Ctrl window handle
     LPSYMENTRY   *lplpSymLink,         // Ptr to ptr to SYMENTRY link
     LPWCHAR       lpwszVersion,        // Ptr to workspace version text
     LPDICTIONARY  lpDict,              // Ptr to workspace dictionary
     LPWCHAR      *lplpwErrMsg)         // Ptr to ptr to (constant error message text

{
    WCHAR        wcTmp,                 // Temporary char
                 wcQuote;               // Surrounding quote mark for a char
    LPWCHAR      lpwCharEnd,            // Temporary ptr
                 lpwDataEnd;            // ...
    STFLAGS      stFlags = {0};         // SymTab flags
    LPSYMENTRY   lpSymEntry,            // Ptr to STE for HGLOBAL
                 lpSymTmp;              // Ptr to temporary LPSYMENTRY
    APLINT       aplInteger;            // Temporary integer
    APLFLOAT     aplFloat;              // ...       float
    APLSTYPE     aplTypeObj;            // Object storage type
    HGLOBAL      hGlbObj;               // Object global memory handle
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPWCHAR      lpwszFormat;           // Ptr to formatting save area

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get ptr to formatting save area
    lpwszFormat = lpMemPTD->lpwszFormat;

    // Tell 'em we're looking for )LOAD objects
////ZeroMemory (&stFlags, sizeof (stFlags));
    stFlags.Inuse   = TRUE;
    stFlags.ObjName = OBJNAME_LOD;

    // If it's in the [Globals] section, ...
    if (*lpwSrc EQ FMTCHR_LEAD)
    {
        // Find the trailing L' '
        lpwCharEnd = SkipToCharW (lpwSrc, L' ');
        lpwDataEnd = &lpwSrc[lstrlenW (lpwSrc)] + 1;

        // Save old next char, zap to form zero-terminated name
        wcTmp = *lpwCharEnd; *lpwCharEnd = WC_EOS;

        // Get the matching HGLOBAL
        lpSymEntry =
          SymTabLookupName (lpwSrc, &stFlags);

        // If called from )COPY, it's not found, or has no value, load it from the [Globals] section
        if (bCopyCmd || lpSymEntry EQ NULL || !lpSymEntry->stFlags.Value)
        {
            hGlbObj =
              LoadWorkspaceGlobal_EM (lpwSrc,           // Ptr to keyname (FMTSTR_GLBCNT)
                                      lpwDataEnd,       // Ptr to next available byte
                                      iMaxSize - (APLU3264) ((LPBYTE) lpwDataEnd - (LPBYTE) lpwSrc),  // Maximum size of lpwSrc
                                      hWndEC,           // Edit Ctrl window handle
                                      lpSymEntrySrc,    // Ptr to SYMENTRY of the source (may be NULL)
                                      lplpSymLink,      // Ptr to ptr to SYMENTRY link
                                      lpwszVersion,     // Ptr to workspace version text
                                      lpDict,           // Ptr to workspace dictionary
                                      lplpwErrMsg);     // Ptr to ptr to (constant) error message text
            if (hGlbObj EQ NULL)
                goto ERROR_EXIT;
        } else
        {
            hGlbObj = lpSymEntry->stData.stGlbData; lpSymEntry = NULL;
            if (hGlbObj EQ NULL)
                goto CORRUPTWS_EXIT;
        } // End IF/ELSE

        // Restore the original value
        *lpwCharEnd = wcTmp;

        // Save in the result and skip over it
        *((LPAPLNESTED) *lplpMemObj)++ = hGlbObj;

        // Increment the reference count
        DbgIncrRefCntDir_PTB (hGlbObj);

        // Skip to the next field
        lpwSrc = &lpwCharEnd[1];

        // If the caller wants the storage type, ...
        if (lpaplTypeObj NE NULL)
        {
            LPVARARRAY_HEADER lpMemHdrObj = NULL;   // Ptr to object header

            // stData is a valid HGLOBAL variable array
            Assert (IsGlbTypeVarDir_PTB (hGlbObj));

            // Lock the memory to get a ptr to it
            lpMemHdrObj = MyGlobalLockVar (hGlbObj);

            // Save the storage type
#define lpHeader        lpMemHdrObj
            *lpaplTypeObj = lpHeader->ArrType;
#undef  lpHeader

            // We no longer need this ptr
            MyGlobalUnlock (hGlbObj); lpMemHdrObj = NULL;
        } // End IF

        // If the caller wants the immediate flag, ...
        if (lpbImmed NE NULL)
            // Save as not immediate
            *lpbImmed = FALSE;
    } else
    {
        // Get the storage type from the immediate char
        aplTypeObj = TranslateCharToArrayType (*lpwSrc);

        // If the caller wants the storage type, ...
        if (lpaplTypeObj NE NULL)
            // Save the storage type
            *lpaplTypeObj = aplTypeObj;

        // If the caller wants the immediate flag, ...
        if (lpbImmed NE NULL)
            // Save as immediate
            *lpbImmed = TRUE;

        // Skip to the NELM
        lpwSrc = SkipPastCharW (lpwSrc, L' ');

        Assert (*lpwSrc EQ L'1');

        // Skip to the rank
        lpwSrc = SkipPastCharW (lpwSrc, L' ');

        Assert (*lpwSrc EQ L'0');

        // Skip to the value
        lpwSrc = SkipPastCharW (lpwSrc, L' ');

        // Split cases based upon the storage type
        switch (aplTypeObj)
        {
            case ARRAY_BOOL:        // Boolean
            case ARRAY_INT:         // Integer
                // Scan in the value
                sscanfW (lpwSrc, SCANFSTR_APLINT, &aplInteger);

                // Skip to the next field
                lpwSrc = SkipPastCharW (lpwSrc, L' ');

                // If we're to save the SymTab, ...
                if (bSymTab)
                {
                    // Save in the result and skip over it
                    *((LPAPLHETERO) *lplpMemObj)++ =
                    lpSymTmp =
                      SymTabAppendInteger_EM (aplInteger, TRUE);
                    if (lpSymTmp EQ NULL)
                        goto ERROR_EXIT;
                } else
                    // Save the result directly
                    *((LPAPLINT) *lplpMemObj) = aplInteger;
                break;

            case ARRAY_CHAR:        // Character
                // Get the leading quote mark
                //   and skip over it
                wcQuote = *lpwSrc++;

                Assert (wcQuote EQ WC_SQ || wcQuote EQ WC_DQ);

                // Convert the single {name} or other char to UTF16_xxx
                if (L'{' EQ  *lpwSrc)
                {
                    // Get the next char
                    // Because we created this name, we can expect it to be found
                    //   and so don't need to check for zero result
                    wcTmp = SymbolNameToChar (lpwSrc);

                    // Skip to the next field
                    lpwSrc = SkipPastCharW (lpwSrc, L'}');
                } else
                    wcTmp = *lpwSrc++;

                Assert (wcQuote EQ *lpwSrc); lpwSrc++;

                // Skip to the next field
                lpwSrc = SkipPastCharW (lpwSrc, L' ');

                // If we're to save the SymTab, ...
                if (bSymTab)
                {
                    // Save in the result and skip over it
                    *((LPAPLHETERO) *lplpMemObj)++ =
                    lpSymTmp =
                      SymTabAppendChar_EM (wcTmp, TRUE);
                    if (lpSymTmp EQ NULL)
                        goto ERROR_EXIT;
                } else
                    // Save the result directly
                    *((LPAPLCHAR) *lplpMemObj) = wcTmp;
                break;

            case ARRAY_FLOAT:       // Float
                // Find the trailing L' '
                lpwCharEnd = SkipToCharW (lpwSrc, L' ');

                // Save old next char, zap to form zero-terminated name
                wcTmp = *lpwCharEnd; *lpwCharEnd = WC_EOS;

                // Convert the format string to ASCII
                W2A ((LPCHAR) lpwszFormat, lpwSrc, (DEF_WFORMAT_MAXNELM - 1) * sizeof (WCHAR));

                // Restore the original value
                *lpwCharEnd = wcTmp;

                // Check for positive infinity
                if (lstrcmp ((LPCHAR) lpwszFormat, TEXT_INFINITY) EQ 0)
                    // Save in the result and skip over it
                    aplFloat = fltPosInfinity;
                else
                // Check for negative infinity
                if (lstrcmp ((LPCHAR) lpwszFormat, "-" TEXT_INFINITY) EQ 0)
                    // Save in the result and skip over it
                    aplFloat = fltNegInfinity;
                else
                    aplFloat = MyStrtod ((LPCHAR) lpwszFormat, NULL);

                // If we're to save the SymTab, ...
                if (bSymTab)
                {
                    // Save in the result and skip over it
                    *((LPAPLHETERO) *lplpMemObj)++ =
                    lpSymTmp =
                      SymTabAppendFloat_EM (aplFloat);
                    if (lpSymTmp EQ NULL)
                        goto ERROR_EXIT;
                } else
                    // Save the result directly
                    *((LPAPLFLOAT) *lplpMemObj) = aplFloat;

                // Skip to the next field
                lpwSrc = &lpwCharEnd[1];

                break;

            defstop
                break;
        } // End FOR/SWITCH
    } // End IF

    return lpwSrc;

CORRUPTWS_EXIT:
    *lplpwErrMsg = ERRMSG_CORRUPT_WS APPEND_NAME;

    goto ERROR_EXIT;

ERROR_EXIT:
    return NULL;
} // End ParseSavedWsVar_EM
#undef  APPEND_NAME


//***************************************************************************
//  $LoadWorkspaceGlobal_EM
//
//  Return the global memory handle corresponding
//    to an entry in the [Globals] section
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- LoadWorkspaceGlobal_EM"
#else
#define APPEND_NAME
#endif

HGLOBAL LoadWorkspaceGlobal_EM
    (LPWCHAR      lpwGlbName,               // Ptr to keyname (FMTSTR_GLBCNT)
     LPWCHAR      lpwSrc,                   // Ptr to next available byte
     APLI3264     iMaxSize,                 // Maximum size of lpwSrc
     HWND         hWndEC,                   // Edit Ctrl window handle
     LPSYMENTRY   lpSymEntrySrc,            // Ptr to SYMENTRY of the source (may be NULL)
     LPSYMENTRY  *lplpSymLink,              // Ptr to ptr to SYMENTRY link
     LPWCHAR      lpwszVersion,             // Ptr to workspace version text
     LPDICTIONARY lpDict,                   // Ptr to workspace dictionary
     LPWCHAR     *lplpwErrMsg)              // Ptr to ptr to (constant) error message text

{
    APLSTYPE          aplTypeObj;           // Object storage type
    APLNELM           aplNELMObj;           // Object NELM
    APLRANK           aplRankObj;           // Object rank
    LPAPLDIM          lpMemDimObj;          // Ptr to object dimensions
    HGLOBAL           hGlbObj,              // Object global memory handle
                      hGlbDfnHdr,           // AFO global memory handle
                      hGlbChk;              // Result from CheckGlobals
    APLUINT           ByteObj,              // # bytes needed for the object
                      uObj;                 // Loop counter
    STFLAGS           stFlags = {0};        // SymTab flags
    LPSYMENTRY        lpSymEntry,           // Ptr to STE for HGLOBAL
                      lpSymLink;            // Ptr to SYMENTRY temp for *lplpSymLink
    WCHAR             wcTmp,                // Temporary char
                      wszTimeStamp[16 + 1]; // Output save area for time stamp
    LPWCHAR           lpwFcnName,           // Ptr to function name
                      lpwSectName,          // Ptr to section name
                      lpwSrcStart,          // Ptr to starting point
                      lpwCharEnd;           // Temporary ptr
    UINT              uBitIndex,            // Bit index for looping through Boolean result
                      uLineCnt,             // # lines in the current function including the header
                      uCnt,                 // Loop counter
                      Count;                // Temporary count for monitor info
    mp_prec_t         uDefPrec,             // Default precision to use when inputting VFP numbers
                      uCommPrec = 0,        // Common precision for array of VFP numbers
                      uOldPrec;             // Old precision to be restored later
    FILETIME          ftCreation,           // Function creation time
                      ftLastMod;            // ...      last modification time
    SYSTEMTIME        systemTime;           // Current system (UTC) time
    UBOOL             bUserDefined = FALSE, // TRUE iff the current function is User-Defined
                      bPermNdx     = FALSE, // ...          var is a permanent
                      bAFO         = FALSE, // TRUE iff the current function is an AFO
                      bQuadFEATURE;         // TRUE iff the symbol is []FEATURE
    LPVARARRAY_HEADER lpMemHdrObj = NULL;   // Ptr to the array header
    LPVOID            lpMemObj;             // Ptr to object global memory
    APLINT            aplInteger;           // Temporary integer
    LPPERTABDATA      lpMemPTD;             // Ptr to PerTabData global memory
    LPCHAR            lpFmt;                // Temporary ptr
    LPWCHAR           lpwszFormat,          // Ptr to formatting save area
                      lpwszProf,            // Ptr to profile string
                      lpwszOldTemp;         // Ptr to temporary save area

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get ptr to formatting save area
    lpwszFormat = lpMemPTD->lpwszFormat;

    // Save the starting ptr
    lpwSrcStart = lpwSrc;

    // Read the corresponding string from [Globals]
    lpwszProf =
      ProfileGetString (SECTNAME_GLOBALS,   // Ptr to the section name
                        lpwGlbName,         // Ptr to the key name
                        L"",                // Ptr to the default value
                        lpDict);            // Ptr to workspace dictionary
    // Parse the array attributes
    // The result in lpwszProf is
    //   V T N R S value    for variables
    //   F nnn.Name         for functions

    // Ensure it's non-empty
    if  (lpwszProf[0] EQ WC_EOS)
        goto CORRUPTWS_EXIT;

    // Split cases based upon Variable vs. Function/Operator
    switch (*lpwszProf++)
    {
        case L'V':
            Assert (*lpwszProf EQ L' '); lpwszProf++;

            // Get the object storage type
            aplTypeObj = TranslateCharToArrayType (*lpwszProf++);

            // Ensure there's a valid separator
            if (*lpwszProf NE L' ')
                goto CORRUPTWS_EXIT;

            // Skip over it
            lpwszProf++;

            // Get the object NELM
            sscanfW (lpwszProf, SCANFSTR_APLUINT, &aplNELMObj); lpwszProf = SkipBlackW (lpwszProf);

            // Ensure there's a valid separator
            if (*lpwszProf NE L' ')
                goto CORRUPTWS_EXIT;

            // Skip over it
            lpwszProf++;

            // Get the object rank
            sscanfW (lpwszProf, SCANFSTR_APLUINT, &aplRankObj); lpwszProf = SkipBlackW (lpwszProf);

            // Ensure there's a valid separator
            if (*lpwszProf NE L' ')
                goto CORRUPTWS_EXIT;

            // Skip over it
            lpwszProf++;

            //***************************************************************
            // Check for []FEATURE too short
            //***************************************************************
            if (lpSymEntrySrc NE NULL)
            {
                bQuadFEATURE = IsSymSysName (lpSymEntrySrc, $QUAD_FEATURE);

                if (bQuadFEATURE)
                    aplNELMObj = max (aplNELMObj, FEATURENDX_LENGTH);
            } else
                bQuadFEATURE = FALSE;

            //***************************************************************
            // Calculate space needed for the object
            //***************************************************************
            ByteObj = CalcArraySize (aplTypeObj, aplNELMObj, aplRankObj);

            //***************************************************************
            // Check for overflow
            //***************************************************************
            if (ByteObj NE (APLU3264) ByteObj)
                goto WSFULL_EXIT;

            //***************************************************************
            // Now we can allocate the storage for the result
            //***************************************************************
            hGlbObj = DbgGlobalAlloc (GHND, (APLU3264) ByteObj);
            if (hGlbObj EQ NULL)
                goto WSFULL_EXIT;

            // Lock the memory to get a ptr to it
            lpMemHdrObj = MyGlobalLock000 (hGlbObj);

#define lpHeader        lpMemHdrObj
            // Fill in the header
            lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
            lpHeader->ArrType    = aplTypeObj;
////////////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////////////lpHeader->SysVar     = FALSE;           // Already zero from GHND
            lpHeader->RefCnt     = 1;
            lpHeader->NELM       = aplNELMObj;
            lpHeader->Rank       = aplRankObj;

            // Skip over the header to the dimensions
            lpMemDimObj = lpMemObj = VarArrayBaseToDim (lpMemHdrObj);

            // Fill in the result's dimension
            for (uObj = 0; uObj < aplRankObj; uObj++)
            {
                // Scan in the next dimension
                sscanfW (lpwszProf, SCANFSTR_APLUINT, (LPAPLINT) lpMemObj);

                // Skip to the next field
                lpwszProf = SkipPastCharW (lpwszProf, L' ');

                // Skip over the dimension
                ((LPAPLDIM) lpMemObj)++;
            } // End FOR

            //***************************************************************
            // Check for []FEATURE too short
            //***************************************************************
            if (bQuadFEATURE)
                lpMemDimObj[0] = max (lpMemDimObj[0], FEATURENDX_LENGTH);

            // Clear all array properties
            lpHeader->PV0   =
            lpHeader->PV1   =
            lpHeader->All2s = FALSE;

            // Check for array properties
            if (*lpwszProf EQ L'(')
            {
#ifdef DEBUG
                APLCHAR wcChar = L'(';
#endif
                while (TRUE)
                {
                    // Skip over the separator (initial left paren or trailing blank)
                    Assert (*lpwszProf EQ wcChar); lpwszProf++;

                    if (strncmpW (lpwszProf, AP_PV0, strcountof (AP_PV0)) EQ 0)
                    {
                        lpHeader->PV0 = TRUE;
                        lpwszProf += strcountof (AP_PV0);
                    } else
                    if (strncmpW (lpwszProf, AP_PV1, strcountof (AP_PV1)) EQ 0)
                    {
                        lpHeader->PV1 = TRUE;
                        lpwszProf += strcountof (AP_PV1);
                    } else
                    if (strncmpW (lpwszProf, AP_ALL2S, strcountof (AP_ALL2S)) EQ 0)
                    {
                        lpHeader->All2s = TRUE;
                        lpwszProf += strcountof (AP_ALL2S);
                    } else
                    if (strncmpW (lpwszProf, AP_FPC, strcountof (AP_FPC)) EQ 0)
                    {
                        lpwszProf += strcountof (AP_FPC);

                        // If there's a common precision, ...
                        if (isdigit ((char) *lpwszProf))
                        {
                            // Scan the common precision
                            sscanfW (lpwszProf, L"%u", &uCommPrec);

                            // Skip over the digits
                            while (isdigit ((char) *lpwszProf))
                                lpwszProf++;
                        } // End IF
                    } else
                    {
                        MBC ("Unknown array property when loading workspace -- Load terminated.")

                        goto CORRUPTWS_EXIT;
                    } // End IF/ELSE/...

                    // Check for the end
                    if (*lpwszProf EQ L')')
                        break;
#ifdef DEBUG
                    // Set new char to test
                    wcChar = L' ';
#endif
                } // End WHILE

                // Skip over the trailing separator
                Assert (*lpwszProf EQ L')'); lpwszProf++;

                // Skip over the trailing blank
                Assert (*lpwszProf EQ L' '); lpwszProf++;
            } // End IF
#undef  lpHeader

            // lpwszProf now points to the data

            // Split cases based upon the object storage type
            switch (aplTypeObj)
            {
                case ARRAY_BOOL:
                    // Initialize the bit index
                    uBitIndex = 0;

                    // Loop through the elements
                    for (uObj = 0; uObj < aplNELMObj; uObj++)
                    {
                        // Scan in the next value
////////////////////////sscanfW (lpwszProf, SCANFSTR_APLUINT, &aplInteger);
                        aplInteger = _wtoi64 (lpwszProf);

                        // Skip to the next field
                        lpwszProf = SkipPastCharW (lpwszProf, L' ');

                        // Save in the result
                        *((LPAPLBOOL) lpMemObj) |= aplInteger << uBitIndex;

                        // Check for end-of-byte
                        if (++uBitIndex EQ NBIB)
                        {
                            uBitIndex = 0;              // Start over
                            ((LPAPLBOOL) lpMemObj)++;   // Skip to next byte
                        } // End IF
                    } // End FOR

                    break;

                case ARRAY_INT:
                    // Loop through the elements
                    for (uObj = 0; uObj < aplNELMObj; uObj++)
                    {
                        // Scan in the next value and skip over it
////////////////////////sscanfW (lpwszProf, SCANFSTR_APLINT, ((LPAPLINT) lpMemObj)++);
                        *((LPAPLINT) lpMemObj)++ = _wtoi64 (lpwszProf);

                        // Skip to the next field
                        lpwszProf = SkipPastCharW (lpwszProf, L' ');
                    } // End FOR

                    break;

                case ARRAY_FLOAT:
                    // Loop through the elements
                    for (uObj = 0; uObj < aplNELMObj; uObj++)
                    {
                        // Find the trailing L' '
                        lpwCharEnd = SkipToCharW (lpwszProf, L' ');

                        // Save old next char, zap to form zero-terminated name
                        wcTmp = *lpwCharEnd; *lpwCharEnd = WC_EOS;

                        // Convert the format string to ASCII
                        W2A ((LPCHAR) lpwszFormat, lpwszProf, (DEF_WFORMAT_MAXNELM - 1) * sizeof (WCHAR));

                        // Restore the original value
                        *lpwCharEnd = wcTmp;

                        // Check for positive infinity
                        if (lstrcmp ((LPCHAR) lpwszFormat, TEXT_INFINITY) EQ 0)
                            // Save in the result and skip over it
                            *((LPAPLFLOAT) lpMemObj)++ = fltPosInfinity;
                        else
                        // Check for negative infinity
                        if (lstrcmp ((LPCHAR) lpwszFormat, "-" TEXT_INFINITY) EQ 0)
                            // Save in the result and skip over it
                            *((LPAPLFLOAT) lpMemObj)++ = fltNegInfinity;
                        else
                            // Use David Gay's routines
                            // Save in the result and skip over it
                            *((LPAPLFLOAT) lpMemObj)++ = MyStrtod ((LPCHAR) lpwszFormat, NULL);
                        // Skip to the next field
                        lpwszProf = SkipPastCharW (lpwszProf, L' ');
                    } // End FOR

                    break;

                case ARRAY_CHAR:
                    Assert (WC_SQ EQ *lpwszProf); lpwszProf++;

                    // Loop through the elements
                    for (uObj = 0; uObj < aplNELMObj; uObj++)
                    {
                        // Convert the single {name} or other char to UTF16_xxx
                        if (L'{' EQ  *lpwszProf)
                        {
                            // Get the next char
                            // Because we created this name, we can expect it to be found
                            //   and so don't need to check for zero result
                            wcTmp = SymbolNameToChar (lpwszProf);

                            // Skip to the next field
                            lpwszProf = SkipPastCharW (lpwszProf, L'}');
                        } else
                            wcTmp = *lpwszProf++;

                        // Save in the result and skip over it
                        *((LPAPLCHAR) lpMemObj)++ = wcTmp;
                    } // End FOR

                    Assert (WC_SQ EQ *lpwszProf); lpwszProf++;

                    break;

                case ARRAY_APA:
                    // The next two values are the APA offset and multiplier

                    // Scan in the next value and skip over it
                    sscanfW (lpwszProf, SCANFSTR_APLINT, ((LPAPLINT) lpMemObj)++);

                    // Skip to the next field
                    lpwszProf = SkipPastCharW (lpwszProf, L' ');

                    // Scan in the next value and skip over it
                    sscanfW (lpwszProf, SCANFSTR_APLINT, ((LPAPLINT) lpMemObj)++);

                    // Skip to the next field
                    lpwszProf = SkipPastCharW (lpwszProf, L' ');

                    break;

                case ARRAY_NESTED:
                    // Handle empty case
                    aplNELMObj = max (aplNELMObj, 1);

                    // Fall through to common code

                case ARRAY_HETERO:
                    // The elements consist of either a simple scalar
                    //   (starts with one of the storage types), or a
                    //   global (starts with a FMTCHR_LEAD)

                    // Loop through the elements
                    for (uObj = 0; uObj < aplNELMObj; uObj++)
                        lpwszProf =
                          ParseSavedWsVar_EM (lpwszProf,    // Ptr to input buffer
                                              iMaxSize - (APLU3264) ((LPBYTE) lpwSrc - (LPBYTE) lpwSrcStart),  // Maximum size of lpwSrc
                                             &lpMemObj,     // Ptr to ptr to output element
                                              NULL,         // Ptr to storage type (may be NULL)
                                              NULL,         // Ptr to immediate flag (TRUE iff result is immediate) (may be NULL)
                                              NULL,         // Ptr to SYMENTRY of the source (may be NULL)
                                              TRUE,         // TRUE iff to save SymTabAppend values, FALSE to save values directly
                                              FALSE,        // TRUE iff this is called from )COPY
                                              hWndEC,       // Edit Ctrl window handle
                                              lplpSymLink,  // Ptr to ptr to SYMENTRY link
                                              lpwszVersion, // Ptr to workspace version text
                                              lpDict,       // Ptr to workspace dictionary
                                              lplpwErrMsg); // Ptr to ptr to (constant error message text
                    break;

                case ARRAY_RAT:
                    // Loop through the elements
                    for (uObj = 0; uObj < aplNELMObj; uObj++)
                    {
                        LPWCHAR lpwWS,
                                lpwStr;
                        LPCHAR  lpStr;

                        // Skip to the next white space
                        lpwWS = SkipToCharW (lpwszProf, L' ');

                        // Convert it to a WC_EOS
                        wcTmp = *lpwWS; *lpwWS = WC_EOS;

                        // Initialize the save area
                        mpq_init ((LPAPLRAT) lpMemObj);

                        // Convert the string from WCHAR to char
                        lpwStr = lpwszProf; lpStr = (LPCHAR) lpwStr;
                        while (*lpwStr)
                            *lpStr++ = (char) *lpwStr++;
                        *lpStr = AC_EOS;

                        // Restore the original value
                        *lpwWS = wcTmp;

                        // Check for positive infinity
                        if (lstrcmp ((LPCHAR) lpwszProf, TEXT_INFINITY) EQ 0)
                            mpq_set_inf (((LPAPLRAT) lpMemObj)++, 1);
                        else
                        // Check for negative infinity
                        if (lstrcmp ((LPCHAR) lpwszProf, "-" TEXT_INFINITY) EQ 0)
                            mpq_set_inf (((LPAPLRAT) lpMemObj)++, -1);
                        else
                            // Convert the string to rational
                            mpq_set_str (((LPAPLRAT) lpMemObj)++, (LPCHAR) lpwszProf, 10);

                        // Skip to the next field
                        lpwszProf = SkipPastCharW (lpwWS, L' ');
                    } // End FOR

                    break;

                case ARRAY_VFP:
                    // Get the current precision
                    uOldPrec = mpfr_get_default_prec ();

                    // Loop through the elements
                    for (uObj = 0; uObj < aplNELMObj; uObj++)
                    {
                        // Skip to the next white space
                        lpwCharEnd = SkipToCharW (lpwszProf, L' ');

                        // Convert it to a WC_EOS
                        wcTmp = *lpwCharEnd; *lpwCharEnd = WC_EOS;

                        // If there's a preceding FPC as (nnn), ...
                        if (*lpwszProf EQ L'(')
                        {
                            // Skip over the leading paren
                            Assert (*lpwszProf EQ L'('); lpwszProf++;

                            // Scan the new precision
                            sscanfW (lpwszProf, L"%u", &uDefPrec);

                            // Set the default precision
                            mpfr_set_default_prec (uDefPrec);

                            // Skip past the trailing paren
                            lpwszProf = SkipPastCharW (lpwszProf, L')');
                        } else
                            // Set the default precision
                            mpfr_set_default_prec ((uCommPrec EQ 0) ? uOldPrec : uCommPrec);

                        // Convert the format string to ASCII
                        W2A (lpFmt = (LPCHAR) lpwszFormat, lpwszProf, (DEF_WFORMAT_MAXNELM - 1) * sizeof (WCHAR));

                        // Restore the original value
                        *lpwCharEnd = wcTmp;

                        // Initialize the save area to 0 using the precision set above
                        mpfr_init0 ((LPAPLVFP) lpMemObj);

                        // Check for positive infinity
                        if (lstrcmp (lpFmt, TEXT_INFINITY) EQ 0)
                            mpfr_set_inf (((LPAPLVFP) lpMemObj)++, 1);
                        else
                        // Check for negative infinity
                        if (lstrcmp (lpFmt, "-" TEXT_INFINITY) EQ 0)
                            mpfr_set_inf (((LPAPLVFP) lpMemObj)++, -1);
                        else
                            // Convert the string to VFP
                            mpfr_set_str (((LPAPLVFP) lpMemObj)++, lpFmt, 10, MPFR_RNDN);

                        // Skip to the next field
                        lpwszProf = SkipPastCharW (lpwszProf, L' ');
                    } // End FOR

                    // Restore the default precision
                    mpfr_set_default_prec (uOldPrec);

                    break;

                defstop
                    break;
            } // End SWITCH

            // We no longer need this ptr
            MyGlobalUnlock (hGlbObj); lpMemHdrObj = NULL;

            // Check to see if this value duplicates one of the already
            //   allocated permanent globals
            hGlbChk = CheckGlobals (hGlbObj);
            if (hGlbChk NE NULL)
            {
                LPVARARRAY_HEADER lpMemHdrChk = NULL;

                // We no longer need this storage
                FreeResultGlobalVar (hGlbObj); hGlbObj = NULL;

                // Copy the global memory handle
                hGlbObj = CopySymGlbDirAsGlb (hGlbChk);

                // Lock the memory to get a ptr to it
                lpMemHdrChk = MyGlobalLockVar (hGlbChk);

                // Mark if this is a PERMNDX_xxx value
                bPermNdx = (lpMemHdrChk->PermNdx NE PERMNDX_NONE);

                // We no longer need this ptr
                MyGlobalUnlock (hGlbChk); lpMemHdrChk = NULL;
            } // End IF

            // Set the ptr type bits
            hGlbObj = MakePtrTypeGlb (hGlbObj);

            break;

        case L'F':
            // Copy the string to a save area
            // DO NOT USE lstrcpyW as it doesn't trigger a visible Page Fault
            strcpyW (lpwSrc, lpwszProf);

            Assert (*lpwSrc EQ L' '); lpwSrc++;

            // Point to the section name of the form nnn.Name where nnn is the count
            lpwSectName = lpwSrc;

            // Get a ptr to the function name
            lpwFcnName = SkipPastCharW (lpwSectName, L'.');

            // Skip past the section name and its terminating zero
            lpwSrc = &lpwSectName[lstrlenW (lpwSectName) + 1];

            // Copy the function name past the end of the section name
            strcpyW (lpwSrc, lpwFcnName);

            // Point to the function name
            lpwFcnName = lpwSrc;

            // Convert the {name}s and other chars to UTF16_xxx
            (void) ConvertNameInPlace (lpwFcnName);

            // Skip past the converted function name and its terminating zero
            lpwSrc = &lpwSrc[lstrlenW (lpwSrc) + 1];

            // Get the count for the section name
            uLineCnt =
              ProfileGetInt (lpwSectName,   // Ptr to the section name
                             KEYNAME_COUNT, // Ptr to the key name
                             0,             // Default value if not found
                             lpDict);       // Ptr to workspace dictionary
            Assert (uLineCnt > 0);

            // Get the UserDefined flag
            bUserDefined =
              ProfileGetBoolean (lpwSectName,           // Ptr to the section name
                                 KEYNAME_USERDEFINED,   // Ptr to the key name
                                 0,                     // Default value if not found
                                 lpDict);               // Ptr to workspace dictionary
            // Get the AFO flag
            bAFO       =
              ProfileGetBoolean (lpwSectName,           // Ptr to the section name
                                 KEYNAME_AFO,           // Ptr to the key name
                                 0,                     // Default value if not found
                                 lpDict);               // Ptr to workspace dictionary
            // Get the current system (UTC) time
            GetSystemTime (&systemTime);

            // Convert system time to file time and save as creation time
            SystemTimeToFileTime (&systemTime, &ftCreation);

            // Format the CreationTime/LastModTime
            MySprintfW (wszTimeStamp,
                        sizeof (wszTimeStamp),
                        FMTSTR_DATETIME,
                        ftCreation.dwHighDateTime,
                        ftCreation.dwLowDateTime);
            // Get the CreationTime string
            lpwszProf =
              ProfileGetString (lpwSectName,            // Ptr to the section name
                                KEYNAME_CREATIONTIME,   // Ptr to the key name
                                wszTimeStamp,           // Ptr to the default value
                                lpDict);                // Ptr to workspace dictionary
            // Convert the CreationTime string to time
            sscanfW (lpwszProf, SCANFSTR_TIMESTAMP, (LPAPLINT) &ftCreation);

            // Get the LastModTime string
            lpwszProf =
              ProfileGetString (lpwSectName,            // Ptr to the section name
                                KEYNAME_LASTMODTIME,    // Ptr to the key name
                                wszTimeStamp,           // Ptr to the default value
                                lpDict);                // Ptr to workspace dictionary
            // Convert the LastModTime string to time
            sscanfW (lpwszProf, SCANFSTR_TIMESTAMP, (LPAPLINT) &ftLastMod);

            // If it's a user-defined or AFO, ...
            if (bUserDefined
             || bAFO)
            {
                SF_FCNS      SF_Fcns = {0};         // Common struc for SaveFunctionCom
                LW_PARAMS    LW_Params = {0};       // Local  ...

                // Get the Undo buffer string
                lpwszProf =
                  ProfileGetString (lpwSectName,    // Ptr to the section name
                                    KEYNAME_UNDO,   // Ptr to the key name
                                    L"",            // Ptr to the default value
                                    lpDict);        // Ptr to workspace dictionary
                // Fill in common values
                SF_Fcns.bDisplayErr     = TRUE;             // Display Errors
                SF_Fcns.bAFO            = bAFO;             // Parsing an AFO
////////////////SF_Fcns.bRet            =                   // Filled in by SaveFunctionCom
////////////////SF_Fcns.uErrLine        =                   // ...
////////////////SF_Fcns.lpSymName       =                   // ...
                SF_Fcns.lptkFunc        = NULL;             // Ptr to function token ***FIXME*** -- Used on error
                SF_Fcns.SF_LineLen      = SF_LineLenLW;     // Ptr to line length function
                SF_Fcns.SF_ReadLine     = SF_ReadLineLW;    // Ptr to read line function
                SF_Fcns.SF_IsLineCont   = SF_IsLineContLW;  // Ptr to Is Line Continued function
                SF_Fcns.SF_NumPhyLines  = SF_NumPhyLinesLW; // Ptr to get # physical lines function
                SF_Fcns.SF_NumLogLines  = SF_NumLogLinesLW; // Ptr to get # logical  ...
                SF_Fcns.SF_CreationTime = SF_CreationTimeLW;// Ptr to get function creation time
                SF_Fcns.SF_LastModTime  = SF_LastModTimeLW; // Ptr to get function last modification time
                SF_Fcns.SF_UndoBuffer   = SF_UndoBufferLW;  // Ptr to get function Undo Buffer global memory handle
////////////////SF_Fcns.numLocalsSTE    = 0;                // # locals in AFO (Already zero from = {0})
////////////////SF_Fcns.lplpLocalSTEs   = NULL;             // Ptr to save area for local STEs          (Already zero from = {0})
                SF_Fcns.LclParams       = &LW_Params;       // Ptr to local parameters
                SF_Fcns.sfTypes         = SFTYPES_LOAD;     // Ptr to local parameters

                // Fill in local values
                LW_Params.lpwSectName   = lpwSectName;      // Ptr to section name
                LW_Params.lpDict        = lpDict;           // Ptr to workspace dictionary
                LW_Params.lpwBuffer     = lpwSrc + 1;       // Ptr to buffer
                LW_Params.lpMemUndoTxt  = lpwszProf;        // Ptr to Undo Buffer in text format
                LW_Params.iMaxSize      = iMaxSize - ((LPBYTE) lpwSrc - (LPBYTE) lpwSrcStart); // Maximum size of lpwSrc
                LW_Params.ftCreation    = ftCreation;       // Function Creation Time
                LW_Params.ftLastMod     = ftLastMod;        // Function Last Modification Time
                LW_Params.lpwszVersion  = lpwszVersion;     // Workspace version #

                // Call common routine
                if (!SaveFunctionCom (NULL, &SF_Fcns))
                {
                    // Format the error message
                    wsprintfW (lpwszFormat,
                               L"Error loading function <%s> line <%d> in workspace <%s> -- WORKSPACE MAY BE CORRUPT:  PROCEED WITH CAUTION",
                               lpwFcnName,
                               SF_Fcns.uErrLine,
                               lpDict->lpwszDPFE);
                    MBW (lpwszFormat);

                    goto CORRUPTWS_EXIT;
                } // End IF/ELSE

                // Save the function/operator's global memory handle
                hGlbDfnHdr = SF_Fcns.hGlbDfnHdr;

                // Read in and process the monitor info
                lpwszProf =
                  ProfileGetString (lpwSectName,        // Ptr to the section name
                                    KEYNAME_MONINFO,    // Ptr to the key name
                                    L"",                // Ptr to the default value
                                    lpDict);            // Ptr to workspace dictionary
                // If there's monitor info, ...
                if (lpwszProf[0])
                {
                    LPDFN_HEADER lpMemDfnHdr;           // Ptr to user-defined function/operator header
                    LPINTMONINFO lpMemMonInfo;          // Ptr to function line monitoring info

                    // Lock the memory to get a ptr to it
                    lpMemDfnHdr = MyGlobalLockDfn (SF_Fcns.hGlbDfnHdr);

                    // Allocate space for the monitor info
                    lpMemDfnHdr->hGlbMonInfo =
                      DbgGlobalAlloc (GHND, (lpMemDfnHdr->numFcnLines + 1) * sizeof (INTMONINFO));
                    if (lpMemDfnHdr->hGlbMonInfo)
                    {
                        // Lock the memory to get a ptr to it
                        lpMemMonInfo = MyGlobalLock000 (lpMemDfnHdr->hGlbMonInfo);

                        // Loop through the function header & lines
                        for (uCnt = 0; uCnt < uLineCnt; uCnt++, lpMemMonInfo++)
                        {
                            // Scan in the first field
                            sscanfW (lpwszProf,
                                     L"%I64u %I64u %u",
                                    &lpMemMonInfo->IncSubFns,
                                    &lpMemMonInfo->ExcSubFns,
                                    &Count);
                            // Save the count
                            lpMemMonInfo->Count = Count;

                            // Increment past the input fields
                            lpwszProf = SkipPastCharW (lpwszProf, L',');
                        } // End FOR

                        // We no longer need this ptr
                        MyGlobalUnlock (lpMemDfnHdr->hGlbMonInfo); lpMemMonInfo = NULL;

                        // Mark as monitor info present
                        lpMemDfnHdr->MonOn = TRUE;
                    } // End IF

                    // We no longer need this ptr
                    MyGlobalUnlock (SF_Fcns.hGlbDfnHdr); lpMemDfnHdr = NULL;
                } // End IF
            } else
            // It's a function array
            {
                LPWCHAR          lpwLine;           // Ptr to line to execute
                LPPERTABDATA     lpMemPTD;          // Ptr to PerTabData global memory
                LOADWSGLBVARPARM LoadWsGlbVarParm;  // Extra parms for LoadWsGlbVarConv
                VARS_TEMP_OPEN
#ifdef DEBUG
                EXIT_TYPES exitType;
#endif
                // Append the variable name and an assignment arrow to the output buffer
                lpwLine = lpwSrc;
                strcpyW (lpwLine, lpwFcnName);
                lpwSrc += lstrlenW (lpwSrc);
                *lpwSrc++ = UTF16_LEFTARROW;

                // Get the one (and only) line
                lpwszProf =
                  ProfileGetString (lpwSectName,    // Ptr to the section name
                                    L"0",           // Ptr to the key name
                                    L"",            // Ptr to the default value
                                    lpDict);        // Ptr to workspace dictionary
                // Copy to the save area ("+ 1" to include the trailing zero)
                // DO NOT USE lstrcpyW as it doesn't trigger a visible Page Fault
                strcpyW (lpwSrc, lpwszProf);

////////////////// Convert in place
////////////////lpwSrcStart = lpwSrc;

                // Convert the {name}s and other chars to UTF16_xxx
                (void) ConvertNameInPlace (lpwSrc);

                // Fill in the extra parms
                LoadWsGlbVarParm.lpwSrc        = &lpwSrc[lstrlenW (lpwSrc) + 1];    // "+ 1" to skip over WC_EOS
                LoadWsGlbVarParm.iMaxSize      = iMaxSize - ((LPBYTE) lpwSrc - (LPBYTE) lpwSrcStart); // Maximum size of lpwSrc
                LoadWsGlbVarParm.hWndEC        = hWndEC;
                LoadWsGlbVarParm.lplpSymLink   = lplpSymLink;
                LoadWsGlbVarParm.lpwszVersion  = lpwszVersion;
                LoadWsGlbVarParm.lpDict        = lpDict;
                LoadWsGlbVarParm.lplpwErrMsg   = lplpwErrMsg;

                // Get ptr to PerTabData global memory
                lpMemPTD = GetMemPTD ();

                // Save in PerTabData struc
                lpMemPTD->lpLoadWsGlbVarParm = &LoadWsGlbVarParm;
                lpMemPTD->lpLoadWsGlbVarConv = &LoadWsGlbVarConv;

                // Protect lpMemPTD->lpwszTemp
                lpwszOldTemp        = lpMemPTD->lpwszTemp;
                CHECK_TEMP_OPEN
                EXIT_TEMP_OPEN
                lpMemPTD->lpwszTemp = LoadWsGlbVarParm.lpwSrc;

                // Execute the statement
#ifdef DEBUG
                exitType =
#endif
                PrimFnMonUpTackJotCSPLParse (hWndEC,                // Edit Ctrl window handle
                                             lpMemPTD,              // Ptr to PerTabData global memory
                                             lpwLine,               // Ptr to text of line to execute
                                             lstrlenW (lpwLine),    // Length of the line to execute
                                             TRUE,                  // TRUE iff we should act on errors
                                             FALSE,                 // TRUE iff we're to skip the depth check
                                             DFNTYPE_EXEC,          // DfnType for FillSISNxt
                                             NULL);                 // Ptr to function token
                Assert (exitType EQ EXITTYPE_NOVALUE);

                // Restore lpMemPTD->lpwszTemp
                lpMemPTD->lpwszTemp = lpwszOldTemp;

                // Save in PerTabData struc
                lpMemPTD->lpLoadWsGlbVarParm = NULL;
                lpMemPTD->lpLoadWsGlbVarConv = NULL;
            } // End IF/ELSE

            // Lookup the STE and get its HGLOBAL
            //   so we can save it in the STE for FMTSTR_GLBCNT

            // Set the flags for what we're looking up
            ZeroMemory (&stFlags, sizeof (stFlags));
            stFlags.Inuse   = TRUE;
            stFlags.ObjName = OBJNAME_USR;

            // Lookup the name in the symbol table
            lpSymEntry =
              SymTabLookupName (lpwFcnName, &stFlags);
            Assert (lpSymEntry NE NULL);

            // If it's an AFO, ...
            if (bAFO)
            {
                // Copy the HGLOBAL
                hGlbObj = MakePtrTypeGlb (hGlbDfnHdr);

                // Set flags
                lpSymEntry->stFlags.Value  =
                lpSymEntry->stFlags.UsrDfn = TRUE;
            } else
                // Copy the HGLOBAL
                hGlbObj = lpSymEntry->stData.stGlbData;

            // Ensure it's a function/operator/train.
            Assert (IsNameTypeFnOp (lpSymEntry->stFlags.stNameType));

            break;

        defstop
            break;
    } // End SWITCH

    // Set the flags for what we're appending
    ZeroMemory (&stFlags, sizeof (stFlags));
    stFlags.Inuse   =
    stFlags.Value   = TRUE;
    stFlags.ObjName = OBJNAME_LOD;

    // Create a symbol table entry for the )LOAD HGLOBAL
    lpSymEntry =
      SymTabAppendName_EM (lpwGlbName, &stFlags);
    if (lpSymEntry EQ NULL)
        goto ERROR_EXIT;

    // Set the handle
    lpSymEntry->stData.stGlbData = hGlbObj;

    // Link this SYMENTRY into the chain
    //   unless it's already linked
    //   or is a Permanent var
    if (!bPermNdx
     && lpSymEntry->stSymLink EQ NULL)
    {
        lpSymLink             = *lplpSymLink;
       *lplpSymLink           =  lpSymEntry;
        lpSymEntry->stSymLink =  lpSymLink;
    } // End IF

    goto NORMAL_EXIT;

CORRUPTWS_EXIT:
    *lplpwErrMsg = ERRMSG_CORRUPT_WS APPEND_NAME;

    goto ERROR_EXIT;

WSFULL_EXIT:
    *lplpwErrMsg = ERRMSG_WS_FULL APPEND_NAME;

    goto ERROR_EXIT;

ERROR_EXIT:
    hGlbObj = NULL;
NORMAL_EXIT:
    return hGlbObj;
} // End LoadWorkspaceGlobal_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CheckGlobals
//
//  Compare a global memory handle against permanent globals
//    to avoid duplicates
//***************************************************************************

HGLOBAL CheckGlobals
    (HGLOBAL hGlbObj)                   // Incoming global memory handle

{
    // Compare the incoming handle against the permanent globals
    if (CompareGlobals (hGlbObj, hGlbQuadFC_SYS))
        // Copy the global memory handle
        return CopySymGlbDirAsGlb (hGlbQuadFC_SYS);
    else
    if (CompareGlobals (hGlbObj, hGlbQuadFC_CWS))
        // Copy the global memory handle
        return CopySymGlbDirAsGlb (hGlbQuadFC_CWS);
    else
    if (CompareGlobals (hGlbObj, hGlbQuadFEATURE_SYS))
        // Copy the global memory handle
        return CopySymGlbDirAsGlb (hGlbQuadFEATURE_SYS);
    else
    if (CompareGlobals (hGlbObj, hGlbQuadFEATURE_CWS))
        // Copy the global memory handle
        return CopySymGlbDirAsGlb (hGlbQuadFEATURE_CWS);
    else
    if (CompareGlobals (hGlbObj, hGlbQuadIC_SYS))
        // Copy the global memory handle
        return CopySymGlbDirAsGlb (hGlbQuadIC_SYS);
    else
    if (CompareGlobals (hGlbObj, hGlbQuadIC_CWS))
        // Copy the global memory handle
        return CopySymGlbDirAsGlb (hGlbQuadIC_CWS);
    else
    if (CompareGlobals (hGlbObj, hGlbQuadxLX))
        // Copy the global memory handle
        return CopySymGlbDirAsGlb (hGlbQuadxLX);
    else
    if (CompareGlobals (hGlbObj, hGlbV0Char))
        // Copy the global memory handle
        return CopySymGlbDirAsGlb (hGlbV0Char);
    return NULL;
} // End CheckGlobals


//***************************************************************************
//  $CompareGlobals
//
//  Compare the values in two global memory handles
//***************************************************************************

UBOOL CompareGlobals
    (HGLOBAL hGlb1,                     // Global memory handle #1
     HGLOBAL hGlb2)                     // ...                   2

{
    UBOOL             bRet = FALSE;     // TRUE iff the result is valid
    LPVARARRAY_HEADER lpMemHdrGlb1,     // Ptr to global memory #1 header
                      lpMemHdrGlb2;     // ...                   2 ...
    LPAPLDIM          lpMemGlb1,        // ...                  #1 dimensions
                      lpMemGlb2;        //                      #2 ...
    APLSTYPE          aplType;          // Common storage type
    APLNELM           aplNELM;          // ...    NELM
    APLRANK           aplRank;          // ...    rank
    UINT              uCnt;             // Loop counter

    if (!IsValidHandle (hGlb1)
     || !IsValidHandle (hGlb2))
        return FALSE;

    // Lock the memory to get a ptr to it
    lpMemHdrGlb1 = MyGlobalLockVar (hGlb1);
    lpMemHdrGlb2 = MyGlobalLockVar (hGlb2);

    // Get the storage type and NELM
    aplType = lpMemHdrGlb1->ArrType;
    aplNELM = lpMemHdrGlb1->NELM;
    aplRank = lpMemHdrGlb1->Rank;

    // Check the signature
    if (lpMemHdrGlb1->Sig.nature NE lpMemHdrGlb2->Sig.nature)
        goto ERROR_EXIT;

    // Check the storage type
    if (aplType NE lpMemHdrGlb2->ArrType)
        goto ERROR_EXIT;

    // Check the NELM
    if (aplNELM NE lpMemHdrGlb2->NELM   )
        goto ERROR_EXIT;

    // Check the rank
    if (aplRank NE lpMemHdrGlb2->Rank   )
        goto ERROR_EXIT;

    // Skip over the header to the dimensions
    lpMemGlb1 = VarArrayBaseToDim (lpMemHdrGlb1);
    lpMemGlb2 = VarArrayBaseToDim (lpMemHdrGlb2);

    // Check the dimensions
    for (uCnt = 0; uCnt < aplRank; uCnt++)
    if (*lpMemGlb1++ NE *lpMemGlb2++)
        goto ERROR_EXIT;

    // At this point, both lpMemGlb1 & lpMemGlb2 point to
    //   their respective data -- compare 'em
    bRet = CompareMemory (lpMemGlb1,
                          lpMemGlb2,
                          CalcArraySize (aplType, aplNELM, aplRank) - CalcHeaderSize (aplRank));
ERROR_EXIT:
    // We no longer need these ptrs
    MyGlobalUnlock (hGlb2); lpMemHdrGlb2 = NULL;
    MyGlobalUnlock (hGlb1); lpMemHdrGlb1 = NULL;

    return bRet;
} // End CompareGlobals


//***************************************************************************
//  $CompareMemory
//
//  Compare two areas of global memory
//***************************************************************************

UBOOL CompareMemory
    (LPVOID  lpMemGlb1,                 // Ptr to global memory #1
     LPVOID  lpMemGlb2,                 // ...                   2
     APLUINT uByteLen)                  // Byte length to compare

{
    APLUINT uLen,                       // Current length (DWORDs, WORDs, BYTEs)
            uCnt;                       // Loop counter

    // Calculate the DWORD length
    uLen = uByteLen / 4;
    uByteLen -= uLen * 4;

    for (uCnt = 0; uCnt < uLen; uCnt++)
    if (*((LPDWORD) lpMemGlb1)++ NE *((LPDWORD) lpMemGlb2)++)
        return FALSE;

    // Calculate the remaining WORD length
    uLen = uByteLen / 2;
    uByteLen -= uLen * 2;

    for (uCnt = 0; uCnt < uLen; uCnt++)
    if (*((LPWORD) lpMemGlb1)++ NE *((LPWORD) lpMemGlb2)++)
        return FALSE;

    // Calculate the remaining BYTE length
    uLen = uByteLen / 1;
////uByteLen -= uLen * 1;

    for (uCnt = 0; uCnt < uLen; uCnt++)
    if (*((LPBYTE) lpMemGlb1)++ NE *((LPBYTE) lpMemGlb2)++)
        return FALSE;

    return TRUE;
} // End CompareMemory


//***************************************************************************
//  $LoadWsGlbVarConv
//
//  Callback function for ParseLine to convert a FMTSTR_GLBCNT to an HGLOBAL
//***************************************************************************

HGLOBAL LoadWsGlbVarConv
    (UINT               uGlbCnt,
     LPLOADWSGLBVARPARM lpLoadWsGlbVarParm)

{
    WCHAR      wszGlbCnt[8 + 1];        // Save area for formatted uGlbCnt
    STFLAGS    stFlags = {0};           // SymTab flags
    LPSYMENTRY lpSymEntry;              // Ptr to STE for HGLOBAL

    // Tell 'em we're looking for )LOAD objects
////ZeroMemory (&stFlags, sizeof (stFlags));
    stFlags.Inuse   = TRUE;
    stFlags.ObjName = OBJNAME_LOD;

    // Format the global count
    MySprintfW (wszGlbCnt,
                sizeof (wszGlbCnt),
                FMTSTR_GLBCNT,
                uGlbCnt);
    // Get the matching HGLOBAL
    lpSymEntry =
      SymTabLookupName (wszGlbCnt, &stFlags);

    // If it's not found or has no value, load it from the [Globals] section
    if (lpSymEntry EQ NULL || !lpSymEntry->stFlags.Value)
        return
          LoadWorkspaceGlobal_EM (wszGlbCnt,                        // Ptr to keyname (FMTSTR_GLBCNT)
                                  lpLoadWsGlbVarParm->lpwSrc,       // Ptr to next available byte
                                  lpLoadWsGlbVarParm->iMaxSize,     // Maximum size of lpwSrc
                                  lpLoadWsGlbVarParm->hWndEC,       // Edit Ctrl window handle
                                  NULL,                             // Ptr to SYMENTRY of teh source (may be NULL)
                                  lpLoadWsGlbVarParm->lplpSymLink,  // Ptr to ptr to SYMENTRY link
                                  lpLoadWsGlbVarParm->lpwszVersion, // Ptr to workspace version text
                                  lpLoadWsGlbVarParm->lpDict,       // Ptr to workspace dictionary
                                  lpLoadWsGlbVarParm->lplpwErrMsg); // Ptr to ptr to (constant) error message text
    else
        return lpSymEntry->stData.stGlbData;
} // End LoadWsGlbVarConv


//***************************************************************************
//  End of File: sc_load.c
//***************************************************************************
