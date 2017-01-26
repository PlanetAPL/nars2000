//***************************************************************************
//  NARS2000 -- My Routines For Debugging
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

#if RESDEBUG
#define STRICT
#include <windows.h>
#include "headers.h"

//#define DEBUG_HEAP
//#define DEBUG_SEMAPHORE
//#define DEBUG_THREAD

#define OBJ_GLBLOCK     15
#define OBJ_GLBALLOC    16
#define OBJ_SEMAPHORE   17
#define OBJ_HEAPALLOC   18


// ************** DEBUGGING DATA ********************************************

UINT
 auLinNumPEN        [MAXOBJ],               //  1
 auLinNumBRUSH      [MAXOBJ],               //  2
 auLinNumDC         [MAXOBJ],               //  3
 auLinNumMETADC     [MAXOBJ],               //  4
 auLinNumPAL        [MAXOBJ],               //  5
 auLinNumFONT       [MAXOBJ],               //  6
 auLinNumBITMAP     [MAXOBJ],               //  7
 auLinNumREGION     [MAXOBJ],               //  8
 auLinNumMETAFILE   [MAXOBJ],               //  9
 auLinNumMEMDC      [MAXOBJ],               // 10
 auLinNumEXTPEN     [MAXOBJ],               // 11
 auLinNumENHMETADC  [MAXOBJ],               // 12
 auLinNumENHMETAFILE[MAXOBJ],               // 13
 auLinNumCOLORSPACE [MAXOBJ],               // 14
 auLinNumGLBLOCK    [MAXOBJ],               // 15
 auLinNumGLBALLOC   [MAXOBJ],               // 16
 auLinNumSEMAPHORE  [MAXOBJ],               // 17
 auLinNumHEAPALLOC  [MAXOBJ];               // 18

UINT *lpaauLinNum[] =
{&auLinNumPEN        [0],                   //  1
 &auLinNumBRUSH      [0],                   //  2
 &auLinNumDC         [0],                   //  3
 &auLinNumMETADC     [0],                   //  4
 &auLinNumPAL        [0],                   //  5
 &auLinNumFONT       [0],                   //  6
 &auLinNumBITMAP     [0],                   //  7
 &auLinNumREGION     [0],                   //  8
 &auLinNumMETAFILE   [0],                   //  9
 &auLinNumMEMDC      [0],                   // 10
 &auLinNumEXTPEN     [0],                   // 11
 &auLinNumENHMETADC  [0],                   // 12
 &auLinNumENHMETAFILE[0],                   // 13
 &auLinNumCOLORSPACE [0],                   // 14
 &auLinNumGLBLOCK    [0],                   // 15
 &auLinNumGLBALLOC   [0],                   // 16
 &auLinNumSEMAPHORE  [0],                   // 17
 &auLinNumHEAPALLOC  [0],                   // 18
};

int
 iCountPEN         = 0,                     //  1
 iCountBRUSH       = 0,                     //  2
 iCountDC          = 0,                     //  3
 iCountMETADC      = 0,                     //  4
 iCountPAL         = 0,                     //  5
 iCountFONT        = 0,                     //  6
 iCountBITMAP      = 0,                     //  7
 iCountREGION      = 0,                     //  8
 iCountMETAFILE    = 0,                     //  9
 iCountMEMDC       = 0,                     // 10
 iCountEXTPEN      = 0,                     // 11
 iCountENHMETADC   = 0,                     // 12
 iCountENHMETAFILE = 0,                     // 13
 iCountCOLORSPACE  = 0,                     // 14
 iCountGLBLOCK     = 0,                     // 15
 iCountGLBALLOC    = 0,                     // 16
 iCountSEMAPHORE   = 0,                     // 17
 iCountHEAPALLOC   = 0;                     // 18

int *lpiaCount[] =
{&iCountPEN        ,                        //  1
 &iCountBRUSH      ,                        //  2
 &iCountDC         ,                        //  3
 &iCountMETADC     ,                        //  4
 &iCountPAL        ,                        //  5
 &iCountFONT       ,                        //  6
 &iCountBITMAP     ,                        //  7
 &iCountREGION     ,                        //  8
 &iCountMETAFILE   ,                        //  9
 &iCountMEMDC      ,                        // 10
 &iCountEXTPEN     ,                        // 11
 &iCountENHMETADC  ,                        // 12
 &iCountENHMETAFILE,                        // 13
 &iCountCOLORSPACE ,                        // 14
 &iCountGLBLOCK    ,                        // 15
 &iCountGLBALLOC   ,                        // 16
 &iCountSEMAPHORE  ,                        // 17
 &iCountHEAPALLOC  ,                        // 18
};

HANDLE
 ahPEN         [MAXOBJ],                    //  1
 ahBRUSH       [MAXOBJ],                    //  2
 ahDC          [MAXOBJ],                    //  3
 ahMETADC      [MAXOBJ],                    //  4
 ahPAL         [MAXOBJ],                    //  5
 ahFONT        [MAXOBJ],                    //  6
 ahBITMAP      [MAXOBJ],                    //  7
 ahREGION      [MAXOBJ],                    //  8
 ahMETAFILE    [MAXOBJ],                    //  9
 ahMEMDC       [MAXOBJ],                    // 10
 ahEXTPEN      [MAXOBJ],                    // 11
 ahENHMETADC   [MAXOBJ],                    // 12
 ahENHMETAFILE [MAXOBJ],                    // 13
 ahCOLORSPACE  [MAXOBJ],                    // 14
 ahGLBLOCK     [MAXOBJ],                    // 15
 ahGLBALLOC    [MAXOBJ],                    // 16
 ahSEMAPHORE   [MAXOBJ],                    // 17
 ahHEAPALLOC   [MAXOBJ];                    // 18

