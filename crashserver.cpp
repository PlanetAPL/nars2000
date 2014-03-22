//***************************************************************************
//  NARS2000 -- Crash Server Instantiation Code
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2014 Sudley Place Software

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

#ifndef DEBUG

#include <windows.h>
#include "CrashHandler.h"


//***************************************************************************
//  $CrashServer
//
//  Instantiate the Crash Server
//***************************************************************************

extern "C"
int CrashServer
    (void)

{
    // Instantiate the crash handler
    CrashHandler g_crashHandler
        (
         "87a09389-fa07-4be3-9555-4277d1a10209",    // GUID assigned to this application.
         "NARS2000",                                // Prefix that will be used with the dump name: YourPrefix_v1.v2.v3.v4_YYYYMMDD_HHMMSS.mini.dmp.
        L"NARS2000",                                // Application name that will be used in message box.
        L"Sudley Place Software"                    // Company name that will be used in message box.
        );

    return g_crashHandler.IsCrashHandlingEnabled ();
} // End CrashServer

#endif


//***************************************************************************
//  End of File: crashserver.cpp
//***************************************************************************
