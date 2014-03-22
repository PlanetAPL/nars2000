//***************************************************************************
//  NARS2000 -- System Command:  )SAVE
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
    (LPWCHAR lpwszTail)                 // Ptr to command line tail

{
    HGLOBAL      hGlbName,              // STE name global memory handle
                 hGlbCnt = NULL,        // Vars/Fcns/Oprs counters global memory handle
                 stGlbData;             // SYMENTRY's stGlbData
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    LPVOID       lpMemOldWSID = NULL;   // Ptr to old []WSID global memory
    LPUINT       lpMemCnt = NULL;       // Ptr to Vars/Fcns counters
    WCHAR        wszTailDPFE[_MAX_PATH],// Save area for canonical form of given ws name
                 wszWsidDPFE[_MAX_PATH],// ...           ...               []WSID
                 wszTempDPFE[_MAX_PATH],// ...           temporary
                 wszSectName[15],       // ...           section name (e.g., [Vars.sss])
                 wszCount[8];           // ...           formatted uSymxxx counter
    LPWCHAR      lpMemSaveWSID,         // Ptr to WSID to save to
                 lpMemName,             // Ptr to STE name
                 lpaplChar,             // Ptr to output save area
                 lpw;                   // Temporary ptr
    APLNELM      aplNELMWSID;           // []WSID NELM
    APLRANK      aplRankWSID;           // ...    rank
    APLUINT      ByteRes,               // # bytes in the result
                 uQuadPP;               // []PP save area
    UBOOL        bRet = FALSE;          // TRUE iff result is valid
    APLU3264     uNameLen;              // Length of the object name in WCHARs
    int          iCmp;                  // Comparison result
    UINT         uGlbCnt=0;             // # entries in [Globals] section
    FILE        *fStream;               // Ptr to file stream
    LPSYMENTRY   lpSymEntry,            // Ptr to STE
                 lpSymTabNext;          // ...
    STFLAGS      stFlags;               // STE flags
    LPWCHAR      lpwszFormat,           // Ptr to formatting save area
                 lpwszOrigTemp,         // Original lpMemPTD->lpwszTemp
                 lpwszTemp;             // Ptr to temporary storage
    VARS_TEMP_OPEN

    // Skip to the next blank
    lpw = SkipToCharDQW (lpwszTail, L' ');

    // Zap it in case there are trailing blanks
    if (*lpw)
        *lpw = WC_EOS;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Lock the memory to get a ptr to it
    lpMemOldWSID = MyGlobalLock (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_WSID]->stData.stGlbData);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemOldWSID)
    // Get the NELM and Rank
    aplNELMWSID = lpHeader->NELM;
    aplRankWSID = lpHeader->Rank;
#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMemSaveWSID = VarArrayBaseToData (lpMemOldWSID, aplRankWSID);

    // Because the global memory doesn't have a zero terminator,
    //   we must copy the data to a temporary location and then
    //   append a zero terminator
    lstrcpynW (wszTempDPFE, lpMemSaveWSID, (UINT) aplNELMWSID + 1);
