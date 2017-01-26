//***************************************************************************
//  MAKEVER -- Make a version string into a file
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

#define EQ ==
#define NE !=


char szVarFileInfo[] = "\\VarFileInfo\\Translation";


//***************************************************************************
//  LclFileVersionStr
//
//  Get file version string
//***************************************************************************

void LclFileVersionStr
    (LPSTR pszFileName,
     LPSTR szFileVer)

{
    DWORD dwVerHandle;  // Version file handle
    DWORD dwVerSize;    // Size of the VERSIONINFO struc
    DWORD dwTrans;  // Translation value
    LPSTR lpVer, lpBuf;
    HLOCAL hLoc;
    UINT  cb;
    char szTemp[128];

    szTemp[0] = '\0';       // Ensure properly terminated in case we fail
    dwVerSize = GetFileVersionInfoSize (pszFileName, &dwVerHandle);

    if (dwVerSize EQ 0)     // If it didn't work, ...
        return;

    // Allocate space for the file version info
    hLoc = LocalAlloc (LPTR, (long) dwVerSize);
    lpVer = (LPSTR) hLoc;
    if (lpVer EQ NULL)      // If it didn't work
        return;

    // Read in the file version info
    if (!GetFileVersionInfo (pszFileName, dwVerHandle, dwVerSize, lpVer))
        goto FREE;

    // Get the translation string
    // Note that we must put the VarFileInfo string into read-write
    // memory because, although the prototype for VerQueryValue has
    // this string cast as LPCSTR, the Win 3.1x version of this API
    // call writes into the string (zapping the intermediate backslash
    // so it can lstrcmpi against various possible cases).
////if (!VerQueryValue (lpVer, "\\VarFileInfo\\Translation", &lpBuf, &cb))
    if (!VerQueryValue (lpVer, szVarFileInfo, &lpBuf, &cb))
        goto FREE;

    // Extract the translation value (with the swapped words)
    dwTrans = *(DWORD FAR *)lpBuf;
    wsprintf (szTemp, "\\StringFileInfo\\%08lX\\FileVersion",
            MAKELONG (HIWORD (dwTrans), LOWORD (dwTrans)));
    if (!VerQueryValue (lpVer, szTemp, &lpBuf, &cb))
        goto FREE;

    // Copy to local storage as we're about to free the memory
    lstrcpy (szFileVer, lpBuf);
FREE:
    LocalFree (hLoc);       // Free the local memory
} // End LclFileVersionStr


//***************************************************************************
//  WinMain
//
//  Start the process
//***************************************************************************

int PASCAL WinMain
    (HINSTANCE   hInstance,
     HINSTANCE   hPrevInstance,     // Zero under Win32
     LPSTR       lpCmdLine,
     int         nCmdShow)

{
    char szFileVer[32], *p;
    FILE *fp;

    // Strip off leading blanks
    p = lpCmdLine;
    while (*p EQ ' ') p++;

    // Read in the application's File Version String
    LclFileVersionStr (p, szFileVer);

    // Write it out to a standard file
    fp = fopen ("nars2000.ver", "w");
    fprintf (fp, "%s", szFileVer);
    fclose (fp); fp = NULL;

    return 0;
} // End WinMain


//***************************************************************************
//  End of File: makever.c
//***************************************************************************
