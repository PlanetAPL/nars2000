//***************************************************************************
//  NARS2000 -- Check ESP
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2009 Sudley Place Software

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
//  _chkesp
//
//  This needs to be in a separate file so its prototype
//    isn't included in any file because C doesn't like the (naked).
//***************************************************************************

void __declspec (naked) __cdecl _chkesp (void)
{
    _asm {  jz      exit_chkesp     };
    _asm {  int     3           };
exit_chkesp:
    _asm {  ret             };
} // End _chkesp


//***************************************************************************
//  End of File: chkesp.c
//***************************************************************************