////wszTempDPFE[aplNELMWSID] = WC_EOS;  // Already done via "+ 1" in lstrcpynW

    // Convert the []WSID workspace name into a canonical form (without WS_WKSEXT)
    MakeWorkspaceNameCanonical (wszWsidDPFE, wszTempDPFE, lpwszWorkDir);

    // If there is a specified WS Name,
    //   and the current []WSID is different from the specified WS Name,
    //   and a WS with the specified WS Name already exists,
    //   display an error
    //     "NOT SAVED, THIS WS IS ",[]WSID
    if (*lpwszTail)
    {
        // Convert the given workspace name into a canonical form (without WS_WKSEXT)
        MakeWorkspaceNameCanonical (wszTailDPFE, lpwszTail, lpwszWorkDir);

        // Compare the workspace names (without their extensions)
        iCmp = lstrcmpiW (wszWsidDPFE, wszTailDPFE);

        // Append the common workspace extension
        lstrcatW (wszTailDPFE, WS_WKSEXT);

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
                    lstrcpyW (wszTemp, ERRMSG_NOT_SAVED_CLEAR_WS);
                else
                {
                    // Copy the leading part of the message
                    lstrcpyW (wszTemp, ERRMSG_NOT_SAVED);

                    // Followed by the old []WSID
                    lstrcpynW (&wszTemp[lstrlenW (wszTemp)], lpMemSaveWSID, (UINT) aplNELMWSID + 1);
                } // End IF/ELSE

                // Display the error message
                ReplaceLastLineCRPmt (wszTemp);

                goto ERROR_EXIT;
            } // End IF

            // We no longer need this ptr
            MyGlobalUnlock (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_WSID]->stData.stGlbData); lpMemOldWSID = NULL;

            // Set the value of the new []WSID as wszTailDPFE
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
        lstrcatW (wszWsidDPFE, WS_WKSEXT);

        // Save as the target WSID
        lpMemSaveWSID = wszWsidDPFE;
    } // End IF/ELSE

    if (lpMemOldWSID)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_WSID]->stData.stGlbData); lpMemOldWSID = NULL;
    } // End IF

    // The full workspace name to save to is in lpMemSaveWSID

    // Make a backup copy of the workspace
    if (OptionFlags.bBackupOnSave)
        MakeWorkspaceBackup (lpMemSaveWSID, SAVEBAK_EXT);

    // Calculate space needed for the two counters
    ByteRes = 2 * sizeof (UINT) * (lpMemPTD->SILevel + 1);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for two counters (Vars & Fcns) per SI level
    hGlbCnt = MyGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (!hGlbCnt)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemCnt = MyGlobalLock (hGlbCnt);

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

    // Create (or truncate the file)
    fStream = fopenW (lpMemSaveWSID, L"wb");
    if (!fStream)
        goto NOTSAVED_FILE_EXIT;

    // Close it after creating the file
    fclose (fStream); fStream = NULL;

    // Write out to the [General] section
    WritePrivateProfileStringW (SECTNAME_GENERAL,   // Ptr to the section name
                                KEYNAME_VERSION,    // Ptr to the key name
                                WS_VERSTR,          // Ptr to the key value
                                lpMemSaveWSID);     // Ptr to the file name
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
        if (lpSymEntry->stHshEntry->htGlbName           // Must have a name (not steZero, etc.),
         && lpSymEntry->stFlags.ObjName NE OBJNAME_MFO  // and not be a Magic Function/Operator,
         && lpSymEntry->stFlags.ObjName NE OBJNAME_LOD) // and not be a )LOAD HGLOBAL
        {
            // Get the symbol name's global memory handle
            hGlbName = lpSymEntry->stHshEntry->htGlbName;

            // Lock the memory to get a ptr to it
            lpMemName = MyGlobalLock (hGlbName);

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

            // Check for []WSID (if so, write out to [General] section and skip over it)
            if (lstrcmpiW (lpwszTemp, L"{quad}wsid") EQ 0)
            {
                HGLOBAL hGlbWSID;
                LPWCHAR lpMemWSID;

                // Get the global memory handle
                hGlbWSID = lpSymEntry->stData.stGlbData;

                // Lock the memory to get a ptr to it
                lpMemWSID = MyGlobalLock (hGlbWSID);

                // Skip oer the header and dimensions
                lpMemWSID = VarArrayBaseToData (lpMemWSID, 1);

                // Write out to the [General] section
                WritePrivateProfileStringW (SECTNAME_GENERAL,   // Ptr to the section name
                                            KEYNAME_WSID,       // Ptr to the key name
                                            lpMemWSID,          // Ptr to the key value
                                            lpMemSaveWSID);     // Ptr to the file name
                // We no longer need this ptr
                MyGlobalUnlock (hGlbWSID); lpMemWSID = NULL;

                continue;
            } else
                // Save the GlbData handle
                stGlbData = lpSymEntry->stData.stGlbData;

            if (stFlags.Value)                      // Must have a value
            {
                // Split cases based upon the object type
                switch (stFlags.stNameType)
                {
                    case NAMETYPE_VAR:
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
                                        lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_PP]->stData.stInteger = DEF_MAX_QUADPP64;

                                    // Format the value
                                    lpaplChar =
                                      FormatImmedFC (lpaplChar,                         // Ptr to input string
                                                     stFlags.ImmType,                   // Immediate type
                                                    &lpSymEntry->stData.stLongest,      // Ptr to value to format
                                                     DEF_MAX_QUADPP64,                  // Precision to use
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
                              SavedWsFormGlbVar (lpaplChar,
                                                 stGlbData,
                                                 lpMemSaveWSID,
                                                &uGlbCnt,
                                                 lpSymEntry);
                        // Format the counter
                        wsprintfW (wszCount, L"%d", lpMemCnt[0 + 2 * lpSymEntry->stSILevel]++);

                        // Format the section name as [Vars.sss] where sss is the SI level
                        wsprintfW (wszSectName, SECTNAME_VARS L".%d", lpSymEntry->stSILevel);

                        // Write out the entry (nnn=Name=value)
                        //   in the [Vars.sss] section
                        WritePrivateProfileStringW (wszSectName,            // Ptr to the section name
                                                    wszCount,               // Ptr to the key name
                                                    lpwszTemp,              // Ptr to the key value
                                                    lpMemSaveWSID);         // Ptr to the file name
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
                                                 lpMemSaveWSID,                 // Ptr to saved WS file DPFE
                                                &uGlbCnt,                       // Ptr to [Globals] count
                                                 lpSymEntry);                   // Ptr to this global's SYMENTRY
                        // Format the counter
                        wsprintfW (wszCount, L"%d", lpMemCnt[1 + 2 * lpSymEntry->stSILevel]++);

                        // Format the section name as [Fcns.sss] where sss is the SI level
                        wsprintfW (wszSectName, SECTNAME_FCNS L".%d", lpSymEntry->stSILevel);

                        // Write out the entry (nnn=Name=Type=Value)
                        //   in the [Fcns.sss] section
                        WritePrivateProfileStringW (wszSectName,            // Ptr to the section name
                                                    wszCount,               // Ptr to the key name
                                                    lpwszTemp,              // Ptr to the key value
                                                    lpMemSaveWSID);         // Ptr to the file name
                        break;

                    defstop
                        break;
                } // End SWITCH
            } else
            if (lpSymEntry->stSILevel)      // Must be suspended
            {
                // Format the section name
                wsprintfW (wszSectName, SECTNAME_VARS L".%d", lpSymEntry->stSILevel);

                // Write out the entry (nnn = Name = value)
                //   in the [Vars.sss] section
                WritePrivateProfileStringW (wszSectName,            // Ptr to the section name
                                            lpwszTemp,              // Ptr to the key name
                                            L"",                    // Ptr to the key value
                                            lpMemSaveWSID);         // Ptr to the file name
            } // End IF/ELSE/...
        } // End FOR/IF/...
    } __except (CheckException (GetExceptionInformation (), L"CmdSave_EM"))
    {
        EXIT_TEMP_OPEN          // lpwszTemp is used in CleanUpAfterSav

        // Clean up after )SAVE
        CleanupAfterSave (lpMemPTD, lpMemCnt, lpMemSaveWSID, uGlbCnt);

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
    WritePrivateProfileSectionW (SECTNAME_TEMPGLOBALS,  // Ptr to the section name
                                 NULL,                  // Ptr to data
                                 lpMemSaveWSID);        // Ptr to the file name
    EXIT_TEMP_OPEN          // lpwszTemp is used in CleanUpAfterSav

    // Clean up after )SAVE
    CleanupAfterSave (lpMemPTD, lpMemCnt, lpMemSaveWSID, uGlbCnt);

    // Note if the SI is non-empty
    if (lpMemPTD->SILevel)
        ReplaceLastLineCRPmt (L"WARNING:  SI non-empty -- not restartable after )LOAD");

    // Get the length of the []WSID excluding WS_WKSEXT
    iCmp = lstrlenW (lpMemSaveWSID) - WS_WKSEXT_LEN;

    // Omit the trailing WS_WKSEXT
    Assert (lpMemSaveWSID[iCmp] EQ L'.');
    lpMemSaveWSID[iCmp] = WC_EOS;

    CHECK_TEMP_OPEN         // Restore flag

    // Copy the (possibly shortened WSID)
    lstrcpyW (lpwszTemp, ShortenWSID (lpMemSaveWSID));

    // Copy the " SAVED " text
    lstrcatW (lpwszTemp, L" SAVED ");

    // Format the current date & time
    FormatCurDateTime (&lpwszTemp[lstrlenW (lpwszTemp)]);

    // Protect the text
    lpwszOrigTemp = lpMemPTD->lpwszTemp;
    lpMemPTD->lpwszTemp += lstrlenW (lpMemPTD->lpwszTemp);

    EXIT_TEMP_OPEN

    // Write out the WSID and SAVED date & time
    ReplaceLastLineCRPmt (lpwszTemp);

    // Restore the original ptr
    lpMemPTD->lpwszTemp = lpwszOrigTemp;

    CHECK_TEMP_OPEN

    // Mark as successful
    bRet = TRUE;

    goto NORMAL_EXIT;

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
    ReplaceLastLineCRPmt (ERRMSG_NOT_SAVED_CLEAR_WS);

    goto ERROR_EXIT;

NOTSAVED_FILE_EXIT:
    // Display the error message
    ReplaceLastLineCRPmt (ERRMSG_NOT_SAVED_FILE_ERROR APPEND_NAME);

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (lpMemOldWSID)
    {
        // We no longer need this ptr
        MyGlobalUnlock (lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_WSID]->stData.stGlbData); lpMemOldWSID = NULL;
    } // End IF

    if (hGlbCnt)
    {
        if (lpMemCnt)
        {
            // We no longer need this ptr
            MyGlobalUnlock (hGlbCnt); lpMemCnt = NULL;
        } // End IF

        // We no longer need this storage
        MyGlobalFree (hGlbCnt); hGlbCnt = NULL;
    } // End IF

    EXIT_TEMP_OPEN

    return bRet;
} // End CmdSave_EM
#undef  APPEND_NAME


