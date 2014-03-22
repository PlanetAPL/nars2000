//***************************************************************************
//  NARS2000 -- []EC Header
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


typedef enum tagE_RETCODES          // []EC[0] return codes
{
    EC_RETCODE_ERROR = 0    ,       // 00:  Error
    EC_RETCODE_DISPLAY      ,       // 01:  Result displayed
    EC_RETCODE_NODISPLAY    ,       // 02:  Result not displayed
    EC_RETCODE_NOVALUE      ,       // 03:  No result
    EC_RETCODE_GOTO_LINE    ,       // 04:  {goto} line #
    EC_RETCODE_RESET_ONE    ,       // 05:  {goto}
    EC_RETCODE_UNK = -1             // -1:  Unknow error code
} EC_RETCODES, *LPEC_RETCODES;


//***************************************************************************
//  End of File: qf_ec.h
//***************************************************************************