HANDLE *lpaah[] =
{&ahPEN        [0],                         //  1
 &ahBRUSH      [0],                         //  2
 &ahDC         [0],                         //  3
 &ahMETADC     [0],                         //  4
 &ahPAL        [0],                         //  5
 &ahFONT       [0],                         //  6
 &ahBITMAP     [0],                         //  7
 &ahREGION     [0],                         //  8
 &ahMETAFILE   [0],                         //  9
 &ahMEMDC      [0],                         // 10
 &ahEXTPEN     [0],                         // 11
 &ahENHMETADC  [0],                         // 12
 &ahENHMETAFILE[0],                         // 13
 &ahCOLORSPACE [0],                         // 14
 &ahGLBLOCK    [0],                         // 15
 &ahGLBALLOC   [0],                         // 16
 &ahSEMAPHORE  [0],                         // 17
 &ahHEAPALLOC  [0],                         // 18
};

LPCHAR
 lpaFileNameGLBLOCK     [MAXOBJ],           // 15
 lpaFileNameGLBALLOC    [MAXOBJ],           // 16
 lpaFileNameSEMAPHORE   [MAXOBJ];           // 17

LPCHAR *lpaaFileName[] =
{
 NULL,                                      //  1
 NULL,                                      //  2
 NULL,                                      //  3
 NULL,                                      //  4
 NULL,                                      //  5
 NULL,                                      //  6
 NULL,                                      //  7
 NULL,                                      //  8
 NULL,                                      //  9
 NULL,                                      // 10
 NULL,                                      // 11
 NULL,                                      // 12
 NULL,                                      // 13
 NULL,                                      // 14
 &lpaFileNameGLBLOCK[0],                    // 15
 &lpaFileNameGLBALLOC[0],                   // 16
 &lpaFileNameSEMAPHORE[0],                  // 17
 NULL,                                      // 18
};


//***************************************************************************
//  $SaveObj
//
//  Save the allocation of an object
//***************************************************************************

void _SaveObj
    (DWORD   dwType,
     HGDIOBJ hObject,
     LPCHAR  lpFileName,    // Ptr to filename (may be NULL)
     UINT    uLine)         // Line #

{
    int *lpiCount;

    if (bCSO)
        EnterCriticalSection (&CSORsrc);

    // Get a ptr to the counter
    lpiCount = lpiaCount[dwType - 1];

    if (*lpiCount < (MAXOBJ - 1))
    {
        // Save the line number
        lpaauLinNum[dwType - 1][*lpiCount] = uLine;

        // Count in another OBJECT
        lpaah[dwType - 1][*lpiCount] = hObject;

        // Save the filename (if any)
        if (lpaaFileName[dwType - 1])
            lpaaFileName[dwType - 1][*lpiCount] = lpFileName;

        // Increment the counter
        (*lpiCount)++;
    } else
        DbgBrk ();              // #ifdef DEBUG

    if (bCSO)
        LeaveCriticalSection (&CSORsrc);
} // _SaveObj


//***************************************************************************
//  $DeleObj
//
//  Delete the allocation of an object
//***************************************************************************

void _DeleObj
    (DWORD   dwType,
     HGDIOBJ hObject)

{
    int     i, iLen;
    int    *lpiCount;
    HANDLE *lpah;

    if (bCSO)
        EnterCriticalSection (&CSORsrc);

    // Get a ptr to the counter
    lpiCount = lpiaCount[dwType - 1];

    // Get a ptr to the handle array
    lpah = lpaah[dwType - 1];

    // Find this object in the array
    //   looking backwards
    iLen = *lpiCount;
    for (i = iLen - 1; i >= 0; i--)
    if (hObject EQ lpah[i])
        break;

    // If we didn't find it, ...
    if (i < 0)
        DbgBrk ();              // #ifdef DEBUG
    else
    {
        // Move down the saved values above this entry
        OverLapMemory (&lpaauLinNum[dwType - 1][i],
                       &lpaauLinNum[dwType - 1][i + 1],
                        (iLen - i) * sizeof (lpaauLinNum[0][0]));
        OverLapMemory (&lpah[i],
                       &lpah[i + 1],
                        (iLen - i) * sizeof (lpah[0]));
        if (lpaaFileName[dwType - 1])
            OverLapMemory (&lpaaFileName[dwType - 1][i],
                           &lpaaFileName[dwType - 1][i + 1],
                            (iLen - i) * sizeof (lpaaFileName[0][0]));
    } // End IF/ELSE

    if (*lpiCount < MAXOBJ)
    {
        // Count out another OBJECT
        if (*lpiCount)
            (*lpiCount)--;
        else
            DbgBrk ();              // #ifdef DEBUG
    } else
        DbgBrk ();              // #ifdef DEBUG

    if (bCSO)
        LeaveCriticalSection (&CSORsrc);
} // _DeleObj


//***************************************************************************
//  $OverLapMemory
//
//  Local version of MoveMemory for overalpping memory where
//    we mean to destroy the overlapping part
//***************************************************************************

void OverLapMemory
    (LPVOID lpDst,          // Ptr to destin
     LPVOID lpSrc,          // Ptr to source
     UINT   uLen)           // # bytes to move

{
    while (uLen--)
        *((LPBYTE) lpDst)++ = *((LPBYTE) lpSrc)++;
} // End OverLapMemory


//***************************************************************************
//  $LastTouch
//
//  Format a message about the last place a global memory handle was locked
//    and allocated.
//***************************************************************************

void LastTouch
    (char   *szTemp,
     HGLOBAL hMem,
     UBOOL   bLocked)               // TRUE iff the hMem is currently locked

{
    int i, j,
        iLen;

    // If it's currently locked, ...
    if (bLocked)
    {
        // Find this object in the array
        iLen = *lpiaCount[OBJ_GLBLOCK - 1];

        // Note we loop backwards to find the most recent entry
        for (i = iLen - 1; i >= 0; i--)
        if (hMem EQ lpaah[OBJ_GLBLOCK - 1][i])
            break;
    } // End IF

    // Find this object in the array
    iLen = *lpiaCount[OBJ_GLBALLOC - 1];

    // Note we loop backwards to find the most recent entry
    for (j = iLen - 1; j >= 0; j--)
    if (hMem EQ lpaah[OBJ_GLBALLOC - 1][j])
        break;

    // If we didn't find it, ...
    if (i < 0 || j < 0)
        DbgBrk ();              // #ifdef DEBUG
    else
    // If it's currently locked, ...
    if (bLocked)
        MySprintf (szTemp,
                   sizeof (szTemp),
                   "The global (%p) was last locked in (%s#%d) and allocated in (%s#%d).",
                   hMem,
                   lpaaFileName[OBJ_GLBLOCK  - 1][i],
                   lpaauLinNum [OBJ_GLBLOCK  - 1][i],
                   lpaaFileName[OBJ_GLBALLOC - 1][j],
                   lpaauLinNum [OBJ_GLBALLOC - 1][j]);
    else
        MySprintf (szTemp,
                   sizeof (szTemp),
                   "The global (%p) was last allocated in (%s#%d).",
                   hMem,
                   lpaaFileName[OBJ_GLBALLOC - 1][j],
                   lpaauLinNum [OBJ_GLBALLOC - 1][j]);
} // End LastTouch


