//***************************************************************************
//  NARS2000 -- Transfer Form Header File
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


#define EBCDIC_BLANK        0x40
#define EBCDIC_STAR         0x5C
#define EBCDIC_X            0xE7

#define REC_LEN               80                            // Overall record length (excluding CR and CRLF)
#define TYP_LEN                1                            // Type char length ('*', ' ', 'X')
#define SEQ_LEN                8                            // Sequence numbers length in the record tail
#define INT_LEN             (REC_LEN - TYP_LEN - SEQ_LEN)   // Interior length ("- 1" for the first col)


//***************************************************************************
//  End of File: transfer.h
//***************************************************************************
