//***************************************************************************
//  NARS2000 -- System Command:  )COPY
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
#include "debug.h"              // For xxx_TEMP_OPEN macros


//***************************************************************************
//  $CmdCopy_EM
//
//  Execute the system command:  )COPY wsid [obj1 [obj2 ...]]
//                               )COPY :nnn [obj1 [obj2 ...]]
//    where wsid is a workspace identifier and :nnn is a tab identifier.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CmdCopy_EM"
#else
#define APPEND_NAME
#endif

UBOOL CmdCopy_EM
    (LPWCHAR lpwszTail)                     // Ptr to command line tail

{
    LPPERTABDATA lpMemPTD;                  // Ptr to PerTabData global memory
    WCHAR        wszTailDPFE[_MAX_PATH],    // Save area for canonical form of given ws name
                 wszVersion[WS_VERLEN],     // ...                     version info
                 wcTmp;                     // Temporary char
    LPWCHAR      lpwCmd,                    // Ptr to command line
                 lpwErrMsg,                 // Ptr to (constant) error message text
                 lpwNotFound;               // Ptr to list of names not found
    APLI3264     iMaxSize;                  // Maximum size of lpwszTemp
    int          iCurTabID,                 // Tab ID
                 iSrcTabID,                 // Source tab ID
                 iSrcTabIndex,              // Source tab index
                 iCnt;                      // # fields scanned
    UBOOL        bRet = FALSE;              // TRUE iff result is valid
    FILE        *fStream;                   // Ptr to file stream for the plain text workspace file
    HWND         hWndEC;                    // Edit Ctrl window handle
    LPSYMENTRY   lpSymLink = NULL;          // Anchor of SYMENTRY links for [Globals] values
                                            //   so we may delete them easily
    LPDICTIONARY lpDict = NULL;             // Ptr to workspace dictionary

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    iMaxSize  = lpMemPTD->iTempMaxSize;

    // Get the Session Manager's hWndEC
    hWndEC = GetThreadSMEC ();

    // If there's no WSID, that's an error
    if (lpwszTail EQ WC_EOS)
        goto INCORRECT_COMMAND_EXIT;

    // Save not found ptr
    lpwNotFound = lpwszTail;

    // Split cases based upon the first char
    // If we're copying from another tab, ...
    if (*lpwszTail EQ FMTCHR_LEAD)
    {
        // Scan the command line for the source tab ID
        iCnt = sscanfW (lpwszTail, FMTSTR_GLBCNT, &iSrcTabID);

        // Ensure we got a value
        if (iCnt NE 1)
            goto INCORRECT_COMMAND_EXIT;

        // Skip over the field
        lpwCmd = SkipToCharDQW (lpwszTail, L' ');
        wcTmp = *lpwCmd; *lpwCmd++ = WC_EOS;
        if (wcTmp)
            lpwCmd = SkipWhiteW (lpwCmd);

        // Get the matching source tab index
        iSrcTabIndex = TranslateTabIDToIndex (iSrcTabID);
        if (iSrcTabIndex EQ -1)
            goto TABNOTFOUND_EXIT;

        // Get the tab ID from which this command was issued
        iCurTabID = lpMemPTD->CurTabID;

        AppendLine (L"NONCE COMMAND", FALSE, TRUE);

////    DbgBrk ();












    } else
    // Copy from a workspace file
    {
        LPWCHAR lpwszProf;          // Ptr to profile string

        // Skip over the workspace name
        lpwCmd = SkipToCharDQW (lpwszTail, L' ');
        wcTmp = *lpwCmd; *lpwCmd++ = WC_EOS;
        if (wcTmp)
            lpwCmd = SkipWhiteW (lpwCmd);

        // Convert the given workspace name into a canonical form (without WS_WKSEXT)
        MakeWorkspaceNameCanonical (wszTailDPFE, lpwszTail, lpwszWorkDir);

        // Append the common workspace extension
        MyStrcatW (wszTailDPFE, sizeof (wszTailDPFE), WS_WKSEXT);

        // Attempt to open the workspace
        fStream = fopenW (wszTailDPFE, L"r");

        // If the workspace doesn't exist, ...
        if (fStream EQ NULL)
            goto WSNOTFOUND_EXIT;

        // We no longer need this resource
        fclose (fStream); fStream = NULL;

        // Initialize the iniparser
        lpDict = ProfileLoad_EM (wszTailDPFE, &lpwErrMsg);
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
        strcpyW (wszVersion, lpwszProf);

        // Compare the version #s
        if (lstrcmpW (wszVersion, WS_VERSTR) > 0)
        {
            WCHAR wszTemp[1024];

            // Format the error message text
            MySprintfW (wszTemp,
                        sizeof (wszTemp),
                       L"The version of this workspace (%s) is later than the interpreter expects (%s)."
                       L"  Please try copying from this workspace with a later version of the interpreter.",
                        wszVersion,
                        WS_VERSTR);
            // Tell the user the bad news
            MessageBoxW (hWndMF,
                         wszTemp,
                         WS_APPNAME,
                         MB_OK | MB_ICONSTOP);
            goto ERROR_EXIT;
        } // End IF

        // If the replaced char is EOS, copy all names
        if (wcTmp EQ WC_EOS)
        {
            // Loop through the [Vars.0] section copying
            //   all the names
            if (CopyWsVars (NULL,                   // Ptr to name in command line (may be NULL if bAllNames)
                            lpwCmd,                 // Ptr to command line
                            hWndEC,                 // Edit Ctrl for SM window handle
                           &lpwErrMsg,              // Ptr to ptr to (constant) error message text
                            TRUE,                   // TRUE iff we should process all names
                           &lpSymLink,              // Ptr to ptr to SYMENTRY link
                            wszVersion,             // Ptr to workspace version text
                            lpDict,                 // Ptr to workspace dictionary
                            lpMemPTD,               // Ptr to PerTabData global memory
                            iMaxSize)               // Maximum size of lpMemPTD->lpwszTemp
                    EQ -1)                          // If it's an error, ...
                goto ERRMSG_EXIT;

            // Loop through the [Fcns.0] section copying
            //   all the names
            switch (CopyWsFcns (NULL,                   // Ptr to name in command line (may be NULL if bAllNames)
                                hWndEC,                 // Edit Ctrl for SM window handle
                               &lpwErrMsg,              // Ptr to ptr to (constant) error message text
                                TRUE,                   // TRUE iff we should process all names
                               &lpSymLink,              // Ptr to ptr to SYMENTRY link
                                wszVersion,             // Ptr to workspace version text
                                lpDict,                 // Ptr to workspace dictionary
                                lpMemPTD,               // Ptr to PerTabData global memory
                                iMaxSize))              // Maximum size of lpMemPTD->lpwszTemp
            {
                case -1:        // We encountered an error
                case 1:         // No match???
                    goto ERRMSG_EXIT;

                case 0:         // We found a match
                    break;

                defstop
                    goto ERRMSG_EXIT;
            } // End SWITCH
        } else
        {
            // Loop through the names starting at <lpwCmd>
            while (*lpwCmd)
            {
                LPWCHAR lpwNameInCmd;           // Ptr to the name in the command line

                // Find the next name
                lpwNameInCmd = lpwCmd;
                lpwCmd = SkipToCharDQW (lpwNameInCmd, L' ');
                if (*lpwCmd NE WC_EOS)
                {
                    *lpwCmd++ = WC_EOS;
                    lpwCmd = SkipWhiteW (lpwCmd);
                } // End IF

                // Loop through the [Vars.0] section looking for each given name of the form
                //  xxx=Name=:ggg               for a variable in [Globals]
                //  xxx=Name=T 1 0 value        for an immediate scalar variable
                //                              where T is the variable immediate type (BIFC)
                switch (CopyWsVars (lpwNameInCmd,   // Ptr to name in command line (may be NULL if bAllNames)
                                    lpwCmd,         // Ptr to command line
                                    hWndEC,         // Edit Ctrl for SM window handle
                                   &lpwErrMsg,      // Ptr to ptr to (constant) error message text
                                    FALSE,          // TRUE iff we should process all names
                                   &lpSymLink,      // Ptr to ptr to SYMENTRY link
                                    wszVersion,     // Ptr to workspace version text
                                    lpDict,         // Ptr to workspace dictionary
                                    lpMemPTD,       // Ptr to PerTabData global memory
                                    iMaxSize))      // Maximum size of lpMemPTD->lpwszTemp
                {
                    case -1:        // We encountered an error
                        goto ERRMSG_EXIT;

                    case 0:         // We found a match
                        continue;   // Look for the next name

                    case 1:         // No match
                        break;      // Keep processing

                    defstop
                        break;
                } // End SWITCH

                // Loop through the [Fcns.0] section looking for each given name of the form
                //  xxx=Name=y=:ggg             for a function in [Globals] with NameType y
                //  xxx=Name=y={name} or char   for immediate functions
                switch (CopyWsFcns (lpwNameInCmd,   // Ptr to name in command line (may be NULL if bAllNames)
                                    hWndEC,         // Edit Ctrl for SM window handle
                                   &lpwErrMsg,      // Ptr to ptr to (constant) error message text
                                    FALSE,          // TRUE iff we should process all names
                                   &lpSymLink,      // Ptr to ptr to SYMENTRY link
                                    wszVersion,     // Ptr to workspace version text
                                    lpDict,         // Ptr to workspace dictionary
                                    lpMemPTD,       // Ptr to PerTabData sglobal memory
                                    iMaxSize))      // Maximum size of lpMemPTD->lpwszTemp
                {
                    case -1:        // We encountered an error
                        goto ERRMSG_EXIT;

                    case 0:         // We found a match
                        continue;   // Look for the next name

                    case 1:         // No match
                        break;      // Keep processing

                    defstop
                        goto ERRMSG_EXIT;
                } // End SWITCH

                // We didn't find the name, so add it to the NOT FOUND: list
                strcpyW (lpwNotFound, lpwNameInCmd);

                // Skip to the trailing zero
                lpwNotFound += lstrlenW (lpwNotFound);

                // Insert a blank separator and skip over it for the next entry
                *lpwNotFound++ = L' ';
            } // End WHILE
        } // End IF/ELSE

        // Delete the symbol table entries for vars/fcns we allocated of the form FMTSTR_GLBCNT
        DeleteGlobalLinks (lpSymLink);
    } // End IF/ELSE

    // Display the workspace timestamp
    DisplayWorkspaceStamp (lpDict);

    // Check for names not found
    if (lpwNotFound NE lpwszTail)
    {
        // Ensure properly terminated
        lpwNotFound[-1] = WC_EOS;

        // Write out the leading text followed by the names
        AppendLine (ERRMSG_NOT_FOUND, FALSE, FALSE);
        AppendLine (lpwszTail, FALSE, TRUE);
    } // End IF

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

INCORRECT_COMMAND_EXIT:
    IncorrectCommand ();

    goto ERROR_EXIT;

TABNOTFOUND_EXIT:
    ReplaceLastLineCRPmt (ERRMSG_TAB_NOT_FOUND APPEND_NAME);

    goto ERROR_EXIT;

WSNOTFOUND_EXIT:
    ReplaceLastLineCRPmt (ERRMSG_WS_NOT_FOUND APPEND_NAME);

    goto ERROR_EXIT;

ERRMSG_EXIT:
    ReplaceLastLineCRPmt (lpwErrMsg);

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    // If there's a dictionary, ...
    if (lpDict)
    {
        // Free the dictionary
        ProfileUnload (lpDict); lpDict = NULL;
    } // End IF

    return bRet;
} // End CmdCopy_EM
#undef  APPEND_NAME


