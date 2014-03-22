//***************************************************************************
//  NARS2000 -- Reference Count Routines
//***************************************************************************

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

#define STRICT
#include <windows.h>
#include "headers.h"


#ifdef DEBUG
extern HGLOBAL hGlbRC1,
               hGlbRC2;
#endif


//***************************************************************************
//  $ChangeRefCntDir_PTB
//
//  Increment or decrement the reference count
//    of a direct reference to an LPSYMENTRY or an HGLOBAL
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ChangeRefCntDir_PTB"
#else
#define APPEND_NAME
#endif

int ChangeRefCntDir_PTB
    (HGLOBAL hGlb,              // Global memory handle
     int     iIncr)             // Increment/decrement amount

{
    LPVOID     lpSig;           // Ptr to signature
    UINT       RefCnt;          // Reference count
#ifdef DEBUG
    VFOHDRPTRS vfoHdrPtrs;      // Union of VFO Hdr ptrs
#endif

    // Split cases based upon the ptr type
    switch (GetPtrTypeDir (hGlb))
    {
        case PTRTYPE_STCONST:
            return 1;

        case PTRTYPE_HGLOBAL:
#ifdef DEBUG
            if (hGlb && hGlbRC1 && ClrPtrTypeDir (hGlb) EQ ClrPtrTypeDir (hGlbRC1))
                DbgBrk ();

            if (hGlb && hGlbRC2 && ClrPtrTypeDir (hGlb) EQ ClrPtrTypeDir (hGlbRC2))
                DbgBrk ();
#endif
            // Lock the memory to get a ptr to it
#ifdef DEBUG
            vfoHdrPtrs.lpMemVar =
#endif
            lpSig = MyGlobalLock (hGlb);

            // Split cases based upon the array signature
            switch (GetSignatureMem (lpSig))
            {
                case VARARRAY_HEADER_SIGNATURE:
#ifdef DEBUG
  #define lpHeader      vfoHdrPtrs.lpMemVar
#else
  #define lpHeader      ((LPVARARRAY_HEADER) lpSig)
#endif
                    // Don't change the reference count on Perms
                    if (lpHeader->PermNdx NE PERMNDX_NONE)
                    {
#ifdef DEBUG_REFCNT
                        dprintfWL0 (L"  RefCntNC in " APPEND_NAME L":     %p(res=%d) (%S#%d)", hGlb, lpHeader->RefCnt, FNLN);
#endif
                        RefCnt = NEG1U;

                        // If we're to skip this RefCnt increment, ...
                        if (iIncr EQ 1
                         && lpHeader->SkipRefCntIncr)
                            // Clear the flag
                            lpHeader->SkipRefCntIncr = FALSE;

                        break;
                    } // End IF

                    // Change the reference count
#ifdef DEBUG_REFCNT
                    if (iIncr EQ 1
                     && lpHeader->SkipRefCntIncr)
                        dprintfWL0 (L"  RefCnt** in " APPEND_NAME L":     %p(res=%d) (%S#%d)", hGlb, lpHeader->RefCnt        , FNLN);
                    else
                    if (iIncr EQ 1)
                        dprintfWL0 (L"  RefCnt++ in " APPEND_NAME L":     %p(res=%d) (%S#%d)", hGlb, lpHeader->RefCnt + iIncr, FNLN);
                    else
                    if (iIncr EQ -1)
                        dprintfWL0 (L"  RefCnt-- in " APPEND_NAME L":     %p(res=%d) (%S#%d)", hGlb, lpHeader->RefCnt + iIncr, FNLN);
                    else
                        DbgStop ();
#endif
                    Assert (iIncr NE -1 || lpHeader->RefCnt NE 0);

                    // If we're to skip this RefCnt increment, ...
                    if (iIncr EQ 1
                     && lpHeader->SkipRefCntIncr)
                        // Clear the flag
                        lpHeader->SkipRefCntIncr = FALSE;
                    else
                    {
                        // Clear the flag
                        lpHeader->SkipRefCntIncr = FALSE;

                        lpHeader->RefCnt += iIncr;
                    } // End IF/ELSE

                    RefCnt = lpHeader->RefCnt;
#undef  lpHeader
                    break;

                case FCNARRAY_HEADER_SIGNATURE:
#ifdef DEBUG
  #define lpHeader      vfoHdrPtrs.lpMemFcn
#else
  #define lpHeader      ((LPFCNARRAY_HEADER) lpSig)
#endif
                    // Change the reference count
#ifdef DEBUG_REFCNT
                    if (iIncr EQ 1
                     && lpHeader->SkipRefCntIncr)
                        dprintfWL0 (L"  RefCnt** in " APPEND_NAME L":     %p(res=%d) (%S#%d)", hGlb, lpHeader->RefCnt        , FNLN);
                    else
                    if (iIncr EQ 1)
                        dprintfWL0 (L"  RefCnt++ in " APPEND_NAME L":     %p(res=%d) (%S#%d)", hGlb, lpHeader->RefCnt + iIncr, FNLN);
                    else
                    if (iIncr EQ -1)
                        dprintfWL0 (L"  RefCnt-- in " APPEND_NAME L":     %p(res=%d) (%S#%d)", hGlb, lpHeader->RefCnt + iIncr, FNLN);
                    else
                        DbgStop ();
#endif
                    Assert (iIncr NE -1 || lpHeader->RefCnt NE 0);

                    // If we're to skip this RefCnt increment, ...
                    if (iIncr EQ 1
                     && lpHeader->SkipRefCntIncr)
                        // Clear the flag
                        lpHeader->SkipRefCntIncr = FALSE;
                    else
                    {
                        // Clear the flag
                        lpHeader->SkipRefCntIncr = FALSE;

                        lpHeader->RefCnt += iIncr;
                    } // End IF/ELSE

                    RefCnt = lpHeader->RefCnt;
#undef  lpHeader
                    break;

                case DFN_HEADER_SIGNATURE:
#ifdef DEBUG
  #define lpHeader      vfoHdrPtrs.lpMemDfn
#else
  #define lpHeader      ((LPDFN_HEADER) lpSig)
#endif

                    // Don't change the reference count on permanent functions (i.e. Magic Functions/Operators)
                    if (lpHeader->PermFn)
                    {
#ifdef DEBUG_REFCNT
                        dprintfWL0 (L"  RefCntNC in " APPEND_NAME L":     %p(res=%d) (%S#%d)", hGlb, lpHeader->RefCnt, FNLN);
#endif
                        RefCnt = NEG1U;

                        break;
                    } // End IF

                    // Change the reference count
#ifdef DEBUG_REFCNT
                    if (iIncr EQ 1
                     && lpHeader->SkipRefCntIncr)
                        dprintfWL0 (L"  RefCnt** in " APPEND_NAME L":     %p(res=%d) (%S#%d)", hGlb, lpHeader->RefCnt        , FNLN);
                    else
                    if (iIncr EQ 1)
                        dprintfWL0 (L"  RefCnt++ in " APPEND_NAME L":     %p(res=%d) (%S#%d)", hGlb, lpHeader->RefCnt + iIncr, FNLN);
                    else
                    if (iIncr EQ -1)
                        dprintfWL0 (L"  RefCnt-- in " APPEND_NAME L":     %p(res=%d) (%S#%d)", hGlb, lpHeader->RefCnt + iIncr, FNLN);
                    else
                        DbgStop ();
#endif
                    Assert (iIncr NE -1 || lpHeader->RefCnt NE 0);

                    // If we're to skip this RefCnt increment, ...
                    if (iIncr EQ 1
                     && lpHeader->SkipRefCntIncr)
                        // Clear the flag
                        lpHeader->SkipRefCntIncr = FALSE;
                    else
                    {
                        // Clear the flag
                        lpHeader->SkipRefCntIncr = FALSE;

                        lpHeader->RefCnt += iIncr;
                    } // End IF/ELSE

                    RefCnt = lpHeader->RefCnt;
#undef  lpHeader
                    break;

                defstop
                    return -1;
            } // End SWITCH

            MyGlobalUnlock (hGlb); lpSig = NULL;

            return RefCnt;

        defstop
            return -1;
    } // End SWITCH
} // End ChangeRefCntDir_PTB
#undef  APPEND_NAME


