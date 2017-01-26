//***************************************************************************
//  NARS2000 -- Magic Function -- IotaUnderbar
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
//  This file is encoded in UTF-8 format
//    DO NOT ATTEMPT TO EDIT IT WITH A NON-UNICODE EDITOR.
//***************************************************************************


//***************************************************************************
//  Magic function/operator for Extended Dyadic Iota Underbar
//
//  Extended Dyadic Iota Underbar -- Matrix Iota
//
//  Return an array of indices from searching for the
//    array  of trailing subarrays from the right arg in the
//    vector of trailing subarrays from the left arg.
//***************************************************************************

static APLCHAR DydHeader[] =
  L"Z←L " MFON_DydIotaUnderbar L" R";

static APLCHAR DydLine1[] = 
  L"Z←(⊂⍤¯1 L)⍳⊂⍤(¯1+⍴⍴L) R ⋄ →0";

static APLCHAR DydLine2[] = 
  L"⎕MS:Z←(⊂⍤¯1 L)⍳⊂⍤(¯1+⍴⍴L) R";

static LPAPLCHAR DydBody[] =
{DydLine1,
 DydLine2,
};

MAGIC_FCNOPR MFO_DydIotaUnderbar =
{DydHeader,
 DydBody,
 countof (DydBody),
};
  

//***************************************************************************
//  End of File: mf_iotaund.h
//***************************************************************************