//***************************************************************************
//  $MyCloseSemaphore
//
//  Close a semaphore
//***************************************************************************

UBOOL _MyCloseSemaphore
    (HANDLE hSemaphore,                             // Semaphore handle
     LPCHAR lpFileName,                             // Ptr to filename
     UINT   uLine)                                  // Line #

{
    UBOOL bRet;
    char  szTemp[1024];
#ifdef DEBUG_SEMAPHORE
    dprintfWL0 (L"~~Closing semaphore:    %p (%S#%d)", hSemaphore, lpFileName, uLine);
#endif
    bRet =
      CloseHandle (hSemaphore); // hSemaphore = NULL;   // Semaphore handle
    if (!bRet)
    {
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MBC (szTemp)
        DbgBrk ();              // #ifdef DEBUG
    } else
    if (gbResDebug)
        _DeleObj (OBJ_SEMAPHORE, hSemaphore);

    return bRet;
} // _MyCloseSemaphore


//***************************************************************************
//  $MyCreateCompatibleBitmap
//
//  Create a compatible bitmap
//***************************************************************************

HBITMAP _MyCreateCompatibleBitmap
    (HDC  hDC,      // Handle to device context
     int  nWidth,   // Width of bitmap, in pixels
     int  nHeight,  // Height of bitmap, in pixels
     UINT uLine)    // Line #

{
    HBITMAP hBitmap;
    char    szTemp[1024];

    hBitmap = CreateCompatibleBitmap (hDC, nWidth, nHeight);

    if (!hBitmap)
    {
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MBC (szTemp)
        DbgBrk ();              // #ifdef DEBUG
    } else
    if (gbResDebug)
        _SaveObj (OBJ_BITMAP,   hBitmap, NULL,       uLine);

    return hBitmap;
} // _MyCreateCompatibleBitmap


//***************************************************************************
//  $MyCreateCompatibleDC
//
//  Create a compatible memory DC
//***************************************************************************

HDC _MyCreateCompatibleDC
    (HDC  hDC,                  // Handle to device context
     UINT uLine)                // Line #

{
    HDC  hMEMDC;
    char szTemp[1024];

    hMEMDC = CreateCompatibleDC (hDC);

    if (hMEMDC EQ NULL)
    {
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MBC (szTemp)
        DbgBrk ();              // #ifdef DEBUG
    } else
    if (gbResDebug)
        _SaveObj (OBJ_MEMDC,    hMEMDC,  NULL,       uLine);

    return hMEMDC;
} // _MyCreateCompatibleDC


//***************************************************************************
//  $MyCreatePen
//
//  Create a pen
//***************************************************************************

HPEN _MyCreatePen
    (int      fnPenStyle,   // Pen style
     int      nWidth,       // Pen width
     COLORREF crColor,      // Pen color
     UINT     uLine)        // Line #

{
    HPEN hPen;
    char szTemp[1024];

    hPen = CreatePen (fnPenStyle, nWidth, crColor);

    if (!hPen)
    {
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MBC (szTemp)
        DbgBrk ();              // #ifdef DEBUG
    } else
    if (gbResDebug)
        _SaveObj (OBJ_PEN,      hPen,    NULL,       uLine);

    return hPen;
} // _MyCreatePen


//***************************************************************************
//  $MyCreateFontIndirect
//
//  Create a font indirectly
//***************************************************************************

HFONT _MyCreateFontIndirect
    (CONST LOGFONT *lplf,   // Pointer to logical font structure
     UINT          uLine)   // Line #

{
    HFONT hFont;
    char  szTemp[1024];

    hFont = CreateFontIndirect (lplf);

    if (!hFont)
    {
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MBC (szTemp)
        DbgBrk ();              // #ifdef DEBUG
    } else
    if (gbResDebug)
        _SaveObj (OBJ_FONT,     hFont,   NULL,       uLine);

    return hFont;
} // _MyCreateFontIndirect


//***************************************************************************
//  $MyCreateFontIndirectW
//
//  Create a font indirectly
//***************************************************************************

HFONT _MyCreateFontIndirectW
    (CONST LOGFONTW *lplf,      // Pointer to logical font structure
     UINT            uLine)     // Line #

{
    HFONT hFont;
    char  szTemp[1024];

    hFont = CreateFontIndirectW (lplf);

    if (!hFont)
    {
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MBC (szTemp)
        DbgBrk ();              // #ifdef DEBUG
    } else
    if (gbResDebug)
        _SaveObj (OBJ_FONT,     hFont,   NULL,       uLine);

    return hFont;
} // _MyCreateFontIndirectW


//***************************************************************************
//  $MyCreatePolygonRgn
//
//  Create a polygon region
//***************************************************************************

HRGN _MyCreatePolygonRgn
    (CONST POINT *lppt,     // Pointer to array of points
     int  cPoints,          // Number of points in array
     int  fnPolyFillMode,   // Polygon-filling mode
     UINT uLine)            // Line #

{
    HRGN hRgn;
    char szTemp[1024];

    hRgn = CreatePolygonRgn (lppt, cPoints, fnPolyFillMode);

    if (!hRgn)
    {
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MBC (szTemp)
        DbgBrk ();              // #ifdef DEBUG
    } else
    if (gbResDebug)
        _SaveObj (OBJ_REGION,   hRgn,    NULL,       uLine);

    return hRgn;
} // _MyCreatePolygonRgn


