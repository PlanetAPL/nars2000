//****************************************************************************
//  NARS2000 -- Resource Debugging Header File
//****************************************************************************

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

// Define this name in order to improve performance by avoiding garbage collection
#define ALLOC_GPTR

#ifdef DEBUG
  #define ValidObj(a,b)                   _ValidObj(a,b)
#else
  #define ValidObj(a,b)                   (a)
#endif

#if RESDEBUG
  #define MyCloseSemaphore(a)             _MyCloseSemaphore(a,FNLN)
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
#ifdef ALLOC_GPTR
  #define MyGlobalAlloc(a,b)              _MyGlobalAlloc(GPTR,b,FNLN)                           // _MyGlobalAlloc(a,b,FNLN)
  #define MyGlobalLock(a)                 ClrPtrTypeDir(a)                                      // _MyGlobalLock(ClrPtrTypeDir(a),FNLN)
  #define MyGlobalLockDfn(a)              ValidObj(ClrPtrTypeDir(a),DFN_HEADER_SIGNATURE)       // _MyGlobalLockDfn(ClrPtrTypeDir(a),FNLN)
  #define MyGlobalLockVar(a)              ValidObj(ClrPtrTypeDir(a),VARARRAY_HEADER_SIGNATURE)  // _MyGlobalLockVar(ClrPtrTypeDir(a),FNLN)
  #define MyGlobalLockLst(a)              ValidObj(ClrPtrTypeDir(a),LSTARRAY_HEADER_SIGNATURE)  // _MyGlobalLockLst(ClrPtrTypeDir(a),FNLN)
  #define MyGlobalLockFcn(a)              ValidObj(ClrPtrTypeDir(a),FCNARRAY_HEADER_SIGNATURE)  // _MyGlobalLockFcn(ClrPtrTypeDir(a),FNLN)
  #define MyGlobalLockVnm(a)              ValidObj(ClrPtrTypeDir(a),VARNAMED_HEADER_SIGNATURE)  // _MyGlobalLockVnm(ClrPtrTypeDir(a),FNLN)
  #define MyGlobalLockTkn(a)              ValidObj(ClrPtrTypeDir(a),TOKEN_HEADER_SIGNATURE)     // _MyGlobalLockTkn(ClrPtrTypeDir(a),FNLN)
  #define MyGlobalLockNfn(a)              ValidObj(ClrPtrTypeDir(a),NFNS_HEADER_SIGNATURE)      // _MyGlobalLockNfn(ClrPtrTypeDir(a),FNLN)
  #define MyGlobalLock000(a)              ValidObj(ClrPtrTypeDir(a),0)                          // _MyGlobalLock000(ClrPtrTypeDir(a),FNLN)
  #define MyGlobalLock100(a)              ValidObj(ClrPtrTypeDir(a),100)                        // _MyGlobalLock100(ClrPtrTypeDir(a),FNLN)
  #define MyGlobalLockWsz(a)              ValidObj(ClrPtrTypeDir(a),1)                          // _MyGlobalLockWsz(ClrPtrTypeDir(a),FNLN)
  #define MyGlobalLockInt(a)              ValidObj(ClrPtrTypeDir(a),2)                          // _MyGlobalLockInt(ClrPtrTypeDir(a),FNLN)
  #define MyGlobalLockPad(a)              ValidObj(ClrPtrTypeDir(a),2)                          // _MyGlobalLockPad(ClrPtrTypeDir(a),FNLN)
  #define MyGlobalLockTxt(a)              ValidObj(ClrPtrTypeDir(a),3)                          // _MyGlobalLockTxt(ClrPtrTypeDir(a),FNLN)
  #define MyGlobalHandle(a)               ClrPtrTypeDir(a)                                      // _MyGlobalHandle(ClrPtrTypeDir(a))
  #define MyGlobalUnlock(a)               ClrPtrTypeDir(a)                                      // _MyGlobalUnlock(ClrPtrTypeDir(a),__LINE__)
