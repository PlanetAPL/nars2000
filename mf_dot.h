//***************************************************************************
//  NARS2000 -- Magic Function -- Dot
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
//  Magic function/operator for identity function from the
//    inner product operator
//***************************************************************************

static APLCHAR IdnHeader[] =
  L"Z←" MFON_IdnDot L" R";

static APLCHAR IdnLine1[] = 
  L"⎕ID:Z←∘.=⍨⍳¯1↑1,⍴R";

static LPAPLCHAR IdnBody[] =
{IdnLine1,
};

MAGIC_FCNOPR MFO_IdnDot =
{IdnHeader,
 IdnBody,
 countof (IdnBody),
};

//***************************************************************************
//  Magic function/operator for initialization of the determinant operator's
//    subroutines.
//
//	This algorithm was taken from SATN-42 (Sharp APL Technical Notes),
//	  1982-04-01, "Determinant-Like Functions Produced by the Dot Operator",
//	  by K.E. Iverson (http://www.jsoftware.com/papers/satn42.htm),
//	  with minor changes.
//***************************************************************************

static APLCHAR IniHeader[] =
  MFON_MonDotInit;

static APLCHAR IniLine1[] = 
  MFON_MonDotAll L"←{⎕IO←1 ⋄ y←" MFON_MonDotPerm L"⍴⍵ ⋄ k←⍴⍴y ⋄ ((⍳k),k)⍉⍵[y;]}";

static APLCHAR IniLine2[] = 
  MFON_MonDotPerm L"←{s←(⌊/⍵)↑⌽⍳1↑⍵ ⋄ (s,⍴s)⍴⍉" MFON_MonDotMf L" 1+s⊤¯1+⍳×/s}";

static APLCHAR IniLine3[] = 
  MFON_MonDotMf L"←{0=1↑⍴⍵:⍵ ⋄ x←∇ 1 0↓⍵ ⋄ ⍵[1;]⍪x+⍵[(1↑⍴x)⍴1;]≤x}";

static APLCHAR IniLine4[] = 
  MFON_MonDotCr L"←{0=⍴⍴⍵:⍵ ⋄ ∇ ⍺⍺/⍵}";

static LPAPLCHAR IniBody[] =
{IniLine1,
 IniLine2,
 IniLine3,
 IniLine4,
};

MAGIC_FCNOPR MFO_MonDotInit =
{IniHeader,
 IniBody,
 countof (IniBody),
};

//***************************************************************************
//  Magic function/operator for determinant operator from the
//    inner product operator.
//
//	This algorithm was taken from SATN-42 (Sharp APL Technical Notes),
//	  1982-04-01, "Determinant-Like Functions Produced by the Dot Operator",
//	  by K.E. Iverson (http://www.jsoftware.com/papers/satn42.htm), 
//	  with minor changes.
//***************************************************************************

static APLCHAR MonHeader[] =
  L"Z←(LO " MFON_MonDot L" RO) R";

//L"⍝ Generalized determinant operator";

static APLCHAR MonLine1[] =
  L"Z←⊃LO " MFON_MonDotCr L" RO/" MFON_MonDotAll L"⍪R ⋄ →0";

static APLCHAR MonLine2[] = 
  L"⎕PRO:Z←⊃LO/⍬";

static LPAPLCHAR MonBody[] =
{MonLine1,
 MonLine2,
};

MAGIC_FCNOPR MFO_MonDot =
{MonHeader,
 MonBody,
 countof (MonBody),
};


//***************************************************************************
//  End of File: mf_dot.h
//***************************************************************************