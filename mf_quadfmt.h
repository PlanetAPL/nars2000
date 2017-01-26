//***************************************************************************
//  NARS2000 -- Magic Function -- Monadic ⎕FMT
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

// ⍪(⊂'L"'),¨(1 ⎕cr '#...'),¨⊂'",'
// ⍪2⌽¨(⊂'",L"'),¨1 ⎕cr '#...'

//***************************************************************************
//  Magic function/operator for Monadic []FMT
//
//  Monadic []FMT -- Boxed Display of Arrays
//
//  Return a boxed representation of an array.
//***************************************************************************

static LPAPLCHAR MonBodyFMT[] =
{L"⎕IO←0 ⋄ ⎕FEATURE[0]←1",
 L":if 1<≡R",
 L"  Type←'∊'",
 L":else",
 L"  :if 0∊⍴R",
 L"    Proto←↑R",
 L"  :else",
 L"    Proto←R",
 L"  :endif",
 L"  Type←(0 ' '∊⊤Proto)/'~─¯'[0,1+0∊⍴R]",
 L"  :select ×/⍴Type",
 L"  :case 1",
 L"    Type←⍬⍴Type",
 L"  :case 2",
 L"    Type←'+'",
 L"  :endselect",
 L":endif",
 L"Proto←0",
 L":if 1≥≡Z←1/R",
 L"  :if 0=⍴⍴R",
 L"    Z←⍕Z ⋄ Z←(1,⍴Z)⍴Z",
 L"    :if Type='─'",
 L"      Z←Z⍪'¯'",
 L"    :elseif Type='~'",
 L"      Z←Z⍪'~'",
 L"    :endif",
 L"    :return",
 L"  :endif",
 L"  Z←((-2⌈⍴⍴Z)↑1,⍴Z)⍴Z←' ',⍕Z",
 L"  :if 0∊⍴R",
 L"    :if Type='~'",
 L"      Z←1 2⍴' 0'",
 L"    :else",
 L"      Z←1 1⍴' '",
 L"    :endif",
 L"  :endif",
 L"  Z←Z " MFON_BoxFMT L" R",
 L":else",
 L"  :if 0∊⍴R",
 L"    Z←" MFON_MonFMT L"↑R",
 L"    Z←Z " MFON_BoxFMT L" R",
 L"  :else",
 L"    Z←''",
 L"    :forlcl I :in R",
 L"      A←" MFON_MonFMT L" I",
 L"      :if 0=≡I",
 L"        A←' '⍪A",
 L"      :endif",
 L"      Z←Z,⊂A",
 L"    :endforlcl",
 L"    :if 1=⍴⍴R",
 L"      Z←0 ¯1↓⊃,/(∊⌈/⍴¨Z)[0]↑[0]¨Z,¨' '",
 L"    :else",
 L"      Z←⊃(⍴R)⍴Z",
 L"    :endif",
 L"    Z←((-3⌈⍴⍴Z)↑1,⍴Z)⍴Z",
 L"    Z←(((¯3↓⍴Z),(⍴Z)[¯2],×/(⍴Z)[¯3 ¯1])⍴,[¯2 ¯3] Z) " MFON_BoxFMT L" R",
 L"  :endif",                                                     
 L":endif",
};

MAGIC_FCNOPR MFO_MonFMT =
{L"Z←" MFON_MonFMT L" R;⎕IO;Type;Lft;Top;Proto;A;⎕FEATURE",
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

static LPAPLCHAR MonBodyBoxFMT[] =
{L"LD←⍕(0≠⍴⍴R)/¯1↑⍴R",                             
 L"RD←⍕⍪¯1↓⍴R",                                    
 L"RD←(+/RD=' ')⌽RD",                              
 L"D←¯2+⍴⍴Z",                                      
 L":if D>0",                                       
 L":while 2<⍴⍴Z",                                  
 L"  Z←,[¯3 ¯2] Z,[¯2]' '",                        
 L":endwhile",                                     
 L"Z←(-D)↓[0]Z",                                   
 L":endif",                                        
 L"Z←(¯2↑1,⍴Z)⍴Z ⋄ Z←((⍴Z)⌈(¯1↑⍴RD),⍴LD)↑Z",       
 L":if 1≥⍴⍴R",                                     
 L"  RD←((1↑⍴Z),1)⍴'│'",                           
 L":else",                                         
 L"  RD←⍉(1↑⍴Z)↑[1] RD ⋄ RD[(,RD=' ')/,⍳⍴RD]←'│'", 
 L":endif",                                        
 L"RD←'┬'⍪RD⍪'┴' ⋄ RD[0 ¯1;0]←'┌└'",               
 L"Z←Z,'│'",                                       
 L"D←≡R",                                          
 L":if 1<D",                                       
 L"  D←⍕D",                                        
 L"  Z[⍳-⍴D;¯1]←⌽D",                               
 L":endif",                                        
 L"Z←'─'⍪Z⍪'─'",                                   
 L"Z[0 ¯1;¯1]←'┐┘'",                               
 L"Z[0;⍳⍴LD]←LD",                                  
 L"Z[¯1;0]←Type",                                  
 L"Z←RD,Z",    
};

MAGIC_FCNOPR MFO_BoxFMT =
{L"Z←Z " MFON_BoxFMT L" R;RD;LD;D",
 MonBodyBoxFMT,
 countof (MonBodyBoxFMT),
};


//***************************************************************************
//  End of File: mf_quadfmt.h
//***************************************************************************