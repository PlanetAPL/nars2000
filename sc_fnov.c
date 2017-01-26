//***************************************************************************
//  NARS2000 -- System Command:  )FNS, )NMS, )OPS, )VARS
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


// Calculate length of trailing text for )NMS
#define NMSLEN      strcountof (L".nn")


//***************************************************************************
//  $CmdFns_EM
//
//  Execute the system command:  )FNS  [first][-][last]
//***************************************************************************

UBOOL CmdFns_EM
    (LPWCHAR lpwszTail)             // Ptr to command line tail

{
    return CmdFNOV_EM (lpwszTail, IzitFNS, FALSE);
} // End CmdFns_EM


//***************************************************************************
//  $IzitFNS
//
//  Return TRUE iff the object name type is that of a function
//***************************************************************************

UBOOL IzitFNS
    (NAME_TYPES stNameType)

{
    return IsNameTypeFn (stNameType);
} // End IzitFNS


//***************************************************************************
//  $CmdNms_EM
//
//  Execute the system command:  )NMS  [first][-][last]
//***************************************************************************

UBOOL CmdNms_EM
    (LPWCHAR lpwszTail)             // Ptr to command line tail

{
    return CmdFNOV_EM (lpwszTail, IzitNMS, TRUE);
} // End CmdNms_EM


//***************************************************************************
//  $IzitNMS
//
//  Return TRUE iff the object type is that of a variable/function/operator
//***************************************************************************

UBOOL IzitNMS
    (NAME_TYPES stNameType)

{
    return IsNameTypeName (stNameType);
} // End IzitNMS


//***************************************************************************
//  $CmdOps_EM
//
//  Execute the system command:  )OPS  [first][-][last]
//***************************************************************************

UBOOL CmdOps_EM
    (LPWCHAR lpwszTail)             // Ptr to command line tail

{
    return CmdFNOV_EM (lpwszTail, IzitOPS, FALSE);
} // End CmdOps_EM


//***************************************************************************
//  $IzitOPS
//
//  Return TRUE iff the object type is that of an operator
//***************************************************************************

UBOOL IzitOPS
    (NAME_TYPES stNameType)

{
    return IsNameTypeOp (stNameType);
} // End IzitOPS


//***************************************************************************
//  $CmdFops_EM
//
//  Execute the system command:  )FOPS  [first][-][last]
//***************************************************************************

UBOOL CmdFops_EM
    (LPWCHAR lpwszTail)             // Ptr to command line tail

{
    return CmdFNOV_EM (lpwszTail, IzitFOPS, FALSE);
} // End CmdOps_EM


//***************************************************************************
//  $IzitFOPS
//
//  Return TRUE iff the object type is that of a fucntion/operator
//***************************************************************************

UBOOL IzitFOPS
    (NAME_TYPES stNameType)

{
    return IsNameTypeFnOp (stNameType);
} // End IzitFOPS


//***************************************************************************
//  $CmdVars_EM
//
//  Execute the system command:  )VARS [first][-][last]
//***************************************************************************

UBOOL CmdVars_EM
    (LPWCHAR lpwszTail)             // Ptr to command line tail

{
    return CmdFNOV_EM (lpwszTail, IzitVARS, FALSE);
} // End CmdVars_EM


//***************************************************************************
//  $IzitVARS
//
//  Return TRUE iff the object type is that of a variable
//***************************************************************************

UBOOL IzitVARS
    (NAME_TYPES stNameType)

{
    return IsNameTypeVar (stNameType);
} // End IzitVARS


//***************************************************************************
//  $CmdFNOV_EM
//
//  Execute the system command:  )FNS  [first][-][last]
//                               )NMS  [first][-][last]
//                               )OPS  [first][-][last]
//                            or )VARS [first][-][last]
//***************************************************************************

UBOOL CmdFNOV_EM
    (LPWCHAR lpwszTail,             // Ptr to command line tail
     UBOOL (*IzitFVO) (NAME_TYPES), // Ptr to function to determine name type
     UBOOL   bNMS)                  // TRUE iff the command is )NMS