//***************************************************************************
//  $MyCreateRectRgnIndirect
//
//  Create a rectangular region indirect
//***************************************************************************

HRGN _MyCreateRectRgnIndirect
    (CONST RECT *lprc,      // Pointer to the rectangle
     UINT       uLine)      // Line #

{
    HRGN hRgn;
    char szTemp[1024];

    hRgn = CreateRectRgnIndirect (lprc);

    if (!hRgn)
    {
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MBC (szTemp)
        DbgBrk ();              // #ifdef DEBUG
    } else
    if (gbResDebug)
        _SaveObj (OBJ_REGION,   hRgn,    NULL,       uLine);

    return hRgn;
} // _MyCreateRectRgnIndirect


//***************************************************************************
//  $MyCreateSemaphoreW
//
//  Create a semaphore
//***************************************************************************

HANDLE _MyCreateSemaphoreW
    (LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,   // Ptr to security attributes (may be NULL)
     LONG                  lInitialCount,           // Initial count
     LONG                  lMaximumCount,           // Maximum count
     LPWSTR                lpName,                  // Ptr to name (may be NULL)
     LPCHAR                lpFileName,              // Ptr to filename
     UINT                  uLine)                   // Line #

{
    HANDLE hSemaphore;
    char   szTemp[1024];

    hSemaphore =
      CreateSemaphoreW (lpSemaphoreAttributes,      // Ptr to security attributes (may be NULL)
                        lInitialCount,              // Initial count
                        lMaximumCount,              // Maximum count
                        lpName);                    // Ptr to name (may be NULL)
#ifdef DEBUG_SEMAPHORE
    dprintfWL0 (L"~~Created semaphore:    %p (%S#%d)", hSemaphore, lpFileName, uLine);
#endif
    if (!hSemaphore)
    {
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MBC (szTemp)
        DbgBrk ();              // #ifdef DEBUG
    } else
    if (gbResDebug)
        _SaveObj (OBJ_SEMAPHORE, hSemaphore, lpFileName, uLine);

    return hSemaphore;
} // _MyCreateSemaphoreW


//***************************************************************************
//  $MyCreateSolidBrush
//
//  Create a solid brush
//***************************************************************************

HBRUSH _MyCreateSolidBrush
    (COLORREF crColor,      // Brush color value
     UINT     uLine)        // Line #

{
    HBRUSH hBrush;
    char   szTemp[1024];

    hBrush = CreateSolidBrush (crColor);

    if (!hBrush)
    {
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MBC (szTemp)
        DbgBrk ();              // #ifdef DEBUG
    } else
    if (gbResDebug)
        _SaveObj (OBJ_BRUSH,    hBrush,  NULL,       uLine);

    return hBrush;
} // _MyCreateSolidBrush


//***************************************************************************
//  $MyDeleteDC
//
//  Delete a memory DC
//***************************************************************************

UBOOL _MyDeleteDC
    (HDC  hMEMDC,               // Handle to memory device context
     UINT uLine)                // Line #

{
    UBOOL bRet;
    char  szTemp[1024];

    bRet = DeleteDC (hMEMDC);

    if (!bRet)
    {
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MBC (szTemp)
        DbgBrk ();              // #ifdef DEBUG
    } else
    if (gbResDebug)
        _DeleObj (OBJ_MEMDC, hMEMDC);

    return bRet;
} // _MyDeleteDC


//***************************************************************************
//  $MyDeleteObject
//
//  Delete an object
//***************************************************************************

UBOOL _MyDeleteObject
    (HGDIOBJ hObject,       // Handle to graphic object
     UINT    uLine)         // Line #

{
    UBOOL bRet;
    DWORD dwType;
    char  szTemp[1024];

    // See what kind of object we're deleting
    dwType = GetObjectType (hObject);

    // Validate the object type
    if (!(OBJ_PEN <= dwType && dwType <= OBJ_COLORSPACE))
        DbgBrk ();              // #ifdef DEBUG

    // Delete the object
    bRet = DeleteObject (hObject);

    // If it failed, ...
    if (!bRet)
    {
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MBC (szTemp)
        DbgBrk ();              // #ifdef DEBUG
    } else
    if (gbResDebug)
        _DeleObj (dwType, hObject);

    return bRet;
} // _MyDeleteObject


//***************************************************************************
//  $MyGetDC
//
//  Get a DC
//***************************************************************************

HDC _MyGetDC
    (HWND hWnd,         // Handle of window
     UINT uLine)        // Line #

{
    HDC  hDC;
    char szTemp[1024];

    hDC = GetDC (hWnd);

    if (!hDC)
    {
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MBC (szTemp)
        DbgBrk ();              // #ifdef DEBUG
    } else
    if (gbResDebug)
        _SaveObj (OBJ_DC,       hDC,     NULL,       uLine);

    return hDC;
} // _MyGetDC


//***************************************************************************
//  $MyGetWindowDC
//
//  Get a window DC
//***************************************************************************

HDC _MyGetWindowDC
    (HWND hWnd,             // Handle of window
     UINT uLine)            // Line #

{
    HDC  hDC;
    char szTemp[1024];

    hDC = GetWindowDC (hWnd);

    if (!hDC)
    {
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MBC (szTemp)
        DbgBrk ();              // #ifdef DEBUG
    } else
    if (gbResDebug)
        _SaveObj (OBJ_DC,       hDC,     NULL,       uLine);

    return hDC;
} // _MyGetWindowDC


//***************************************************************************
//  $MyLoadBitmap
//
//  Load a bitmap
//***************************************************************************

HBITMAP _MyLoadBitmap
    (HINSTANCE hInstance,       // Handle of application instance
     LPCTSTR   lpBitmapName,    // Address of bitmap resource name
     UINT      uLine)           // Line #

{
    HBITMAP hBitmap;
    char    szTemp[1024];

    hBitmap = LoadBitmap (hInstance, lpBitmapName);

    if (!hBitmap)
    {
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MBC (szTemp)
        DbgBrk ();              // #ifdef DEBUG
    } else
    if (gbResDebug)
        _SaveObj (OBJ_BITMAP,   hBitmap, NULL,       uLine);

    return hBitmap;
} // _MyLoadBitmap