//***************************************************************************
//  $DeleteGlobalLinks
//
//  Delete SymLink entries
//***************************************************************************

void DeleteGlobalLinks
    (LPSYMENTRY lpSymLink)      // Ptr to anchor of SYMENTRY links for [Globals]

{
    LPSYMENTRY lpSymLast;       // Temporary ptr to previous stSymLink

    while (lpSymLink)
    {
        // Free the global var/fcn/opr
        FreeResultGlobalDFLV (lpSymLink->stData.stGlbData);

        // Point to the next entry and zap it
        lpSymLast = lpSymLink->stSymLink;
        lpSymLink->stSymLink = NULL;

        // Erase the Symbol Table Entry
        //   unless it's a []var
        EraseSTE (lpSymLink, FALSE);

        // ***FIXME*** -- What's involved in deleting the STE??

        // Point to the next one
        lpSymLink = lpSymLast;
    } // End WHILE
} // End DeleteGlobalLinks


//***************************************************************************
//  $CopyWsVars
//
//  Copy all vars from a workspace or just those which match a given name
//***************************************************************************

int CopyWsVars
    (LPWCHAR       lpwNameInCmd,            // Ptr to name in command line (may be NULL if bAllNames)
     LPWCHAR       lpwCmd,                  // Ptr to command line
     HWND          hWndEC,                  // Edit Ctrl for SM window handle
     LPWCHAR      *lplpwErrMsg,             // Ptr to ptr to (constant) error message text
     UBOOL         bAllNames,               // TRUE if we should process all names
     LPSYMENTRY   *lplpSymLink,             // Ptr to ptr to SYMENTRY link
     LPWCHAR       lpwszVersion,            // Ptr to workspace version text
     LPDICTIONARY  lpDict,                  // Ptr to workspace dictionary
     LPPERTABDATA  lpMemPTD,                // Ptr to PerTabData global memory
     APLI3264      iMaxSize)                // Maximum size of lpMemPTD->lpwszTemp

