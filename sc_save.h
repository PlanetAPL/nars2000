//***************************************************************************
//  NARS2000 -- Saved WS Vars
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


//***************************************************************************
//  Saved ws vars
//***************************************************************************

typedef struct tagSAVEDWSVAR_CB
{
    LPWSTR       lpwAppName;    // Section name containing the key name
    LPWSTR       lpwKeyName;    // Key name whose associated string is to be retrieved
    LPDICTIONARY lpDict;        // Ptr to workspace dictionary
    int          iMaxLength,    // Maximum length, although it may be exceeded by a bit
                 iIndex;        // Index of key in lpDict->key (-1 = not saved)
} SAVEDWSVAR_CB, *LPSAVEDWSVAR_CB;


//***************************************************************************
//  End of File: sc_save.h
//***************************************************************************