//***************************************************************************
//  $MyLoadImage
//
//  Load an image
//***************************************************************************

HANDLE _MyLoadImage
    (HINSTANCE hinst,       // Handle of the instance that contains the image
     LPCTSTR   lpszName,    // Name or identifier of image
     UINT      uType,       // Type of image
     int       cxDesired,   // Desired width
     int       cyDesired,   // Desired height
     UINT      fuLoad,      // Load flags
     UINT      uLine)       // Line #

{
    HBITMAP hBitmap;
    char    szTemp[1024];

    hBitmap = LoadImage (hinst, lpszName, uType, cxDesired, cyDesired, fuLoad);

    if (!hBitmap)
    {
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MBC (szTemp)
        DbgBrk ();              // #ifdef DEBUG
    } else
    if (gbResDebug)
        _SaveObj (OBJ_BITMAP,   hBitmap, NULL,       uLine);

    return hBitmap;
} // _MyLoadImage


//***************************************************************************
//  $MyReleaseDC
//
//  Release a DC
//***************************************************************************

UBOOL _MyReleaseDC
    (HWND hWnd,                 // Handle of window
     HDC  hDC,                  // Handle of device context
     UINT uLine)                // Line #

{
    UBOOL bRet;
    char  szTemp[1024];

    bRet = ReleaseDC (hWnd, hDC);

    if (!bRet)
    {
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MBC (szTemp)
        DbgBrk ();              // #ifdef DEBUG
    } else
    if (gbResDebug)
        _DeleObj (OBJ_DC, hDC);

    return bRet;
} // _MyReleaseDC


//***************************************************************************
//  $MyReleaseSemaphore
//
//  Release a semaphore
//***************************************************************************

UBOOL _MyReleaseSemaphore
    (HANDLE hSemaphore,                             // Semaphore handle
     LONG   lReleaseCount,                          // Release count
     LPLONG lpPreviousCount,                        // Ptr to previous count (may be NULL)
     LPCHAR lpFileName,                             // Ptr to filename
     UINT   uLine)                                  // Line #

{
    UBOOL bRet;
    char  szTemp[1024];

#ifdef DEBUG_SEMAPHORE
    dprintfWL0 (L"~~Releasing semaphore:  %p (%S#%d)", hSemaphore, lpFileName, uLine);
#endif
    bRet =
      ReleaseSemaphore (hSemaphore,                 // Semaphore handle
                        lReleaseCount,              // Release count
                        lpPreviousCount);           // Ptr to previous count (may be NULL)
    if (!bRet)
    {
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MBC (szTemp)
        DbgBrk ();              // #ifdef DEBUG
    } // End IF

    return bRet;
} // _MyReleaseSemaphore


//***************************************************************************
//  $MyGlobalAlloc
//
//  Allocate a global variable
//***************************************************************************

LPVOID _MyGlobalAlloc
    (UINT   uFlags,         // Object allocation attributes
     SIZE_T dwBytes,        // Number of bytes to allocate
     LPCHAR lpFileName,     // Ptr to filename
     UINT   uLine)          // Line #

{
    LPVOID lpMem;
    char   szTemp[1024];

////dprintfWL0 (L"CheckMemStat:  %S(#%u)", lpFileName, uLine);

    CheckMemStat ();

    lpMem = GlobalAlloc (uFlags, dwBytes);

    if (lpMem EQ NULL)
    {
        UINT uLastError;

        // Get the last error
        uLastError = GetLastError ();
#ifdef DEBUG
        if (uLastError EQ ERROR_NOT_ENOUGH_MEMORY)
            DisplayGlobals (0);
#endif
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       uLastError,                  // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MBC (szTemp)
        DbgBrk ();              // #ifdef DEBUG
    } else
    if (gbResDebug)
        _SaveObj (OBJ_GLBALLOC, lpMem,   lpFileName, uLine);

    return lpMem;
} // End _MyGlobalAlloc


//***************************************************************************
//  $MyGlobalHandle
//
//  Return the global handle coresponding to a global memory ptr
//***************************************************************************

HGLOBAL _MyGlobalHandle
    (LPVOID lpVoid,         // Address of the global memory object
     LPCHAR lpFileName,     // Ptr to filename
     UINT   uLine)          // Line #

{
    HGLOBAL hGlb;           // Handle of the global memory object
    char    szTemp[1024];

////dprintfWL0 (L"CheckMemStat:  %S(#%u)", lpFileName, uLine);

    CheckMemStat ();

    // Call the Windows function
    hGlb = GlobalHandle (lpVoid);

    if (hGlb EQ NULL)
    {
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MBC (szTemp)
        DbgBrk ();              // #ifdef DEBUG
    } // End IF

    return hGlb;
} // End _MyGlobalHandle


//***************************************************************************
//  $MyGlobalLock
//
//  Lock a global variable
//***************************************************************************

LPVOID _MyGlobalLock
    (HGLOBAL hMem,          // Address of the global memory object
     LPCHAR  lpFileName,    // Ptr to filename
     UINT    uLine)         // Line #

{
    return _MyGlobalLockSub (hMem, TRUE, lpFileName, uLine);
} // End _MyGlobalLock


//***************************************************************************
//  $MyGlobalLockNS
//
//  Lock a global variable, no save filename
//***************************************************************************

LPVOID _MyGlobalLockNS
    (HGLOBAL hMem,          // Address of the global memory object
     LPCHAR  lpFileName,    // Ptr to filename
     UINT    uLine)         // Line #

{
    return _MyGlobalLockSub (hMem, FALSE, lpFileName, uLine);
} // End _MyGlobalLockNS


//***************************************************************************
//  $MyGlobalLockSub
//
//  Lock a global variable subroutine
//***************************************************************************

LPVOID _MyGlobalLockSub
    (HGLOBAL hMem,          // Address of the global memory object
     UBOOL   bSaveFileName, // TRUE iff we should save the filename
     LPCHAR  lpFileName,    // Ptr to filename
     UINT    uLine)         // Line #

