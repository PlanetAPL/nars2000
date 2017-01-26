//***************************************************************************
//  NARS2000 -- Magic Function -- Multiset
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
//  Magic function for Multiset Asymmetric Difference
//
//  Dyadic Tilde -- Multiset Asymmetric Difference
//
//  On scalar or vector args, return the elements in L~R.
//***************************************************************************

static APLCHAR DydHeaderMAD[] =
  L"Z←L " MFON_MAD L" R";

static APLCHAR DydLineMAD1[] = 
  L"⎕MS:Z←(~L∊⍦R)/L ⋄ →0";

static APLCHAR DydLineMAD2[] = 
  L"⎕PRO:Z←⊤(⊤L) " MFON_MAD L" ⊤R";

static LPAPLCHAR DydBodyMAD[] =
{DydLineMAD1,
 DydLineMAD2,
};

MAGIC_FCNOPR MFO_MAD =
{DydHeaderMAD,
 DydBodyMAD,
 countof (DydBodyMAD),
};

//***************************************************************************
//  Magic function for Multiset Symmetric Difference
//
//  Dyadic Section -- Multiset Symmetric Difference
//
//  On scalar or vector args, return the elements in (L~$R),R~$L.
//***************************************************************************

static APLCHAR DydHeaderMSD[] =
  L"Z←L " MFON_MSD L" R";

static APLCHAR DydLineMSD1[] = 
  L"⎕MS:Z←(L " MFON_MAD L" R),R " MFON_MAD L" L ⋄ →0";

static APLCHAR DydLineMSD2[] = 
  L"⎕PRO:Z←⊤(⊤L) " MFON_MSD L" ⊤R";

static LPAPLCHAR DydBodyMSD[] =
{DydLineMSD1,
 DydLineMSD2,
};

MAGIC_FCNOPR MFO_MSD =
{DydHeaderMSD,
 DydBodyMSD,
 countof (DydBodyMSD),
};

//***************************************************************************
//  Magic function for Multiset Union
//
//  Dyadic DownShoe -- Multiset Union
//
//  On scalar or vector args, return the multiset union of the two args
//***************************************************************************

static APLCHAR DydHeaderMU[] =
  L"Z←L " MFON_MU L" R";

static APLCHAR DydLineMU1[] = 
  L"⎕MS:Z←L,R " MFON_MAD L" L ⋄ →0";

static APLCHAR DydLineMU2[] = 
  L"⎕PRO:Z←⊤(⊤L) " MFON_MU L" ⊤R";

static LPAPLCHAR DydBodyMU[] =
{DydLineMU1,
 DydLineMU2,
};

MAGIC_FCNOPR MFO_MU =
{DydHeaderMU,
 DydBodyMU,
 countof (DydBodyMU),
};

//***************************************************************************
//  Magic function for Multiset Intersection
//
//  Dyadic UpShoe -- Intersection
//
//  On scalar or vector args, return the multiset intersection of the two args
//***************************************************************************

static APLCHAR DydHeaderMI[] =
  L"Z←L " MFON_MI L" R";

static APLCHAR DydLineMI1[] = 
  L"⎕MS:Z←(L∊⍦R)/L ⋄ →0";

static APLCHAR DydLineMI2[] = 
  L"⎕PRO:Z←⊤(⊤L) " MFON_MI L" ⊤R";

static LPAPLCHAR DydBodyMI[] =
{DydLineMI1,
 DydLineMI2,
};

MAGIC_FCNOPR MFO_MI =
{DydHeaderMI,
 DydBodyMI,
 countof (DydBodyMI),
};

//***************************************************************************
//  Magic function for Multiset Proper Subset
//
//  Dyadic LeftShoe -- Multiset Proper Subset
//
//  On scalar or vector args, return TRUE iff L is a proper multisubset of R.
//***************************************************************************

static APLCHAR DydHeaderMLRS[] =
  L"Z←L " MFON_MLRS L" R";

static APLCHAR DydLineMLRS1[] = 
  L"⎕MS:Z←(L " MFON_MLRSU L" R)^L≢⍦R ⋄ →0";

static APLCHAR DydLineMLRS2[] = 
  L"⎕PRO:Z←⊤(⊤L) " MFON_MLRS L" ⊤R";

static LPAPLCHAR DydBodyMLRS[] =
{DydLineMLRS1,
 DydLineMLRS2,
};

MAGIC_FCNOPR MFO_MLRS =
{DydHeaderMLRS,
 DydBodyMLRS,
 countof (DydBodyMLRS),
};

//***************************************************************************
//  Magic function for Multiset Subset
//
//  Dyadic LeftShoeUnderbar -- Multiset Subset
//
//  On scalar or vector args, return TRUE iff L is a multisubset of R.
//***************************************************************************

static APLCHAR DydHeaderMLRSU[] =
  L"Z←L " MFON_MLRSU L" R";

static APLCHAR DydLineMLRSU1[] = 
  L"⎕MS:Z←~0∊L " MFON_MEO L" R ⋄ →0";

static APLCHAR DydLineMLRSU2[] = 
  L"⎕PRO:Z←⊤(⊤L) " MFON_MLRSU L" ⊤R";