//***************************************************************************
//  $IncrRefCntDir_PTB
//
//  Increment the reference count of a direct reference
//    to an LPSYMENTRY or an HGLOBAL.
//***************************************************************************

int IncrRefCntDir_PTB
    (HGLOBAL hGlb)

{
    return ChangeRefCntDir_PTB (hGlb, 1);
} // End IncrRefCntDir_PTB


//***************************************************************************
//  $IncrRefCntInd_PTB
//
//  Increment the reference count of an indirect reference
//    to an LPSYMENTRY or an HGLOBAL.
//***************************************************************************

int IncrRefCntInd_PTB
    (LPVOID lpMem)

{
    return ChangeRefCntDir_PTB (*(HGLOBAL *) lpMem, 1);
} // End IncrRefCntInd_PTB


//***************************************************************************
//  $DecrRefCntDir_PTB
//
//  Decrement the reference count of a direct reference
//    to an LPSYMENTRY or an HGLOBAL.
//***************************************************************************

int DecrRefCntDir_PTB
    (HGLOBAL hGlb)

{
    return ChangeRefCntDir_PTB (hGlb, -1);
} // End DecrRefCntDir_PTB


//***************************************************************************
//  $DecrRefCntInd_PTB
//
//  Decrement the reference count of an indirect reference
//    to an LPSYMENTRY or an HGLOBAL.
//***************************************************************************

