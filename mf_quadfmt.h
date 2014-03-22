//***************************************************************************
//  NARS2000 -- Magic Function -- Multiset
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
//  Magic function/operator for Monadic []FMT
//
//  Monadic []FMT -- Boxed Display of Arrays
//
//  Return a boxed representation of an array.
//***************************************************************************

static APLCHAR MonHeaderFMT[] =
  L"Z←" MFON_MonFMT L" R;⎕IO;Type;Lft;Top;Proto;A;⎕FEATURE";

static APLCHAR MonLineFMT1[] = 
  L"⎕IO←0 ⋄ ⎕FEATURE[0]←1";

static APLCHAR MonLineFMT2[] = 
  L":if 1<≡R";

static APLCHAR MonLineFMT3[] = 
  L"  Type←'∊'";

static APLCHAR MonLineFMT4[] = 
  L":else";

static APLCHAR MonLineFMT5[] = 
  L"  :if 0∊⍴R";

static APLCHAR MonLineFMT6[] = 
  L"    Proto←↑R";

static APLCHAR MonLineFMT7[] = 
  L"  :else";

static APLCHAR MonLineFMT8[] = 
  L"    Proto←R";

static APLCHAR MonLineFMT9[] = 
  L"  :endif";

static APLCHAR MonLineFMT10[] =
  L"  Type←(0 ' '∊⊤Proto)/'~─¯'[0,1+0∊⍴R]";

static APLCHAR MonLineFMT11[] =
  L"  :select ×/⍴Type";

static APLCHAR MonLineFMT12[] =
  L"  :case 1";

static APLCHAR MonLineFMT13[] =
  L"    Type←⍬⍴Type";

static APLCHAR MonLineFMT14[] =
  L"  :case 2";

static APLCHAR MonLineFMT15[] =
  L"    Type←'+'";

static APLCHAR MonLineFMT16[] =
  L"  :endselect";

static APLCHAR MonLineFMT17[] =
  L":endif";

static APLCHAR MonLineFMT18[] =
  L"Proto←0";

static APLCHAR MonLineFMT19[] =
  L":if 1≥≡Z←1/R";

static APLCHAR MonLineFMT20[] =
  L"  :if 0=⍴⍴R";

static APLCHAR MonLineFMT21[] =
  L"    Z←⍕Z ⋄ Z←(1,⍴Z)⍴Z";

static APLCHAR MonLineFMT22[] =
  L"    :if Type='─'";

static APLCHAR MonLineFMT23[] =
  L"      Z←Z⍪'¯'";

static APLCHAR MonLineFMT24[] =
  L"    :elseif Type='~'";

static APLCHAR MonLineFMT25[] =
  L"      Z←Z⍪'~'";

static APLCHAR MonLineFMT26[] =
  L"    :endif";

static APLCHAR MonLineFMT27[] =
  L"    :return";

static APLCHAR MonLineFMT28[] =
  L"  :endif";

static APLCHAR MonLineFMT29[] =
  L"  Z←((-2⌈⍴⍴Z)↑1,⍴Z)⍴Z←⍕Z";

static APLCHAR MonLineFMT30[] =
  L"  :if 0∊⍴R";

static APLCHAR MonLineFMT31[] =
  L"    Z←1 1⍴' 0'[Type='~']";

static APLCHAR MonLineFMT32[] =
  L"  :endif";

static APLCHAR MonLineFMT33[] =
  L"  Z←Z " MFON_BoxFMT L" R";

static APLCHAR MonLineFMT34[] =
  L":else";

static APLCHAR MonLineFMT35[] =
  L"  :if 0∊⍴R";

static APLCHAR MonLineFMT36[] =
  L"    Z←" MFON_MonFMT L"↑R";

static APLCHAR MonLineFMT37[] =
  L"    Z←Z " MFON_BoxFMT L" R";

static APLCHAR MonLineFMT38[] =
  L"  :else";

static APLCHAR MonLineFMT39[] =
  L"    Z←''";

static APLCHAR MonLineFMT40[] =
  L"    :forlcl I :in R";

