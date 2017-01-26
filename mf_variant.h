//***************************************************************************
//  NARS2000 -- Magic Function -- Variant Operator
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
//  Magic function/operator for rising/falling factorials
//***************************************************************************

static LPAPLCHAR DydVOFactBody[] =
{L"⎕PRO:",
 L"⎕IO←0",
 L"→(2=≢X)/L1 ⋄ X←X,1",
 L"L1:Z←×/¨R+((×X[0])×|X[1])×⊂⍳|X[0]",
};

MAGIC_FCNOPR MFO_DydVOFact =
{L"Z←" MFON_DydVOFact L"[X] R;⎕IO",
 DydVOFactBody,
 countof (DydVOFactBody),
};


static LPAPLCHAR MonExecuteBody[] =
{L"⎕PRO:",
 L"⎕FPC←L",
 L"Z←⍎R",
};

MAGIC_FCNOPR MFO_MonExecute =
{L"Z←L " MFON_MonExecute L" R;⎕FPC",
 MonExecuteBody,
 countof (MonExecuteBody),
};


//***************************************************************************
//  End of File: mf_variant.h
//***************************************************************************