{
    LPVOID lpVoid;
    char   szTemp[1024];

    Assert (bSaveFileName);     // Unused as yet

////dprintfWL0 (L"CheckMemStat:  %S(#%u)", lpFileName, uLine);

    CheckMemStat ();

    lpVoid = GlobalLock (hMem);

    if (lpVoid EQ NULL)
    {
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MySprintf (&szTemp[lstrlen (szTemp)],
                    sizeof (szTemp) - lstrlen (szTemp),
                    " -- hMem = %p",
                    hMem);
        MBC (szTemp)
        DbgBrk ();              // #ifdef DEBUG
    } else
    if (gbResDebug)
        _SaveObj (OBJ_GLBLOCK,  hMem,    lpFileName, uLine);

    return lpVoid;
} // End _MyGlobalLockSub


//***************************************************************************
//  $MyGlobalUnlock
//
//  Unlock a global variable
//***************************************************************************

UBOOL _MyGlobalUnlock
    (HGLOBAL hMem,          // Address of the global memory object
     LPCHAR  lpFileName,    // Ptr to filename
     UINT    uLine)         // Line #

{
    UBOOL bRet;
    char  szTemp[1024];

////dprintfWL0 (L"CheckMemStat:  %S(#%u)", lpFileName, uLine);

    CheckMemStat ();

    // Ensure the lock count is non-zero
    if ((_MyGlobalFlags (hMem, lpFileName, uLine) & GMEM_LOCKCOUNT) EQ 0)
    {
        // Format a message about the last lock & alloc
        LastTouch (szTemp, hMem, FALSE);
        DbgBrk ();              // #ifdef DEBUG
    } // End IF

    bRet = GlobalUnlock (hMem);
    if (gbResDebug)
        _DeleObj (OBJ_GLBLOCK, hMem);

    return bRet;
} // End _MyGlobalUnlock


//***************************************************************************
//  $MyGlobalSize
//
//  Get the size of a global variable
//***************************************************************************

SIZE_T _MyGlobalSize
    (HGLOBAL hMem,          // Address of the global memory object
     LPCHAR  lpFileName,    // Ptr to filename
     UINT    uLine)         // Line #

{
    SIZE_T dwRet;
    char   szTemp[1024];

////dprintfWL0 (L"CheckMemStat:  %S(#%u)", lpFileName, uLine);

    CheckMemStat ();

    dwRet = GlobalSize (hMem);
    if (dwRet EQ 0)
    {
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MBC (szTemp)
        DbgBrk ();              // #ifdef DEBUG
    } // End IF

    return dwRet;
} // End _MyGlobalSize


//***************************************************************************
//  $MyGlobalFlags
//
//  Get the flags of a global variable
//***************************************************************************

DWORD _MyGlobalFlags
    (HGLOBAL hMem,          // Address of the global memory object
     LPCHAR  lpFileName,    // Ptr to filename
     UINT    uLine)         // Line #

{
    DWORD dwRet;
    char  szTemp[1024];

////dprintfWL0 (L"CheckMemStat:  %S(#%u)", lpFileName, uLine);

    CheckMemStat ();

    dwRet = GlobalFlags (hMem);
    if (dwRet EQ GMEM_INVALID_HANDLE)
    {
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MBC (szTemp)
        DbgBrk ();              // #ifdef DEBUG
    } // End IF

    return dwRet;
} // End _MyGlobalFlags


//***************************************************************************
//  $MyGlobalReAlloc
//
//  Resize a global variable
//***************************************************************************

HGLOBAL _MyGlobalReAlloc
    (HGLOBAL hMem,          // Address of the global memory object
     SIZE_T  dwBytes,       // New size of block
     UINT    uFlags,        // How to reallocate
     LPCHAR  lpFileName,    // Ptr to filename
     UINT    uLine)         // Line #

{
    SIZE_T  dwRet;
    HGLOBAL hGlb;
    char    szTemp[1024];

////dprintfWL0 (L"CheckMemStat:  %S(#%u)", lpFileName, uLine);

    CheckMemStat ();

    dwRet = GlobalSize (hMem);
    if (dwRet EQ 0)
    {
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MBC (szTemp)
        DbgBrk ();              // #ifdef DEBUG
    } // End IF

    if ((_MyGlobalFlags (hMem, lpFileName, uLine) & GMEM_LOCKCOUNT) NE 0)
    {
        // Format a message about the last lock & alloc
        LastTouch (szTemp, hMem, TRUE);
        DbgBrk ();              // #ifdef DEBUG
    } // End IF

    hGlb = GlobalReAlloc (hMem, dwBytes, uFlags);
    if (hGlb EQ NULL)
    {
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MBC (szTemp)
        DbgBrk ();              // #ifdef DEBUG
    } else
    if (gbResDebug)
    {
        if (hGlb NE hMem)
        {
            _DeleObj (OBJ_GLBALLOC, hMem);
            _SaveObj (OBJ_GLBALLOC, hGlb,    lpFileName, uLine);
        } // End IF
    } // End IF/ELSE

    return hGlb;
} // End _MyGlobalReAlloc


//***************************************************************************
//  $MyGlobalFree
//
//  My GlobalFree function which checks for unlocked memory
//***************************************************************************

HGLOBAL _MyGlobalFree
    (HGLOBAL hMem,          // Handle
     LPCHAR  lpFileName,    // Ptr to filename
     UINT    uLine)         // Line #

{
    char szTemp[1024];

////dprintfWL0 (L"CheckMemStat:  %S(#%u)", lpFileName, uLine);

    CheckMemStat ();

    // GlobalFlags returns the lock count in the low-order byte
    if ((_MyGlobalFlags (hMem, lpFileName, uLine) & GMEM_LOCKCOUNT) NE 0)
    {
        // Format a message about the last lock & alloc
        LastTouch (szTemp, hMem, TRUE);
        DbgBrk ();              // #ifdef DEBUG
    } else
    if (gbResDebug)
        _DeleObj (OBJ_GLBALLOC, hMem);

    if (gbResDebug)
        CheckMemStat ();

    return GlobalFree (hMem);
} // _MyGlobalFree


