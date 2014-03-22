//***************************************************************************
//  NARS2000 -- System Command Routines
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
#include "headers.h"

typedef UBOOL (SYSCMD) (LPWCHAR);
typedef SYSCMD *LPSYSCMD;

typedef struct tagSYSCMDSTAB
{
    LPWCHAR      lpszName;
    LPSYSCMD     lpSysCmd_EM;
    SYSCMDS_ENUM sysCmdEnum;
} SYSCMDSTAB, *LPSYSCMDSTAB;

// The lookup table for all system commands
SYSCMDSTAB SysCmdsTab[]
#ifndef PROTO
 =
{
    {L"clear"     ,     &CmdClear_EM    , SYSCMD_CLEAR   },
    {L"close"     ,     &CmdClose_EM    , SYSCMD_CLOSE   },
    {L"copy"      ,     &CmdCopy_EM     , SYSCMD_COPY    },
    {L"drop"      ,     &CmdDrop_EM     , SYSCMD_DROP    },
    {L"edit"      ,     &CmdEdit_EM     , SYSCMD_EDIT    },
    {L"erase"     ,     &CmdErase_EM    , SYSCMD_ERASE   },
    {L"exit"      ,     &CmdExit_EM     , SYSCMD_EXIT    },
    {L"fns"       ,     &CmdFns_EM      , SYSCMD_FNS     },
    {L"in"        ,     &CmdIn_EM       , SYSCMD_IN      },
    {L"inascii"   ,     &CmdInAscii_EM  , SYSCMD_INASCII },
    {L"lib"       ,     &CmdLib_EM      , SYSCMD_LIB     },
    {L"laod"      ,     &CmdLoad_EM     , SYSCMD_LOAD    },
    {L"load"      ,     &CmdLoad_EM     , SYSCMD_LOAD    },
    {L"newtab"    ,     &CmdNewTab_EM   , SYSCMD_NEWTAB  },
    {L"nms"       ,     &CmdNms_EM      , SYSCMD_NMS     },
    {L"off"       ,     &CmdExit_EM     , SYSCMD_EXIT    },
    {L"open"      ,     &CmdLoad_EM     , SYSCMD_LOAD    },
    {L"ops"       ,     &CmdOps_EM      , SYSCMD_OPS     },
    {L"out"       ,     &CmdOut_EM      , SYSCMD_OUT     },
    {L"reset"     ,     &CmdReset_EM    , SYSCMD_RESET   },
    {L"save"      ,     &CmdSave_EM     , SYSCMD_SAVE    },
    {L"svae"      ,     &CmdSave_EM     , SYSCMD_SAVE    },
    {L"si"        ,     &CmdSi_EM       , SYSCMD_SI      },
    {L"sic"       ,     &CmdReset_EM    , SYSCMD_RESET   },
    {L"sinl"      ,     &CmdSinl_EM     , SYSCMD_SINL    },
    {L"ulib"      ,     &CmdUlib_EM     , SYSCMD_ULIB    },
    {L"vars"      ,     &CmdVars_EM     , SYSCMD_VARS    },
    {L"xload"     ,     &CmdXload_EM    , SYSCMD_XLOAD   },
    {L"wsid"      ,     &CmdWsid_EM     , SYSCMD_WSID    },
}
#endif
;

// The # rows in the above table
#define SYSCMDSTAB_NROWS    countof (SysCmdsTab)


//***************************************************************************
//  $IncorrectCommand
//
//  Signal an incorrect command
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- IncorrectCommand"
#else
#define APPEND_NAME
#endif

void IncorrectCommand
    (void)

{
    ReplaceLastLineCRPmt (ERRMSG_INCORRECT_COMMAND APPEND_NAME);
} // End IncorrectCommand
#undef  APPEND_NAME


//***************************************************************************
//  $IncorrectCommandSwitch
//
//  Signal an incorrect command switch
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- IncorrectCommandSwitch"
#else
#define APPEND_NAME
#endif

void IncorrectCommandSwitch
    (void)

{
    ReplaceLastLineCRPmt (ERRMSG_INCORRECT_COMMAND_SWITCH APPEND_NAME);
} // End IncorrectCommandSwitch
#undef  APPEND_NAME


