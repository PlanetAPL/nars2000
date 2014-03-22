//***************************************************************************
//  NARS2000 -- Magic Function -- Iota
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2013 Sudley Place Software

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
//  Magic function/operator for Extended Monadic Iota
//
//  Extended Monadic Iota -- Index Generator For Arrays
//
//  Return an array of indices appropriate to an array of
//    the same shape as the right arg.
//
//  I believe this algorithm was first created by Carl M. Cheney.
//***************************************************************************

static APLCHAR MonHeader[] =
  L"Z←" MFON_MonIota L" R";

static APLCHAR MonLine1[] = 
  L"Z←⊃∘.,/⍳¨R";

static LPAPLCHAR MonBody[] =
{MonLine1,
};

MAGIC_FCNOPR MFO_MonIota =
{MonHeader,
 MonBody,
 countof (MonBody),
};

  
//***************************************************************************
//  Magic function/operator for extended dyadic iota
//
//  Extended dyadic iota
//
//  On rank > 1 left args, return an array of vector indices
//    such that A[A iota R] is R, assuming that all of R is in A.
//***************************************************************************

static APLCHAR DydHeader[] =
  L"Z←L " MFON_DydIota L" R;⎕IO;O;A";

static APLCHAR DydLine1[] = 
  L"O←⎕IO ⋄ ⎕IO←0";

static APLCHAR DydLine2[] = 
  L"Z←(0,⍴L)⊤(,L)⍳R";

static APLCHAR DydLine3[] = 
  L"A←⍸1=0⌷[0] Z";

static APLCHAR DydLine4[] = 
  L"Z[(⍳1+⍴⍴L)∘.,A]←⊃[0] (⍴A)⍴⊂0,⍴L";

static APLCHAR DydLine5[] = 
  L"Z←⊂[0] O+1↓[0] Z";
  
static LPAPLCHAR DydBody[] =
{DydLine1,
 DydLine2,
 DydLine3,
 DydLine4,
 DydLine5,
};

MAGIC_FCNOPR MFO_DydIota =
{DydHeader,
 DydBody,
 countof (DydBody),
};
  

//***************************************************************************
//  End of File: mf_iota.h
//***************************************************************************