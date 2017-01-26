//***************************************************************************
//  NARS2000 -- System Function -- Quad ET
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


//***************************************************************************
//  $SysFnET_EM_YY
//
//  System function:  []ET -- Event Type
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- SysFnET_EM_YY"
#else
#define APPEND_NAME
#endif

LPPL_YYSTYPE SysFnET_EM_YY
    (LPTOKEN lptkLftArg,            // Ptr to left arg token (should be NULL)
     LPTOKEN lptkFunc,              // Ptr to function token
     LPTOKEN lptkRhtArg,            // Ptr to right arg token  (should be NULL)
     LPTOKEN lptkAxis)              // Ptr to axis token (may be NULL)

{
    LPPL_YYSTYPE lpYYRes;           // Ptr to the result
    HGLOBAL      hGlbRes = NULL;    // Result ...
    LPAPLUINT    lpMemRes = NULL;   // Ptr to result global memory
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPSIS_HEADER lpSISCur;          // Ptr to current SIS header
    EVENT_TYPES  EventType;         // Event type
    APLUINT      ByteRes;           // # bytes in the result

    // This function is niladic
    Assert (lptkLftArg EQ NULL && lptkRhtArg EQ NULL);

    // Niladic functions cannot have axis operator
    Assert (lptkAxis EQ NULL);

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get ptr to current SIS header
    lpSISCur = lpMemPTD->lpSISCur;

    while (lpSISCur
        && lpSISCur->DfnType NE DFNTYPE_FCN
        && lpSISCur->DfnType NE DFNTYPE_OP1
        && lpSISCur->DfnType NE DFNTYPE_OP2
        && lpSISCur->DfnType NE DFNTYPE_ERRCTRL)
        lpSISCur = lpSISCur->lpSISPrv;

    if (lpSISCur)
        EventType = lpSISCur->EventType;
    else
        EventType = EVENTTYPE_NOERROR;

    // Calculate space needed for the result
    ByteRes = CalcArraySize (ARRAY_INT, 2, 1);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock000 (hGlbRes);

#define lpHeader        ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_INT;
////lpHeader->PermNdx    = PERMNDX_NONE;    // Already zero from GHND
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = 2;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Skip over the header to the dimension
    lpMemRes = VarArrayBaseToDim (lpMemRes);

    // Fill in the result's dimension
    *((LPAPLDIM) lpMemRes)++ = 2;

    // lpMemRes now points to the data

    *lpMemRes++ = ET_MAJOR (EventType);
    *lpMemRes++ = ET_MINOR (EventType);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;

    // Allocate a YYRes
    lpYYRes = YYAlloc ();

    // Fill in the result token
    lpYYRes->tkToken.tkFlags.TknType   = TKT_VARARRAY;
////lpYYRes->tkToken.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from YYAlloc
////lpYYRes->tkToken.tkFlags.NoDisplay = FALSE;         // Already zero from YYAlloc
    lpYYRes->tkToken.tkData.tkGlbData  = MakePtrTypeGlb (hGlbRes);
    lpYYRes->tkToken.tkCharIndex       = lptkFunc->tkCharIndex;

    return lpYYRes;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkFunc);
    return NULL;
} // End SysFnET_EM_YY
#undef  APPEND_NAME


//***************************************************************************
//  $SetEventTypeMessage
//
//  Set the event type and message
//***************************************************************************

void SetEventTypeMessage
    (EVENT_TYPES EventType,         // Event type (see EVENT_TYPES)
     LPAPLCHAR   lpMemMsg,          // Ptr to event message (may be NULL)
     LPTOKEN     lptkFunc)          // Ptr to function token

{
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPSIS_HEADER lpSISCur;          // Ptr to current SIS header

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get ptr to current SIS header
    lpSISCur = lpMemPTD->lpSISCur;

    // If there's a []EA/[]EC parent in control, ...
    if (lpSISCur->lpSISErrCtrl NE NULL)
        // Get ptr to current []EA/[]EC parent of the current SI stack
        lpSISCur = lpSISCur->lpSISErrCtrl;
    else
        while (lpSISCur
            && lpSISCur->DfnType NE DFNTYPE_FCN
            && lpSISCur->DfnType NE DFNTYPE_OP1
            && lpSISCur->DfnType NE DFNTYPE_OP2
            && lpSISCur->DfnType NE DFNTYPE_ERRCTRL)
            lpSISCur = lpSISCur->lpSISPrv;

    // If there's a message, ...
    if (lpMemMsg)
    {
        ErrorMessageIndirectToken (lpMemMsg, lptkFunc);
        EventType = EVENTTYPE_UNK;
    } // End IF

    // If there's an SIS level, ...
    if (lpSISCur)
        lpSISCur->EventType = EventType;
    else
        lpMemPTD->EventType = EventType;
} // SetEventTypeMessage


//***************************************************************************
//  End of File: qf_et.c
//***************************************************************************
