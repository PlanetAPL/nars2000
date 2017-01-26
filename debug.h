//***************************************************************************
//  NARS2000 -- Debugging Header For Detecting Unsynchronized Variables
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

#ifdef DEBUG

#define VARS_TEMP_OPEN                              \
    WCHAR   wszTempName[256];                       \
    LPWCHAR lpwszOldName;

#define CHECK_TEMP_OPEN                             \
    if (lpMemPTD->bTempOpen)                        \
        DbgBrk ();      /* #ifdef DEBUG */          \
    else                                            \
    {                                               \
        lpwszOldName = lpMemPTD->lpwszTempName;     \
        MySprintfW (wszTempName,                    \
                    sizeof (wszTempName),           \
                   L"%S#%d",                        \
                    FNLN);                          \
        lpMemPTD->lpwszTempName = wszTempName;      \
        lpMemPTD->bTempOpen     = TRUE;             \
        lpMemPTD->lpwszFILE     = WFILE;            \
        lpMemPTD->lpwszLINE     = WLINE;            \
    } /* End IF */

#define EXIT_TEMP_OPEN                              \
    lpMemPTD->bTempOpen     = FALSE;                \
    lpMemPTD->lpwszTempName = lpwszOldName;

#else

#define VARS_TEMP_OPEN
#define CHECK_TEMP_OPEN
#define EXIT_TEMP_OPEN

#endif


//***************************************************************************
//  End of File: debug.h
//***************************************************************************
