//***************************************************************************
//  NARS2000 -- Reference Count Routines
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
                DbgBrk ();      // #ifdef DEBUG

            if (hGlb && hGlbRC2 && ClrPtrTypeDir (hGlb) EQ ClrPtrTypeDir (hGlbRC2))
                DbgBrk ();      // #ifdef DEBUG
#endif
            // Lock the memory to get a ptr to it
#ifdef DEBUG
            vfoHdrPtrs.lpMemVar =
#endif
            // Lock the memory to get a ptr to it
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

                        break;
                    } // End IF

                    // Change the reference count
#ifdef DEBUG_REFCNT
                    if (iIncr EQ 1)
                        dprintfWL0 (L"  RefCnt++ in " APPEND_NAME L":     %p(res=%d) (%S#%d)", hGlb, lpHeader->RefCnt + iIncr, FNLN);
                    else
                    if (iIncr EQ -1)
                        dprintfWL0 (L"  RefCnt-- in " APPEND_NAME L":     %p(res=%d) (%S#%d)", hGlb, lpHeader->RefCnt + iIncr, FNLN);
                    else
                        DbgStop ();
#endif
                    Assert (iIncr NE -1 || lpHeader->RefCnt NE 0);

                    // Change the reference count
                    lpHeader->RefCnt += iIncr;

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
                    if (iIncr EQ 1)
                        dprintfWL0 (L"  RefCnt++ in " APPEND_NAME L":     %p(res=%d) (%S#%d)", hGlb, lpHeader->RefCnt + iIncr, FNLN);
                    else
                    if (iIncr EQ -1)
                        dprintfWL0 (L"  RefCnt-- in " APPEND_NAME L":     %p(res=%d) (%S#%d)", hGlb, lpHeader->RefCnt + iIncr, FNLN);
                    else
                        DbgStop ();
#endif
                    Assert (iIncr NE -1 || lpHeader->RefCnt NE 0);

                    // Change the reference count
                    lpHeader->RefCnt += iIncr;

                    RefCnt = lpHeader->RefCnt;
#undef  lpHeader
                    break;

                case DFN_HEADER_SIGNATURE:
#ifdef DEBUG
  #define lpHeader      vfoHdrPtrs.lpMemDfn
#else
  #define lpHeader      ((LPDFN_HEADER) lpSig)
#endif

                    // Change the reference count
#ifdef DEBUG_REFCNT
                    if (iIncr EQ 1)
                        dprintfWL0 (L"  RefCnt++ in " APPEND_NAME L":     %p(res=%d) (%S#%d)", hGlb, lpHeader->RefCnt + iIncr, FNLN);
                    else
                    if (iIncr EQ -1)
                        dprintfWL0 (L"  RefCnt-- in " APPEND_NAME L":     %p(res=%d) (%S#%d)", hGlb, lpHeader->RefCnt + iIncr, FNLN);
                    else
                        DbgStop ();
#endif
                    Assert (iIncr NE -1 || lpHeader->RefCnt NE 0);

                    // Change the reference count
                    lpHeader->RefCnt += iIncr;

                    RefCnt = lpHeader->RefCnt;