int DecrRefCntInd_PTB
    (LPVOID lpMem)

{
    return ChangeRefCntDir_PTB (*(HGLOBAL *) lpMem, -1);
} // End DecrRefCntInd_PTB


//***************************************************************************
//  $IncrRefCntTkn
//
//  Increment the reference count of a token
//***************************************************************************

int IncrRefCntTkn
    (LPTOKEN lptkVar)

{
    HGLOBAL hGlbVar;

    // Get the global memory handle (if it's a global)
    hGlbVar = GetGlbHandle (lptkVar);

    // If it's a global, ...
    if (hGlbVar)
        return ChangeRefCntDir_PTB (MakePtrTypeGlb (hGlbVar), 1);
    else
        return -1;
} // End IncrRefCntTkn


//***************************************************************************
//  $DecrRefCntTkn
//
//  Decrement the reference count of a token
//***************************************************************************

int DecrRefCntTkn
    (LPTOKEN lptkVar)

{
    HGLOBAL hGlbVar;

    // Get the global memory handle (if it's a global)
    hGlbVar = GetGlbHandle (lptkVar);

    // If it's a global, ...
    if (hGlbVar)
        return ChangeRefCntDir_PTB (MakePtrTypeGlb (hGlbVar), -1);
    else
        return -1;
} // End DecrRefCntTkn


//***************************************************************************
//  $GetRefCntGlb
//
//  Get the reference count from a global memory handle
//***************************************************************************

UINT GetRefCntGlb
    (HGLOBAL hGlbArg)           // Arg global memory handle

{
    LPVOID lpMemHdr;            // Ptr to global memory header
    UINT   uRefCnt;             // The array reference count

    // Lock the memory to get a ptr to it
    lpMemHdr = MyGlobalLock (hGlbArg);

    // Split cases based upon the signature
    switch (GetSignatureMem (lpMemHdr))
    {
        case DFN_HEADER_SIGNATURE:
            // Get the reference count
            uRefCnt = ((LPDFN_HEADER) lpMemHdr)->RefCnt;

            break;

        case FCNARRAY_HEADER_SIGNATURE:
            // Get the reference count
            uRefCnt = ((LPFCNARRAY_HEADER) lpMemHdr)->RefCnt;

            break;

        case VARARRAY_HEADER_SIGNATURE:
            // Get the reference count
            uRefCnt = ((LPVARARRAY_HEADER) lpMemHdr)->RefCnt;

            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbArg); lpMemHdr = NULL;

    return uRefCnt;
} // End GetRefCntGlb