//***************************************************************************
//  $MyHeapAlloc
//
//  Allocate a heap variable
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MyHeapAlloc"
#else
#define APPEND_NAME
#endif

LPVOID _MyHeapAlloc
    (HANDLE hHeap,          // Handle to the heap
     DWORD  dwFlags,        // Heap allocation options
     SIZE_T dwBytes,        // Number of bytes to allocate
     UINT   uLine)          // Line #

{
#if defined (DEBUG) && defined (DEBUG_HEAP)
    LPVOID lpMem;

    HeapValidate (hHeap, 0, NULL);

    __try
    {
////////lpMem = HeapAlloc (hHeap, dwFlags, dwBytes);
        lpMem = dlmalloc (dwBytes);
    } __except (CheckException (GetExceptionInformation (), L"_MyHeapAlloc"))
    {
        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L" #1: %2d (%S#%d)", MyGetExceptionCode (), FNLN);

        Assert (lpMem EQ NULL);
#ifdef DEBUG
        if (MyGetExceptionCode () EQ STATUS_NO_MEMORY)
            DisplayHeap ();
#endif
        MBWC (MyGetExceptionStr (MyGetExceptionCode ()))
        DbgBrk ();              // #ifdef DEBUG
    } // End __try/__except

    if (lpMem)
    {
        if (gbResDebug)
            _SaveObj (OBJ_HEAPALLOC, lpMem,   NULL,       uLine);
#ifdef DEBUG
    } else
    {
        DisplayHeap ();
        DbgBrk ();              // #ifdef DEBUG
#endif
    } // End IF

    return lpMem;
#else
    return dlmalloc (dwBytes);
#endif
} // End _MyHeapAlloc
#undef  APPEND_NAME


//***************************************************************************
//  $MyHeapReAlloc
//
//  Resize a heap variable
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MyHeapReAlloc"
#else
#define APPEND_NAME
#endif

HGLOBAL _MyHeapReAlloc
    (HANDLE hHeap,          // Handle to the heap
     DWORD  dwFlags,        // Heap reallocation options
     LPVOID lpMem,          // Ptr to block of memory
     SIZE_T dwBytes,        // New size of block
     UINT   uLine)          // Line #

{
#if defined (DEBUG) && defined (DEBUG_HEAP)
    HGLOBAL hGlb;

    HeapValidate (hHeap, 0, NULL);

    __try
    {
////////hGlb = HeapReAlloc (hHeap, dwFlags, lpMem, dwBytes);
        hGlb = dlrealloc (lpMem, dwBytes);
    } __except (CheckException (GetExceptionInformation (), L"HeapReAlloc"))
    {
        dprintfWL0 (L"!!Initiating Exception in " APPEND_NAME L" #1: %2d (%S#%d)", MyGetExceptionCode (), FNLN);

        Assert (hGlb EQ NULL);
#ifdef DEBUG
        if (MyGetExceptionCode () EQ STATUS_NO_MEMORY)
            DisplayHeap ();
#endif
        MBWC (MyGetExceptionStr (MyGetExceptionCode ()))
        DbgBrk ();              // #ifdef DEBUG
    } // End __try/__except

#ifdef DEBUG
    if (hGlb EQ NULL)
    {
        DisplayHeap ();
        DbgBrk ();              // #ifdef DEBUG
    } // End IF
#endif
    // If the block moved, ...
    if (gbResDebug)
    if (hGlb && lpMem NE hGlb)
    {
        _DeleObj (OBJ_HEAPALLOC, lpMem);
        _SaveObj (OBJ_HEAPALLOC, hGlb,    NULL,       uLine);
    } // End IF/ELSE

    return hGlb;
#else
    return dlrealloc (lpMem, dwBytes);
#endif
} // End _MyHeapReAlloc
#undef  APPEND_NAME


//***************************************************************************
//  $MyHeapFree
//
//  My HeapFree function which checks for unlocked memory
//***************************************************************************

UBOOL _MyHeapFree
    (HANDLE hHeap,          // Handle to the heap
     DWORD  dwFlags,        // Heap reallocation options
     LPVOID lpMem,          // Ptr to block of memory
     UINT   uLine)          // Line #

{
#if defined (DEBUG) && defined (DEBUG_HEAP)
    if (gbResDebug)
    {
        HeapValidate (hHeap, 0, NULL);
        _DeleObj (OBJ_HEAPALLOC, lpMem);
    } // End IF
#endif

////return HeapFree (hHeap, dwFlags, lpMem);
    dlfree (lpMem);

    return TRUE;
} // _MyHeapFree


//***************************************************************************
//  $MyVirtualAlloc
//
//  My VirtualAlloc function which checks for NULL results
//***************************************************************************

LPVOID _MyVirtualAlloc
    (LPVOID lpAddress,          // Address of region to reserve or commit
     size_t dwSize,             // Size of region
     DWORD  flAllocationType,   // Type of allocation
     DWORD  flProtect,          // Type of access protection
     UINT   uLine)              // Line #

{
    LPVOID lpRet;               // Ptr to result
    char   szTemp[1024];

    // Call the original function
    lpRet =
      VirtualAlloc (lpAddress,          // Address of region to reserve or commit
                    dwSize,             // Size of region
                    flAllocationType,   // Type of allocation
                    flProtect);         // Type of access protection
    if (lpRet EQ NULL)
    {
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MBC (szTemp)
        DbgBrk ();              // #ifdef DEBUG
    } // End IF

    return lpRet;
} // End _MyVirtualAlloc


//***************************************************************************
//  $MyVirtualFree
//
//  My VirtualFree function which checks for FALSE results
//***************************************************************************

UBOOL _MyVirtualFree
    (LPVOID lpAddress,          // Address of region of committed pages
     DWORD  dwSize,             // Size of region
     DWORD  dwFreeType,         // Type of free operation
     UINT   uLine)              // Line #

