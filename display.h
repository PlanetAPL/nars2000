//***************************************************************************
//  NARS2000 -- Display Header
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

/***************************************************************************

The sequence of structs matches the structure of the array being
displayed/formatted.

The topmost struct is

HEADER

If the data for the first item is simple, it follows in the stream.
Otherwise, FMTHEADER appears and the process nests
Each item is treated the same way as above.

COLSTRS:
      FMTCOLSTR
    | COLSTRS FMTCOLSTR
    ;

HEADER:
      FMTHEADER COLSTRS FMTROWSTR
    | FMTHEADER COLSTRS FMTROWSTR Data
    | FMTHEADER COLSTRS FMTROWSTR HEADER
    ;

***************************************************************************/

typedef struct tagFMTHEADER
{
    struct tagFMTHEADER *lpFmtHdrUp;    // 00:  Ptr to next chain up
    struct tagFMTROWSTR *lpFmtRowUp;    // 04:  Ptr to parent FMTROWSTR
    struct tagFMTCOLSTR *lpFmtColUp;    // 08:  Ptr to parent FMTCOLSTR
    struct tagFMTROWSTR *lpFmtRow1st;   // 0C:  Ptr to 1st child FMTROWSTR
    struct tagFMTROWSTR *lpFmtRowLst;   // 10:  Ptr to last child FMTROWSTR
    struct tagFMTCOLSTR *lpFmtCol1st;   // 14:  Ptr to 1st child FMTCOLSTR
    UINT        uRealRows,              // 18:  # rows in the array's dimensions (top level only)
                uActRows,               // 1C:  # FMTROWSTRs to follow
                uActCols,               // 20:  # FMTCOLSTRs to follow
                uFmtRows,               // 24:  # formatted rows in this block (+/FMTROWSTR.uFmtRows)
                uFmtLdBl,               // 28:  ...         LdBl ...           (+/FMTCOLSTR.uLdBl)
                uFmtIntWid,             // 2C:  ...         interior width
                uFmtTrBl;               // 30:  ...         TrBl ...           (+/FMTCOLSTR.uTrBl)
    UINT        uMatRes:1;              // 34:  80000000:  TRUE iff there is a rank > 1 item contained in this item
                                        // 38:  Length
#ifdef DEBUG
  #define FMTHEADER_SIGNATURE 'HHHH'    // 48484848
      HEADER_SIGNATURE Sig;             // 38:  FMTHEADER signature
                                        // 3C:  Length
#endif
} FMTHEADER, *LPFMTHEADER;

typedef enum tagCOL_TYPES
{
    COLTYPE_UNK = 0,                    // 00:  Type is unspecified
    COLTYPE_ALLCHAR,                    // 01:  Column consists of all character data
    COLTYPE_NOTCHAR,                    // 02:  Column is all numeric or mixed character and numeric
                                        // 03:  Available entry (2 bits)
} COLTYPES, *LPCOLTYPES;

// N.B.  The order of the values in the above enum is important such that
//       COLTYPE_NOTCHAR must have a higher value than COLTYPE_ALLCHAR
//       so we can use the max macro.

typedef struct tagFMTCOLSTR
{
    UINT        uLdBlMax,               // 00:  # Leading blanks due to NOTCHAR+
                uLdBlNst,               // 04:  # Leading blanks due to nesting
                uIntWid,                // 08:  Interior width
                uInts,                  // 0C:  # Integer digits in Boolean/Integer/APA/Float column,
                                        //      including sign
                uChrs,                  // 10:  # CHARs
                uFrcs,                  // 14:  # Fractional digits in Float column
                                        //      including decimal point
                uTrBlNst;               // 18:  # Trailing blanks due to nesting
    COLTYPES    colType;                // 1C:  Column type (see COLTYPES)
    struct tagFMTCOLSTR
               *lpFmtColUp;             // 20:  Ptr to parent FMTCOLSTR
                                        // 24:  Length
#ifdef DEBUG
  #define FMTCOLSTR_SIGNATURE 'CCCC'    // 43434343
      HEADER_SIGNATURE Sig;             // 24:  FMTCOLSTR signature
                                        // 28:  Length
#endif
} FMTCOLSTR, *LPFMTCOLSTR;

typedef struct tagFMTROWSTR
{

    UINT        bRptCol:1,              // 00:  10000000:  TRUE iff we're to repeat a col in this row
                bDone:1,                //      20000000:  TRUE iff this row is done with output
                bRealRow:1,             //      40000000:  TRUE iff a real row (not from []TCLF)
                bBlank:1;               //      80000000:  TRUE iff this row is blank
    UINT        uFmtRows,               // 04:  # formatted rows in this row
                uColOff,                // 08:  Column offset of this row
                uItemCount,             // 0C:  # following items
                uAccWid;                // 10:  Accumulated width for this row
    struct tagFMTROWSTR
               *lpFmtRowNxt;            // 14:  Ptr to next sibling FMTROWSTR
    LPAPLCHAR   lpNxtChar,              // 18:  Ptr to next entry for raw output
                lpEndChar;              // 1C:  ...    byte after last entry ...
                                        // 20:  Length
#ifdef DEBUG
      FMTCOLSTR *lpFmtColUp;            // 24:  Ptr to parent FMTCOLSTR
  #define FMTROWSTR_SIGNATURE 'RRRR'    // 52525252
      HEADER_SIGNATURE Sig;             // 28:  FMTROWSTR signature
                                        // 2C:  Length
#endif
} FMTROWSTR, *LPFMTROWSTR;

// ***FIXME*** -- Is this any better than filling as needed??
#define PREFILL         // With blanks


//***************************************************************************
//  End of File: display.h
//***************************************************************************
