//***************************************************************************
//  NARS2000 -- Magic Function -- DotDot
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
//  Magic function/operator for Extended Dyadic DotDot
//
//  Extended Dyadic DotDot -- Sequence Generator For Nested Arrays
//
//  This algorithm is a generalization of 2..5 proposed by John Scholes.
//***************************************************************************

static APLCHAR DydHeader[] =
  L"Z←L " MFON_DydDotDot L" R;⎕IO;⎕CT";
  
static APLCHAR DydLine1[] = 
  L"⎕IO←0 ⋄ ⎕CT←3E¯15";
  
static APLCHAR DydLine2[] = 
  L"L←2↑L,1";

static APLCHAR DydLine3[] = 
  L"Z←((⊃R)-0⊃L)÷|1⊃L";

static APLCHAR DydLine4[] = 
  L"L[1]←⊂(|1⊃L)××Z";
  
static APLCHAR DydLine5[] = 
  L"Z←L[0]+L[1]×⍳1+⊃⌊|Z";

  static LPAPLCHAR DydBody[] =
{DydLine1,
 DydLine2,
 DydLine3,
 DydLine4,
 DydLine5,
};

MAGIC_FCNOPR MFO_DydDotDot =
{DydHeader,
 DydBody,
 countof (DydBody),
};


//***************************************************************************
//  End of File: mf_dotdot.h
//***************************************************************************