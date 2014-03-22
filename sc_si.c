//***************************************************************************
//  NARS2000 -- System Command:  )SI
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
//  $CmdSi_EM
//
//  Execute the system command:  )SI [n]
//***************************************************************************

UBOOL CmdSi_EM
    (LPWCHAR lpwszTail)         // Ptr to command line tail

{
    return CmdSiSinlCom_EM (lpwszTail, FALSE, NULL);
} // End CmdSi_EM


//***************************************************************************
//  $CmdSinl_EM
//
//  Execute the system command:  )SINL [n]
//***************************************************************************

UBOOL CmdSinl_EM
    (LPWCHAR lpwszTail)         // Ptr to command line tail

{
    return CmdSiSinlCom_EM (lpwszTail, TRUE, NULL);
} // End CmdSinl_EM


//***************************************************************************
//  $CmdSiSinlCom_EM
//
//  Execute the system command:  )SI or )SINL
//***************************************************************************

UBOOL CmdSiSinlCom_EM
    (LPWCHAR   lpwszTail,       // Ptr to command line tail
     UBOOL     bSINL,           // TRUE iff )SINL
     LPAPLCHAR lpMemSaveWSID)   // Ptr to the file name (NULL = not called from CmdSave_EM)

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory
    LPSIS_HEADER lpSISCur;      // Ptr to current SIS_HEADER srtuc
    UINT         SILevel;       // SI level (for CmdSave_EM)
    APLCHAR      szSILevel[10]; // Formatted SI level
    LPWCHAR      lpwszFormat,   // Ptr to formatting save area
                 lpwszTemp;     // Ptr to temporary storage
    VARS_TEMP_OPEN

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get ptr to formatting save area & temporary storage
    lpwszFormat = lpMemPTD->lpwszFormat;
    lpwszTemp   = lpMemPTD->lpwszTemp;
    CHECK_TEMP_OPEN

    // ***FIXME*** -- Make sensitive to [n] in lpwszTail

#ifdef DEBUG
    if (lpMemPTD->lpSISCur EQ NULL && lpMemSaveWSID EQ NULL)
       AppendLine (WS_UTF16_ZILDE, FALSE, TRUE);
    else
