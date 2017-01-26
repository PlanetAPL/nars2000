//***************************************************************************
//  NARS2000 -- System Command:  )OUT
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
#include "transfer.h"
#include "debug.h"              // For xxx_TEMP_OPEN macros


// Instead of sequence numbers, why not ...
char Trees[] =
"I think that I shall never see "
"A poem lovely as a tree. "

"A tree whose hungry mouth is prest "
"Against the sweet earth's flowing breast; "

"A tree that looks at God all day, "
"And lifts her leafy arms to pray; "

"A tree that may in summer wear "
"A nest of robins in her hair; "

"Upon whose bosom snow has lain; "
"Who intimately lives with rain. "

"Poems are made by fools like me, "
"But only God can make a tree. "

"-- Joyce Kilmer (1886-1918), published 1914";


//***************************************************************************
//  $CmdOut_EM
//
//  Execute the system command:  )OUT filename.ext
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- CmdOut_EM"
#else
#define APPEND_NAME
#endif

UBOOL CmdOut_EM
    (LPWCHAR lpwszTail)                     // Ptr to command line tail

{
    LPPERTABDATA lpMemPTD;                  // Ptr to PerTabData global memory
    WCHAR        wszDrive[_MAX_DRIVE],
                 wszDir  [_MAX_DIR],
                 wszFname[_MAX_FNAME],
                 wszExt  [_MAX_EXT];
    LPWCHAR      lpwszTemp,                 // Ptr to temporary storage
                 lpwTemp,                   // Ptr to temporary
                 lpwszSwitch,               // Ptr to list of switches
                 lpwGlbName;                // Ptr to SymEntry's name's global memory
    char         szTemp[REC_LEN + EOL_LEN]; // Save area for line output
    UINT         uLen;                      // Current length of szTemp
    FILE        *fStream;                   // Ptr to file stream
    UBOOL        bRet = FALSE,              // TRUE iff the result is valid
                 bStdSysName = FALSE;       // TRUE iff we should save APL Standard names only
    LPSYMENTRY   lpSymEntry,                // Ptr to STE
                 lpSymTabNext;              // ...
    APLNELM      aplNELMRes;                // Length of each transfer form
    FILETIME     ftCreation;                // Save area for function timestamp
    SYSTEMTIME   systemTime;                // Current system (UTC) time
    UINT         uCnt,                      // Loop counter
                 uSwitchCnt;                // # switches on the command line
    LPCHAR       lpTrees;                   // Ptr to Trees poem
    VARS_TEMP_OPEN

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get ptr to temporary format save area
    lpwszTemp = lpMemPTD->lpwszTemp;
    CHECK_TEMP_OPEN

    // Check for empty string
    if (lpwszTail[0] EQ WC_EOS)
        goto INCORRECT_COMMAND_EXIT;

    // Split off the filename from (optional) switches
    lpwszSwitch = SplitSwitches (lpwszTail, &uSwitchCnt);

    // If there are any switches, ...
    while (uSwitchCnt--)
    {
        // Validate the switches
#define SWITCH      L"-std"
        if (lstrcmpW (lpwszSwitch, SWITCH) EQ 0)
        {
            bStdSysName = TRUE;
            lpwszSwitch += strcountof (SWITCH);
#undef  SWITCH
            if (*lpwszSwitch++ EQ WC_EOS)
                continue;
        } else
        {
        } // End IF/ELSE

        goto INCORRECT_COMMAND_SWITCH_EXIT;
    } // End WHILE

    // Strip out leading/trailing DQs
    lpwszTail = StripDQ (lpwszTail);

    // Split out the drive and path from the module filename
    _wsplitpath (lpwszTail, wszDrive, wszDir, wszFname, wszExt);

    // If there's no extension, supply one
    if (wszExt[0] EQ WC_EOS)
        // Put it all back together
        _wmakepath  (lpwszTail, wszDrive, wszDir, wszFname, WS_ATFEXT);

    // Attempt to open the file (in binary mode)
    fStream = fopenW (lpwszTail, L"wb");

    // If not found, ...
    if (fStream EQ NULL)
    {
        ReplaceLastLineCRPmt (ERRMSG_PATH_NOT_FOUND APPEND_NAME);

        goto ERROR_EXIT;
    } // End IF

    // Initialize the end of szTemp
    szTemp[REC_LEN + 0] = AC_CR;
    szTemp[REC_LEN + 1] = AC_LF;

    // Copy the poem's starting point so we may
    //   increment our way through it
    lpTrees = Trees;

    __try
    {
        // Trundle through the Symbol Table
        for (lpSymTabNext = lpMemPTD->lphtsPTD->lpSymTab;
             lpSymTabNext < lpMemPTD->lphtsPTD->lpSymTabNext;
             lpSymTabNext++)
        if (lpSymTabNext->stFlags.Inuse)        // Must be Inuse
        {
            // As this is a system command, we write
            //   out the global entry only
            lpSymEntry = lpSymTabNext;
            while (lpSymEntry->stPrvEntry)
                lpSymEntry = lpSymEntry->stPrvEntry;

            if (lpSymEntry->stHshEntry->htGlbName                   // Must have a name (not steZero, etc.),
             && lpSymEntry->stFlags.Value                           // and have a value,
             && lpSymEntry->stFlags.ObjName NE OBJNAME_MFO          // and not be a Magic Function/Operator,
             && lpSymEntry->stFlags.ObjName NE OBJNAME_LOD          // and not be a )LOAD HGLOBAL
             && (!bStdSysName                                       // If bStdSysName
              || (lpSymEntry->stFlags.ObjName NE OBJNAME_SYS)       // and it's a SysName
              || lpSymEntry->stFlags.StdSysName))                   //     it must be a StdSysName
            {
                // Copy ptr to increment
                lpwTemp = lpwszTemp;

                // Split cases based upon the name type
                switch (lpSymEntry->stFlags.stNameType)
                {
                    case NAMETYPE_UNK:      // Unknown -- ignore
                        break;

                    case NAMETYPE_VAR:
                        // Check for []DM and []WSID and skip it if found

                        // Lock the memory to get a ptr to it
                        lpwGlbName = MyGlobalLockWsz (lpSymEntry->stHshEntry->htGlbName);

                        // Compare the names
                        uCnt = lstrcmpiW (lpwGlbName, $QUAD_WSID) EQ 0
                            || lstrcmpiW (lpwGlbName, $QUAD_DM  ) EQ 0;

                        // We no longer need this ptr
                        MyGlobalUnlock (lpSymEntry->stHshEntry->htGlbName); lpwGlbName = NULL;

                        if (uCnt)
                            continue;

                        // Mark as a variable
                        *lpwTemp++ = L'A';

                        // Display the var
                        lpwTemp =
                          DisplayTransferVar2 (lpwTemp, lpSymEntry);

                        // Clear the timestamp
                        ZeroMemory (&ftCreation, sizeof (ftCreation));

                        break;

                    case NAMETYPE_FN0:
                    case NAMETYPE_FN12:
                    case NAMETYPE_OP1:
                    case NAMETYPE_OP2:
                    case NAMETYPE_OP3:
                    case NAMETYPE_TRN:
                        // Mark as a function
                        *lpwTemp++ = L'F';

                        // Lock the memory to get a ptr to it
                        lpwGlbName = MyGlobalLockWsz (lpSymEntry->stHshEntry->htGlbName);

                        // Copy the function name
                        strcpyW (lpwTemp, lpwGlbName);

                        // We no longer need this ptr
                        MyGlobalUnlock (lpSymEntry->stHshEntry->htGlbName); lpwGlbName = NULL;

                        // Skip over it
                        lpwTemp += lstrlenW (lpwTemp);

                        // Insert a blank as a separator
                        *lpwTemp++ = ' ';

                        // Display the fcn
                        lpwTemp =
                          DisplayTransferFcn2 (lpwTemp, lpSymEntry, &ftCreation);

                        break;

                    defstop
                        break;
                } // End IF/SWITCH

                // Get the string length
                aplNELMRes = lpwTemp - lpwszTemp;

                // Translate the data from NARS to APL2 charset
                TranslateNARSToAPL2 (lpwszTemp, aplNELMRes, TRUE);

                // If there's a function timestamp, ...
                if (ftCreation.dwHighDateTime NE 0
                 || ftCreation.dwLowDateTime NE 0)
                {
                    // Convert the creation time to system time so we can display it
                    FileTimeToSystemTime (&ftCreation, &systemTime);

                    // Format it
                    uLen =
                      wsprintf (szTemp,
                                "*(%u %u %u %u %u %u %u)",
                                systemTime.wYear,
                                systemTime.wMonth,
                                systemTime.wDay,
                                systemTime.wHour,
                                systemTime.wMinute,
                                systemTime.wSecond,
                                systemTime.wMilliseconds);
                    // Fill it out with blanks
                    FillMemory (&szTemp[uLen], TYP_LEN + INT_LEN - uLen, ' ');

                    // Insert the next part of the poem
                    lpTrees = InsertNextTrees (szTemp, lpTrees);

                    // Write it out (including the trailing "\r\n")
                    fwrite (szTemp, sizeof (char), REC_LEN + EOL_LEN, fStream);
                } // End IF

                // Block the output in lpwszTemp and write
                //   it out one record at a time
                for (lpwTemp = lpwszTemp;
                     aplNELMRes;
                     aplNELMRes -= uLen)
                {
                    // Mark as to whether or not it's the last record
                    szTemp[0] = (aplNELMRes <= INT_LEN) ? AC_X : AC_BLANK;

                    // Get the remaining length
                    uLen = min (INT_LEN, (UINT) aplNELMRes);

                    // Copy the data to szTemp
                    for (uCnt = 0; uCnt < uLen; uCnt++)
                        szTemp[uCnt + TYP_LEN] = (char) *lpwTemp++;

                    // Fill the remaining memory with blanks
                    FillMemory (&szTemp[uCnt + TYP_LEN], INT_LEN - uCnt, ' ');

                    // Insert the next part of the poem
                    lpTrees = InsertNextTrees (szTemp, lpTrees);

                    // Write it out (including the trailing "\r\n")
                    fwrite (szTemp, sizeof (char), REC_LEN + EOL_LEN, fStream);
                } // End FOR
            } // End IF
        } // End IF
    } __except (CheckException (GetExceptionInformation (), L"CmdOut_EM"))
    {
        // Display message for unhandled exception
        DisplayException ();
    } // End __try/__except

    // Mark as sucessful
    bRet = TRUE;

    goto NORMAL_EXIT;

INCORRECT_COMMAND_EXIT:
    IncorrectCommand ();

    goto ERROR_EXIT;

INCORRECT_COMMAND_SWITCH_EXIT:
    IncorrectCommandSwitch ();

    goto ERROR_EXIT;

ERROR_EXIT:
NORMAL_EXIT:
    if (fStream)
    {
        // We no longer need this resource
        fclose (fStream); fStream = NULL;
    } // End IF

    EXIT_TEMP_OPEN

    return bRet;
} // End CmdOut_EM
#undef  APPEND_NAME


//***************************************************************************
//  $InsertNextTrees
//
//  Insert into the sequence number field the next chunk
//    from Kilmer's Trees poem
//***************************************************************************

LPCHAR InsertNextTrees
    (LPCHAR szTemp,                     // Ptr to 80-col record
     LPCHAR lpTrees)                    // Ptr to next chunk from the poem

{
    UINT uCnt;                          // Loop counter

    // Insert the next chunk
    for (uCnt = 0; (uCnt < SEQ_LEN) && *lpTrees; uCnt++)
        szTemp[TYP_LEN + INT_LEN + uCnt] = *lpTrees++;

    // In case we're at the end, fill with blanks
    for (;uCnt < SEQ_LEN; uCnt++)
        szTemp[TYP_LEN + INT_LEN + uCnt] = ' ';

    return lpTrees;
} // End InsertNextTrees


//***************************************************************************
//  End of File: sc_out.c
//***************************************************************************