{
    WCHAR        wszCount[8];               // Save area for formatted uSymVar/Fcn counter
    LPWCHAR      lpwNameInWrk,              // Ptr to name in workspace file
                 lpwszProf,                 // Ptr to profile string
                 lpwDataInWrk;              // Ptr to key data from the saved workspace file
    UINT         uSymVar,                   // Var section counter
                 uCnt;                      // Loop counter
    LPSYMENTRY   lpSymEntry;                // Ptr to name in cmd SYMENTRY
    STFLAGS      stFlags = {0};             // STE flags

    // In case <lpwNameInCmd> is that of a system var, ...
    if (lpwNameInCmd
     && IsSysName (lpwNameInCmd))
        // Convert it to lowercase
        CharLowerBuffW (lpwNameInCmd, lstrlenW (lpwNameInCmd));

    // Get the [Vars.0] count
    uSymVar =
      ProfileGetInt (SECTNAME_VARS L".0",   // Ptr to the section name
                     KEYNAME_COUNT,         // Ptr to the key name
                     0,                     // Default value if not found
                     lpDict);               // Ptr to workspace dictionary
    // Loop through the [Vars.0] section
    for (uCnt = 0; uCnt < uSymVar; uCnt++)
    {
        // Format the counter
        MySprintfW (wszCount,
                    sizeof (wszCount),
                   L"%d",
                    uCnt);
        // Point to the name in the workspace
        lpwNameInWrk = lpMemPTD->lpwszTemp;

        // Read the next string
        lpwszProf =
          ProfileGetString (SECTNAME_VARS L".0",    // Ptr to the section name
                            wszCount,               // Ptr to the key name
                            L"",                    // Ptr to the default value
                            lpDict);                // Ptr to workspace dictionary
        // Copy to save area
        // DO NOT USE lstrcpyW as it doesn't trigger a visible Page Fault
        strcpyW (lpwNameInWrk, lpwszProf);

        // Find the separator after the name and zap it
        lpwDataInWrk = strchrW (lpwNameInWrk, L'=');
        *lpwDataInWrk++ = WC_EOS;

#define lpwSrcStart     lpwNameInWrk
#define lpwSrc          lpwDataInWrk

        // Convert the {name}s and other chars to UTF16_xxx
        ConvertNameInPlace (lpwNameInWrk);

        // Compare the names to see if we have a match
        if (bAllNames || lstrcmpW (lpwNameInWrk, lpwNameInCmd) EQ 0)
        {
            APLSTYPE     aplTypeObj;            // Object storage type
            APLLONGEST   aplLongestObj;         // Object immediate value
            LPAPLLONGEST lpaplLongestObj;       // Ptr to ...
            UBOOL        bImmed;                // TRUE iff the result of ParseSavedWsVar_EM is immediate

            // Set the flags for what we're looking up/appending
            stFlags.Inuse   = TRUE;

            if (IsSysName (lpwNameInWrk))
            {
                // If we're copying all names, skip the system vars
                if (bAllNames)
                    continue;

                // Tell 'em we're looking for system names
                stFlags.ObjName = OBJNAME_SYS;
            } else
                // Tell 'em we're looking for user names
                stFlags.ObjName = OBJNAME_USR;

            // Lookup the name in the symbol table
            lpSymEntry =
              SymTabLookupName (lpwNameInWrk, &stFlags);

            // If the name is not found, it must be a user name
            Assert ((stFlags.ObjName EQ OBJNAME_USR) || lpSymEntry NE NULL);

            // If the name is not found, append it as a user name
            if (lpSymEntry EQ NULL)
            {
                // Append the name to get a new LPSYMENTRY
                lpSymEntry = SymTabAppendName_EM (lpwNameInWrk, &stFlags);
                if (lpSymEntry EQ NULL)
                    goto ERROR_EXIT;

                // Mark the SYMENTRY as immediate so we don't free the
                //   (non-existant) stGlbData
                // Set other flags as appropriate
                lpSymEntry->stFlags.Imm        = TRUE;
                lpSymEntry->stFlags.ObjName    = OBJNAME_USR;
                lpSymEntry->stFlags.stNameType = NAMETYPE_VAR;
            } // End IF

            // Clear so we save a clean value
            aplLongestObj = 0;

            // Set this value as lpaplLongestObj is incremented by ParseSavedWsVar_EM
            lpaplLongestObj = &aplLongestObj;

            // Parse the value into aplLongestObj and aplTypeObj
            lpwDataInWrk =
              ParseSavedWsVar_EM (lpwDataInWrk,     // Ptr to input buffer
                                  iMaxSize - (APLU3264) ((LPBYTE) lpwSrc - (LPBYTE) lpwSrcStart),  // Maximum size of lpwSrc
                                 &lpaplLongestObj,  // Ptr to ptr to output element
                                 &aplTypeObj,       // Ptr to storage type (may be NULL)
                                  lpSymEntry,       // Ptr to SYMENTRY of the source (may be NULL)
                                 &bImmed,           // Ptr to immediate flag (TRUE iff result is immediate) (may be NULL)
                                  FALSE,            // TRUE iff to save SymTabAppend values, FALSE to save values directly
                                  TRUE,             // TRUE iff this is called from )COPY
                                  hWndEC,           // Edit Ctrl window handle
                                  lplpSymLink,      // Ptr to ptr to SYMENTRY link
                                  lpwszVersion,     // Ptr to workspace version text
                                  lpDict,           // Ptr to workspace eictionary
                                  lplpwErrMsg);     // Ptr to ptr to (constant error message text
            if (lpwDataInWrk EQ NULL)
                goto ERRMSG_EXIT;

            // Out with the old
            if (!lpSymEntry->stFlags.Imm && lpSymEntry->stFlags.Value)
            {
                FreeResultGlobalVar (lpSymEntry->stData.stGlbData); lpSymEntry->stData.stGlbData = NULL;
            } // End IF

            // Set the stFlags & stData
            lpSymEntry->stFlags.Imm        = bImmed;
            lpSymEntry->stFlags.ImmType    = bImmed ? TranslateArrayTypeToImmType (aplTypeObj)
                                                    : IMMTYPE_ERROR;
            lpSymEntry->stFlags.Value      = TRUE;
            lpSymEntry->stFlags.stNameType = NAMETYPE_VAR;
            lpSymEntry->stData.stLongest   = aplLongestObj;

            // If we're copying a single name, ...
            if (!bAllNames)
                // Mark as successful (we found a match)
                return 0;
        } // End IF
    } // End FOR

    if (!bAllNames)
        // Mark as no match, keep processing
        return 1;
    else
        // Mark as successful (we copied all the vars)
        return 0;
ERROR_EXIT:
ERRMSG_EXIT:
    // Mark as in error
    return -1;
#undef  lpwSrc
#undef  lpwSrcStart
} // End CopyWsVars