//***************************************************************************
//  $CleanupAfterSave
//
//  Execute cleanup code after a )SAVE (successfull or not)
//***************************************************************************

void CleanupAfterSave
    (LPPERTABDATA lpMemPTD,             // Ptr to PerTabData global memory
     LPUINT       lpMemCnt,             // Ptr to Vars/Fcns counters
     LPWCHAR      lpMemSaveWSID,        // Ptr to WSID to save to
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
        wsprintfW (wszSectName, SECTNAME_VARS L".%d", uCnt);

        // Format the Var count
        wsprintfW (wszCount,
                   L"%d",
                   lpMemCnt[0 + 2 * uCnt]);
        // Write out the Var count to the [Vars.sss] section where sss is the SI level
        WritePrivateProfileStringW (wszSectName,                    // Ptr to the section name
                                    KEYNAME_COUNT,                  // Ptr to the key name
                                    wszCount,                       // Ptr to the key value
                                    lpMemSaveWSID);                 // Ptr to the file name
        // Format the section name as [Fcns.sss] where sss is the SI level
        wsprintfW (wszSectName, SECTNAME_FCNS L".%d", uCnt);

        // Format the Fcn/Opr count
        wsprintfW (wszCount,
                   L"%d",
                   lpMemCnt[1 + 2 * uCnt]);
        // Write out the Fcn count to the [Fcns.sss] section where sss is the SI level
        WritePrivateProfileStringW (wszSectName,                    // Ptr to the section name
                                    KEYNAME_COUNT,                  // Ptr to the key name
                                    wszCount,                       // Ptr to the key value
                                    lpMemSaveWSID);                 // Ptr to the file name
    } // End FOR

    // Format the SI level
    wsprintfW (wszCount,
               L"%d",
               lpMemPTD->SILevel);
    // Write out the SI level to the [General] section
    WritePrivateProfileStringW (SECTNAME_GENERAL,               // Ptr to the section name
                                KEYNAME_SILEVEL,                // Ptr to the key name
                                wszCount,                       // Ptr to the key value
                                lpMemSaveWSID);                 // Ptr to the file name
    // Format to []MF timer #
    wsprintfW (wszCount,
               L"%d",
               lpMemPTD->uQuadMF);
    // Write out the []MF timer # to the [General] section
    WritePrivateProfileStringW (SECTNAME_GENERAL,               // Ptr to the section name
                                KEYNAME_MFTIMER,                // Ptr to the key name
                                wszCount,                       // Ptr to the key value
                                lpMemSaveWSID);                 // Ptr to the file name
    // Format the [Globals] count
    wsprintfW (wszCount,
               L"%d",
               uGlbCnt);
    // Write out the count to the [Globals] section
    WritePrivateProfileStringW (SECTNAME_GLOBALS,               // Ptr to the section name
                                KEYNAME_COUNT,                  // Ptr to the key name
                                wszCount,                       // Ptr to the key value
                                lpMemSaveWSID);                 // Ptr to the file name
    // Write out the SI stack to the [SI] section
    CmdSiSinlCom_EM (L"",               // Ptr to command tail
                     FALSE,             // TRUE iff )SINL
                     lpMemSaveWSID);    // Ptr to the file name
    // Get the current system (UTC) time
    GetSystemTime (&systemTime);

    // Convert system time to file time and save as creation time
    SystemTimeToFileTime (&systemTime, &ftCreation);

    // Format the creation time
    wsprintfW (wszTimeStamp,
               FMTSTR_DATETIME,
               ftCreation.dwHighDateTime,
               ftCreation.dwLowDateTime);
    // Write out the creation time to the [General] section
    WritePrivateProfileStringW (SECTNAME_GENERAL,               // Ptr to the section name
                                KEYNAME_CREATIONTIME,           // Ptr to the key name
                                wszTimeStamp,                   // Ptr to the key value
                                lpMemSaveWSID);                 // Ptr to the file name
} // End CleanupAfterSave


