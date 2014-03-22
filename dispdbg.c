//***************************************************************************
//  NARS2000 -- Display Debug Routines
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

#undef  inline
#define inline

#define STRICT
#include <windows.h>
#include "headers.h"

#ifdef DEBUG
// Resource debugging variables
extern HANDLE ahGLBALLOC [MAXOBJ];
extern HANDLE ahSEMAPHORE[MAXOBJ];
extern HANDLE ahHEAPALLOC[MAXOBJ];

extern UINT auLinNumGLBALLOC [MAXOBJ];
extern UINT auLinNumSEMAPHORE[MAXOBJ];
extern UINT auLinNumHEAPALLOC[MAXOBJ];

extern LPCHAR lpaFileNameGLBALLOC[MAXOBJ];
extern LPCHAR lpaFileNameSEMAPHORE[MAXOBJ];
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayHshTab
//
//  Display the Hash Table entries
//***************************************************************************

void DisplayHshTab
    (LPHSHTABSTR lpHTS)             // Ptr to hshTab struc

{
    LPHSHENTRY lpHshEntry;          // Ptr to current HshTab entry
    int        i;                   // Loop counter
    WCHAR      wszTemp[1024];       // Ptr to temporary output area

    DbgMsgW (L"********** Start Hash Table ****************************");

    wsprintfW (wszTemp,
               L"lpHshTab = %p, SplitNext = %p, Last = %p",
               lpHTS->lpHshTab,
               lpHTS->lpHshTabSplitNext,
              &lpHTS->lpHshTab[lpHTS->iHshTabTotalNelm]);
    DbgMsgW (wszTemp);

    // Display the hash table
    for (lpHshEntry = lpHTS->lpHshTab, i = 0;
         i < lpHTS->iHshTabTotalNelm;
         lpHshEntry++, i++)
    {
        // Format the HTE
        FormatHTE (lpHshEntry, wszTemp, i);

        DbgMsgW (wszTemp);
    } // End FOR

    DbgMsgW (L"********** End Hash Table ******************************");

    UpdateDBWindow ();
} // End DisplayHshTab
#endif


//***************************************************************************
//  $FormatHTE
//
//  Format a HTE
//***************************************************************************

void FormatHTE
    (LPHSHENTRY lpHshEntry,
     LPWCHAR    wszTemp,
     UINT       i)

{
    WCHAR wszFlags[128] = {WC_EOS};
    UINT  htFlags,
          j;

    typedef struct tagHT_FLAGNAMES
    {
        UINT    uMask;
        LPWCHAR lpwszName;
    } HT_FLAGNAMES, *LPHT_FLAGNAMES;

    // Hash Table flag names
    static HT_FLAGNAMES ahtFlagNames[] =
    {
     {0x00001,  L" Inuse"      },
     {0x00002,  L" PrinHash"   },
     {0x00004,  L" CharIsValid"},
     {0x00008,  L" Temp"       },
    };

// The # rows in the above table
#define HT_FLAGNAMES_NROWS  countof (ahtFlagNames)

    // Check for invalid HshEntry
    if (lpHshEntry EQ NULL)
    {
        wsprintfW (wszTemp,
                   L"HT:%3d ***INVALID HSHENTRY (NULL)***",
                   i);
        return;
    } // End IF

    // Format the flags
    htFlags = *(LPUINT) &lpHshEntry->htFlags;
    for (j = 0;
         j < HT_FLAGNAMES_NROWS;
         j++)
    if (htFlags & ahtFlagNames[j].uMask)
        lstrcatW (wszFlags, ahtFlagNames[j].lpwszName);

    // In case we didn't find any matching flags,
    //   set the second byte to zero as well as
    //   when we do find flags, we skip over the
    //   leading blank.
    if (wszFlags[0] EQ WC_EOS)
        wszFlags[1] =  WC_EOS;

    if (lpHshEntry->htFlags.Inuse
     && lpHshEntry->htSymEntry)
    {
        LPSYMENTRY lpSymEntry;

        lpSymEntry = lpHshEntry->htSymEntry;
        if (lpSymEntry->stFlags.Imm)
            wsprintfW (wszTemp,
                       L"HT:%3d uH=%08X, uH&M=%d, <%s>, ull=%I64X, Sym=%p",
                       i,
                       lpHshEntry->uHash,
                       lpHshEntry->uHashAndMask,
                      &wszFlags[1],
                       lpSymEntry->stData.stInteger,
                       lpSymEntry);
        else
        if (lpSymEntry->stFlags.ObjName NE OBJNAME_NONE)
        {
            LPWCHAR lpwGlbName;

            // Lock the memory to get a ptr to it
            lpwGlbName = GlobalLock (lpHshEntry->htGlbName); Assert (lpwGlbName NE NULL);

            wsprintfW (wszTemp,
                       L"HT:%3d uH=%08X, uH&M=%d, <%s>, <%s>, Sym=%p, %p-%p",
                       i,
                       lpHshEntry->uHash,
                       lpHshEntry->uHashAndMask,
                      &wszFlags[1],
                       lpwGlbName,
                       lpSymEntry,
                       lpHshEntry->NextSameHash,
                       lpHshEntry->PrevSameHash);
            // We no longer need this ptr
            GlobalUnlock (lpHshEntry->htGlbName); lpwGlbName = NULL;
        } // End IF/ELSE/IF
    } else
        wsprintfW (wszTemp,
                   L"HT:%3d (EMPTY) <%s>, Sym=%p, <%p-%p>",
                   i,
                  &wszFlags[1],
                   lpHshEntry->htSymEntry,
                   lpHshEntry->NextSameHash,
                   lpHshEntry->PrevSameHash);
} // End FormatHTE


#ifdef DEBUG
//***************************************************************************
//  $DisplaySymTab
//
//  Display the Symbol Table entries
//
//  If bDispAll is FALSE, display only those non-OBJNAME_SYS entries
//    with a non-zero reference count.
//***************************************************************************

void DisplaySymTab
    (LPHSHTABSTR lpHTS,             // Ptr to HshTab struc
     UBOOL       bDispAll)          // TRUE iff we're to display the entire SymTab

{
    LPSYMENTRY   lpSymEntry;        // Ptr to current SYMENTRY
    int          i;                 // Loop counter
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    WCHAR        wszTemp[1024];     // Ptr to temporary output area

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    if (bDispAll)
    {
        DbgMsgW (L"********** Start Symbol Table **************************");
    } else
    {
        DbgMsgW (L"********** Symbol Table Referenced Non-SysNames ********");
    } // End IF/ELSE -- DO NOT REMOVE as the DbgMsgW macro needs
    //                  this because of the trailing semicolon

    wsprintfW (wszTemp,
               L"lpSymTab = %p, Last = %p",
               lpHTS->lpSymTab,
              &lpHTS->lpSymTab[lpHTS->iSymTabTotalNelm]);
    DbgMsgW (wszTemp);

    // Display the symbol table
    for (lpSymEntry = lpHTS->lpSymTab, i = 0;
         lpSymEntry NE lpHTS->lpSymTabNext;
         lpSymEntry++, i++)
    if (bDispAll ||
        lpSymEntry->stFlags.ObjName NE OBJNAME_SYS)
    {
        // Format the STE
        FormatSTE (lpSymEntry, wszTemp);

        DbgMsgW (wszTemp);
    } // End FOR

    DbgMsgW (L"********** End Symbol Table ****************************");

    UpdateDBWindow ();
} // End DisplaySymTab
#endif


//***************************************************************************
//  $FormatSTE
//
//  Format a STE
//***************************************************************************

void FormatSTE
    (LPSYMENTRY lpSymEntry,             // Ptr to the SYMENTRY to format
     LPWCHAR    wszTemp)                // Ptr to output save area

{
    WCHAR   wszFlags[128] = {WC_EOS};
    STFLAGS stFlags;
    LPWCHAR lpwGlbName;
    int     j;                  // Loop counter

    typedef struct tagST_FLAGNAMES
    {
        UINT    uMask;
        LPWCHAR lpwszName;
    } ST_FLAGNAMES, *LPST_FLAGNAMES;

    // Symbol Table flag names
    static ST_FLAGNAMES astFlagNames[] =
    {
//// {0x00000001,  L" Imm"        },
//// {0x0000001E,  L" ImmType"    },
     {0x00000020,  L" Inuse"      },
     {0x00000040,  L" Value"      },
/////{0x00000380,  L" ObjName"    },
//// {0x00003C00,  L" stNameType" },
//// {0x0007C000,  L" SysVarValid"},
     {0x00080000,  L" UsrDfn"     },
     {0x00100000,  L" DfnLabel"   },
     {0x00200000,  L" DfnSysLabel"},
     {0x00400000,  L" DfnAxis"    },
     {0x00800000,  L" FcnDir"     },
     {0x01000000,  L" StdSysName" },
     {0x02000000,  L" bIsAlpha"   },
     {0x04000000,  L" bIsOmega"   },
    };

// The # rows in the above table
#define ST_FLAGNAMES_NROWS  countof (astFlagNames)

    // Format the flags
    stFlags = lpSymEntry->stFlags;
    if (stFlags.Imm)
        wsprintfW (&wszFlags[lstrlenW (wszFlags)],
                    L" Imm/Type=%d",
                    stFlags.ImmType);
    if (stFlags.ObjName NE OBJNAME_NONE)
        wsprintfW (&wszFlags[lstrlenW (wszFlags)],
                    L" ObjName=%s",
                    lpwObjNameStr[stFlags.ObjName]);
    if (stFlags.stNameType NE NAMETYPE_UNK)
        wsprintfW (&wszFlags[lstrlenW (wszFlags)],
                    L" stNameType=%s",
                    lpwNameTypeStr[stFlags.stNameType]);

    for (j = 0;
         j < ST_FLAGNAMES_NROWS;
         j++)
    if ((*(UINT *) &stFlags) & astFlagNames[j].uMask)
        lstrcatW (wszFlags, astFlagNames[j].lpwszName);

    if (IsNameTypeVar (stFlags.stNameType)
     && !stFlags.DfnSysLabel)
        wsprintfW (&wszFlags[lstrlenW (wszFlags)],
                    L" SysVarValid=%d",
                    stFlags.SysVarValid);

    // In case we didn't find any matching flags,
    //   set the second WCHAR to zero as well --
    //   when we do find flags, we skip over the
    //   leading blank.
    if (wszFlags[0] EQ WC_EOS)
        wszFlags[1] =  WC_EOS;

    if (lpSymEntry->stFlags.Inuse)
    {
#define WSZNAME_LEN     128
        WCHAR wszName[WSZNAME_LEN] = {'\0'};
        LPSYMENTRY lpPrvEntry;

        if (lpSymEntry->stFlags.ObjName NE OBJNAME_NONE)
        {
            LPHSHENTRY lpHshEntry;

            lpHshEntry = lpSymEntry->stHshEntry;

            if (lpHshEntry)
            {
                Assert (lpHshEntry->htGlbName NE NULL);

                lpwGlbName = GlobalLock (lpHshEntry->htGlbName); Assert (lpwGlbName NE NULL);

                lstrcpynW (wszName, lpwGlbName, WSZNAME_LEN);

                // We no longer need this ptr
                GlobalUnlock (lpHshEntry->htGlbName); lpwGlbName = NULL;
            } // End IF
        } // End IF

        // Get the previous (shadowed) entry (if any)
        lpPrvEntry = lpSymEntry->stPrvEntry;

        if (lpSymEntry->stFlags.Imm)
        {
            wsprintfW (wszTemp,
                       L"ST:%p <%s> <%s>, ull=%I64X, Hsh=%p, Prv=%p",
                       lpSymEntry,
                       wszName,
                      &wszFlags[1],
                       lpSymEntry->stData.stInteger,
                       lpSymEntry->stHshEntry,
                       lpPrvEntry);
        } else
        if (lpSymEntry->stFlags.ObjName NE OBJNAME_NONE)
        {
            LPHSHENTRY lpHshEntry;

            lpHshEntry = lpSymEntry->stHshEntry;

            if (lpHshEntry)
            {
                wsprintfW (wszTemp,
                           L"ST:%p <%s>, <%s>, Data=%p, Hsh=%p, Prv=%p",
                           lpSymEntry,
                           wszName,
                          &wszFlags[1],
                           lpSymEntry->stData.stVoid,
                           lpHshEntry,
                           lpPrvEntry);
            } else
                wsprintfW (wszTemp,
                           L"ST:%p <******>, <%s>, Hsh=0, Prv=%p",
                           lpSymEntry,
                          &wszFlags[1],
                           lpPrvEntry);
        } // End IF/ELSE/IF
    } else
        wsprintfW (wszTemp,
                   L"ST:%p (EMPTY) <%s>, Hsh=%p, Prv=%p",
                   lpSymEntry,
                  &wszFlags[1],
                   lpSymEntry->stHshEntry,
                   lpSymEntry->stPrvEntry);
} // End FormatSTE