{
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPWCHAR      lpwszFormat,       // Ptr to formatting save area
                 lpwszLeadRange,    // Ptr to the leading range
                 lpwszTailRange,    // ...        trailing ... (after the separator)
                                    //                         (may be NULL if no separator)
                *lplpwszArgs;       // Ptr to ptr to args
    LPSYMENTRY   lpSymEntry;        // Ptr to current SYMENTRY
    LPAPLCHAR    lpMemName;         // Ptr to name global memory
    LPSYMENTRY  *lpSymSort;         // Ptr to LPSYMENTRYs for sorting
    UINT         uSymCnt,           // Count of # matching STEs
                 uSymNum,           // Loop counter
                 uLineChar,         // Current char # in output line
                 uNameLen,          // Length of the current name
                 uQuadPW,           // Current value of []PW
                 uArgCnt,           // # args after the system command
                 uLeadRangeLen,     // Length of lpwszLeadRange
                 uTailRangeLen,     // ...       lpwszTailRange
                 uMaxNameLen = 0;   // Length of longest name
    UBOOL        bLineCont;         // TRUE iff this line is a continuation

    // Check for command line switches
    if (!CheckCommandLine (lpwszTail,           // Ptr to command line (after the command itself)
                          &uArgCnt,             // Ptr to # args
                          &lplpwszArgs,         // Ptr to ptr to ptr to args
                          &lpwszLeadRange,      // Ptr to ptr to leading range
                          &lpwszTailRange,      // ...           trailing range
                           FALSE))              // TRUE iff )LIB
        return FALSE;

    // Set the range lengths
    uLeadRangeLen = lstrlenW (lpwszLeadRange);
    if (lpwszTailRange)
        uTailRangeLen = lstrlenW (lpwszTailRange);
    else
        uTailRangeLen = 0;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get ptr to formatting save area
    lpwszFormat = lpMemPTD->lpwszFormat;

    // Initialize the LPSYMENTRY sort array
    lpSymSort = (LPSYMENTRY *) lpMemPTD->lpwszTemp;

    // Trundle through the Symbol Table
    //   looking for functions (niladic or monadic/dyadic),
    //   operators (monadic or dyadic), and/or variables
    for (lpSymEntry = lpMemPTD->lphtsPTD->lpSymTab, uSymCnt = 0;
         lpSymEntry < lpMemPTD->lphtsPTD->lpSymTabNext;
         lpSymEntry++)
    if (lpSymEntry->stFlags.Inuse
     && lpSymEntry->stFlags.Value
     && lpSymEntry->stFlags.ObjName NE OBJNAME_MFO
     && (lpSymEntry->stFlags.ObjName NE OBJNAME_SYS
      || lpSymEntry->stFlags.DfnLabel)
     && (*IzitFVO) (lpSymEntry->stFlags.stNameType))
    {
        LPSYMENTRY lpGlbEntry;

        // Find a ptr to the global SYMENTRY (stPrvEntry EQ NULL)
        for (lpGlbEntry = lpSymEntry;
             lpGlbEntry->stPrvEntry;
             lpGlbEntry = lpGlbEntry->stPrvEntry)
            ;

        // Lock the memory to get a ptr to it
        lpMemName = MyGlobalLockWsz (lpGlbEntry->stHshEntry->htGlbName);

        // Check against leading and trailing ranges
        if ((lpwszLeadRange[0] EQ WC_EOS
          || strncmpiW (lpMemName, lpwszLeadRange, uLeadRangeLen) >= 0)
         && (lpwszTailRange NE NULL
          || strncmpiW (lpMemName, lpwszLeadRange, uLeadRangeLen) EQ 0)
         && (lpwszTailRange EQ NULL
          || lpwszTailRange[0] EQ WC_EOS
          || strncmpiW (lpMemName, lpwszTailRange, uTailRangeLen) <= 0))
        {
            // Get the name length
            uNameLen = lstrlenW (lpMemName) + bNMS * NMSLEN;

            // Find the longest name
            uMaxNameLen = max (uMaxNameLen, uNameLen);

            // Save the LPSYMENTRY ptr for later use
            lpSymSort[uSymCnt] = lpGlbEntry;

            // Count in another matching name
            uSymCnt++;
        } // End IF

        // We no longer need this ptr
        MyGlobalUnlock (lpGlbEntry->stHshEntry->htGlbName); lpMemName = NULL;
    } // End FOR/IF

    // Sort the HGLOBALs
    ShellSort (lpSymSort, uSymCnt, CmpLPSYMENTRY);

    // Get the current value of []PW
    uQuadPW = (UINT) GetQuadPW ();

    // Re-initialize the output area
    FillMemoryW (lpwszFormat, uQuadPW, L' ');

    // Display the names
    for (bLineCont = FALSE, uLineChar = uSymNum = 0;
         uSymNum < uSymCnt;
         uSymNum++)
    {
        // Lock the memory to get a ptr to it
        lpMemName = MyGlobalLockWsz (lpSymSort[uSymNum]->stHshEntry->htGlbName);

        // Get the name length
        uNameLen = lstrlenW (lpMemName);

        // If the line is too long, skip to the next one
        if ((uLineChar + uNameLen + bNMS * NMSLEN) > uQuadPW
         && uLineChar NE 0)
        {
            // Ensure properly terminated
            lpwszFormat[min (uLineChar, uQuadPW)] = WC_EOS;

            // Output the current line
            AppendLine (lpwszFormat, bLineCont, TRUE);

            // Mark all lines from here on as continuations
            bLineCont = TRUE;

            // Re-initialize the output area
            FillMemoryW (lpwszFormat, uQuadPW, L' ');

            // Re-initialize the char counter
            uLineChar = 0;
        } // End IF

        // Copy the name to the output area
        CopyMemoryW (&lpwszFormat[uLineChar], lpMemName, uNameLen);

        // If it's )NMS, append the name class
        if (bNMS)
        {
            UINT uNC;               // Name class

            // Calculate the name class
            uNC = (UINT) CalcNameClass (lpSymSort[uSymNum]);

            lpwszFormat[uLineChar + uNameLen + 0] = L'.';

            // If the name class is two-digits, ...
            if (uNC > 9)
            {
                lpwszFormat[uLineChar + uNameLen + 1] = L'0' + uNC / 10;
                lpwszFormat[uLineChar + uNameLen + 2] = L'0' + uNC % 10;
            } else
                lpwszFormat[uLineChar + uNameLen + 1] = L'0' + uNC;
        } // End IF

        // Skip to the next name boundary
        uLineChar += uMaxNameLen + 1;

        // We no longer need this ptr
        MyGlobalUnlock (lpSymSort[uSymNum]->stHshEntry->htGlbName); lpMemName = NULL;
    } // End FOR

    // If there's still text in the buffer, output it
    if (uLineChar NE 0)
    {
        // Ensure properly terminated
        lpwszFormat[uLineChar] = WC_EOS;

        // Output the current line
        AppendLine (lpwszFormat, TRUE, TRUE);
    } // End IF

    return FALSE;
} // End CmdFNOV_EM


//***************************************************************************
//  End of File: sc_fnov.c
//***************************************************************************