//***************************************************************************
//  $ExecSysCmd
//
//  Execute a system command
//***************************************************************************

SYSCMDS_ENUM ExecSysCmd
    (LPWCHAR lpwszLine,         // Ptr to system command line to execute
     HWND    hWndEC)            // Edit Ctrl window handle

{
    int     i;
    LPWCHAR wp, wpEnd;

    // Set the cursor to indicate the new state
    ForceSendCursorMsg (hWndEC, TRUE);

    // As this command might take a while, update the EC window so as
    //   to avoid it looking like we're frozen
    UpdateWindow (hWndEC);

    // Skip over the leading ')' and any following spaces
    while (*++lpwszLine EQ L' ');

    // Look for the next space as a separator
    wp = strchrW (lpwszLine, L' ');

    // If not found, goto the end of the line
    if (wp EQ NULL)
        wp = &lpwszLine[lstrlenW (lpwszLine)];
    else
    {
        // Make the command into a zero-terminated string
        *wp++ = WC_EOS;

        // Otherwise, skip over all leading spaces
        while (*wp EQ L' ')
            wp++;
    } // End IF/ELSE

    // Point to the end of the string
    wpEnd = &wp[lstrlenW (wp)];

    // Delete trailing blanks
    while (wpEnd > lpwszLine && wpEnd[-1] EQ L' ')
        *--wpEnd = WC_EOS;

    // Search for this command in the table
    for (i = 0; i < SYSCMDSTAB_NROWS; i++)
    if (lstrcmpiW (SysCmdsTab[i].lpszName, lpwszLine) EQ 0)
    {
        // Execute the system command
        (*SysCmdsTab[i].lpSysCmd_EM) (wp);

        // Return the type of system command
        return SysCmdsTab[i].sysCmdEnum;
    } // End IF/FOR

    IncorrectCommand ();

    return SYSCMD_None;
} // End ExecSysCmd


//***************************************************************************
//  $CmdClear_EM
//
//  Execute the system command:  )CLEAR
//***************************************************************************

UBOOL CmdClear_EM
    (LPWCHAR lpwszTail)

{
    if (!OptionFlags.bNewTabOnClear)
        CloseTab (TabCtrl_GetCurSel (hWndTC));

    return CreateNewTab (hWndMF,
                         L"",
                         TabCtrl_GetItemCount (hWndTC),
                         FALSE);
} // End CmdClear_EM


//***************************************************************************
//  $CmdClose_EM
//
//  Execute the system command:  )CLOSE
//***************************************************************************

UBOOL CmdClose_EM
    (LPWCHAR lpwszTail)

{
    if (TabCtrl_GetItemCount (hWndTC) EQ 1)
    {
        ReplaceLastLineCRPmt (L"Can't close last workspace -- use )EXIT");

        return FALSE;
    } // End IF

    // Set the over tab index
    gOverTabIndex = TabCtrl_GetCurSel (hWndTC);

    return CloseTab (gOverTabIndex);
} // End CmdClose_EM


//***************************************************************************
//  $CmdEdit_EM
//
//  Execute the system command:  )EDIT
//***************************************************************************

UBOOL CmdEdit_EM
    (LPWCHAR lpwszTail)

{
    // ***FIXME*** -- Make sensitive to user option to edit a
    //                function, vector, or matrix by default

    // Open a Function Editor window
    return CreateFcnWindow (lpwszTail);
} // End CmdEdit_EM


//***************************************************************************
//  $CmdExit_EM
//
//  Execute the system command:  )EXIT
//***************************************************************************

UBOOL CmdExit_EM
    (LPWCHAR lpwszTail)

{
    PostMessageW (hWndMF, WM_CLOSE, 0, 0);

    return TRUE;
} // End CmdExit_EM


//***************************************************************************
//  $CmdNewTab_EM
//
//  Execute the system command:  )NEWTAB
//***************************************************************************

UBOOL CmdNewTab_EM
    (LPWCHAR lpwszTail)

{
    return CreateNewTab (hWndMF,
                         L"",
                         TabCtrl_GetItemCount (hWndTC),
                         FALSE);
} // End CmdNewTab_EM


//***************************************************************************
//  End of File: syscmds.c
//***************************************************************************
