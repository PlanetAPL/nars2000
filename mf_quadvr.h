//***************************************************************************
//  NARS2000 -- Magic Function -- Quad VR
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
//  Magic function/operator for Monadic []VR
//
//  Monadic []VR -- Visual Representation
//
//  Return a visual representation of a user-defined function/operator
//***************************************************************************

static APLCHAR MonHeaderVR[] =
  L"Z←" MFON_MonVR L" R;L;⎕IO";

static APLCHAR MonLineVR1[] =  
  L"⎕IO←0 ⋄ Z←1 ##.⎕CR R";
  
static APLCHAR MonLineVR2[] =  
  L"→(0≠⍴Z)/L1 ⋄ Z←'' ⋄ →0";
  
static APLCHAR MonLineVR3[] =  
  L"L1:→(1=≡Z)/0";
  
static APLCHAR MonLineVR4[] =  
  L"L←'∇'∊0⊃Z";
  
static APLCHAR MonLineVR5[] =  
  L"Z←(∊((⊂[1]'LO<    ∇>P<[>Q<]>I6' ⎕FMT⍳⍴Z),¨Z),¨⎕TCNL),'    ∇'";
  
static APLCHAR MonLineVR6[] =  
  L"→L/0 ⋄ Z←Z,(' ',⍕2 ##.⎕AT R),' (UTC)'";

static LPAPLCHAR MonBodyVR[] =
{MonLineVR1,
 MonLineVR2,
 MonLineVR3,
 MonLineVR4,
 MonLineVR5,
 MonLineVR6,
};

MAGIC_FCNOPR MFO_MonVR =
{MonHeaderVR,
 MonBodyVR,
 countof (MonBodyVR),
};


//***************************************************************************
//  End of File: mf_quadvr.h
//***************************************************************************