//***************************************************************************
//  $CopyWsFcns
//
//  Copy all fcns/oprs from a workspace or just those which match a given name
//***************************************************************************

int CopyWsFcns
    (LPWCHAR       lpwNameInCmd,            // Ptr to name in command line (may be NULL if bAllNames)
     HWND          hWndEC,                  // Edit Ctrl for SM window handle
     LPWCHAR      *lplpwErrMsg,             // Ptr to ptr to (constant) error message text
     UBOOL         bAllNames,               // TRUE if we should process all names
     LPSYMENTRY   *lplpSymLink,             // Ptr to ptr to SYMENTRY link
     LPWCHAR       lpwszVersion,            // Ptr to workspace version text
     LPDICTIONARY  lpDict,                  // Ptr to workspace dictionary
     LPPERTABDATA  lpMemPTD,                // Ptr to PerTabData global memory
     APLI3264      iMaxSize)                // Maximum size of lpMemPTD->lpwszTemp

{
    WCHAR        wszCount[8];               // Save area for formatted uSymVar/Fcn counter
    LPWCHAR      lpwNameInWrk,              // Ptr to name in workspace file
                 lpwszProf,                 // Ptr to profile string
                 lpwDataInWrk;              // Ptr to key data from the saved workspace file
    UINT         uSymFcn,                   // FcnOpr section counter
                 uCnt;                      // Loop counter
    LPSYMENTRY   lpSymEntry;                // Ptr to name in command's SYMENTRY
    STFLAGS      stFlags = {0};             // STE flags
    APLU3264     uLen;                      // Protection length
    UBOOL        bRet;                      // TRUE iff the result is valid
    LPWCHAR      lpwszOrigTemp;             // Original lpMemPTD->lpwszTemp
    VARS_TEMP_OPEN

    // Get the [Fcns.0] count
    uSymFcn =
      ProfileGetInt (SECTNAME_FCNS L".0",   // Ptr to the section name
                     KEYNAME_COUNT,         // Ptr to the key name
                     0,                     // Default value if not found
                     lpDict);               // Ptr to workspace dictionary
    // Loop through the [Fcns.0] section
    for (uCnt = 0; uCnt < uSymFcn; uCnt++)
    {
        NAME_TYPES nameType;

        // Format the counter
        MySprintfW (wszCount,
                    sizeof (wszCount),
                   L"%d",
                    uCnt);
        // Point to the name in the workspace
        lpwNameInWrk = lpMemPTD->lpwszTemp;
        CHECK_TEMP_OPEN

#define lpwSrcStart     lpwNameInWrk
#define lpwSrc          lpwDataInWrk

        // Read the next string
        lpwszProf =
          ProfileGetString (SECTNAME_FCNS L".0",  // Ptr to the section name
                            wszCount,             // Ptr to the key name
                            L"",                  // Ptr to the default value
                            lpDict);              // Ptr to workspace dictionary
        // Copy to save area
        // DO NOT USE lstrcpyW as it doesn't trigger a visible Page Fault
        strcpyW (lpwNameInWrk, lpwszProf);

        // Find the separator after the name and zap it
        lpwDataInWrk = strchrW (lpwNameInWrk, L'=');
        *lpwDataInWrk++ = WC_EOS;

        // Convert the {name}s and other chars to UTF16_xxx
        ConvertNameInPlace (lpwNameInWrk);

        // Compare the names to see if we have a match
        if (bAllNames || lstrcmpW (lpwNameInWrk, lpwNameInCmd) EQ 0)
        {
            // Parse the name type (2 = FN0, 3 = FN12, 4 = OP1, 5 = OP2, 6 = OP3)
            nameType = *lpwDataInWrk++ - '0';

            Assert (*lpwDataInWrk EQ L'='); lpwDataInWrk++;

            // Set the flags for what we're looking up
            stFlags.Inuse   = TRUE;
            stFlags.ObjName = OBJNAME_USR;

            // Lookup the name in the symbol table
            lpSymEntry =
              SymTabLookupName (lpwNameInWrk, &stFlags);

            // If the name is not found, it must be a user name
            Assert ((stFlags.ObjName EQ OBJNAME_USR) || lpSymEntry NE NULL);

            // If the name is not found, append it as a user name
            if (lpSymEntry EQ NULL)
            {
                // Append the name to get a new LPSYMENTRY
                lpSymEntry = SymTabAppendName_EM (lpwNameInWrk, &stFlags);
                if (lpSymEntry EQ NULL)
                    goto ERROR_EXIT;

                // Set stFlags as appropriate
                lpSymEntry->stFlags.ObjName    = OBJNAME_USR;
////////////////lpSymEntry->stFlags.stNameType = nameType;          // Set in ParseSavedWsFcn_EM
            } // End IF

            // Protect the text
            lpwszOrigTemp = lpMemPTD->lpwszTemp;
            uLen = (&lpwDataInWrk[lstrlenW (lpwDataInWrk)] - lpMemPTD->lpwszTemp);
            lpMemPTD->lpwszTemp += uLen;

            EXIT_TEMP_OPEN

            // Parse the line into lpSymEntry->stData
            bRet = ParseSavedWsFcn_EM (lpwDataInWrk,      // Ptr to input buffer
                                       iMaxSize - (APLU3264) ((LPBYTE) lpwSrc - (LPBYTE) lpwSrcStart),  // Maximum size of lpwSrc
                                       lpSymEntry,        // Ptr to STE for the object
                                       nameType,          // Function name type (see NAME_TYPES)
                                       hWndEC,            // Edit Ctrl window handle
                                       lplpSymLink,       // Ptr to ptr to SYMENTRY link
                                       lpwszVersion,      // Ptr to workspace version text
                                       lpDict,            // Ptr to workspace dictionary
                                       lplpwErrMsg);      // Ptr to ptr to (constant) error message text
            if (!bRet)
                goto ERRMSG_EXIT;
            // If we're copying a single name, ...
            if (!bAllNames)
                // Mark as successful (we found a match)
                return 0;

            CHECK_TEMP_OPEN
        } // End IF

        EXIT_TEMP_OPEN
    } // End FOR

    if (!bAllNames)
        // Mark as no match, keep processing
        return 1;
    else
        // Mark as successful (we copied all the vars)
        return 0;
ERROR_EXIT:
ERRMSG_EXIT:
    // Mark as in error
    return -1;
#undef  lpwSrc
#undef  lpwSrcStart
} // End CopyWsFcns


//***************************************************************************
//  End of File: sc_copy.c
//***************************************************************************