#endif
    // Loop backwards through the SI levels
    for (lpSISCur = lpMemPTD->lpSISCur, SILevel = 0;
         lpSISCur;
         lpSISCur = lpSISCur->lpSISPrv, SILevel++)
    {
        LPAPLCHAR lpMemName;            // Ptr to function name global memory

        // Format the SI level
        wsprintfW (szSILevel,
                   L"%d",
                   SILevel);
        // Split cases based upon the caller's function type
        switch (lpSISCur->DfnType)
        {
            case DFNTYPE_IMM:
#ifdef DEBUG
                // If it's not CmdSave_EM, ...
                if (lpMemSaveWSID EQ NULL)
                {
                    AppendLine (WS_UTF16_IOTA, FALSE, TRUE);
                } // End IF
#endif
                break;

            case DFNTYPE_OP1:
            case DFNTYPE_OP2:
            case DFNTYPE_FCN:
#ifndef DEBUG
                // If it's a Magic Function/Operator, skip it
                if (lpSISCur->PermFn)
                    break;
#endif
                // Lock the memory to get a ptr to it
                lpMemName = MyGlobalLock (lpSISCur->hGlbFcnName);

                // If it's CmdSave_EM, ...
                if (lpMemSaveWSID)
                    // Format the text as an ASCII string with non-ASCII chars
                    //   represented as either {symbol} or {\xXXXX} where XXXX is
                    //   a four-digit hex number.
                    ConvertWideToName (lpwszFormat, lpMemName);

                // Format the Name, Line #, and Suspension marker
                wsprintfW (lpwszTemp,
                           L"%s[%d] %c",
                           (lpMemSaveWSID EQ NULL) ? lpMemName : lpwszFormat,
                           lpSISCur->CurLineNum,
                           " *"[lpSISCur->Suspended]);
                // We no longer need this ptr
                MyGlobalUnlock (lpSISCur->hGlbFcnName); lpMemName = NULL;

                // If it's not CmdSave_EM, ...
                if (lpMemSaveWSID EQ NULL)
                    // Display the function name & line #
                    AppendLine (lpwszTemp, FALSE, !bSINL);
                else
                    WritePrivateProfileStringW (SECTNAME_SI,    // Ptr to the section name
                                                szSILevel,      // Ptr to the key name
                                                lpwszTemp,      // Ptr to the key value
                                                lpMemSaveWSID); // Ptr to the file name
                // If it's )SINL, display the namelist
                if (bSINL)
                {
                    UINT       numSymEntries,   // # LPSYMENTRYs localized on the stack
                               numSym,          // Loop counter
                               uLen;            // Loop counter
                    LPSYMENTRY lpSymEntryNxt;   // Ptr to next localized LPSYMENTRY on the SIS
                    APLUINT    uQuadPW,         // []PW
                               uLineInd,        // Line indent
                               uLineLen,        // Current line length
                               uNameLen;        // Name length
                    LPAPLCHAR  lpw;             // Ptr to end of name

                    // Get the current value of []PW
                    uQuadPW = GetQuadPW ();

                    // Initialize the line indent and current line length
                    uLineLen = uLineInd = lstrlenW (lpwszTemp);

                    // Get # LPSYMENTRYs on the stack
                    numSymEntries = lpSISCur->numSymEntries;

                    // Point to the localized LPSYMENTRYs
                    lpSymEntryNxt = (LPSYMENTRY) ByteAddr (lpSISCur, sizeof (SIS_HEADER));

                    // Loop through the # LPSYMENTRYs
                    for (numSym = 0; numSym < numSymEntries; numSym++)
                    {
                        // Copy the STE name to local storage
                        lpw = CopySteName (lpwszTemp, &lpSymEntryNxt[numSym], &uNameLen);

                        // Ensure properly terminated
                        *lpw = WC_EOS;

                        // Get the name length plus two leading blanks
                        uNameLen += 2;

                        // If the name's length would push us over the
                        //   page width, go to a new line
                        if (uQuadPW < (uLineLen + uNameLen))
                        {
                            AppendLine (L"", TRUE, TRUE);
                            for (uLen = 0; uLen < uLineInd; uLen++)
                                AppendLine (L" ", TRUE, FALSE);
                            uLineLen = uLineInd;
                        } // End IF

                        // Display the name preceded by two blanks
                        AppendLine (L"  ", FALSE, FALSE);
                        AppendLine (lpwszTemp, FALSE, FALSE);

                        uLineLen += uNameLen;
                    } // End FOR

                    // End the last line
                    AppendLine (L"", TRUE, TRUE);
                } // End IF

                break;

            case DFNTYPE_EXEC:
                // If it's not CmdSave_EM, ...
                if (lpMemSaveWSID EQ NULL)
                    AppendLine (WS_UTF16_UPTACKJOT, FALSE, TRUE);
                else
                    WritePrivateProfileStringW (SECTNAME_SI,        // Ptr to the section name
                                                szSILevel,          // Ptr to the key name
                                                L"{uptackjot}",     // Ptr to the key value
                                                lpMemSaveWSID);     // Ptr to the file name
                break;

            case DFNTYPE_QUAD:
                // If it's not CmdSave_EM, ...
                if (lpMemSaveWSID EQ NULL)
                    AppendLine (WS_UTF16_QUAD, FALSE, TRUE);
                else
                    WritePrivateProfileStringW (SECTNAME_SI,        // Ptr to the section name
                                                szSILevel,          // Ptr to the key name
                                                L"{quad}",          // Ptr to the key value
                                                lpMemSaveWSID);     // Ptr to the file name
                break;

            case DFNTYPE_UNK:
            defstop
                break;
        } // End SWITCH
    } // End IF/FOR

    EXIT_TEMP_OPEN

    return TRUE;
} // End CmdSiSinlCom_EM


//***************************************************************************
//  End of File: sc_si.c
//***************************************************************************
