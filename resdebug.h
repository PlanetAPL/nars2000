//****************************************************************************
//  NARS2000 -- Resource Debugging Header File
//****************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2014 Sudley Place Software

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
  #define MyCloseSemaphore(a)             _MyCloseSemaphore(a,__LINE__)
  #define MyCreateCompatibleBitmap(a,b,c) _MyCreateCompatibleBitmap(a,b,c,__LINE__)
  #define MyCreateCompatibleDC(a)         _MyCreateCompatibleDC(a,__LINE__)
  #define MyCreateFontIndirect(a)         _MyCreateFontIndirect(a,__LINE__)
  #define MyCreateFontIndirectW(a)        _MyCreateFontIndirectW(a,__LINE__)
  #define MyCreatePen(a,b,c)              _MyCreatePen(a,b,c,__LINE__)
  #define MyCreatePolygonRgn(a,b,c)       _MyCreatePolygonRgn(a,b,c,__LINE__)
  #define MyCreateRectRgnIndirect(a)      _MyCreateRectRgnIndirect(a,__LINE__)
  #define MyCreateSemaphoreW(a,b,c,d)     _MyCreateSemaphoreW(a,b,c,d,FNLN)
  #define MyCreateSolidBrush(a)           _MyCreateSolidBrush(a,__LINE__)
  #define MyDeleteDC(a)                   _MyDeleteDC(a,__LINE__)
  #define MyDeleteObject(a)               _MyDeleteObject(a,__LINE__)
  #define MyGetDC(a)                      _MyGetDC(a,__LINE__)
  #define MyGetWindowDC(a)                _MyGetWindowDC(a,__LINE__)
  #define MyGlobalAlloc(a,b)              _MyGlobalAlloc(GPTR,b,FNLN)           // _MyGlobalAlloc(a,b,FNLN)
  #define MyGlobalLock(a)                 ClrPtrTypeDir(a)                      // _MyGlobalLock(ClrPtrTypeDir(a),FNLN)
  #define MyGlobalHandle(a)               ClrPtrTypeDir(a)                      // _MyGlobalHandle(ClrPtrTypeDir(a))
  #define MyGlobalUnlock(a)               ClrPtrTypeDir(a)                      // _MyGlobalUnlock(ClrPtrTypeDir(a),__LINE__)
  #define MyGlobalSize(a)                 _MyGlobalSize(ClrPtrTypeDir(a),__LINE__)
  #define MyGlobalFlags(a)                _MyGlobalFlags(ClrPtrTypeDir(a),__LINE__)
  #define MyGlobalReAlloc(a,b,c)          _MyGlobalReAlloc(ClrPtrTypeDir(a),b,c,FNLN)
  #define MyGlobalFree(a)                 _MyGlobalFree(ClrPtrTypeDir(a),__LINE__)
  #define MyHeapAlloc(a,b,c)              _MyHeapAlloc(a,b,c,__LINE__)
  #define MyHeapReAlloc(a,b,c,d)          _MyHeapReAlloc(a,b,c,d,__LINE__)
  #define MyHeapFree(a,b,c)               _MyHeapFree(a,b,c,__LINE__)
  #define MyReleaseSemaphore(a,b,c)       _MyReleaseSemaphore(a,b,c,__LINE__)
  #define MyVirtualAlloc(a,b,c,d)         _MyVirtualAlloc(a,b,c,d,__LINE__)
  #define MyVirtualFree(a,b,c)            _MyVirtualFree(a,b,c,__LINE__)
  #define MyLoadBitmap(a,b)               _MyLoadBitmap(a,b,__LINE__)
  #define MyLoadImage(a,b,c,d,e,f)        _MyLoadImage(a,b,c,d,e,f,__LINE__)
  #define MyQueryObject(a,b)              _MyQueryObject(a,b,__LINE__)
  #define MyReleaseDC(a,b)                _MyReleaseDC(a,b,__LINE__)

  #define OBJ_GLBLOCK     15
  #define OBJ_GLBALLOC    16
  #define OBJ_SEMAPHORE   17
#else
  #define MyCloseSemaphore(a)             CloseHandle(a)
  #define MyCreateCompatibleBitmap(a,b,c) CreateCompatibleBitmap(a,b,c)
  #define MyCreateCompatibleDC(a)         CreateCompatibleDC(a)
  #define MyCreateFontIndirect(a)         CreateFontIndirect(a)
  #define MyCreateFontIndirectW(a)        CreateFontIndirectW(a)
  #define MyCreatePen(a,b,c)              CreatePen(a,b,c)
  #define MyCreatePolygonRgn(a,b,c)       CreatePolygonRgn(a,b,c)
  #define MyCreateRectRgnIndirect(a)      CreateRectRgnIndirect(a)
  #define MyCreateSemaphoreW(a,b,c,d)     CreateSemaphoreW(a,b,c,d)
  #define MyCreateSolidBrush(a)           CreateSolidBrush(a)
  #define MyDeleteDC(a)                   DeleteDC(a)
  #define MyDeleteObject(a)               DeleteObject(a)
  #define MyGetDC(a)                      GetDC(a)
  #define MyGetWindowDC(a)                GetWindowDC(a)
  #define MyGlobalAlloc(a,b)              GlobalAlloc(GPTR,b)
  #define MyGlobalLock(a)                 ClrPtrTypeDir(a)  // GlobalLock(a)
  #define MyGlobalHandle(a)               ClrPtrTypeDir(a)  // GlobalHandle(a)
  #define MyGlobalUnlock(a)               /* empty */       // GlobalUnlock(a)
  #define MyGlobalSize(a)                 GlobalSize(ClrPtrTypeDir(a))
  #define MyGlobalFlags(a)                GlobalFlags(ClrPtrTypeDir(a))
  #define MyGlobalReAlloc(a,b,c)          GlobalReAlloc(ClrPtrTypeDir(a),b,c)
  #define MyGlobalFree(a)                 GlobalFree(ClrPtrTypeDir(a))
  #define MyHeapAlloc(a,b,c)              malloc(c)
  #define MyHeapReAlloc(a,b,c,d)          realloc(c,d)
  #define MyHeapFree(a,b,c)               free(c)
  #define MyReleaseSemaphore(a,b,c)       ReleaseSemaphore(a,b,c)
  #define MyVirtualAlloc(a,b,c,d)         VirtualAlloc(a,b,c,d)
  #define MyVirtualFree(a,b,c)            VirtualFree(a,b,c)
  #define MyLoadBitmap(a,b)               LoadBitmap(a,b)
  #define MyLoadImage(a,b,c,d,e,f)        LoadImage(a,b,c,d,e,f)
  #define MyQueryObject(a,b)
  #define MyReleaseDC(a,b)                ReleaseDC(a,b)
#endif

//***************************************************************************
//  End of File: resdebug.h
//***************************************************************************