#undef  lpHeader
                    break;

                defstop
                    return -1;
            } // End SWITCH

            // We no longer need this ptr
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
    // If the global memory handle is that of a function array, ...
    if (IsGlbFcnArray (hGlb))
        return ChangeRefCntFcnArray (hGlb, 1);
    else
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
    HGLOBAL hGlb = MakeGlbFromPtr (lpMem);

    // If the global memory handle is that of a function array, ...
    if (IsGlbFcnArray (hGlb))
        return ChangeRefCntFcnArray (hGlb, 1);
    else
        return ChangeRefCntDir_PTB (hGlb, 1);
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
    // If the global memory handle is that of a function array, ...
    if (IsGlbFcnArray (hGlb))
        return ChangeRefCntFcnArray (hGlb, -1);
    else
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
    HGLOBAL hGlb = MakeGlbFromPtr (lpMem);

    // If the global memory handle is that of a function array, ...
    if (IsGlbFcnArray (hGlb))
        return ChangeRefCntFcnArray (hGlb, -1);
    else
        return ChangeRefCntDir_PTB (hGlb, -1);
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

    // If the token is a named direct fcn/opr, ...
    if (IsTknNamedFcnOpr (lptkVar)
     && lptkVar->tkData.tkSym->stFlags.FcnDir)
        return -1;

    // Get the global memory handle (if it's a global)
    hGlbVar = GetGlbHandle (lptkVar);

    // If it's a global, ...
    if (hGlbVar)
    {
        // If the global memory handle is that of a function array, ...
        if (IsGlbFcnArray (hGlbVar))
            return ChangeRefCntFcnArray (hGlbVar, 1);
        else
            return ChangeRefCntDir_PTB (MakePtrTypeGlb (hGlbVar), 1);
    } else
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

    // If the token is a named direct fcn/opr, ...
    if (IsTknNamedFcnOpr (lptkVar)
     && lptkVar->tkData.tkSym->stFlags.FcnDir)
        return -1;

    // Get the global memory handle (if it's a global)
    hGlbVar = GetGlbHandle (lptkVar);

    // If it's a global, ...
    if (hGlbVar)
    {
        // If the global memory handle is that of a function array, ...
        if (IsGlbFcnArray (hGlbVar))
            return ChangeRefCntFcnArray (hGlbVar, -1);
        else
            return ChangeRefCntDir_PTB (MakePtrTypeGlb (hGlbVar), -1);
    } else
        return -1;
} // End DecrRefCntTkn


//***************************************************************************
//  $ChangeRefCntTkn
//
//  Incr/Decrement the reference count of a token
//***************************************************************************

int ChangeRefCntTkn
    (LPTOKEN lptkVar,
     int     iChangeRefCnt)

{
    HGLOBAL hGlbVar;

    // If the token is a named direct fcn/opr, ...
    if (IsTknNamedFcnOpr (lptkVar)
     && lptkVar->tkData.tkSym->stFlags.FcnDir)
        return -1;

    // Get the global memory handle (if it's a global)
    hGlbVar = GetGlbHandle (lptkVar);

    // If it's a global, ...
    if (hGlbVar)
    {
        // If the global memory handle is that of a function array, ...
        if (IsGlbFcnArray (hGlbVar))
            return ChangeRefCntFcnArray (hGlbVar, iChangeRefCnt);
        else
            return ChangeRefCntDir_PTB (MakePtrTypeGlb (hGlbVar), iChangeRefCnt);
    } else
        return -1;
} // End ChangeRefCntTkn


//***************************************************************************
//  $IncrRefCntFcnArray
//
//  Increment the reference count of each item in a Function Array
//***************************************************************************

int IncrRefCntFcnArray
    (HGLOBAL hGlbFcn)

{
    return ChangeRefCntFcnArray (hGlbFcn, 1);
} // End IncrRefCntFcnArray


//***************************************************************************
//  $DecrRefCntFcnArray
//
//  Decrement the reference count of each item in a Function Array
//***************************************************************************

int DecrRefCntFcnArray
    (HGLOBAL hGlbFcn)

{
    return ChangeRefCntFcnArray (hGlbFcn, -1);
} // End DecrRefCntFcnArray


//***************************************************************************
//  $ChangeRefCntFcnArray
//
//  Incr/Decrement the reference count of each item in a Function Array
//***************************************************************************

int ChangeRefCntFcnArray
    (HGLOBAL hGlbFcn,
     int     iChangeRefCnt)