static APLCHAR MonLineFMT41[] =
  L"      A←" MFON_MonFMT L" I";

static APLCHAR MonLineFMT42[] =
  L"      :if 0=≡I";

static APLCHAR MonLineFMT43[] =
  L"        A←' '⍪A";

static APLCHAR MonLineFMT44[] =
  L"      :endif";

static APLCHAR MonLineFMT45[] =
  L"      Z←Z,⊂A";

static APLCHAR MonLineFMT46[] =
  L"    :endforlcl";

static APLCHAR MonLineFMT47[] =
  L"    :if 1=⍴⍴R";

static APLCHAR MonLineFMT48[] =
  L"      Z←0 ¯1↓⊃,/(∊⌈/⍴¨Z)[0]↑[0]¨Z,¨' '";

static APLCHAR MonLineFMT49[] =
  L"    :else";

static APLCHAR MonLineFMT50[] =
  L"      Z←⊃(⍴R)⍴Z";

static APLCHAR MonLineFMT51[] =
  L"    :endif";

static APLCHAR MonLineFMT52[] =
  L"    Z←((-3⌈⍴⍴Z)↑1,⍴Z)⍴Z";

static APLCHAR MonLineFMT53[] =
  L"    Z←(((¯3↓⍴Z),(⍴Z)[¯2],×/(⍴Z)[¯3 ¯1])⍴,[¯2 ¯3] Z) " MFON_BoxFMT L" R";

static APLCHAR MonLineFMT54[] =
  L"  :endif";

static APLCHAR MonLineFMT55[] =
  L":endif";

static LPAPLCHAR MonBodyFMT[] =
{MonLineFMT1,
 MonLineFMT2,
 MonLineFMT3,
 MonLineFMT4,
 MonLineFMT5,
 MonLineFMT6,
 MonLineFMT7,
 MonLineFMT8,
 MonLineFMT9,
 MonLineFMT10,
 MonLineFMT11,
 MonLineFMT12,
 MonLineFMT13,
 MonLineFMT14,
 MonLineFMT15,
 MonLineFMT16,
 MonLineFMT17,
 MonLineFMT18,
 MonLineFMT19,
 MonLineFMT20,
 MonLineFMT21,
 MonLineFMT22,
 MonLineFMT23,
 MonLineFMT24,
 MonLineFMT25,
 MonLineFMT26,
 MonLineFMT27,
 MonLineFMT28,
 MonLineFMT29,
 MonLineFMT30,
 MonLineFMT31,
 MonLineFMT32,
 MonLineFMT33,
 MonLineFMT34,
 MonLineFMT35,
 MonLineFMT36,
 MonLineFMT37,
 MonLineFMT38,
 MonLineFMT39,
 MonLineFMT40,
 MonLineFMT41,
 MonLineFMT42,
 MonLineFMT43,
 MonLineFMT44,
 MonLineFMT45,
 MonLineFMT46,
 MonLineFMT47,
 MonLineFMT48,
 MonLineFMT49,
 MonLineFMT50,
 MonLineFMT51,
 MonLineFMT52,
 MonLineFMT53,
 MonLineFMT54,
 MonLineFMT55,
};

MAGIC_FCNOPR MFO_MonFMT =
{MonHeaderFMT,
 MonBodyFMT,
 countof (MonBodyFMT),
};


//***************************************************************************
//  Magic function/operator for Box FMT
//
//  Box function -- Boxed Display For Arrays
//
//  Place line drawing chars around an array.
//***************************************************************************

static APLCHAR MonHeaderBoxFMT[] =
  L"Z←Z " MFON_BoxFMT L" R;RD;LD;D";

static APLCHAR MonLineBoxFMT1[] = 
  L"LD←⍕(0≠⍴⍴R)/¯1↑⍴R";

static APLCHAR MonLineBoxFMT2[] = 
  L"RD←⍕⍪¯1↓⍴R";

static APLCHAR MonLineBoxFMT3[] = 
  L"RD←(+/RD=' ')⌽RD";

