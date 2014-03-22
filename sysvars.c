//***************************************************************************
//  NARS2000 -- System Variable Routines
//***************************************************************************

/***************************************************************************
    NARS2000 -- An Experimental APL Interpreter
    Copyright (C) 2006-2013 Sudley Place Software

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
#include "debug.h"              // For xxx_TEMP_OPEN macros


#define SYSLBL      8
#define SYSVAR      9

SYSNAME aSystemNames[] =
{ // Name                       Valence       Var?   Std?   Exec Routine          SYS_VARS
    {WS_UTF16_QUAD L"alx"      , SYSVAR,      TRUE , FALSE, NULL                , SYSVAR_ALX     },    // Attention Latent Expression
    {WS_UTF16_QUAD L"ct"       , SYSVAR,      TRUE , TRUE , NULL                , SYSVAR_CT      },    // Comparison Tolerance
    {WS_UTF16_QUAD L"dm"       , SYSVAR,      TRUE , FALSE, NULL                , SYSVAR_DM      },    // Diagnostic Message (Read-only)
    {WS_UTF16_QUAD L"dt"       , SYSVAR,      TRUE , FALSE, NULL                , SYSVAR_DT      },    // Distribution Type
    {WS_UTF16_QUAD L"elx"      , SYSVAR,      TRUE , FALSE, NULL                , SYSVAR_ELX     },    // Error Latent Expression
    {WS_UTF16_QUAD L"fc"       , SYSVAR,      TRUE , FALSE, NULL                , SYSVAR_FC      },    // Format Control
    {WS_UTF16_QUAD L"feature"  , SYSVAR,      TRUE , FALSE, NULL                , SYSVAR_FEATURE },    // Feature Control
    {WS_UTF16_QUAD L"fpc"      , SYSVAR,      TRUE , FALSE, NULL                , SYSVAR_FPC     },    // Floating Point Control
    {WS_UTF16_QUAD L"ic"       , SYSVAR,      TRUE , FALSE, NULL                , SYSVAR_IC      },    // Indeterminate Control
    {WS_UTF16_QUAD L"io"       , SYSVAR,      TRUE , TRUE , NULL                , SYSVAR_IO      },    // Index Origin
    {WS_UTF16_QUAD L"lx"       , SYSVAR,      TRUE , TRUE , NULL                , SYSVAR_LX      },    // Latent Expression
    {WS_UTF16_QUAD L"pp"       , SYSVAR,      TRUE , TRUE , NULL                , SYSVAR_PP      },    // Print Precision
    {WS_UTF16_QUAD L"pr"       , SYSVAR,      TRUE , FALSE, NULL                , SYSVAR_PR      },    // Prompt Replacement
    {WS_UTF16_QUAD L"pw"       , SYSVAR,      TRUE , TRUE , NULL                , SYSVAR_PW      },    // Print Width
    {WS_UTF16_QUAD L"rl"       , SYSVAR,      TRUE , TRUE , NULL                , SYSVAR_RL      },    // Random Link
    {WS_UTF16_QUAD L"sa"       , SYSVAR,      TRUE , FALSE, NULL                , SYSVAR_SA      },    // Stop Action
    {WS_UTF16_QUAD L"wsid"     , SYSVAR,      TRUE , TRUE , NULL                , SYSVAR_WSID    },    // Workspace Identifier
    {WS_UTF16_QUAD L"z"        , SYSVAR,      TRUE , FALSE, NULL                , SYSVAR_Z       },    // Temporary result used in 2 []TF
    {WS_UTF16_QUAD SYSLBL_ID   , SYSLBL,      TRUE , FALSE, NULL                , 0              },    // User-defined function/operator entry point for []ID  = identity
    {WS_UTF16_QUAD SYSLBL_INV  , SYSLBL,      TRUE , FALSE, NULL                , 0              },    // ...                                            []INV = inverse
    {WS_UTF16_QUAD SYSLBL_MS   , SYSLBL,      TRUE , FALSE, NULL                , 0              },    // ...                                            []MS  = multiset
    {WS_UTF16_QUAD SYSLBL_PRO  , SYSLBL,      TRUE , FALSE, NULL                , 0              },    // ...                                            []PRO = prototype
    {WS_UTF16_QUAD SYSLBL_SGL  , SYSLBL,      TRUE , FALSE, NULL                , 0              },    // ...                                            []SGL = singleton

// Niladic system functions
    {WS_UTF16_QUAD L"a"        ,      0,      FALSE, FALSE, SysFnA_EM_YY        , 0              },    // Alphabet
    {WS_UTF16_QUAD L"av"       ,      0,      FALSE, TRUE , SysFnAV_EM_YY       , 0              },    // Atomic Vector
    {WS_UTF16_QUAD L"em"       ,      0,      FALSE, TRUE , SysFnEM_EM_YY       , 0              },    // Event Message
    {WS_UTF16_QUAD L"et"       ,      0,      FALSE, TRUE , SysFnET_EM_YY       , 0              },    // Event Type
    {WS_UTF16_QUAD L"lc"       ,      0,      FALSE, TRUE , SysFnLC_EM_YY       , 0              },    // Line Counter
    {WS_UTF16_QUAD L"nnames"   ,      0,      FALSE, FALSE, SysFnNNAMES_EM_YY   , 0              },    // Names of Open Native Files
    {WS_UTF16_QUAD L"nnums"    ,      0,      FALSE, FALSE, SysFnNNUMS_EM_YY    , 0              },    // Tie Numbers of Open Native Files
////{WS_UTF16_QUAD L"si"       ,      0,      FALSE, FALSE, SysFnSI_EM_YY       , 0              },    // State Indicator
////{WS_UTF16_QUAD L"sinl"     ,      0,      FALSE, FALSE, SysFnSINL_EM_YY     , 0              },    // State Indicator w/Name List
    {WS_UTF16_QUAD L"sysid"    ,      0,      FALSE, FALSE, SysFnSYSID_EM_YY    , 0              },    // System Identifier
    {WS_UTF16_QUAD L"sysver"   ,      0,      FALSE, FALSE, SysFnSYSVER_EM_YY   , 0              },    // System Version
    {WS_UTF16_QUAD L"tc"       ,      0,      FALSE, TRUE , SysFnTC_EM_YY       , 0              },    // Terminal Control Characters
    {WS_UTF16_QUAD L"tcbel"    ,      0,      FALSE, FALSE, SysFnTCBEL_EM_YY    , 0              },    // Terminal Control Character, Bell
    {WS_UTF16_QUAD L"tcbs"     ,      0,      FALSE, FALSE, SysFnTCBS_EM_YY     , 0              },    // Terminal Control Character, Backspace
    {WS_UTF16_QUAD L"tcesc"    ,      0,      FALSE, FALSE, SysFnTCESC_EM_YY    , 0              },    // Terminal Control Character, Escape
    {WS_UTF16_QUAD L"tcff"     ,      0,      FALSE, FALSE, SysFnTCFF_EM_YY     , 0              },    // Terminal Control Character, Form Feed
    {WS_UTF16_QUAD L"tcht"     ,      0,      FALSE, FALSE, SysFnTCHT_EM_YY     , 0              },    // Terminal Control Character, Horizontal Tab
    {WS_UTF16_QUAD L"tclf"     ,      0,      FALSE, FALSE, SysFnTCLF_EM_YY     , 0              },    // Terminal Control Character, Line Feed
    {WS_UTF16_QUAD L"tcnl"     ,      0,      FALSE, FALSE, SysFnTCNL_EM_YY     , 0              },    // Terminal Control Character, New Line
    {WS_UTF16_QUAD L"tcnul"    ,      0,      FALSE, FALSE, SysFnTCNUL_EM_YY    , 0              },    // Terminal Control Character, Null
    {WS_UTF16_QUAD L"ts"       ,      0,      FALSE, TRUE , SysFnTS_EM_YY       , 0              },    // Time Stamp
    {WS_UTF16_QUAD L"wa"       ,      0,      FALSE, TRUE , SysFnWA_EM_YY       , 0              },    // Workspace Available

// Monadic or dyadic system functions
    {WS_UTF16_QUAD L"at"       ,      1,      FALSE, FALSE, SysFnAT_EM_YY       , 0              },    // Attributes
    {WS_UTF16_QUAD L"cr"       ,      1,      FALSE, TRUE , SysFnCR_EM_YY       , 0              },    // Canonical Representation
    {WS_UTF16_QUAD L"dl"       ,      1,      FALSE, TRUE , SysFnDL_EM_YY       , 0              },    // Delay Execution
    {WS_UTF16_QUAD L"dr"       ,      1,      FALSE, FALSE, SysFnDR_EM_YY       , 0              },    // Data Representation
    {WS_UTF16_QUAD L"ea"       ,      1,      FALSE, TRUE , SysFnEA_EM_YY       , 0              },    // Execute Alternate
    {WS_UTF16_QUAD L"ec"       ,      1,      FALSE, TRUE , SysFnEC_EM_YY       , 0              },    // Execute Controlled
    {WS_UTF16_QUAD L"error"    ,      1,      FALSE, FALSE, SysFnERROR_EM_YY    , 0              },    // Signal Error
    {WS_UTF16_QUAD L"es"       ,      1,      FALSE, TRUE , SysFnES_EM_YY       , 0              },    // Event Simulate
    {WS_UTF16_QUAD L"ex"       ,      1,      FALSE, TRUE , SysFnEX_EM_YY       , 0              },    // Expunge Names
////{WS_UTF16_QUAD L"fi"       ,      1,      FALSE, FALSE, SysFnFI_EM_YY       , 0              },    // Format Items
    {WS_UTF16_QUAD L"fmt"      ,      1,      FALSE, FALSE, SysFnFMT_EM_YY      , 0              },    // Format
    {WS_UTF16_QUAD L"fx"       ,      1,      FALSE, TRUE , SysFnFX_EM_YY       , 0              },    // Function Fix
    {WS_UTF16_QUAD L"mf"       ,      1,      FALSE, FALSE, SysFnMF_EM_YY       , 0              },    // Monitor Function
    {WS_UTF16_QUAD L"nappend"  ,      1,      FALSE, FALSE, SysFnNAPPEND_EM_YY  , 0              },    // Append Data To An Open Native File
    {WS_UTF16_QUAD L"nc"       ,      1,      FALSE, TRUE , SysFnNC_EM_YY       , 0              },    // Name Classification
    {WS_UTF16_QUAD L"ncreate"  ,      1,      FALSE, FALSE, SysFnNCREATE_EM_YY  , 0              },    // Create And Open A Native File
    {WS_UTF16_QUAD L"nerase"   ,      1,      FALSE, FALSE, SysFnNERASE_EM_YY   , 0              },    // Erase An Open Native File
    {WS_UTF16_QUAD L"nl"       ,      1,      FALSE, TRUE , SysFnNL_EM_YY       , 0              },    // Name List
    {WS_UTF16_QUAD L"nlock"    ,      1,      FALSE, FALSE, SysFnNLOCK_EM_YY    , 0              },    // Lock An Open Native File
    {WS_UTF16_QUAD L"nread"    ,      1,      FALSE, FALSE, SysFnNREAD_EM_YY    , 0              },    // Read Data From An Open Native File
    {WS_UTF16_QUAD L"nrename"  ,      1,      FALSE, FALSE, SysFnNRENAME_EM_YY  , 0              },    // Rename An Open Native File
    {WS_UTF16_QUAD L"nreplace" ,      1,      FALSE, FALSE, SysFnNREPLACE_EM_YY , 0              },    // Replace Data In An Open Native File
    {WS_UTF16_QUAD L"nresize"  ,      1,      FALSE, FALSE, SysFnNRESIZE_EM_YY  , 0              },    // Resize An Open Native File
    {WS_UTF16_QUAD L"nsize"    ,      1,      FALSE, FALSE, SysFnNSIZE_EM_YY    , 0              },    // Get The Size Of An Open Native File
    {WS_UTF16_QUAD L"ntie"     ,      1,      FALSE, FALSE, SysFnNTIE_EM_YY     , 0              },    // Open A Native File
    {WS_UTF16_QUAD L"nuntie"   ,      1,      FALSE, FALSE, SysFnNUNTIE_EM_YY   , 0              },    // Close A Native File
////{WS_UTF16_QUAD L"stop"     ,      1,      FALSE, TRUE , SysFnSTOP_EM_YY     , 0              },    // Manage Stop Points
    {WS_UTF16_QUAD L"tf"       ,      1,      FALSE, TRUE , SysFnTF_EM_YY       , 0              },    // Transfer Form
////{WS_UTF16_QUAD L"trace"    ,      1,      FALSE, TRUE , SysFnTRACE_EM_YY    , 0              },    // Manage Trace Points
    {WS_UTF16_QUAD L"ucs"      ,      1,      FALSE, FALSE, SysFnUCS_EM_YY      , 0              },    // Universal Character Set
////{WS_UTF16_QUAD L"vi"       ,      1,      FALSE, FALSE, SysFnVI_EM          , 0              },    // Verify Items
    {WS_UTF16_QUAD L"vr"       ,      1,      FALSE, FALSE, SysFnVR_EM_YY       , 0              },    // Vector Representation of a Function
};

// The # rows in the above table
#define ASYSTEMNAMES_NROWS  countof (aSystemNames)


//***************************************************************************
//  $MakePermVars
//
//  Make various permanent variables
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakePermVars"
#else
#define APPEND_NAME
#endif

void MakePermVars
    (void)

{
    LPVARARRAY_HEADER lpHeader;     // Ptr to array header

    //***************************************************************
    // Create []A
    //***************************************************************

#define ALPHABET    L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define ALPHANELM   strcountof (ALPHABET)

    // Note, we can't use DbgGlobalAlloc here as the
    //   PTD has not been allocated as yet
    hGlbQuadA = MyGlobalAlloc (GHND, (APLU3264) CalcArraySize (ARRAY_CHAR, ALPHANELM, 1));
    if (!hGlbQuadA)
    {
        DbgStop ();         // We should never get here
    } // End IF

    // Lock the memory to get a ptr to it
    lpHeader = MyGlobalLock (hGlbQuadA);

    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
    lpHeader->PermNdx    = PERMNDX_QUADA;   // So we don't free it
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
////lpHeader->RefCnt     = 0;               // Ignore as this is perm
    lpHeader->NELM       = ALPHANELM;
    lpHeader->Rank       = 1;

    // Save the vector length
    *VarArrayBaseToDim (lpHeader) = ALPHANELM;

    // Skip over the header and dimensions to the data
    lpHeader = VarArrayDataFmBase (lpHeader);

    // Copy the data to the result
    CopyMemoryW (lpHeader, ALPHABET, ALPHANELM);

    // We no longer need this ptr
     MyGlobalUnlock (hGlbQuadA); lpHeader = NULL;

    //***************************************************************
    // Create zilde
    //***************************************************************

    // Note, we can't use DbgGlobalAlloc here as the
    //   PTD has not been allocated as yet
    hGlbZilde = MyGlobalAlloc (GHND, (APLU3264) CalcArraySize (ARRAY_BOOL, 0, 1));
    if (!hGlbZilde)
    {
        DbgStop ();         // We should never get here
    } // End IF

    // Lock the memory to get a ptr to it
    lpHeader = MyGlobalLock (hGlbZilde);

    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_BOOL;
    lpHeader->PermNdx    = PERMNDX_ZILDE;   // So we don't free it
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
////lpHeader->RefCnt     = 0;               // Ignore as this is perm
////lpHeader->NELM       = 0;               // Already zero from GHND
    lpHeader->Rank       = 1;

    // Mark as zero length
////*VarArrayBaseToDim (lpHeader) = 0;      // Already zero from GHND

    // We no longer need this ptr
    MyGlobalUnlock (hGlbZilde); lpHeader = NULL;

    //***************************************************************
    // Create initial value for []EC[2] (0 x 0 Boolean matrix)
    //***************************************************************
    hGlb0by0 = MyGlobalAlloc (GHND, (APLU3264) CalcArraySize (ARRAY_BOOL, 0, 2));
    if (!hGlb0by0)
    {
        DbgStop ();         // We should never get here
    } // End IF

    // Lock the memory to get a ptr to it
    lpHeader = MyGlobalLock (hGlb0by0);

    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_BOOL;
    lpHeader->PermNdx    = PERMNDX_0BY0;    // So we don't free it
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
////lpHeader->RefCnt     = 0;               // Ignore as this is perm
////lpHeader->NELM       = 0;               // Already zero from GHND
    lpHeader->Rank       = 2;

    // Mark as shape 0 by 0
////(VarArrayBaseToDim (lpHeader))[0] = 0;  // Already zero from GHND
////(VarArrayBaseToDim (lpHeader))[1] = 0;  // Already zero from GHND

    // We no longer need this ptr
    MyGlobalUnlock (hGlb0by0); lpHeader = NULL;

    // Set the PTB
    hGlb0by0 = MakePtrTypeGlb (hGlb0by0);

    //***************************************************************
    // Create initial value for []EM (3 x 0 char matrix)
    //***************************************************************
    hGlb3by0 = MyGlobalAlloc (GHND, (APLU3264) CalcArraySize (ARRAY_CHAR, 0, 2));
    if (!hGlb3by0)
    {
        DbgStop ();         // We should never get here
    } // End IF

    // Lock the memory to get a ptr to it
    lpHeader = MyGlobalLock (hGlb3by0);

    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
    lpHeader->PermNdx    = PERMNDX_3BY0;    // So we don't free it
////lpHeader->SysVar     = FALSE;           // Already zero from GHND
////lpHeader->RefCnt     = 0;               // Ignore as this is perm
////lpHeader->NELM       = 0;               // Already zero from GHND
    lpHeader->Rank       = 2;

    // Mark as shape 3 by 0
    (VarArrayBaseToDim (lpHeader))[0] = 3;
////(VarArrayBaseToDim (lpHeader))[1] = 0;  // Already zero from GHND

    // We no longer need this ptr
    MyGlobalUnlock (hGlb3by0); lpHeader = NULL;

    // Set the PTB
    hGlb3by0 = MakePtrTypeGlb (hGlb3by0);

    //***************************************************************
    // Create various permanent vectors
    //***************************************************************
    hGlbQuadxLX         = MakePermCharVector ($QUAD_DM            , PERMNDX_QUADxLX                      );
    hGlbV0Char          = MakePermCharVector (V0Char              , PERMNDX_V0CHAR                       );
    hGlbSAEmpty         = hGlbV0Char;
    hGlbSAClear         = MakePermCharVector (SAClear             , PERMNDX_SACLEAR                      );
    hGlbSAError         = MakePermCharVector (SAError             , PERMNDX_SAERROR                      );
    hGlbSAExit          = MakePermCharVector (SAExit              , PERMNDX_SAEXIT                       );
    hGlbSAOff           = MakePermCharVector (SAOff               , PERMNDX_SAOFF                        );
    hGlbQuadWSID_CWS    = hGlbV0Char;
    hGlbQuadFC_SYS      = MakePermCharVector (DEF_QUADFC_CWS      , PERMNDX_QUADFC                       );
    hGlbQuadFEATURE_SYS = MakePermIntVector  (DEF_QUADFEATURE_CWS , PERMNDX_QUADFEATURE , FEATURENDX_LENGTH );
    hGlbQuadIC_SYS      = MakePermIntVector  (DEF_QUADIC_CWS      , PERMNDX_QUADIC      , ICNDX_LENGTH   );

    // Create []AV
    MakeQuadAV ();
} // End MakePermVars
#undef  APPEND_NAME


//***************************************************************************
//  $MakePermCharVector
//
//  Make a permanent character vector
//***************************************************************************

HGLOBAL MakePermCharVector
    (LPAPLCHAR lpwc,                // Ptr to char vector
     PERM_NDX  permNdx)             // PERM_NDX value

{
    return MakePermVectorCom (lpwc,
                              permNdx,
                              ARRAY_CHAR,
                              sizeof (APLCHAR),
                              lstrlenW (lpwc));
} // End MakePermCharVector


//***************************************************************************
//  $MakePermIntVector
//
//  Make a permanent integer vector
//***************************************************************************

HGLOBAL MakePermIntVector
    (LPAPLINT lpwc,                 // Ptr to int vector
     PERM_NDX permNdx,              // PERM_NDX value
     UINT     uLen)                 // Length of the integer vector

{
    return MakePermVectorCom (lpwc,
                              permNdx,
                              ARRAY_INT,
                              sizeof (APLINT),
                              uLen);
} // End MakePermIntVector


//***************************************************************************
//  $MakePermVectorCom
//
//  Make a permanent vector
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- MakePermVectorCom"
#else
#define APPEND_NAME
#endif

HGLOBAL MakePermVectorCom
    (LPVOID   lpwc,                 // Ptr to common vector
     PERM_NDX permNdx,              // PERM_NDX value
     APLSTYPE aplTypeCom,           // Common array storage type
     UINT     aplSizeCom,           // Size of each common item
     UINT     uLen)                 // Length of the common value

{
    HGLOBAL hGlbRes;
    LPVARARRAY_HEADER lpHeader;

    // Note, we can't use DbgGlobalAlloc here as the
    //   PTD has not been allocated as yet
    hGlbRes = MyGlobalAlloc (GHND, (APLU3264) CalcArraySize (aplTypeCom, uLen, 1));
    if (!hGlbRes)
    {
        DbgStop ();         // We should never get here
    } // End IF

    // Lock the memory to get a ptr to it
    lpHeader = MyGlobalLock (hGlbRes);

    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = aplTypeCom;
    lpHeader->PermNdx    = permNdx; // So we don't free it
////lpHeader->SysVar     = FALSE;   // Already zero from GHND
////lpHeader->RefCnt     = 0;       // Ignore as this is perm
    lpHeader->NELM       = uLen;
    lpHeader->Rank       = 1;

    // Save the dimension
    *VarArrayBaseToDim (lpHeader) = uLen;

    // Skip over the header and dimensions to the data
    lpHeader = VarArrayDataFmBase (lpHeader);

    // Copy the data to memory
    CopyMemory (lpHeader, lpwc, uLen * aplSizeCom);

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpHeader = NULL;

    return hGlbRes;
} // End MakePermVectorCom
#undef  APPEND_NAME


//***************************************************************************
//  $SymTabAppendOneSysName_EM
//
//  Append a system name to the symbol table
//***************************************************************************

UBOOL SymTabAppendOneSysName_EM
    (LPSYSNAME   lpSysName,
     LPSYMENTRY *lplpSymEntry,
     LPHSHTABSTR lpHTS)                 // Ptr to HshTabStr

{
    STFLAGS    stFlags = {0};
    LPSYMENTRY lpSymEntry;

    // Set the flags of the entry we're appending
    if (lpSysName->bSysVar)
    {
        stFlags.stNameType  = NAMETYPE_VAR;
        stFlags.DfnSysLabel = (lpSysName->uValence EQ SYSLBL);
        stFlags.StdSysName  = lpSysName->bStdSysName;
    } else
    {
        Assert (IsBooleanValue (lpSysName->uValence));

        if (lpSysName->uValence EQ 0)
            stFlags.stNameType = NAMETYPE_FN0;
        else
            stFlags.stNameType = NAMETYPE_FN12;
        stFlags.FcnDir = TRUE;
    } // End IF/ELSE

    // Set the flags for what we're appending
    stFlags.Inuse   = TRUE;
    stFlags.ObjName = OBJNAME_SYS;

    // Append the name as new
    lpSymEntry =
      _SymTabAppendNewName_EM (lpSysName->lpwszName, &stFlags, lpHTS);

    // Check for error
    if (!lpSymEntry)
        return FALSE;

    // Save the LPSYMENTRY if requested
    if (lplpSymEntry)
        *lplpSymEntry = lpSymEntry;

    // Save the address of the execution routine
    lpSymEntry->stData.stNameFcn = lpSysName->lpNameFcn;

    return TRUE;
} // End SymTabAppendOneSysName_EM


//***************************************************************************
//  $SymTabAppendAllSysNames_EM
//
//  Append all system names to the symbol table
//***************************************************************************

UBOOL SymTabAppendAllSysNames_EM
    (LPHSHTABSTR lpHTS)                 // Ptr to HshTabStr

{
    int i;                      // Loop counter

    for (i = 0; i < ASYSTEMNAMES_NROWS; i++)
    if (!SymTabAppendOneSysName_EM (&aSystemNames[i],
                                    &lpHTS->lpSymQuad[aSystemNames[i].sysVarIndex],
                                     lpHTS))
        return FALSE;

    return TRUE;
} // End SymTabAppendAllSysNames_EM


//***************************************************************************
//  $InitSystemNames_EM
//
//  Append all system names to the symbol table
//***************************************************************************

UBOOL InitSystemNames_EM
    (void)

{
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPHSHTABSTR  lphtsPTD;          // Ptr to HshTab struc
    UBOOL        bRet = TRUE;       // TRUE iff result is valid

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get a ptr to the HshTab struc
    lphtsPTD = lpMemPTD->lphtsPTD;

    Assert (HshTabFrisk (lphtsPTD));

    // Append all system names
    bRet = SymTabAppendAllSysNames_EM (lphtsPTD);
    if (bRet)
        lphtsPTD->bSysNames = TRUE;

    Assert (HshTabFrisk (lphtsPTD));

    return bRet;
} // End InitSystemNames_EM


//***************************************************************************
//  $AssignGlobalCWS
//
//  Assign a global to a name
//***************************************************************************

void AssignGlobalCWS
    (HGLOBAL    hGlbVal_CWS,
     UINT       SysVarValid,
     LPSYMENTRY lpSymEntryDest)

{
    // Make a copy of the CLEAR WS value, and
    //   save the global memory ptr
    lpSymEntryDest->stData.stGlbData = CopySymGlbDirAsGlb (hGlbVal_CWS);

    // Save the validate index
    lpSymEntryDest->stFlags.SysVarValid = SysVarValid;

    // Mark as having a value
    lpSymEntryDest->stFlags.Value = TRUE;
} // End AssignGlobalCWS


//***************************************************************************
//  $AssignBoolScalarCWS
//
//  Assign an Boolean scalar to a name
//***************************************************************************

void AssignBoolScalarCWS
    (APLBOOL    aplBoolean,
     UINT       SysVarValid,
     LPSYMENTRY lpSymEntryDest)

{
    STFLAGS stFlags = {0};

    // Save the constant
    lpSymEntryDest->stData.stBoolean = aplBoolean;

    // Mark as immediate Boolean constant
    stFlags.Imm     = TRUE;
    stFlags.ImmType = IMMTYPE_BOOL;
    stFlags.Value   = TRUE;

    // Save the flags
    stFlags.SysVarValid = SysVarValid;
    *(UINT *) &lpSymEntryDest->stFlags |= *(UINT *) &stFlags;
} // End AssignBoolScalarCWS


//***************************************************************************
//  $AssignIntScalarCWS
//
//  Assign an integer scalar to a name
//***************************************************************************

void AssignIntScalarCWS
    (APLINT     aplInteger,
     UINT       SysVarValid,
     LPSYMENTRY lpSymEntryDest)

{
    STFLAGS stFlags = {0};

    // Save the constant
    lpSymEntryDest->stData.stInteger = aplInteger;

    // Mark as immediate Integer constant
    stFlags.Imm     = TRUE;
    stFlags.ImmType = IMMTYPE_INT;
    stFlags.Value   = TRUE;

    // Save the flags
    stFlags.SysVarValid = SysVarValid;
    *(UINT *) &lpSymEntryDest->stFlags |= *(UINT *) &stFlags;
} // End AssignIntScalarCWS


//***************************************************************************
//  $AssignRealScalarCWS
//
//  Assign a real number scalar to a name
//***************************************************************************

void AssignRealScalarCWS
    (APLFLOAT   fFloat,
     UINT       SysVarValid,
     LPSYMENTRY lpSymEntryDest)

{
    STFLAGS stFlags = {0};

    // Save the constant
    lpSymEntryDest->stData.stFloat = fFloat;

    // Mark as immediate floating point constant
    stFlags.Imm     = TRUE;
    stFlags.ImmType = IMMTYPE_FLOAT;
    stFlags.Value   = TRUE;

    // Save the flags
    stFlags.SysVarValid = SysVarValid;
    *(UINT *) &lpSymEntryDest->stFlags |= *(UINT *) &stFlags;
} // End AssignRealScalarCWS


//***************************************************************************
//  $AssignCharScalarCWS
//
//  Assign a character scalar to a name
//***************************************************************************

void AssignCharScalarCWS
    (APLCHAR    aplChar,
     UINT       SysVarValid,
     LPSYMENTRY lpSymEntryDest)

{
    STFLAGS stFlags = {0};

    // Save the constant
    lpSymEntryDest->stData.stChar = aplChar;

    // Mark as immediate Character constant
    stFlags.Imm     = TRUE;
    stFlags.ImmType = IMMTYPE_CHAR;
    stFlags.Value   = TRUE;

    // Save the flags
    stFlags.SysVarValid = SysVarValid;
    *(UINT *) &lpSymEntryDest->stFlags |= *(UINT *) &stFlags;
} // End AssignCharScalarCWS


//***************************************************************************
//  $ValidateCharDT_EM
//
//  Validate a value about to be assigned to an character system var.
//
//  We allow any character scalar or one-element vector.
//
//  The order of error checking is RANK, LENGTH, DOMAIN.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ValidateCharDT_EM"
#else
#define APPEND_NAME
#endif

UBOOL ValidateCharDT_EM
    (LPTOKEN  lptkNamArg,           // Ptr to name token
     LPTOKEN  lptkExpr,             // Ptr to value token
     APLCHAR  cDefault)             // Default value

{
    APLSTYPE aplTypeRht;            // Right arg storage type
    APLNELM  aplNELMRht;            // Right arg NELM
    APLRANK  aplRankRht;            // Right arg rank
    HGLOBAL  hGlbRht = NULL;        // Right arg global memory handle
    LPVOID   lpMemRht = NULL;       // Ptr to right arg global memory
    UBOOL    bRet = FALSE;          // TRUE iff the result is valid
    APLCHAR  aplChar;

    // Split cases based upon the token type
    switch (lptkExpr->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkExpr->tkData.tkVoid) EQ PTRTYPE_STCONST);

            if (!lptkExpr->tkData.tkSym->stFlags.Imm)
            {
                // Get the HGLOBAL
                hGlbRht = lptkExpr->tkData.tkSym->stData.stGlbData;

                break;      // Continue with HGLOBAL processing
            } // End IF

            // Handle the immediate case

            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkExpr->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // Split cases based upon the symbol table immediate type
            switch (lptkExpr->tkData.tkSym->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                case IMMTYPE_INT:
                case IMMTYPE_FLOAT:
                    break;

                case IMMTYPE_CHAR:
                    // Get the value
                    aplChar = lptkExpr->tkData.tkSym->stData.stChar;

                    // Test the value
                    bRet = (strchrW (DEF_QUADDT_ALLOW, aplChar) NE NULL);

                    break;
            } // End SWITCH

            if (bRet)
                goto NORMAL_EXIT;
            else
                goto DOMAIN_EXIT;

        case TKT_VARIMMED:
            // Split cases based upon the token immediate type
            switch (lptkExpr->tkFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                case IMMTYPE_INT:
                case IMMTYPE_FLOAT:
                    break;

                case IMMTYPE_CHAR:
                    // Get the value
                    aplChar = lptkExpr->tkData.tkChar;

                    // Test the value
                    bRet = (strchrW (DEF_QUADDT_ALLOW, aplChar) NE NULL);

                    break;
            } // End SWITCH

            if (bRet)
                goto NORMAL_EXIT;
            else
                goto DOMAIN_EXIT;

        case TKT_LISTPAR:   // The tkData is an HGLOBAL of an array of HGLOBALs
            goto SYNTAX_EXIT;

        case TKT_CHRSTRAND: // tkData is an HGLOBAL of an array of ???
        case TKT_NUMSTRAND: // tkData is an HGLOBAL of an array of ???
        case TKT_VARARRAY:  // tkData is an HGLOBAL of an array of ???
            // Get the HGLOBAL
            hGlbRht = lptkExpr->tkData.tkGlbData;

            break;          // Continue with HGLOBAL processing

        defstop
            goto DOMAIN_EXIT;
    } // End SWITCH

    // st/tkData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir_PTB (hGlbRht));

    // Clear the type bits
    hGlbRht = ClrPtrTypeDir (hGlbRht);

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (hGlbRht);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRht)
    // Get the Array Type, NELM, and Rank
    aplTypeRht = lpHeader->ArrType;
    aplNELMRht = lpHeader->NELM;
    aplRankRht = lpHeader->Rank;
#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayDataFmBase (lpMemRht);

    // Check for scalar or vector
    if (IsMultiRank (aplRankRht))
        goto RANK_EXIT;
    else
    // Check for singleton or empty
    if (IsMultiNELM (aplNELMRht))
        goto LENGTH_EXIT;
    else
    if (IsEmpty (aplNELMRht))
    {
        // Must be simple to be valid
        bRet = IsSimple (aplTypeRht);
        if (bRet)
            // Use the system default value
            aplChar = cDefault;
    } else
    // Split cases based upon the array type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            break;

        case ARRAY_CHAR:
            // Get the value
            aplChar = *(LPAPLCHAR) lpMemRht;

            // Test the value
            bRet = (strchrW (DEF_QUADDT_ALLOW, aplChar) NE NULL);

            break;

        defstop
            break;
    } // End IF/ELSE/SWITCH

    if (!bRet)
        goto DOMAIN_EXIT;
NORMAL_EXIT:
    // Save the value in the name
    lptkNamArg->tkData.tkSym->stData.stChar = aplChar;
    lptkNamArg->tkFlags.NoDisplay = TRUE;

    goto UNLOCK_EXIT;

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkExpr);
    goto ERROR_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkExpr);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkExpr);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkExpr);
    goto ERROR_EXIT;

ERROR_EXIT:
UNLOCK_EXIT:
    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return bRet;
} // End ValidateCharDT_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ValidateInteger_EM
//
//  Validate a value about to be assigned to an integer system var.
//
//  We allow any numeric scalar or one-element vector whose value
//    is integral and in a given range.
//
//  The order of error checking is RANK, LENGTH, DOMAIN.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ValidateInteger_EM"
#else
#define APPEND_NAME
#endif

UBOOL ValidateInteger_EM
    (LPTOKEN  lptkNamArg,           // Ptr to name token
     LPTOKEN  lptkExpr,             // Ptr to value token
     APLINT   iValidLo,             // Low range value (inclusive)
     APLINT   iDefault,             // Default   ...
     APLINT   iValidHi,             // High range ...
     UBOOL    bRangeLimit)          // TRUE iff an incoming value outside
                                    //   the given range [uValidLo, uValidHi]
                                    //   is adjusted to be the closer range limit

{
    APLSTYPE aplTypeRht;            // Right arg storage type
    APLNELM  aplNELMRht;            // Right arg NELM
    APLRANK  aplRankRht;            // Right arg rank
    HGLOBAL  hGlbRht = NULL;        // Right arg global memory handle
    LPVOID   lpMemRht = NULL;       // Ptr to right arg global memory
    UBOOL    bRet = FALSE;          // TRUE iff the result is valid
    APLINT   aplInteger;

    // Split cases based upon the token type
    switch (lptkExpr->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkExpr->tkData.tkVoid) EQ PTRTYPE_STCONST);

            if (!lptkExpr->tkData.tkSym->stFlags.Imm)
            {
                // Get the HGLOBAL
                hGlbRht = lptkExpr->tkData.tkSym->stData.stGlbData;

                break;      // Continue with HGLOBAL processing
            } // End IF

            // Handle the immediate case

            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkExpr->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // Split cases based upon the symbol table immediate type
            switch (lptkExpr->tkData.tkSym->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    // Get the value
                    aplInteger = lptkExpr->tkData.tkSym->stData.stBoolean;

                    // Test the value
                    bRet = ValidateIntegerTest (&aplInteger,        // Ptr to the integer to test
                                                 iValidLo,          // Low range value (inclusive)
                                                 iValidHi,          // High ...
                                                 bRangeLimit);      // TRUE iff we're range limiting
                    break;

                case IMMTYPE_INT:
                    // Get the value
                    aplInteger = lptkExpr->tkData.tkSym->stData.stInteger;

                    // Test the value
                    bRet = ValidateIntegerTest (&aplInteger,        // Ptr to the integer to test
                                                 iValidLo,          // Low range value (inclusive)
                                                 iValidHi,          // High ...
                                                 bRangeLimit);      // TRUE iff we're range limiting
                    break;

                case IMMTYPE_FLOAT:
                    // Attempt to convert the float to an integer using System []CT
                    aplInteger = FloatToAplint_SCT (lptkExpr->tkData.tkSym->stData.stFloat,
                                                   &bRet);
                    // Test the value
                    if (bRangeLimit || bRet)
                        bRet = ValidateIntegerTest (&aplInteger,    // Ptr to the integer to test
                                                     iValidLo,      // Low range value (inclusive)
                                                     iValidHi,      // High ...
                                                     bRangeLimit);  // TRUE iff we're range limiting
                    break;

                case IMMTYPE_CHAR:
                    break;
            } // End SWITCH

            if (bRet)
                goto NORMAL_EXIT;
            else
                goto DOMAIN_EXIT;

        case TKT_VARIMMED:
            // Split cases based upon the token immediate type
            switch (lptkExpr->tkFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    // Get the value
                    aplInteger = lptkExpr->tkData.tkBoolean;

                    // Test the value
                    bRet = ValidateIntegerTest (&aplInteger,        // Ptr to the integer to test
                                                 iValidLo,          // Low range value (inclusive)
                                                 iValidHi,          // High ...
                                                 bRangeLimit);      // TRUE iff we're range limiting
                    break;

                case IMMTYPE_INT:
                    // Get the value
                    aplInteger = lptkExpr->tkData.tkInteger;

                    // Test the value
                    bRet = ValidateIntegerTest (&aplInteger,        // Ptr to the integer to test
                                                 iValidLo,          // Low range value (inclusive)
                                                 iValidHi,          // High ...
                                                 bRangeLimit);      // TRUE iff we're range limiting
                    break;

                case IMMTYPE_FLOAT:
                    // Attempt to convert the float to an integer using System []CT
                    aplInteger = FloatToAplint_SCT (lptkExpr->tkData.tkFloat,
                                                   &bRet);
                    // Test the value
                    if (bRangeLimit || bRet)
                        bRet = ValidateIntegerTest (&aplInteger,    // Ptr to the integer to test
                                                     iValidLo,      // Low range value (inclusive)
                                                     iValidHi,      // High ...
                                                     bRangeLimit);  // TRUE iff we're range limiting
                    break;

                case IMMTYPE_CHAR:
                    break;
            } // End SWITCH

            if (bRet)
                goto NORMAL_EXIT;
            else
                goto DOMAIN_EXIT;

        case TKT_LISTPAR:   // The tkData is an HGLOBAL of an array of HGLOBALs
            goto SYNTAX_EXIT;

        case TKT_CHRSTRAND: // tkData is an HGLOBAL of an array of ???
        case TKT_NUMSTRAND: // tkData is an HGLOBAL of an array of ???
        case TKT_NUMSCALAR: // tkData is an HGLOBAL of an array of ???
        case TKT_VARARRAY:  // tkData is an HGLOBAL of an array of ???
            // Get the HGLOBAL
            hGlbRht = lptkExpr->tkData.tkGlbData;

            break;          // Continue with HGLOBAL processing

        defstop
            goto DOMAIN_EXIT;
    } // End SWITCH

    // st/tkData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir_PTB (hGlbRht));

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (hGlbRht);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRht)
    // Get the Array Type, NELM, and Rank
    aplTypeRht = lpHeader->ArrType;
    aplNELMRht = lpHeader->NELM;
    aplRankRht = lpHeader->Rank;
#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayDataFmBase (lpMemRht);

    // Check for scalar or vector
    if (IsMultiRank (aplRankRht))
        goto RANK_EXIT;
    else
    // Check for singleton or empty
    if (IsMultiNELM (aplNELMRht))
        goto LENGTH_EXIT;
    else
    if (IsEmpty (aplNELMRht))
    {
        // Must be simple to be valid
        bRet = IsSimple (aplTypeRht);
        if (bRet)
            // Use the system default value
            aplInteger = iDefault;
    } else
    // Split cases based upon the array type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            // Get the value
            aplInteger = BIT0 & *(LPAPLBOOL) lpMemRht;

            // Test the value
            bRet = ValidateIntegerTest (&aplInteger,                // Ptr to the integer to test
                                         iValidLo,                  // Low range value (inclusive)
                                         iValidHi,                  // High ...
                                         bRangeLimit);              // TRUE iff we're range limiting
            break;

        case ARRAY_INT:
            // Get the value
            aplInteger = *(LPAPLINT) lpMemRht;

            // Test the value
            bRet = ValidateIntegerTest (&aplInteger,                // Ptr to the integer to test
                                         iValidLo,                  // Low range value (inclusive)
                                         iValidHi,                  // High ...
                                         bRangeLimit);              // TRUE iff we're range limiting
            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            break;

        case ARRAY_FLOAT:
            // Attempt to convert the float to an integer using System []CT
            aplInteger = FloatToAplint_SCT (*(LPAPLFLOAT) lpMemRht,
                                           &bRet);
            // Test the value
            if (bRangeLimit || bRet)
                bRet = ValidateIntegerTest (&aplInteger,            // Ptr to the integer to test
                                             iValidLo,              // Low range value (inclusive)
                                             iValidHi,              // High ...
                                             bRangeLimit);          // TRUE iff we're range limiting
            break;

        case ARRAY_RAT:
            // Attempt to convert the RAT to an integer
            aplInteger = mpq_get_sx ((LPAPLRAT) lpMemRht, &bRet);

            // Test the value
            if (bRangeLimit || bRet)
                bRet = ValidateIntegerTest (&aplInteger,            // Ptr to the integer to test
                                             iValidLo,              // Low range value (inclusive)
                                             iValidHi,              // High ...
                                             bRangeLimit);          // TRUE iff we're range limiting
            break;

        case ARRAY_VFP:
            // Attempt to convert the VFP to an integer
            aplInteger = mpfr_get_sx ((LPAPLVFP) lpMemRht, &bRet);

            // Test the value
            if (bRangeLimit || bRet)
                bRet = ValidateIntegerTest (&aplInteger,            // Ptr to the integer to test
                                             iValidLo,              // Low range value (inclusive)
                                             iValidHi,              // High ...
                                             bRangeLimit);          // TRUE iff we're range limiting
            break;

        defstop
            break;
    } // End IF/ELSE/SWITCH

    if (!bRet)
        goto DOMAIN_EXIT;
NORMAL_EXIT:
    // Save the value in the name
    lptkNamArg->tkData.tkSym->stData.stInteger = aplInteger;
    lptkNamArg->tkFlags.NoDisplay = TRUE;

    goto UNLOCK_EXIT;

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkExpr);
    goto ERROR_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkExpr);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkExpr);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkExpr);
    goto ERROR_EXIT;

ERROR_EXIT:
UNLOCK_EXIT:
    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return bRet;
} // End ValidateInteger_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ValidateIntegerTest
//
//  Validate an integer within a given range, possibly range limited
//***************************************************************************

UBOOL ValidateIntegerTest
    (LPAPLINT lpaplInteger,         // Ptr to the integer to test
     APLINT   iValidLo,             // Low range value (inclusive)
     APLINT   iValidHi,             // High ...
     UBOOL    bRangeLimit)          // TRUE iff an incoming value outside
                                    //   the given range [uValidLo, uValidHi]
                                    //   is adjusted to be the closer range limit

{
    // If we're range limiting, ...
    if (bRangeLimit)
    {
        // If it's too small, use the lower limit
        if (*lpaplInteger < iValidLo)
            *lpaplInteger = iValidLo;
        // If it's too large, use the upper limit
        if (*lpaplInteger > iValidHi)
            *lpaplInteger = iValidHi;
        return TRUE;
    } else
        return (iValidLo <= *lpaplInteger
             &&             *lpaplInteger <= iValidHi);
} // End ValidateIntegerTest


//***************************************************************************
//  $ValidateFloat_EM
//
//  Validate a value about to be assigned to a float system var.
//
//  We allow any numeric scalar or one-element vector whose value
//    is in a given range.
//
//  The order of error checking is RANK, LENGTH, DOMAIN.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ValidateFloat_EM"
#else
#define APPEND_NAME
#endif

UBOOL ValidateFloat_EM
    (LPTOKEN  lptkNamArg,           // Ptr to name token
     LPTOKEN  lpToken,              // Ptr to value token
     APLFLOAT fValidLo,             // Low range value (inclusive)
     APLFLOAT fDefault,             // Default   ...
     APLFLOAT fValidHi,             // High range ...
     UBOOL    bRangeLimit)          // TRUE iff an incoming value outside
                                    //   the given range [uValidLo, uValidHi]
                                    //   is adjusted to be the closer range limit

{
    HGLOBAL  hGlbRht = NULL;        // Right arg global memory handle
    LPVOID   lpMemRht = NULL;       // Ptr to right arg global memory
    APLSTYPE aplTypeRht;            // Right arg storage type
    APLNELM  aplNELMRht;            // Right arg NELM
    APLRANK  aplRankRht;            // Right arg rank
    UBOOL    bRet = FALSE;          // TRUE iff result is valid
    APLFLOAT aplFloat;              // Temporary float

    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            if (!lpToken->tkData.tkSym->stFlags.Imm)
            {
                // Get the HGLOBAL
                hGlbRht = lpToken->tkData.tkSym->stData.stGlbData;

                break;      // Continue with HGLOBAL processing
            } // End IF

            // Handle the immediate case

            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // Split cases based upon the symbol table immediate type
            switch (lpToken->tkData.tkSym->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    // Get the value
                    aplFloat = lpToken->tkData.tkSym->stData.stBoolean;

                    // Test the value
                    bRet = ValidateFloatTest (&aplFloat, fValidLo, fValidHi, bRangeLimit);

                    break;

                case IMMTYPE_INT:
                    // Get the value
                    aplFloat = (APLFLOAT) (lpToken->tkData.tkSym->stData.stInteger);

                    // Test the value
                    bRet = ValidateFloatTest (&aplFloat, fValidLo, fValidHi, bRangeLimit);

                    break;

                case IMMTYPE_FLOAT:
                    // Get the value
                    aplFloat = lpToken->tkData.tkSym->stData.stFloat;

                    // Test the value
                    bRet = ValidateFloatTest (&aplFloat, fValidLo, fValidHi, bRangeLimit);

                    break;

                case IMMTYPE_CHAR:
                    break;
            } // End SWITCH

            if (bRet)
                goto NORMAL_EXIT;
            else
                goto DOMAIN_EXIT;

        case TKT_VARIMMED:
            // Split cases based upon the token immediate type
            switch (lpToken->tkFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                    // Get the value
                    aplFloat = lpToken->tkData.tkBoolean;

                    // Test the value
                    bRet = ValidateFloatTest (&aplFloat, fValidLo, fValidHi, bRangeLimit);

                    break;

                case IMMTYPE_INT:
                    // Get the value
                    aplFloat = (APLFLOAT) (lpToken->tkData.tkInteger);

                    // Test the value
                    bRet = ValidateFloatTest (&aplFloat, fValidLo, fValidHi, bRangeLimit);

                    break;

                case IMMTYPE_FLOAT:
                    // Get the value
                    aplFloat = lpToken->tkData.tkFloat;

                    // Test the value
                    bRet = ValidateFloatTest (&aplFloat, fValidLo, fValidHi, bRangeLimit);

                    break;

                case IMMTYPE_CHAR:
                    break;
            } // End SWITCH

            if (bRet)
                goto NORMAL_EXIT;
            else
                goto DOMAIN_EXIT;

        case TKT_LISTPAR:   // The tkData is an HGLOBAL of an array of HGLOBALs
            goto SYNTAX_EXIT;

        case TKT_CHRSTRAND: // tkData is an HGLOBAL of an array of ???
        case TKT_NUMSTRAND: // tkData is an HGLOBAL of an array of ???
        case TKT_NUMSCALAR: // tkData is an HGLOBAL of an array of ???
        case TKT_VARARRAY:  // tkData is an HGLOBAL of an array of ???
            // Get the HGLOBAL
            hGlbRht = lpToken->tkData.tkGlbData;

            break;          // Continue with HGLOBAL processing

        defstop
            goto DOMAIN_EXIT;
    } // End SWITCH

    // st/tkData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir_PTB (hGlbRht));

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (hGlbRht);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRht)
    // Get the Array Type, NELM, and Rank
    aplTypeRht = lpHeader->ArrType;
    aplNELMRht = lpHeader->NELM;
    aplRankRht = lpHeader->Rank;
#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayDataFmBase (lpMemRht);

    // Check for scalar or vector
    if (IsMultiRank (aplRankRht))
        goto RANK_EXIT;
    else
    // Check for singleton or empty
    if (IsMultiNELM (aplNELMRht))
        goto LENGTH_EXIT;
    else
    if (IsEmpty (aplNELMRht))
    {
        // Must be simple to be valid
        bRet = IsSimple (aplTypeRht);
        if (bRet)
            // Use the system default value
            aplFloat = fDefault;
    } else
    // Split cases based upon the array type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            // Get the value
            aplFloat = *(LPAPLBOOL) lpMemRht;

            // Test the value
            bRet = ValidateFloatTest (&aplFloat, fValidLo, fValidHi, bRangeLimit);

            break;

        case ARRAY_INT:
            // Get the value
            aplFloat = (APLFLOAT) *(LPAPLINT) lpMemRht;

            // Test the value
            bRet = ValidateFloatTest (&aplFloat, fValidLo, fValidHi, bRangeLimit);

            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            break;

        case ARRAY_FLOAT:
            // Get the value
            aplFloat = *(LPAPLFLOAT) lpMemRht;

            // Test the value
            bRet = ValidateFloatTest (&aplFloat, fValidLo, fValidHi, bRangeLimit);

            break;

        case ARRAY_RAT:
            // Convert the value to a float
            aplFloat = mpq_get_d ((LPAPLRAT) lpMemRht);

            // Test the value
            bRet = ValidateFloatTest (&aplFloat, fValidLo, fValidHi, bRangeLimit);

            break;

        case ARRAY_VFP:
            // Convert the value to a float
            aplFloat = mpfr_get_d ((LPAPLVFP) lpMemRht, MPFR_RNDN);

            // Test the value
            bRet = ValidateFloatTest (&aplFloat, fValidLo, fValidHi, bRangeLimit);

            break;

        defstop
            break;
    } // End IF/ELSE/SWITCH

    if (!bRet)
        goto NORMAL_EXIT;
NORMAL_EXIT:
    // Save the value in the name
    lptkNamArg->tkData.tkSym->stData.stFloat = aplFloat;
    lptkNamArg->tkFlags.NoDisplay = TRUE;

    goto UNLOCK_EXIT;

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lpToken);
    goto ERROR_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lpToken);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lpToken);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lpToken);
    goto ERROR_EXIT;

ERROR_EXIT:
UNLOCK_EXIT:
    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return bRet;
} // End ValidateFloat_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ValidateFloatTest
//
//  Validate a float within a given range, possibly range limited
//***************************************************************************

UBOOL ValidateFloatTest
    (LPAPLFLOAT lpaplFloat,         // Ptr to the float to test
     APLFLOAT   fValidLo,           // Low range value (inclusive)
     APLFLOAT   fValidHi,           // High ...
     UBOOL      bRangeLimit)        // TRUE iff an incoming value outside
                                    //   the given range [uValidLo, uValidHi]
                                    //   is adjusted to be the closer range limit

{
    // If we're range limiting, ...
    if (bRangeLimit)
    {
        // If it's too small, use the lower limit
        if (*lpaplFloat < fValidLo)
            *lpaplFloat = fValidLo;
        // If it's too large, use the upper limit
        if (*lpaplFloat > fValidHi)
            *lpaplFloat = fValidHi;
        return TRUE;
    } else
        return (fValidLo <= *lpaplFloat
             &&             *lpaplFloat <= fValidHi);
} // End ValidateFloatTest


//***************************************************************************
//  $ValidateCharVector_EM
//
//  Validate a value about to be assigned to a character vector system var.
//
//  We allow any character scalar or vector.
//
//  The order of error checking is RANK, LENGTH, DOMAIN.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ValidateCharVector_EM"
#else
#define APPEND_NAME
#endif

UBOOL ValidateCharVector_EM
    (LPTOKEN  lptkNamArg,           // Ptr to name token
     LPTOKEN  lpToken,              // Ptr to value token
     UBOOL    bWSID)                // TRUE iff this is []WSID

{
    HGLOBAL      hGlbRht = NULL,    // Right arg global memory handle
                 hGlbRes;           // Result    ...
    LPVOID       lpMemRht = NULL,   // Ptr to right arg global memory
                 lpMemRes;          // Ptr to result    ...
    UBOOL        bRet = FALSE,      // TRUE iff result is valid
                 bScalar = FALSE;   // TRUE iff right arg is scalar
    APLCHAR      aplChar;           // Right arg first char
    APLSTYPE     aplTypeRht;        // Right arg storage type
    APLNELM      aplNELMRht,        // Right arg NELM
                 aplNELMRes;        // Result    ...
    APLRANK      aplRankRht;        // Right arg rank
    APLUINT      ByteRes;           // # bytes in the result
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
    LPWCHAR      lpwszTemp;         // Ptr to temporary storage
    VARS_TEMP_OPEN

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Get ptr to temporary storage
    lpwszTemp = lpMemPTD->lpwszTemp;
    CHECK_TEMP_OPEN

    // Split cases based upon the token type
    switch (lpToken->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // Split cases based upon the symbol table immediate type
            if (!lpToken->tkData.tkSym->stFlags.Imm)
            {
                // Get the HGLOBAL
                hGlbRht = lpToken->tkData.tkSym->stData.stGlbData;

                break;      // Continue with HGLOBAL processing
            } // End IF

            // Handle the immediate case

            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lpToken->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // Split cases based upon the symbol table immediate type
            switch (lpToken->tkData.tkSym->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                case IMMTYPE_INT:
                case IMMTYPE_FLOAT:
                    goto DOMAIN_EXIT;

                case IMMTYPE_CHAR:
                    aplChar = lpToken->tkData.tkSym->stData.stChar;

                    bScalar = TRUE;

                    goto MAKE_VECTOR;
            } // End SWITCH

            break;

        case TKT_VARIMMED:
            // Split cases based upon the token immediate type
            switch (lpToken->tkFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                case IMMTYPE_INT:
                case IMMTYPE_FLOAT:
                    goto DOMAIN_EXIT;

                case IMMTYPE_CHAR:
                    aplChar = lpToken->tkData.tkChar;

                    bScalar = TRUE;

                    goto MAKE_VECTOR;
            } // End SWITCH

            goto NORMAL_EXIT;

        case TKT_LISTPAR:   // The tkData is an HGLOBAL of an array of HGLOBALs
            goto SYNTAX_EXIT;

        case TKT_CHRSTRAND: // tkData is an HGLOBAL of an array of ???
        case TKT_NUMSTRAND: // tkData is an HGLOBAL of an array of ???
        case TKT_NUMSCALAR: // tkData is an HGLOBAL of an array of ???
        case TKT_VARARRAY:  // tkData is an HGLOBAL of an array of ???
            // Get the HGLOBAL
            hGlbRht = lpToken->tkData.tkGlbData;

            break;          // Continue with HGLOBAL processing

        defstop
            goto DOMAIN_EXIT;
    } // End SWITCH

    // st/tkData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir_PTB (hGlbRht));

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (hGlbRht);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRht)
    // Get the right arg Type, NELM, and Rank
    aplTypeRht = lpHeader->ArrType;
    aplNELMRht = lpHeader->NELM;
    aplRankRht = lpHeader->Rank;
#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayDataFmBase (lpMemRht);

    // Check for scalar or vector
    if (IsMultiRank (aplRankRht))
        goto RANK_EXIT;
    else
    // Split cases based upon the array type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            goto DOMAIN_EXIT;

        case ARRAY_CHAR:
            aplChar = *(LPAPLCHAR) lpMemRht;

            bScalar = IsScalar (aplRankRht);

            break;

        defstop
            goto DOMAIN_EXIT;
    } // End IF/ELSE/SWITCH

    // If the argument is a scalar, make a vector out of it
    if (bScalar)
        goto MAKE_VECTOR;

    // If this is []WSID, expand the name
    if (bWSID)
    {
#define lpMemChar       ((LPAPLCHAR) lpMemRht)

        // Delete leading blanks
        aplNELMRes = aplNELMRht;
        while (aplNELMRes && *lpMemChar EQ L' ')
        {
            aplNELMRes--;
            lpMemChar++;
        } // End WHILE

        // Delete trailing blanks
        while (aplNELMRes && lpMemChar[aplNELMRes - 1] EQ L' ')
            aplNELMRes--;

#undef  lpMemChar

        // If there's anything left, ...
        if (aplNELMRes)
        {
            // Convert the []WSID workspace name into a canonical form
            MakeWorkspaceNameCanonical (lpwszTemp, lpMemRht, lpwszWorkDir);

            // Get length of the name as the NELM
            aplNELMRes = lstrlenW (lpwszTemp);

            goto ALLOC_VECTOR;
        } else
        {
            // The result is an empry char vector
            hGlbRes = hGlbV0Char;

            goto NORMAL_EXIT;
        } // End IF/ELSE
    } // End IF

    // Copy the right arg global as the result
    hGlbRes = CopySymGlbDirAsGlb (hGlbRht);

    goto NORMAL_EXIT;

MAKE_VECTOR:
    // If the []var is []WSID, expand the name to be fully-qualified
    if (bWSID)
    {
        WCHAR wsz[2];

        // Save the char as a string
        wsz[0] = aplChar;
        wsz[1] = WC_EOS;

        // Convert the []WSID workspace name into a canonical form
        MakeWorkspaceNameCanonical (lpwszTemp, wsz, lpwszWorkDir);

        // Get length of the name as the NELM
        aplNELMRes = lstrlenW (lpwszTemp);
    } else
        aplNELMRes = 1;
ALLOC_VECTOR:
    // Calculate space needed for the result
    //   (a character vector)
    ByteRes = CalcArraySize (ARRAY_CHAR, aplNELMRes, 1);

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto DOMAIN_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_CHAR;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Save the dimension
    *VarArrayBaseToDim (lpMemRes) = aplNELMRes;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemRes);

    if (bWSID)
        CopyMemoryW (lpMemRes, lpwszTemp, (APLU3264) aplNELMRes);
    else
        *((LPAPLCHAR) lpMemRes) = aplChar;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
NORMAL_EXIT:
    // Free the old value
    FreeResultGlobalVar (lptkNamArg->tkData.tkSym->stData.stGlbData); lptkNamArg->tkData.tkSym->stData.stGlbData = NULL;

    // Save as new value
    lptkNamArg->tkData.tkSym->stData.stGlbData = MakePtrTypeGlb (hGlbRes);
    lptkNamArg->tkFlags.NoDisplay = TRUE;

    // Mark as successful
    bRet = TRUE;

    goto UNLOCK_EXIT;

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lpToken);
    goto ERROR_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lpToken);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lpToken);
    goto ERROR_EXIT;

ERROR_EXIT:
UNLOCK_EXIT:
    EXIT_TEMP_OPEN

    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return bRet;
} // End ValidateCharVector_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ValidateIntegerVector_EM
//
//  Validate a value about to be assigned to a integer vector system var.
//
//  We allow any integer scalar or vector.
//
//  The order of error checking is RANK, LENGTH, DOMAIN.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ValidateIntegerVector_EM"
#else
#define APPEND_NAME
#endif

UBOOL ValidateIntegerVector_EM
    (LPTOKEN  lptkNamArg,           // Ptr to name token
     LPTOKEN  lptkRhtArg,           // Ptr to right arg token
     UINT     uValidLo,             // Low range value (inclusive)
     UINT     uValidHi,             // High ...
     UBOOL    bRangeLimit)          // TRUE iff an incoming value outside
                                    //   the given range [uValidLo, uValidHi]
                                    //   is adjusted to be the closer range limit

{
    HGLOBAL  hGlbRht = NULL,        // Right arg global memory handle
             hGlbRes;               // Result    ...
    LPVOID   lpMemRht = NULL,       // Ptr to right arg global memory
             lpMemIniRht,           // ...
             lpMemRes;              // Ptr to result    ...
    UBOOL    bRet = FALSE,          // TRUE iff result is valid
             bScalar = FALSE;       // TRUE iff right arg is a scalar
    APLINT   aplInteger,            // Right arg first integer
             apaOffRht,             // Right arg APA offset
             apaMulRht;             // Right arg APA multiplier
    APLSTYPE aplTypeRht;            // Right arg storage type
    APLNELM  aplNELMRht,            // Right arg NELM
             aplNELMRes;            // Result    ...
    APLRANK  aplRankRht;            // Right arg rank
    APLUINT  ByteRes,               // # bytes in the result
             uRht;                  // Loop counter
    UINT     uBitMask;              // Bit mask for looping through Booleans

    // Split cases based upon the token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // Split cases based upon the symbol table immediate type
            if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
            {
                // Get the HGLOBAL
                hGlbRht = lptkRhtArg->tkData.tkSym->stData.stGlbData;

                break;      // Continue with HGLOBAL processing
            } // End IF

            // Handle the immediate case

            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // Split cases based upon the symbol table immediate type
            switch (lptkRhtArg->tkData.tkSym->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                case IMMTYPE_INT:
                    aplInteger = lptkRhtArg->tkData.tkSym->stData.stInteger;

                    // Test the value
                    bRet = ValidateIntegerTest (&aplInteger,        // Ptr to the integer to test
                                                 uValidLo,          // Low range value (inclusive)
                                                 uValidHi,          // High ...
                                                 bRangeLimit);      // TRUE iff we're range limiting
                    if (bRet)
                    {
                        bScalar = TRUE;

                        goto MAKE_VECTOR;
                    } // End IF

                    break;

                case IMMTYPE_FLOAT:
                    // Attempt to convert the float to an integer using System []CT
                    aplInteger = FloatToAplint_SCT (lptkRhtArg->tkData.tkSym->stData.stFloat,
                                                   &bRet);
                    if (bRet)
                    {
                        // Test the value
                        bRet = ValidateIntegerTest (&aplInteger,    // Ptr to the integer to test
                                                     uValidLo,      // Low range value (inclusive)
                                                     uValidHi,      // High ...
                                                     bRangeLimit);  // TRUE iff we're range limiting
                        if (bRet)
                        {
                            bScalar = TRUE;

                            goto MAKE_VECTOR;
                        } // End IF
                    } // End IF

                    break;

                case IMMTYPE_CHAR:
                    break;

                defstop
                    break;
            } // End SWITCH

            goto DOMAIN_EXIT;

        case TKT_VARIMMED:
            // Split cases based upon the token immediate type
            switch (lptkRhtArg->tkFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                case IMMTYPE_INT:
                    aplInteger = lptkRhtArg->tkData.tkInteger;

                    // Test the value
                    bRet = ValidateIntegerTest (&aplInteger,        // Ptr to the integer to test
                                                 uValidLo,          // Low range value (inclusive)
                                                 uValidHi,          // High ...
                                                 bRangeLimit);      // TRUE iff we're range limiting
                    if (bRet)
                    {
                        bScalar = TRUE;

                        goto MAKE_VECTOR;
                    } // End IF

                    break;

                case IMMTYPE_FLOAT:
                    // Attempt to convert the float to an integer using System []CT
                    aplInteger = FloatToAplint_SCT (lptkRhtArg->tkData.tkFloat,
                                                   &bRet);
                    if (bRet)
                    {
                        // Test the value
                        bRet = ValidateIntegerTest (&aplInteger,    // Ptr to the integer to test
                                                     uValidLo,      // Low range value (inclusive)
                                                     uValidHi,      // High ...
                                                     bRangeLimit);  // TRUE iff we're range limiting
                        if (bRet)
                        {
                            bScalar = TRUE;

                            goto MAKE_VECTOR;
                        } // End IF
                    } // End IF

                    break;

                case IMMTYPE_CHAR:
                    break;

                defstop
                    break;
            } // End SWITCH

            goto DOMAIN_EXIT;

        case TKT_LISTPAR:   // The tkData is an HGLOBAL of an array of HGLOBALs
            goto SYNTAX_EXIT;

        case TKT_VARARRAY:  // tkData is an HGLOBAL of an array of ???
            // Get the HGLOBAL
            hGlbRht = lptkRhtArg->tkData.tkGlbData;

            break;          // Continue with HGLOBAL processing

        defstop
            goto DOMAIN_EXIT;
    } // End SWITCH

    // st/tkData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir_PTB (hGlbRht));

    // Lock the memory to get a ptr to it
    lpMemIniRht = lpMemRht = MyGlobalLock (hGlbRht);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRht)
    // Get the Array Type, NELM, and Rank
    aplTypeRht = lpHeader->ArrType;
    aplNELMRht = lpHeader->NELM;
    aplRankRht = lpHeader->Rank;
#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayDataFmBase (lpMemRht);

    // Ensure scalar or vector
    if (IsMultiRank (aplRankRht))
        goto RANK_EXIT;
    else
    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
            uBitMask = BIT0;

            // Test all the values
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Get the value
                aplInteger = (uBitMask & *(LPAPLBOOL) lpMemRht) ? TRUE : FALSE;

                // Test the value
                bRet = ValidateIntegerTest (&aplInteger,            // Ptr to the integer to test
                                             uValidLo,              // Low range value (inclusive)
                                             uValidHi,              // High ...
                                             bRangeLimit);          // TRUE iff we're range limiting
                if (!bRet)
                    goto DOMAIN_EXIT;

                // Shift over the bit mask
                uBitMask <<= 1;

                // Check for end-of-byte
                if (uBitMask EQ END_OF_BYTE)
                {
                    uBitMask = BIT0;            // Start over
                    ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                } // End IF
            } // End FOR

            bScalar = IsScalar (aplRankRht);

            break;

        case ARRAY_INT:
            // Test all the values
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Get the value
                aplInteger = ((LPAPLINT) lpMemRht)[uRht];

                // Test the value
                bRet = ValidateIntegerTest (&aplInteger,            // Ptr to the integer to test
                                             uValidLo,              // Low range value (inclusive)
                                             uValidHi,              // High ...
                                             bRangeLimit);          // TRUE iff we're range limiting
                if (!bRet)
                    goto DOMAIN_EXIT;
            } // End FOR

            bScalar = IsScalar (aplRankRht);

            break;

        case ARRAY_FLOAT:
            // Test all the values
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Attempt to convert the float to an integer using System []CT
                aplInteger = FloatToAplint_SCT (((LPAPLFLOAT) lpMemRht)[uRht],
                                               &bRet);
                // Test the value
                bRet = ValidateIntegerTest (&aplInteger,            // Ptr to the integer to test
                                             uValidLo,              // Low range value (inclusive)
                                             uValidHi,              // High ...
                                             bRangeLimit);          // TRUE iff we're range limiting
                if (!bRet)
                    goto DOMAIN_EXIT;
            } // End FOR

            bScalar = IsScalar (aplRankRht);

            break;

        case ARRAY_APA:
#define lpAPA           ((LPAPLAPA) lpMemRht)
            // Get the APA parameters
            apaOffRht = lpAPA->Off;
            apaMulRht = lpAPA->Mul;
#undef  lpAPA
            // Test all the values
            for (uRht = 0; uRht < aplNELMRht; uRht++)
            {
                // Get the value
                aplInteger = apaOffRht + apaMulRht * uRht;

                // Test the value
                bRet = ValidateIntegerTest (&aplInteger,            // Ptr to the integer to test
                                             uValidLo,              // Low range value (inclusive)
                                             uValidHi,              // High ...
                                             bRangeLimit);          // TRUE iff we're range limiting
                if (!bRet)
                    goto DOMAIN_EXIT;
            } // End FOR

            bScalar = IsScalar (aplRankRht);

            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            goto DOMAIN_EXIT;

        defstop
            break;
    } // End IF/ELSE/SWITCH

    // If the argument is a scalar, make a vector out of it
    if (bScalar)
        goto MAKE_VECTOR;

    // Split cases based upon the storage type of the right arg
    switch (aplTypeRht)
    {
        case ARRAY_INT:
            // Copy the right arg global as the result
            hGlbRes = CopySymGlbDirAsGlb (hGlbRht);

            break;

        case ARRAY_BOOL:
        case ARRAY_FLOAT:
        case ARRAY_APA:
            // Calculate space needed for a duplicate vector of integers
            ByteRes = CalcArraySize (ARRAY_INT, aplNELMRht, 1);

            // Allocate space for the result
            hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
            if (hGlbRes NE NULL)
            {
                // Lock the memory to get a ptr to it
                lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
                // Fill in the header values
                lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
                lpHeader->ArrType    = ARRAY_INT;
////////////////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////////////////lpHeader->SysVar     = FALSE;       // Already zero from GHND
                lpHeader->RefCnt     = 1;
                lpHeader->NELM       = aplNELMRht;
                lpHeader->Rank       = 1;
#undef  lpHeader

                // Save the dimension
                *VarArrayBaseToDim (lpMemRes) = aplNELMRht;

                // Skip over the header and dimensions to the data
                lpMemRes = VarArrayDataFmBase (lpMemRes);

                // Point back to the start of the right data
                lpMemRht = lpMemIniRht;

                // Loop through the right arg, converting to integers
                //   and copying them to the result
                for (uRht = 0; uRht < aplNELMRht; uRht++)
                // Split cases based upon the storage type of the right arg
                switch (aplTypeRht)
                {
                    case ARRAY_BOOL:
                        if (uBitMask & *(LPAPLBOOL) lpMemRht)
                            *((LPAPLINT) lpMemRes) = 1;

                        // Skip over the result value
                        ((LPAPLINT) lpMemRes)++;

                        // Shift over the bit mask
                        uBitMask <<= 1;

                        // Check for end-of-byte
                        if (uBitMask EQ END_OF_BYTE)
                        {
                            uBitMask = BIT0;            // Start over
                            ((LPAPLBOOL) lpMemRht)++;   // Skip to next byte
                        } // End IF

                        break;

                    case ARRAY_FLOAT:
                        // Attempt to convert the float to an integer using System []CT
                        aplInteger = FloatToAplint_SCT (*((LPAPLFLOAT) lpMemRht)++,
                                                       &bRet);
                        if (bRet)
                            *((LPAPLINT) lpMemRes)++ = aplInteger;
                        else
                            goto DOMAIN_EXIT;

                    case ARRAY_APA:
                        *((LPAPLINT) lpMemRes)++ = apaOffRht + apaMulRht * uRht;

                        break;

                    defstop
                        break;
                } // End FOR/SWITCH

                // We no longer need this ptr
                MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
            } else
                goto DOMAIN_EXIT;
            break;

        defstop
            break;
    } // End SWITCH

    goto NORMAL_EXIT;

MAKE_VECTOR:
    aplNELMRes = 1;

    // Calculate space needed for the result
    //   (an integer vector)
    ByteRes = CalcArraySize (ARRAY_INT, aplNELMRes, 1);

    // Check for overflow
    if (ByteRes NE (APLU3264) ByteRes)
        goto WSFULL_EXIT;

    // Allocate space for the result
    hGlbRes = DbgGlobalAlloc (GHND, (APLU3264) ByteRes);
    if (hGlbRes EQ NULL)
        goto WSFULL_EXIT;

    // Lock the memory to get a ptr to it
    lpMemRes = MyGlobalLock (hGlbRes);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRes)
    // Fill in the header values
    lpHeader->Sig.nature = VARARRAY_HEADER_SIGNATURE;
    lpHeader->ArrType    = ARRAY_INT;
////lpHeader->PermNdx    = PERMNDX_NONE;// Already zero from GHND
////lpHeader->SysVar     = FALSE;       // Already zero from GHND
    lpHeader->RefCnt     = 1;
    lpHeader->NELM       = aplNELMRes;
    lpHeader->Rank       = 1;
#undef  lpHeader

    // Save the dimension
    *VarArrayBaseToDim (lpMemRes) = aplNELMRes;

    // Skip over the header and dimensions to the data
    lpMemRes = VarArrayDataFmBase (lpMemRes);

    *((LPAPLINT) lpMemRes) = aplInteger;

    // We no longer need this ptr
    MyGlobalUnlock (hGlbRes); lpMemRes = NULL;
NORMAL_EXIT:
    // Free the old value
    FreeResultGlobalVar (lptkNamArg->tkData.tkSym->stData.stGlbData); lptkNamArg->tkData.tkSym->stData.stGlbData = NULL;

    // Save as new value
    lptkNamArg->tkData.tkSym->stData.stGlbData = MakePtrTypeGlb (hGlbRes);
    lptkNamArg->tkFlags.NoDisplay = TRUE;

    // Mark as successful
    bRet = TRUE;

    goto UNLOCK_EXIT;

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

WSFULL_EXIT:
    ErrorMessageIndirectToken (ERRMSG_WS_FULL APPEND_NAME,
                               lptkRhtArg);
    goto ERROR_EXIT;

ERROR_EXIT:
UNLOCK_EXIT:
    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return bRet;
} // End ValidateIntegerVector_EM
#undef  APPEND_NAME


//***************************************************************************
//  $VariantValidateCom_EM
//
//  Validate a value to be assigned to []CT/[]DT/[]IO/[]PP
//***************************************************************************

UBOOL VariantValidateCom_EM
    (IMM_TYPES       immType,           // Immediate type
     LPAPLLONGEST    lpaplLongest,      // Ptr to immediate value (ignored if bReset)
     UBOOL           bReset,            // TRUE iff assignment value is empty (we're resetting to CLEAR WS/System)
     ASYSVARVALIDSET aSysVarValidSet,   // Ptr to validate set function
     LPSYMENTRY      lpSymEntry,        // Ptr to sysvar SYMENTRY
     LPTOKEN         lptkFunc)          // Ptr to function token

{
    TOKEN tkNamArg = {0},       // Temporary named var token
          tkRhtArg = {0};       // ...       right arg ...

    // Set the named var token
    tkNamArg.tkFlags.TknType = TKT_VARNAMED;
    tkNamArg.tkFlags.ImmType   = IMMTYPE_ERROR;
////tkNamArg.tkFlags.NoDisplay = FALSE;         // Already set by = {0}
    tkNamArg.tkData.tkSym      = lpSymEntry;
////tkNamArg.tkCharIndex       = 0;             // Unused and already set by = {0}

    // If we're resetting, ...
    if (bReset)
    {
        // Setup the token
        tkRhtArg.tkFlags.TknType   = TKT_VARARRAY;
////////tkRhtArg.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from = {0}
////////tkRhtArg.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
        tkRhtArg.tkData.tkGlbData  = MakePtrTypeGlb (hGlbZilde);
        tkRhtArg.tkCharIndex       = lptkFunc->tkCharIndex;
    } else
    if (IsImmGlbNum (immType))
    {
        // Setup the token
        tkRhtArg.tkFlags.TknType   = TKT_VARARRAY;
////////tkRhtArg.tkFlags.ImmType   = IMMTYPE_ERROR; // Already zero from = {0}
////////tkRhtArg.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
        tkRhtArg.tkData.tkGlbData  = (HGLOBAL) lpaplLongest;
        tkRhtArg.tkCharIndex       = lptkFunc->tkCharIndex;
    } else
    {
        // Setup the token
        tkRhtArg.tkFlags.TknType   = TKT_VARIMMED;
        tkRhtArg.tkFlags.ImmType   = immType;
////////tkRhtArg.tkFlags.NoDisplay = FALSE;         // Already zero from = {0}
        tkRhtArg.tkData.tkLongest  = *lpaplLongest;
        tkRhtArg.tkCharIndex       = lptkFunc->tkCharIndex;
    } // End IF/ELSE

    // Validate the arg
    return (*aSysVarValidSet) (&tkNamArg,   // Ptr to name arg token
                               &tkRhtArg);  // Ptr to right arg token
} // End VariantValidateCom_EM


//***************************************************************************
//  $ValidSetALX_EM
//
//  Validate a value before assigning it to []ALX
//***************************************************************************

UBOOL ValidSetALX_EM
    (LPTOKEN lptkNamArg,            // Ptr to name arg token
     LPTOKEN lptkRhtArg)            // Ptr to right arg token

{
    // Ensure the argument is either a character scalar (promoted to a vector)
    //   or a character vector.
    return ValidateCharVector_EM (lptkNamArg, lptkRhtArg, FALSE);
} // End ValidSetALX_EM


//***************************************************************************
//  $ValidNdxChar
//
//  Validate a single value before assigning it to
//    a position in a char-only SysVar.
//***************************************************************************

UBOOL ValidNdxChar
    (APLINT       aplIntegerLst,            // The origin-0 index value (in case the position is important)
     APLSTYPE     aplTypeRht,               // Right arg storage type
     LPAPLLONGEST lpaplLongestRht,          // Ptr to the right arg value
     LPIMM_TYPES  lpimmTypeRht,             // Ptr to right arg immediate type (may be NULL)
     HGLOBAL      lpSymGlbRht,              // Ptr to right arg global value
     LPTOKEN      lptkFunc)                 // Ptr to function token

{
    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
        case ARRAY_APA:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
        case ARRAY_RAT:
        case ARRAY_VFP:
            return FALSE;

        case ARRAY_CHAR:
            return TRUE;

        defstop
            return FALSE;
    } // End SWITCH
} // End ValidNdxChar


//***************************************************************************
//  $ValidSetCT_EM
//
//  Validate a value before assigning it to []CT
//***************************************************************************

UBOOL ValidSetCT_EM
    (LPTOKEN lptkNamArg,            // Ptr to name arg token
     LPTOKEN lptkRhtArg)            // Ptr to right arg token

{
    // Ensure the argument is either a real scalar or
    //   one-element vector (demoted to a scalar)
    //   between DEF_MIN_QUADCT and DEF_MAX_QUADCT inclusive.
    return ValidateFloat_EM (lptkNamArg,            // Ptr to name arg token
                             lptkRhtArg,            // Ptr to right arg token
                             DEF_MIN_QUADCT,        // Minimum value
             bResetVars.CT ? DEF_QUADCT_CWS
                           : fQuadCT_CWS,           // Default ...
                             DEF_MAX_QUADCT,        // Maximum ...
                             bRangeLimit.CT);       // TRUE iff range limiting
} // End ValidSetCT_EM


//***************************************************************************
//  $ValidNdxCT
//
//  Validate a single value before assigning it to a position in []CT.
//
//  We allow any number between DEF_MIN_QUADCT and DEF_MAX_QUADCT inclusive.
//***************************************************************************

UBOOL ValidNdxCT
    (APLINT       aplIntegerLst,            // The origin-0 index value (in case the position is important)
     APLSTYPE     aplTypeRht,               // Right arg storage type
     LPAPLLONGEST lpaplLongestRht,          // Ptr to the right arg value
     LPIMM_TYPES  lpimmTypeRht,             // Ptr to right arg immediate type (may be NULL)
     HGLOBAL      lpSymGlbRht,              // Ptr to right arg global value
     LPTOKEN      lptkFunc)                 // Ptr to function token

{
    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            // The right arg is integer -- convert to float
            *((LPAPLFLOAT) lpaplLongestRht) = (APLFLOAT) (APLINT) *lpaplLongestRht;

            break;

        case ARRAY_FLOAT:
            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            return FALSE;

        case ARRAY_RAT:
            // Attempt to convert the RAT to a float
            *((LPAPLFLOAT) lpaplLongestRht) = mpq_get_d ((LPAPLRAT) lpSymGlbRht);

            break;

        case ARRAY_VFP:
            // Attempt to convert the VFP to a float
            *((LPAPLFLOAT) lpaplLongestRht) = mpfr_get_d ((LPAPLVFP) lpSymGlbRht, MPFR_RNDN);

            break;

        defstop
            break;
    } // End SWITCH

    // Set the new immediate type
    if (lpimmTypeRht)
        *lpimmTypeRht = IMMTYPE_FLOAT;

    // Test the value
    return ValidateFloatTest ((LPAPLFLOAT) lpaplLongestRht,
                              DEF_MIN_QUADCT,
                              DEF_MAX_QUADCT,
                              bRangeLimit.CT);
} // End ValidNdxCT


//***************************************************************************
//  $ValidSetDM_EM
//
//  Validate a value before assigning it to []DM.
//
//  We don't allow assignment into []DM.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ValidSetDM_EM"
#else
#define APPEND_NAME
#endif

UBOOL ValidSetDM_EM
    (LPTOKEN lptkNamArg,            // Ptr to name arg token
     LPTOKEN lptkRhtArg)            // Ptr to right arg token

{
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkRhtArg);
    return FALSE;
} // End ValidSetDM_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ValidNdxDM
//
//  Validate a single value before assigning it to a position in []DM.
//
//  We don't allow indexed assignment into []DM.
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ValidNdxDM"
#else
#define APPEND_NAME
#endif

UBOOL ValidNdxDM
    (APLINT       aplIntegerLst,            // The origin-0 index value (in case the position is important)
     APLSTYPE     aplTypeRht,               // Right arg storage type
     LPAPLLONGEST lpaplLongestRht,          // Ptr to the right arg value
     LPIMM_TYPES  lpimmTypeRht,             // Ptr to right arg immediate type (may be NULL)
     HGLOBAL      lpSymGlbRht,              // Ptr to right arg global value
     LPTOKEN      lptkFunc)                 // Ptr to function token

{
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkFunc);
    return FALSE;
} // End ValidNdxDM
#undef  APPEND_NAME


//***************************************************************************
//  $ValidNdxDT
//
//  Validate a single value before assigning it to a position in []DT.
//
//  We allow the characters in DEF_QUADDT_ALLOW..
//***************************************************************************

UBOOL ValidNdxDT
    (APLINT       aplIntegerLst,            // The origin-0 index value (in case the position is important)
     APLSTYPE     aplTypeRht,               // Right arg storage type
     LPAPLLONGEST lpaplLongestRht,          // Ptr to the right arg value
     LPIMM_TYPES  lpimmTypeRht,             // Ptr to right arg immediate type (may be NULL)
     HGLOBAL      lpSymGlbRht,              // Ptr to right arg global value
     LPTOKEN      lptkFunc)                 // Ptr to function token

{
    APLCHAR aplChar;                // The value to validate

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
        case ARRAY_APA:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            return FALSE;

        case ARRAY_CHAR:
            aplChar = (APLCHAR) *lpaplLongestRht;

            return (strchrW (DEF_QUADDT_ALLOW, aplChar) NE NULL);

        defstop
            return FALSE;
    } // End SWITCH
} // End ValidNdxDT


//***************************************************************************
//  $ValidSetDT_EM
//
//  Validate a value before assigning it to []DT
//***************************************************************************

UBOOL ValidSetDT_EM
    (LPTOKEN lptkNamArg,            // Ptr to name arg token
     LPTOKEN lptkRhtArg)            // Ptr to right arg token

{
    // Ensure the argument is either a real scalar or
    //   one-element vector (demoted to a scalar)
    return ValidateCharDT_EM (lptkNamArg,             // Ptr to name arg token
                              lptkRhtArg,             // Ptr to right arg token
              bResetVars.DT ? DEF_QUADDT_CWS[0]
                            : cQuadDT_CWS);           // Default ...
} // End ValidSetDT_EM


//***************************************************************************
//  $ValidSetELX_EM
//
//  Validate a value before assigning it to []ELX
//***************************************************************************

UBOOL ValidSetELX_EM
    (LPTOKEN lptkNamArg,            // Ptr to name arg token
     LPTOKEN lptkRhtArg)            // Ptr to right arg token

{
    // Ensure the argument is either a character scalar (promoted to a vector)
    //   or a character vector.
    return ValidateCharVector_EM (lptkNamArg, lptkRhtArg, FALSE);
} // End ValidSetELX_EM


//***************************************************************************
//  $ValidSetFC_EM
//
//  Validate a value before assigning it to []FC
//***************************************************************************

UBOOL ValidSetFC_EM
    (LPTOKEN lptkNamArg,            // Ptr to name arg token
     LPTOKEN lptkRhtArg)            // Ptr to right arg token

{
    APLSTYPE aplTypeRht;            // Right arg storage type
    APLNELM  aplNELMRht;            // ...       NELM
    APLRANK  aplRankRht;            // ...       rank

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // If the right arg is an empty simple non-heterogeneous vector,
    //   convert the result to the default values
    if (IsSimpleNH (aplTypeRht)
     && IsEmpty (aplNELMRht)
     && IsVector (aplRankRht))
    {
        // Free the named arg
        FreeResultGlobalVar (lptkNamArg->tkData.tkSym->stData.stGlbData); lptkNamArg->tkData.tkSym->stData.stGlbData = NULL;

        // Save as new value
        lptkNamArg->tkData.tkSym->stData.stGlbData =
          CopySymGlbDirAsGlb (MakePtrTypeGlb (bResetVars.FC ? hGlbQuadFC_SYS
                                                            : hGlbQuadFC_CWS));
        // Mark as no need to display
        lptkNamArg->tkFlags.NoDisplay = TRUE;

        return TRUE;
    } // End IF

    // Ensure the argument is either a character scalar (promoted to a vector)
    //   or a character vector.
    return ValidateCharVector_EM (lptkNamArg, lptkRhtArg, FALSE);
} // End ValidSetFC_EM


//***************************************************************************
//  $ValidSetFEATURE_EM
//
//  Validate a value before assigning it to []FEATURE
//***************************************************************************

UBOOL ValidSetFEATURE_EM
    (LPTOKEN lptkNamArg,            // Ptr to name arg token
     LPTOKEN lptkRhtArg)            // Ptr to right arg token

{
    APLSTYPE aplTypeRht;            // Right arg storage type
    APLNELM  aplNELMRht;            // ...       NELM
    APLRANK  aplRankRht;            // ...       rank
    UBOOL    bRet;                  // TRUE iff the result is valid

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // If the right arg is an empty simple non-heterogeneous vector,
    //   convert the result to the default values
    if (IsSimpleNH (aplTypeRht)
     && IsEmpty (aplNELMRht)
     && IsVector (aplRankRht))
    {
        // Free the named arg
        FreeResultGlobalVar (lptkNamArg->tkData.tkSym->stData.stGlbData); lptkNamArg->tkData.tkSym->stData.stGlbData = NULL;

        // Save as new value
        lptkNamArg->tkData.tkSym->stData.stGlbData =
          CopySymGlbDirAsGlb (MakePtrTypeGlb (bResetVars.FEATURE ? hGlbQuadFEATURE_SYS
                                                                 : hGlbQuadFEATURE_CWS));
        // Mark as no need to display
        lptkNamArg->tkFlags.NoDisplay = TRUE;

        return TRUE;
    } // End IF

    // Ensure the argument is either an integer scalar (promoted to a vector)
    //   or an integer vector
    bRet =
      ValidateIntegerVector_EM (lptkNamArg,             // Ptr to name token
                                lptkRhtArg,             // Ptr to right arg token
                                DEF_MIN_QUADFEATURE,    // Low range value (inclusive)
                                DEF_MAX_QUADFEATURE,    // High ...
                                bRangeLimit.FEATURE);   // TRUE iff an incoming value outside
                                                        //   the given range [uValidLo, uValidHi]
                                                        //   is adjusted to be the closer range limit
    // If the result is valid, ...
    if (bRet)
    {
        LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory
        HGLOBAL      hGlbRht;           // Right arg global memory handle
        LPAPLINT     lpMemRht;          // Ptr to right arg global memory
        APLLONGEST   aplLongestRht;     // Right arg immediate value

        // Get right arg's global ptrs
        aplLongestRht = GetGlbPtrs_LOCK (lptkRhtArg, &hGlbRht, &lpMemRht);

        // Get ptr to PerTabData global memory
        lpMemPTD = GetMemPTD ();

        // If the right arg is a global, ...
        if (hGlbRht)
        {
            // Skip over the header and dimensions to the data
            lpMemRht = VarArrayDataFmBase (lpMemRht);
        } else
            // Point to the data
            lpMemRht = &aplLongestRht;

        // Save the current values for later use
        CopyMemory (lpMemPTD->aplCurrentFEATURE, lpMemRht, (APLU3264) min (FEATURENDX_LENGTH, aplNELMRht) * sizeof (APLINT));

        // If the right arg is a global, ...
        if (hGlbRht)
        {
            MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
        } // End IF
    } // End IF

    return bRet;
} // End ValidSetFEATURE_EM


//***************************************************************************
//  $ValidNdxFEATURE
//
//  Validate a single value before assigning it to a position in []FEATURE.
//
//  We allow any number between DEF_MIN_QUADFEATURE and DEF_MAX_QUADFEATURE inclusive.
//***************************************************************************

UBOOL ValidNdxFEATURE
    (APLINT       aplIntegerLst,            // The origin-0 index value (in case the position is important)
     APLSTYPE     aplTypeRht,               // Right arg storage type
     LPAPLLONGEST lpaplLongestRht,          // Ptr to the right arg value
     LPIMM_TYPES  lpimmTypeRht,             // Ptr to right arg immediate type (may be NULL)
     HGLOBAL      lpSymGlbRht,              // Ptr to right arg global value
     LPTOKEN      lptkFunc)                 // Ptr to function token

{
    UBOOL bRet = TRUE;                      // TRUE iff the result is valid

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            break;

        case ARRAY_FLOAT:
            // The right arg is float -- convert to integer
            *lpaplLongestRht = FloatToAplint_SCT (*(LPAPLFLOAT) lpaplLongestRht, &bRet);

            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            return FALSE;

        case ARRAY_RAT:
            // Attempt to convert the RAT to an INT
            *lpaplLongestRht = mpq_get_sx ((LPAPLRAT) lpSymGlbRht, &bRet);

            break;

        case ARRAY_VFP:
            // Attempt to convert the VFP to an INT
            *lpaplLongestRht = mpfr_get_sx ((LPAPLVFP) lpSymGlbRht, &bRet);

            break;

        defstop
            break;
    } // End SWITCH

    if (!bRet)
        return bRet;

    // Set the new immediate type
    if (lpimmTypeRht)
        *lpimmTypeRht = IMMTYPE_INT;

    // Test the value
    bRet =
      ValidateIntegerTest ((LPAPLINT) lpaplLongestRht,      // Ptr to the integer to test
                           DEF_MIN_QUADFEATURE,             // Low range value (inclusive)
                           DEF_MAX_QUADFEATURE,             // High ...
                           bRangeLimit.FEATURE);            // TRUE iff we're range limiting
    // If the result is valid, ...
    if (bRet)
    {
        LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

        // Get ptr to PerTabData global memory
        lpMemPTD = GetMemPTD ();

        // Save the current value for later use
        lpMemPTD->aplCurrentFEATURE[aplIntegerLst] = *lpaplLongestRht;
    } // End IF

    return bRet;
} // End ValidNdxFEATURE


//***************************************************************************
//  $SetCurrentFeatureCWS
//
//  Copy the CLEARWS values of []FEATURES to the current copy in PTD
//***************************************************************************

void SetCurrentFeatureCWS
    (LPPERTABDATA lpMemPTD)

{
    LPAPLINT lpMemCWS;

    // Lock the memory to get a ptr to it
    lpMemCWS = MyGlobalLock (hGlbQuadFEATURE_CWS);

    // Skip over the header and dimensions to the data
    lpMemCWS = VarArrayDataFmBase (lpMemCWS);

    // Save the values in the PTD
    CopyMemory (lpMemPTD->aplCurrentFEATURE,
                lpMemCWS,
                FEATURENDX_LENGTH * sizeof (APLINT));
    // We no longer need this ptr
    MyGlobalUnlock (hGlbQuadFEATURE_CWS); lpMemCWS = NULL;
} // End SetCurrentFeatureCWS


//***************************************************************************
//  $ValidSetFPC_EM
//
//  Validate a value before assigning it to []FPC
//
//  We allow any number between DEF_MIN_QUADFPC and DEF_MAX_QUADFPC inclusive.
//***************************************************************************

UBOOL ValidSetFPC_EM
    (LPTOKEN lptkNamArg,                // Ptr to name arg token
     LPTOKEN lptkRhtArg)                // Ptr to right arg token

{
    UBOOL bRet;                             // TRUE iff the result is valid

    // Ensure the argument is either a Boolean scalar or
    //   one-element vector (demoted to a scalar).
    bRet = ValidateInteger_EM (lptkNamArg,          // Ptr to name arg token
                               lptkRhtArg,          // Ptr to right arg token
                               DEF_MIN_QUADFPC,     // Minimum value
              bResetVars.FPC ? DEF_QUADFPC_CWS
                             : uQuadFPC_CWS,        // Default ...
                               DEF_MAX_QUADFPC,     // Maximum ...
                               bRangeLimit.FPC);    // TRUE iff range limiting
    if (bRet)
    {
        // Initialize VFP constants as the default precision has changed
        InitVfpPrecision (lptkNamArg->tkData.tkSym->stData.stInteger);

        // Initialize PerTabData vars
        InitPTDVars (GetMemPTD ());
    } // End IF

    return bRet;
} // End ValidSetFPC_EM


//***************************************************************************
//  $ValidNdxFPC
//
//  Validate a single value before assigning it to a position in []FPC.
//
//  We allow any number between DEF_MIN_QUADFPC and DEF_MAX_QUADFPC inclusive.
//***************************************************************************

UBOOL ValidNdxFPC
    (APLINT       aplIntegerLst,            // The origin-0 index value (in case the position is important)
     APLSTYPE     aplTypeRht,               // Right arg storage type
     LPAPLLONGEST lpaplLongestRht,          // Ptr to the right arg value
     LPIMM_TYPES  lpimmTypeRht,             // Ptr to right arg immediate type (may be NULL)
     HGLOBAL      lpSymGlbRht,              // Ptr to right arg global value
     LPTOKEN      lptkFunc)                 // Ptr to function token

{
    UBOOL bRet = TRUE;                      // TRUE iff the result is valid

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_FLOAT:
            // The right arg is float -- convert to integer
            *lpaplLongestRht = FloatToAplint_SCT (*(LPAPLFLOAT) lpaplLongestRht, &bRet);

            break;

        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            return FALSE;

        case ARRAY_RAT:
            // Attempt to convert the RAT to an INT
            *lpaplLongestRht = mpq_get_sx ((LPAPLRAT) lpSymGlbRht, &bRet);

            break;

        case ARRAY_VFP:
            // Attempt to convert the VFP to an INT
            *lpaplLongestRht = mpfr_get_sx ((LPAPLVFP) lpSymGlbRht, &bRet);

            break;

        defstop
            break;
    } // End SWITCH

    if (!bRet)
        return bRet;

    // Set the new immediate type (in case it was integer/APA)
    if (lpimmTypeRht)
        *lpimmTypeRht = IMMTYPE_BOOL;

    // Test the value
    bRet =
      ValidateIntegerTest ((LPAPLINT) lpaplLongestRht,      // Ptr to the integer to test
                           DEF_MIN_QUADFPC,                 // Low range value (inclusive)
                           DEF_MAX_QUADFPC,                 // High ...
                           bRangeLimit.FPC);                // TRUE iff we're range limiting
    if (bRet)
    {
        // Initialize VFP constants as the default precision has changed
        InitVfpPrecision (*(LPAPLINT) lpaplLongestRht);

        // Initialize PerTabData vars
        InitPTDVars (GetMemPTD ());
    } // End IF

    return bRet;
} // End ValidNdxFPC


//***************************************************************************
//  $ValidSetIC_EM
//
//  Validate a value before assigning it to []IC.
//
//  We allow any numeric singleton or vector whose values are
//    DEF_MIN_QUADIC through DEF_MAX_QUADIC.
//***************************************************************************

UBOOL ValidSetIC_EM
    (LPTOKEN lptkNamArg,            // Ptr to name arg token
     LPTOKEN lptkRhtArg)            // Ptr to right arg token

{
    APLSTYPE aplTypeRht;            // Right arg storage type
    APLNELM  aplNELMRht;            // ...       NELM
    APLRANK  aplRankRht;            // ...       rank

    // Get the attributes (Type, NELM, and Rank)
    //   of the right arg
    AttrsOfToken (lptkRhtArg, &aplTypeRht, &aplNELMRht, &aplRankRht, NULL);

    // If the right arg is an empty simple non-heterogeneous vector,
    //   convert the result to the default values
    if (IsSimpleNH (aplTypeRht)
     && IsEmpty (aplNELMRht)
     && IsVector (aplRankRht))
    {
        // Free the named arg
        FreeResultGlobalVar (lptkNamArg->tkData.tkSym->stData.stGlbData); lptkNamArg->tkData.tkSym->stData.stGlbData = NULL;

        // Save as new value
        lptkNamArg->tkData.tkSym->stData.stGlbData =
          CopySymGlbDirAsGlb (MakePtrTypeGlb (bResetVars.IC ? hGlbQuadIC_SYS
                                                            : hGlbQuadIC_CWS));
        // Mark as no need to display
        lptkNamArg->tkFlags.NoDisplay = TRUE;

        return TRUE;
    } // End IF

    // Ensure the argument is either an integer scalar (promoted to a vector)
    //   or an integer vector
    return
      ValidateIntegerVector_EM (lptkNamArg,         // Ptr to name token
                                lptkRhtArg,         // Ptr to right arg token
                                DEF_MIN_QUADIC,     // Low range value (inclusive)
                                DEF_MAX_QUADIC,     // High ...
                                bRangeLimit.IC);    // TRUE iff an incoming value outside
                                                    //   the given range [uValidLo, uValidHi]
                                                    //   is adjusted to be the closer range limit
} // End ValidSetIC_EM


//***************************************************************************
//  $ValidNdxIC
//
//  Validate a single value before assigning it to a position in []IC.
//
//  We allow any number between DEF_MIN_QUADIC and DEF_MAX_QUADIC inclusive.
//***************************************************************************

UBOOL ValidNdxIC
    (APLINT       aplIntegerLst,            // The origin-0 index value (in case the position is important)
     APLSTYPE     aplTypeRht,               // Right arg storage type
     LPAPLLONGEST lpaplLongestRht,          // Ptr to the right arg value
     LPIMM_TYPES  lpimmTypeRht,             // Ptr to right arg immediate type (may be NULL)
     HGLOBAL      lpSymGlbRht,              // Ptr to right arg global value
     LPTOKEN      lptkFunc)                 // Ptr to function token

{
    UBOOL bRet = TRUE;                      // TRUE iff the result is valid

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            break;

        case ARRAY_FLOAT:
            // The right arg is float -- convert to integer
            *lpaplLongestRht = FloatToAplint_SCT (*(LPAPLFLOAT) lpaplLongestRht, &bRet);

            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            return FALSE;

        case ARRAY_RAT:
            // Attempt to convert the RAT to an integer
            *lpaplLongestRht = mpq_get_sx ((LPAPLRAT) lpSymGlbRht, &bRet);

            break;

        case ARRAY_VFP:
            // Attempt to convert the RAT to an integer
            *lpaplLongestRht = mpfr_get_sx ((LPAPLVFP) lpSymGlbRht, &bRet);

            break;

        defstop
            break;
    } // End SWITCH

    if (!bRet)
        return bRet;

    // Set the new immediate type
    if (lpimmTypeRht)
        *lpimmTypeRht = IMMTYPE_INT;

    // Test the value
    return
      ValidateIntegerTest ((LPAPLINT) lpaplLongestRht,      // Ptr to the integer to test
                           DEF_MIN_QUADIC,                  // Low range value (inclusive)
                           DEF_MAX_QUADIC,                  // High ...
                           bRangeLimit.IC);                 // TRUE iff we're range limiting
} // End ValidNdxIC


//***************************************************************************
//  $ValidSetIO_EM
//
//  Validate a value before assigning it to []IO
//
//  We allow any numeric singleton whose value is 0 or 1.
//***************************************************************************

UBOOL ValidSetIO_EM
    (LPTOKEN lptkNamArg,                // Ptr to name arg token
     LPTOKEN lptkRhtArg)                // Ptr to right arg token

{
    // Ensure the argument is either a Boolean scalar or
    //   one-element vector (demoted to a scalar).
    return ValidateInteger_EM (lptkNamArg,          // Ptr to name arg token
                               lptkRhtArg,          // Ptr to right arg token
                               DEF_MIN_QUADIO,      // Minimum value
               bResetVars.IO ? DEF_QUADIO_CWS
                             : bQuadIO_CWS,         // Default ...
                               DEF_MAX_QUADIO,      // Maximum ...
                               bRangeLimit.IO);     // TRUE iff range limiting
} // End ValidSetIO_EM


//***************************************************************************
//  $ValidNdxIO
//
//  Validate a single value before assigning it to a position in []IO.
//
//  We allow any number between DEF_MIN_QUADIO and DEF_MAX_QUADIO inclusive.
//***************************************************************************

UBOOL ValidNdxIO
    (APLINT       aplIntegerLst,            // The origin-0 index value (in case the position is important)
     APLSTYPE     aplTypeRht,               // Right arg storage type
     LPAPLLONGEST lpaplLongestRht,          // Ptr to the right arg value
     LPIMM_TYPES  lpimmTypeRht,             // Ptr to right arg immediate type (may be NULL)
     HGLOBAL      lpSymGlbRht,              // Ptr to right arg global value
     LPTOKEN      lptkFunc)                 // Ptr to function token

{
    UBOOL bRet = TRUE;                      // TRUE iff the result is valid

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_FLOAT:
            // The right arg is float -- convert to integer
            *lpaplLongestRht = FloatToAplint_SCT (*(LPAPLFLOAT) lpaplLongestRht, &bRet);

            break;

        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            return FALSE;

        case ARRAY_RAT:
            // Attempt to convert the RAT to an INT
            *lpaplLongestRht = mpq_get_sx ((LPAPLRAT) lpSymGlbRht, &bRet);

            break;

        case ARRAY_VFP:
            // Attempt to convert the VFP to an INT
            *lpaplLongestRht = mpfr_get_sx ((LPAPLVFP) lpSymGlbRht, &bRet);

            break;

        defstop
            break;
    } // End SWITCH

    if (!bRet)
        return bRet;

    // Set the new immediate type (in case it was integer/APA)
    if (lpimmTypeRht)
        *lpimmTypeRht = IMMTYPE_BOOL;

    // Test the value
    return
      ValidateIntegerTest ((LPAPLINT) lpaplLongestRht,      // Ptr to the integer to test
                           DEF_MIN_QUADIO,                  // Low range value (inclusive)
                           DEF_MAX_QUADIO,                  // High ...
                           bRangeLimit.IO);                 // TRUE iff we're range limiting
} // End ValidNdxIO


//***************************************************************************
//  $ValidSetLX_EM
//
//  Validate a value before assigning it to []LX
//***************************************************************************

UBOOL ValidSetLX_EM
    (LPTOKEN lptkNamArg,                // Ptr to name arg token
     LPTOKEN lptkRhtArg)                // Ptr to right arg token

{
    // Ensure the argument is either a character scalar (promoted to a vector)
    //   or a character vector.
    return ValidateCharVector_EM (lptkNamArg, lptkRhtArg, FALSE);
} // End ValidSetLX_EM


//***************************************************************************
//  $ValidSetPP_EM
//
//  Validate a value before assigning it to []PP
//***************************************************************************

UBOOL ValidSetPP_EM
    (LPTOKEN lptkNamArg,            // Ptr to name arg token
     LPTOKEN lptkRhtArg)            // Ptr to right arg token

{
    // Ensure the argument is either an integer scalar or
    //   one-element vector (demoted to a scalar)
    //   in the range for []PP.
    return ValidateInteger_EM (lptkNamArg,          // Ptr to name arg token
                               lptkRhtArg,          // Ptr to right arg token
                               DEF_MIN_QUADPP,      // Minimum value
               bResetVars.PP ? DEF_QUADPP_CWS
                             : (UINT) uQuadPP_CWS,  // Default ...
                               DEF_MAX_QUADPPVFP,   // Maximum ...
                               bRangeLimit.PP);     // TRUE iff range limiting
} // End ValidSetPP_EM


//***************************************************************************
//  $ValidNdxPP
//
//  Validate a single value before assigning it to a position in []PP.
//
//  We allow any number between DEF_MIN_QUADPP and DEF_MAX_QUADPPVFP inclusive.
//***************************************************************************

UBOOL ValidNdxPP
    (APLINT       aplIntegerLst,            // The origin-0 index value (in case the position is important)
     APLSTYPE     aplTypeRht,               // Right arg storage type
     LPAPLLONGEST lpaplLongestRht,          // Ptr to the right arg value
     LPIMM_TYPES  lpimmTypeRht,             // Ptr to right arg immediate type (may be NULL)
     HGLOBAL      lpSymGlbRht,              // Ptr to right arg global value
     LPTOKEN      lptkFunc)                 // Ptr to function token

{
    UBOOL bRet = TRUE;                      // TRUE iff the result is valid

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            break;

        case ARRAY_FLOAT:
            // The right arg is float -- convert to integer
            *lpaplLongestRht = FloatToAplint_SCT (*(LPAPLFLOAT) lpaplLongestRht, &bRet);

            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            return FALSE;

        case ARRAY_RAT:
            // Attempt to convert the RAT to an INT
            *lpaplLongestRht = mpq_get_sx ((LPAPLRAT) lpSymGlbRht, &bRet);

            break;

        case ARRAY_VFP:
            // Attempt to convert the VFP to an INT
            *lpaplLongestRht = mpfr_get_sx ((LPAPLVFP) lpSymGlbRht, &bRet);

            break;

        defstop
            break;
    } // End SWITCH

    if (!bRet)
        return bRet;

    // Set the new immediate type
    if (lpimmTypeRht)
        *lpimmTypeRht = IMMTYPE_INT;

    // Test the value
    return
      ValidateIntegerTest ((LPAPLINT) lpaplLongestRht,      // Ptr to the integer to test
                           DEF_MIN_QUADPP,                  // Low range value (inclusive)
                           DEF_MAX_QUADPPVFP,               // High ...
                           bRangeLimit.PP);                 // TRUE iff we're range limiting
} // End ValidNdxPP


//***************************************************************************
//  $ValidSetPR_EM
//
//  Validate a value before assigning it to []PR
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ValidSetPR_EM"
#else
#define APPEND_NAME
#endif

UBOOL ValidSetPR_EM
    (LPTOKEN lptkNamArg,            // Ptr to name arg token
     LPTOKEN lptkRhtArg)            // Ptr to right arg token

{
    HGLOBAL      hGlbRht = NULL;    // Right arg global memory handle
    LPVOID       lpMemRht = NULL;   // Ptr to right arg global memory
    UBOOL        bRet = FALSE;      // TRUE iff result is valid
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Ensure the argument is either a character scalar, or
    //   one-element vector (demoted to a scalar), or
    //   an empty vector.

    // Split cases based upon the token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
            {
                // Get the HGLOBAL
                hGlbRht = lptkRhtArg->tkData.tkSym->stData.stGlbData;

                break;      // Continue with HGLOBAL processing
            } // End IF

            // Handle the immediate case

            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // Split cases based upon the token immediate type
            switch (lptkRhtArg->tkData.tkSym->stFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                case IMMTYPE_INT:
                case IMMTYPE_FLOAT:
                    goto DOMAIN_EXIT;

                case IMMTYPE_CHAR:
                    lpMemPTD->cQuadPR = lptkRhtArg->tkData.tkSym->stData.stChar;

                    goto MAKE_SCALAR;
            } // End SWITCH

            break;

        case TKT_VARIMMED:
            // Split cases based upon the token immediate type
            switch (lptkRhtArg->tkFlags.ImmType)
            {
                case IMMTYPE_BOOL:
                case IMMTYPE_INT:
                case IMMTYPE_FLOAT:
                    goto DOMAIN_EXIT;

                case IMMTYPE_CHAR:
                    lpMemPTD->cQuadPR = lptkRhtArg->tkData.tkChar;

                    goto MAKE_SCALAR;
            } // End SWITCH

            break;

        case TKT_LISTPAR:   // The tkData is an HGLOBAL of an array of HGLOBALs
            goto SYNTAX_EXIT;

        case TKT_CHRSTRAND: // tkData is an HGLOBAL of an array of ???
        case TKT_NUMSTRAND: // tkData is an HGLOBAL of an array of ???
        case TKT_NUMSCALAR: // tkData is an HGLOBAL of an array of ???
        case TKT_VARARRAY:  // tkData is an HGLOBAL of an array of ???
            // Get the HGLOBAL
            hGlbRht = lptkRhtArg->tkData.tkGlbData;

            break;          // Continue with HGLOBAL processing

        defstop
            goto DOMAIN_EXIT;
    } // End SWITCH

    // tkData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir_PTB (hGlbRht));

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (hGlbRht);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRht)
    // Check for scalar or vector
    if (IsMultiRank (lpHeader->Rank))
        goto RANK_EXIT;
    else
    // Check for empty or singleton
    if (IsMultiNELM (lpHeader->NELM))
        goto LENGTH_EXIT;
    else
    // Split cases based upon the array type
    switch (lpHeader->ArrType)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_FLOAT:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            goto DOMAIN_EXIT;

        case ARRAY_CHAR:
            // Izit an empty vector?
            if (IsVector (lpHeader->Rank) && IsEmpty (lpHeader->NELM))
                lpMemPTD->cQuadPR = CQUADPR_MT;
            else
                lpMemPTD->cQuadPR = *(LPAPLCHAR) VarArrayDataFmBase (lpHeader);
            break;

        defstop
            break;
    } // End IF/ELSE/SWITCH

#undef  lpHeader

MAKE_SCALAR:
    lptkNamArg->tkData.tkSym->stFlags.Imm = (lpMemPTD->cQuadPR NE CQUADPR_MT);
    if (lpMemPTD->cQuadPR EQ CQUADPR_MT)
        lptkNamArg->tkData.tkSym->stData.stGlbData = MakePtrTypeGlb (hGlbV0Char);
    else
        lptkNamArg->tkData.tkSym->stData.stChar = lpMemPTD->cQuadPR;
    lptkNamArg->tkFlags.NoDisplay = TRUE;

    // Mark as successful
    bRet = TRUE;

    goto UNLOCK_EXIT;

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkNamArg);
    goto ERROR_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                               lptkNamArg);
    goto ERROR_EXIT;

LENGTH_EXIT:
    ErrorMessageIndirectToken (ERRMSG_LENGTH_ERROR APPEND_NAME,
                               lptkNamArg);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkNamArg);
    goto ERROR_EXIT;

ERROR_EXIT:
UNLOCK_EXIT:
    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return bRet;
} // End ValidSetPR_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ValidSetPW_EM
//
//  Validate a value before assigning it to []PW
//***************************************************************************

UBOOL ValidSetPW_EM
    (LPTOKEN lptkNamArg,            // Ptr to name arg token
     LPTOKEN lptkRhtArg)            // Ptr to right arg token

{
    // Ensure the argument is either an integer scalar or
    //   one-element vector (demoted to a scalar)
    //   in the range for []PW.
    return ValidateInteger_EM (lptkNamArg,          // Ptr to name arg token
                               lptkRhtArg,          // Ptr to right arg token
                               DEF_MIN_QUADPW,      // Minimum value
               bResetVars.PW ? DEF_QUADPW_CWS
                             : (UINT) uQuadPW_CWS,  // Default ...
                               DEF_MAX_QUADPW,      // Maximum ...
                               bRangeLimit.PW);     // TRUE iff range limiting
} // End ValidSetPW_EM


//***************************************************************************
//  $ValidNdxPW
//
//  Validate a single value before assigning it to a position in []PW.
//
//  We allow any number between DEF_MIN_QUADPW and DEF_MAX_QUADPW inclusive.
//***************************************************************************

UBOOL ValidNdxPW
    (APLINT       aplIntegerLst,            // The origin-0 index value (in case the position is important)
     APLSTYPE     aplTypeRht,               // Right arg storage type
     LPAPLLONGEST lpaplLongestRht,          // Ptr to the right arg value
     LPIMM_TYPES  lpimmTypeRht,             // Ptr to right arg immediate type (may be NULL)
     HGLOBAL      lpSymGlbRht,              // Ptr to right arg global value
     LPTOKEN      lptkFunc)                 // Ptr to function token

{
    UBOOL bRet = TRUE;                      // TRUE iff the result is valid

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            break;

        case ARRAY_FLOAT:
            // The right arg is float -- convert to integer
            *lpaplLongestRht = FloatToAplint_SCT (*(LPAPLFLOAT) lpaplLongestRht, &bRet);

            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            return FALSE;

        case ARRAY_RAT:
            // Attempt to convert the RAT to an INT
            *lpaplLongestRht = mpq_get_sx ((LPAPLRAT) lpSymGlbRht, &bRet);

            break;

        case ARRAY_VFP:
            // Attempt to convert the VFP to an INT
            *lpaplLongestRht = mpfr_get_sx ((LPAPLVFP) lpSymGlbRht, &bRet);

            break;

        defstop
            break;
    } // End SWITCH

    if (!bRet)
        return bRet;

    // Set the new immediate type
    if (lpimmTypeRht)
        *lpimmTypeRht = IMMTYPE_INT;

    // Test the value
    return
      ValidateIntegerTest ((LPAPLINT) lpaplLongestRht,      // Ptr to the integer to test
                           DEF_MIN_QUADPW,                  // Low range value (inclusive)
                           DEF_MAX_QUADPW,                  // High ...
                           bRangeLimit.PW);                 // TRUE iff we're range limiting
} // End ValidNdxPW


//***************************************************************************
//  $ValidSetRL_EM
//
//  Validate a value before assigning it to []RL
//***************************************************************************

UBOOL ValidSetRL_EM
    (LPTOKEN lptkNamArg,            // Ptr to name arg token
     LPTOKEN lptkRhtArg)            // Ptr to right arg token

{
    // Ensure the argument is either an integer scalar or
    //   one-element vector (demoted to a scalar)
    //   in the range for []RL.
    return ValidateInteger_EM (lptkNamArg,          // Ptr to name arg token
                               lptkRhtArg,          // Ptr to right arg token
                               DEF_MIN_QUADRL,      // Minimum value
               bResetVars.RL ? DEF_QUADRL_CWS
                             : (UINT) uQuadRL_CWS,  // Default ...
                               DEF_MAX_QUADRL,      // Maximum ...
                               bRangeLimit.RL);     // TRUE iff range limiting
} // End ValidSetRL_EM


//***************************************************************************
//  $ValidNdxRL
//
//  Validate a single value before assigning it to a position in []RL.
//
//  We allow any number between DEF_MIN_QUADRL and DEF_MAX_QUADRL inclusive.
//***************************************************************************

UBOOL ValidNdxRL
    (APLINT       aplIntegerLst,            // The origin-0 index value (in case the position is important)
     APLSTYPE     aplTypeRht,               // Right arg storage type
     LPAPLLONGEST lpaplLongestRht,          // Ptr to the right arg value
     LPIMM_TYPES  lpimmTypeRht,             // Ptr to right arg immediate type (may be NULL)
     HGLOBAL      lpSymGlbRht,              // Ptr to right arg global value
     LPTOKEN      lptkFunc)                 // Ptr to function token

{
    UBOOL bRet = TRUE;                      // TRUE iff the result is valid

    // Split cases based upon the right arg storage type
    switch (aplTypeRht)
    {
        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
            break;

        case ARRAY_FLOAT:
            // The right arg is float -- convert to integer
            *lpaplLongestRht = FloatToAplint_SCT (*(LPAPLFLOAT) lpaplLongestRht, &bRet);

            break;

        case ARRAY_CHAR:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            return FALSE;

        case ARRAY_RAT:
            // Attempt to convert the RAT to an INT
            *lpaplLongestRht = mpq_get_sx ((LPAPLRAT) lpSymGlbRht, &bRet);

            break;

        case ARRAY_VFP:
            // Attempt to convert the VFP to an INT
            *lpaplLongestRht = mpfr_get_sx ((LPAPLVFP) lpSymGlbRht, &bRet);

            break;

        defstop
            break;
    } // End SWITCH

    if (!bRet)
        return bRet;

    // Set the new immediate type
    if (lpimmTypeRht)
        *lpimmTypeRht = IMMTYPE_INT;

    // Test the value
    return
      ValidateIntegerTest ((LPAPLINT) lpaplLongestRht,      // Ptr to the integer to test
                           DEF_MIN_QUADRL,                  // Low range value (inclusive)
                           DEF_MAX_QUADRL,                  // High ...
                           bRangeLimit.RL);                 // TRUE iff we're range limiting
} // End ValidNdxRL


//***************************************************************************
//  $ValidSetSA_EM
//
//  Validate a value before assigning it to []SA
//***************************************************************************

#ifdef DEBUG
#define APPEND_NAME     L" -- ValidSetSA_EM"
#else
#define APPEND_NAME
#endif

UBOOL ValidSetSA_EM
    (LPTOKEN lptkNamArg,            // Ptr to name arg token
     LPTOKEN lptkRhtArg)            // Ptr to right arg token

{
    APLSTYPE     aplTypeRht;        // Right arg storage type
    APLNELM      aplNELMRht;        // Right arg NELM
    APLRANK      aplRankRht;        // Right arg rank
    HGLOBAL      hGlbRht = NULL,    // Right arg global memory handle
                 hGlbRes;           // Result    ...
    LPVOID       lpMemRht = NULL;   // Ptr to right arg global memory
    UBOOL        bRet = FALSE;      // TRUE iff result is valid
    LPPERTABDATA lpMemPTD;          // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Ensure the argument is either a character scalar (promoted to a vector)
    //   or vector, and a valid Stop Action value
    //   ('', 'EXIT', 'ERROR', 'CLEAR', 'OFF') uppercase only

    // Split cases based upon the token type
    switch (lptkRhtArg->tkFlags.TknType)
    {
        case TKT_VARNAMED:
            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            if (!lptkRhtArg->tkData.tkSym->stFlags.Imm)
            {
                // Get the HGLOBAL
                hGlbRht = lptkRhtArg->tkData.tkSym->stData.stGlbData;

                break;      // Continue with HGLOBAL processing
            } // End IF

            // Handle the immediate case

            // tkData is an LPSYMENTRY
            Assert (GetPtrTypeDir (lptkRhtArg->tkData.tkVoid) EQ PTRTYPE_STCONST);

            // Fall through to common error code

        case TKT_VARIMMED:
            goto RANK_EXIT;

        case TKT_LISTPAR:   // The tkData is an HGLOBAL of an array of HGLOBALs
            goto SYNTAX_EXIT;

        case TKT_CHRSTRAND: // tkData is an HGLOBAL of an array of ???
        case TKT_NUMSTRAND: // tkData is an HGLOBAL of an array of ???
        case TKT_NUMSCALAR: // tkData is an HGLOBAL of an array of ???
        case TKT_VARARRAY:  // tkData is an HGLOBAL of an array of ???
            // Get the HGLOBAL
            hGlbRht = lptkRhtArg->tkData.tkGlbData;

            break;          // Continue with HGLOBAL processing

        defstop
            return FALSE;
    } // End SWITCH

    // tkData is a valid HGLOBAL variable array
    Assert (IsGlbTypeVarDir_PTB (hGlbRht));

    // Lock the memory to get a ptr to it
    lpMemRht = MyGlobalLock (hGlbRht);

#define lpHeader    ((LPVARARRAY_HEADER) lpMemRht)
    // Get the Array Type, NELM, and Rank
    aplTypeRht = lpHeader->ArrType;
    aplNELMRht = lpHeader->NELM;
    aplRankRht = lpHeader->Rank;
#undef  lpHeader

    // Skip over the header and dimensions to the data
    lpMemRht = VarArrayDataFmBase (lpMemRht);

    // Check for vector
    if (!IsVector (aplRankRht))
        goto RANK_EXIT;
    else
    // Split cases based upon the array type
    switch (aplTypeRht)
    {
        case ARRAY_CHAR:
            // Check for the various possible values for []SA
            switch (aplNELMRht)
            {
                case 0:     // ""
                    hGlbRes = hGlbSAEmpty;
                    lpMemPTD->cQuadxSA = SAVAL_Empty;

                    break;

                case 3:     // "OFF"
                    if (memcmp (lpMemRht, SAOff  , sizeof (SAOff)   - sizeof (APLCHAR)) EQ 0)
                    {
                        hGlbRes = hGlbSAOff;
                        lpMemPTD->cQuadxSA = SAVAL_Off;
                    } else
                        goto DOMAIN_EXIT;
                    break;

                case 4:     // "EXIT"
                    if (memcmp (lpMemRht, SAExit , sizeof (SAExit)  - sizeof (APLCHAR)) EQ 0)
                    {
                        hGlbRes = hGlbSAExit;
                        lpMemPTD->cQuadxSA = SAVAL_Exit;
                    } else
                        goto DOMAIN_EXIT;
                    break;

                case 5:     // "CLEAR", "ERROR"
                    if (memcmp (lpMemRht, SAClear, sizeof (SAClear) - sizeof (APLCHAR)) EQ 0)
                    {
                        hGlbRes = hGlbSAClear;
                        lpMemPTD->cQuadxSA = SAVAL_Clear;
                    } else
                    if (memcmp (lpMemRht, SAError, sizeof (SAError) - sizeof (APLCHAR)) EQ 0)
                    {
                        hGlbRes = hGlbSAError;
                        lpMemPTD->cQuadxSA = SAVAL_Error;
                    } else
                        goto DOMAIN_EXIT;
                    break;

                default:
                    goto DOMAIN_EXIT;
            } // End SWITCH

            break;

        case ARRAY_BOOL:
        case ARRAY_INT:
        case ARRAY_APA:
        case ARRAY_FLOAT:
        case ARRAY_HETERO:
        case ARRAY_NESTED:
            goto DOMAIN_EXIT;

        defstop
            break;
    } // End IF/ELSE/SWITCH

    // Free the old value
    FreeResultGlobalVar (lptkNamArg->tkData.tkSym->stData.stGlbData); lptkNamArg->tkData.tkSym->stData.stGlbData = NULL;

    // Save as new value
    lptkNamArg->tkData.tkSym->stData.stGlbData = MakePtrTypeGlb (hGlbRes);
    lptkNamArg->tkFlags.NoDisplay = TRUE;

    // Mark as successful
    bRet = TRUE;

    goto UNLOCK_EXIT;

RANK_EXIT:
    ErrorMessageIndirectToken (ERRMSG_RANK_ERROR APPEND_NAME,
                              lptkNamArg);
    goto ERROR_EXIT;

DOMAIN_EXIT:
    ErrorMessageIndirectToken (ERRMSG_DOMAIN_ERROR APPEND_NAME,
                               lptkNamArg);
    goto ERROR_EXIT;

SYNTAX_EXIT:
    ErrorMessageIndirectToken (ERRMSG_SYNTAX_ERROR APPEND_NAME,
                               lptkNamArg);
    goto ERROR_EXIT;

ERROR_EXIT:
UNLOCK_EXIT:
    if (hGlbRht && lpMemRht)
    {
        // We no longer need this ptr
        MyGlobalUnlock (hGlbRht); lpMemRht = NULL;
    } // End IF

    return bRet;
} // End ValidSetSA_EM
#undef  APPEND_NAME


//***************************************************************************
//  $ValidSetWSID_EM
//
//  Validate a value before assigning it to []WSID
//***************************************************************************

UBOOL ValidSetWSID_EM
    (LPTOKEN lptkNamArg,            // Ptr to name arg token
     LPTOKEN lptkRhtArg)            // Ptr to right arg token

{
    // Ensure the argument is either a character scalar (promoted to a vector)
    //   or a character vector.
    return ValidateCharVector_EM (lptkNamArg, lptkRhtArg, TRUE);
} // End ValidSetWSID_EM


//***************************************************************************
//  $ValidPostWSID
//
//  Post-validate code for []WSID
//***************************************************************************

void ValidPostWSID
    (LPTOKEN lptkNamArg)            // Ptr to name arg token

{
    LPAPLCHAR lpMemWSID;

    // Lock the memory to get a ptr to it
    lpMemWSID = MyGlobalLock (lptkNamArg->tkData.tkGlbData);

    // Skip over the header and dimensions to the data
    lpMemWSID = VarArrayDataFmBase (lpMemWSID);

    // Tell the Tab Ctrl about the new workspace name
    NewTabName (lpMemWSID);

    // We no longer need this ptr
    MyGlobalUnlock (lptkNamArg->tkData.tkGlbData); lpMemWSID = NULL;
} // End ValidPostWSID_EM


//***************************************************************************
//  $ValidSetZ_EM
//
//  Validate a value before assigning it to []Z
//***************************************************************************

UBOOL ValidSetZ_EM
    (LPTOKEN lptkNamArg,            // Ptr to name arg token
     LPTOKEN lptkRhtArg)            // Ptr to right arg token

{
    HGLOBAL    hGlbRht;             // Right arg global memory handle
    APLLONGEST aplLongestRht;       // Right arg immediate value

    // Get right arg's global ptrs
    aplLongestRht = GetGlbPtrs (lptkRhtArg, &hGlbRht);

    // Free the old value (if there is one)
    if (lptkNamArg->tkData.tkSym->stFlags.Value)
    {
        FreeResultGlobalVar (lptkNamArg->tkData.tkSym->stData.stGlbData); lptkNamArg->tkData.tkSym->stData.stGlbData = NULL;
    } // End IF

    // If the right arg is a global, ...
    if (hGlbRht)
        // Save as new value
        lptkNamArg->tkData.tkSym->stData.stGlbData = CopySymGlbDirAsGlb (hGlbRht);
    else
    {
        // Save as new value
        lptkNamArg->tkData.tkSym->stFlags.Imm      = TRUE;
        lptkNamArg->tkData.tkSym->stFlags.ImmType  = lptkRhtArg->tkFlags.ImmType;
        lptkNamArg->tkData.tkSym->stData.stLongest = aplLongestRht;
    } // End IF/ELSE

    // Set common flags
    lptkNamArg->tkFlags.NoDisplay = TRUE;
    lptkNamArg->tkData.tkSym->stFlags.Value = TRUE;

    return TRUE;
} // End ValidSetZ_EM


//***************************************************************************
//  $ValidNdxAny
//
//  Validate a single value before assigning it to
//    a position in a SysVar.
//***************************************************************************

UBOOL ValidNdxAny
    (APLINT       aplIntegerLst,            // The origin-0 index value (in case the position is important)
     APLSTYPE     aplTypeRht,               // Right arg storage type
     LPAPLLONGEST lpaplLongestRht,          // Ptr to the right arg value
     LPIMM_TYPES  lpimmTypeRht,             // Ptr to right arg immediate type (may be NULL)
     HGLOBAL      lpSymGlbRht,              // Ptr to right arg global value
     LPTOKEN      lptkFunc)                 // Ptr to function token

{
    return TRUE;
} // End ValidNdxAny


//***************************************************************************
//  $ValidPostNone
//
//  Post-validation for SysVars that don't need any
//***************************************************************************

void ValidPostNone
    (LPTOKEN lptkNamArg)                    // Ptr to name arg token

{
} // End ValidPostNone


//***************************************************************************
//  $AssignDefaultSysVars
//
//  Assign default values to the system vars
//***************************************************************************

void AssignDefaultSysVars
    (LPPERTABDATA lpMemPTD)             // Ptr to PerTabData global memory

{
    _AssignDefaultSysVars (lpMemPTD, NULL);
} // End AssignDefaultSysvars


//***************************************************************************
//  $_AssignDefaultSysVars
//
//  Assign default values to the system vars using a specific HTS
//***************************************************************************

void _AssignDefaultSysVars
    (LPPERTABDATA lpMemPTD,             // Ptr to PerTabData global memory
     LPHSHTABSTR  lphtsPTD)             // Ptr to HshTabStr (may be NULL)

{
    LPSYMENTRY *lpSymQuad;

    // If we're to supply our own HTS, ...
    if (lphtsPTD EQ NULL)
        // Get a ptr to the HshTab & SymTab strucs
        lphtsPTD = lpMemPTD->lphtsPTD;

    lpSymQuad = lphtsPTD->lpSymQuad;

    AssignGlobalCWS     (hGlbQuadALX_CWS     , SYSVAR_ALX     , lpSymQuad[SYSVAR_ALX     ]);    // Attention Latent Expression
    AssignRealScalarCWS (fQuadCT_CWS         , SYSVAR_CT      , lpSymQuad[SYSVAR_CT      ]);    // Comparison Tolerance
    AssignGlobalCWS     (hGlbV0Char          , SYSVAR_DM      , lpSymQuad[SYSVAR_DM      ]);    // Diagnostic Message
    AssignCharScalarCWS (cQuadDT_CWS         , SYSVAR_DT      , lpSymQuad[SYSVAR_DT      ]);    // Distribution Type
    AssignGlobalCWS     (hGlbQuadELX_CWS     , SYSVAR_ELX     , lpSymQuad[SYSVAR_ELX     ]);    // Error Latent Expression
    AssignGlobalCWS     (hGlbQuadFC_CWS      , SYSVAR_FC      , lpSymQuad[SYSVAR_FC      ]);    // Format Control
    AssignGlobalCWS     (hGlbQuadFEATURE_CWS , SYSVAR_FEATURE , lpSymQuad[SYSVAR_FEATURE ]);    // Feature Control
    AssignIntScalarCWS  (uQuadFPC_CWS        , SYSVAR_FPC     , lpSymQuad[SYSVAR_FPC     ]);    // Floating Point Control
    AssignGlobalCWS     (hGlbQuadIC_CWS      , SYSVAR_IC      , lpSymQuad[SYSVAR_IC      ]);    // Indeterminate Control
    AssignBoolScalarCWS (bQuadIO_CWS         , SYSVAR_IO      , lpSymQuad[SYSVAR_IO      ]);    // Index Origin
    AssignGlobalCWS     (hGlbQuadLX_CWS      , SYSVAR_LX      , lpSymQuad[SYSVAR_LX      ]);    // Latent Expression
    AssignIntScalarCWS  (uQuadPP_CWS         , SYSVAR_PP      , lpSymQuad[SYSVAR_PP      ]);    // Print Precision
    if (cQuadPR_CWS EQ CQUADPR_MT)
        AssignGlobalCWS (hGlbQuadPR_CWS      , SYSVAR_PR      , lpSymQuad[SYSVAR_PR      ]);    // Prompt Replacement
    else
        AssignCharScalarCWS (cQuadPR_CWS     , SYSVAR_PR      , lpSymQuad[SYSVAR_PR      ]);    // Prompt Replacement
    AssignIntScalarCWS  (uQuadPW_CWS         , SYSVAR_PW      , lpSymQuad[SYSVAR_PW      ]);    // Print Width
    AssignIntScalarCWS  (uQuadRL_CWS         , SYSVAR_RL      , lpSymQuad[SYSVAR_RL      ]);    // Random Link
    AssignGlobalCWS     (hGlbQuadSA_CWS      , SYSVAR_SA      , lpSymQuad[SYSVAR_SA      ]);    // Stop Action
    AssignGlobalCWS     (hGlbQuadWSID_CWS    , SYSVAR_WSID    , lpSymQuad[SYSVAR_WSID    ]);    // Workspace Identifier

    // Set the values for []Z
    lpSymQuad[SYSVAR_Z]->stFlags = lphtsPTD->steNoValue->stFlags;
    lpSymQuad[SYSVAR_Z]->stFlags.Inuse       = TRUE;
    lpSymQuad[SYSVAR_Z]->stFlags.SysVarValid = SYSVAR_Z;
} // End _AssignDefaultSysVars


//***************************************************************************
//  $CopySysVars
//
//  Make a copy of the current system vars so we have up-to-date values
//***************************************************************************

void CopySysVars
    (LPHSHTABSTR lphtsDst,          // Destination HSHTABSTR
     LPHSHTABSTR lphtsSrc)          // Source      ...

{

    // Copy scalar values
    lphtsDst->lpSymQuad[SYSVAR_CT      ]->stData.stLongest = lphtsSrc->lpSymQuad[SYSVAR_CT      ]->stData.stLongest;
    lphtsDst->lpSymQuad[SYSVAR_DT      ]->stData.stLongest = lphtsSrc->lpSymQuad[SYSVAR_DT      ]->stData.stLongest;
    lphtsDst->lpSymQuad[SYSVAR_FPC     ]->stData.stLongest = lphtsSrc->lpSymQuad[SYSVAR_FPC     ]->stData.stLongest;
    lphtsDst->lpSymQuad[SYSVAR_IO      ]->stData.stLongest = lphtsSrc->lpSymQuad[SYSVAR_IO      ]->stData.stLongest;
    lphtsDst->lpSymQuad[SYSVAR_PP      ]->stData.stLongest = lphtsSrc->lpSymQuad[SYSVAR_PP      ]->stData.stLongest;
    lphtsDst->lpSymQuad[SYSVAR_PW      ]->stData.stLongest = lphtsSrc->lpSymQuad[SYSVAR_PW      ]->stData.stLongest;
    lphtsDst->lpSymQuad[SYSVAR_RL      ]->stData.stLongest = lphtsSrc->lpSymQuad[SYSVAR_RL      ]->stData.stLongest;

    // Delete existing destination HGLOBAL values in case they are not permanents
    DeleSysVars (lphtsDst);

    // Copy HGLOBAL values
////lphtsDst->lpSymQuad[SYSVAR_ALX     ]->stData.stGlbData = CopySymGlbDir_PTB (lphtsSrc->lpSymQuad[SYSVAR_ALX     ]->stData.stGlbData);    // Not used in {}
////lphtsDst->lpSymQuad[SYSVAR_DM      ]->stData.stGlbData = CopySymGlbDir_PTB (lphtsSrc->lpSymQuad[SYSVAR_DM      ]->stData.stGlbData);    // ...
////lphtsDst->lpSymQuad[SYSVAR_ELX     ]->stData.stGlbData = CopySymGlbDir_PTB (lphtsSrc->lpSymQuad[SYSVAR_ELX     ]->stData.stGlbData);    // Not used in {}
    lphtsDst->lpSymQuad[SYSVAR_FC      ]->stData.stGlbData = CopySymGlbDir_PTB (lphtsSrc->lpSymQuad[SYSVAR_FC      ]->stData.stGlbData); lphtsDst->lpSymQuad[SYSVAR_FC      ]->stFlags.Value = TRUE;
    lphtsDst->lpSymQuad[SYSVAR_FEATURE ]->stData.stGlbData = CopySymGlbDir_PTB (lphtsSrc->lpSymQuad[SYSVAR_FEATURE ]->stData.stGlbData); lphtsDst->lpSymQuad[SYSVAR_FEATURE ]->stFlags.Value = TRUE;
    lphtsDst->lpSymQuad[SYSVAR_IC      ]->stData.stGlbData = CopySymGlbDir_PTB (lphtsSrc->lpSymQuad[SYSVAR_IC      ]->stData.stGlbData); lphtsDst->lpSymQuad[SYSVAR_IC      ]->stFlags.Value = TRUE;
////lphtsDst->lpSymQuad[SYSVAR_LX      ]->stData.stGlbData = CopySymGlbDir_PTB (lphtsSrc->lpSymQuad[SYSVAR_LX      ]->stData.stGlbData);    // Not used in {}
////lphtsDst->lpSymQuad[SYSVAR_PR      ]->stData.stGlbData = CopySymGlbDir_PTB (lphtsSrc->lpSymQuad[SYSVAR_PR      ]->stData.stGlbData);    // Not used in {}
////lphtsDst->lpSymQuad[SYSVAR_SA      ]->stData.stGlbData = CopySymGlbDir_PTB (lphtsSrc->lpSymQuad[SYSVAR_SA      ]->stData.stGlbData);    // Not used in {}
////lphtsDst->lpSymQuad[SYSVAR_WSID    ]->stData.stGlbData = CopySymGlbDir_PTB (lphtsSrc->lpSymQuad[SYSVAR_WSID    ]->stData.stGlbData);    // Not used in {}
////lphtsDst->lpSymQuad[SYSVAR_Z       ]->stData.stGlbData = CopySymGlbDir_PTB (lphtsSrc->lpSymQuad[SYSVAR_Z       ]->stData.stGlbData);    // Local to {}
} // End CopySysVars


//***************************************************************************
//  $DeleSysVars
//
//  Delete HGLOBAL system vars
//***************************************************************************

void DeleSysVars
    (LPHSHTABSTR lphtsDst)          // Destination HSHTABSTR

{
    // If the HSHTABSTR is valid, ...
    if (lphtsDst)
    {
    // Delete HGLOBAL values
////////FreeResultGlobalVar (lphtsDst->lpSymQuad[SYSVAR_ALX     ]->stData.stGlbData);   // Not used in {}
////////FreeResultGlobalVar (lphtsDst->lpSymQuad[SYSVAR_DM      ]->stData.stGlbData);   // Not used in {}
////////FreeResultGlobalVar (lphtsDst->lpSymQuad[SYSVAR_ELX     ]->stData.stGlbData);   // Not used in {}
        FreeResultGlobalVar (lphtsDst->lpSymQuad[SYSVAR_FC      ]->stData.stGlbData); lphtsDst->lpSymQuad[SYSVAR_FC      ]->stData.stGlbData = NULL; lphtsDst->lpSymQuad[SYSVAR_FC      ]->stFlags.Value = FALSE;
        FreeResultGlobalVar (lphtsDst->lpSymQuad[SYSVAR_FEATURE ]->stData.stGlbData); lphtsDst->lpSymQuad[SYSVAR_FEATURE ]->stData.stGlbData = NULL; lphtsDst->lpSymQuad[SYSVAR_FEATURE ]->stFlags.Value = FALSE;
        FreeResultGlobalVar (lphtsDst->lpSymQuad[SYSVAR_IC      ]->stData.stGlbData); lphtsDst->lpSymQuad[SYSVAR_IC      ]->stData.stGlbData = NULL; lphtsDst->lpSymQuad[SYSVAR_IC      ]->stFlags.Value = FALSE;
////////FreeResultGlobalVar (lphtsDst->lpSymQuad[SYSVAR_LX      ]->stData.stGlbData);   // Not used in {}
////////FreeResultGlobalVar (lphtsDst->lpSymQuad[SYSVAR_PR      ]->stData.stGlbData);   // Not used in {}
////////FreeResultGlobalVar (lphtsDst->lpSymQuad[SYSVAR_SA      ]->stData.stGlbData);   // Not used in {}
////////FreeResultGlobalVar (lphtsDst->lpSymQuad[SYSVAR_WSID    ]->stData.stGlbData);   // Not used in {}
////////FreeResultGlobalVar (lphtsDst->lpSymQuad[SYSVAR_Z       ]->stData.stGlbData);   // Not used in {}
    } // End IF
} // End DeleSysVars


//***************************************************************************
//  $InitSysVars
//
//  Initialize all system vars
//***************************************************************************

void InitSysVars
    (void)

{
    LPPERTABDATA lpMemPTD;      // Ptr to PerTabData global memory

    // Get ptr to PerTabData global memory
    lpMemPTD = GetMemPTD ();

    // Set the variant operator validation routines
    aVariantKeyStr[VARIANT_KEY_CT].aSysVarValidSet = ValidSetCT_EM;
    aVariantKeyStr[VARIANT_KEY_DT].aSysVarValidSet = ValidSetDT_EM;
    aVariantKeyStr[VARIANT_KEY_IO].aSysVarValidSet = ValidSetIO_EM;
    aVariantKeyStr[VARIANT_KEY_PP].aSysVarValidSet = ValidSetPP_EM;

    // Set the array set validation routines
    aSysVarValidSet[SYSVAR_ALX     ] = ValidSetALX_EM      ;
    aSysVarValidSet[SYSVAR_CT      ] = ValidSetCT_EM       ;
    aSysVarValidSet[SYSVAR_DM      ] = ValidSetDM_EM       ;
    aSysVarValidSet[SYSVAR_DT      ] = ValidSetDT_EM       ;
    aSysVarValidSet[SYSVAR_ELX     ] = ValidSetELX_EM      ;
    aSysVarValidSet[SYSVAR_FC      ] = ValidSetFC_EM       ;
    aSysVarValidSet[SYSVAR_FEATURE ] = ValidSetFEATURE_EM  ;
    aSysVarValidSet[SYSVAR_FPC     ] = ValidSetFPC_EM      ;
    aSysVarValidSet[SYSVAR_IC      ] = ValidSetIC_EM       ;
    aSysVarValidSet[SYSVAR_IO      ] = ValidSetIO_EM       ;
    aSysVarValidSet[SYSVAR_LX      ] = ValidSetLX_EM       ;
    aSysVarValidSet[SYSVAR_PP      ] = ValidSetPP_EM       ;
    aSysVarValidSet[SYSVAR_PR      ] = ValidSetPR_EM       ;
    aSysVarValidSet[SYSVAR_PW      ] = ValidSetPW_EM       ;
    aSysVarValidSet[SYSVAR_RL      ] = ValidSetRL_EM       ;
    aSysVarValidSet[SYSVAR_SA      ] = ValidSetSA_EM       ;
    aSysVarValidSet[SYSVAR_WSID    ] = ValidSetWSID_EM     ;
    aSysVarValidSet[SYSVAR_Z       ] = ValidSetZ_EM        ;

    // Set the array index validation routine
    aSysVarValidNdx[SYSVAR_ALX     ] = ValidNdxChar        ;
    aSysVarValidNdx[SYSVAR_CT      ] = ValidNdxCT          ;
    aSysVarValidNdx[SYSVAR_DM      ] = ValidNdxDM          ;
    aSysVarValidNdx[SYSVAR_DT      ] = ValidNdxDT          ;
    aSysVarValidNdx[SYSVAR_ELX     ] = ValidNdxChar        ;
    aSysVarValidNdx[SYSVAR_FC      ] = ValidNdxChar        ;
    aSysVarValidNdx[SYSVAR_FEATURE ] = ValidNdxFEATURE     ;
    aSysVarValidNdx[SYSVAR_FPC     ] = ValidNdxFPC         ;
    aSysVarValidNdx[SYSVAR_IC      ] = ValidNdxIC          ;
    aSysVarValidNdx[SYSVAR_IO      ] = ValidNdxIO          ;
    aSysVarValidNdx[SYSVAR_LX      ] = ValidNdxChar        ;
    aSysVarValidNdx[SYSVAR_PP      ] = ValidNdxPP          ;
    aSysVarValidNdx[SYSVAR_PR      ] = ValidNdxChar        ;
    aSysVarValidNdx[SYSVAR_PW      ] = ValidNdxPW          ;
    aSysVarValidNdx[SYSVAR_RL      ] = ValidNdxRL          ;
    aSysVarValidNdx[SYSVAR_SA      ] = ValidNdxChar        ;
    aSysVarValidNdx[SYSVAR_WSID    ] = ValidNdxChar        ;
    aSysVarValidNdx[SYSVAR_Z       ] = ValidNdxAny         ;

    // Set the array index validation routine
    aSysVarValidPost[SYSVAR_ALX     ] = ValidPostNone       ;
    aSysVarValidPost[SYSVAR_CT      ] = ValidPostNone       ;
    aSysVarValidPost[SYSVAR_DM      ] = ValidPostNone       ;
    aSysVarValidPost[SYSVAR_DT      ] = ValidPostNone       ;
    aSysVarValidPost[SYSVAR_ELX     ] = ValidPostNone       ;
    aSysVarValidPost[SYSVAR_FC      ] = ValidPostNone       ;
    aSysVarValidPost[SYSVAR_FEATURE ] = ValidPostNone       ;
    aSysVarValidPost[SYSVAR_FPC     ] = ValidPostNone       ;
    aSysVarValidPost[SYSVAR_IC      ] = ValidPostNone       ;
    aSysVarValidPost[SYSVAR_IO      ] = ValidPostNone       ;
    aSysVarValidPost[SYSVAR_LX      ] = ValidPostNone       ;
    aSysVarValidPost[SYSVAR_PP      ] = ValidPostNone       ;
    aSysVarValidPost[SYSVAR_PR      ] = ValidPostNone       ;
    aSysVarValidPost[SYSVAR_PW      ] = ValidPostNone       ;
    aSysVarValidPost[SYSVAR_RL      ] = ValidPostNone       ;
    aSysVarValidPost[SYSVAR_SA      ] = ValidPostNone       ;
    aSysVarValidPost[SYSVAR_WSID    ] = ValidPostWSID       ;
    aSysVarValidPost[SYSVAR_Z       ] = ValidPostNone       ;

    // Assign default values to the system vars
    AssignDefaultSysVars (lpMemPTD);

    // Save the index value
    lpMemPTD->cQuadxSA = cQuadxSA_CWS;
} // End InitSysVars


//***************************************************************************
//  End of File: sysvars.c
//***************************************************************************
