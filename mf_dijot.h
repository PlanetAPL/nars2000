//***************************************************************************
//  NARS2000 -- Magic Function -- DieresisJot
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
//  Magic function/operator for monadic derived function from the rank dyadic operator
//
//  This operator was originally based upon code found in the paper by J. Philip Benkhard,
//    "Extending structure, type, and expression in APL2", ACM SIGAPL APL Quote Quad,
//    v.21 n.4, p.20-38, Aug. 1991, but later was modified to reflect the
//    definition of the Rank operator in the ISO-IEC 13751 Extended APL Standard.
//***************************************************************************

static APLCHAR MonHeader[] =
  L"Z←(LO " MFON_MonRank L" Y) R;YR";

static APLCHAR MonLine1[] = 
  L"YR←(1↑⌽3⍴⌽Y)⌊⍴⍴R";

static APLCHAR MonLine2[] = 
  L":if 0>YR ⋄ YR←0⌈YR+⍴⍴R ⋄ :end";

static APLCHAR MonLine3[] = 
  L"Z←LO¨⊂[(-YR)↑⍳⍴⍴R] R ⋄ →0";

static APLCHAR MonLine4[] = 
  L"⎕PRO:YR←(1↑⌽3⍴⌽Y)⌊⍴⍴R";

static APLCHAR MonLine5[] = 
  L":if 0>YR ⋄ YR←0⌈YR+⍴⍴R ⋄ :end";

static APLCHAR MonLine6[] = 
  L"Z←⊃LO¨¨⊂[(-YR)↑⍳⍴⍴R]¨0⍴⊂R";

static LPAPLCHAR MonBody[] =
{MonLine1,
 MonLine2,
 MonLine3,
 MonLine4,
 MonLine5,
 MonLine6,
};

MAGIC_FCNOPR MFO_MonRank =
{MonHeader,
 MonBody,
 countof (MonBody),
};


//***************************************************************************
//  Magic function/operator for the Conform step for monadic/dyadic derived
//    function from the rank dyadic operator
//
//  This function is based upon code found in the paper by J. Philip Benkhard,
//    "Extending structure, type, and expression in APL2", ACM SIGAPL APL Quote Quad,
//    v.21 n.4, p.20-38, Aug. 1991.
//***************************************************************************

static APLCHAR ConHeader[] =
  L"Z←L " MFON_Conform L" R";

static APLCHAR ConLine1[] = 
  L"Z←(((L-∊⍴∘⍴¨R)⍴¨1),¨⍴¨R)⍴¨R";

static LPAPLCHAR ConBody[] =
{ConLine1,
};

MAGIC_FCNOPR MFO_Conform =
{ConHeader,
 ConBody,
 countof (ConBody),
};


//***************************************************************************
//  Magic function/operator for dyadic derived function from the rank dyadic operator
//
//  This operator was originally based upon code found in the paper by J. Philip Benkhard,
//    "Extending structure, type, and expression in APL2", ACM SIGAPL APL Quote Quad,
//    v.21 n.4, p.20-38, Aug. 1991, but later was modified to reflect the
//    definition of the Rank operator in the ISO-IEC 13751 Extended APL Standard.
//***************************************************************************

static APLCHAR DydHeader[] =
  L"Z←L (LO " MFON_DydRank L" Y) R;YL;YR";

static APLCHAR DydLine1[] = 
  L"(YL YR)←(1↓⌽3⍴⌽Y)⌊(⍴⍴L),⍴⍴R";

static APLCHAR DydLine2[] = 
  L":if 0>YL ⋄ YL←0⌈YL+⍴⍴L ⋄ :end";

static APLCHAR DydLine3[] = 
  L":if 0>YR ⋄ YR←0⌈YR+⍴⍴R ⋄ :end";

static APLCHAR DydLine4[] = 
  L"Z←(⊂[(-YL)↑⍳⍴⍴L] L) LO¨⊂[(-YR)↑⍳⍴⍴R] R ⋄ →0";

static APLCHAR DydLine5[] = 
  L"⎕PRO:(YL YR)←(1↓⌽3⍴⌽Y)⌊(⍴⍴L),⍴⍴R";

static APLCHAR DydLine6[] = 
  L":if 0>YL ⋄ YL←0⌈YL+⍴⍴L ⋄ :end";

static APLCHAR DydLine7[] = 
  L":if 0>YR ⋄ YR←0⌈YR+⍴⍴R ⋄ :end";

static APLCHAR DydLine8[] = 
  L"Z←⊃(⊂[(-YL)↑⍳⍴⍴L]¨0⍴⊂L) LO¨¨⊂[(-YR)↑⍳⍴⍴R]¨0⍴⊂R";

static LPAPLCHAR DydBody[] =
{DydLine1,
 DydLine2,
 DydLine3,
 DydLine4,
 DydLine5,
 DydLine6,
 DydLine7,
 DydLine8,
};

MAGIC_FCNOPR MFO_DydRank =
{DydHeader,
 DydBody,
 countof (DydBody),
};


//***************************************************************************
//  End of File: mf_dijot.h
//***************************************************************************