//***************************************************************************
//  NARS2000 -- Edit Ctrl Header
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2010 Sudley Place Software

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

#define EM_SETPAINTHOOK     0x00BF

#define GWLEC_ES            0
#define GWLEC_PAINTHOOK     GWLEC_ES        + 1 * sizeof (HANDLE_PTR)   // Ptr to LclECPaintHook proc
#define GWLEC_HBITMAP       GWLEC_PAINTHOOK + 1 * sizeof (HANDLE_PTR)   // Handle of the matching caret bitmap
#define GWLEC_VKSTATE       GWLEC_HBITMAP   + 1 * sizeof (HANDLE_PTR)   // Virtal Key state (VKSTATE struc)
#define GWLEC_EXTRA         GWLEC_VKSTATE   + 1 * sizeof (long)         // Total # extra bytes


typedef int PAINTHOOK (HWND, HDC, int, int, LPWSTR, int, int, long, int, int, UBOOL);
typedef PAINTHOOK *LPPAINTHOOK;

#define MYWM_REDO           (WM_APP + 101)
#define MYWM_SELECTALL      (WM_APP + 102)
#define MYWM_KEYDOWN        (WM_APP + 103)
#define MYWM_PASTE_APL      (WM_APP + 104)
#define MYWM_COPY_APL       (WM_APP + 105)
#define MYWM_LOCALIZE       (WM_APP + 106)
#define MYWM_UNLOCALIZE     (WM_APP + 107)
#define MYWM_LINE_HEIGHT    (WM_APP + 108)

#define FCN_INDENT  6

#define WM_REDO     (WM_USER + 0)

#define GWL_ID              (-12)

typedef struct tagNMEDITCTRL
{
    NMHDR   nmHdr;          // 00:  Initial struc
    UINT    cbSize;         // 0C:  Byte size of NMCARETWIDTH struc
    HBITMAP hBitMap;        // 10:  Bitmap handle for replace mode (NULL = insert mode)
                            // 14:  Length
} NMEDITCTRL, *LPNMEDITCTRL;


//***************************************************************************
//  End of File: editctrl.h
//***************************************************************************
