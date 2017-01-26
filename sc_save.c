//***************************************************************************
//  NARS2000 -- System Command:  )SAVE
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
//  $CmdSave_EM
//
//  Execute the system command:  )SAVE [wsid]
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CmdSave_EM"
#else
#define APPEND_NAME
#endif

UBOOL CmdSave_EM
    (LPWCHAR lpwszTail)                     // Ptr to command line tail

{
    HGLOBAL      hGlbName,                  // STE name global memory handle
                 hGlbCnt = NULL,            // Vars/Fcns/Oprs counters global memory handle
                 stGlbData;                 // SYMENTRY's stGlbData
    LPPERTABDATA lpMemPTD;                  // Ptr to PerTabData global memory
    LPVOID       lpMemOldWSID = NULL;       // Ptr to old []WSID global memory
    UINT         (*lpMemCnt)[][2] = NULL;   // Ptr to Vars/Fcns counters, [0] = Vars, [1] = Fcns
    WCHAR        wszTailDPFE[_MAX_PATH],    // Save area for canonical form of given ws name
                 wszWsidDPFE[_MAX_PATH],    // ...           ...               []WSID
                 wszSectName[15],           // ...           section name (e.g., [Vars.sss])
                 wszCount[8];               // ...           formatted uSymxxx counter
    LPWCHAR      lpMemSaveWSID,             // Ptr to WSID to save to
                 lpMemName,                 // Ptr to STE name
                 lpaplChar,                 // Ptr to output save area
                 lpw;                       // Temporary ptr
    APLNELM      aplNELMWSID;               // []WSID NELM
    APLRANK      aplRankWSID;               // ...    rank
    APLUINT      ByteRes,                   // # bytes in the result
                 uQuadPP;                   // []PP save area
    UBOOL        bRet = FALSE,              // TRUE iff result is valid
                 bDispMPSuf;                // Save area for OptionFlags value
    APLU3264     uNameLen;                  // Length of the object name in WCHARs
    int          iCmp;                      // Comparison result
    UINT         uGlbCnt=0;                 // # entries in [Globals] section
    FILE        *fStream;                   // Ptr to file stream
    LPSYMENTRY   lpSymEntry,                // Ptr to STE
                 lpSymTabNext;              // ...
    STFLAGS      stFlags;                   // STE flags
    LPWCHAR      lpwszFormat,               // Ptr to formatting save area
                 lpwszOrigTemp,             // Original lpMemPTD->lpwszTemp
                 lpwErrMsg,                 // Ptr to (constant) error message text
                 lpwszTemp;                 // Ptr to temporary storage
    LPDICTIONARY lpDict = NULL;             // Ptr to workspace dictionary
    SAVEDWSVAR_CB savedWsVar_CB;            // Saved WS Var Callback struct
    VARS_TEMP_OPEN

    // Skip to the next blank
    lpw = SkipToCharDQW (lpwszTail, L' ');

    // Zap it in case there are trailing blanks
    if (*lpw NE WC_EOS)
        *lpw = WC_EOS;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Lock the memory to get a ptr to it
    lpMemOldWSID = MyGlobalLockVar (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_WSID]->stData.stGlbData);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemOldWSID)
    // Get the NELM and Rank
    aplNELMWSID = lpHeader->NELM;
    aplRankWSID = lpHeader->Rank;