{
    LPFCNARRAY_HEADER lpMemHdr;     // Ptr to Function Array header
    UINT              tknNELM,      // The # tokens in the function array
                      u,            // Loop counter
                      RefCnt;       // Reference count
    LPPL_YYSTYPE      lpYYToken;    // Ptr to function array token

    // Data is an valid HGLOBAL function array
    Assert (IsGlbTypeFcnDir_PTB (hGlbFcn));

#ifdef DEBUG
    if (hGlbFcn && hGlbRC1 && ClrPtrTypeDir (hGlbFcn) EQ ClrPtrTypeDir (hGlbRC1))
        DbgBrk ();      // #ifdef DEBUG

    if (hGlbFcn && hGlbRC2 && ClrPtrTypeDir (hGlbFcn) EQ ClrPtrTypeDir (hGlbRC2))
        DbgBrk ();      // #ifdef DEBUG
#endif
    // Lock the memory to get a ptr to it
    lpMemHdr = MyGlobalLockFcn (hGlbFcn);

    // Get the Type, RefCnt, NELM, and line text handle
    lpMemHdr->RefCnt += iChangeRefCnt;
    RefCnt      = lpMemHdr->RefCnt;
    tknNELM     = lpMemHdr->tknNELM;

    // Skip over the header to the data (PL_YYSTYPEs)
    lpYYToken = FcnArrayBaseToData (lpMemHdr);

    // Loop through the PL_YYSTYPEs
    for (u = 0; u < tknNELM; u++, lpYYToken++)
    // Split cases based upon the token type
    switch (lpYYToken->tkToken.tkFlags.TknType)
    {
        case TKT_FCNIMMED:      // Nothing to do
        case TKT_VARIMMED:      // ...
        case TKT_OP1IMMED:      // ...
        case TKT_OP2IMMED:      // ...
        case TKT_OP3IMMED:      // ...
        case TKT_AXISIMMED:     // ...
        case TKT_OPJOTDOT:      // ...
        case TKT_FILLJOT:       // ...
            break;              // Ignore immediates

        case TKT_FCNNAMED:      // Increment the RefCnt in the named function array
        case TKT_OP1NAMED:      // ...
        case TKT_OP2NAMED:      // ...
        case TKT_OP3NAMED:      // ...
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

            // Check for internal functions
            if (lpYYToken->tkToken.tkData.tkSym->stFlags.FcnDir)
                break;          // Ignore internal functions

            // Increment the RefCnt
            DbgChangeRefCntTkn (&lpYYToken->tkToken, iChangeRefCnt);

            break;

        case TKT_FCNARRAY:      // Increment the RefCnt in the the function array
            // This DEBUG stmt probably never is triggered because
            //    YYCopyFcnStr converts all Function Arrays to items
#ifdef DEBUG
            DbgStop ();         // ***Probably never executed***
#endif
            // Increment the RefCnt
            DbgChangeRefCntTkn (&lpYYToken->tkToken, iChangeRefCnt); // EXAMPLE:  ***Probably never executed***

            break;

        case TKT_FCNAFO:
        case TKT_OP1AFO:
        case TKT_OP2AFO:
        case TKT_DELAFO:
            // Increment the RefCnt
            DbgChangeRefCntTkn (&lpYYToken->tkToken, iChangeRefCnt); // EXAMPLE:  f{is}{-omega}{jot}{divide} {diamond} f 3

            break;

        case TKT_VARARRAY:      // Increment the var array (strand arg to dyadic op)
        case TKT_AXISARRAY:     // ...           axis array
        case TKT_CHRSTRAND:     // ...           character strand
        case TKT_NUMSTRAND:     // ...           numeric strand
        case TKT_NUMSCALAR:     // ...           numeric scalar
            // Increment the RefCnt
            DbgChangeRefCntTkn (&lpYYToken->tkToken, iChangeRefCnt); // EXAMPLE:  A f {each} [1 2] B

            break;

        case TKT_VARNAMED:
            // This DEBUG stmt probably never is triggered because
            //    pl_yylex converts all unassigned named vars to temps
#ifdef DEBUG
            DbgStop ();         // ***Probably never executed***
#endif
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpYYToken->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not an immediate, ...
            if (!lpYYToken->tkToken.tkData.tkSym->stFlags.Imm)
                // Increment the RefCnt
                DbgChangeRefCntTkn (&lpYYToken->tkToken, iChangeRefCnt);

            break;

        defstop
            break;
    } // End FOR/SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbFcn); lpMemHdr = NULL;

    return RefCnt;
} // End ChangeRefCntFcnArray


//***************************************************************************
//  $GetRefCntGlb
//
//  Get the reference count from a global memory handle
//***************************************************************************

UINT GetRefCntGlb
    (HGLOBAL hGlbArg)           // Arg global memory handle

