//***************************************************************************
//  NARS2000 -- System Command:  )RESET
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
//  $CmdReset_EM
//
//  Execute the system command:  )RESET [n]
//***************************************************************************

UBOOL CmdReset_EM
    (LPWCHAR lpwszTail)         // Ptr to command line tail

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // ***FIXME*** -- Make sensitive to [n] in lpwszTail

    // If there's an SIS layer, ...
    if (lpMemPTD->lpSISCur)
    {
        // Set the reset flag
        lpMemPTD->lpSISCur->ResetFlag = RESETFLAG_ALL;

        Assert (lpMemPTD->lpSISCur->hSemaphore NE NULL);

        // Set the immediate execution exit type
        lpMemPTD->ImmExecExitType = EXITTYPE_RESET_ALL;

        // Signal the next layer to begin resetting
        MyReleaseSemaphore (lpMemPTD->lpSISCur->hSemaphore, 1, NULL);
    } else
        DisplayPrompt (GetThreadSMEC (), 3);

    return TRUE;
} // End CmdReset_EM


//***************************************************************************
//  End of File: sc_reset.c
//***************************************************************************