#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMemSaveWSID = VarArrayDataFmBase (lpMemOldWSID);

    // Convert the []WSID workspace name into a canonical form (without WS_WKSEXT)
    MakeWorkspaceNameCanonical (wszWsidDPFE, lpMemSaveWSID, lpwszWorkDir);

    // If there is a specified WS Name,
    //   and the current []WSID is different from the specified WS Name,
    //   and a WS with the specified WS Name already exists,
    //   display an error
    //     "NOT SAVED, THIS WS IS ",[]WSID
    if (*lpwszTail NE WC_EOS)
    {
        // Convert the given workspace name into a canonical form (without WS_WKSEXT)
        MakeWorkspaceNameCanonical (wszTailDPFE, lpwszTail, lpwszWorkDir);

        // Compare the workspace names (without their extensions)
        iCmp = lstrcmpiW (wszWsidDPFE, wszTailDPFE);

        // Append the common workspace extension
        MyStrcatW (wszTailDPFE, sizeof (wszTailDPFE), WS_WKSEXT);

        // If unequal and the given named ws already exists,
        //   display an error
        if (iCmp NE 0)
        {
            // Attempt to open the workspace
            fStream = fopenW (wszTailDPFE, L"r");

            // If it already exists, display an error
            if (fStream NE NULL)
            {
                WCHAR wszTemp[1024];

                // We no longer need this resource
                fclose (fStream); fStream = NULL;

                // If []WSID is empty, it's a CLEAR WS
                if (IsEmpty (aplNELMWSID))
                    strcpyW (wszTemp, ERRMSG_NOT_SAVED_CLEAR_WS);
                else
                {
                    // Copy the leading part of the message
                    strcpyW (wszTemp, ERRMSG_NOT_SAVED);

                    // Followed by the old []WSID
                    strcpynW (&wszTemp[lstrlenW (wszTemp)], lpMemSaveWSID, (UINT) aplNELMWSID + 1);
                } // End IF/ELSE

                // Display the error message
                ReplaceLastLineCRPmt (wszTemp);

                goto ERROR_EXIT;
            } // End IF

            // We no longer need this ptr
            MyGlobalUnlock (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_WSID]->stData.stGlbData); lpMemOldWSID = NULL;

            // Save wszTailDPFE as the value of the new []WSID
            if (!SaveNewWsid_EM (wszTailDPFE))
                goto ERROR_EXIT;
        } // End IF

        // Save as the target WSID
        lpMemSaveWSID = wszTailDPFE;
    } else
    {
        // If []WSID is empty, it's a CLEAR WS
        if (IsEmpty (aplNELMWSID))
            goto NOTSAVED_CLEAR_EXIT;

        // Append the common workspace extension
        MyStrcatW (wszWsidDPFE, sizeof (wszWsidDPFE), WS_WKSEXT);

        // Save as the target WSID
        lpMemSaveWSID = wszWsidDPFE;
    } // End IF/ELSE

    if (lpMemOldWSID NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_WSID]->stData.stGlbData); lpMemOldWSID = NULL;
    } // End IF

    // The full workspace name to save to is in lpMemSaveWSID

    // Initialize the profile
    lpDict = ProfileInit_EM (lpMemSaveWSID, &lpwErrMsg);
    if (lpDict EQ NULL)
        goto ERRMSG_EXIT;

    // Make a backup copy of the workspace
    if (OptionFlags.bBackupOnSave)
        MakeWorkspaceBackup (lpMemSaveWSID, SAVEBAK_EXT);

    // Note if the SI is non-empty
    if (lpMemPTD->SILevel NE 0)
        ReplaceLastLineCRPmt (L"WARNING:  SI non-empty -- not restartable after )LOAD");

    // Calculate space needed for the two counters
    ByteRes = 2 * sizeof (UINT) * (lpMemPTD->SILevel + 1);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for two counters (Vars & Fcns) per SI level
    hGlbCnt = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbCnt EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemCnt = MyGlobalLock000 (hGlbCnt);

    // The format of a saved ws is as an .ini file
    //   (named *.ws.nars) with the following sections:
    //   [General]    (General settings)
    //   [Globals]    (Ptrs to global memory variables/function/operators)
    //   [Vars.0]     (Global variables) -- targets of )COPY
    //   [Vars.1]     (Variables at level 1 in the SI stack)
    //   [Vars.sss]   (Variables at level sss in the SI stack)
    //   [Fcns.0]     (Global functions) -- targets of )COPY
    //   [Fcns.1]     (Functions at level 1 in the SI stack)
    //   [Fcns.sss]   (Functions at level sss in the SI stack)
    // The [Globals] section has content of
    //  FMTSTR_GLBCNT=V T N R S val for variables -- %c %d %d %*(d) ...
    //                              where V is V indicating a variable
    //                                    T is the variable type (BIFCHNA) -- %c
    //                                    N is the NELM -- %d
    //                                    R is the rank -- %d
    //                                    S is the shape -- Rank occurrences of %d
    //                                    value is the series of values of similar form to FMTSTR_GLBCNT
    //                                      without the leading V.
    //  FMTSTR_GLBCNT=F nnn.Name    for user-defined functions/operators
    //                              where F is F indicating a function
    //                                    nnn is a counter across functions & variables
    //                                    Name is the name of the function
    // The [Vars.sss] section has content of
    //  xxx=Name=FMTSTR_GLBCNT      for a variable in [Globals]
    //  xxx=Name=T 1 0 value        for an immediate scalar variable
    //                              where T is the variable immediate type (BIFC)
    // The [Fcns.sss] section has content of
    //  xxx=Name=y=FMTSTR_GLBCNT    for a function in [Globals] with NameType y
    //  xxx=Name=y={name} or char   for immediate functions
    // The [nnn.Name] section (used for user-defined functions/operators only)
    //   has content of
    //  xxx=<Function Header/Lines>

    // Get ptr to formatting save area & temporary storage
    lpwszFormat = lpMemPTD->lpwszFormat;
    lpwszTemp   = lpMemPTD->lpwszTemp;
    CHECK_TEMP_OPEN
    // Save the current bDispMPSuf flag and set to FALSE
    //  so we display values the same way every time
    bDispMPSuf = OptionFlags.bDispMPSuf;
    OptionFlags.bDispMPSuf = FALSE;

    // Create the [General] section
    ProfileSetSection (SECTNAME_GENERAL,        // Ptr to the section name
                       lpDict);                 // Ptr to the dictionary
    // Create the [Globals] section
    ProfileSetSection (SECTNAME_GLOBALS,        // Ptr to the section name
                       lpDict);                 // Ptr to the dictionary
    // Create the [TempGlobals] section
    ProfileSetSection (SECTNAME_TEMPGLOBALS,    // Ptr to the section name
                       lpDict);                 // Ptr to the dictionary
    // Create the [Vars.0] section
    ProfileSetSection (SECTNAME_VARS L".0",     // Ptr to the section name
                       lpDict);                 // Ptr to the dictionary
    // Create the [Fcns.0] section
    ProfileSetSection (SECTNAME_FCNS L".0",     // Ptr to the section name
                       lpDict);                 // Ptr to the dictionary
    // If there are additional SI levels, ...
    if (lpMemPTD->SILevel > 0)
    {
        WCHAR wszTemp[128];
        UINT  i;

        // Note that the SI is non-empty
        ReplaceLastLineCRPmt (L"WARNING:  SI non-empty -- not restartable after )LOAD");

        // Create the [SI] section
        ProfileSetSection (SECTNAME_SI,             // Ptr to the section name
                           lpDict);                 // Ptr to the dictionary
        // Loop through the remaining SI levels
        for (i = 1; i <= lpMemPTD->SILevel; i++)
        {
            // Format the section name
            MySprintfW (wszTemp,
                        sizeof (wszTemp),
                        SECTNAME_VARS L".%d",
                        i);
            // Create the [Vars.x] section
            ProfileSetSection (wszTemp,                 // Ptr to the section name
                               lpDict);                 // Ptr to the dictionary
            // Format the section name
            MySprintfW (wszTemp,
                        sizeof (wszTemp),
                        SECTNAME_FCNS L".%d",
                        i);
            // Create the [Fcns.x] section
            ProfileSetSection (wszTemp,                 // Ptr to the section name
                               lpDict);                 // Ptr to the dictionary
        } // End FOR
    } // End IF

    // Write out to the [General] section
    ProfileSetString (SECTNAME_GENERAL,     // Ptr to the section name
                      KEYNAME_VERSION,      // Ptr to the key name
                      WS_VERSTR,            // Ptr to the key value
                      lpDict);              // Ptr to the dictionary
    __try
    {
        // Trundle through the Symbol Table
        for (lpSymTabNext = lpMemPTD->lphtsPTD->lpSymTab;
             lpSymTabNext < lpMemPTD->lphtsPTD->lpSymTabNext;
             lpSymTabNext++)
        if (lpSymTabNext->stFlags.Inuse)        // Must be Inuse
        // Handle different SI levels
        for (lpSymEntry = lpSymTabNext;
             lpSymEntry;
             lpSymEntry = lpSymEntry->stPrvEntry)
        if (lpSymEntry->stHshEntry->htGlbName NE NULL   // Must have a name (not steZero, etc.),
         && lpSymEntry->stFlags.ObjName NE OBJNAME_MFO  // and not be a Magic Function/Operator,
         && lpSymEntry->stFlags.ObjName NE OBJNAME_LOD) // and not be a )LOAD HGLOBAL
        {
            // Get the symbol name's global memory handle
            hGlbName = lpSymEntry->stHshEntry->htGlbName;

            // Lock the memory to get a ptr to it
            lpMemName = MyGlobalLockWsz (hGlbName);

            // Format the text as an ASCII string with non-ASCII chars
            //   represented as either {symbol} or {\xXXXX} where XXXX is
            //   a four-digit hex number.
            uNameLen =
              ConvertWideToName (lpwszTemp,
                                 lpMemName);
            // We no longer need this ptr
            MyGlobalUnlock (hGlbName); lpMemName = NULL;

            // Point to the end of the name
            lpaplChar         = &lpwszTemp[uNameLen];

            // Get the flags
            stFlags = lpSymEntry->stFlags;

            // Check for []WSID (if so, skip over it)
            if (lstrcmpiW (lpwszTemp, L"{quad}wsid") EQ 0)
                continue;
            else
                // Save the GlbData handle
                stGlbData = lpSymEntry->stData.stGlbData;

            if (stFlags.Value)                      // Must have a value
            {
                // Split cases based upon the object type
                switch (stFlags.stNameType)
                {
                    case NAMETYPE_VAR:
                        // Format the counter
                        MySprintfW (wszCount,
                                    sizeof (wszCount),
                                   L"%d",
                                    (*lpMemCnt)[lpSymEntry->stSILevel][0]++);
                        // Format the section name as [Vars.sss] where sss is the SI level
                        MySprintfW (wszSectName,
                                    sizeof (wszSectName),
                                    SECTNAME_VARS L".%d",
                                    lpSymEntry->stSILevel);
                        // Fill in the Saved Ws Var Callback struct
                        savedWsVar_CB.lpwAppName   = wszSectName;
                        savedWsVar_CB.lpwKeyName   = wszCount;
                        savedWsVar_CB.lpDict       = lpDict;
                        savedWsVar_CB.iMaxLength   = 1000;
                        savedWsVar_CB.iIndex       = -1;

                        // Append separator
                        *lpaplChar++ = L'=';

                        // If it's immediate, ...
                        if (stFlags.Imm)
                        {
                            // Append the header for the simple scalar
                            lpaplChar =
                              AppendArrayHeader (lpaplChar,                         // Ptr to output save area
                                                 TranslateImmTypeToChar (stFlags.ImmType),// Object storage type as WCHAR
                                                 1,                                 // Object NELM
                                                 0,                                 // Object rank
                                                 NULL,                              // Ptr to object dimensions
                                                 NULL,                              // Ptr to common VFP array precision (0 if none) (may be NULL)
                                                 NULL);                             // Ptr to array header
                            // Split cases based upon the immediate type
                            switch (stFlags.ImmType)
                            {
                                case IMMTYPE_BOOL:
                                case IMMTYPE_INT:
                                case IMMTYPE_FLOAT:
                                    // Ensure we format with full precision in case it's floating point
                                    uQuadPP = lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_PP]->stData.stInteger;
                                    if (IsImmFlt (stFlags.ImmType))
                                        lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_PP]->stData.stInteger = DEF_MAX_QUADPP_IEEE;

                                    // Format the value
                                    lpaplChar =
                                      FormatImmedFC (lpaplChar,                         // Ptr to input string
                                                     stFlags.ImmType,                   // Immediate type
                                                    &lpSymEntry->stData.stLongest,      // Ptr to value to format
                                                     DEF_MAX_QUADPP_IEEE,               // Precision to use
                                                     UTF16_DOT,                         // Char to use as decimal separator
                                                     UTF16_BAR,                         // Char to use as overbar
                                                     FLTDISPFMT_RAWFLT,                 // Float display format
                                                     TRUE);                             // TRUE iff we're to substitute text for infinity
                                    // Restore user's precision
                                    lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_PP]->stData.stInteger = uQuadPP;

                                    // Delete the last blank in case it matters,
                                    //   and ensure properly terminated
                                    if (lpaplChar[-1] EQ L' ')
                                        *--lpaplChar = WC_EOS;
                                    else
                                        *lpaplChar = WC_EOS;
                                    break;

                                case IMMTYPE_CHAR:
                                    // Append a leading single quote
                                    *lpaplChar++ = WC_SQ;

                                    // Format the text as an ASCII string with non-ASCII chars
                                    //   represented as either {symbol} or {\xXXXX} where XXXX is
                                    //   a four-digit hex number.
                                    lpaplChar +=
                                      ConvertWideToNameLength (lpaplChar,                   // Ptr to output save buffer
                                                              &lpSymEntry->stData.stChar,   // Ptr to incoming chars
                                                               1);                          // # chars to convert
                                    // Append a trailing single quote
                                    *lpaplChar++ = WC_SQ;

                                    // Ensure properly terminated
                                    *lpaplChar = WC_EOS;

                                    break;

                                defstop
                                    break;
                            } // End SWITCH
                        } else
                            // Convert the variable in global memory to saved ws form
                            lpaplChar =
                              SavedWsFormGlbVar (lpaplChar,         // Ptr to output save area
                                                 stGlbData,         // WS object global memory handle
                                                 lpDict,            // Ptr to the dictionary
                                                &uGlbCnt,           // Ptr to [Globals] count
                                                 lpSymEntry,        // Ptr to this global's SYMENTRY
                                                &savedWsVar_CB);    // Ptr to Callback struct (may be NULL)
                        // If the string hasn't been saved into the dictionary, ...
                        if (savedWsVar_CB.iIndex EQ -1)
                            // Write out the entry (nnn=Name=value)
                            //   in the [Vars.sss] section
                            ProfileSetString (wszSectName,      // Ptr to the section name
                                              wszCount,         // Ptr to the key name
                                              lpwszTemp,        // Ptr to the key value
                                              lpDict);          // Ptr to the dictionary
                        break;

                    case NAMETYPE_FN0:
                    case NAMETYPE_FN12:
                    case NAMETYPE_OP1:
                    case NAMETYPE_OP2:
                    case NAMETYPE_OP3:
                    case NAMETYPE_TRN:
                        // Format the function section name as F nnn.Name where nnn is the count
                        wsprintfW (lpwszFormat,
                                   L"F %d.%s",
                                   uGlbCnt++,
                                   lpwszTemp);
                        // Append separator
                        *lpaplChar++ = L'=';

                        // Append the name type
                        *lpaplChar++ = L'0' + stFlags.stNameType;

                        // Append separator
                        *lpaplChar++ = L'=';

                        // If it's immediate, ...
                        if (stFlags.Imm)
                        {
                            APLCHAR aplChar;

                            // Translate from INDEX_xxx to UTF16_xxx
                            aplChar = TranslateFcnOprToChar (lpSymEntry->stData.stChar);

                            // Format the text as an ASCII string with non-ASCII chars
                            //   represented as either {symbol} or {\xXXXX} where XXXX is
                            //   a four-digit hex number.
                            lpaplChar +=
                              ConvertWideToNameLength (lpaplChar,   // Ptr to output save buffer
                                                      &aplChar,     // Ptr to incoming chars
                                                       1);          // # chars to convert
                        } else
                            // Convert the function/operator in global memory to saved ws form
                            lpaplChar =
                              SavedWsFormGlbFcn (lpaplChar,                     // Ptr to output save area
                                                 lpwszFormat,                   // Ptr to the function section name as F nnn.Name where nnn is the count
                                                 lpwszTemp,                     // Ptr to the function name (for FCNARRAY_HEADER only)
                                                 lpSymEntry->stData.stGlbData,  // WS object global memory handle
                                                 lpDict,                        // Ptr to saved WS file DPFE
                                                &uGlbCnt,                       // Ptr to [Globals] count
                                                 lpSymEntry);                   // Ptr to this global's SYMENTRY
                        // Format the counter
                        MySprintfW (wszCount,
                                    sizeof (wszCount),
                                   L"%d",
                                    (*lpMemCnt)[lpSymEntry->stSILevel][1]++);
                        // Format the section name as [Fcns.sss] where sss is the SI level
                        MySprintfW (wszSectName,
                                    sizeof (wszSectName),
                                    SECTNAME_FCNS L".%d",
                                    lpSymEntry->stSILevel);
                        // Write out the entry (nnn=Name=Type=Value)
                        //   in the [Fcns.sss] section
                        ProfileSetString (wszSectName,      // Ptr to the section name
                                          wszCount,         // Ptr to the key name
                                          lpwszTemp,        // Ptr to the key value
                                          lpDict);          // Ptr to the dictionary
                        break;

                    defstop
                        break;
                } // End SWITCH
            } else
            if (lpSymEntry->stSILevel NE 0) // Must be suspended
            {
                // Format the section name
                MySprintfW (wszSectName,
                            sizeof (wszSectName),
                            SECTNAME_VARS L".%d",
                            lpSymEntry->stSILevel);
                // Write out the entry (nnn = Name = value)
                //   in the [Vars.sss] section
                ProfileSetString (wszSectName,      // Ptr to the section name
                                  lpwszTemp,        // Ptr to the key name
                                 L"",               // Ptr to the key value
                                  lpDict);          // Ptr to the dictionary
            } // End IF/ELSE/...
        } // End FOR/IF/...
    } __except (CheckException (GetExceptionInformation (), L"CmdSave_EM #1"))
    {
        EXIT_TEMP_OPEN          // lpwszTemp is used in CleanUpAfterSav

        // Clean up after )SAVE
        CleanupAfterSave (lpMemPTD, lpMemCnt, lpDict, uGlbCnt);

        CHECK_TEMP_OPEN         // Restore flag

        // Split cases based upon the ExceptionCode
        switch (MyGetExceptionCode ())
        {
            case EXCEPTION_LIMIT_ERROR:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                goto LIMIT_EXIT;

            default:
                // Display message for unhandled exception
                DisplayException ();

                break;
        } // End SWITCH
    } // End __try/__except

    // Delete the FMTSTR_GLBOBJ=FMTSTR_GLBCNT references in the [TempGlobals] section
    // In fact, delete the entire section
    ProfileDelSection (SECTNAME_TEMPGLOBALS,    // Ptr to the section name
                       lpDict);                 // Ptr to the dictionary
    EXIT_TEMP_OPEN          // lpwszTemp is used in CleanUpAfterSave

    // Clean up after )SAVE
    CleanupAfterSave (lpMemPTD, lpMemCnt, lpDict, uGlbCnt);

    // Get the length of the []WSID excluding WS_WKSEXT
    iCmp = lstrlenW (lpMemSaveWSID) - WS_WKSEXT_LEN;

    // Omit the trailing WS_WKSEXT
    Assert (lpMemSaveWSID[iCmp] EQ L'.');
    lpMemSaveWSID[iCmp] = WC_EOS;

    CHECK_TEMP_OPEN         // Restore flag

    // Copy the (possibly shortened WSID)
    strcpyW (lpwszTemp, ShortenWSID (lpMemSaveWSID));

    // Restore trailing WS_WKSEXT
    lpMemSaveWSID[iCmp] = L'.';

    // Copy the " SAVED " text
    strcatW (lpwszTemp, L" SAVED ");

    // Format the current date & time
    FormatCurDateTime (&lpwszTemp[lstrlenW (lpwszTemp)]);