//***************************************************************************
//  $SavedWsFormGlbFcn
//
//  Convert a function/operator in global memory to saved ws form
//***************************************************************************

LPAPLCHAR SavedWsFormGlbFcn
    (LPAPLCHAR   lpaplChar,             // Ptr to output save area
     LPAPLCHAR   lpwszFcnTypeName,      // Ptr to the function section name as F nnn.Name where nnn is the count
     LPAPLCHAR   lpwszFcnName,          // Ptr to the function name (for FCNARRAY_HEADER only)
     HGLOBAL     hGlbObj,               // WS object global memory handle
     LPAPLCHAR   lpMemSaveWSID,         // Ptr to saved WS file DPFE
     LPUINT      lpuGlbCnt,             // Ptr to [Globals] count
     LPSYMENTRY  lpSymEntry)            // Ptr to this global's SYMENTRY

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
    wsprintfW (wszGlbObj,
               FMTSTR_GLBOBJ,
               hGlbObj);
    // Save as ptr to the profile keyname
    lpMemProKeyName = lpaplChar;

    // Check to see if this global has already been saved
    //   in the [TempGlobals] section
    if (GetPrivateProfileStringW (SECTNAME_TEMPGLOBALS,         // Ptr to the section name
                                  wszGlbObj,                    // Ptr to the key name
                                  L"",                          // Default value if not found
                                  lpaplChar,                    // Ptr to the output buffer
                                  8 * sizeof (lpaplChar[0]),    // Byte size of the output buffer
                                  lpMemSaveWSID))               // Ptr to the file name
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
            SavedWsGlbVarParm.lpMemSaveWSID    = lpMemSaveWSID;
            SavedWsGlbVarParm.lpuGlbCnt        = lpuGlbCnt;
            SavedWsGlbVarParm.lpSymEntry       = lpSymEntry;
            SavedWsGlbVarParm.lplpSymLink      = NULL;

            SavedWsGlbFcnParm.lpMemSaveWSID    = lpMemSaveWSID;
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

            // Write out the single line
            WritePrivateProfileStringW (lpwszSectName,      // Ptr to the section name
                                        L"0",               // Ptr to the key name
                                        lpaplCharStart2,    // Ptr to the key value
                                        lpMemSaveWSID);     // Ptr to the file name
            // Write out the Count key value
            WritePrivateProfileStringW (lpwszSectName,      // Ptr to the section name
                                        KEYNAME_COUNT,      // Ptr to the key name
                                        L"1",               // Ptr to the key value
                                        lpMemSaveWSID);     // Ptr to the file name
            // Write out the UserDefined key value
            WritePrivateProfileStringW (lpwszSectName,      // Ptr to the section name
                                        KEYNAME_USERDEFINED,// Ptr to the key name
                                        L"0",               // Ptr to the key value
                                        lpMemSaveWSID);     // Ptr to the file name
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
                lstrcpyW (&lpwszFcnTypeName[lstrlenW (lpwszFcnTypeName)], lpwszFcnName);

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

            // Write out the function header
            WriteFunctionLine (lpwszSectName,   // Ptr to the section name
                               lpaplChar,       // Ptr to save area for the formatted line
                               0,               // Line #
                               lpMemDfnHdr->hGlbTxtHdr,
                               lpMemSaveWSID);  // Ptr to the file name
            // Loop through the function lines
            for (uLine = 0; uLine < numFcnLines; uLine++, lpFcnLines++)
                WriteFunctionLine (lpwszSectName,   // Ptr to the section name
                                   lpaplChar,       // Ptr to save area for the formatted line
                                   uLine + 1,       // Line #
                                   lpFcnLines->hGlbTxtLine,
                                   lpMemSaveWSID);  // Ptr to the file name
            // Format the function line count
            wsprintfW (lpaplChar,
                       L"%d",
                       numFcnLines + 1);
            // Write out the Count key value
            WritePrivateProfileStringW (lpwszSectName,      // Ptr to the section name
                                        KEYNAME_COUNT,      // Ptr to the key name
                                        lpaplChar,          // Ptr to the key value
                                        lpMemSaveWSID);     // Ptr to the file name
            // Write out the UserDefined key value
            WritePrivateProfileStringW (lpwszSectName,      // Ptr to the section name
                                        KEYNAME_USERDEFINED,// Ptr to the key name
                                        L"1",               // Ptr to the key value
                                        lpMemSaveWSID);     // Ptr to the file name
            // If it's an AFO, ...
            if (lpMemDfnHdr->bAFO)
                // Write out the bAFO key value
                WritePrivateProfileStringW (lpwszSectName,      // Ptr to the section name
                                            KEYNAME_AFO,        // Ptr to the key name
                                            L"1",               // Ptr to the key value
                                            lpMemSaveWSID);     // Ptr to the file name