#ifdef DEBUG_REFCNT
//***************************************************************************
//  $_DbgIncrRefCntDir_PTB
//
//  Increment the reference count of a direct reference
//    to an LPSYMENTRY or an HGLOBAL.
//***************************************************************************

int _DbgIncrRefCntDir_PTB
    (HGLOBAL hGlbData,
     LPWCHAR lpwFmtStr,
     LPSTR   lpFileName,
     UINT    uLineNum)

{
    dprintfWL0 (lpwFmtStr, ClrPtrTypeDir (hGlbData), lpFileName, uLineNum);
    return IncrRefCntDir_PTB (hGlbData);
} // End _DbgIncrRefCntDir_PTB
#endif


#ifdef DEBUG_REFCNT
//***************************************************************************
//  $_DbgIncrRefCntInd_PTB
//
//  Increment the reference count of an indirect reference
//    to an LPSYMENTRY or an HGLOBAL.
//***************************************************************************

int _DbgIncrRefCntInd_PTB
    (HGLOBAL hGlbData,
     LPWCHAR lpwFmtStr,
     LPSTR   lpFileName,
     UINT    uLineNum)

{
    dprintfWL0 (lpwFmtStr, ClrPtrTypeInd (hGlbData), lpFileName, uLineNum);
    return IncrRefCntInd_PTB (hGlbData);
} // End _DbgIncrRefCntInd_PTB
#endif


#ifdef DEBUG_REFCNT
//***************************************************************************
//  $_DbgIncrRefCntTkn
//
//  Increment the reference count of a token
//***************************************************************************

int _DbgIncrRefCntTkn
    (LPTOKEN lptkVar,
     LPWCHAR lpwFmtStr,
     LPSTR   lpFileName,
     UINT    uLineNum)

{
    dprintfWL0 (lpwFmtStr, ClrPtrTypeDir (GetGlbHandle (lptkVar)), lpFileName, uLineNum);
    return IncrRefCntTkn (lptkVar);
} // End _DbgIncrRefCntTkn
#endif


#ifdef DEBUG_REFCNT
//***************************************************************************
//  $_DbgDecrRefCntDir_PTB
//
//  Decrement the reference count of a direct reference
//    to an LPSYMENTRY or an HGLOBAL.
//***************************************************************************

int _DbgDecrRefCntDir_PTB
    (HGLOBAL hGlbData,
     LPWCHAR lpwFmtStr,
     LPSTR   lpFileName,
     UINT    uLineNum)

{
    dprintfWL0 (lpwFmtStr, ClrPtrTypeDir (hGlbData), lpFileName, uLineNum);
    return DecrRefCntDir_PTB (hGlbData);
} // End _DbgDecrRefCntDir_PTB
#endif


#ifdef DEBUG_REFCNT
//***************************************************************************
//  $_DbgDecrRefCntInd_PTB
//
//  Decrement the reference count of an indirect reference
//    to an LPSYMENTRY or an HGLOBAL.
//***************************************************************************

int _DbgDecrRefCntInd_PTB
    (HGLOBAL hGlbData,
     LPWCHAR lpwFmtStr,
     LPSTR   lpFileName,
     UINT    uLineNum)

{
    dprintfWL0 (lpwFmtStr, ClrPtrTypeInd (hGlbData), lpFileName, uLineNum);
    return DecrRefCntInd_PTB (hGlbData);
} // End _DbgDecrRefCntInd_PTB
#endif


#ifdef DEBUG_REFCNT
//***************************************************************************
//  $_DbgDecrRefCntTkn
//
//  Decrement the reference count of a token
//***************************************************************************

int _DbgDecrRefCntTkn
    (LPTOKEN lptkVar,
     LPWCHAR lpwFmtStr,
     LPSTR   lpFileName,
     UINT    uLineNum)

{
    dprintfWL0 (lpwFmtStr, ClrPtrTypeDir (GetGlbHandle (lptkVar)), lpFileName, uLineNum);
    return DecrRefCntTkn (lptkVar);
} // End _DbgDecrRefCntTkn
#endif


//***************************************************************************
//  End of File: refcnt.c
//***************************************************************************