#ifdef DEBUG
    // Append the workspace version
    wsprintfW (&lpwszTemp[lstrlenW (lpwszTemp)],
               L" (ver %s)",
                WS_VERSTR);
#endif
    // Protect the text
    lpwszOrigTemp = lpMemPTD->lpwszTemp;
    lpMemPTD->lpwszTemp += lstrlenW (lpMemPTD->lpwszTemp);

    // Save lpMemSaveWSID as the value of the new []WSID
    if (!SaveNewWsid_EM (lpMemSaveWSID))
        goto ERROR_EXIT;

    // Write out to the .ini file
    if (!ProfileWrite (lpDict))
        goto LIMIT_EXIT;

    EXIT_TEMP_OPEN

    // Write out the WSID and SAVED date & time
    ReplaceLastLineCRPmt (lpwszTemp);

    // Restore the original ptr
    lpMemPTD->lpwszTemp = lpwszOrigTemp;

    CHECK_TEMP_OPEN

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

ERRMSG_EXIT:
    ReplaceLastLineCRPmt (lpwErrMsg);

    goto ERROR_EXIT;

WSFULL_EXIT:
    // Display the error message
    ReplaceLastLineCRPmt (ERRMSG_WS_FULL APPEND_NAME);

    goto ERROR_EXIT;

LIMIT_EXIT:
    // Display the error message
    ReplaceLastLineCRPmt (ERRMSG_LIMIT_ERROR APPEND_NAME);

    goto ERROR_EXIT;

NOTSAVED_CLEAR_EXIT:
    // Display the error message
    ReplaceLastLineCRPmt (ERRMSG_NOT_SAVED_CLEAR_WS APPEND_NAME);

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (lpMemOldWSID NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_WSID]->stData.stGlbData); lpMemOldWSID = NULL;
    } // End IF

    // Unlock and free (and set to NULL) a global name and ptr
    UnlFreeGlbName (hGlbCnt, lpMemCnt);

    // If there's a dictionary, ...
    if (lpDict NE NULL)
    {
        // Free the dictionary
        ProfileUnload (lpDict); lpDict = NULL;
    } // End IF

    EXIT_TEMP_OPEN

    // Restore the OptionFlags values
    OptionFlags.bDispMPSuf = bDispMPSuf;

    return bRet;
} // End CmdSave_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CleanupAfterSave
//
//  Execute cleanup code after a )SAVE (successful or not)
//***************************************************************************

void CleanupAfterSave
    (LPPERTABDATA lpMemPTD,             // Ptr to PerTabData global memory
     UINT         (*lpMemCnt)[][2],     // Ptr to Vars/Fcns counters, [0] = Vars, [1] = Fcns
     LPDICTIONARY lpDict,               // Ptr to the dictionary
     UINT         uGlbCnt)              // # entries in [Globals] section

{
    UINT       uCnt;                    // Loop counter
    WCHAR      wszSectName[15],         // Save area for section name (e.g., [Vars.sss])
               wszCount[8],             // ...           formatted uSymxxx counter
               wszTimeStamp[16 + 1];    // ...           ...       time stamps
    SYSTEMTIME systemTime;              // Current system (UTC) time
    FILETIME   ftCreation;              // Creation time

    // Loop through the SI levels
    for (uCnt = 0; uCnt < (lpMemPTD->SILevel + 1); uCnt++)
    {
        // Format the section name as [Vars.sss] where sss is the SI level
        MySprintfW (wszSectName,
                    sizeof (wszSectName),
                    SECTNAME_VARS L".%d",
                    uCnt);
        // Format the Var count
        MySprintfW (wszCount,
                    sizeof (wszCount),
                   L"%d",
                    (*lpMemCnt)[uCnt][0]);
        // Write out the Var count to the [Vars.sss] section where sss is the SI level
        ProfileSetString (wszSectName,          // Ptr to the section name
                          KEYNAME_COUNT,        // Ptr to the key name
                          wszCount,             // Ptr to the key value
                          lpDict);              // Ptr to the dictionary
        // Format the section name as [Fcns.sss] where sss is the SI level
        MySprintfW (wszSectName,
                    sizeof (wszSectName),
                    SECTNAME_FCNS L".%d",
                    uCnt);
        // Format the Fcn/Opr count
        MySprintfW (wszCount,
                    sizeof (wszCount),
                   L"%d",
                    (*lpMemCnt)[uCnt][1]);
        // Write out the Fcn count to the [Fcns.sss] section where sss is the SI level
        ProfileSetString (wszSectName,          // Ptr to the section name
                          KEYNAME_COUNT,        // Ptr to the key name
                          wszCount,             // Ptr to the key value
                          lpDict);              // Ptr to the dictionary
    } // End FOR

    // Format the SI level
    MySprintfW (wszCount,
                sizeof (wszCount),
               L"%d",
                lpMemPTD->SILevel);
    // Write out the SI level to the [General] section
    ProfileSetString (SECTNAME_GENERAL,     // Ptr to the section name
                      KEYNAME_SILEVEL,      // Ptr to the key name
                      wszCount,             // Ptr to the key value
                      lpDict);              // Ptr to the dictionary
    // Format to []MF timer #
    MySprintfW (wszCount,
                sizeof (wszCount),
               L"%d",
                lpMemPTD->uQuadMF);
    // Write out the []MF timer # to the [General] section
    ProfileSetString (SECTNAME_GENERAL,     // Ptr to the section name
                      KEYNAME_MFTIMER,      // Ptr to the key name
                      wszCount,             // Ptr to the key value
                      lpDict);              // Ptr to the dictionary
    // Format the [Globals] count
    MySprintfW (wszCount,
                sizeof (wszCount),
               L"%d",
                uGlbCnt);
    // Write out the count to the [Globals] section
    ProfileSetString (SECTNAME_GLOBALS,     // Ptr to the section name
                      KEYNAME_COUNT,        // Ptr to the key name
                      wszCount,             // Ptr to the key value
                      lpDict);              // Ptr to the dictionary
    // Write out the SI stack to the [SI] section
    CmdSiSinlCom_EM (L"",               // Ptr to command tail
                     FALSE,             // TRUE iff )SINL
                     lpDict);           // Ptr to the dictionary
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
    // Write out the creation time to the [General] section
    ProfileSetString (SECTNAME_GENERAL,         // Ptr to the section name
                      KEYNAME_CREATIONTIME,     // Ptr to the key name
                      wszTimeStamp,             // Ptr to the key value
                      lpDict);                  // Ptr to the dictionary
} // End CleanupAfterSave


//***************************************************************************
//  $SavedWsFormGlbFcn
//
//  Convert a function/operator in global memory to saved ws form
//***************************************************************************

LPAPLCHAR SavedWsFormGlbFcn
    (LPAPLCHAR    lpaplChar,            // Ptr to output save area
     LPAPLCHAR    lpwszFcnTypeName,     // Ptr to the function section name as F nnn.Name where nnn is the count
     LPAPLCHAR    lpwszFcnName,         // Ptr to the function name (for FCNARRAY_HEADER only)
     HGLOBAL      hGlbObj,              // WS object global memory handle
     LPDICTIONARY lpDict,               // Ptr to the dictionary
     LPUINT       lpuGlbCnt,            // Ptr to [Globals] count
     LPSYMENTRY   lpSymEntry)           // Ptr to this global's SYMENTRY

