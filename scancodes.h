//***************************************************************************
//  NARS2000 -- Scan Codes
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2011 Sudley Place Software

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

typedef enum tagSCSANCODES
{
    SCANCODE_UNK00           ,      // 0x00
    SCANCODE_ESC             ,      // 0x01
    SCANCODE_1               ,      // 0x02
    SCANCODE_2               ,      // 0x03
    SCANCODE_3               ,      // 0x04
    SCANCODE_4               ,      // 0x05
    SCANCODE_5               ,      // 0x06
    SCANCODE_6               ,      // 0x07
    SCANCODE_7               ,      // 0x08
    SCANCODE_8               ,      // 0x09
    SCANCODE_9               ,      // 0x0A
    SCANCODE_0               ,      // 0x0B
    SCANCODE_MINUS           ,      // 0x0C
    SCANCODE_EQUAL           ,      // 0x0D
    SCANCODE_BS              ,      // 0x0E
    SCANCODE_HT              ,      // 0x0F

    SCANCODE_CR = 0x1C       ,      // 0x1C
    SCANCODE_CTRL            ,      // 0x1D     Left & Right

    SCANCODE_LSHFT = 0x2A    ,      // 0x2A

    SCANCODE_DECIMAL = 0x34  ,      // 0x34
    SCANCODE_PADSLASH        ,      // 0x35     and '/'
    SCANCODE_RSHFT           ,      // 0x36
    SCANCODE_PADSTAR         ,      // 0x37     and PrntScrn
    SCANCODE_ALT             ,      // 0x38     Left & Right
    SCANCODE_SP              ,      // 0x39
    SCANCODE_CAPSLOCK        ,      // 0x3A
    SCANCODE_F1              ,      // 0x3B
    SCANCODE_F2              ,      // 0x3C
    SCANCODE_F3              ,      // 0x3D
    SCANCODE_F4              ,      // 0x3E
    SCANCODE_F5              ,      // 0x3F
    SCANCODE_F6              ,      // 0x40
    SCANCODE_F7              ,      // 0x41
    SCANCODE_F8              ,      // 0x42
    SCANCODE_F9              ,      // 0x43
    SCANCODE_F10             ,      // 0x44
    SCANCODE_NUMLOCK         ,      // 0x45
    SCANCODE_SCRLOCK         ,      // 0x46
    SCANCODE_HOME            ,      // 0x47
    SCANCODE_UP              ,      // 0x48
    SCANCODE_PGUP            ,      // 0x49
    SCANCODE_PADMINUS        ,      // 0x4A
    SCANCODE_LEFT            ,      // 0x4B
    SCANCODE_CENTER          ,      // 0x4C ??
    SCANCODE_RIGHT           ,      // 0x4D
    SCANCODE_PADPLUS         ,      // 0x4E
    SCANCODE_END             ,      // 0x4F
    SCANCODE_DOWN            ,      // 0x50
    SCANCODE_PGDN            ,      // 0x51
    SCANCODE_INS             ,      // 0x52
    SCANCODE_DEL             ,      // 0x53
    SCANCODE_SYSREQ          ,      // 0x54
    SCANCODE_UNK55           ,      // 0x55
    SCANCODE_UNK56           ,      // 0x56
    SCANCODE_F11             ,      // 0x57
    SCANCODE_F12             ,      // 0x58
    SCANCODE_UNK59           ,      // 0x59
    SCANCODE_UNK5A           ,      // 0x5A
    SCANCODE_LWIN            ,      // 0x5B
    SCANCODE_RWIN            ,      // 0x5C
    SCANCODE_APP             ,      // 0x5D
    SCANCODE_POWER           ,      // 0x5E
    SCANCODE_SLEEP           ,      // 0x5F
    SCANCODE_NONO60          ,      // 0x60
    SCANCODE_NONO61          ,      // 0x61
    SCANCODE_UNK62           ,      // 0x62
    SCANCODE_WAKE            ,      // 0x63
} SCANCODES, *LPSCANCODES;


//***************************************************************************
//  End of File: scancodes.h
//***************************************************************************