#define lpUndoIni       lpaplChar       // Start of output save area

            // Write out the Undo buffer
            if (lpMemDfnHdr->hGlbUndoBuff)
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
                            if (!(lpUndoChar = CharToSymbolName (lpMemUndo->Char)))
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
                lstrcpyW (lpUndoIni, L"0");

            // Write out the Undo Buffer contents
            WritePrivateProfileStringW (lpwszSectName,          // Ptr to the section name
                                        KEYNAME_UNDO,           // Ptr to the key name
                                        lpUndoIni,              // Ptr to the key value
                                        lpMemSaveWSID);         // Ptr to the file name
            // ***FINISHME*** -- Write out the stop/trace bits










            if (lpMemDfnHdr->hGlbMonInfo)
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
                      FormatAplint (lpaplCharMon,               // Ptr to output save area
                                    lpMemMonInfo->IncSubFns);   // The value to format
                    // Format the ExcSubFns value
                    lpaplCharMon =
                      FormatAplint (lpaplCharMon,               // Ptr to output save area
                                    lpMemMonInfo->ExcSubFns);   // The value to format
                    // Format the Count value
                    lpaplCharMon =
                      FormatAplint (lpaplCharMon,               // Ptr to output save area
                                    lpMemMonInfo->Count);       // The value to format
                    // Append a separator
                    lpaplCharMon[-1] = L',';
                } // End FOR

                // We no longer need this ptr
                MyGlobalUnlock (lpMemDfnHdr->hGlbMonInfo); lpMemMonInfo = NULL;

                // Ensure properly terminated and zap the trailing blank
                lpaplCharMon[-1] = WC_EOS;

                // Write out the Monitor Info
                WritePrivateProfileStringW (lpwszSectName,          // Ptr to the section name
                                            KEYNAME_MONINFO,        // Ptr to the key name
                                            lpaplChar,              // Ptr to the key value
                                            lpMemSaveWSID);         // Ptr to the file name
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
    WritePrivateProfileStringW (lpwszSectName,          // Ptr to the section name
                                KEYNAME_CREATIONTIME,   // Ptr to the key name
                                lpaplChar,              // Ptr to the key value
                                lpMemSaveWSID);         // Ptr to the file name
    // Save the ftLastMod time
    wsprintfW (lpaplChar,
               FMTSTR_DATETIME,
               ftLastMod.dwHighDateTime,
               ftLastMod.dwLowDateTime);
    WritePrivateProfileStringW (lpwszSectName,          // Ptr to the section name
                                KEYNAME_LASTMODTIME,    // Ptr to the key name
                                lpaplChar,              // Ptr to the key value
                                lpMemSaveWSID);         // Ptr to the file name
    // Format the global count
    wsprintfW (wszGlbCnt,
               FMTSTR_GLBCNT,
               uGlbCnt);
    // Write out the entry in the [Globals] section
    WritePrivateProfileStringW (SECTNAME_GLOBALS,               // Ptr to the section name
                                wszGlbCnt,                      // Ptr to the key value
                                lpwszFcnTypeName,               // Ptr to the key name
                                lpMemSaveWSID);                 // Ptr to the file name
    // Write out the saved marker
    WritePrivateProfileStringW (SECTNAME_TEMPGLOBALS,           // Ptr to the section name
                                wszGlbObj,                      // Ptr to the key name
                                wszGlbCnt,                      // Ptr to the key value
                                lpMemSaveWSID);                 // Ptr to the file name
    // Move back to the start
    lpaplChar = lpaplCharStart;

    // Copy the formatted GlbCnt to the start of the buffer as the result
    lstrcpyW (lpaplChar, wszGlbCnt);