{
    LPVOID            lpMemObj = NULL;          // Ptr to WS object ...
    LPAPLCHAR         lpMemProKeyName,          // Ptr to profile keyname
                      lpaplCharStart,           // Ptr to start of buffer
                      lpaplCharStart2,          // Ptr to start of buffer
                      lpwszSectName;            // Ptr to the section name as nnn.Name where nnn is the count
    UINT              uLine,                    // Function line loop counter
                      uCnt,                     // Loop counter
                      uLen,                     // Loop length
                      uGlbCnt;                  // # entries in [Globals] section
    FILETIME          ftCreation,               // Object creation time
                      ftLastMod;                // ...    last modification time
    WCHAR             wszGlbObj[1 + 16 + 1],    // Save area for formatted hGlbObj
                                                //   (room for FMTCHR_LEAD, 64-bit ptr, and terminating zero)
                      wszGlbCnt[8 + 1];         // Save area for formatted *lpuGlbCnt
    SAVEDWSGLBVARPARM SavedWsGlbVarParm;        // Extra parms for SavedWsGlbVarConv
    SAVEDWSGLBFCNPARM SavedWsGlbFcnParm;        // Extra parms for SavedWsGlbFcnConv

    Assert (IsGlbTypeFcnDir_PTB (hGlbObj)
         || IsGlbTypeDfnDir_PTB (hGlbObj));

    // Save ptr to start of buffer
    lpaplCharStart = lpaplChar;

    // Format the hGlbObj
    MySprintfW (wszGlbObj,
                sizeof (wszGlbObj),
                FMTSTR_GLBOBJ,
                hGlbObj);
    // Save as ptr to the profile keyname
    lpMemProKeyName = lpaplChar;

    // Check to see if this global has already been saved
    //   in the [TempGlobals] section
    if (ProfileCopyString (SECTNAME_TEMPGLOBALS,        // Ptr to the section name
                           wszGlbObj,                   // Ptr to the key name
                           NULL,                        // Default value if not found
                           lpaplChar,                   // Ptr to the output buffer
                           8 * sizeof (lpaplChar[0]),   // Byte size of the output buffer
                           lpDict))                     // Ptr to the file name
        goto NORMAL_EXIT;

    // Get the current count as we might define several functions here
    //   and count in another entry
    uGlbCnt = (*lpuGlbCnt)++;

    // Lock the memory to get a ptr to it
    lpMemObj = MyGlobalLock (hGlbObj);

    // Split cases based upon the object's signature
    switch (GetSignatureMem (lpMemObj))
    {
        case FCNARRAY_HEADER_SIGNATURE:
#define lpMemFcnHdr     ((LPFCNARRAY_HEADER) lpMemObj)
            // Skip over the 'F ' to point to the section name
            lpwszSectName = &lpwszFcnTypeName[2];

            // Fill in the extra parms
            SavedWsGlbVarParm.lpDict           = lpDict;
            SavedWsGlbVarParm.lpuGlbCnt        = lpuGlbCnt;
            SavedWsGlbVarParm.lpSymEntry       = lpSymEntry;
            SavedWsGlbVarParm.lplpSymLink      = NULL;

            SavedWsGlbFcnParm.lpDict           = lpDict;
            SavedWsGlbFcnParm.lpwszFcnTypeName = &lpwszFcnTypeName[lstrlenW (lpwszFcnTypeName) + 1];
            SavedWsGlbFcnParm.lpuGlbCnt        = lpuGlbCnt;
            SavedWsGlbFcnParm.lpSymEntry       = lpSymEntry;
            SavedWsGlbFcnParm.lplpSymLink      = NULL;

            // Call the common function display function
            lpaplChar =
              DisplayFcnGlb (lpaplCharStart,        // Ptr to output save area
                             hGlbObj,               // Function array global memory handle
                             FALSE,                 // TRUE iff we're to display the header
                            &SavedWsGlbVarConv,     // Ptr to function to convert an HGLOBAL to {{nnn}} (may be NULL)
                            &SavedWsGlbVarParm,     // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                            &SavedWsGlbFcnConv,     // Ptr to function to convert an HGLOBAL to {{nnn}} (may be NULL)
                            &SavedWsGlbFcnParm);    // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
            // Ensure properly terminated
            *lpaplChar++ = WC_EOS;

            // Save as new start of buffer
            lpaplCharStart2 = lpaplChar;

            // Format the text as an ASCII string with non-ASCII chars
            //   represented as either {symbol} or {\xXXXX} where XXXX is
            //   a four-digit hex number.
            lpaplChar +=
              ConvertWideToNameLength (lpaplChar,                   // Ptr to output save buffer
                                       lpaplCharStart,              // Ptr to incoming chars
                                       lstrlenW (lpaplCharStart));  // # chars to convert
            // Ensure properly terminated
            *lpaplChar++ = WC_EOS;

            // Copy creation time
            ftCreation = ((LPFCNARRAY_HEADER) lpMemObj)->ftCreation;

            // Copy last mod time
            ftLastMod  = ((LPFCNARRAY_HEADER) lpMemObj)->ftLastMod;

            // Create the [nn.Name] section
            ProfileSetSection (lpwszSectName,   // Ptr to the section name
                               lpDict);         // Ptr to the dictionary
            // Write out the single line
            ProfileSetString (lpwszSectName,        // Ptr to the section name
                             L"0",                  // Ptr to the key name
                              lpaplCharStart2,      // Ptr to the key value
                              lpDict);              // Ptr to the dictionary
            // Write out the Count key value
            ProfileSetString (lpwszSectName,        // Ptr to the section name
                              KEYNAME_COUNT,        // Ptr to the key name
                             L"1",                  // Ptr to the key value
                              lpDict);              // Ptr to the dictionary
            // Write out the UserDefined key value
            ProfileSetString (lpwszSectName,        // Ptr to the section name
                              KEYNAME_USERDEFINED,  // Ptr to the key name
                             L"0",                  // Ptr to the key value
                              lpDict);              // Ptr to the dictionary
            break;
#undef  lpMemFcnHdr

        case DFN_HEADER_SIGNATURE:
        {
            UINT      numFcnLines;
            LPFCNLINE lpFcnLines;       // Ptr to array of function line structs (FCNLINE[numFcnLines])

#define lpMemDfnHdr     ((LPDFN_HEADER) lpMemObj)
            // Format the function section name as F nnn.Name where nnn is the count
            wsprintfW (lpwszFcnTypeName,
                       L"F %d.",
                       uGlbCnt);
            // If this is an AFO, ...
            if (lpMemDfnHdr->bAFO)
            {
                LPWCHAR p;

                // Find the '=' separator
                p = SkipToCharW (lpwszFcnName, '=');

                // Zap it
                *p = WC_EOS;

                // Append the function name
                strcpyW (&lpwszFcnTypeName[lstrlenW (lpwszFcnTypeName)], lpwszFcnName);

                // Restore the zapped char
                *p = '=';

                // Return the pointer to the next char in the buffer
                lpwszFcnName = &lpwszFcnTypeName[lstrlenW (lpwszFcnTypeName)];
            } else
            {
                // Skip over the leading part
                lpwszFcnName = &lpwszFcnTypeName[lstrlenW (lpwszFcnTypeName)];

                // Copy and convert the function name from the STE
                lpwszFcnName =
                  ConvSteName (lpwszFcnName,            // Ptr to output area
                               lpMemDfnHdr->steFcnName, // Ptr to function STE
                               NULL);                   // Ptr to name length (may be NULL)
            } // End IF/ELSE

            // Append separator
            *lpaplChar++ = L'=';

            // Append the name type
            *lpaplChar++ = L'0' + lpMemDfnHdr->steFcnName->stFlags.stNameType;

            // Append separator
            *lpaplChar++ = L'=';

            // Skip over the 'F ' to point to the section name
            lpwszSectName = &lpwszFcnTypeName[2];

            // Put the function definition in a separate
            //   section named [nnn.Name] with each line
            //   of the function on a separate line as in
            //   0 = <Header>
            //   1 = <First Line>, etc.
            //   CreationTime = xxxxxxxxxxxxxxxx (64-bit hex number)

            // Save the # function lines
            numFcnLines = lpMemDfnHdr->numFcnLines;

            // Get ptr to array of function line structs (FCNLINE[numFcnLines])
            lpFcnLines = (LPFCNLINE) ByteAddr (lpMemDfnHdr, lpMemDfnHdr->offFcnLines);

            // Create the [nn.Name] section
            ProfileSetSection (lpwszSectName,   // Ptr to the section name
                               lpDict);         // Ptr to the dictionary
            // Write out the function header
            WriteFunctionLine (lpwszSectName,   // Ptr to the section name
                               lpaplChar,       // Ptr to save area for the formatted line
                               0,               // Line #
                               lpMemDfnHdr->hGlbTxtHdr,
                               lpDict);         // Ptr to the dictionary
            // Loop through the function lines
            for (uLine = 0; uLine < numFcnLines; uLine++, lpFcnLines++)
                WriteFunctionLine (lpwszSectName,   // Ptr to the section name
                                   lpaplChar,       // Ptr to save area for the formatted line
                                   uLine + 1,       // Line #
                                   lpFcnLines->hGlbTxtLine,
                                   lpDict);         // Ptr to the dictionary
            // Format the function line count
            wsprintfW (lpaplChar,
                       L"%d",
                       numFcnLines + 1);
            // Write out the Count key value
            ProfileSetString (lpwszSectName,        // Ptr to the section name
                              KEYNAME_COUNT,        // Ptr to the key name
                              lpaplChar,            // Ptr to the key value
                              lpDict);              // Ptr to the dictionary
            // Write out the UserDefined key value
            ProfileSetString (lpwszSectName,        // Ptr to the section name
                              KEYNAME_USERDEFINED,  // Ptr to the key name
                             L"1",                  // Ptr to the key value
                              lpDict);              // Ptr to the dictionary
            // If it's an AFO, ...
            if (lpMemDfnHdr->bAFO)
                // Write out the bAFO key value
                ProfileSetString (lpwszSectName,    // Ptr to the section name
                                  KEYNAME_AFO,      // Ptr to the key name
                                 L"1",              // Ptr to the key value
                                  lpDict);          // Ptr to the dictionary
#define lpUndoIni       lpaplChar       // Start of output save area

            // Write out the Undo buffer
            if (lpMemDfnHdr->hGlbUndoBuff NE NULL)
            {
                LPUNDO_BUF lpMemUndo;               // Ptr to Undo Buffer global memory
                SIZE_T     uUndoCount;              // # entries in the Undo Buffer
                LPAPLCHAR  lpUndoOut;               // Ptr to output save area
                WCHAR      wcTmp[2];                // Temporary char string

                // Ensure properly terminated
                wcTmp[1] = WC_EOS;

                // Get the # entries
                uUndoCount = (MyGlobalSize (lpMemDfnHdr->hGlbUndoBuff)) / sizeof (UNDO_BUF);

                // Start with the # entries
                lpUndoOut = lpUndoIni +
                  wsprintfW (lpUndoIni,
                             L"%d ",
                             uUndoCount);
                // Lock the memory to get a ptr to it
                lpMemUndo = MyGlobalLock (lpMemDfnHdr->hGlbUndoBuff);

                // Loop through the undo entries
                for (;
                     uUndoCount;
                     uUndoCount--, lpMemUndo++)
                {
                    LPAPLCHAR lpUndoChar;

                    // Split cases based upon the undo char
                    switch (lpMemUndo->Char)
                    {
                        case WC_EOS:
                            lpUndoChar = L"";

                            break;

                        case WC_LF:
                            lpUndoChar = WS_SLOPE L"n";

                            break;

                        case WC_CR:
                            lpUndoChar = WS_SLOPE L"r";

                            break;

                        case WC_SQ:
                            lpUndoChar = WS_SLOPE WS_SQ;

                            break;

                        case WC_SLOPE:
                            lpUndoChar = WS_SLOPE;

                            break;

                        default:
                            // Copy to string
                            if ((lpUndoChar = CharToSymbolName (lpMemUndo->Char)) EQ NULL)
                            {
                                // Save as one-char string
                                wcTmp[0] = lpMemUndo->Char;
                                lpUndoChar = wcTmp;
                            } // End IF

                            break;
                    } // End SWITCH

                    // Format this element of the Undo Buffer
                    lpUndoOut +=
                      wsprintfW (lpUndoOut,
                                 (*lpUndoChar EQ WC_EOS) ? L"%c %d %d %d, "
                                                         : L"%c %d %d %d '%s', ",
                                 UndoActToChar[lpMemUndo->Action],
                                 lpMemUndo->CharPosBeg,
                                 lpMemUndo->CharPosEnd,
                                 lpMemUndo->Group,
                                 lpUndoChar);
                } // End FOR

                // We no longer need this ptr
                MyGlobalUnlock (lpMemDfnHdr->hGlbUndoBuff); lpMemUndo = NULL;

                // Ensure properly terminated (zap the trailing comma)
                if (lpUndoOut[-2] EQ L',')
                    lpUndoOut[-2] = WC_EOS;
                else
                    lpUndoOut[ 0] = WC_EOS;
            } else
                // Ensure properly terminated
                strcpyW (lpUndoIni, L"0");

            // Write out the Undo Buffer contents
            ProfileSetString (lpwszSectName,    // Ptr to the section name
                              KEYNAME_UNDO,     // Ptr to the key name
                              lpUndoIni,        // Ptr to the key value
                              lpDict);          // Ptr to the dictionary
            // ***FINISHME*** -- Write out the stop/trace bits










            if (lpMemDfnHdr->hGlbMonInfo NE NULL)
            {
                LPINTMONINFO lpMemMonInfo;          // Ptr to function line monitoring info
                LPAPLCHAR    lpaplCharMon;          // Ptr to formatted monitor info

                // Lock the memory to get a ptr to it
                lpMemMonInfo = MyGlobalLock (lpMemDfnHdr->hGlbMonInfo);

                // Get the loop length
                uLen = numFcnLines + 1;

                // Save the ptr
                lpaplCharMon = lpaplChar;

                // Format the monitor info as integers
                for (uCnt = 0; uCnt < uLen; uCnt++, lpMemMonInfo++)
                {
                    // Format the IncSubFns value
                    lpaplCharMon =
                      FormatAplIntFC (lpaplCharMon,             // Ptr to output save area
                                      lpMemMonInfo->IncSubFns,  // The value to format
                                      UTF16_OVERBAR);           // Char to use as overbar
                    // Format the ExcSubFns value
                    lpaplCharMon =
                      FormatAplIntFC (lpaplCharMon,             // Ptr to output save area
                                      lpMemMonInfo->ExcSubFns,  // The value to format
                                      UTF16_OVERBAR);           // Char to use as overbar
                    // Format the Count value
                    lpaplCharMon =
                      FormatAplIntFC (lpaplCharMon,             // Ptr to output save area
                                      lpMemMonInfo->Count,      // The value to format
                                      UTF16_OVERBAR);           // Char to use as overbar
                    // Append a separator
                    lpaplCharMon[-1] = L',';
                } // End FOR

                // We no longer need this ptr
                MyGlobalUnlock (lpMemDfnHdr->hGlbMonInfo); lpMemMonInfo = NULL;

                // Ensure properly terminated and zap the trailing blank
                lpaplCharMon[-1] = WC_EOS;

                // Write out the Monitor Info
                ProfileSetString (lpwszSectName,    // Ptr to the section name
                                  KEYNAME_MONINFO,  // Ptr to the key name
                                  lpaplChar,        // Ptr to the key value
                                  lpDict);          // Ptr to the dictionary
            } // End IF

            // Copy creation time
            ftCreation = lpMemDfnHdr->ftCreation;

            // Copy last modification time
            ftLastMod  = lpMemDfnHdr->ftLastMod;
#undef  lpMemDfnHdr
            break;
        } // End DFN_HEADER_SIGNATURE

        defstop
            break;
    } // End SWITCH

    // Save the ftCreation time
    wsprintfW (lpaplChar,
               FMTSTR_DATETIME,
               ftCreation.dwHighDateTime,
               ftCreation.dwLowDateTime);
    ProfileSetString (lpwszSectName,        // Ptr to the section name
                      KEYNAME_CREATIONTIME, // Ptr to the key name
                      lpaplChar,            // Ptr to the key value
                      lpDict);              // Ptr to the dictionary
    // Save the ftLastMod time
    wsprintfW (lpaplChar,
               FMTSTR_DATETIME,
               ftLastMod.dwHighDateTime,
               ftLastMod.dwLowDateTime);
    ProfileSetString (lpwszSectName,        // Ptr to the section name
                      KEYNAME_LASTMODTIME,  // Ptr to the key name
                      lpaplChar,            // Ptr to the key value
                      lpDict);              // Ptr to the dictionary
    // Format the global count
    MySprintfW (wszGlbCnt,
                sizeof (wszGlbCnt),
                FMTSTR_GLBCNT,
                uGlbCnt);
    // Write out the entry in the [Globals] section
    ProfileSetString (SECTNAME_GLOBALS,     // Ptr to the section name
                      wszGlbCnt,            // Ptr to the key value
                      lpwszFcnTypeName,     // Ptr to the key name
                      lpDict);              // Ptr to the dictionary
    // Write out the saved marker
    ProfileSetString (SECTNAME_TEMPGLOBALS, // Ptr to the section name
                      wszGlbObj,            // Ptr to the key name
                      wszGlbCnt,            // Ptr to the key value
                      lpDict);              // Ptr to the dictionary
    // Move back to the start
    lpaplChar = lpaplCharStart;

    // Copy the formatted GlbCnt to the start of the buffer as the result
    strcpyW (lpaplChar, wszGlbCnt);
NORMAL_EXIT:
    if (hGlbObj NE NULL && lpMemObj NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbObj); lpMemObj = NULL;
    } // End IF

    // Return a ptr to the profile keyname's terminating zero
    return &lpaplChar[lstrlenW (lpaplChar)];
} // End SavedWsFormGlbFcn


