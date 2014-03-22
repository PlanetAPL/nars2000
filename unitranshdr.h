//****************************************************************************
//  NARS2000 -- Unicode Translation Header
//****************************************************************************

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


typedef enum tagUNI_TRANS
{
    UNITRANS_APLWIN = 0,    // 00:  APL+Win 3.6 (and maybe later)
    UNITRANS_ISO,           // 01:  ISO Standard (What Typeface is this???)
    UNITRANS_APL2,          // 02:  APL2 ???     ***FINISHME***
    UNITRANS_DYALOG,        // 03:  Dyalog ???   ***FINISHME***
    UNITRANS_PC3270,        // 04:  PC3270
    UNITRANS_NORMAL,        // 05:  Normal paste translation (from Ctrl-v)
    UNITRANS_NARS,          // 06:  NARS2000
    UNITRANS_LENGTH,        // 07:  Length of the enum *MUST* be last active value
    UNITRANS_BRACES,        // 08:  {braces} handled outside uniTransTab
                            // 09-0F:  Available entries (4 bits)
} UNI_TRANS;

// N.B.:  Whenever changing the above enum (UNI_TRANS),
//   be sure to make a corresponding change to
//   <uniTransStr> in <customize.c>, and
//   <uniTransTab> in <unitranstab.h>.


//***************************************************************************
//  End of File: unitranshdr.h
//***************************************************************************