static APLCHAR MonLineBoxFMT4[] = 
  L"D←¯2+⍴⍴Z";

static APLCHAR MonLineBoxFMT5[] = 
  L":if D>0";

static APLCHAR MonLineBoxFMT6[] = 
  L":while 2<⍴⍴Z";

static APLCHAR MonLineBoxFMT7[] = 
  L"  Z←,[¯3 ¯2] Z,[¯2]' '";

static APLCHAR MonLineBoxFMT8[] = 
  L":endwhile";

static APLCHAR MonLineBoxFMT9[] = 
  L"Z←(-D)↓[0]Z";

static APLCHAR MonLineBoxFMT10[] =
  L":endif";

static APLCHAR MonLineBoxFMT11[] =
  L"Z←(¯2↑1,⍴Z)⍴Z ⋄ Z←((⍴Z)⌈(¯1↑⍴RD),⍴LD)↑Z";

static APLCHAR MonLineBoxFMT12[] =
  L":if 1≥⍴⍴R";

static APLCHAR MonLineBoxFMT13[] =
  L"  RD←((1↑⍴Z),1)⍴'│'";

static APLCHAR MonLineBoxFMT14[] =
  L":else";

static APLCHAR MonLineBoxFMT15[] =
  L"  RD←⍉(1↑⍴Z)↑[1] RD ⋄ RD[(,RD=' ')/,⍳⍴RD]←'│'";

static APLCHAR MonLineBoxFMT16[] =
  L":endif";

static APLCHAR MonLineBoxFMT17[] =
  L"RD←'┬'⍪RD⍪'┴' ⋄ RD[0 ¯1;0]←'┌└'";

static APLCHAR MonLineBoxFMT18[] =
  L"Z←Z,'│'";

static APLCHAR MonLineBoxFMT19[] =
  L"D←≡R";

static APLCHAR MonLineBoxFMT20[] =
  L":if 1<D";

static APLCHAR MonLineBoxFMT21[] =
  L"  D←⍕D";

static APLCHAR MonLineBoxFMT22[] =
  L"  Z[⍳-⍴D;¯1]←⌽D";

static APLCHAR MonLineBoxFMT23[] =
  L":endif";

static APLCHAR MonLineBoxFMT24[] =
  L"Z←'─'⍪Z⍪'─'";

static APLCHAR MonLineBoxFMT25[] =
  L"Z[0 ¯1;¯1]←'┐┘'";

static APLCHAR MonLineBoxFMT26[] =
  L"Z[0;⍳⍴LD]←LD";

static APLCHAR MonLineBoxFMT27[] =
  L"Z[¯1;0]←Type";

static APLCHAR MonLineBoxFMT28[] =
  L"Z←RD,Z";

static LPAPLCHAR MonBodyBoxFMT[] =
{MonLineBoxFMT1,
 MonLineBoxFMT2,
 MonLineBoxFMT3,
 MonLineBoxFMT4,
 MonLineBoxFMT5,
 MonLineBoxFMT6,
 MonLineBoxFMT7,
 MonLineBoxFMT8,
 MonLineBoxFMT9,
 MonLineBoxFMT10,
 MonLineBoxFMT11,
 MonLineBoxFMT12,
 MonLineBoxFMT13,
 MonLineBoxFMT14,
 MonLineBoxFMT15,
 MonLineBoxFMT16,
 MonLineBoxFMT17,
 MonLineBoxFMT18,
 MonLineBoxFMT19,
 MonLineBoxFMT20,
 MonLineBoxFMT21,
 MonLineBoxFMT22,
 MonLineBoxFMT23,
 MonLineBoxFMT24,
 MonLineBoxFMT25,
 MonLineBoxFMT26,
 MonLineBoxFMT27,
 MonLineBoxFMT28,
};

MAGIC_FCNOPR MFO_BoxFMT =
{MonHeaderBoxFMT,
 MonBodyBoxFMT,
 countof (MonBodyBoxFMT),
};


//***************************************************************************
//  End of File: mf_multiset.h
//***************************************************************************