//***************************************************************************
//  $WriteFunctionLine
//
//  Write out a function line to the saved WS file
//***************************************************************************

void WriteFunctionLine
    (LPWCHAR      lpwszSectName,    // Ptr to section name
     LPWCHAR      lpwFormat,        // Ptr to output save area for the formatted function line
     UINT         uLineNum,         // The line #
     HGLOBAL      hGlbTxtLine,      // Line text global memory handle
     LPDICTIONARY lpDict)           // Ptr to the dictionary

{
    LPMEMTXT_UNION lpMemTxtLine;
    WCHAR          wszCount[10];

    // Format the line number
    MySprintfW (wszCount,
                sizeof (wszCount),
               L"%d",
                uLineNum);
    // Lock the memory to get a ptr to it
    lpMemTxtLine = MyGlobalLockTxt (hGlbTxtLine);

    // Format the text as an ASCII string with non-ASCII chars
    //   represented as either {symbol} or {\xXXXX} where XXXX is
    //   a four-digit hex number.
    ConvertWideToNameLength (lpwFormat,                     // Ptr to output save buffer
                            &lpMemTxtLine->C,               // Ptr to incoming chars
                             lpMemTxtLine->U);              // # chars to convert
    // We no longer need this ptr
    MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;

    // Write out the entry (nnn = FunctionLine)
    ProfileSetString (lpwszSectName,        // Ptr to the section name
                      wszCount,             // Ptr to the key name
                      lpwFormat,            // Ptr to the key value
                      lpDict);              // Ptr to the dictionary
} // End WriteFunctionLine


//***************************************************************************
//  $SavedWsFormGlbVar
//
//  Convert a variable in global memory to saved ws form
//***************************************************************************

LPAPLCHAR SavedWsFormGlbVar
    (LPAPLCHAR       lpaplChar,         // Ptr to output save area
     HGLOBAL         hGlbObj,           // WS object global memory handle
     LPDICTIONARY    lpDict,            // Ptr to the dictionary
     LPUINT          lpuGlbCnt,         // Ptr to [Globals] count
     LPSYMENTRY      lpSymEntry,        // Ptr to this global's SYMENTRY
     LPSAVEDWSVAR_CB lpSavedWsVar_CB)   // Ptr to Callback struct (may be NULL)