#else
  #define MyGlobalAlloc(a,b)              _MyGlobalAlloc(a,b,FNLN)
  #define MyGlobalLock(a)                 _MyGlobalLock(ClrPtrTypeDir(a),FNLN)
  #define MyGlobalLockDfn(a)              _MyGlobalLock(ValidObj(ClrPtrTypeDir(a),DFN_HEADER_SIGNATURE      ),FNLN)
  #define MyGlobalLockVar(a)              _MyGlobalLock(ValidObj(ClrPtrTypeDir(a),VARARRAY_HEADER_SIGNATURE ),FNLN)
  #define MyGlobalLockLst(a)              _MyGlobalLock(ValidObj(ClrPtrTypeDir(a),LSTARRAY_HEADER_SIGNATURE ),FNLN)
  #define MyGlobalLockFcn(a)              _MyGlobalLock(ValidObj(ClrPtrTypeDir(a),FCNARRAY_HEADER_SIGNATURE ),FNLN)
  #define MyGlobalLockVnm(a)              _MyGlobalLock(ValidObj(ClrPtrTypeDir(a),VARNAMED_HEADER_SIGNATURE ),FNLN)
  #define MyGlobalLockTkn(a)              _MyGlobalLock(ValidObj(ClrPtrTypeDir(a),TOKEN_HEADER_SIGNATURE    ),FNLN)
  #define MyGlobalLockNfn(a)              _MyGlobalLock(ValidObj(ClrPtrTypeDir(a),NFNS_HEADER_SIGNATURE     ),FNLN)
  #define MyGlobalLock000(a)              _MyGlobalLock(ValidObj(ClrPtrTypeDir(a),0                         ),FNLN)
  #define MyGlobalLock100(a)              _MyGlobalLock(ValidObj(ClrPtrTypeDir(a),100                       ),FNLN)
  #define MyGlobalLockWsz(a)              _MyGlobalLock(ValidObj(ClrPtrTypeDir(a),1                         ),FNLN)
  #define MyGlobalLockInt(a)              _MyGlobalLock(ValidObj(ClrPtrTypeDir(a),2                         ),FNLN)
  #define MyGlobalLockPad(a)              _MyGlobalLock(ValidObj(ClrPtrTypeDir(a),2                         ),FNLN)
  #define MyGlobalLockTxt(a)              _MyGlobalLock(ValidObj(ClrPtrTypeDir(a),3                         ),FNLN)
  #define MyGlobalHandle(a)               _MyGlobalHandle(ClrPtrTypeDir(a),FNLN)
  #define MyGlobalUnlock(a)               _MyGlobalUnlock(ClrPtrTypeDir(a),FNLN)
#endif
  #define MyGlobalSize(a)                 _MyGlobalSize(ClrPtrTypeDir(a),FNLN)
  #define MyGlobalFlags(a)                _MyGlobalFlags(ClrPtrTypeDir(a),FNLN)
  #define MyGlobalReAlloc(a,b,c)          _MyGlobalReAlloc(ClrPtrTypeDir(a),b,c,FNLN)
  #define MyGlobalFree(a)                 _MyGlobalFree(ClrPtrTypeDir(a),FNLN)
  #define MyHeapAlloc(a,b,c)              _MyHeapAlloc(a,b,c,__LINE__)
  #define MyHeapReAlloc(a,b,c,d)          _MyHeapReAlloc(a,b,c,d,__LINE__)
  #define MyHeapFree(a,b,c)               _MyHeapFree(a,b,c,__LINE__)
  #define MyReleaseSemaphore(a,b,c)       _MyReleaseSemaphore(a,b,c,FNLN)
  #define MyVirtualAlloc(a,b,c,d)         _MyVirtualAlloc(a,b,c,d,__LINE__)
  #define MyVirtualFree(a,b,c)            _MyVirtualFree(a,b,c,__LINE__)
  #define MyLoadBitmap(a,b)               _MyLoadBitmap(a,b,__LINE__)
  #define MyLoadImage(a,b,c,d,e,f)        _MyLoadImage(a,b,c,d,e,f,__LINE__)
  #define MyQueryObject(a,b)              _MyQueryObject(a,b,__LINE__)
  #define MyReleaseDC(a,b)                _MyReleaseDC(a,b,__LINE__)
  #define MyWaitForSemaphore(a,b,c)       _MyWaitForSemaphore (a,b,c,FNLN)
  #define MyWaitForThread(a,b,c)          _MyWaitForThread (a,b,c,FNLN)

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
#ifdef ALLOC_GPTR
  #define MyGlobalAlloc(a,b)              GlobalAlloc(GPTR,b)                                   // GlobalAlloc(a,b)
  #define MyGlobalLock(a)                 ClrPtrTypeDir(a)                                      // GlobalLock(a)
  #define MyGlobalLockDfn(a)              ValidObj(ClrPtrTypeDir(a),DFN_HEADER_SIGNATURE      ) // GlobalLock(a)
  #define MyGlobalLockVar(a)              ValidObj(ClrPtrTypeDir(a),VARARRAY_HEADER_SIGNATURE ) // GlobalLock(a)
  #define MyGlobalLockLst(a)              ValidObj(ClrPtrTypeDir(a),LSTARRAY_HEADER_SIGNATURE ) // GlobalLock(a)
  #define MyGlobalLockFcn(a)              ValidObj(ClrPtrTypeDir(a),FCNARRAY_HEADER_SIGNATURE ) // GlobalLock(a)
  #define MyGlobalLockVnm(a)              ValidObj(ClrPtrTypeDir(a),VARNAMED_HEADER_SIGNATURE ) // GlobalLock(a)
  #define MyGlobalLockTkn(a)              ValidObj(ClrPtrTypeDir(a),TOKEN_HEADER_SIGNATURE    ) // GlobalLock(a)
  #define MyGlobalLockNfn(a)              ValidObj(ClrPtrTypeDir(a),NFNS_HEADER_SIGNATURE     ) // GlobalLock(a)
  #define MyGlobalLock000(a)              ValidObj(ClrPtrTypeDir(a),0                         ) // GlobalLock(a)
  #define MyGlobalLock100(a)              ValidObj(ClrPtrTypeDir(a),100                       ) // GlobalLock(a)
  #define MyGlobalLockWsz(a)              ValidObj(ClrPtrTypeDir(a),1                         ) // GlobalLock(a)
  #define MyGlobalLockInt(a)              ValidObj(ClrPtrTypeDir(a),2                         ) // GlobalLock(a)
  #define MyGlobalLockPad(a)              ValidObj(ClrPtrTypeDir(a),2                         ) // GlobalLock(a)
  #define MyGlobalLockTxt(a)              ValidObj(ClrPtrTypeDir(a),3                         ) // GlobalLock(a)
  #define MyGlobalHandle(a)               ClrPtrTypeDir(a)                                      // GlobalHandle(a)
  #define MyGlobalUnlock(a)               /* empty */                                           // GlobalUnlock(a)
