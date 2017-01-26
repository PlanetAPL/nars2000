//***************************************************************************
//  NARS2000 -- System Function -- Quad NFNS Header
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

enum tagNINFOPROPERTY           // []NINFO Properties
{
    NPROP_DPFE       = 0,       // 0 = DPFE
    NPROP_TYPE          ,       // 1 = Type (see NFILETYPES)
    NPROP_SIZE          ,       // 2 = Size in bytes
    NPROP_LASTMOD       ,       // 3 = Last modification time
    NPROP_OWNER_ID      ,       // 4 = Owner user ID
    NPROP_OWNER_NAME    ,       // 5 = Owner name
    NPROP_HIDDEN        ,       // 6 = Hidden or not
    NPROP_SYMBLINK      ,       // 7 = Target of symbolic link
    NPROP_LENGTH        ,       // 8 = # entries in this enum
} NINIFO_PROPERTY, *LPNINFO_PROPERTY;

// N.B.:  Whenever changing the above enum (NINFO_PROPERTY),
//   be sure to make a corresponding change to
//   <NINFO_FILETYPES> in <qf_nfns.h>, and
//   <propArrType> in <qf_nfns.c>.

enum tagNINFOFILETYPES          // []NINFO File Types
{
    NFILETYPE_UNK = 0,          // 0:  Unknown
    NFILETYPE_DIR    ,          // 1:  Directory
    NFILETYPE_REG    ,          // 2:  Regular file
    NFILETYPE_CHRDEV ,          // 3:  Character device
    NFILETYPE_LINK   ,          // 4:  Symbolic link (only when Follow = 0)
    NFILETYPE_BLKDEV ,          // 5:  Block device
    NFILETYPE_FIFO   ,          // 6:  FIFO (not Windows)
    NFILETYPE_SOCKET ,          // 7:  Socket (not Windows)
} NINFO_FILETYPES, *LPNINFO_FILETYPES;

#define NPROP_INIT      100     // # initial entries when allocating NINFO properties for wildcards
#define NPROP_INCR      100     // # entries to create on 2nd and subsequent allocations

typedef struct tagNPROPSTR
{
    UINT    uWildCur,           // 00:  Current wildcard index
            uWildMax;           // 04:  Maximum ...
    HGLOBAL hWildGlb;           // 08:  Wildcard HGLOBAL
                                // 0C:  Length in bytes
} NPROP_STR, *LPNPROP_STR;


//***************************************************************************
//  End of File: qf_nfns.h
//***************************************************************************