{
    APLSTYPE     aplTypeObj;            // WS object storage type
    APLNELM      aplNELMObj;            // ...       NELM
    APLRANK      aplRankObj;            // ...       rank
    APLINT       apaOff,                // APA offset
                 apaMul;                // ... multiplier
    APLUINT      uObj,                  // WS object loop counter
                 uQuadPP,               // []PP save area
                 uCommPrec;             // Common precision for an array of VFP numbers (0 if none)
    LPVOID       lpMemObj = NULL;       // Ptr to WS object global memory
    LPAPLCHAR    lpMemProKeyName,       // Ptr to profile keyname
                 lpaplCharStart;        // Ptr to start of buffer
    UINT         uBitIndex;             // Bit index when marching through Booleans
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    PERM_NDX     permNdx;               // Permanent object index
    STFLAGS      stFlags;               // Object SymTab flags
    WCHAR        wszGlbObj[1 + 16 + 1], // Save area for formatted hGlbObj
                                        //   (room for FMTCHR_LEAD, 64-bit ptr, and terminating zero)
                 wszGlbCnt[8 + 1];      // Save area for formatted *lpuGlbCnt
    UBOOL        bUsrDfn;               // TRUE iff the object is a user-defined function/operator

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Mark as not saved into workspace
    lpSavedWsVar_CB->iIndex = -1;

    // Check on user-defined functions/operators
    bUsrDfn = IsGlbTypeDfnDir_PTB (hGlbObj);

    // stData is a valid HGLOBAL variable array
    //   or user-defined function/operator
    Assert (IsGlbTypeVarDir_PTB (hGlbObj)
         || bUsrDfn);

    // Format the hGlbObj
    MySprintfW (wszGlbObj,
                sizeof (wszGlbObj),
                FMTSTR_GLBOBJ,
                hGlbObj);
    // Check to see if this global has already been saved
    //   in the [Globals] section
    if (ProfileCopyString (SECTNAME_TEMPGLOBALS,        // Ptr to the section name
                           wszGlbObj,                   // Ptr to the key name
                           NULL,                        // Default value if not found
                           lpaplChar,                   // Ptr to the output buffer
                           8 * sizeof (lpaplChar[0]),   // Byte size of the output buffer
                           lpDict))                     // Ptr to the dictionary
    {
        if (bUsrDfn)
        {
            // Lock the memory to get a ptr to it
            lpMemObj = MyGlobalLockDfn (hGlbObj);

            // Copy the STE name instead as we don't use :nnn convention in
            //   function arrays
#define lpHeader        ((LPDFN_HEADER) lpMemObj)
            // ***FIXME*** -- We certainly use :nnn in function arrays as in Z{is}(L{jot}lmx)MonPower 2
            lpaplChar =
              CopySteName (lpaplChar,               // Ptr to output area
                           lpHeader->steFcnName,    // Ptr to function STE
                           NULL);                   // Ptr to name length (may be NULL)
#undef  lpHeader
            // Ensure properly terminated
            *lpaplChar = WC_EOS;
        } // End IF

        goto NORMAL_EXIT;
    } // End IF

    // Save as ptr to the profile keyname
    lpMemProKeyName = lpaplChar;

    // Save ptr to start of buffer
    lpaplCharStart = lpaplChar;

    // Append the marker as a variable
    *lpaplChar++ = L'V';
    *lpaplChar++ = L' ';

    // Lock the memory to get a ptr to it
    lpMemObj = MyGlobalLockVar (hGlbObj);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemObj)
    // Get the array attributes
    aplTypeObj = lpHeader->ArrType;
    aplNELMObj = lpHeader->NELM;
    aplRankObj = lpHeader->Rank;
    permNdx    = lpHeader->PermNdx;

    // The array values are preceded by the array
    //   attributes in the form of
    //   StorageType   NELM   Rank  Shape
    //   "BIFCHNLA"     %d     %d   *(%d )
    //   %c %d %d %*(d )
    lpaplChar =
      AppendArrayHeader (lpaplChar,                             // Ptr to output save area
                         TranslateArrayTypeToChar (aplTypeObj), // Object storage type as WCHAR
                         aplNELMObj,                            // Object NELM
                         aplRankObj,                            // Object rank
                         VarArrayBaseToDim (lpHeader),          // Ptr to object dimensions
                        &uCommPrec,                             // Ptr to common VFP array precision (0 if none) (may be NULL)
                         lpHeader);                             // Ptr to array header
#undef  lpHeader
    // Skip over the header and dimensions to the data
    lpMemObj = VarArrayDataFmBase (lpMemObj);

    __try
    {
        // Split cases based the object storage type
        switch (aplTypeObj)
        {
            case ARRAY_BOOL:
                // Initialize
                uBitIndex = 0;

                // Loop through the array elements
                for (uObj = 0; uObj < aplNELMObj; uObj++)
                {
                    // Format the value
                    lpaplChar =
                      FormatAplIntFC (lpaplChar,                                        // Ptr to output save area
                                      BIT0 & ((*(LPAPLBOOL) lpMemObj) >> uBitIndex),    // The value to format
                                      UTF16_BAR);                                       // Char to use as overbar
                    // Check for end-of-byte
                    if (++uBitIndex EQ NBIB)
                    {
                        uBitIndex = 0;              // Start over
                        ((LPAPLBOOL) lpMemObj)++;   // Skip to next byte
                    } // End IF

////////////////////// If we're over our limit, ...
////////////////////// Note that <lpaplChar> points to the character after the trailing blank,
//////////////////////   so we must subtract one to get the correct string length.
////////////////////if (lpSavedWsVar_CB NE NULL
//////////////////// && ((lpaplChar - 1) - lpaplCharStart) > lpSavedWsVar_CB->iMaxLength)
////////////////////    CheckWsVarLimit (&lpaplChar, lpaplCharStart, &lpSavedWsVar_CB->iIndex, lpDict, lpSavedWsVar_CB);
                } // End FOR

////////////////// If the tail is not empty, ...
////////////////// Note that <lpaplChar> points to the character after the trailing blank,
//////////////////   so we must subtract one to get the correct string length.
////////////////if (lpSavedWsVar_CB NE NULL
//////////////// && ((lpaplChar - 1) - lpaplCharStart) > 0)
////////////////    CheckWsVarLimit (&lpaplChar, lpaplCharStart, &lpSavedWsVar_CB->iIndex, lpDict, lpSavedWsVar_CB);
                break;

            case ARRAY_INT:
                // Loop through the array elements
                for (uObj = 0; uObj < aplNELMObj; uObj++, ((LPAPLINT) lpMemObj)++)
                {
                    // Format the value
                    lpaplChar =
                      FormatAplIntFC (lpaplChar,            // Ptr to output save area
                                     *(LPAPLINT) lpMemObj,  // The value to format
                                      UTF16_BAR);           // Char to use as overbar
////////////////////// If we're over our limit, ...
////////////////////// Note that <lpaplChar> points to the character after the trailing blank,
//////////////////////   so we must subtract one to get the correct string length.
////////////////////if (lpSavedWsVar_CB NE NULL
//////////////////// && ((lpaplChar - 1) - lpaplCharStart) > lpSavedWsVar_CB->iMaxLength)
////////////////////    CheckWsVarLimit (&lpaplChar, lpaplCharStart, &lpSavedWsVar_CB->iIndex, lpDict, lpSavedWsVar_CB);
                } // End FOR

////////////////// If the tail is not empty, ...
////////////////// Note that <lpaplChar> points to the character after the trailing blank,
//////////////////   so we must subtract one to get the correct string length.
////////////////if (lpSavedWsVar_CB NE NULL
//////////////// && ((lpaplChar - 1) - lpaplCharStart) > 0)
////////////////    CheckWsVarLimit (&lpaplChar, lpaplCharStart, &lpSavedWsVar_CB->iIndex, lpDict, lpSavedWsVar_CB);
                break;

            case ARRAY_FLOAT:
                // Loop through the array elements
                for (uObj = 0; uObj < aplNELMObj; uObj++, ((LPAPLFLOAT) lpMemObj)++)
                {
                    // Format the value
                    lpaplChar =
                      FormatAplFltFC (lpaplChar,            // Ptr to output save area
                                    *(LPAPLFLOAT) lpMemObj, // The value to format
                                      DEF_MAX_QUADPP_IEEE,  // Precision to use
                                      UTF16_DOT,            // Char to use as decimal separator
                                      UTF16_BAR,            // Char to use as overbar
                                      FLTDISPFMT_RAWFLT,    // Float display format
                                      TRUE);                // TRUE iff we're to substitute text for infinity
////////////////////// If we're over our limit, ...
////////////////////// Note that <lpaplChar> points to the character after the trailing blank,
//////////////////////   so we must subtract one to get the correct string length.
////////////////////if (lpSavedWsVar_CB NE NULL
//////////////////// && ((lpaplChar - 1) - lpaplCharStart) > lpSavedWsVar_CB->iMaxLength)
////////////////////    CheckWsVarLimit (&lpaplChar, lpaplCharStart, &lpSavedWsVar_CB->iIndex, lpDict, lpSavedWsVar_CB);
                } // End FOR

////////////////// If the tail is not empty, ...
////////////////// Note that <lpaplChar> points to the character after the trailing blank,
//////////////////   so we must subtract one to get the correct string length.
////////////////if (lpSavedWsVar_CB NE NULL
//////////////// && ((lpaplChar - 1) - lpaplCharStart) > 0)
////////////////    CheckWsVarLimit (&lpaplChar, lpaplCharStart, &lpSavedWsVar_CB->iIndex, lpDict, lpSavedWsVar_CB);
                break;

            case ARRAY_CHAR:
                // Append a leading single quote
                *lpaplChar++ = WC_SQ;

                // Loop through the array elements
                for (uObj = 0; uObj < aplNELMObj; uObj++, ((LPAPLCHAR) lpMemObj)++)
                {
                    // Format the text as an ASCII string with non-ASCII chars
                    //   represented as either {symbol} or {\xXXXX} where XXXX is
                    //   a four-digit hex number.
                    lpaplChar +=
                      ConvertWideToNameLength (lpaplChar,           // Ptr to output save buffer
                                               (LPAPLCHAR) lpMemObj,// Ptr to incoming chars
                                               1);                  // # chars to convert
////////////////////// If we're over our limit, ...
////////////////////if (lpSavedWsVar_CB NE NULL
//////////////////// && (lpaplChar - lpaplCharStart) > lpSavedWsVar_CB->iMaxLength)
////////////////////    CheckWsVarLimit (&lpaplChar, lpaplCharStart, &lpSavedWsVar_CB->iIndex, lpDict, lpSavedWsVar_CB);
                } // End FOR

                // Append a trailing single quote
                *lpaplChar++ = WC_SQ;

////////////////// If the tail is not empty, ...
////////////////if (lpSavedWsVar_CB NE NULL
//////////////// && (lpaplChar - lpaplCharStart) > 0)
////////////////    CheckWsVarLimit (&lpaplChar, lpaplCharStart, &lpSavedWsVar_CB->iIndex, lpDict, lpSavedWsVar_CB);
                break;

            case ARRAY_APA:
#define lpAPA       ((LPAPLAPA) lpMemObj)
                // Get the APA parameters
                apaOff = lpAPA->Off;
                apaMul = lpAPA->Mul;
#undef  lpAPA
                // Format as an APA

                // Append the offset
                lpaplChar =
                  FormatAplIntFC (lpaplChar,        // Ptr to output save area
                                  apaOff,           // The value to format
                                  UTF16_BAR);       // Char to use as overbar
                // Append the multiplier
                lpaplChar =
                  FormatAplIntFC (lpaplChar,        // Ptr to output save area
                                  apaMul,           // The value to format
                                  UTF16_BAR);       // Char to use as overbar
                break;

            case ARRAY_NESTED:
                // Take into account nested prototypes
                aplNELMObj = max (aplNELMObj, 1);

                // Fall through to common code

            case ARRAY_HETERO:
                // Write out each element as T N R S V

                // Loop through the array elements
                for (uObj = 0; uObj < aplNELMObj; uObj++, ((LPAPLNESTED) lpMemObj)++)
                // Split case based upon the ptr type
                switch (GetPtrTypeInd (lpMemObj))
                {
                    case PTRTYPE_STCONST:
#define lpSymEntry      (*(LPAPLHETERO) lpMemObj)

                        // Get the SymTab flags
                        stFlags = lpSymEntry->stFlags;

                        // Append the header for the simple scalar
                        lpaplChar =
                          AppendArrayHeader (lpaplChar,                         // Ptr to output save area
                                             TranslateImmTypeToChar (stFlags.ImmType),// Object storage type as WCHAR
                                             1,                                 // Object NELM
                                             0,                                 // Object rank
                                             NULL,                              // Ptr to object dimensions
                                             NULL,                              // Ptr to common VFP array precision (0 if none) (may be NULL)
                                             NULL);                             // Ptr to array header
                        // Split cases based upon the immediate type
                        switch (stFlags.ImmType)
                        {
                            case IMMTYPE_BOOL:
                            case IMMTYPE_INT:
                            case IMMTYPE_FLOAT:
                                // Ensure we format with full precision in case it's floating point
                                uQuadPP = lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_PP]->stData.stInteger;
                                if (IsImmFlt (stFlags.ImmType))
                                    lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_PP]->stData.stInteger = DEF_MAX_QUADPP_IEEE;

                                // Format the value
                                lpaplChar =
                                  FormatImmedFC (lpaplChar,                             // Ptr to input string
                                                 stFlags.ImmType,                       // Immediate type
                                                &lpSymEntry->stData.stLongest,          // Ptr to value to format
                                                 DEF_MAX_QUADPP_IEEE,                   // Precision to use
                                                 UTF16_DOT,                             // Char to use as decimal separator
                                                 UTF16_BAR,                             // Char to use as overbar
                                                 FLTDISPFMT_RAWFLT,                     // Float display format
                                                 TRUE);                                 // TRUE iff we're to substitute text for infinity
                                // Restore user's precision
                                lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_PP]->stData.stInteger = uQuadPP;

                                break;

                            case IMMTYPE_CHAR:
                                // Append a leading single quote
                                *lpaplChar++ = WC_SQ;

                                // Format the text as an ASCII string with non-ASCII chars
                                //   represented as either {symbol} or {\xXXXX} where XXXX is
                                //   a four-digit hex number.
                                lpaplChar +=
                                  ConvertWideToNameLength (lpaplChar,                               // Ptr to output save buffer
                                                          &(*(LPAPLHETERO) lpMemObj)->stData.stChar,// Ptr to incoming chars
                                                           1);                                      // # chars to convert
                                // Append a trailing single quote
                                *lpaplChar++ = WC_SQ;

                                // Append a trailing blank
                                *lpaplChar++ = L' ';

                                break;

                            defstop
                                break;
                        } // End SWITCH