NORMAL_EXIT:
    if (hGlbObj && lpMemObj)
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
//  Write out a function line to a the saved WS file
//***************************************************************************

void WriteFunctionLine
    (LPWCHAR lpwszSectName,     // Ptr to section name
     LPWCHAR lpwFormat,         // Ptr to output save area for the formatted function line
     UINT    uLineNum,          // The line #
     HGLOBAL hGlbTxtLine,       // Line text global memory handle
     LPWCHAR lpMemSaveWSID)     // Ptr to saved WS file DPFE

{
    LPMEMTXT_UNION lpMemTxtLine;
    WCHAR          wszCount[10];

    // Format the line number
    wsprintfW (wszCount,
               L"%d",
               uLineNum);
    // Lock the memory to get a ptr to it
    lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

    // Format the text as an ASCII string with non-ASCII chars
    //   represented as either {symbol} or {\xXXXX} where XXXX is
    //   a four-digit hex number.
    ConvertWideToNameLength (lpwFormat,                     // Ptr to output save buffer
                            &lpMemTxtLine->C,               // Ptr to incoming chars
                             lpMemTxtLine->U);              // # chars to convert
    // We no longer need this ptr
    MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;

    // Write out the entry (nnn = FunctionLine)
    WritePrivateProfileStringW (lpwszSectName,              // Ptr to the section name
                                wszCount,                   // Ptr to the key name
                                lpwFormat,                  // Ptr to the key value
                                lpMemSaveWSID);             // Ptr to the file name
} // End WriteFunctionLine


//***************************************************************************
//  $SavedWsFormGlbVar
//
//  Convert a variable in global memory to saved ws form
//***************************************************************************