#ifdef DEBUG
//***************************************************************************
//  $UpdateDBWindow
//
//  Call UpdateWindow on the DB window
//***************************************************************************

void UpdateDBWindow
    (void)

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    UpdateWindow (lpMemPTD->hWndDB);
} // End UpdateDBWindow
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayGlobals
//
//  Display outstanding global memory objects
//***************************************************************************

void DisplayGlobals
    (UINT uDispGlb)     // 0 = Display non-permanent non-sysvars
                        // 1 = ...     non-sysvars
                        // 2 = ...     all globals

{
    int          i;                 // Loop counter
    HGLOBAL      hGlb;              // Current global memory handle
    LPVOID       lpMem;             // Ptr to current global memory (static header)
    APLDIM       aplDim;            // Dimension
    LPVOID       lpData;            // Ptr to global memory data (dynamic data)
    APLCHAR      aplArrChar[10024];
    LPAPLCHAR    lpaplChar,         // Ptr to output save area
                 lpwsz;
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    WCHAR        wszTemp[1024];     // Ptr to temporary output area
    IMM_TYPES    immType;           // Immediate type of the incoming array
    UBOOL        uValid;            // TRUE iff the value is valid

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    DbgMsgW (L"********** Start Globals *******************************");

#define MAX_VAL_LEN     12
    for (i = 0; i < MAXOBJ; i++)
    if (hGlb = ahGLBALLOC[i])
    {
        // Note we don't use MyGlobalLock here as that function might well
        //   fail and trigger a hard error which we prefer to catch softly.

        // Lock the memory to get a ptr to it
        lpMem = GlobalLock (hGlb);
        if (!lpMem)
        {
            wsprintfW (wszTemp,
                       L"hGlb=%p *** INVALID ***",
                       hGlb);
            DbgMsgW (wszTemp);

            continue;
        } // End IF

#define lpHeader    ((LPVARARRAY_HEADER) lpMem)
        // Split cases based upon the array signature
        switch (lpHeader->Sig.nature)
#undef  lpHeader
        {
            case VARARRAY_HEADER_SIGNATURE:
            {
                LPVARARRAY_HEADER lpHeader = lpMem;

                // Calc the immediate type
                immType = TranslateArrayTypeToImmType (lpHeader->ArrType);

                // If we're to display all globals or
                //   this one is not a sysvar, ...
                if (uDispGlb EQ 2
                 || !lpHeader->SysVar)
                {
                    // It's a valid HGLOBAL variable array
                    Assert (IsGlbTypeVarDir_PTB (MakePtrTypeGlb (hGlb)));

                    if (IsScalar (lpHeader->Rank))
                        aplDim = (APLDIM) -1;
                    else
                        aplDim = *VarArrayBaseToDim (lpHeader);
                    // Skip over the header and dimension to the data
                    lpData = VarArrayBaseToData (lpHeader, lpHeader->Rank);

#define MAXDISPLEN  7

                    // Split cases based upon the array storage type
                    switch (lpHeader->ArrType)
                    {
                        case ARRAY_BOOL:
                            if (IsEmpty (lpHeader->NELM))
                            {
                                aplArrChar[0] = UTF16_ZILDE;
                                aplArrChar[1] = WC_EOS;
                            } else
                            {
                                UINT       uArr,
                                           uBitMask = BIT0;
                                APLLONGEST aplZero = 0,
                                           aplOne  = 1;

                                for (lpwsz = aplArrChar, uArr = 0; uArr < min (MAXDISPLEN, lpHeader->NELM); uArr++)
                                {
                                    lpwsz =
                                      FormatImmed (lpwsz,
                                                   immType,
                                                   (LPAPLLONGEST) ((uBitMask & *(LPAPLBOOL) lpData)) ? &aplOne : &aplZero);
                                    uBitMask <<= 1;
                                } // End FOR

                                if (lpHeader->NELM > MAXDISPLEN)
                                {
                                    lpwsz[-1] = UTF16_HORIZELLIPSIS;
                                    *lpwsz++ = L' ';
                                } // End IF

                                // Delete the trailing blank
                                lpwsz[-1] = WC_EOS;
                            } // End IF/ELSE

                            break;

                        case ARRAY_INT:
                        case ARRAY_FLOAT:
                            if (IsEmpty (lpHeader->NELM))
                            {
                                aplArrChar[0] = UTF16_ZILDE;
                                aplArrChar[1] = WC_EOS;
                            } else
                            {
                                UINT uArr;

                                for (lpwsz = aplArrChar, uArr = 0; uArr < min (MAXDISPLEN, lpHeader->NELM); uArr++)
                                    lpwsz =
                                      FormatImmed (lpwsz,
                                                   immType,
                                                  &((LPAPLLONGEST) lpData)[uArr]);
                                if (lpHeader->NELM > MAXDISPLEN)
                                {
                                    lpwsz[-1] = UTF16_HORIZELLIPSIS;
                                    *lpwsz++ = L' ';
                                } // End IF

                                // Delete the trailing blank
                                lpwsz[-1] = WC_EOS;
                            } // End IF/ELSE

                            break;

                        case ARRAY_APA:
                        {
                            APLINT apaOff,
                                   apaMul;

                            // Get the APA parameters
                            apaOff = ((LPAPLAPA) lpData)->Off;
                            apaMul = ((LPAPLAPA) lpData)->Mul;

                            lpwsz = aplArrChar;

                            lpwsz =
                              FormatImmed (lpwsz,
                                           immType,
                                          &apaOff);
                            lpwsz[-1] = L'+';
                            lpwsz =
                              FormatImmed (lpwsz,
                                           immType,
                                          &apaMul);
                            lpwsz[-1] = UTF16_TIMES;
                            *lpwsz++  = UTF16_IOTA;
                            lpwsz =
                              FormatImmed (lpwsz,
                                           immType,
                                          &lpHeader->NELM);
                            lstrcpyW (lpwsz, $QUAD_IO WS_UTF16_LEFTARROW L"0");

                            break;
                        } // End ARRAY_APA

                        case ARRAY_HETERO:
                        case ARRAY_NESTED:
                            aplArrChar[0] = WC_EOS;

                            break;

                        case ARRAY_CHAR:
                            lstrcpynW (aplArrChar, lpData, 1 + (UINT) min (MAX_VAL_LEN, lpHeader->NELM));
                            aplArrChar[min (MAX_VAL_LEN, lpHeader->NELM)] = WC_EOS;

                            break;

                        case ARRAY_LIST:
                            lstrcpyW (aplArrChar, L"[...]");

                            break;

                        case ARRAY_RAT:
                            lpwsz =
                              FormatAplRat (aplArrChar,             // Ptr to output save area
                                           *(LPAPLRAT) lpData);     // The value to format
                            // Zap the trailing blank
                            lpwsz[-1] = WC_EOS;

                            break;

                        case ARRAY_VFP:
                            lpwsz =
                              FormatAplVfp (aplArrChar,             // Ptr to output save area
                                          *(LPAPLVFP) lpData,       // The value to format
                                            0);                     // Use this many significant digits for VFP
                            // Zap the trailing blank
                            lpwsz[-1] = WC_EOS;

                            break;

                        defstop
                            break;
                    } // End SWITCH
#undef  MAXDISPLEN

                    // Check for non-permanents
                    if (uDispGlb EQ 1
                     || uDispGlb EQ 2
                     || ((lpHeader->PermNdx EQ PERMNDX_NONE)
                  && (lpHeader->bMFOvar EQ FALSE)))
                    {
                        wsprintfW (wszTemp,
                                   L"%shGlb=%p AType=%c%c NELM=%3d RC=%2d Rnk=%2d Dim1=%3d Lck=%d (%S#%d) (%s)",
                                   (lpHeader->RefCnt NE 1 || lpHeader->SkipRefCntIncr) ? WS_UTF16_REFCNT_NE1 : L"",
                                   hGlb,
                                   ArrayTypeAsChar[lpHeader->ArrType],
                                   L" *"[lpHeader->PermNdx NE PERMNDX_NONE],
                                   LODWORD (lpHeader->NELM),
                                   lpHeader->RefCnt,
                                   LODWORD (lpHeader->Rank),
                                   LODWORD (aplDim),
                                   (MyGlobalFlags (hGlb) & GMEM_LOCKCOUNT) - 1,
                                   lpaFileNameGLBALLOC[i],
                                   auLinNumGLBALLOC[i],
                                   aplArrChar);
                        DbgMsgW (wszTemp);
                    } // End IF
                } // End IF

                break;
            } // End VARARRAY_HEADER_SIGNATURE

            case FCNARRAY_HEADER_SIGNATURE:
            {
                LPFCNARRAY_HEADER lpHeader = lpMem;

                // It's a valid HGLOBAL function array
                Assert (IsGlbTypeFcnDir_PTB (MakePtrTypeGlb (hGlb)));

                lpaplChar =
                  DisplayFcnGlb (&aplArrChar[1],    // Ptr to output save area
                                  hGlb,             // Function array global memory handle
                                  FALSE,            // TRUE iff we're to display the header
                                  NULL,             // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                                  NULL,             // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                                  NULL,             // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                                  NULL);            // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
                // Ensure properly terminated
                *lpaplChar = WC_EOS;

                // Surround with parens if not a Train
                //   (already surrounded by parens)
                if (lpHeader->fnNameType NE NAMETYPE_TRN)
                {
                    // Prepend a leading left paren
                    aplArrChar[0] = L'(';

                    // Append a trailing right paren
                    *lpaplChar++ = L')';

                    // Ensure properly terminated
                    *lpaplChar = WC_EOS;

                    // Point to leading left paren
                    lpaplChar = &aplArrChar[0];
                } else
                    // Point to leading left paren
                    lpaplChar = &aplArrChar[1];

                wsprintfW (wszTemp,
                           L"%shGlb=%p NType=%sNELM=%3d RC=%2d                 Lck=%d (%S#%4d) %s",
                           (lpHeader->RefCnt NE 1) ? WS_UTF16_REFCNT_NE1 : L"",
                           hGlb,
                           lpwNameTypeStr[lpHeader->fnNameType],
                           lpHeader->tknNELM,
                           lpHeader->RefCnt,
                           (MyGlobalFlags (hGlb) & GMEM_LOCKCOUNT) - 1,
                           lpaFileNameGLBALLOC[i],
                           auLinNumGLBALLOC[i],
                           lpaplChar);
                DbgMsgW (wszTemp);

                break;
            } // End FCNARRAY_HEADER_SIGNATURE

            case DFN_HEADER_SIGNATURE:
            {
                LPDFN_HEADER lpHeader = lpMem;      // Ptr to user-defined function/operator header
                APLUINT      uNameLen;              // User-defined function/operator name length

                // It's a valid HGLOBAL user-defined function/operator
                Assert (IsGlbTypeDfnDir_PTB (MakePtrTypeGlb (hGlb)));

                // Copy the user-defined function/operator name
                CopySteName (lpMemPTD->lpwszTemp,       // Ptr to global memory
                             lpHeader->steFcnName,      // Ptr to function symbol table entry
                            &uNameLen);                 // Ptr to name length (may be NULL)

                // If we're to display all globals or
                //   this one is not a Magic Function/Operator, ...
                if (uDispGlb EQ 2
                 || (!IsMFOName (lpMemPTD->lpwszTemp)
                  && !lpHeader->bMFO))
                {
                    // Copy the name to local storage
                    lstrcpynW (aplArrChar, lpMemPTD->lpwszTemp, 1 + (UINT) min (MAX_VAL_LEN, uNameLen));
                    aplArrChar[min (MAX_VAL_LEN, uNameLen)] = WC_EOS;

                    wsprintfW (wszTemp,
                               L"%shGlb=%p DType=%c  NELM=%3d RC=%2d                 Lck=%d (%S#%4d) (%s)",
                               (lpHeader->RefCnt NE 1) ? WS_UTF16_REFCNT_NE1 : L"",
                               hGlb,
                               cDfnTypeStr[lpHeader->DfnType],
                               lpHeader->numFcnLines,
                               lpHeader->RefCnt,
                               (MyGlobalFlags (hGlb) & GMEM_LOCKCOUNT) - 1,
                               lpaFileNameGLBALLOC[i],
                               auLinNumGLBALLOC[i],
                               aplArrChar);
                    DbgMsgW (wszTemp);
                } // End IF

                break;
            } // End DFN_HEADER_SIGNATURE

            default:
                if (uDispGlb EQ 2)
                {
                    wsprintfW (wszTemp,
                               L"hGlb=%p -- No NARS/FCNS Signature (%u bytes)",
                               hGlb,
                               MyGlobalSize (hGlb));
                    DbgMsgW (wszTemp);
                } // End IF/ELSE

                break;
        } // End SWITCH

        // We no longer need this ptr
        GlobalUnlock (hGlb); lpMem = NULL;
    } // End FOR/IF
#undef  MAX_VAL_LEN

    DbgMsgW (L"********** End Globals *********************************");

////DbgMsgW (L"********** Start Semaphores ****************************");

    for (uValid = FALSE, i = 0; i < MAXOBJ; i++)
    if (hGlb = ahSEMAPHORE[i])
    {
        wsprintfW (wszTemp,
                   L"hGlb=%p                                               (%S#%4d)",
                   hGlb,
                   lpaFileNameSEMAPHORE[i],
                   auLinNumSEMAPHORE[i]);
        DbgMsgW (wszTemp);

        // Mark as valid
        uValid = TRUE;
    } // End FOR/IF

    if (uValid)
        DbgMsgW (L"********** End Semaphores ******************************");

    UpdateDBWindow ();
} // End DisplayGlobals
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayHeap
//
//  Display outstanding heap memory objects
//***************************************************************************

void DisplayHeap
    (void)

{
    int     i, j;               // Loop counters
    APLINT  aplSize;            // Size of the global memory
    HGLOBAL hGlb;               // Current global memory handle
    WCHAR   wszTemp[1024];      // Ptr to temporary output area
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

    DbgMsgW (L"********** Start Heap **********************************");

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Clear the MPFR cache constants
    mpfr_free_cache ();

    for (i = 0; i < MAXOBJ; i++)
    if (hGlb = ahHEAPALLOC[i])
    {
        // Weed out heap allocations for various constants
        if (hGlb EQ mpzMinInt._mp_d
         || hGlb EQ mpzMaxInt._mp_d
         || hGlb EQ mpzMaxUInt._mp_d
         || hGlb EQ mpqMinInt._mp_num._mp_d
         || hGlb EQ mpqMinInt._mp_den._mp_d
         || hGlb EQ mpqMaxInt._mp_num._mp_d
         || hGlb EQ mpqMaxInt._mp_den._mp_d
         || hGlb EQ mpqMaxUInt._mp_num._mp_d
         || hGlb EQ mpqMaxUInt._mp_den._mp_d
         || hGlb EQ mpqHalf._mp_num._mp_d
         || hGlb EQ mpqHalf._mp_den._mp_d
         || hGlb EQ mpfMinInt._mpfr_d
         || hGlb EQ mpfMaxInt._mpfr_d
         || hGlb EQ mpfMaxUInt._mpfr_d
         || hGlb EQ lpMemPTD->mpfrE._mpfr_d
         || hGlb EQ lpMemPTD->mpfrPi._mpfr_d
         || hGlb EQ mpfHalf._mpfr_d
         || hGlb EQ (HGLOBAL) ByteDiff (mpfMinInt       ._mpfr_d, 4)
         || hGlb EQ (HGLOBAL) ByteDiff (mpfMaxInt       ._mpfr_d, 4)
         || hGlb EQ (HGLOBAL) ByteDiff (mpfMaxUInt      ._mpfr_d, 4)
         || hGlb EQ (HGLOBAL) ByteDiff (lpMemPTD->mpfrE ._mpfr_d, 4)
         || hGlb EQ (HGLOBAL) ByteDiff (lpMemPTD->mpfrPi._mpfr_d, 4)
         || hGlb EQ (HGLOBAL) ByteDiff (mpfHalf         ._mpfr_d, 4)
           )
            continue;
        // Loop through NthPowerTab
        for (j = 0; j < (int) NthPowerCnt; j++)
        if (hGlb EQ NthPowerTab[j].aplMPI._mp_d)
            break;
        // If we found a match, ...
        if (j < (int) NthPowerCnt)
            continue;
        // Get the size of the global memory
////////aplSize = HeapSize (GetProcessHeap (), 0, hGlb);
        aplSize = dlmalloc_usable_size (hGlb);

        wsprintfW (wszTemp,
                   L"hGlb=%p, size=%I64u (%S#%d)",
                   hGlb,
                   aplSize,
                   "",      // lpaFileNameHEAPALLOC[i],
                   auLinNumHEAPALLOC[i]);
        DbgMsgW (wszTemp);
    } // End FOR/IF

    DbgMsgW (L"********** End Heap ************************************");
} // End DisplayHeap
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayTokens
//
//  Display the contents of the current token stream
//***************************************************************************

void DisplayTokens
    (LPTOKEN_HEADER lpMemTknHdr)    // Ptr to header of tokenized line

{
    int          i,                 // Loop counter
                 iLen;              // Token count
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    WCHAR        wszTemp[1024];     // Ptr to temporary output area
    LPTOKEN      lpMemTknLine;      // Ptr to tokenized line

    // Check debug level
    if (gDbgLvl < 3)
        return;

    DbgMsgW (L"********** Start Tokens ********************************");

    // Ensure it's valid
    if (!lpMemTknHdr)
    {
        DbgMsgW (L"DisplayTokens:  ***INAVLID HANDLE***:  lpMemTknHdr EQ 0");
        return;
    } // End IF

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    wsprintfW (wszTemp,
               L"lpMemTknLine = %p, Version # = %d, TokenCnt = %d, PrevGroup = %d",
               lpMemTknHdr,
               lpMemTknHdr->Version,
               lpMemTknHdr->TokenCnt,
               lpMemTknHdr->PrevGroup);
    DbgMsgW (wszTemp);

    iLen = lpMemTknHdr->TokenCnt;

    lpMemTknLine = TokenBaseToStart (lpMemTknHdr);   // Skip over TOKEN_HEADER

    for (i = 0; i < iLen; i++, lpMemTknLine++)
    {
        wsprintfW (wszTemp,
                   L"(%2d) Data=%I64X, CharIndex=%2d, Type=%S",
                   i,
                   *(LPAPLINT) &lpMemTknLine->tkData.tkFloat,
                   lpMemTknLine->tkCharIndex,
                   GetTokenTypeName (lpMemTknLine->tkFlags.TknType));
        DbgMsgW (wszTemp);
    } // End FOR

    DbgMsgW (L"********** End Tokens **********************************");
} // End DisplayTokensSub
#endif


#ifdef DEBUG
//***************************************************************************
//  $GetTokenTypeName
//
//  Convert a token type value to a name
//***************************************************************************

LPCHAR GetTokenTypeName
    (TOKEN_TYPES uType)

{
typedef struct tagTOKENNAMES
{
    LPCHAR lpsz;
    UINT   uTokenNum;
} TOKENNAMES, *LPTOKENNAMES;

static TOKENNAMES tokenNames[] =
{{"UNUSED"        , TKT_UNUSED        },    // 00: Unused token type -- an error if encounteredLPSYMENTRY)
 {"VARNAMED"      , TKT_VARNAMED      },    // 01: Symbol table entry for a named var (data is LPSYMENTRY)
 {"CHRSTRAND"     , TKT_CHRSTRAND     },    // 02: Character strand  (data is HGLOBAL)
 {"NUMSTRAND"     , TKT_NUMSTRAND     },    // 03: Numeric   ...     (data is HGLOBAL)
 {"NUMSCALAR"     , TKT_NUMSCALAR     },    // 04: Numeric   scalar  (data is HGLOBAL)
 {"VARIMMED"      , TKT_VARIMMED      },    // 05: Immediate data (data is immediate)
 {"ASSIGN"        , TKT_ASSIGN        },    // 06: Assignment symbol (data is UTF16_LEFTARROW)
 {"LISTSEP"       , TKT_LISTSEP       },    // 07: List separator    (data is ';')
 {"LABELSEP"      , TKT_LABELSEP      },    // 08: Label ...         (data is ':')
 {"COLON"         , TKT_COLON         },    // 09: Colon             (data is ':')
 {"FCNIMMED"      , TKT_FCNIMMED      },    // 0A: Primitive function (any valence) (data is UTF16_***)
 {"OP1IMMED"      , TKT_OP1IMMED      },    // 0B: Monadic primitive operator (data is UTF16_***)
 {"OP2IMMED"      , TKT_OP2IMMED      },    // 0C: Dyadic  ...
 {"OP3IMMED"      , TKT_OP3IMMED      },    // 0D: Ambiguous ...
 {"OPJOTDOT"      , TKT_OPJOTDOT      },    // 0E: Outer product monadic operator (with right scope) (data is NULL)
 {"LEFTPAREN"     , TKT_LEFTPAREN     },    // 0F: Left paren (data is TKT_LEFTPAREN)
 {"RIGHTPAREN"    , TKT_RIGHTPAREN    },    // 10: Right ...   ...         RIGHTPAREN
 {"LEFTBRACKET"   , TKT_LEFTBRACKET   },    // 11: Left bracket ...        LEFTBRACKET
 {"RIGHTBRACKET"  , TKT_RIGHTBRACKET  },    // 12: Right ...   ...         RIGHTBRACKET
 {"LEFTBRACE"     , TKT_LEFTBRACE     },    // 13: Left bracket ...        LEFTBRACKET
 {"RIGHTBRACE"    , TKT_RIGHTBRACE    },    // 14: Right ...   ...         RIGHTBRACKET
 {"EOS"           , TKT_EOS           },    // 15: End-of-Stmt (data is length of stmt including this token)
 {"EOL"           , TKT_EOL           },    // 16: End-of-Line  ...
 {"SOS"           , TKT_SOS           },    // 17: Start-of-Stmt (data is NULL)
 {"LINECONT"      , TKT_LINECONT      },    // 18: Line continuation (data is NULL)
 {"INPOUT"        , TKT_INPOUT        },    // 19: Input/Output (data is UTF16_QUAD or UTF16_QUAD2 or UTF16_QUOTEQUAD)
 {"VARARRAY"      , TKT_VARARRAY      },    // 1A: Array of data (data is HGLOBAL)
 {"CS_ANDIF"      , TKT_CS_ANDIF      },    // 1B: Control Structure:  ANDIF     (Data is Line/Stmt #)
 {"CS_ASSERT"     , TKT_CS_ASSERT     },    // 1C: ...                 ASSERT     ...
 {"CS_CASE"       , TKT_CS_CASE       },    // 1D: ...                 CASE       ...
 {"CS_CASELIST"   , TKT_CS_CASELIST   },    // 1E: ...                 CASELIST   ...
 {"CS_CONTINUE"   , TKT_CS_CONTINUE   },    // 1F: ...                 CONTINUE   ...
 {"CS_CONTINUEIF" , TKT_CS_CONTINUEIF },    // 20: ...                 CONTINUEIF ...
 {"CS_ELSE"       , TKT_CS_ELSE       },    // 21: ...                 ELSE       ...
 {"CS_ELSEIF"     , TKT_CS_ELSEIF     },    // 22: ...                 ELSEIF     ...
 {"CS_END"        , TKT_CS_END        },    // 23: ...                 END        ...
 {"CS_ENDFOR"     , TKT_CS_ENDFOR     },    // 24: ...                 ENDFOR     ...
 {"CS_ENDFORLCL"  , TKT_CS_ENDFORLCL  },    // 25: ...                 ENDFORLCL  ...
 {"CS_ENDIF"      , TKT_CS_ENDIF      },    // 26: ...                 ENDIF      ...
 {"CS_ENDREPEAT"  , TKT_CS_ENDREPEAT  },    // 27: ...                 ENDREPEAT  ...
 {"CS_ENDSELECT"  , TKT_CS_ENDSELECT  },    // 28: ...                 ENDSELECT  ...
 {"CS_ENDWHILE"   , TKT_CS_ENDWHILE   },    // 29: ...                 ENDWHILE   ...
 {"CS_FOR"        , TKT_CS_FOR        },    // 2A: ...                 FOR        ...
 {"CS_FOR2"       , TKT_CS_FOR2       },    // 2B: ...                 FOR2       ...
 {"CS_FORLCL"     , TKT_CS_FORLCL     },    // 2C: ...                 FORLCL     ...
 {"CS_GOTO"       , TKT_CS_GOTO       },    // 2D: ...                 GOTO       ...
 {"CS_IF"         , TKT_CS_IF         },    // 2E: ...                 IF         ...
 {"CS_IF2"        , TKT_CS_IF2        },    // 2F: ...                 IF2        ...
 {"CS_IN"         , TKT_CS_IN         },    // 30: ...                 IN         ...
 {"CS_LEAVE"      , TKT_CS_LEAVE      },    // 31: ...                 LEAVE      ...
 {"CS_LEAVEIF"    , TKT_CS_LEAVEIF    },    // 32: ...                 LEAVEIF    ...
 {"CS_ORIF"       , TKT_CS_ORIF       },    // 33: ...                 ORIF       ...
 {"CS_REPEAT"     , TKT_CS_REPEAT     },    // 34: ...                 REPEAT     ...
 {"CS_REPEAT2"    , TKT_CS_REPEAT2    },    // 35: ...                 REPEAT2    ...
 {"CS_RETURN"     , TKT_CS_RETURN     },    // 36: ...                 RETURN     ...
 {"CS_SELECT"     , TKT_CS_SELECT     },    // 37: ...                 SELECT     ...
 {"CS_SELECT2"    , TKT_CS_SELECT2    },    // 38: ...                 SELECT2    ...
 {"CS_UNTIL"      , TKT_CS_UNTIL      },    // 39: ...                 UNTIL      ...
 {"CS_WHILE"      , TKT_CS_WHILE      },    // 3A: ...                 WHILE      ...
 {"CS_WHILE2"     , TKT_CS_WHILE2     },    // 3B: ...                 WHILE2     ...
 {"CS_SKIPCASE"   , TKT_CS_SKIPCASE   },    // 3C: ...                 Special token
 {"CS_SKIPEND"    , TKT_CS_SKIPEND    },    // 3D: ...                 Special token
 {"SYS_NS"        , TKT_SYS_NS        },    // 3E: System namespace
 {"SYNTERR"       , TKT_SYNTERR       },    // 3F: Syntax Error
 {"SETALPHA"      , TKT_SETALPHA      },    // 40: Set {alpha}
 {"DEL"           , TKT_DEL           },    // 41: Del -- always a function
 {"DELDEL"        , TKT_DELDEL        },    // 42: Del Del -- either a monadic or dyadic operator
 {"DELAFO"        , TKT_DELAFO        },    // 43: Anonymous function or monadic/dyadic operator, bound to its operands
 {"FCNAFO"        , TKT_FCNAFO        },    // 44: Anonymous function
 {"OP1AFO"        , TKT_OP1AFO        },    // 45: ...       monadic operator
 {"OP2AFO"        , TKT_OP2AFO        },    // 46: ...       dyadic  ...
 {"GLBDFN"        , TKT_GLBDFN        },    // 47: Placeholder for hGlbDfnHdr
 {"NOP"           , TKT_NOP           },    // 48: NOP
 {"AFOGUARD"      , TKT_AFOGUARD      },    // 49: AFO guard
 {"AFORETURN"     , TKT_AFORETURN     },    // 4A: AFO guard
 {"STRAND"        , TKT_STRAND        },    // 4B: Strand accumulating (data is LPTOKEN)
 {"LISTINT"       , TKT_LISTINT       },    // 4C: List intermediate (data is HGLOBAL)
 {"LISTPAR"       , TKT_LISTPAR       },    // 4D: List in parens    (data is HGLOBAL)
 {"LSTIMMED"      , TKT_LSTIMMED      },    // 4E: List in brackets, single element, immed (data is immediate)
 {"LSTARRAY"      , TKT_LSTARRAY      },    // 4F: List in brackets, single element, array (data is HGLOBAL)
 {"LSTMULT"       , TKT_LSTMULT       },    // 50: List in brackets, multiple elements (data is HGLOBAL)
 {"FCNARRAY"      , TKT_FCNARRAY      },    // 51: Array of functions (data is HGLOBAL)
 {"FCNNAMED"      , TKT_FCNNAMED      },    // 52: Symbol table entry for a named function (data is LPSYMENTRY)
 {"AXISIMMED"     , TKT_AXISIMMED     },    // 53: An immediate axis specification (data is immediate)
 {"AXISARRAY"     , TKT_AXISARRAY     },    // 54: An array of  ...   (data is HGLOBAL)
 {"OP1NAMED"      , TKT_OP1NAMED      },    // 55: A named monadic primitive operator (data is LPSYMENTRY)
 {"OP2NAMED"      , TKT_OP2NAMED      },    // 56: ...     dyadic  ...
 {"OP3NAMED"      , TKT_OP3NAMED      },    // 57: ...     ambiguous ...
 {"STRNAMED"      , TKT_STRNAMED      },    // 58: ...     strand  ...
 {"CS_NEC"        , TKT_CS_NEC        },    // 59: Control Structure:  Special token (cs_yyparse only)
 {"CS_EOL"        , TKT_CS_EOL        },    // 5A: ...                 Special token (cs_yyparse only)
 {"CS_ENS"        , TKT_CS_ENS        },    // 5B: ...                 Special token (cs_yyparse only)
 {"FILLJOT"       , TKT_FILLJOT       },    // 5C: Fill jot
};

// The # rows in the above table
#define TOKENNAMES_NROWS    countof (tokenNames)

    if (TOKENNAMES_NROWS > uType)
        return tokenNames[uType].lpsz;
    else
    {
        static char szTemp[64];

        wsprintf (szTemp,
                  "***Unknown Token Type:  %d***",
                  uType);
        return szTemp;
    } // End IF/ELSE
} // End GetTokenTypeName
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayFcnStrand
//
//  Display a function strand
//***************************************************************************

void DisplayFcnStrand
    (LPTOKEN lptkFunc,              // Ptr to function token
     UBOOL   bDispHdr)              // TRUE iff we're to display the header

{
    HGLOBAL      hGlbData;          // Function array global memory handle
    LPWCHAR      lpaplChar,         // Ptr to output save area
                 lpaplCharIni;      // Initial ptr to output save area
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

    // Check debug level
    if (bDispHdr
#ifdef DEBUG
     && gDbgLvl < gFcnLvl
#endif
     )
        return;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get a ptr to a temporary save area
    lpaplChar = lpaplCharIni = lpMemPTD->lpwszTemp;

    // Split cases based upon the token type
    switch (lptkFunc->tkFlags.TknType)
    {
        case TKT_VARNAMED:          // Because we don't distinguish between
                                    //   functions and variables in AssignName_EM
            DbgBrk ();          // ***FINISHME*** -- TKT_VARNAMED in DisplayFcnStrand






            break;

        case TKT_FCNIMMED:
        case TKT_OP1IMMED:
        case TKT_OP2IMMED:
        case TKT_OP3IMMED:
            if (bDispHdr)
                lpaplChar += wsprintfW (lpaplChar,
                                        L"fnNameType=%1d, NELM=%3d, RC=%2d, Fn:  ",
                                        NAMETYPE_FN12,  // lpHeader->fnNameType,
                                        1,              // LODWORD (lpHeader->NELM),
                                        0);             // lpHeader->RefCnt);
            // Translate from INDEX_xxx to UTF16_xxx
            *lpaplChar++ = TranslateFcnOprToChar (lptkFunc->tkData.tkChar);

            break;

        case TKT_FCNNAMED:
        case TKT_OP1NAMED:
        case TKT_OP2NAMED:
        case TKT_OP3NAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkFunc->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not immediate, ...
            if (!lptkFunc->tkData.tkSym->stFlags.Imm)
            {
                hGlbData = lptkFunc->tkData.tkSym->stData.stGlbData;

                // stData is an internal function or a HGLOBAL function array
                //   or user-defined function/operator
                Assert (lptkFunc->tkData.tkSym->stFlags.FcnDir
                     || IsGlbTypeFcnDir_PTB (hGlbData)
                     || IsGlbTypeDfnDir_PTB (hGlbData));

                // Check for internal functions
                if (lptkFunc->tkData.tkSym->stFlags.FcnDir)
                {
                    if (bDispHdr)
                        // Start off with the header
                        lpaplChar += wsprintfW (lpaplChar,
                                                L"fnNameType=%1d, NELM=%3d, RC=%2d, Fn:  ",
                                                NAMETYPE_FN12,  // lpHeader->fnNameType,
                                                1,              // LODWORD (lpHeader->NELM),
                                                0);             // lpHeader->RefCnt);
                    // Display the function name from the symbol table
                    lpaplChar =
                      CopySteName (lpaplChar,               // Ptr to result global memory
                                   lptkFunc->tkData.tkSym,  // Ptr to function symbol table entry
                                   NULL);                   // Ptr to name length (may be NULL)
                } else
                if (!lptkFunc->tkData.tkSym->stFlags.UsrDfn)
                    lpaplChar =
                      DisplayFcnGlb (lpaplChar,         // Ptr to output save area
                                     hGlbData,          // Function array global memory handle
                                     bDispHdr,          // TRUE iff we're to display the header
                                     NULL,              // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                                     NULL,              // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                                     NULL,              // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                                     NULL);             // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
            } else
            {
                if (bDispHdr)
                    // Start off with the header
                    lpaplChar += wsprintfW (lpaplChar,
                                            L"fnNameType=%1d, NELM=%3d, RC=%2d, Fn:  ",
                                            NAMETYPE_FN12,  // lpHeader->fnNameType,
                                            1,              // LODWORD (lpHeader->NELM),
                                            0);             // lpHeader->RefCnt);
                *lpaplChar++ = lptkFunc->tkData.tkSym->stData.stChar;
            } // End IF/ELSE

            break;

        case TKT_FCNARRAY:
            hGlbData = lptkFunc->tkData.tkGlbData;

            // tkData is an HGLOBAL function array
            Assert (IsGlbTypeFcnDir_PTB (hGlbData));

            lpaplChar =
              DisplayFcnGlb (lpaplChar,                 // Ptr to output save area
                             hGlbData,                  // Function array global memory handle
                             bDispHdr,                  // TRUE iff we're to display the header
                             NULL,                      // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                             NULL,                      // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                             NULL,                      // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                             NULL);                     // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
            break;

        defstop
            break;
    } // End SWITCH

    // Ensure properly terminated
    *lpaplChar = WC_EOS;

    if (bDispHdr)
        // Display the line in the debugging window
        DbgMsgW (lpaplCharIni);
    else
        // Display the line in the Session Manager window
        AppendLine (lpaplCharIni, FALSE, TRUE);
} // End DisplayFcnStrand
#endif


//***************************************************************************
//  $DisplayFcnGlb
//
//  Display a function from an HGLOBAL
//***************************************************************************

LPWCHAR DisplayFcnGlb
    (LPWCHAR             lpaplChar,             // Ptr to output save area
     HGLOBAL             hGlbFcnArr,            // Function array global memory handle
     UBOOL               bDispHeader,           // TRUE iff we're to display the header
     LPSAVEDWSGLBVARCONV lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
     LPSAVEDWSGLBVARPARM lpSavedWsGlbVarParm,   // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
     LPSAVEDWSGLBFCNCONV lpSavedWsGlbFcnConv,   // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
     LPSAVEDWSGLBFCNPARM lpSavedWsGlbFcnParm)   // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)

{
    LPFCNARRAY_HEADER lpHeader;                 // Ptr to function array header
    LPPL_YYSTYPE      lpMemFcnArr;              // ...                   global memory
    UINT              tknNELM;                  // # tokens in the function array
    NAME_TYPES        fnNameType;               // Function array name type

    // Lock the memory to get a ptr to it
    lpHeader = MyGlobalLock (hGlbFcnArr);

    // Get the NELM and NAMETYPE_xxx
    tknNELM    = lpHeader->tknNELM;
    fnNameType = lpHeader->fnNameType;

#ifdef DEBUG
    if (bDispHeader)
        lpaplChar += wsprintfW (lpaplChar,
                                L"%sfnNameType=%s, NELM=%3d, RC=%2d%s, Fn:  ",
                                (lpHeader->RefCnt NE 1) ? WS_UTF16_REFCNT_NE1 : L"",
                                lpwNameTypeStr[lpHeader->fnNameType],
                                tknNELM,
                                lpHeader->RefCnt,
                                lpHeader->SkipRefCntIncr ? L"*" : L" ");
#endif
    // Skip over the function array header
    lpMemFcnArr = FcnArrayBaseToData (lpHeader);

    // Display the function array in global memory
    lpaplChar =
      DisplayFcnMem (lpaplChar,             // Ptr to output save area
                     lpMemFcnArr,           // Ptr to function array data
                     tknNELM,               // Token NELM
                     fnNameType,            // Function array name type
                     lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                     lpSavedWsGlbVarParm,   // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                     lpSavedWsGlbFcnConv,   // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                     lpSavedWsGlbFcnParm);  // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
    // We no longer need this ptr
    MyGlobalUnlock (hGlbFcnArr); lpMemFcnArr = NULL;

    return lpaplChar;
} // End DisplayFcnGlb


//***************************************************************************
//  $DisplayFcnMem
//
//  Display function from global memory
//***************************************************************************

LPWCHAR DisplayFcnMem
    (LPWCHAR             lpaplChar,             // Ptr to output save area
     LPPL_YYSTYPE        lpMemFcnArr,           // Ptr to function array data
     UINT                tknNELM,               // Token NELM
     NAME_TYPES          fnNameType,            // Function array name type
     LPSAVEDWSGLBVARCONV lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
     LPSAVEDWSGLBVARPARM lpSavedWsGlbVarParm,   // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
     LPSAVEDWSGLBFCNCONV lpSavedWsGlbFcnConv,   // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
     LPSAVEDWSGLBFCNPARM lpSavedWsGlbFcnParm)   // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)

{
    // Check for Trains
    if (fnNameType EQ NAMETYPE_TRN)
    {
        // Skip to the next entry
        lpMemFcnArr = &lpMemFcnArr[tknNELM];

        // Start with surrounding parens
        *lpaplChar++ = L'(';

        // Loop through the function array entries
        //   skipping the first (TKT_OP1IMMED/INDEX_OPTRAIN) entry
        while (tknNELM-- > 1)
        {
            // Back off to next item
            lpMemFcnArr--;

            lpaplChar =
              DisplayFcnSub (lpaplChar,             // Ptr to output save area
                             lpMemFcnArr,           // Ptr to function array data
                             1,                     // Token NELM
                             lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                             lpSavedWsGlbVarParm,   // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                             lpSavedWsGlbFcnConv,   // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                             lpSavedWsGlbFcnParm);  // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
            // Append visual separator
            *lpaplChar++ = L' ';
        } // End WHILE

        // If there's a trailing blank, ...
        if (lpaplChar[-1] EQ L' ')
            // Back up over it
            lpaplChar--;

        // Ending paren
        *lpaplChar++ = L')';

        // Ensure properly terminated
        *lpaplChar = WC_EOS;
    } else
        lpaplChar =
          DisplayFcnSub (lpaplChar,             // Ptr to output save area
                         lpMemFcnArr,           // Ptr to function array data
                         tknNELM,               // Token NELM
                         lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                         lpSavedWsGlbVarParm,   // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                         lpSavedWsGlbFcnConv,   // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                         lpSavedWsGlbFcnParm);  // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
    return lpaplChar;
} // End DisplayFcnMem


//***************************************************************************
//  $DisplayFcnSub
//
//  Display function subroutine
//***************************************************************************

LPWCHAR DisplayFcnSub
    (LPWCHAR             lpaplChar,             // Ptr to output save area
     LPPL_YYSTYPE        lpYYMem,               // Ptr to function array data
     UINT                tknNELM,               // Token NELM
     LPSAVEDWSGLBVARCONV lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
     LPSAVEDWSGLBVARPARM lpSavedWsGlbVarParm,   // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
     LPSAVEDWSGLBFCNCONV lpSavedWsGlbFcnConv,   // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
     LPSAVEDWSGLBFCNPARM lpSavedWsGlbFcnParm)   // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)

{
    HGLOBAL      hGlbData;          // Function array global memory handle
    LPVOID       lpMemData;         // Ptr to function array global memory
    UINT         TknCount;          // Token count
    NAME_TYPES   fnNameType;        // Function array name type
    LPPL_YYSTYPE lpMemFcnArr;       // Ptr to function array data
    LPDFN_HEADER lpMemDfnHdr;       // Ptr to AFO global memory header

    // Split cases based upon the token type
    switch (lpYYMem[0].tkToken.tkFlags.TknType)
    {
        case TKT_OP1IMMED:
        case TKT_OP3IMMED:
            // Check for axis operator
            if (tknNELM > 1
             && (lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
              || lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
            {
                // If there's a function, ...
                if (tknNELM > 2)
                    lpaplChar =
                      DisplayFcnSub (lpaplChar,                                         // Fcn
                                    &lpYYMem[2],
                                     tknNELM - 2,
                                     lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                                     lpSavedWsGlbVarParm,   // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                                     lpSavedWsGlbFcnConv,   // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                                     lpSavedWsGlbFcnParm);  // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
                // Translate from INDEX_xxx to UTF16_xxx
                *lpaplChar++ = TranslateFcnOprToChar (lpYYMem[0].tkToken.tkData.tkChar);// Op1
                lpaplChar =
                  DisplayFcnSub (lpaplChar,                                             // [X]
                                &lpYYMem[1],
                                 1,
                                 lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                                 lpSavedWsGlbVarParm,   // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                                 lpSavedWsGlbFcnConv,   // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                                 lpSavedWsGlbFcnParm);  // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
            } else
            // If the monadic operator is not INDEX_OPTRAIN, ...
            if (lpYYMem[0].tkToken.tkData.tkChar NE INDEX_OPTRAIN)
            {
                if (tknNELM > 1)
                    lpaplChar =
                      DisplayFcnSub (lpaplChar,                                         // Fcn
                                    &lpYYMem[1],
                                     tknNELM - 1,
                                     lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                                     lpSavedWsGlbVarParm,   // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                                     lpSavedWsGlbFcnConv,   // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                                     lpSavedWsGlbFcnParm);  // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
                    // Translate from INDEX_xxx to UTF16_xxx
                    *lpaplChar++ = TranslateFcnOprToChar (lpYYMem[0].tkToken.tkData.tkChar);// Op1
            } else
            {
                // Skip to the next entry
                lpMemFcnArr = &lpYYMem[tknNELM];

                // Start with surrounding parens
                *lpaplChar++ = L'(';

                // Loop through the function array entries
                //   skipping the first (TKT_OP1IMMED/INDEX_OPTRAIN) entry
                while (tknNELM-- > 1)
                {
                    // Back off to next item
                    lpMemFcnArr--;

                    lpaplChar =
                      DisplayFcnSub (lpaplChar,                                         // Fcn
                                     lpMemFcnArr,
                                     1,
                                     lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                                     lpSavedWsGlbVarParm,   // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                                     lpSavedWsGlbFcnConv,   // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                                     lpSavedWsGlbFcnParm);  // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
                    // Append visual separator
                    *lpaplChar++ = L' ';
                } // End WHILE

                // If there's a trailing blank, ...
                if (lpaplChar[-1] EQ L' ')
                    // Back up over it
                    lpaplChar--;

                // Ending paren
                *lpaplChar++ = L')';
            } // End IF/ELSE/...

            break;

        case TKT_OP2IMMED:
            TknCount = 1 + lpYYMem[1].TknCount;
            lpaplChar =
              DisplayFcnSub (lpaplChar,                                                 // Lfcn
                            &lpYYMem[1],
                             lpYYMem[1].TknCount,
                             lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                             lpSavedWsGlbVarParm,   // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                             lpSavedWsGlbFcnConv,   // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                             lpSavedWsGlbFcnParm);  // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
            // Translate from INDEX_xxx to UTF16_xxx
            *lpaplChar++ = TranslateFcnOprToChar (lpYYMem[0].tkToken.tkData.tkChar);    // Op2
            if (lpYYMem[TknCount].TknCount > 1)
                *lpaplChar++ = L'(';
            lpaplChar =
              DisplayFcnSub (lpaplChar,                                                 // Rfcn
                            &lpYYMem[TknCount],
                             lpYYMem[TknCount].TknCount,
                             lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                             lpSavedWsGlbVarParm,   // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                             lpSavedWsGlbFcnConv,   // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                             lpSavedWsGlbFcnParm);  // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
            if (lpYYMem[TknCount].TknCount > 1)
                *lpaplChar++ = L')';
            break;

        case TKT_FCNIMMED:
        case TKT_FILLJOT:
            // Translate from INDEX_xxx to UTF16_xxx
            *lpaplChar++ = TranslateFcnOprToChar (lpYYMem[0].tkToken.tkData.tkChar);    // Fcn

            // Check for axis operator
            if (tknNELM > 1
             && (lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
              || lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
                lpaplChar =
                  DisplayFcnSub (lpaplChar,                                             // [X]
                                &lpYYMem[1],
                                 1,
                                 lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                                 lpSavedWsGlbVarParm,   // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                                 lpSavedWsGlbFcnConv,   // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                                 lpSavedWsGlbFcnParm);  // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
            break;

        case TKT_FCNNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpYYMem->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

            // If it's not an immediate function, ...
            if (!lpYYMem->tkToken.tkData.tkSym->stFlags.Imm)
            {
                // Check for internal functions
                if (lpYYMem->tkToken.tkData.tkSym->stFlags.FcnDir)
                    // Copy the internal function name
                    lpaplChar =
                      CopySteName (lpaplChar,                       // Ptr to result global memory
                                   lpYYMem->tkToken.tkData.tkSym,   // Ptr to function symbol table entry
                                   NULL);                           // Ptr to name length (may be NULL)
                else
                {
                    // Get the function array global memory handle
                    hGlbData = lpYYMem->tkToken.tkData.tkSym->stData.stGlbData;

                    // Split cases based upon the array signature
                    switch (GetSignatureGlb_PTB (hGlbData))
                    {
                        LPDFN_HEADER lpMemDfnHdr;       // Ptr to user-defined function/operator header

                        case FCNARRAY_HEADER_SIGNATURE:
                            // Display the function strand in global memory
                            lpaplChar =
                              DisplayFcnGlb (lpaplChar,                 // Ptr to output save area
                                             hGlbData,                  // Function array global memory handle
                                             FALSE,                     // TRUE iff we're to display the header
                                             lpSavedWsGlbVarConv,       // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                                             lpSavedWsGlbVarParm,       // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                                             lpSavedWsGlbFcnConv,       // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                                             lpSavedWsGlbFcnParm);      // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
                            break;

                        case DFN_HEADER_SIGNATURE:
                            // Lock the memory to get a ptr to it
                            lpMemDfnHdr = MyGlobalLock (hGlbData);

                            // Append the function name from the symbol table
                            lpaplChar = CopySteName (lpaplChar, lpMemDfnHdr->steFcnName, NULL);

                            // We no longer need this ptr
                            MyGlobalUnlock (hGlbData); lpMemDfnHdr = NULL;

                            break;

                        defstop
                            break;
                    } // End SWITCH
                } // End IF/ELSE
            } else
                // Handle the immediate case
                *lpaplChar++ = lpYYMem->tkToken.tkData.tkSym->stData.stChar;

            // Check for axis operator
            if (tknNELM > 1
             && (lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
              || lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
                lpaplChar =
                  DisplayFcnSub (lpaplChar,                                             // [X]
                                &lpYYMem[1],
                                 1,
                                 lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                                 lpSavedWsGlbVarParm,   // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                                 lpSavedWsGlbFcnConv,   // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                                 lpSavedWsGlbFcnParm);  // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
            break;

        case TKT_OPJOTDOT:
            *lpaplChar++ = UTF16_JOT;
            *lpaplChar++ = L'.';

            // Surround with parens if more than one token
            if (tknNELM > 2)
                *lpaplChar++ = L'(';
            if (tknNELM > 1)
                lpaplChar =
                  DisplayFcnSub (lpaplChar,                                             // Fcn
                                &lpYYMem[1],
                                tknNELM - 1,
                                 lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                                 lpSavedWsGlbVarParm,   // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                                 lpSavedWsGlbFcnConv,   // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                                 lpSavedWsGlbFcnParm);  // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
            if (tknNELM > 2)
                *lpaplChar++ = L')';
            break;

        case TKT_AXISIMMED:
            *lpaplChar++ = L'[';

            // Display the var immed
            lpaplChar =
              FormatImmed (lpaplChar,           // ***FIXME*** Use FormatImmedFC ??
                           lpYYMem[0].tkToken.tkFlags.ImmType,
                          &lpYYMem[0].tkToken.tkData.tkLongest);
            lpaplChar[-1] = L']';   // Overwrite the trailing blank

            break;

        case TKT_AXISARRAY:
            *lpaplChar++ = L'[';

            // Display the var array
            lpaplChar =
              DisplayVarSub (lpaplChar,                         // Ptr to output save area
                             lpYYMem->tkToken.tkData.tkGlbData, // Object global memory handle
                             lpSavedWsGlbVarConv,               // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                             lpSavedWsGlbVarParm);              // Ptr to extra parameters for lpSavedWsGlbVarConv

            *lpaplChar++ = L']';

            break;

        case TKT_VARIMMED:
            lpaplChar =
              FormatImmed (lpaplChar,           // ***FIXME*** Use FormatImmedFC ??
                           lpYYMem[0].tkToken.tkFlags.ImmType,
                          &lpYYMem[0].tkToken.tkData.tkLongest);
            if (lpaplChar[-1] EQ L' ')
                lpaplChar--;            // Back over the trailing blank
            break;

        case TKT_CHRSTRAND:
        case TKT_NUMSTRAND:
        case TKT_NUMSCALAR:
        case TKT_VARARRAY:
            // Display the var array
            lpaplChar =
              DisplayVarSub (lpaplChar,                         // Ptr to output save area
                             lpYYMem->tkToken.tkData.tkGlbData, // Object global memory handle
                             lpSavedWsGlbVarConv,               // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                             lpSavedWsGlbVarParm);              // Ptr to extra parameters for lpSavedWsGlbVarConv
            break;

        case TKT_FCNARRAY:
            // Get the function array global memory handle
            hGlbData = lpYYMem->tkToken.tkData.tkGlbData;

            // tkData is a valid HGLOBAL function array
            //   or user-defined function/operator
            Assert (IsGlbTypeFcnDir_PTB (hGlbData)
                 || IsGlbTypeDfnDir_PTB (hGlbData));

            // Lock the memory to get a ptr to it
            lpMemData = MyGlobalLock (hGlbData);

            // Split cases based upon the array signature
            switch (GetSignatureMem (lpMemData))
            {
                case FCNARRAY_HEADER_SIGNATURE:
                    // Get the NELM and NAMETYPE_xxx
                    tknNELM    = ((LPFCNARRAY_HEADER) lpMemData)->tknNELM;
                    fnNameType = ((LPFCNARRAY_HEADER) lpMemData)->fnNameType;

                    // Skip over the header to the data
                    lpMemData = FcnArrayBaseToData (lpMemData);

                    // Display the function array in global memory
                    lpaplChar =
                      DisplayFcnMem (lpaplChar,             // Ptr to output save area
                                     lpMemData,             // Ptr to function array data
                                     tknNELM,               // Token NELM
                                     fnNameType,            // Function array name type
                                     lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                                     lpSavedWsGlbVarParm,   // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                                     lpSavedWsGlbFcnConv,   // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                                     lpSavedWsGlbFcnParm);  // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
                    break;

                case DFN_HEADER_SIGNATURE:
                    // Split cases based upon the UDFO type
                    switch (((LPDFN_HEADER) lpMemData)->DfnType)
                    {
                        case DFNTYPE_OP1:
                            // Check for axis operator
                            if (tknNELM > 1
                             && (lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
                              || lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
                            {
                                // If there's a function, ...
                                if (tknNELM > 2)
                                {
                                    lpaplChar =
                                      DisplayFcnSub (lpaplChar,                                         // Fcn
                                                    &lpYYMem[2],
                                                     tknNELM - 2,
                                                     lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                                                     lpSavedWsGlbVarParm,   // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                                                     lpSavedWsGlbFcnConv,   // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                                                     lpSavedWsGlbFcnParm);  // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
                                    *lpaplChar++ = L' ';                                                // Sep
                                } // End IF

                                lpaplChar =
                                  FillDfnName (lpaplChar,               // Ptr to output save area      // Op1
                                               hGlbData,                // Global memory handle
                                               lpMemData,               // Ptr to global memory
                                               lpSavedWsGlbFcnConv,     // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                                               lpSavedWsGlbFcnParm);    // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
                                *lpaplChar++ = L' ';                                                    // Sep
                                lpaplChar =
                                  DisplayFcnSub (lpaplChar,                                             // [X]
                                                &lpYYMem[1],
                                                 1,
                                                 lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                                                 lpSavedWsGlbVarParm,   // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                                                 lpSavedWsGlbFcnConv,   // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                                                 lpSavedWsGlbFcnParm);  // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
                            } else
                            {
                                if (tknNELM > 1)
                                {
                                    lpaplChar =
                                      DisplayFcnSub (lpaplChar,                                         // Fcn
                                                    &lpYYMem[1],
                                                     tknNELM - 1,
                                                     lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                                                     lpSavedWsGlbVarParm,   // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                                                     lpSavedWsGlbFcnConv,   // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                                                     lpSavedWsGlbFcnParm);  // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
                                    *lpaplChar++ = L' ';                                                // Sep
                                } // End IF

                                lpaplChar =
                                  FillDfnName (lpaplChar,               // Ptr to output save area      // Op1
                                               hGlbData,                // Global memory handle
                                               lpMemData,               // Ptr to global memory
                                               lpSavedWsGlbFcnConv,     // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                                               lpSavedWsGlbFcnParm);    // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
                            } // End IF/ELSE

                            break;

                        case DFNTYPE_OP2:
                            TknCount = 1 + lpYYMem[1].TknCount;

                            // If there's a left operand, ...
                            if (tknNELM > 2)
                            {
                                lpaplChar =
                                  DisplayFcnSub (lpaplChar,                                             // Lfcn
                                                &lpYYMem[1],
                                                 lpYYMem[1].TknCount,
                                                 lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                                                 lpSavedWsGlbVarParm,   // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                                                 lpSavedWsGlbFcnConv,   // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                                                 lpSavedWsGlbFcnParm);  // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
                                *lpaplChar++ = L' ';                                                    // Sep
                            } // End IF

                            lpaplChar =
                              FillDfnName (lpaplChar,               // Ptr to output save area          // Op2
                                           hGlbData,                // Global memory handle
                                           lpMemData,               // Ptr to global memory
                                           lpSavedWsGlbFcnConv,     // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                                           lpSavedWsGlbFcnParm);    // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
                            if (lpYYMem[TknCount].TknCount > 1)
                                *lpaplChar++ = L'(';
                            else
                                *lpaplChar++ = L' ';
                            lpaplChar =
                              DisplayFcnSub (lpaplChar,                                                 // Rfcn
                                            &lpYYMem[TknCount],
                                             lpYYMem[TknCount].TknCount,
                                             lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                                             lpSavedWsGlbVarParm,   // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                                             lpSavedWsGlbFcnConv,   // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                                             lpSavedWsGlbFcnParm);  // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
                            if (lpYYMem[TknCount].TknCount > 1)
                                *lpaplChar++ = L')';
                            break;

                        case DFNTYPE_FCN:
                            lpaplChar =
                              FillDfnName (lpaplChar,               // Ptr to output save area          // Fcn
                                           hGlbData,                // Global memory handle
                                           lpMemData,               // Ptr to global memory
                                           lpSavedWsGlbFcnConv,     // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                                           lpSavedWsGlbFcnParm);    // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
                            // Check for axis operator
                            if (tknNELM > 1
                             && (lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
                              || lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
                                lpaplChar =
                                  DisplayFcnSub (lpaplChar,                                             // [X]
                                                &lpYYMem[1],
                                                 1,
                                                 lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                                                 lpSavedWsGlbVarParm,   // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                                                 lpSavedWsGlbFcnConv,   // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                                                 lpSavedWsGlbFcnParm);  // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
                            break;

                        defstop
                            break;
                    } // End SWITCH

                    break;

                defstop
                    break;
            } // End SWITCH

            // We no longer need this ptr
            MyGlobalUnlock (hGlbData); lpMemData = NULL;

            break;

        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpYYMem->tkToken.tkData.tkVoid) EQ PTRTYPE_STCONST);

            // Copy the STE name
            lpaplChar =
              CopySteName (lpaplChar,                           // Ptr to result global memory
                           lpYYMem->tkToken.tkData.tkSym,       // Ptr to function symbol table entry
                           NULL);                               // Ptr to name length (may be NULL)
            break;

        case TKT_FCNAFO:
            // Get the AFO global memory handle
            hGlbData = lpYYMem->tkToken.tkData.tkGlbData;

            // Lock the memory to get a ptr to it
            lpMemDfnHdr = MyGlobalLock (hGlbData);

            lpaplChar =
              FillDfnName (lpaplChar,               // Ptr to output save area          // Fcn
                           hGlbData,                // Global memory handle
                           lpMemDfnHdr,             // Ptr to global memory
                           lpSavedWsGlbFcnConv,     // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                           lpSavedWsGlbFcnParm);    // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
            // Check for axis operator
            if (tknNELM > 1
             && (lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISIMMED
              || lpYYMem[1].tkToken.tkFlags.TknType EQ TKT_AXISARRAY))
                lpaplChar =
                  DisplayFcnSub (lpaplChar,                                             // [X]
                                &lpYYMem[1],
                                 1,
                                 lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                                 lpSavedWsGlbVarParm,   // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                                 lpSavedWsGlbFcnConv,   // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                                 lpSavedWsGlbFcnParm);  // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
            // We no longer need this ptr
            MyGlobalUnlock (hGlbData); lpMemDfnHdr = NULL;

            break;

        case TKT_OP1AFO:
            // Get the AFO global memory handle
            hGlbData = lpYYMem->tkToken.tkData.tkGlbData;

            // Lock the memory to get a ptr to it
            lpMemDfnHdr = MyGlobalLock (hGlbData);

            if (tknNELM > 1)
            {
                lpaplChar =
                  DisplayFcnSub (lpaplChar,                                         // Fcn
                                &lpYYMem[1],
                                 tknNELM - 1,
                                 lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                                 lpSavedWsGlbVarParm,   // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                                 lpSavedWsGlbFcnConv,   // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                                 lpSavedWsGlbFcnParm);  // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
                *lpaplChar++ = L' ';                                                // Sep
            } // End IF

            lpaplChar =
              FillDfnName (lpaplChar,               // Ptr to output save area      // Op1
                           hGlbData,                // Global memory handle
                           lpMemDfnHdr,             // Ptr to global memory
                           lpSavedWsGlbFcnConv,     // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                           lpSavedWsGlbFcnParm);    // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
            // We no longer need this ptr
            MyGlobalUnlock (hGlbData); lpMemDfnHdr = NULL;

            break;

        case TKT_OP2AFO:
            // Get the AFO global memory handle
            hGlbData = lpYYMem->tkToken.tkData.tkGlbData;

            // Lock the memory to get a ptr to it
            lpMemDfnHdr = MyGlobalLock (hGlbData);

            TknCount = 1 + lpYYMem[1].TknCount;

            // If there's a left operand, ...
            if (tknNELM > 2)
            {
                lpaplChar =
                  DisplayFcnSub (lpaplChar,                                             // Lfcn
                                &lpYYMem[1],
                                 lpYYMem[1].TknCount,
                                 lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                                 lpSavedWsGlbVarParm,   // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                                 lpSavedWsGlbFcnConv,   // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                                 lpSavedWsGlbFcnParm);  // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
                *lpaplChar++ = L' ';                                                    // Sep
            } // End IF

            lpaplChar =
              FillDfnName (lpaplChar,               // Ptr to output save area          // Op2
                           hGlbData,                // Global memory handle
                           lpMemDfnHdr,             // Ptr to global memory
                           lpSavedWsGlbFcnConv,     // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                           lpSavedWsGlbFcnParm);    // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
            if (lpYYMem[TknCount].TknCount > 1)
                *lpaplChar++ = L'(';
            else
                *lpaplChar++ = L' ';
            lpaplChar =
              DisplayFcnSub (lpaplChar,                                                 // Rfcn
                            &lpYYMem[TknCount],
                             lpYYMem[TknCount].TknCount,
                             lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                             lpSavedWsGlbVarParm,   // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                             lpSavedWsGlbFcnConv,   // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                             lpSavedWsGlbFcnParm);  // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
            if (lpYYMem[TknCount].TknCount > 1)
                *lpaplChar++ = L')';

            // We no longer need this ptr
            MyGlobalUnlock (hGlbData); lpMemDfnHdr = NULL;

            break;

        case TKT_DELDEL:
            *lpaplChar++ = UTF16_DEL;

            // Fall through to common code

        case TKT_DELAFO:
            *lpaplChar++ = UTF16_DEL;

            break;

        case TKT_OP1NAMED:          // At the moment, named operators are all one char
        case TKT_OP2NAMED:          //   symbols (no assignment of {jot}{dot}), so they
        case TKT_OP3NAMED:          //   are all immediates and are handled as such above.
        defstop
            break;
    } // End SWITCH

    // Ensure properly terminated
    *lpaplChar = WC_EOS;

    return lpaplChar;
} // End DisplayFcnSub


//***************************************************************************
//  $DisplayVarSub
//
//  Display a variable within a function strand
//***************************************************************************

LPWCHAR DisplayVarSub
    (LPWCHAR             lpaplChar,             // Ptr to output save area
     HGLOBAL             hGlbData,              // Global memory handle
     LPSAVEDWSGLBVARCONV lpSavedWsGlbVarConv,   // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
     LPSAVEDWSGLBVARPARM lpSavedWsGlbVarParm)   // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)

{
    // tkData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir_PTB (hGlbData));

    // If there's a callback function, use it
    if (lpSavedWsGlbVarConv)
        lpaplChar =
          (*lpSavedWsGlbVarConv) (lpaplChar,                // Ptr to output save area
                                  hGlbData,                 // Object global memory handle
                                  lpSavedWsGlbVarParm);     // Ptr to extra parameters for lpSavedWsGlbVarConv
    else
    {
        APLSTYPE aplType;           // The array storage type
        APLNELM  aplNELM;           // The array NELM
        APLRANK  aplRank;           // The array rank
        APLUINT  uCnt;              // Loop counter
        LPVOID   lpMemData;         // Ptr to array global memory

        // Get the axis array global attrs
        AttrsOfGlb (hGlbData, &aplType, &aplNELM, &aplRank, NULL);

        // If the array is a simple scalar or two- or more element vector, ...
        if (IsSimpleNH (aplType)
         && (IsScalar (aplRank)
          || (IsVector (aplRank)
           && IsMultiNELM (aplNELM))))
        {
            // Lock the memory to get a ptr to it
            lpMemData = MyGlobalLock (hGlbData);

            // Skip over the header and dimensions to the data
            lpMemData = VarArrayBaseToData (lpMemData, aplRank);

            // Split cases based upon the storage type
            switch (aplType)
            {
                case ARRAY_BOOL:
                case ARRAY_INT:
                case ARRAY_APA:
                    // Loop through the elements
                    for (uCnt = 0; uCnt < aplNELM; uCnt++)
                        // Format the next value as an integer
                        lpaplChar =
                          FormatAplint (lpaplChar,                                  // Ptr to output save area
                                        GetNextInteger (lpMemData, aplType, uCnt)); // The value to format
                    break;

                case ARRAY_FLOAT:
                    // Loop through the elements
                    for (uCnt = 0; uCnt < aplNELM; uCnt++)
                        // Format the next value as an integer
                        lpaplChar =
                          FormatFloat (lpaplChar,                                   // Ptr to output savea area
                                       GetNextFloat (lpMemData, aplType, uCnt),     // The value to format
                                       0);                                          // Use default significant digits
                    break;

                case ARRAY_CHAR:
                    // Append a leading quote marker
                    *lpaplChar++ = WC_DQ;

                    // Loop through the elements
                    for (uCnt = 0; uCnt < aplNELM; uCnt++)
                    {
                        // If the char is a double quote, double it
                        if (((LPAPLCHAR) lpMemData)[uCnt] EQ WC_DQ)
                            *lpaplChar++ = WC_DQ;
                        *lpaplChar++ = ((LPAPLCHAR) lpMemData)[uCnt];
                    } // End FOR

                    // Append a trailing quote marker
                    *lpaplChar++ = WC_DQ;

                    break;

                defstop
                    break;
            } // End SWITCH

            // We no longer need this ptr
            MyGlobalUnlock (hGlbData); lpMemData = NULL;
        } else
        {
            *lpaplChar++ = UTF16_HORIZELLIPSIS;
            *lpaplChar++ = L' ';                        // N.B.:  trailing blank is significant
        } // End IF/ELSE
    } // End IF

    if (lpaplChar[-1] EQ L' ')
        lpaplChar--;            // Back over the trailing blank

    // Ensure properly terminated
    *lpaplChar = WC_EOS;

    return lpaplChar;
} // End DisplayVarSub


//***************************************************************************
//  $FillDfnName
//
//  Fill in the name of a UDFO
//***************************************************************************

LPWCHAR FillDfnName
    (LPWCHAR             lpaplChar,             // Ptr to output save area
     HGLOBAL             hGlbData,              // Global memory handle
     LPVOID              lpMemData,             // Ptr to global memory
     LPSAVEDWSGLBFCNCONV lpSavedWsGlbFcnConv,   // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
     LPSAVEDWSGLBFCNPARM lpSavedWsGlbFcnParm)   // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)

{
    // If there's a callback function, use it
    if (lpSavedWsGlbFcnConv)
        return
          (*lpSavedWsGlbFcnConv) (lpaplChar,                // Ptr to output save area
                                  MakePtrTypeGlb (hGlbData),// Object global memory handle
                                  lpSavedWsGlbFcnParm);     // Ptr to extra parameters for lpSavedWsGlbVarConv
    else
        // Copy the user-defined function/operator name
        return
          CopySteName (lpaplChar,                               // Ptr to result global memory
                       ((LPDFN_HEADER) lpMemData)->steFcnName,  // Ptr to function symbol table entry
                       NULL);                                   // Ptr to name length (may be NULL)
} // End FillDfnName


#ifdef DEBUG
//***************************************************************************
//  $DisplayFcnArr
//
//  Display a Function Array
//***************************************************************************

void DisplayFcnArr
    (HGLOBAL hGlbStr)               // Function array global memory handle

{
    WCHAR wszTemp[1024];            // Ptr to temporary output area

    Assert (IsGlbTypeFcnDir_PTB (MakePtrTypeGlb (hGlbStr)));

    // Check debug level
    if (gDbgLvl < gFcnLvl)
        return;

    DbgMsgW (L"********** Start Function Array ************************");

    DisplayFcnGlb (wszTemp,         // Ptr to output save area
                   hGlbStr,         // Function array global memory handle
                   TRUE,            // TRUE iff we're to display the header
                   NULL,            // Ptr to function to convert an HGLOBAL var to FMTSTR_GLBOBJ (may be NULL)
                   NULL,            // Ptr to extra parameters for lpSavedWsGlbVarConv (may be NULL)
                   NULL,            // Ptr to function to convert an HGLOBAL fcn to FMTSTR_GLBOBJ (may be NULL)
                   NULL);           // Ptr to extra parameters for lpSavedWsGlbFcnConv (may be NULL)
    DbgMsgW (wszTemp);

    DbgMsgW (L"********** End Function Array **************************");
} // End DisplayFcnArr
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayFcnLine
//
//  Display a given function line
//***************************************************************************

void DisplayFcnLine
    (HGLOBAL      hGlbTxtLine,      // Function line text global memory handle (may be NULL if uLineNum EQ NEG1U)
     LPPERTABDATA lpMemPTD,         // Ptr to PerTabData global memory
     UINT         uLineNum)         // Function line # (NEG1U for terminating)

{
    LPMEMTXT_UNION lpMemTxtLine;
    LPAPLCHAR      lpMemFcnName;

    // Check debug level
    if (gDbgLvl < gFcnLvl)
        return;

    // If the handle is valid, ...
    if (hGlbTxtLine)
        // Lock the memory to get a ptr to it
        lpMemTxtLine = MyGlobalLock (hGlbTxtLine);

    // Lock the memory to get a ptr to it
    lpMemFcnName = MyGlobalLock (lpMemPTD->lpSISCur->hGlbFcnName);

    if (uLineNum EQ NEG1U)
        wsprintfW (lpMemPTD->lpwszTemp,
                   L"Terminating <%s>",
                   lpMemFcnName);
    else
        wsprintfW (lpMemPTD->lpwszTemp,
                   L"Executing line %d of <%s>:  %s",
                   uLineNum,
                   lpMemFcnName,
                  &lpMemTxtLine->C);
    DbgMsgW (lpMemPTD->lpwszTemp);

    // We no longer need this ptr
    MyGlobalUnlock (lpMemPTD->lpSISCur->hGlbFcnName); lpMemFcnName = NULL;

    // If the handle is valid, ...
    if (hGlbTxtLine)
        // We no longer need this ptr
        MyGlobalUnlock (hGlbTxtLine); lpMemTxtLine = NULL;
} // End DisplayFcnline
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayStrand
//
//  Display the strand stack
//***************************************************************************

void DisplayStrand
    (STRAND_INDS strType)           // Strand type (see STRAND_INDS)

{
    LPPL_YYSTYPE  lp,
                  lpLast;
    LPPLLOCALVARS lpplLocalVars;
    LPPERTABDATA  lpMemPTD;         // Ptr to PerTabData global memory
    WCHAR         wszTemp[1024];    // Ptr to temporary output area
    UBOOL         bIsTknImmed;      // TRUE iff the token is immediate

    // Split cases based upon the strand type
    //   so we can display or not depending upon the type
    switch (strType)
    {
        case STRAND_VAR:
            // Check debug level
            if (gDbgLvl < gVarLvl)
                return;
            break;

        case STRAND_FCN:
            // Check debug level
            if (gDbgLvl < gFcnLvl)
                return;
            break;

        case STRAND_LST:
            // Check debug level
            if (gDbgLvl < gLstLvl)
                return;
            break;

        case STRAND_NAM:
            // Check debug level
            if (gDbgLvl < gNamLvl)
                return;
            break;

        defstop
            break;
    } // End SWITCH

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get this thread's LocalVars ptr
    lpplLocalVars = TlsGetValue (dwTlsPlLocalVars);

    switch (strType)
    {
        case STRAND_VAR:
            DbgMsgW (L"********** Start Variable Strands **********************");

            break;

        case STRAND_FCN:
            DbgMsgW (L"********** Start Function Strands **********************");

            break;

        case STRAND_LST:
            DbgMsgW (L"********** Start List Strands **************************");

            break;

        case STRAND_NAM:
            DbgMsgW (L"********** Start Name Strands **************************");

            break;

        defstop
            break;
    } // End SWITCH

    wsprintfW (wszTemp,
               L"Start=%p Base=%p Next=%p",
               lpplLocalVars->lpYYStrArrStart[strType],
               lpplLocalVars->lpYYStrArrBase[strType],
               lpplLocalVars->lpYYStrArrNext[strType]);
    DbgMsgW (wszTemp);

    for (lp = lpplLocalVars->lpYYStrArrStart[strType], lpLast = NULL;
         lp NE lpplLocalVars->lpYYStrArrNext[strType];
         lp ++)
    {
        if (lpLast NE lp->lpYYStrandBase)
        {
            DbgMsgW (L"--------------------------------------------------------");
            lpLast  = lp->lpYYStrandBase;
        } // End IF

        // Get token immediate status
        bIsTknImmed = IsTknImmed (&lp->tkToken);

        // Get the function array

        wsprintfW (wszTemp,
                   L"Strand (%p): %-9.9S D=%8I64X CI=%2d TC=%1d%s IN=%1d F=%p B=%p",
                   lp,
                   GetTokenTypeName (lp->tkToken.tkFlags.TknType),
                   bIsTknImmed
                 ?           lp->tkToken.tkData.tkInteger
                 : (APLUINT) lp->tkToken.tkData.tkGlbData,
                   lp->tkToken.tkCharIndex,
                   lp->TknCount,
                   GetVFOArraySRCIFlag (&lp->tkToken) ? L"*" : L" ",
                   lp->YYIndirect,
                   lp->lpYYFcnBase,
                   lpLast);
        DbgMsgW (wszTemp);
    } // End FOR

    DbgMsgW (L"********** End Strands *********************************");
} // End DisplayStrand
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayUndo
//
//  Display the Undo Buffer
//***************************************************************************

void DisplayUndo
    (HWND hWnd)         // Window handle of the Edit Ctrl

{
    UINT         uCharPos,
                 uLineCount;
    HGLOBAL      hGlbEC;
    LPWCHAR      lpwsz, p;
    HWND         hWndParent;
    LPUNDO_BUF   lpUndoBeg,             // Ptr to start of Undo Buffer
                 lpUndoNxt;             // ...    next available slot in the Undo Buffer
    UBOOL        bShift;
    LPPERTABDATA lpMemPTD;              // Ptr to PerTabData global memory
    WCHAR        wszTemp[1024];         // Ptr to temporary output area
    static LPWCHAR Actions[]={L"None",
                              L"Ins",
                              L"Rep",
                              L"Del",
                              L"Sel",
                              L"Back",
                              L"InsToggle"};

    // Check debug level
    if (gDbgLvl < 3)
        return;

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    DbgMsgW (L"********** Start Undo Buffer ***************************");

    // Get the shift key state
    bShift = (GetKeyState (VK_SHIFT) & BIT15);

    // Get the char position of the caret
    uCharPos = GetCurCharPos (hWnd);

    // Get the lines in the text
    uLineCount = (UINT) SendMessageW (hWnd, EM_GETLINECOUNT, 0, 0);

    // Get the Edit Ctrl's memory handle
    (HANDLE_PTR) hGlbEC = SendMessageW (hWnd, EM_GETHANDLE, 0, 0);

    // Display it
    dprintfWL9 (L"Caret position = %d, # lines = %d, hGlbEC = %p (%S#%d)",
              uCharPos,
              uLineCount,
              hGlbEC,
              FNLN);
    // Lock the memory to get a ptr to it
    lpwsz = MyGlobalLock (hGlbEC);

#define VIS_CR  L'\xAE'
#define VIS_LF  L'\xA9'
#define VIS_HT  L'\xBB'

    // Replace CR with a visible char
    while (p = strchrW (lpwsz, WC_CR))
        *p = VIS_CR;

    // Replace LF with a visible char
    while (p = strchrW (lpwsz, WC_LF))
        *p = VIS_LF;

    // Replace HT with a visible char
    while (p = strchrW (lpwsz, WC_HT))
        *p = VIS_HT;

    // Display it
    dprintfWL9 (L"Text = <%s>",
              lpwsz);

    // Restore HT
    while (p = strchrW (lpwsz, VIS_HT))
        *p = WC_HT;

    // Restore LF
    while (p = strchrW (lpwsz, VIS_LF))
        *p = WC_LF;

    // Restore CR
    while (p = strchrW (lpwsz, VIS_CR))
        *p = WC_CR;

    if (bShift)
        DbgBrk ();

    // We no longer need this ptr
    MyGlobalUnlock (hGlbEC); lpwsz = NULL;

    // Get the parent window handle
    hWndParent = GetParent (hWnd);

    // Get the ptrs to the next available slot in our Undo Buffer
    (HANDLE_PTR) lpUndoNxt = GetWindowLongPtrW (hWndParent, GWLSF_UNDO_NXT);
    (HANDLE_PTR) lpUndoBeg = GetWindowLongPtrW (hWndParent, GWLSF_UNDO_BEG);

    // Loop through the undo buffer entries
    for (; lpUndoBeg < lpUndoNxt; lpUndoBeg++)
    {
        wsprintfW (wszTemp,
                   L"Act = %9s, %2d-%2d, Group = %3d, Char = 0x%04X",
                   Actions[lpUndoBeg->Action],
                   lpUndoBeg->CharPosBeg,
                   lpUndoBeg->CharPosEnd,
                   lpUndoBeg->Group,
                   lpUndoBeg->Char);
        DbgMsgW (wszTemp);
    } // End FOR

    DbgMsgW (L"********** End Undo Buffer *****************************");
} // End DisplayUndo
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayFnHdr
//
//  Display the function header
//***************************************************************************

void DisplayFnHdr
    (LPFHLOCALVARS lpfhLocalVars)       // Ptr to Function Header local vars

{
    WCHAR     wszTemp[1024];            // Ptr to temporary output area
    HGLOBAL   hGlbName;
    LPAPLCHAR lpMemName;
    UINT      uLen, uItm;

    // Check debug level
    if (gDbgLvl < gFcnLvl)
        return;

    // Initialize the output string
    lstrcpyW (wszTemp, L"Saving function header:  " WS_UTF16_DEL L" ");

    // Check for result
    if (lpfhLocalVars->lpYYResult)
    {
        // Get the strand length
        uLen = lpfhLocalVars->lpYYResult->uStrandLen;

        // Append a separator
        if (uLen > 1)
            lstrcatW (wszTemp, L"(");

        for (uItm = 0; uItm < uLen; uItm++)
        {
            // Get the Name's global memory handle
            hGlbName = lpfhLocalVars->lpYYResult[uItm].tkToken.tkData.tkSym->stHshEntry->htGlbName;

            // Lock the memory to get a ptr to it
            lpMemName = MyGlobalLock (hGlbName);

            // Copy the name
            lstrcatW (wszTemp, lpMemName);

            // If we're not at the last item, separate with a space
            if (uItm < (uLen - 1))
                lstrcatW (wszTemp, L" ");

            // We no longer need this ptr
            MyGlobalUnlock (hGlbName); lpMemName = NULL;
        } // End FOR

        // Append a separator
        if (uLen > 1)
            lstrcatW (wszTemp, L")");

        // Append a left arrow
        lstrcatW (wszTemp, WS_UTF16_LEFTARROW);
    } // End IF

    // Check for left argument
    if (lpfhLocalVars->lpYYLftArg)
    {
        // Get the strand length
        uLen = lpfhLocalVars->lpYYLftArg->uStrandLen;

        // Append a separator
        if (lpfhLocalVars->FcnValence EQ FCNVALENCE_AMB)
            lstrcatW (wszTemp, L"{");
        else
        if (uLen > 1)
            lstrcatW (wszTemp, L"(");

        for (uItm = 0; uItm < uLen; uItm++)
        {
            // Get the Name's global memory handle
            hGlbName = lpfhLocalVars->lpYYLftArg[uItm].tkToken.tkData.tkSym->stHshEntry->htGlbName;

            // Lock the memory to get a ptr to it
            lpMemName = MyGlobalLock (hGlbName);

            // Copy the name
            lstrcatW (wszTemp, lpMemName);

            // If we're not at the last item, separate with a space
            if (uItm < (uLen - 1))
                lstrcatW (wszTemp, L" ");

            // We no longer need this ptr
            MyGlobalUnlock (hGlbName); lpMemName = NULL;
        } // End FOR

        // Append a separator
        if (lpfhLocalVars->FcnValence EQ FCNVALENCE_AMB)
            lstrcatW (wszTemp, L"}");
        else
        if (uLen > 1)
            lstrcatW (wszTemp, L")");

        // Append a separator
        lstrcatW (wszTemp, L" ");
    } // End IF

    // Split cases based upon the user-defined function/operator type
    switch (lpfhLocalVars->DfnType)
    {
        case DFNTYPE_OP1:
        case DFNTYPE_OP2:
            // Append a separator
            lstrcatW (wszTemp, L"(");

            // Get the Name's global memory handle
            hGlbName = lpfhLocalVars->lpYYLftOpr->tkToken.tkData.tkSym->stHshEntry->htGlbName;

            // Lock the memory to get a ptr to it
            lpMemName = MyGlobalLock (hGlbName);

            // Copy the name
            lstrcatW (wszTemp, lpMemName);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbName); lpMemName = NULL;

            // Append a separator
            lstrcatW (wszTemp, L" ");

            // Get the Name's global memory handle
            hGlbName = lpfhLocalVars->lpYYFcnName->tkToken.tkData.tkSym->stHshEntry->htGlbName;

            // Lock the memory to get a ptr to it
            lpMemName = MyGlobalLock (hGlbName);

            // Copy the name
            lstrcatW (wszTemp, lpMemName);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbName); lpMemName = NULL;

            // Check for right operand
            if (lpfhLocalVars->lpYYRhtOpr)
            {
                // Append a separator
                lstrcatW (wszTemp, L" ");

                // Get the Name's global memory handle
                hGlbName = lpfhLocalVars->lpYYRhtOpr->tkToken.tkData.tkSym->stHshEntry->htGlbName;

                // Lock the memory to get a ptr to it
                lpMemName = MyGlobalLock (hGlbName);

                // Copy the name
                lstrcatW (wszTemp, lpMemName);

                // We no longer need this ptr
                MyGlobalUnlock (hGlbName); lpMemName = NULL;
            } // End IF

            // Append a separator
            lstrcatW (wszTemp, L")");

            break;

        case DFNTYPE_FCN:
            // Get the Name's global memory handle
            hGlbName = lpfhLocalVars->lpYYFcnName->tkToken.tkData.tkSym->stHshEntry->htGlbName;

            // Lock the memory to get a ptr to it
            lpMemName = MyGlobalLock (hGlbName);

            // Copy the name
            lstrcatW (wszTemp, lpMemName);

            // We no longer need this ptr
            MyGlobalUnlock (hGlbName); lpMemName = NULL;

            break;

        case DFNTYPE_UNK:
            lstrcatW (wszTemp, L"<empty>");

            break;

        defstop
            break;
    } // End SWITCH

    // Check for right argument
    if (lpfhLocalVars->lpYYRhtArg)
    {
        // Append a separator
        lstrcatW (wszTemp, L" ");

        // Get the strand length
        uLen = lpfhLocalVars->lpYYRhtArg->uStrandLen;

        // Append a separator
        if (uLen > 1)
            lstrcatW (wszTemp, L"(");

        for (uItm = 0; uItm < uLen; uItm++)
        {
            // Get the Name's global memory handle
            hGlbName = lpfhLocalVars->lpYYRhtArg[uItm].tkToken.tkData.tkSym->stHshEntry->htGlbName;

            // Lock the memory to get a ptr to it
            lpMemName = MyGlobalLock (hGlbName);

            // Copy the name
            lstrcatW (wszTemp, lpMemName);

            // If we're not at the last item, separate with a space
            if (uItm < (uLen - 1))
                lstrcatW (wszTemp, L" ");

            // We no longer need this ptr
            MyGlobalUnlock (hGlbName); lpMemName = NULL;
        } // End FOR

        // Append a separator
        if (uLen > 1)
            lstrcatW (wszTemp, L")");
    } // End IF

    // Display it in the debug window
    DbgMsgW (wszTemp);
} // End DisplayFnHdr
#endif


#ifdef DEBUG
//***************************************************************************
//  $DisplayYYRes
//
//  Display a YYRes
//***************************************************************************

void DisplayYYRes
    (LPPL_YYSTYPE lpYYRes)

{
    LPDWORD lpdw;

    lpdw = (DWORD *) &lpYYRes->tkToken;

    wsprintfW (lpwszGlbTemp,
               L"%08X-%08X-%08X-%08X",
               lpdw[0],
               lpdw[1],
               lpdw[2],
               lpdw[3]);
    MBWC (lpwszGlbTemp)
} // End DisplayYYRes
#endif


//***************************************************************************
//  End of File: dispdbg.c
//***************************************************************************