#undef  lpSymEntry
////////////////////////// If we're over our limit, ...
////////////////////////// Note that <lpaplChar> points to the character after the trailing blank,
//////////////////////////   so we must subtract one to get the correct string length.
////////////////////////if (lpSavedWsVar_CB NE NULL
//////////////////////// && ((lpaplChar - 1 ) - lpaplCharStart) > lpSavedWsVar_CB->iMaxLength)
////////////////////////    CheckWsVarLimit (&lpaplChar, lpaplCharStart, &lpSavedWsVar_CB->iIndex, lpDict, lpSavedWsVar_CB);
                        break;

                    case PTRTYPE_HGLOBAL:
                    {
                        APLSTYPE aplTypeSub;

#define hGlbSub     (*(LPAPLNESTED *) lpMemObj)
                        // Get the global's Type
                        AttrsOfGlb (hGlbSub, &aplTypeSub, NULL, NULL, NULL);

                        // Convert the variable in global memory to saved ws form
                        lpaplChar =
                          SavedWsFormGlbVar (lpaplChar,         // Ptr to output save area
                                             hGlbSub,           // WS object global memory handle
                                             lpDict,            // Ptr to the dictionary
                                             lpuGlbCnt,         // Ptr to [Globals] count
                                             lpSymEntry,        // Ptr to this global's SYMENTRY
                                             lpSavedWsVar_CB);  // Ptr to Callback struct (may be NULL)
                        // Ensure there's a trailing blank
                        if (lpaplChar[-1] NE L' ')
                        {
                            *lpaplChar++ = L' ';
                            *lpaplChar   = WC_EOS;
                        } // End IF
#undef  hGlbSub
                        break;
                    } // End PTRTYPE_HGLOBAL

                    defstop
                        break;
                } // End FOR/SWITCH

                break;

            case ARRAY_RAT:
                // Loop through the array elements
                for (uObj = 0; uObj < aplNELMObj; uObj++)
                {
                    // Format the value
                    lpaplChar =
                      FormatAplRatFC (lpaplChar,            // Ptr to output save area
                          *(LPAPLRAT) lpMemObj,             // The value to format
                                      UTF16_BAR,            // Char to use as overbar
                                      L'/',                 // Char to use as rational separator
                                      TRUE,                 // TRUE iff we're to substitute text for infinity
                                      FALSE);               // TRUE iff this RAT is inside a larger syntax
                    // Skip over the formatted value
                    ((LPAPLRAT) lpMemObj) ++;
////////////////////// If we're over our limit, ...
////////////////////// Note that <lpaplChar> points to the character after the trailing blank,
//////////////////////   so we must subtract one to get the correct string length.
////////////////////if (lpSavedWsVar_CB NE NULL
//////////////////// && ((lpaplChar - 1) - lpaplCharStart) > lpSavedWsVar_CB->iMaxLength)
////////////////////    CheckWsVarLimit (&lpaplChar, lpaplCharStart, &lpSavedWsVar_CB->iIndex, lpDict, lpSavedWsVar_CB);
                } // End FOR

////////////////// If the tail is not empty, ...
////////////////// Note that <lpaplChar> points to the character after the trailing blank,
//////////////////   so we must subtract one to get the correct string length.
////////////////if (lpSavedWsVar_CB NE NULL
//////////////// && ((lpaplChar - 1) - lpaplCharStart) > 0)
////////////////    CheckWsVarLimit (&lpaplChar, lpaplCharStart, &lpSavedWsVar_CB->iIndex, lpDict, lpSavedWsVar_CB);
                break;

            case ARRAY_VFP:
                // Loop through the array elements
                for (uObj = 0; uObj < aplNELMObj; uObj++)
                {
                    // Format the value
                    lpaplChar =
                      FormatAplVfpFC (lpaplChar,            // Ptr to output save area
                          *(LPAPLVFP) lpMemObj,             // The value to format
                                      0,                    // # significant digits (0 = all)
                                      L'.',                 // Char to use as decimal separator
                                      UTF16_BAR,            // Char to use as overbar
                                      FALSE,                // TRUE iff nDigits is # fractional digits
                                      TRUE,                 // TRUE iff we're to substitute text for infinity
                                      uCommPrec EQ 0);      // TRUE iff we're to precede the display with (FPCnnn)
                    // Skip over the formatted value
                    ((LPAPLVFP) lpMemObj)++;
////////////////////// If we're over our limit, ...
////////////////////// Note that <lpaplChar> points to the character after the trailing blank,
//////////////////////   so we must subtract one to get the correct string length.
////////////////////if (lpSavedWsVar_CB NE NULL
//////////////////// && ((lpaplChar - 1) - lpaplCharStart) > lpSavedWsVar_CB->iMaxLength)
////////////////////    CheckWsVarLimit (&lpaplChar, lpaplCharStart, &lpSavedWsVar_CB->iIndex, lpDict, lpSavedWsVar_CB);
                } // End FOR

////////////////// If the tail is not empty, ...
////////////////// Note that <lpaplChar> points to the character after the trailing blank,
//////////////////   so we must subtract one to get the correct string length.
////////////////if (lpSavedWsVar_CB NE NULL
//////////////// && ((lpaplChar - 1) - lpaplCharStart) > 0)
////////////////    CheckWsVarLimit (&lpaplChar, lpaplCharStart, &lpSavedWsVar_CB->iIndex, lpDict, lpSavedWsVar_CB);
                break;

            defstop
                break;
        } // End SWITCH
    } __except (CheckException (GetExceptionInformation (), L"CmdSave_EM #2"))
    {
        if (hGlbObj NE NULL && lpMemObj NE NULL)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbObj); lpMemObj = NULL;
        } // End IF

        // Split cases based upon the ExceptionCode
        switch (MyGetExceptionCode ())
        {
            case EXCEPTION_LIMIT_ERROR:
                MySetExceptionCode (EXCEPTION_SUCCESS); // Reset

                RaiseException (EXCEPTION_LIMIT_ERROR, 0, 0, NULL);

            default:
                // Display message for unhandled exception
                DisplayException ();

                break;
        } // End SWITCH
    } // End __try/__except

    // Delete the last blank in case it matters,
    //   and ensure properly terminated
    if (lpaplChar[-1] EQ L' ')
        *--lpaplChar = WC_EOS;
    else
        *lpaplChar = WC_EOS;

    // Format the global count
    MySprintfW (wszGlbCnt,
                sizeof (wszGlbCnt),
                FMTSTR_GLBCNT,
               *lpuGlbCnt);
    // Count in another entry
    (*lpuGlbCnt)++;

    // Write out the entry in the [Globals] section
    ProfileSetString (SECTNAME_GLOBALS,     // Ptr to the section name
                      wszGlbCnt,            // Ptr to the key value
                      lpMemProKeyName,      // Ptr to the key name
                      lpDict);              // Ptr to the dictionary
    // Write out the saved marker
    ProfileSetString (SECTNAME_TEMPGLOBALS, // Ptr to the section name
                      wszGlbObj,            // Ptr to the key name
                      wszGlbCnt,            // Ptr to the key value
                      lpDict);              // Ptr to the dictionary
    // Move back to the start
    lpaplChar = lpaplCharStart;

    // Copy the formatted GlbCnt to the start of the buffer as the result
    strcpyW (lpaplChar, wszGlbCnt);
