//***************************************************************************
//  NARS2000 -- Magic Function -- Dyadic Scan
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
//  Magic function/operator for dyadic derived function from the scan monadic operator
//***************************************************************************

static APLCHAR DydHeader[] =
  L"Z←L (LO " MFON_DydScan L"[X]) R";

static APLCHAR DydLine1[] = 
  L":if 0=⎕NC 'X'";

static APLCHAR DydLine2[] = 
  L"  Z←L LO/(((¯1↓⍴R),0⌈(|L)-1)⍴LO/0⍴R),R";

static APLCHAR DydLine3[] = 
  L":else";

static APLCHAR DydLine4[] = 
  L"  Z←L LO/[X]((((X≠⍳⍴⍴R)/⍴R),0⌈(|L)-1)[⍋⍋X=⍳⍴⍴R]⍴LO/0⍴R),[X]R";

static APLCHAR DydLine5[] = 
  L":endif";

static LPAPLCHAR DydBody[] =
{DydLine1,
 DydLine2,
 DydLine3,
 DydLine4,
 DydLine5,
};

MAGIC_FCNOPR MFO_DydScan =
{DydHeader,
 DydBody,
 countof (DydBody),
};


//***************************************************************************
//  Magic function/operator for dyadic derived function from the scan monadic operator
//***************************************************************************

static APLCHAR Dyd1Header[] =
  L"Z←L (LO " MFON_DydScan1 L"[X]) R";

static APLCHAR Dyd1Line1[] = 
  L":if 0=⎕NC 'X'";

static APLCHAR Dyd1Line2[] = 
  L"  Z←L LO⌿(((0⌈(|L)-1),1↓⍴R)⍴LO/0⍴R)⍪R";

static APLCHAR Dyd1Line3[] = 
  L":else";

static APLCHAR Dyd1Line4[] = 
  L"  Z←L LO/[X]((((X≠⍳⍴⍴R)/⍴R),0⌈(|L)-1)[⍋⍋X=⍳⍴⍴R]⍴LO/0⍴R),[X]R";

static APLCHAR Dyd1Line5[] = 
  L":endif";

static LPAPLCHAR Dyd1Body[] =
{Dyd1Line1,
 Dyd1Line2,
 Dyd1Line3,
 Dyd1Line4,
 Dyd1Line5,
};

MAGIC_FCNOPR MFO_DydScan1 =
{Dyd1Header,
 Dyd1Body,
 countof (Dyd1Body),
};



//***************************************************************************
//  End of File: mf_dydscan.h
//***************************************************************************