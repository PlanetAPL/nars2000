//***************************************************************************
//  NARS2000 -- Magic Function -- DieresisDownTack
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
//  Magic function/operator for identity function from the
//    Convolution operator
//***************************************************************************

static APLCHAR IdnHeader[] =
  L"Z←(LO " MFON_IdnConv L" RO) R";

static APLCHAR IdnLine1[] = 
  L"⎕ID:Z←LO/RO/⍬";

static LPAPLCHAR IdnBody[] =
{IdnLine1,
};

MAGIC_FCNOPR MFO_IdnConv =
{IdnHeader,
 IdnBody,
 countof (IdnBody),
};


//***************************************************************************
//  Magic function/operator for dyadic derived function from the convolution dyadic operator
//
//  This operator is based upon design from the original NARS system.
//***************************************************************************

static APLCHAR DydHeader[] =
  L"Z←L (LO " MFON_DydConv L" RO) R;LN;RN";

//  ⍝ Convolution of LO and RO between L and R
static APLCHAR DydLine1[] = 
  L"L←1/L ⋄ R←1/R ⋄ Z←⍬";

static APLCHAR DydLine2[] = 
  L"⎕ERROR ((1≠≢⍴L)∨1≠≢⍴R)/'RANK ERROR'";

static APLCHAR DydLine3[] = 
  L"LN←≢L ⋄ RN←≢R";

//  ⍝ Short leading

static APLCHAR DydLine4[] = 
  L":if 1<LN⌊RN";

static APLCHAR DydLine5[] = 
  L"  :forlcl I :in 1..¯1+LN⌊RN";

static APLCHAR DydLine6[] = 
  L"    Z←Z,(I↑L) LO.RO ⌽I↑R";

static APLCHAR DydLine7[] =
  L"  :end";

static APLCHAR DydLine8[] =
  L":end";

//  ⍝ Full middle

static APLCHAR DydLine9[] =
  L"  :forlcl I :in 0..|LN-RN";

static APLCHAR DydLine10[] =
  L"    :if LN≥RN";

static APLCHAR DydLine11[] =
  L"      Z←Z,(RN↑I↓L) LO.RO ⌽R";

static APLCHAR DydLine12[] =
  L"    :else";

static APLCHAR DydLine13[] =
  L"      Z←Z,L LO.RO ⌽LN↑I↓R";

static APLCHAR DydLine14[] =
  L"    :end";

static APLCHAR DydLine15[] =
  L"  :end";

//  ⍝ Short trailing

static APLCHAR DydLine16[] =
  L":if 1<LN⌊RN";

static APLCHAR DydLine17[] =
  L"  :forlcl I :in -⌽1..¯1+LN⌊RN";

static APLCHAR DydLine18[] =
  L"    Z←Z,(I↑L) LO.RO ⌽I↑R";

static APLCHAR DydLine19[] =
  L"  :end";

static APLCHAR DydLine20[] =
  L":end";

static LPAPLCHAR DydBody[] =
{DydLine1,
 DydLine2,
 DydLine3,
 DydLine4,
 DydLine5,
 DydLine6,
 DydLine7,
 DydLine8,
 DydLine9,
 DydLine10,
 DydLine11,
 DydLine12,
 DydLine13,
 DydLine14,
 DydLine15,
 DydLine16,
 DydLine17,
 DydLine18,
 DydLine19,
 DydLine20,
};

MAGIC_FCNOPR MFO_DydConv =
{DydHeader,
 DydBody,
 countof (DydBody),
};


//***************************************************************************
//  End of File: mf_didtack.h
//***************************************************************************