NORMAL_EXIT:
    if (hGlbObj NE NULL && lpMemObj NE NULL)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbObj); lpMemObj = NULL;
    } // End IF

    // Return a ptr to the profile keyname's terminating zero
    return &lpaplChar[lstrlenW (lpaplChar)];
} // End SavedWsFormGlbVar


//***************************************************************************
//  $CheckWsVarLimit
//
//  Check on saved ws var limit
//***************************************************************************

void CheckWsVarLimit
    (LPWCHAR        *lplpaplChar,
     LPWCHAR         lpaplCharStart,
     LPINT           lpiIndex,
     LPDICTIONARY    lpDict,
     LPSAVEDWSVAR_CB lpSavedWsVar_CB)

{
    Assert (lpSavedWsVar_CB NE NULL);

    LPWCHAR lpaplChar = *lplpaplChar;

    Assert (lpaplChar[-1] EQ L' ');

    // Zap the trailing blank to form a szString
    lpaplChar[-1] = WC_EOS;

    // If it's not already in the dictionary, ...
    if (*lpiIndex EQ -1)
    {
        ProfileSetString (lpSavedWsVar_CB->lpwAppName,      // Section name containing the key name
                          lpSavedWsVar_CB->lpwKeyName,      // Key name whose associated string is to be retrieved
                          lpaplCharStart,                   // Ptr to new value to associate with the section:keyname
                          lpSavedWsVar_CB->lpDict);         // Ptr to workspace dictionary
        // Get the string index
        // Ignore the return value as we're interested in the index only
        ProfileGetStringEx (lpSavedWsVar_CB->lpwAppName,    // Section name containing the key name
                            lpSavedWsVar_CB->lpwKeyName,    // Key name whose associated string is to be retrieved
                            NULL,                           // Ptr to default result if lpwkeyname not found
                            lpiIndex,                       // Ptr to index on output (may be null)
                            lpSavedWsVar_CB->lpDict);       // Ptr to workspace dictionary
    } else
    {
        size_t  iLenOld, iLenNew;
        LPWCHAR lpwNew;

        // Calculate the new and old string lengths
        // Note that <lpaplChar> points to the character after the trailing blank,
        //   so we must subtract one to get the correct string length.
        iLenOld = lstrlenW (lpDict->key[*lpiIndex]);
        iLenNew = (lpaplChar - 1) - lpaplCharStart;

        // Allocate room for the combined string
        //   including a trailing zero
        lpwNew = calloc (iLenOld + iLenNew + 1, 1);
        if (lpwNew EQ NULL)
            RaiseException (EXCEPTION_LIMIT_ERROR, 0, 0, NULL);

        // Copy the old string to the beginning of the new one
        strcpyW (lpwNew, lpDict->key[*lpiIndex]);

        // Append the new string to the old
        strcpyW (&lpwNew[iLenOld], lpaplCharStart);

        // Release the old one
        free (lpDict->key[*lpiIndex]); lpDict->key[*lpiIndex] = NULL;

        // Save back in the dictionary
        lpDict->key[*lpiIndex] = lpwNew;
    } // End IF/ELSE

    // Restore the zapped trailing blank
    lpaplChar[-1] = L' ';

    // Start the string buffer over again
    *lplpaplChar = lpaplCharStart;
} // End CheckWsVarLimit


//***************************************************************************
//  $AppendArrayHeader
//
//  Append the array header
//***************************************************************************

LPAPLCHAR AppendArrayHeader
    (LPAPLCHAR         lpaplChar,               // Ptr to output save area
     APLCHAR           aplCharObj,              // Object storage type as WCHAR
     APLNELM           aplNELMObj,              // Object NELM
     APLRANK           aplRankObj,              // Object rank
     LPAPLDIM          lpaplDimObj,             // Ptr to object dimensions
     LPAPLUINT         lpuCommPrec,             // Ptr to common VFP array precision (0 if none) (may be NULL)
     LPVARARRAY_HEADER lpHeader)                // Ptr to array header

{
    APLRANK uObj;                   // Loop counter

    // If common VFP array precision requested, ...
    if (lpuCommPrec NE NULL)
        // Initialize it as none
        *lpuCommPrec = 0;

    // Append the storage type as a WCHAR
    *lpaplChar++ = aplCharObj;

    // Append a separator
    *lpaplChar++ = L' ';

    // Append the NELM (with a trailing blank)
    lpaplChar =
      FormatAplIntFC (lpaplChar,                // Ptr to output save area
                      aplNELMObj,               // The value to format
                      UTF16_BAR);               // Char to use as overbar
    // Append the rank (with a trailing blank)
    lpaplChar =
      FormatAplIntFC (lpaplChar,                // Ptr to output save area
                      aplRankObj,               // The value to format
                      UTF16_BAR);               // Char to use as overbar

    // Append the shape (each with a trailing blank)
    for (uObj = 0; uObj < aplRankObj; uObj++)
        // Format the dimension
        lpaplChar =
          FormatAplIntFC (lpaplChar,            // Ptr to output save area
                          lpaplDimObj[uObj],    // The value to format
                          UTF16_BAR);           // Char to use as overbar
    // Append array properties
    if (lpHeader NE NULL)
    {
        // Append leading separator
        *lpaplChar++ = L'(';

        // Check for array property:  PV0
        if (lpHeader->PV0)
        {
            strcpyW (lpaplChar, AP_PV0);           // Copy the prefix
            lpaplChar += strcountof (AP_PV0);       // Skip over it
            *lpaplChar++ = L' ';                    // Append a trailing blank
        } // End IF

        // Check for array property:  PV1
        if (lpHeader->PV1)
        {
            strcpyW (lpaplChar, AP_PV1);           // Copy the prefix
            lpaplChar += strcountof (AP_PV1);       // Skip over it
            *lpaplChar++ = L' ';                    // Append a trailing blank
        } // End IF

        // Check for array property:  All2s
        if (lpHeader->All2s)
        {
            strcpyW (lpaplChar, AP_ALL2S);         // Copy the prefix
            lpaplChar += strcountof (AP_ALL2S);     // Skip over it
            *lpaplChar++ = L' ';                    // Append a trailing blank
        } // End IF

        // If common VFP array precision requested,
        //   and the array is non-empty, ...
        if (lpuCommPrec NE NULL
         && IsVfp (lpHeader->ArrType)
         && !IsEmpty (aplNELMObj))
        {
            LPAPLVFP lpaplVfp;

            // Skip over the header and dimensions to the data
            lpaplVfp = VarArrayDataFmBase (lpHeader);

            // Get the initial precision
            *lpuCommPrec = mpfr_get_prec (lpaplVfp++);

            for (uObj = 1; uObj < aplNELMObj; uObj++)
            if (*lpuCommPrec NE mpfr_get_prec (lpaplVfp++))
            {
                // Mark as none
                *lpuCommPrec = 0;

                break;
            } // End FOR

            strcpyW (lpaplChar, AP_FPC);       // Copy the prefix
            lpaplChar += strcountof (AP_FPC);   // Skip over it

            // If there's a common precision, ...
            if (*lpuCommPrec NE 0)
            {
                lpaplChar +=
                  wsprintfW (lpaplChar,             // Format the common precision
                             L"%I64u ",             // Note trailing blank
                            *lpuCommPrec);
            } else
                *lpaplChar++ = L' ';
        } // End IF

        if (lpaplChar[-1] NE L'(')
        {
            // Append trailing separator
            lpaplChar[-1] = L')';
            *lpaplChar++ = L' ';
        } else
            // Zap leading separator
            *--lpaplChar = WC_EOS;
    } // End IF

    return lpaplChar;
} // End AppendArrayHeader


//***************************************************************************
//  $SavedWsGlbVarConv
//
//  Callback function for DisplayFcnGlb to format a global variable
//***************************************************************************

LPAPLCHAR SavedWsGlbVarConv
    (LPAPLCHAR           lpaplChar,                 // Ptr to output save area
     HGLOBAL             hGlbObj,                   // Object global memory handle
     LPSAVEDWSGLBVARPARM lpSavedWsGlbVarParm)       // Ptr to extra parameters for lpSavedWsGlbVarConv

{
    // Convert the variable in global memory to saved ws form
    lpaplChar =
      SavedWsFormGlbVar (lpaplChar,                             // Ptr to output save area
                         hGlbObj,                               // WS object global memory handle
                         lpSavedWsGlbVarParm->lpDict,           // Ptr to the dictionary
                         lpSavedWsGlbVarParm->lpuGlbCnt,        // Ptr to [Globals] count
                         lpSavedWsGlbVarParm->lpSymEntry,       // Ptr to this global's SYMENTRY
                         NULL);                                 // Ptr to Callback struct (may be NULL)
    // Include a trailing blank
    *lpaplChar++ = L' ';

    return lpaplChar;
} // End SavedWsGlbVarConv


//***************************************************************************
//  $SavedWsGlbFcnConv
//
//  Callback function for DisplayFcnGlb to format a global function
//***************************************************************************

LPAPLCHAR SavedWsGlbFcnConv
    (LPAPLCHAR           lpaplChar,                 // Ptr to output save area
     HGLOBAL             hGlbObj,                   // Object global memory handle
     LPSAVEDWSGLBFCNPARM lpSavedWsGlbFcnParm)       // Ptr to extra parameters for lpSavedWsGlbVarConv

{
    // Convert the function in global memory to saved ws form
    lpaplChar =
      SavedWsFormGlbFcn (lpaplChar,                             // Ptr to output save area
                         lpSavedWsGlbFcnParm->lpwszFcnTypeName, // Ptr to the function section name as F nnn.Name where nnn is the count
                         NULL,                                  // Ptr to the function name (for FCNARRAY_HEADER only)
                         hGlbObj,                               // WS object global memory handle
                         lpSavedWsGlbFcnParm->lpDict,           // Ptr to saved WS file DPFE
                         lpSavedWsGlbFcnParm->lpuGlbCnt,        // Ptr to [Globals] count
                         lpSavedWsGlbFcnParm->lpSymEntry);      // Ptr to this global's SYMENTRY
    // Include a trailing blank
    *lpaplChar++ = L' ';

    return lpaplChar;
} // End SavedWsGlbFcnConv


//***************************************************************************
//  End of File: sc_save.c
//***************************************************************************