LPAPLCHAR SavedWsFormGlbVar
    (LPAPLCHAR   lpaplChar,             // Ptr to output save area
     HGLOBAL     hGlbObj,               // WS object global memory handle
     LPAPLCHAR   lpMemSaveWSID,         // Ptr to saved WS file DPFE
     LPUINT      lpuGlbCnt,             // Ptr to [Globals] count
     LPSYMENTRY  lpSymEntry)            // Ptr to this global's SYMENTRY

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

    // Check on user-defined functions/operators
    bUsrDfn = IsGlbTypeDfnDir_PTB (hGlbObj);

    // stData is a valid HGLOBAL variable array
    //   or user-defined function/operator
    Assert (IsGlbTypeVarDir_PTB (hGlbObj)
         || bUsrDfn);

    // Lock the memory to get a ptr to it
    lpMemObj = MyGlobalLock (hGlbObj);

    // Format the hGlbObj
    wsprintfW (wszGlbObj,
               FMTSTR_GLBOBJ,
               hGlbObj);
    // Check to see if this global has already been saved
    //   in the [Globals] section
    if (GetPrivateProfileStringW (SECTNAME_TEMPGLOBALS,         // Ptr to the section name
                                  wszGlbObj,                    // Ptr to the key name
                                  L"",                          // Default value if not found
                                  lpaplChar,                    // Ptr to the output buffer
                                  8 * sizeof (lpaplChar[0]),    // Byte size of the output buffer
                                  lpMemSaveWSID))               // Ptr to the file name
    {
        if (bUsrDfn)
        {
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
                         VarArrayBaseToDim (lpMemObj),          // Ptr to object dimensions
                        &uCommPrec,                             // Ptr to common VFP array precision (0 if none) (may be NULL)
                         lpHeader);                             // Ptr to array header
#undef  lpHeader
    // Skip over the header and dimensions to the data
    lpMemObj = VarArrayBaseToData (lpMemObj, aplRankObj);

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
                      FormatAplintFC (lpaplChar,                                        // Ptr to output save area
                                      BIT0 & ((*(LPAPLBOOL) lpMemObj) >> uBitIndex),    // The value to format
                                      UTF16_BAR);                                       // Char to use as overbar
                    // Check for end-of-byte
                    if (++uBitIndex EQ NBIB)
                    {
                        uBitIndex = 0;              // Start over
                        ((LPAPLBOOL) lpMemObj)++;   // Skip to next byte
                    } // End IF
                } // End FOR

                break;

            case ARRAY_INT:
                // Loop through the array elements
                for (uObj = 0; uObj < aplNELMObj; uObj++, ((LPAPLINT) lpMemObj)++)
                    // Format the value
                    lpaplChar =
                      FormatAplintFC (lpaplChar,            // Ptr to output save area
                                     *(LPAPLINT) lpMemObj,  // The value to format
                                      UTF16_BAR);           // Char to use as overbar
                break;

            case ARRAY_FLOAT:
                // Loop through the array elements
                for (uObj = 0; uObj < aplNELMObj; uObj++, ((LPAPLFLOAT) lpMemObj)++)
                    // Format the value
                    lpaplChar =
                      FormatFloatFC (lpaplChar,             // Ptr to output save area
                                    *(LPAPLFLOAT) lpMemObj, // The value to format
                                     DEF_MAX_QUADPP64,      // Precision to use
                                     UTF16_DOT,             // Char to use as decimal separator
                                     UTF16_BAR,             // Char to use as overbar
                                     FLTDISPFMT_RAWFLT,     // Float display format
                                     TRUE);                 // TRUE iff we're to substitute text for infinity
                break;

            case ARRAY_CHAR:
                // Append a leading single quote
                *lpaplChar++ = WC_SQ;

                // Loop through the array elements
                for (uObj = 0; uObj < aplNELMObj; uObj++, ((LPAPLCHAR) lpMemObj)++)
                    // Format the text as an ASCII string with non-ASCII chars
                    //   represented as either {symbol} or {\xXXXX} where XXXX is
                    //   a four-digit hex number.
                    lpaplChar +=
                      ConvertWideToNameLength (lpaplChar,           // Ptr to output save buffer
                                               (LPAPLCHAR) lpMemObj,// Ptr to incoming chars
                                               1);                  // # chars to convert
                // Append a trailing single quote
                *lpaplChar++ = WC_SQ;

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
                  FormatAplintFC (lpaplChar,        // Ptr to output save area
                                  apaOff,           // The value to format
                                  UTF16_BAR);       // Char to use as overbar
                // Append the multiplier
                lpaplChar =
                  FormatAplintFC (lpaplChar,        // Ptr to output save area
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
                                    lpMemPTD->lphtsPTD->lpSymQuad[SYSVAR_PP]->stData.stInteger = DEF_MAX_QUADPP64;

                                // Format the value
                                lpaplChar =
                                  FormatImmedFC (lpaplChar,                             // Ptr to input string
                                                 stFlags.ImmType,                       // Immediate type
                                                &lpSymEntry->stData.stLongest,          // Ptr to value to format
                                                 DEF_MAX_QUADPP64,                      // Precision to use
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
                        break;

                    case PTRTYPE_HGLOBAL:
                    {
                        APLSTYPE aplTypeSub;

    #define hGlbSub     (*(LPAPLNESTED *) lpMemObj)

                        if (PtrReusedDir (hGlbObj))
                        {
                            // Append the HGLOBAL name
                            lpaplChar +=
                              wsprintfW (lpaplChar,
                                         L"REUSED ");
                        } else
                        {
                            // Get the global's Type
                            AttrsOfGlb (hGlbSub, &aplTypeSub, NULL, NULL, NULL);

                            // Convert the variable in global memory to saved ws form
                            lpaplChar =
                              SavedWsFormGlbVar (lpaplChar,
                                                 hGlbSub,
                                                 lpMemSaveWSID,
                                                 lpuGlbCnt,
                                                 lpSymEntry);
                            // Ensure there's a trailing blank
                            if (lpaplChar[-1] NE L' ')
                            {
                                *lpaplChar++ = L' ';
                                *lpaplChar   = WC_EOS;
                            } // End IF
    #undef  hGlbSub
                        } // End IF/ELSE

                        break;
                    } // End PTRTYPE_HGLOBAL

                    defstop
                        break;
                } // End FOR

                break;

            case ARRAY_RAT:
                // Loop through the array elements
                for (uObj = 0; uObj < aplNELMObj; uObj++, ((LPAPLRAT) lpMemObj)++)
                    // Format the value
                    lpaplChar =
                      FormatAplRatFC (lpaplChar,            // Ptr to output save area
                                     *(LPAPLRAT) lpMemObj,  // The value to format
                                      UTF16_BAR,            // Char to use as overbar
                                      L'/',                 // Char to use as rational separator
                                      TRUE);                // TRUE iff we're to substitute text for infinity
                break;

            case ARRAY_VFP:
                // Loop through the array elements
                for (uObj = 0; uObj < aplNELMObj; uObj++, ((LPAPLVFP) lpMemObj)++)
                    // Format the value
                    lpaplChar =
                      FormatAplVfpFC (lpaplChar,            // Ptr to output save area
                                     *(LPAPLVFP) lpMemObj,  // The value to format
                                      0,                    // # significant digits (0 = all)
                                      L'.',                 // Char to use as decimal separator
                                      UTF16_BAR,            // Char to use as overbar
                                      FALSE,                // TRUE iff nDigits is # fractional digits
                                      TRUE,                 // TRUE iff we're to substitute text for infinity
                                      uCommPrec EQ 0);      // TRUE iff we're to precede the display with (FPCnnn)
                break;

            defstop
                break;
        } // End SWITCH
    } __except (CheckException (GetExceptionInformation (), L"CmdSave_EM"))
    {
        if (hGlbObj && lpMemObj)
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
    wsprintfW (wszGlbCnt,
               FMTSTR_GLBCNT,
              *lpuGlbCnt);
    // Count in another entry
    (*lpuGlbCnt)++;

    // Write out the entry in the [Globals] section
    WritePrivateProfileStringW (SECTNAME_GLOBALS,               // Ptr to the section name
                                wszGlbCnt,                      // Ptr to the key value
                                lpMemProKeyName,                // Ptr to the key name
                                lpMemSaveWSID);                 // Ptr to the file name
    // Write out the saved marker
    WritePrivateProfileStringW (SECTNAME_TEMPGLOBALS,           // Ptr to the section name
                                wszGlbObj,                      // Ptr to the key name
                                wszGlbCnt,                      // Ptr to the key value
                                lpMemSaveWSID);                 // Ptr to the file name
    // Move back to the start
    lpaplChar = lpaplCharStart;

    // Copy the formatted GlbCnt to the start of the buffer as the result
    lstrcpyW (lpaplChar, wszGlbCnt);
NORMAL_EXIT:
    if (hGlbObj && lpMemObj)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbObj); lpMemObj = NULL;
    } // End IF

    // Return a ptr to the profile keyname's terminating zero
    return &lpaplChar[lstrlenW (lpaplChar)];
} // End SavedWsFormGlbVar


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
    if (lpuCommPrec)
        // Initialize it as none
        *lpuCommPrec = 0;

    // Append the storage type as a WCHAR
    *lpaplChar++ = aplCharObj;

    // Append a separator
    *lpaplChar++ = L' ';

    // Append the NELM (with a trailing blank)
    lpaplChar =
      FormatAplintFC (lpaplChar,                // Ptr to output save area
                      aplNELMObj,               // The value to format
                      UTF16_BAR);               // Char to use as overbar
    // Append the rank (with a trailing blank)
    lpaplChar =
      FormatAplintFC (lpaplChar,                // Ptr to output save area
                      aplRankObj,               // The value to format
                      UTF16_BAR);               // Char to use as overbar

    // Append the shape (each with a trailing blank)
    for (uObj = 0; uObj < aplRankObj; uObj++)
        // Format the dimension
        lpaplChar =
          FormatAplintFC (lpaplChar,            // Ptr to output save area
                          lpaplDimObj[uObj],    // The value to format
                          UTF16_BAR);           // Char to use as overbar
    // Append array properties
    if (lpHeader)
    {
        // Append leading separator
        *lpaplChar++ = L'(';

        // Check for array property:  PV0
        if (lpHeader->PV0)
        {
            lstrcpyW (lpaplChar, AP_PV0);           // Copy the prefix
            lpaplChar += strcountof (AP_PV0);       // Skip over it
            *lpaplChar++ = L' ';                    // Append a trailing blank
        } // End IF

        // Check for array property:  PV1
        if (lpHeader->PV1)
        {
            lstrcpyW (lpaplChar, AP_PV1);           // Copy the prefix
            lpaplChar += strcountof (AP_PV1);       // Skip over it
            *lpaplChar++ = L' ';                    // Append a trailing blank
        } // End IF

        // Check for array property:  All2s
        if (lpHeader->All2s)
        {
            lstrcpyW (lpaplChar, AP_ALL2S);         // Copy the prefix
            lpaplChar += strcountof (AP_ALL2S);     // Skip over it
            *lpaplChar++ = L' ';                    // Append a trailing blank
        } // End IF

        // If common VFP array precision requested, ...
        if (lpuCommPrec
         && IsVfp (lpHeader->ArrType))
        {
            LPAPLVFP lpaplVfp;

            // Skip over the header and dimensions to the data
            lpaplVfp = VarArrayBaseToData (lpHeader, aplRankObj);

            // Get the initial precision
            *lpuCommPrec = mpfr_get_prec (lpaplVfp++);

            for (uObj = 1; uObj < aplNELMObj; uObj++)
            if (*lpuCommPrec NE mpfr_get_prec (lpaplVfp++))
            {
                // Mark as none
                *lpuCommPrec = 0;

                break;
            } // End FOR

            // If there's a common precision, ...
            if (*lpuCommPrec NE 0)
            {
                lstrcpyW (lpaplChar, AP_FPC);       // Copy the prefix
                lpaplChar += strcountof (AP_FPC);   // Skip over it
                lpaplChar +=
                  wsprintfW (lpaplChar,             // Format the common precision
                             L"%I64u ",             // Note trailing blank
                            *lpuCommPrec);
            } // End IF
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
      SavedWsFormGlbVar (lpaplChar,
                         hGlbObj,
                         lpSavedWsGlbVarParm->lpMemSaveWSID,
                         lpSavedWsGlbVarParm->lpuGlbCnt,
                         lpSavedWsGlbVarParm->lpSymEntry);
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
                         lpSavedWsGlbFcnParm->lpMemSaveWSID,    // Ptr to saved WS file DPFE
                         lpSavedWsGlbFcnParm->lpuGlbCnt,        // Ptr to [Globals] count
                         lpSavedWsGlbFcnParm->lpSymEntry);      // Ptr to this global's SYMENTRY
    // Include a trailing blank
    *lpaplChar++ = L' ';

    return lpaplChar;
} // End SavedWsGlbFcnConv


//***************************************************************************
//  End of File: sc_save.c
//***************************************************************************