{
    UBOOL bRet;                 // TRUE iff the result is valid
    char  szTemp[1024];

    // Call the original function
    bRet =
      VirtualFree (lpAddress,   // Address of region of committed pages
                   dwSize,      // Size of region
                   dwFreeType); // Type of free operation
    if (!bRet)
    {
        FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM,  // Source and processing options
                       NULL,                        // Pointer to  message source
                       GetLastError (),             // Requested message identifier
                       0,                           // Language identifier for requested message
                       szTemp,                      // Pointer to message buffer
                       sizeof (szTemp),             // Maximum size of message buffer
                       NULL);                       // Address of array of message inserts
        MBC (szTemp)
        DbgBrk ();              // #ifdef DEBUG
    } // End IF

    return bRet;
} // End _MyVirtualFree


//***************************************************************************
//  $MyQueryObject
//
//  Check to see how many of a given OBJECT we have outstanding
//***************************************************************************

HANDLE _MyQueryObject
    (DWORD dwType,
     int   iCount,
     UINT  uLine)           // Line #

{
    int *lpiCount;

    // Get a ptr to the counter
    lpiCount = lpiaCount[dwType - 1];

    if (iCount NE *lpiCount)
        DbgBrk ();              // #ifdef DEBUG

    return &lpaah[dwType - 1][(*lpiCount) - 1];
} // End _MyQueryObject


//***************************************************************************
//  $MyWaitForSemaphore
//
//  Wait for a semaphore
//***************************************************************************

DWORD _MyWaitForSemaphore
    (HANDLE  hSemaphore,                            // Semaphore handle
     DWORD   dwMilliseconds,                        // Time-out interval
     LPWCHAR lpCallerIdent,                         // Ptr to caller identification
     LPCHAR  lpFileName,                            // Ptr to filename
     UINT    uLine)                                 // Line #

{
    DWORD   dwWaitRes;
#ifdef DEBUG_SEMAPHORE
    WCHAR   wszTemp[1024];
    LPWCHAR lpwszWaitRes;

    dprintfWL0 (L"~~WaitForSemaphore (ENTRY):  %p %s (%S#%d)", hSemaphore, lpCallerIdent, lpFileName, uLine);
#endif
    dwWaitRes =
      WaitForSingleObject (hSemaphore, dwMilliseconds);
#ifdef DEBUG_SEMAPHORE
    switch (dwWaitRes)
    {
        case WAIT_FAILED:
            lpwszWaitRes = L"WAIT_FAILED";

            break;

        case WAIT_OBJECT_0:
            lpwszWaitRes = L"WAIT_OBJECT_0";

            break;

        case WAIT_ABANDONED:
            lpwszWaitRes = L"WAIT_ABANDONED";

            break;

        case WAIT_TIMEOUT:
            lpwszWaitRes = L"WAIT_TIMEOUT";

            break;

        case WAIT_IO_COMPLETION:
            lpwszWaitRes = L"WAIT_IO_COMPLETION";

            break;

        default:
            MySprintfW (wszTemp,
                        sizeof (wszTemp),
                       L"***Unknown Wait Reason (%08X)***",
                        dwWaitRes);
            lpwszWaitRes = wszTemp;

            break;
    } // End SWITCH

    dprintfWL0 (L"~~WaitForSemaphore (EXIT ):  %p %s %s (%S#%d)", hSemaphore, lpCallerIdent, lpwszWaitRes, lpFileName, uLine);
#endif

    return dwWaitRes;
} // _MyWaitForSemaphore


//***************************************************************************
//  $MyWaitForThread
//
//  Wait for a thread handle
//***************************************************************************

DWORD _MyWaitForThread
    (HANDLE  hThread,                               // Thread handle
     DWORD   dwMilliseconds,                        // Time-out interval
     LPWCHAR lpCallerIdent,                         // Ptr to caller identification
     LPCHAR  lpFileName,                            // Ptr to filename
     UINT    uLine)                                 // Line #

{
    DWORD   dwWaitRes;
#ifdef DEBUG_THREAD
    WCHAR   wszTemp[1024];
    LPWCHAR lpwszWaitRes;

    dprintfWL0 (L"~~WaitForThread (ENTRY):  %p %s (%S#%d)", hThread, lpCallerIdent, lpFileName, uLine);
#endif
    dwWaitRes =
      WaitForSingleObject (hThread, dwMilliseconds);
#ifdef DEBUG_THREAD
    switch (dwWaitRes)
    {
        case WAIT_FAILED:
            lpwszWaitRes = L"WAIT_FAILED";

            break;

        case WAIT_OBJECT_0:
            lpwszWaitRes = L"WAIT_OBJECT_0";

            break;

        case WAIT_ABANDONED:
            lpwszWaitRes = L"WAIT_ABANDONED";

            break;

        case WAIT_TIMEOUT:
            lpwszWaitRes = L"WAIT_TIMEOUT";

            break;

        case WAIT_IO_COMPLETION:
            lpwszWaitRes = L"WAIT_IO_COMPLETION";

            break;

        default:
            MySprintfW (wszTemp,
                        sizeof (wszTemp),
                       L"***Unknown Wait Reason (%08X)***",
                        dwWaitRes);
            lpwszWaitRes = wszTemp;

            break;
    } // End SWITCH

    dprintfWL0 (L"~~WaitForThread (EXIT ):  %p %s %s (%S#%d)", hThread, lpCallerIdent, lpwszWaitRes, lpFileName, uLine);
#endif

    return dwWaitRes;
} // _MyWaitForThread


#ifdef DEBUG
//***************************************************************************
//  $CheckMemStat
//
//  Check on memory status
//***************************************************************************

void _CheckMemStat
    (void)

{
////MEMORYSTATUS memStat;
////
////memStat.dwLength = sizeof (memStat);
////
////// Check the status
////GlobalMemoryStatus (&memStat);
////
////if (memStat.dwMemoryLoad EQ 100)
////    DbgBrk ();              // #ifdef DEBUG
////
#if defined (DEBUG) && defined (DEBUG_HEAP)
    if (!HeapValidate (GetProcessHeap (), 0, NULL))
        DbgBrk ();              // #ifdef DEBUG
#endif
} // End _CheckMemStat
#endif
#endif


//***************************************************************************
//  End of File: resdebug.c
//***************************************************************************