{
    VFOHDRPTRS vfoHdrPtrs;      // Ptr to global memory header
    UINT       uRefCnt;         // The array reference count

    // Lock the memory to get a ptr to it
    vfoHdrPtrs.lpMemVFO = MyGlobalLock (hGlbArg);

    // Split cases based upon the signature
    switch (GetSignatureMem (vfoHdrPtrs.lpMemVFO))
    {
        case DFN_HEADER_SIGNATURE:
            // Get the reference count
            uRefCnt = vfoHdrPtrs.lpMemDfn->RefCnt;

            break;

        case FCNARRAY_HEADER_SIGNATURE:
            // Get the reference count
            uRefCnt = vfoHdrPtrs.lpMemFcn->RefCnt;

            break;

        case VARARRAY_HEADER_SIGNATURE:
            // Get the reference count
            uRefCnt = vfoHdrPtrs.lpMemVar->RefCnt;

            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbArg); vfoHdrPtrs.lpMemVFO = NULL;

    return uRefCnt;
} // End GetRefCntGlb


//***************************************************************************
//  $SetRefCntGlb
//
//  Set the reference count from a global memory handle to a given value
//***************************************************************************

void SetRefCntGlb
    (HGLOBAL hGlbArg,           // Arg global memory handle
     UINT    uRefCnt)           // New value for RefCnt (typically 1)

{
    VFOHDRPTRS vfoHdrPtrs;      // Ptr to global memory header

    // Lock the memory to get a ptr to it
    vfoHdrPtrs.lpMemVFO = MyGlobalLock (hGlbArg);

    // Split cases based upon the signature
    switch (GetSignatureMem (vfoHdrPtrs.lpMemVFO))
    {
        case DFN_HEADER_SIGNATURE:
            // Set the reference count
            vfoHdrPtrs.lpMemDfn->RefCnt = uRefCnt;

            break;

        case FCNARRAY_HEADER_SIGNATURE:
            // Set the reference count
            vfoHdrPtrs.lpMemFcn->RefCnt = uRefCnt;

            break;

        case VARARRAY_HEADER_SIGNATURE:
            // Set the reference count
            vfoHdrPtrs.lpMemVar->RefCnt = uRefCnt;

            break;

        defstop
            break;
    } // End SWITCH

    // We no longer need this ptr
    MyGlobalUnlock (hGlbArg); vfoHdrPtrs.lpMemVFO = NULL;
} // End SetRefCntGlb


#ifdef DEBUG_REFCNT
//***************************************************************************
//  $DbgIncrRefCntDir_PTB
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
//  $DbgIncrRefCntInd_PTB
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
//  $DbgIncrRefCntTkn
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
//  $DbgChangeRefCntTkn
//
//  Increment the reference count of a token
//***************************************************************************

int _DbgChangeRefCntTkn
    (LPTOKEN lptkVar,
     int     iChangeRefCnt,
     LPWCHAR lpwFmtStr,
     LPSTR   lpFileName,
     UINT    uLineNum)

{
    dprintfWL0 (lpwFmtStr, ClrPtrTypeDir (GetGlbHandle (lptkVar)), iChangeRefCnt, lpFileName, uLineNum);
    return ChangeRefCntTkn (lptkVar, iChangeRefCnt);
} // End _DbgChangeRefCntTkn
#endif


#ifdef DEBUG_REFCNT
//***************************************************************************
//  $DbgIncrRefCntFcnArray
//
//  Increment the reference count of a Function Array
//***************************************************************************

int _DbgIncrRefCntFcnArray
    (HGLOBAL hGlbFcn,
     LPWCHAR lpwFmtStr,
     LPSTR   lpFileName,
     UINT    uLineNum)

{
    dprintfWL0 (lpwFmtStr, ClrPtrTypeDir (hGlbFcn), lpFileName, uLineNum);
    return IncrRefCntFcnArray (hGlbFcn);
} // End _DbgIncrRefCntFcnArray
#endif


#ifdef DEBUG_REFCNT
//***************************************************************************
//  $DbgDecrRefCntFcnArray
//
//  Decrement the reference count of a Function Array
//***************************************************************************

int _DbgDecrRefCntFcnArray
    (HGLOBAL hGlbFcn,
     LPWCHAR lpwFmtStr,
     LPSTR   lpFileName,
     UINT    uLineNum)

{
    dprintfWL0 (lpwFmtStr, ClrPtrTypeDir (hGlbFcn), lpFileName, uLineNum);
    return DecrRefCntFcnArray (hGlbFcn);
} // End _DbgDecrRefCntFcnArray
#endif


#ifdef DEBUG_REFCNT
//***************************************************************************
//  $DbgDecrRefCntDir_PTB
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
//  $DbgDecrRefCntInd_PTB
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
//  $DbgDecrRefCntTkn
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