#else
  #define MyGlobalAlloc(a,b)              GlobalAlloc((a),b)
  #define MyGlobalLock(a)                 GlobalLock(ClrPtrTypeDir(a))
  #define MyGlobalLockDfn(a)              GlobalLock(ValidObj(ClrPtrTypeDir(a),DFN_HEADER_SIGNATURE      ))
  #define MyGlobalLockVar(a)              GlobalLock(ValidObj(ClrPtrTypeDir(a),VARARRAY_HEADER_SIGNATURE ))
  #define MyGlobalLockLst(a)              GlobalLock(ValidObj(ClrPtrTypeDir(a),LSTARRAY_HEADER_SIGNATURE ))
  #define MyGlobalLockFcn(a)              GlobalLock(ValidObj(ClrPtrTypeDir(a),FCNARRAY_HEADER_SIGNATURE ))
  #define MyGlobalLockVnm(a)              GlobalLock(ValidObj(ClrPtrTypeDir(a),VARNAMED_HEADER_SIGNATURE ))
  #define MyGlobalLockTkn(a)              GlobalLock(ValidObj(ClrPtrTypeDir(a),TOKEN_HEADER_SIGNATURE    ))
  #define MyGlobalLockNfn(a)              GlobalLock(ValidObj(ClrPtrTypeDir(a),NFNS_HEADER_SIGNATURE     ))
  #define MyGlobalLock000(a)              GlobalLock(ValidObj(ClrPtrTypeDir(a),0                         ))
  #define MyGlobalLock100(a)              GlobalLock(ValidObj(ClrPtrTypeDir(a),100                       ))
  #define MyGlobalLockWsz(a)              GlobalLock(ValidObj(ClrPtrTypeDir(a),1                         ))
  #define MyGlobalLockInt(a)              GlobalLock(ValidObj(ClrPtrTypeDir(a),2                         ))
  #define MyGlobalLockPad(a)              GlobalLock(ValidObj(ClrPtrTypeDir(a),2                         ))
  #define MyGlobalLockTxt(a)              GlobalLock(ValidObj(ClrPtrTypeDir(a),3                         ))
  #define MyGlobalHandle(a)               GlobalHandle(ClrPtrTypeDir(a))
  #define MyGlobalUnlock(a)               GlobalUnlock(ClrPtrTypeDir(a))
#endif
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
  #define MyWaitForSemaphore(a,b,c)       WaitForSingleObject (a,b)
  #define MyWaitForThread(a,b,c)          WaitForSingleObject (a,b)
#endif

//***************************************************************************
//  End of File: resdebug.h
//***************************************************************************
