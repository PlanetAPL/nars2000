//***************************************************************************
//  NARS2000 -- Quad DR Header
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2012 Sudley Place Software

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


typedef enum tagDR_VAL
{
    DR_SHOW          =    0,    // Return a character vector representation
    DR_FLOAT2CHAR    =    1,    // Convert between float   and its 16-digit hexadecimal character representation
    DR_INT2CHAR      =    2,    // Convert between integer and its 16-digit hexadecimal character representation
    DR_BOOL          =  110,    //  1 bit  per value
    DR_CHAR8         =  811,    //  8 bits ...
    DR_CHAR16        = 1611,    // 16 ...  ...
    DR_CHAR32        = 3211,    // 32 ...  ...
    DR_INT8          =  812,    // INT8 one-dimensional numbers
    DR_INT16         = 1612,    // INT16 ...
    DR_INT32         = 3212,    // INT32 ...
    DR_INT64         = 6412,    // INT64 ...
    DR_FLOAT         = 6413,    // FLT64 ...
    DR_RAT           =   14,    // RAT bytes per item plus size of limbs
    DR_VFP           =   15,    // VFP ...
    DR_COMPLEX_I     = 1216,    // INT64 two-dimensional numbers
    DR_COMPLEX_F     = 1316,    // FLT64 ...
    DR_COMPLEX_Q     = 1416,    // RAT   ...
    DR_COMPLEX_V     = 1516,    // VFP   ...
    DR_QUATERNIONS_I = 1217,    // INT64 four-dimensional numbers
    DR_QUATERNIONS_F = 1317,    // FLT64 ...
    DR_QUATERNIONS_Q = 1417,    // RAT   ...
    DR_QUATERNIONS_V = 1517,    // VFP   ...
    DR_OCTONIONS_I   = 1218,    // INT64 eight-dimensional numbers
    DR_OCTONIONS_F   = 1318,    // FLT64 ...
    DR_OCTONIONS_Q   = 1418,    // RAT   ...
    DR_OCTONIONS_V   = 1518,    // VFP   ...
    DR_APA           =   19,    // APA bits offset & multiplier
    DR_HETERO        =   20,    // PTR ...  per item
    DR_NESTED        =   21,    // PTR ...  ...
} DR_VAL, *LPDR_VAL;


//***************************************************************************
//  End of File: qf_dr.h
//***************************************************************************