static LPAPLCHAR DydBodyMLRSU[] =
{DydLineMLRSU1,
 DydLineMLRSU2,
};

MAGIC_FCNOPR MFO_MLRSU =
{DydHeaderMLRSU,
 DydBodyMLRSU,
 countof (DydBodyMLRSU),
};

//***************************************************************************
//  Magic function for Multiset Index Of
//
//  Dyadic Iota -- Multiset Index Of
//
//  On scalar or vector args, return the elements in L⍳⍦R.
//***************************************************************************

static APLCHAR DydHeaderMIO[] =
  L"Z←L " MFON_MIO L" R";

static APLCHAR DydLineMIO1[] = 
  L"⎕MS:L←1/L⋄Z←((⍴L)⍴⍋⍋L⍳L,R)⍳(⍴R)⍴⍋⍋L⍳R,L ⋄ →0";

static APLCHAR DydLineMIO2[] = 
  L"⎕PRO:Z←⊤(⊤L) " MFON_MIO L" ⊤R";
  
static LPAPLCHAR DydBodyMIO[] =
{DydLineMIO1,
 DydLineMIO2,
};

MAGIC_FCNOPR MFO_MIO =
{DydHeaderMIO,
 DydBodyMIO,
 countof (DydBodyMIO),
};

//***************************************************************************
//  Magic function for Multiset Element Of
//
//  Dyadic Epsilon -- Multiset Element Of
//
//  On scalar or vector args, return the elements in L∊⍦R.
//***************************************************************************

static APLCHAR DydHeaderMEO[] =
  L"Z←L " MFON_MEO L" R";

static APLCHAR DydLineMEO1[] = 
  L"⎕MS:L←1/L⋄Z←((⍴L)⍴⍋⍋L⍳L,R)∊(⍴R)⍴⍋⍋L⍳R,L ⋄ →0";

static APLCHAR DydLineMEO2[] = 
  L"⎕PRO:Z←⊤(⊤L) " MFON_MEO L" ⊤R";

static LPAPLCHAR DydBodyMEO[] =
{DydLineMEO1,
 DydLineMEO2,
};

MAGIC_FCNOPR MFO_MEO =
{DydHeaderMEO,
 DydBodyMEO,
 countof (DydBodyMEO),
};

//***************************************************************************
//  Magic function for Multiset Match
//
//  Dyadic Match -- Multiset Match
//
//  On scalar or vector args, return the elements in L≡⍦R.
//***************************************************************************

static APLCHAR DydHeaderMM[] =
  L"Z←L " MFON_MM L" R";

static APLCHAR DydLineMM1[] = 
  L"⎕MS:Z←(⍴1/L)≡⍴1/R ⋄ →Z↓0";

static APLCHAR DydLineMM2[] = 
  L"Z←∧/L∊⍦R ⋄ →0";

static APLCHAR DydLineMM3[] = 
  L"⎕PRO:Z←⊤(⊤L) " MFON_MM L" ⊤R";

static LPAPLCHAR DydBodyMM[] =
{DydLineMM1,
 DydLineMM2,
 DydLineMM3,
};

MAGIC_FCNOPR MFO_MM =
{DydHeaderMM,
 DydBodyMM,
 countof (DydBodyMM),
};

//***************************************************************************
//  Magic function for Multiset Multiplicities
//
//  Monadic DownShoe -- Multiset Multiplicities
//
//  On scalar or vector args, return the multiplicities of the unique elements
//    in the same order as the unique elements.
//***************************************************************************

static APLCHAR DydHeaderMMUL[] =
  L"Z←" MFON_MMUL L" R";

static APLCHAR DydLineMMUL1[] = 
  L"⎕MS:Z←¯2-/⍸1,(2≠/R[⍋(1/R)⍳R]),1⋄→0";

static APLCHAR DydLineMMUL2[] = 
  L"⎕PRO:Z←⊤ " MFON_MMUL L" ⊤R";

static LPAPLCHAR DydBodyMMUL[] =
{DydLineMMUL1,
 DydLineMMUL2,
};

MAGIC_FCNOPR MFO_MMUL =
{DydHeaderMMUL,
 DydBodyMMUL,
 countof (DydBodyMMUL),
};


//***************************************************************************
//  Magic function/operator for Multiset Dyadic Iota Underbar
//
//  Multiset Dyadic Iota Underbar -- Matrix Iota
//
//  Return an array of indices from searching for the
//    array  of trailing subarrays from the right arg in the
//    vector of trailing subarrays from the left arg.
//***************************************************************************

static APLCHAR DydHeaderMDIU[] =
  L"Z←L " MFON_MDIU L" R";

static APLCHAR DydLineMDIU1[] = 
  L"⎕MS:Z←(⊂⍤¯1 L)⍳⍦⊂⍤(¯1+⍴⍴L) R ⋄ →0";

static LPAPLCHAR DydBodyMDIU[] =
{DydLineMDIU1,
};

MAGIC_FCNOPR MFO_MDIU =
{DydHeaderMDIU,
 DydBodyMDIU,
 countof (DydBodyMDIU),
};
  

//***************************************************************************
//  End of File: mf_multiset.h
//***************************************************************************