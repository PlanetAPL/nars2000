//***************************************************************************
//  NARS2000 -- Starting Adddresses
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

#ifdef _WIN64
  extern void __chkstk (void);
#else
  extern void  _chkstk (void);
#endif

typedef struct tagSTART_ADDRESSES
{
    char   *StartAddressName;
    LPUCHAR StartAddressAddr;
} START_ADDRESSES, *LPSTART_ADDRESSES;


#ifndef PROTO
  START_ADDRESSES StartAddresses[] =
  {
    // afofns.c
    "AfoReturn"                 , (LPUCHAR) &AfoReturn                  ,
    "AfoGuard"                  , (LPUCHAR) &AfoGuard                   ,
    "MakeAfo_EM_YY"             , (LPUCHAR) &MakeAfo_EM_YY              ,
    "AfoDisplay_EM"             , (LPUCHAR) &AfoDisplay_EM              ,
    "SISAfo"                    , (LPUCHAR) &SISAfo                     ,
    "IsAfoName"                 , (LPUCHAR) &IsAfoName                  ,
    "ConvertAfoAlias"           , (LPUCHAR) &ConvertAfoAlias            ,

    // assign.c
    "AssignName_EM"             , (LPUCHAR) &AssignName_EM              ,
    "GetNameType"               , (LPUCHAR) &GetNameType                ,
    "AssignArrayCommon"         , (LPUCHAR) &AssignArrayCommon          ,
    "AssignNamedVars_EM"        , (LPUCHAR) &AssignNamedVars_EM         ,
    "ModifyAssignNameVals_EM"   , (LPUCHAR) &ModifyAssignNameVals_EM    ,

    // axisfns.c
    "CheckAxisOper"             , (LPUCHAR) &CheckAxisOper              ,

    // bjhash.c
    "hashlittle"                , (LPUCHAR) &hashlittle                 ,

    // chkesp.c

    // common.c

    // convert.c

    // crash.c
    "CCWndProc"                 , (LPUCHAR) &CCWndProc                  ,
    "LclCCListboxWndProc"       , (LPUCHAR) &LclCCListboxWndProc        ,

    // cs_parse.c
    "cs_yyparse"                , (LPUCHAR) &cs_yyparse                 ,
    "ParseCtrlStruc_EM"         , (LPUCHAR) &ParseCtrlStruc_EM          ,

    // cs_proc.c
    "CS_ChangeTokenType"        , (LPUCHAR) &CS_ChangeTokenType         ,
    "CS_ChainTokens"            , (LPUCHAR) &CS_ChainTokens             ,
    "CS_LinkStmt"               , (LPUCHAR) &CS_LinkStmt                ,
    "CS_UnlinkStmt"             , (LPUCHAR) &CS_UnlinkStmt              ,
    "CS_SetCLIndex"             , (LPUCHAR) &CS_SetCLIndex              ,
    "CS_SetTokenCLIndex"        , (LPUCHAR) &CS_SetTokenCLIndex         ,
    "CS_CASE_Stmt"              , (LPUCHAR) &CS_CASE_Stmt               ,
    "CS_CONTINUE_Stmt"          , (LPUCHAR) &CS_CONTINUE_Stmt           ,
    "CS_ELSE_Stmt"              , (LPUCHAR) &CS_ELSE_Stmt               ,
    "FindMatchingForStmt"       , (LPUCHAR) &FindMatchingForStmt        ,
    "CS_ENDFOR_Stmt_EM"         , (LPUCHAR) &CS_ENDFOR_Stmt_EM          ,
    "CS_DoneFOR"                , (LPUCHAR) &CS_DoneFOR                 ,
    "CS_ENDREPEAT_Stmt"         , (LPUCHAR) &CS_ENDREPEAT_Stmt          ,
    "CS_ENDWHILE_Stmt"          , (LPUCHAR) &CS_ENDWHILE_Stmt           ,
    "CS_FOR_Stmt_EM"            , (LPUCHAR) &CS_FOR_Stmt_EM             ,
    "CS_IF_Stmt_EM"             , (LPUCHAR) &CS_IF_Stmt_EM              ,
    "CS_LEAVE_Stmt"             , (LPUCHAR) &CS_LEAVE_Stmt              ,
    "CS_SELECT_Stmt_EM"         , (LPUCHAR) &CS_SELECT_Stmt_EM          ,
    "CS_SKIPCASE_Stmt"          , (LPUCHAR) &CS_SKIPCASE_Stmt           ,
    "CS_SKIPEND_Stmt"           , (LPUCHAR) &CS_SKIPEND_Stmt            ,
    "CS_GetToken_ORIG"          , (LPUCHAR) &CS_GetToken_ORIG           ,
    "CS_GetToken_NEXT"          , (LPUCHAR) &CS_GetToken_NEXT           ,
    "GetDfnHdrHandle"           , (LPUCHAR) &GetDfnHdrHandle            ,
    "CS_CopyOrigToNext"         , (LPUCHAR) &CS_CopyOrigToNext          ,
    "CS_PointToSOSToken_NEXT"   , (LPUCHAR) &CS_PointToSOSToken_NEXT    ,
    "CS_SetNextStmtToStmtAt_NEXT", (LPUCHAR) &CS_SetNextStmtToStmtAt_NEXT,
////"CS_SetNextStmtToStmtAt_ORIG", (LPUCHAR) &CS_SetNextStmtToStmtAt_ORIG,
    "CS_SetNextStmtToStmtAfter_NEXT", (LPUCHAR) &CS_SetNextStmtToStmtAfter_NEXT,
    "CS_SetNextStmtToStmtAfter_ORIG", (LPUCHAR) &CS_SetNextStmtToStmtAfter_ORIG,
    "CS_SetNextToken_NEXT"      , (LPUCHAR) &CS_SetNextToken_NEXT       ,
    "CS_GetTokenType_ORIG"      , (LPUCHAR) &CS_GetTokenType_ORIG       ,
    "CS_GetTokenType_NEXT"      , (LPUCHAR) &CS_GetTokenType_NEXT       ,
    "CS_GetEOSTokenLen_NEXT"    , (LPUCHAR) &CS_GetEOSTokenLen_NEXT     ,

    // customize.c
    "CustomizeDlgProc"          , (LPUCHAR) &CustomizeDlgProc           ,
    "LclKeybEditCtrlWndProc"    , (LPUCHAR) &LclKeybEditCtrlWndProc     ,
    "LclFontsStaticTextWndProc" , (LPUCHAR) &LclFontsStaticTextWndProc  ,
    "DrawButton"                , (LPUCHAR) &DrawButton                 ,
    "DisplayKeybTCValue"        , (LPUCHAR) &DisplayKeybTCValue         ,
    "KeybHighlight"             , (LPUCHAR) &KeybHighlight              ,
    "DispKeybLayout"            , (LPUCHAR) &DispKeybLayout             ,
    "DispKeybCtrlKeycaps"       , (LPUCHAR) &DispKeybCtrlKeycaps        ,
    "KeybScanCodeToID"          , (LPUCHAR) &KeybScanCodeToID           ,
    "KeybCharToScanCode"        , (LPUCHAR) &KeybCharToScanCode         ,
    "SetKeybFont"               , (LPUCHAR) &SetKeybFont                ,
    "NewKeybDlgProc"            , (LPUCHAR) &NewKeybDlgProc             ,
    "EnumCallbackResizeMC"      , (LPUCHAR) &EnumCallbackResizeMC       ,
    "EnumCallbackRepaint"       , (LPUCHAR) &EnumCallbackRepaint        ,
    "FillSyntaxColor"           , (LPUCHAR) &FillSyntaxColor            ,
    "GetClearWsChrValue"        , (LPUCHAR) &GetClearWsChrValue         ,
    "GetClearWsComValue"        , (LPUCHAR) &GetClearWsComValue         ,
    "InitThemes"                , (LPUCHAR) &InitThemes                 ,
    "FinThemes"                 , (LPUCHAR) &FinThemes                  ,
    "GetFontWeightW"            , (LPUCHAR) &GetFontWeightW             ,
    "GetLogPixelsY"             , (LPUCHAR) &GetLogPixelsY              ,
    "DirsBrowseCallbackProc"    , (LPUCHAR) &DirsBrowseCallbackProc     ,

    // debug.c
  #ifdef DEBUG
    "CreateDebuggerWindow"      , (LPUCHAR) &CreateDebuggerWindow       ,
    "CreateDebuggerInThread"    , (LPUCHAR) &CreateDebuggerInThread     ,
    "DB_Create"                 , (LPUCHAR) DB_Create                   ,
    "DB_Delete"                 , (LPUCHAR) DB_Delete                   ,
    "DBWndProc"                 , (LPUCHAR) DBWndProc                   ,
    "LclListboxWndProc"         , (LPUCHAR) &LclListboxWndProc          ,
    "DbgMsg"                    , (LPUCHAR) &DbgMsg                     ,
    "DbgMsgW"                   , (LPUCHAR) &DbgMsgW                    ,
    "DbgClr"                    , (LPUCHAR) &DbgClr                     ,
    "oprintfW"                  , (LPUCHAR) &oprintfW                   ,
    "dprintfWL0"                , (LPUCHAR) &dprintfWL0                 ,
    "dprintfWL9"                , (LPUCHAR) &dprintfWL9                 ,
    "DbgGlobalAllocSub"         , (LPUCHAR) &DbgGlobalAllocSub          ,
    "FileNameOnly"              , (LPUCHAR) &FileNameOnly               ,
  #endif

    // fh_parse.c/y

    // dispdbg.c
  #ifdef DEBUG
    "DisplayHshTab"             , (LPUCHAR) &DisplayHshTab              ,
  #endif
    "FormatHTE"                 , (LPUCHAR) &FormatHTE                  ,
  #ifdef DEBUG
    "DisplaySymTab"             , (LPUCHAR) &DisplaySymTab              ,
  #endif
    "FormatSTE"                 , (LPUCHAR) &FormatSTE                  ,
  #ifdef DEBUG
    "UpdateDBWindow"            , (LPUCHAR) &UpdateDBWindow             ,
    "DisplayGlobals"            , (LPUCHAR) &DisplayGlobals             ,
    "DisplayHeap"               , (LPUCHAR) &DisplayHeap                ,
    "DisplayTokens"             , (LPUCHAR) &DisplayTokens              ,
    "DisplayFcnStrand"          , (LPUCHAR) &DisplayFcnStrand           ,
  #endif
    "DisplayFcnGlb"             , (LPUCHAR) &DisplayFcnGlb              ,
    "DisplayFcnMem"             , (LPUCHAR) &DisplayFcnMem              ,
    "DisplayFcnSub"             , (LPUCHAR) &DisplayFcnSub              ,
    "DisplayVarSub"             , (LPUCHAR) &DisplayVarSub              ,
  #ifdef DEBUG
    "DisplayFcnArr"             , (LPUCHAR) &DisplayFcnArr              ,
    "DisplayFcnLine"            , (LPUCHAR) &DisplayFcnLine             ,
    "DisplayStrand"             , (LPUCHAR) &DisplayStrand              ,
    "DisplayUndo"               , (LPUCHAR) &DisplayUndo                ,
    "DisplayFnHdr"              , (LPUCHAR) &DisplayFnHdr               ,
    "DisplayYYRes"              , (LPUCHAR) &DisplayYYRes               ,
  #endif

    // display.c
    "ArrayDisplay_EM"           , (LPUCHAR) &ArrayDisplay_EM            ,
    "DisplayGlbArr_EM"          , (LPUCHAR) &DisplayGlbArr_EM           ,
    "FormatImmed"               , (LPUCHAR) &FormatImmed                ,
    "FormatImmedFC"             , (LPUCHAR) &FormatImmedFC              ,
    "FormatAplint"              , (LPUCHAR) &FormatAplint               ,
    "FormatAplintFC"            , (LPUCHAR) &FormatAplintFC             ,
    "FormatAplRat"              , (LPUCHAR) &FormatAplRat               ,
    "FormatAplRatFC"            , (LPUCHAR) &FormatAplRatFC             ,
    "FormatFloat"               , (LPUCHAR) &FormatFloat                ,
    "FormatFloatFC"             , (LPUCHAR) &FormatFloatFC              ,
    "FormatExpFmt"              , (LPUCHAR) &FormatExpFmt               ,
    "FormatAplVfp"              , (LPUCHAR) &FormatAplVfp               ,
    "FormatAplVfpFC"            , (LPUCHAR) &FormatAplVfpFC             ,
    "FormatSymTabConst"         , (LPUCHAR) &FormatSymTabConst          ,

    // dtoa.c

    // editctrl.c
    "EditWndProcW"              , (LPUCHAR) &EditWndProcW               ,
    "EditWndProcA"              , (LPUCHAR) &EditWndProcA               ,

    // editfcn.c
    "CreateFcnWindow"           , (LPUCHAR) &CreateFcnWindow            ,
    "SetMarginsFE"              , (LPUCHAR) &SetMarginsFE               ,
    "FE_Create"                 , (LPUCHAR) &FE_Create                  ,
    "FE_Delete"                 , (LPUCHAR) &FE_Delete                  ,
    "FEWndProc"                 , (LPUCHAR) &FEWndProc                  ,
    "GetFunctionName"           , (LPUCHAR) &GetFunctionName            ,
    "SetFETitle"                , (LPUCHAR) &SetFETitle                 ,
    "SyntaxColor"               , (LPUCHAR) &SyntaxColor                ,
    "LclECPaintHook"            , (LPUCHAR) &LclECPaintHook             ,
    "IzitNameChar"              , (LPUCHAR) &IzitNameChar               ,
    "GetVkState"                , (LPUCHAR) &GetVkState                 ,
    "LclEditCtrlWndProc"        , (LPUCHAR) &LclEditCtrlWndProc         ,

    // editmat.c
    "MEWndProc"                 , (LPUCHAR) &MEWndProc                  ,

    // editvec.c
    "VEWndProc"                 , (LPUCHAR) &VEWndProc                  ,

    // errmsg.c
    "BreakMessage"              , (LPUCHAR) &BreakMessage               ,
    "ErrorMessageDirect"        , (LPUCHAR) &ErrorMessageDirect         ,
    "ErrorMessageIndirect"      , (LPUCHAR) &ErrorMessageIndirect       ,
    "ErrorMessageIndirectToken" , (LPUCHAR) &ErrorMessageIndirectToken  ,
    "ErrorMessageSetToken"      , (LPUCHAR) &ErrorMessageSetToken       ,
    "ErrorMessageSetCharIndex"  , (LPUCHAR) &ErrorMessageSetCharIndex   ,

    // except.c
    "MyGetExceptionCode"        , (LPUCHAR) &MyGetExceptionCode         ,
    "MySetExceptionCode"        , (LPUCHAR) &MySetExceptionCode         ,
    "CheckException"            , (LPUCHAR) &CheckException             ,
    "CompareStartAddresses"     , (LPUCHAR) &CompareStartAddresses      ,
    "_DisplayException"         , (LPUCHAR) &DisplayException           ,

    // execdfn.c
    "ExecDfnGlbProto_EM_YY"     , (LPUCHAR) &ExecDfnGlbProto_EM_YY      ,
    "ExecDfnGlb_EM_YY"          , (LPUCHAR) &ExecDfnGlb_EM_YY           ,
    "ExecDfnOprGlb_EM_YY"       , (LPUCHAR) &ExecDfnOprGlb_EM_YY        ,
    "LocalizeAll"               , (LPUCHAR) &LocalizeAll                ,
    "_CheckSymEntries"          , (LPUCHAR) &_CheckSymEntries           ,
    "ExecuteFunction_EM_YY"     , (LPUCHAR) &ExecuteFunction_EM_YY      ,
    "CheckDfnExitError_EM"      , (LPUCHAR) &CheckDfnExitError_EM       ,
    "UnlocalizeSTEs"            , (LPUCHAR) &UnlocalizeSTEs             ,
    "LocalizeLabels"            , (LPUCHAR) &LocalizeLabels             ,
    "InitVarSTEs"               , (LPUCHAR) &InitVarSTEs                ,
    "InitFcnSTEs"               , (LPUCHAR) &InitFcnSTEs                ,
    "LocalizeSymEntries"        , (LPUCHAR) &LocalizeSymEntries         ,

    // execfns.c
    "ExecuteFn0"                , (LPUCHAR) &ExecuteFn0                 ,
    "ExecFunc_EM_YY"            , (LPUCHAR) &ExecFunc_EM_YY             ,
    "ExecFcnGlb_EM_YY"          , (LPUCHAR) &ExecFcnGlb_EM_YY           ,
    "ExecFuncStr_EM_YY"         , (LPUCHAR) &ExecFuncStr_EM_YY          ,
    "ExecOp1_EM_YY"             , (LPUCHAR) &ExecOp1_EM_YY              ,
    "ExecOp2_EM_YY"             , (LPUCHAR) &ExecOp2_EM_YY              ,

    // execmfn.c
    "ExecuteMagicFunction_EM_YY", (LPUCHAR) &ExecuteMagicFunction_EM_YY ,

    // fastbool.c
    "FastBoolScan"              , (LPUCHAR) &FastBoolScan               ,

    // fmtspec.c
    "fnFmtExit"                 , (LPUCHAR) &fnFmtExit                  ,
    "fnFmtDone"                 , (LPUCHAR) &fnFmtDone                  ,
    "fnDnLvl"                   , (LPUCHAR) &fnDnLvl                    ,
    "InitFmtSpecStr"            , (LPUCHAR) &InitFmtSpecStr             ,
    "fnUpLvl"                   , (LPUCHAR) &fnUpLvl                    ,
    "fnSetText"                 , (LPUCHAR) &fnSetText                  ,
    "SaveText"                  , (LPUCHAR) &SaveText                   ,
    "fnRepAcc"                  , (LPUCHAR) &fnRepAcc                   ,
    "ScanNumberFS"              , (LPUCHAR) &ScanNumberFS               ,
    "fnSetModO"                 , (LPUCHAR) &fnSetModO                  ,
    "fnSetQual"                 , (LPUCHAR) &fnSetQual                  ,
    "fnError"                   , (LPUCHAR) &fnError                    ,
    "FmtSpec_EM"                , (LPUCHAR) &FmtSpec_EM                 ,
    "CharTransFS"               , (LPUCHAR) &CharTransFS                ,
  #ifdef DEBUG
    "InitFsaTabFS"              , (LPUCHAR) &InitFsaTabFS               ,
  #endif

    // free.c
    "FreeResultName"            , (LPUCHAR) &FreeResultName             ,
    "FreeResult"                , (LPUCHAR) &FreeResult                 ,
    "FreeResultTkn"             , (LPUCHAR) &FreeResultTkn              ,
    "FreeResNNU"                , (LPUCHAR) &FreeResNNU                 ,
    "FreeResultSub"             , (LPUCHAR) &FreeResultSub              ,
    "FreeResultGlobalDFLV"      , (LPUCHAR) &FreeResultGlobalDFLV       ,
    "FreeResultGlobalLst"       , (LPUCHAR) &FreeResultGlobalLst        ,
    "FreeResultGlobalVar"       , (LPUCHAR) &FreeResultGlobalVar        ,
    "FreeResultGlobalIncompleteVar", (LPUCHAR) &FreeResultGlobalIncompleteVar,
    "FreeResultGlobalVarSub"    , (LPUCHAR) &FreeResultGlobalVarSub     ,
    "FreeResultGlobalFcn"       , (LPUCHAR) &FreeResultGlobalFcn        ,
    "FreeResultGlobalDfn"       , (LPUCHAR) &FreeResultGlobalDfn        ,
    "FreeResultGlobalDfnStruc"  , (LPUCHAR) &FreeResultGlobalDfnStruc   ,
    "FreeYYFcn1"                , (LPUCHAR) &FreeYYFcn1                 ,

    // getfns.c
    "GetFirstItemToken"         , (LPUCHAR) &GetFirstItemToken          ,
    "GetNextValueTokenIntoToken", (LPUCHAR) &GetNextValueTokenIntoToken ,
    "GetNextValueToken"         , (LPUCHAR) &GetNextValueToken          ,
    "GetFirstValueToken"        , (LPUCHAR) &GetFirstValueToken         ,
    "GetFirstValueImm"          , (LPUCHAR) &GetFirstValueImm           ,
    "GetFirstValueGlb"          , (LPUCHAR) &GetFirstValueGlb           ,
    "GetNextValueMemIntoToken"  , (LPUCHAR) &GetNextValueMemIntoToken   ,
    "GetNextInteger"            , (LPUCHAR) &GetNextInteger             ,
    "GetNextFloat"              , (LPUCHAR) &GetNextFloat               ,
    "GetNextHetero"             , (LPUCHAR) &GetNextHetero              ,
    "GetNextItemGlb"            , (LPUCHAR) &GetNextItemGlb             ,
    "GetNextValueGlb"           , (LPUCHAR) &GetNextValueGlb            ,
    "GetNextValueMem"           , (LPUCHAR) &GetNextValueMem            ,
    "GetNextItemMem"            , (LPUCHAR) &GetNextItemMem             ,
    "GetNextValueMemSub"        , (LPUCHAR) &GetNextValueMemSub         ,
    "GetGlbHandle"              , (LPUCHAR) &GetGlbHandle               ,
    "GetGlbPtrs"                , (LPUCHAR) &GetGlbPtrs                 ,
    "GetGlbPtrs_LOCK"           , (LPUCHAR) &GetGlbPtrs_LOCK            ,
    "GetSteZero"                , (LPUCHAR) &GetSteZero                 ,
    "GetSteOne"                 , (LPUCHAR) &GetSteOne                  ,
    "GetSteBlank"               , (LPUCHAR) &GetSteBlank                ,
    "GetQuadCT"                 , (LPUCHAR) &GetQuadCT                  ,
    "GetQuadIO"                 , (LPUCHAR) &GetQuadIO                  ,
    "SetQuadIO"                 , (LPUCHAR) &SetQuadIO                  ,
    "GetQuadPP"                 , (LPUCHAR) &GetQuadPP                  ,
    "GetQuadPW"                 , (LPUCHAR) &GetQuadPW                  ,
    "GetQuadRL"                 , (LPUCHAR) &GetQuadRL                  ,
    "SetQuadRL"                 , (LPUCHAR) &SetQuadRL                  ,
    "SavePrimSpecRL"            , (LPUCHAR) &SavePrimSpecRL             ,
    "RestPrimSpecRL"            , (LPUCHAR) &RestPrimSpecRL             ,
    "GetPrototypeFcnPtr"        , (LPUCHAR) &GetPrototypeFcnPtr         ,
    "GetPrimFlagsPtr"           , (LPUCHAR) &GetPrimFlagsPtr            ,
    "GetImmedType"              , (LPUCHAR) &GetImmedType               ,
    "GetImmTypeGlb"             , (LPUCHAR) &GetImmTypeGlb              ,
    "GetSignatureGlb"           , (LPUCHAR) &GetSignatureGlb            ,
    "GetSignatureGlb_PTB"       , (LPUCHAR) &GetSignatureGlb_PTB        ,
    "GetMemPTD"                 , (LPUCHAR) &GetMemPTD                  ,

    // getreent.c
    "__getreent"                , (LPUCHAR) &__getreent                 ,

    // goto.c
    "GotoLine_EM"               , (LPUCHAR) &GotoLine_EM                ,

    // immexec.c
    "WaitForImmExecStmt"        , (LPUCHAR) &WaitForImmExecStmt         ,
    "ImmExecStmtInThread"       , (LPUCHAR) &ImmExecStmtInThread        ,

    // initdata.c
    "InitPrimTabs"              , (LPUCHAR) &InitPrimTabs               ,

    // main.c
    "EnumCallbackPassMsg"       , (LPUCHAR) &EnumCallbackPassMsg        ,
    "MFWndProc"                 , (LPUCHAR) &MFWndProc                  ,

    // mpfns.c
    "mp_alloc"                  , (LPUCHAR) &mp_alloc                   ,
    "mp_realloc"                , (LPUCHAR) &mp_realloc                 ,
    "mp_free"                   , (LPUCHAR) &mp_free                    ,
    "mpq_init_set"              , (LPUCHAR) &mpq_init_set               ,
    "mpq_init_set_sx"           , (LPUCHAR) &mpq_init_set_sx            ,
    "mpq_init_set_si"           , (LPUCHAR) &mpq_init_set_si            ,
    "mpq_init_set_ui"           , (LPUCHAR) &mpq_init_set_ui            ,
    "mpq_init_set_d"            , (LPUCHAR) &mpq_init_set_d             ,
    "mpq_set_sx"                , (LPUCHAR) &mpq_set_sx                 ,
    "mpfr_init_set_sx"          , (LPUCHAR) &mpfr_init_set_sx           ,
    "mpfr_set_sx"               , (LPUCHAR) &mpfr_set_sx                ,
    "mpfr_pow"                  , (LPUCHAR) &mpfr_pow                   ,
    "mpfr_ui_cmp"               , (LPUCHAR) &mpfr_ui_cmp                ,
    "mpfr_si_cmp"               , (LPUCHAR) &mpfr_si_cmp                ,
    "Myf_clear"                 , (LPUCHAR) &Myf_clear                  ,

    // mpifns.c
    "mp_get_invalid_functions"  , (LPUCHAR) &mp_get_invalid_functions   ,
    "mp_set_invalid_functions"  , (LPUCHAR) &mp_set_invalid_functions   ,
    "signumint"                 , (LPUCHAR) &signumint                  ,
    "signumflt"                 , (LPUCHAR) &signumflt                  ,
    "mpz_exit"                  , (LPUCHAR) &mpz_exit                   ,
    "mpz_inf_p"                 , (LPUCHAR) &mpz_inf_p                  ,
    "IsMpzNULL"                 , (LPUCHAR) &IsMpzNULL                  ,

    // pf_bar.c
    "PrimFnBar_EM_YY"           , (LPUCHAR) &PrimFnBar_EM_YY            ,

    // pf_circle.c
    "PrimFnCircle_EM_YY"        , (LPUCHAR) &PrimFnCircle_EM_YY         ,

    // pf_colonbar.c
    "PrimFnColonBar_EM_YY"      , (LPUCHAR) &PrimFnColonBar_EM_YY       ,

    // pf_comma.c
    "PrimFnComma_EM_YY"         , (LPUCHAR) &PrimFnComma_EM_YY          ,

    // pf_cslope.c
    "PrimFnCircleSlope_EM_YY"   , (LPUCHAR) &PrimFnCircleSlope_EM_YY    ,

    // pf_cstar.c
    "PrimFnCircleStar_EM_YY"    , (LPUCHAR) &PrimFnCircleStar_EM_YY     ,

    // pf_cstile.c
    "PrimFnCircleStile_EM_YY"   , (LPUCHAR) &PrimFnCircleStile_EM_YY    ,

    // pf_darrow.c
    "PrimFnDownArrow_EM_YY"     , (LPUCHAR) &PrimFnDownArrow_EM_YY      ,

    // pf_dcaret.c
    "PrimFnDownCaret_EM_YY"     , (LPUCHAR) &PrimFnDownCaret_EM_YY      ,

    // pf_deltastile.c
    "PrimFnMonGradeCommon_EM_YY", (LPUCHAR) &PrimFnMonGradeCommon_EM_YY ,
    "PermVecGrade"              , (LPUCHAR) &PermVecGrade               ,
    "BoolVecGrade"              , (LPUCHAR) &BoolVecGrade               ,
    "PrimFnDydGradeCommon_EM_YY", (LPUCHAR) &PrimFnDydGradeCommon_EM_YY ,
    "PrimFnGradeCompare"        , (LPUCHAR) &PrimFnGradeCompare         ,

    // pf_domino.c
    "PrimFnDomino_EM_YY"        , (LPUCHAR) &PrimFnDomino_EM_YY         ,

    // pf_dshoe.c
    "PrimFnDownShoe_EM_YY"      , (LPUCHAR) &PrimFnDownShoe_EM_YY       ,

    // pf_dstile.c
    "PrimFnDownStile_EM_YY"     , (LPUCHAR) &PrimFnDownStile_EM_YY      ,

    // pf_dtack.c
    "PrimFnDownTack_EM_YY"      , (LPUCHAR) &PrimFnDownTack_EM_YY       ,
    "PrimProtoFnDownTack_EM_YY" , (LPUCHAR) &PrimProtoFnDownTack_EM_YY  ,
    "PrimFnMonDownTack_EM_YY"   , (LPUCHAR) &PrimFnMonDownTack_EM_YY    ,
    "PrimFnDydDownTack_EM_YY"   , (LPUCHAR) &PrimFnDydDownTack_EM_YY    ,

    // pf_dtackjot.c
    "PrimFnDownTackJot_EM_YY"   , (LPUCHAR) &PrimFnDownTackJot_EM_YY    ,
    "PrimProtoFnDownTackJot_EM_YY",(LPUCHAR) &PrimProtoFnDownTackJot_EM_YY,
    "PrimFnMonDownTackJot_EM_YY", (LPUCHAR) &PrimFnMonDownTackJot_EM_YY ,
    "CompileArrBool"            , (LPUCHAR) &CompileArrBool             ,
    "CompileArrInteger"         , (LPUCHAR) &CompileArrInteger          ,
    "CompileArrFloat"           , (LPUCHAR) &CompileArrFloat            ,
    "CompileArrChar"            , (LPUCHAR) &CompileArrChar             ,
    "CompileArrAPA"             , (LPUCHAR) &CompileArrAPA              ,
    "CompileArrHetero"          , (LPUCHAR) &CompileArrHetero           ,
    "CompileArrNested"          , (LPUCHAR) &CompileArrNested           ,
    "CompileArrNestedCon"       , (LPUCHAR) &CompileArrNestedCon        ,
    "CompileArrNestedGlb"       , (LPUCHAR) &CompileArrNestedGlb        ,
    "CompileArrRat"             , (LPUCHAR) &CompileArrRat              ,
    "CompileArrVfp"             , (LPUCHAR) &CompileArrVfp              ,
    "InteriorColWidth"          , (LPUCHAR) &InteriorColWidth           ,
    "ExteriorColWidth"          , (LPUCHAR) &ExteriorColWidth           ,
    "InteriorHdrWidth"          , (LPUCHAR) &InteriorHdrWidth           ,
    "ExteriorHdrWidth"          , (LPUCHAR) &ExteriorHdrWidth           ,
    "PropagateRowColCount"      , (LPUCHAR) &PropagateRowColCount       ,
    "CompileBlankRows"          , (LPUCHAR) &CompileBlankRows           ,
    "AppendBlankRows"           , (LPUCHAR) &AppendBlankRows            ,
    "FormatArrSimple"           , (LPUCHAR) &FormatArrSimple            ,
    "FormatArrNested"           , (LPUCHAR) &FormatArrNested            ,
    "FormatArrNestedCon"        , (LPUCHAR) &FormatArrNestedCon         ,
    "FormatArrNestedGlb"        , (LPUCHAR) &FormatArrNestedGlb         ,

    // pf_epsilon.c
    "PrimFnEpsilon_EM_YY"       , (LPUCHAR) &PrimFnEpsilon_EM_YY        ,

    // pf_epsund.c
    "PrimFnEpsilonUnderbar_EM_YY", (LPUCHAR) &PrimFnEpsilonUnderbar_EM_YY,

    // pf_equal.c
    "PrimFnEqual_EM_YY"         , (LPUCHAR) &PrimFnEqual_EM_YY          ,

    // pf_equalund.c
    "PrimFnEqualUnderbar_EM_YY" , (LPUCHAR) &PrimFnEqualUnderbar_EM_YY  ,

    // pf_index.c
    "ArrayIndexRef_EM_YY"       , (LPUCHAR) &ArrayIndexRef_EM_YY        ,
    "ArrayIndexRefLstImm_EM_YY" , (LPUCHAR) &ArrayIndexRefLstImm_EM_YY  ,
    "ArrayIndexRefLstSimpGlb_EM_YY", (LPUCHAR) &ArrayIndexRefLstSimpGlb_EM_YY,
    "ArrayIndexRefNamImmed_EM_YY", (LPUCHAR) &ArrayIndexRefNamImmed_EM_YY,
    "ArrayIndexValidZilde_EM"   , (LPUCHAR) &ArrayIndexValidZilde_EM    ,
    "ArrayIndexRefRect_EM_YY"   , (LPUCHAR) &ArrayIndexRefRect_EM_YY    ,
    "ListIndexRef_EM_YY"        , (LPUCHAR) &ListIndexRef_EM_YY         ,
    "ArrayIndexSet_EM"          , (LPUCHAR) &ArrayIndexSet_EM           ,
    "ArrayIndexSetNamImmed_EM"  , (LPUCHAR) &ArrayIndexSetNamImmed_EM   ,
    "ArrayIndexSetNoLst_EM"     , (LPUCHAR) &ArrayIndexSetNoLst_EM      ,
    "ArrayIndexSetSingLst_EM"   , (LPUCHAR) &ArrayIndexSetSingLst_EM    ,
    "ArrayIndexSetVector_EM"    , (LPUCHAR) &ArrayIndexSetVector_EM     ,
    "ArrayIndexSetRect_EM"      , (LPUCHAR) &ArrayIndexSetRect_EM       ,
    "ArrayIndexReplace_EM"      , (LPUCHAR) &ArrayIndexReplace_EM       ,
    "ArrayIndexFcnSet_EM"       , (LPUCHAR) &ArrayIndexFcnSet_EM        ,

    // pf_iota.c
    "PrimFnIota_EM_YY"          , (LPUCHAR) &PrimFnIota_EM_YY           ,

    // pf_iotaund.c
    "PrimFnIotaUnderbar_EM_YY"  , (LPUCHAR) &PrimFnIotaUnderbar_EM_YY   ,

    // pf_lcaret.c
    "PrimFnLeftCaret_EM_YY"     , (LPUCHAR) &PrimFnLeftCaret_EM_YY      ,

    // pf_lcaretund.c
    "PrimFnLeftCaretUnderbar_EM_YY", (LPUCHAR) &PrimFnLeftCaretUnderbar_EM_YY,

    // pf_lshoe.c
    "PrimFnLeftShoe_EM_YY"      , (LPUCHAR) &PrimFnLeftShoe_EM_YY       ,

    // pf_lshoeund.c
    "PrimFnLeftShoeUnderbar_EM_YY", (LPUCHAR) &PrimFnLeftShoeUnderbar_EM_YY,

    // pf_ltack.c
    "PrimFnLeftTack_EM_YY"      , (LPUCHAR) &PrimFnLeftTack_EM_YY       ,

    // pf_ndcaret.c
    "PrimFnDownCaretTilde_EM_YY", (LPUCHAR) &PrimFnDownCaretTilde_EM_YY ,

    // pf_nequal.c
    "PrimFnNotEqual_EM_YY"      , (LPUCHAR) &PrimFnNotEqual_EM_YY       ,

    // pf_nucaret.c
    "PrimFnUpCaretTilde_EM_YY"  , (LPUCHAR) &PrimFnUpCaretTilde_EM_YY   ,

    // pf_pi.c
    "PrimFnPi_EM_YY"            , (LPUCHAR) &PrimFnPi_EM_YY             ,

    // pf_plus.c
    "PrimFnPlus_EM_YY"          , (LPUCHAR) &PrimFnPlus_EM_YY           ,

    // pf_qdot.c
    "PrimFnQuoteDot_EM_YY"      , (LPUCHAR) &PrimFnQuoteDot_EM_YY       ,

    // pf_query.c
    "PrimFnQuery_EM_YY"         , (LPUCHAR) &PrimFnQuery_EM_YY          ,
    "PrimFnDydQuery_EM_YY"      , (LPUCHAR) &PrimFnDydQuery_EM_YY       ,

    // pf_rcaret.c
    "PrimFnRightCaret_EM_YY"    , (LPUCHAR) &PrimFnRightCaret_EM_YY     ,

    // pf_rcaretund.c
    "PrimFnRightCaretUnderbar_EM_YY", (LPUCHAR) &PrimFnRightCaretUnderbar_EM_YY,

    // pf_rho.c
    "PrimFnRho_EM_YY"           , (LPUCHAR) &PrimFnRho_EM_YY            ,

    // pf_root.c
    "PrimFnRoot_EM_YY"          , (LPUCHAR) &PrimFnRoot_EM_YY           ,

    // pf_rshoe.c
    "PrimFnRightShoe_EM_YY"     , (LPUCHAR) &PrimFnRightShoe_EM_YY      ,

    // pf_rshoeund.c
    "PrimFnRightShoeUnderbar_EM_YY", (LPUCHAR) &PrimFnRightShoeUnderbar_EM_YY,

    // pf_rtack.c
    "PrimFnRightTack_EM_YY"     , (LPUCHAR) &PrimFnRightTack_EM_YY      ,

    // pf_section.c
    "PrimFnSection_EM_YY"       , (LPUCHAR) &PrimFnSection_EM_YY        ,

    // pf_slash.c
    "PrimFnDydSlash_EM_YY"      , (LPUCHAR) &PrimFnDydSlash_EM_YY       ,

    // pf_slope.c
    "PrimFnDydSlope_EM_YY"      , (LPUCHAR) &PrimFnDydSlope_EM_YY       ,

    // pf_squad.c
    "PrimFnSquad_EM_YY"         , (LPUCHAR) &PrimFnSquad_EM_YY          ,
    "PrimProtoFnSquad_EM_YY"    , (LPUCHAR) &PrimProtoFnSquad_EM_YY     ,
    "PrimFnMonSquad_EM_YY"      , (LPUCHAR) &PrimFnMonSquad_EM_YY       ,
    "PrimFnDydSquad_EM_YY"      , (LPUCHAR) &PrimFnDydSquad_EM_YY       ,
    "PrimFnDydSquadGlb_EM_YY"   , (LPUCHAR) &PrimFnDydSquadGlb_EM_YY    ,

    // pf_star.c
    "PrimFnStar_EM_YY"          , (LPUCHAR) &PrimFnStar_EM_YY           ,

    // pf_stile.c
    "PrimFnStile_EM_YY"         , (LPUCHAR) &PrimFnStile_EM_YY          ,

    // pf_tilde.c
    "PrimFnTilde_EM_YY"         , (LPUCHAR) &PrimFnTilde_EM_YY          ,

    // pf_times.c
    "PrimFnTimes_EM_YY"         , (LPUCHAR) &PrimFnTimes_EM_YY          ,

    // pf_uarrow.c
    "PrimFnUpArrow_EM_YY"       , (LPUCHAR) &PrimFnUpArrow_EM_YY        ,

    // pf_ucaret.c
    "PrimFnUpCaret_EM_YY"       , (LPUCHAR) &PrimFnUpCaret_EM_YY        ,

    // pf_ushoe.c
    "PrimFnUpShoe_EM_YY"        , (LPUCHAR) &PrimFnUpShoe_EM_YY         ,

    // pf_ustile.c
    "PrimFnUpStile_EM_YY"       , (LPUCHAR) &PrimFnUpStile_EM_YY        ,

    // pf_utack.c
    "PrimFnUpTack_EM_YY"        , (LPUCHAR) &PrimFnUpTack_EM_YY         ,
    "PrimFnMonUpTack_EM_YY"     , (LPUCHAR) &PrimFnMonUpTack_EM_YY      ,
    "PrimFnDydUpTack_EM_YY"     , (LPUCHAR) &PrimFnDydUpTack_EM_YY      ,

    // pf_utackjot.c
    "PrimFnMonUptackJotCSPLParse",(LPUCHAR) &PrimFnMonUpTackJotCSPLParse,
    "PrimFnMonUptackJotPLParse" , (LPUCHAR) &PrimFnMonUpTackJotPLParse  ,

    // pl_parse.c/y
    "pl_yyparse"                , (LPUCHAR) &pl_yyparse                 ,
    "ParseLine"                 , (LPUCHAR) &ParseLine                  ,
    "SymbTypeVFO"               , (LPUCHAR) &SymbTypeVFO                ,
    "LookaheadSurround"         , (LPUCHAR) &LookaheadSurround          ,
    "LookaheadAdjacent"         , (LPUCHAR) &LookaheadAdjacent          ,
    "LookaheadDyadicOp"         , (LPUCHAR) &LookaheadDyadicOp          ,
    "LookbehindOp"              , (LPUCHAR) &LookbehindOp               ,
    "pl_yylex"                  , (LPUCHAR) &pl_yylex                   ,
    "CheckNullOp3"              , (LPUCHAR) &CheckNullOp3               ,
    "pl_yyerror"                , (LPUCHAR) &pl_yyerror                 ,
    "pl_yyfprintf"              , (LPUCHAR) &pl_yyfprintf               ,
    "WaitForInput"              , (LPUCHAR) &WaitForInput               ,
    "AmbOpSwap_EM"              , (LPUCHAR) &AmbOpSwap_EM               ,
    "AmbOpToFcn"                , (LPUCHAR) &AmbOpToFcn                 ,
    "AmbOpToOp1"                , (LPUCHAR) &AmbOpToOp1                 ,
    "ArrExprCheckCaller"        , (LPUCHAR) &ArrExprCheckCaller         ,
    "CheckSelSpec_EM"           , (LPUCHAR) &CheckSelSpec_EM            ,
    "IsSelectSpec"              , (LPUCHAR) &IsSelectSpec               ,
    "MakeTempCopy"              , (LPUCHAR) &MakeTempCopy               ,
    "GetEOSIndex"               , (LPUCHAR) &GetEOSIndex                ,
    "IsLastStmt"                , (LPUCHAR) &IsLastStmt                 ,

    // pn_parse.c/y
    "pn_yyparse"                , (LPUCHAR) &pn_yyparse                 ,

    // pn_proc.c
    "PN_NumAcc"                 , (LPUCHAR) &PN_NumAcc                  ,
    "PN_NumCalc"                , (LPUCHAR) &PN_NumCalc                 ,
    "PN_ChrAcc"                 , (LPUCHAR) &PN_ChrAcc                  ,
    "PN_MakeBasePoint"          , (LPUCHAR) &PN_MakeBasePoint           ,
    "PN_MakeEulerPoint"         , (LPUCHAR) &PN_MakeEulerPoint          ,
    "PN_MakeExpPoint"           , (LPUCHAR) &PN_MakeExpPoint            ,
    "PN_MakePiPoint"            , (LPUCHAR) &PN_MakePiPoint             ,
    "PN_MakeVfpPoint"           , (LPUCHAR) &PN_MakeVfpPoint            ,
    "PN_VectorAcc"              , (LPUCHAR) &PN_VectorAcc               ,
    "PN_VectorRes"              , (LPUCHAR) &PN_VectorRes               ,
    "PN_SetInfinity"            , (LPUCHAR) &PN_SetInfinity             ,

    // po_cmiddot.c
    "PrimOpDydCircleMiddleDotCommon_EM_YY", (LPUCHAR) &PrimOpDydCircleMiddleDotCommon_EM_YY,

    // po_dicircle.c
    "PrimOpDieresisCircle_EM_YY", (LPUCHAR) &PrimOpDieresisCircle_EM_YY ,

    // po_dieresis.c
    "PrimOpDieresis_EM_YY"      , (LPUCHAR) &PrimOpDieresis_EM_YY       ,

    // po_dijot.c
    "PrimOpDieresisJot_EM_YY"   , (LPUCHAR) &PrimOpDieresisJot_EM_YY    ,

    // po_ditilde.c
    "PrimOpDieresisTilde_EM_YY" , (LPUCHAR) &PrimOpDieresisTilde_EM_YY  ,

    // po_dot.c
    "PrimOpDot_EM_YY"           , (LPUCHAR) &PrimOpDot_EM_YY            ,
    "PrimProtoOpDot_EM_YY"      , (LPUCHAR) &PrimProtoOpDot_EM_YY       ,
    "PrimIdentOpDot_EM_YY"      , (LPUCHAR) &PrimIdentOpDot_EM_YY       ,
    "PrimOpMonDot_EM_YY"        , (LPUCHAR) &PrimOpMonDot_EM_YY         ,
    "PrimOpMonDotCommon_EM_YY"  , (LPUCHAR) &PrimOpMonDotCommon_EM_YY   ,
    "PrimOpDydDot_EM_YY"        , (LPUCHAR) &PrimOpDydDot_EM_YY         ,
    "PrimOpDydDotCommon_EM_YY"  , (LPUCHAR) &PrimOpDydDotCommon_EM_YY   ,
    "ExecDydProto_EM"           , (LPUCHAR) &ExecDydProto_EM            ,

    // po_jot.c
    "PrimOpJot_EM_YY"           , (LPUCHAR) &PrimOpJot_EM_YY            ,

    // po_jotdot.c
    "PrimOpJotDot_EM_YY"        , (LPUCHAR) &PrimOpJotDot_EM_YY         ,

    // po_multiset.c
    "PrimOpDownShoeStile_EM_YY" , (LPUCHAR) &PrimOpDownShoeStile_EM_YY  ,

    // po_slash.c
    "PrimOpSlash_EM_YY"         , (LPUCHAR) &PrimOpSlash_EM_YY          ,
    "PrimProtoOpSlash_EM_YY"    , (LPUCHAR) &PrimProtoOpSlash_EM_YY     ,
    "PrimOpMonSlash_EM_YY"      , (LPUCHAR) &PrimOpMonSlash_EM_YY       ,
    "PrimOpMonSlashCommon_EM_YY", (LPUCHAR) &PrimOpMonSlashCommon_EM_YY ,
    "PrimOpMonSlashScalar_EM_YY", (LPUCHAR) &PrimOpMonSlashScalar_EM_YY ,
    "PrimOpDydSlash_EM_YY"      , (LPUCHAR) &PrimOpDydSlash_EM_YY       ,
    "PrimOpDydSlashCommon_EM_YY", (LPUCHAR) &PrimOpDydSlashCommon_EM_YY ,
    "PrimOpDydSlashInsertDim_EM", (LPUCHAR) &PrimOpDydSlashInsertDim_EM ,
    "PrimOpDydSlashAllocate_EM" , (LPUCHAR) &PrimOpDydSlashAllocate_EM  ,

    // po_slope.c
    "PrimOpSlope_EM_YY"         , (LPUCHAR) &PrimOpSlope_EM_YY          ,

    // primfns.c
    "IsTknFcnOpr"               , (LPUCHAR) &IsTknFcnOpr                ,
    "TokenTypeFV"               , (LPUCHAR) &TokenTypeFV                ,
    "RankOfGlb"                 , (LPUCHAR) &RankOfGlb                  ,
    "AttrsOfToken"              , (LPUCHAR) &AttrsOfToken               ,
    "AttrsOfGlb"                , (LPUCHAR) &AttrsOfGlb                 ,
    "StorageType"               , (LPUCHAR) &StorageType                ,
    "IncrOdometer"              , (LPUCHAR) &IncrOdometer               ,
    "PrimScalarFnDydAllocate_EM", (LPUCHAR) &PrimScalarFnDydAllocate_EM ,
    "MakeMonPrototype_EM_PTB"   , (LPUCHAR) &MakeMonPrototype_EM_PTB    ,
    "MakeDydPrototype_EM_PTB"   , (LPUCHAR) &MakeDydPrototype_EM_PTB    ,
    "IsFirstSimpleGlb"          , (LPUCHAR) &IsFirstSimpleGlb           ,
    "CopySymGlbDir_PTB"         , (LPUCHAR) &CopySymGlbDir_PTB          ,
    "CopyArray_EM"              , (LPUCHAR) &CopyArray_EM               ,
    "CopyGlbAsType_EM"          , (LPUCHAR) &CopyGlbAsType_EM           ,
    "CheckRankLengthError_EM"   , (LPUCHAR) &CheckRankLengthError_EM    ,
    "RoundUpBitsToBytes"        , (LPUCHAR) &RoundUpBitsToBytes         ,
    "RoundUpBitsInArray"        , (LPUCHAR) &RoundUpBitsInArray         ,
    "BytesIn"                   , (LPUCHAR) &BytesIn                    ,
    "abs64"                     , (LPUCHAR) &abs64                      ,
    "_iadd64"                   , (LPUCHAR) &_iadd64                    ,
    "_isub64"                   , (LPUCHAR) &_isub64                    ,
    "_imul64"                   , (LPUCHAR) &_imul64                    ,
    "CalcArraySize"             , (LPUCHAR) &CalcArraySize              ,
    "CalcHeaderSize"            , (LPUCHAR) &CalcHeaderSize             ,
    "CalcFcnSize"               , (LPUCHAR) &CalcFcnSize                ,
    "MakeNoValue_YY"            , (LPUCHAR) &MakeNoValue_YY             ,
    "GetSISLayer"               , (LPUCHAR) &GetSISLayer                ,
    "FillSISNxt"                , (LPUCHAR) &FillSISNxt                 ,
    "PassSigaphore"             , (LPUCHAR) &PassSigaphore              ,
    "IsTknNamed"                , (LPUCHAR) &IsTknNamed                 ,
    "IsTknNamedFcnOpr"          , (LPUCHAR) &IsTknNamedFcnOpr           ,
    "IsTknTypeFcnOpr"           , (LPUCHAR) &IsTknTypeFcnOpr            ,
    "IsTknTypeVar"              , (LPUCHAR) &IsTknTypeVar               ,
    "IsTknNamedVar"             , (LPUCHAR) &IsTknNamedVar              ,
    "IsTknUsrDfn"               , (LPUCHAR) &IsTknUsrDfn                ,
    "IsTknImmed"                , (LPUCHAR) &IsTknImmed                 ,
    "SetVFOArraySRCIFlag"       , (LPUCHAR) &SetVFOArraySRCIFlag        ,
  #ifdef DEBUG
    "GetVFOArraySRCIFlag"       , (LPUCHAR) &GetVFOArraySRCIFlag        ,
  #endif
    "ClrVFOArraySRCIFlag"       , (LPUCHAR) &ClrVFOArraySRCIFlag        ,
////"mod64"                     , (LPUCHAR) &mod64                      ,   // Static

    // primspec.c
    "PrimFnSyntaxError_EM"      , (LPUCHAR) &PrimFnSyntaxError_EM       ,
    "PrimFnValenceError_EM"     , (LPUCHAR) &PrimFnValenceError_EM      ,
    "PrimFnNonceError_EM"       , (LPUCHAR) &PrimFnNonceError_EM        ,
    "PrimFnDomainError_EM"      , (LPUCHAR) &PrimFnDomainError_EM       ,
    "PrimFnValueError_EM"       , (LPUCHAR) &PrimFnValueError_EM        ,
    "PrimFnMonValenceError_EM"  , (LPUCHAR) &PrimFnMonValenceError_EM   ,
    "PrimProtoFnMixed_EM_YY"    , (LPUCHAR) &PrimProtoFnMixed_EM_YY     ,
    "PrimProtoFnScalar_EM_YY"   , (LPUCHAR) &PrimProtoFnScalar_EM_YY    ,
    "PrimIdentFnScalar_EM_YY"   , (LPUCHAR) &PrimIdentFnScalar_EM_YY    ,
    "PrimIdentFnScalarCommon_EM", (LPUCHAR) &PrimIdentFnScalarCommon_EM ,
    "PrimFnMon_EM_YY"           , (LPUCHAR) &PrimFnMon_EM_YY            ,
    "PrimFnMonGlb_EM"           , (LPUCHAR) &PrimFnMonGlb_EM            ,
    "PrimFnDyd_EM_YY"           , (LPUCHAR) &PrimFnDyd_EM_YY            ,
    "PrimFnDydSimpNest_EM"      , (LPUCHAR) &PrimFnDydSimpNest_EM       ,
    "PrimFnDydNestSimp_EM"      , (LPUCHAR) &PrimFnDydNestSimp_EM       ,
    "PrimFnDydNestSiSc_EM"      , (LPUCHAR) &PrimFnDydNestSiSc_EM       ,
    "FillToken_PTB"             , (LPUCHAR) &FillToken_PTB              ,
    "PrimFnDydNestNest_EM"      , (LPUCHAR) &PrimFnDydNestNest_EM       ,
    "PrimFnDydSingMult_EM"      , (LPUCHAR) &PrimFnDydSingMult_EM       ,
    "PrimFnDydMultSing_EM"      , (LPUCHAR) &PrimFnDydMultSing_EM       ,
    "PrimFnDydSiScNest_EM"      , (LPUCHAR) &PrimFnDydSiScNest_EM       ,
    "PrimFnDydSiScSiSc_EM"      , (LPUCHAR) &PrimFnDydSiScSiSc_EM       ,
    "PrimFnDydSiScSiScSub_EM"   , (LPUCHAR) &PrimFnDydSiScSiScSub_EM    ,
    "PrimFnDydSimpSimp_EM"      , (LPUCHAR) &PrimFnDydSimpSimp_EM       ,
    "CalcLftRhtArgIndices"      , (LPUCHAR) &CalcLftRhtArgIndices       ,
    "IsTknBooleanAPA"           , (LPUCHAR) &IsTknBooleanAPA            ,

    // qf_*.c

    // qf_a.c
    "SysFnA_EM_YY"              , (LPUCHAR) &SysFnA_EM_YY               ,

    // qf_at.c
    "SysFnAT_EM_YY"             , (LPUCHAR) &SysFnAT_EM_YY              ,

    // qf_av.c
    "SysFnAV_EM_YY"             , (LPUCHAR) &SysFnAV_EM_YY              ,

    // qf_cr.c
    "SysFnCR_EM_YY"             , (LPUCHAR) &SysFnCR_EM_YY              ,
    "CopySteName"               , (LPUCHAR) &CopySteName                ,

    // qf_dl.c
    "SysFnDL_EM_YY"             , (LPUCHAR) &SysFnDL_EM_YY              ,

    // qf_dr.c
    "SysFnDR_EM_YY"             , (LPUCHAR) &SysFnDR_EM_YY              ,

    // qf_ea.c
    "SysFnEA_EM_YY"             , (LPUCHAR) &SysFnEA_EM_YY              ,

    // qf_ec.c
    "SysFnEC_EM_YY"             , (LPUCHAR) &SysFnEC_EM_YY              ,

    // qf_em.c
    "SysFnEM_EM_YY"             , (LPUCHAR) &SysFnEM_EM_YY              ,

    // qf_error.c
    "SysFnERROR_EM_YY"          , (LPUCHAR) &SysFnERROR_EM_YY           ,

    // qf_es.c
    "SysFnES_EM_YY"             , (LPUCHAR) &SysFnES_EM_YY              ,

    // qf_et.c
    "SysFnET_EM_YY"             , (LPUCHAR) &SysFnET_EM_YY              ,

    // qf_ex.c
    "SysFnEX_EM_YY"             , (LPUCHAR) &SysFnEX_EM_YY              ,

    // qf_fmt.c
    "SysFnFMT_EM_YY"            , (LPUCHAR) &SysFnFMT_EM_YY             ,
    "SysFnMonFMT_EM_YY"         , (LPUCHAR) &SysFnMonFMT_EM_YY          ,
    "SysFnDydFMT_EM_YY"         , (LPUCHAR) &SysFnDydFMT_EM_YY          ,
    "SysFnDydFMTSimp"           , (LPUCHAR) &SysFnDydFMTSimp            ,
    "QFMT_CommonG"              , (LPUCHAR) &QFMT_CommonG               ,
    "QFMT_CommonEFIR"           , (LPUCHAR) &QFMT_CommonEFIR            ,
    "QFMT_ZeroFill"             , (LPUCHAR) &QFMT_ZeroFill              ,
    "QFMT_ThousandSep"          , (LPUCHAR) &QFMT_ThousandsSep          ,
    "QFMT_FillBg"               , (LPUCHAR) &QFMT_FillBg                ,
    "QFMT_SubstValue"           , (LPUCHAR) &QFMT_SubstValue            ,

    // qf_fx.c
    "SysFnFX_EM_YY"             , (LPUCHAR) &SysFnFX_EM_YY              ,

    // qf_mf.c
    "SysFnMF_EM_YY"             , (LPUCHAR) &SysFnMF_EM_YY              ,
    "SysFnMonMF_EM_YY"          , (LPUCHAR) &SysFnMonMF_EM_YY           ,
    "SysFnMonMF_Numeric_EM"     , (LPUCHAR) &SysFnMonMF_Numeric_EM      ,
    "SysFnDydMF_EM_YY"          , (LPUCHAR) &SysFnDydMF_EM_YY           ,
    "ToggleMonInfo"             , (LPUCHAR) &ToggleMonInfo              ,
    "StartStopMonInfo"          , (LPUCHAR) &StartStopMonInfo           ,
    "GetPerformanceCount"       , (LPUCHAR) &GetPerformanceCount        ,

    // qf_nc.c
    "SysFnNC_EM_YY"             , (LPUCHAR) &SysFnNC_EM_YY              ,

    // qf_nfns
    "SysFnNAPPEND_EM_YY"        , (LPUCHAR) &SysFnNAPPEND_EM_YY         ,
    "SysFnMonNAPPEND_EM_YY"     , (LPUCHAR) &SysFnMonNAPPEND_EM_YY      ,
    "SysFnDydNAPPEND_EM_YY"     , (LPUCHAR) &SysFnDydNAPPEND_EM_YY      ,
    "SysFnNCREATE_EM_YY"        , (LPUCHAR) &SysFnNCREATE_EM_YY         ,
    "SysFnMonNCREATE_EM_YY"     , (LPUCHAR) &SysFnMonNCREATE_EM_YY      ,
    "SysFnDydNCREATE_EM_YY"     , (LPUCHAR) &SysFnDydNCREATE_EM_YY      ,
    "SysFnCreateTie_EM_YY"      , (LPUCHAR) &SysFnCreateTie_EM_YY       ,
    "SysFnNERASE_EM_YY"         , (LPUCHAR) &SysFnNERASE_EM_YY          ,
    "SysFnMonNERASE_EM_YY"      , (LPUCHAR) &SysFnMonNERASE_EM_YY       ,
    "SysFnDydNERASE_EM_YY"      , (LPUCHAR) &SysFnDydNERASE_EM_YY       ,
////"SysFnLock_EM_YY"           , (LPUCHAR) &SysFnLock_EM_YY            ,
////"SysFnMonLOCK_EM_YY"        , (LPUCHAR) &SysFnMonLOCK_EM_YY         ,
////"SysFnDydLOCK_EM_YY"        , (LPUCHAR) &SysFnDydLOCK_EM_YY         ,
    "SysFnNNAMES_EM_YY"         , (LPUCHAR) &SysFnNNAMES_EM_YY          ,
    "SysFnNNUMS_EM_YY"          , (LPUCHAR) &SysFnNNUMS_EM_YY           ,
    "SysFnNREAD_EM_YY"          , (LPUCHAR) &SysFnNREAD_EM_YY           ,
    "SysFnMonNREAD_EM_YY"       , (LPUCHAR) &SysFnMonNREAD_EM_YY        ,
    "SysFnDydNREAD_EM_YY"       , (LPUCHAR) &SysFnDydNREAD_EM_YY        ,
    "SysFnNRENAME_EM_YY"        , (LPUCHAR) &SysFnNRENAME_EM_YY         ,
    "SysFnMonNRENAME_EM_YY"     , (LPUCHAR) &SysFnMonNRENAME_EM_YY      ,
    "SysFnDydNRENAME_EM_YY"     , (LPUCHAR) &SysFnDydNRENAME_EM_YY      ,
    "SysFnNREPLACE_EM_YY"       , (LPUCHAR) &SysFnNREPLACE_EM_YY        ,
    "SysFnMonNREPLACE_EM_YY"    , (LPUCHAR) &SysFnMonNREPLACE_EM_YY     ,
    "SysFnDydNREPLACE_EM_YY"    , (LPUCHAR) &SysFnDydNREPLACE_EM_YY     ,
    "SysFnNRESIZE_EM_YY"        , (LPUCHAR) &SysFnNRESIZE_EM_YY         ,
    "SysFnMonNRESIZE_EM_YY"     , (LPUCHAR) &SysFnMonNRESIZE_EM_YY      ,
    "SysFnDydNRESIZE_EM_YY"     , (LPUCHAR) &SysFnDydNRESIZE_EM_YY      ,
    "SysFnNSIZE_EM_YY"          , (LPUCHAR) &SysFnNSIZE_EM_YY           ,
    "SysFnMonNSIZE_EM_YY"       , (LPUCHAR) &SysFnMonNSIZE_EM_YY        ,
    "SysFnDydNSIZE_EM_YY"       , (LPUCHAR) &SysFnDydNSIZE_EM_YY        ,
    "SysFnNTIE_EM_YY"           , (LPUCHAR) &SysFnNTIE_EM_YY            ,
    "SysFnMonNTIE_EM_YY"        , (LPUCHAR) &SysFnMonNTIE_EM_YY         ,
    "SysFnDydNTIE_EM_YY"        , (LPUCHAR) &SysFnDydNTIE_EM_YY         ,
    "SysFnNUNTIE_EM_YY"         , (LPUCHAR) &SysFnNUNTIE_EM_YY          ,
    "SysFnMonNUNTIE_EM_YY"      , (LPUCHAR) &SysFnMonNUNTIE_EM_YY       ,
    "SysFnDydNUNTIE_EM_YY"      , (LPUCHAR) &SysFnDydNUNTIE_EM_YY       ,
    "NfnsReleaseOneResource"    , (LPUCHAR) &NfnsReleaseOneResource     ,
    "NfnsReleaseResources"      , (LPUCHAR) &NfnsReleaseResources       ,
    "NfnsArgTieNum"             , (LPUCHAR) &NfnsArgTieNum              ,
    "NfnsArgAplint"             , (LPUCHAR) &NfnsArgAplint              ,
    "NfnsArgMode"               , (LPUCHAR) &NfnsArgMode                ,
    "NfnsArgConv"               , (LPUCHAR) &NfnsArgConv                ,
    "NestedCharVec"             , (LPUCHAR) &NestedCharVec              ,
    "GetNextTieNum_EM"          , (LPUCHAR) &GetNextTieNum_EM           ,
    "CmpAPLINT_DESC"            , (LPUCHAR) &CmpAPLINT_DESC             ,
    "IsDuplicateTieNum_EM"      , (LPUCHAR) &IsDuplicateTieNum_EM       ,
    "InitGlbNfns_EM"            , (LPUCHAR) &InitGlbNfns_EM             ,
    "IncrGlbNfns"               , (LPUCHAR) &IncrGlbNfns                ,
    "NfnsSaveData_EM"           , (LPUCHAR) &NfnsSaveData_EM            ,
    "SysErrMsg_EM"              , (LPUCHAR) &SysErrMsg_EM               ,
    "NfnsEraseRenameResize_EM"  , (LPUCHAR) &NfnsEraseRenameResize_EM   ,
    "NfnsErase"                 , (LPUCHAR) &NfnsErase                  ,
    "NfnsRename"                , (LPUCHAR) &NfnsRename                 ,
    "NfnsResize"                , (LPUCHAR) &NfnsResize                 ,
    "NfnsWriteData_EM"          , (LPUCHAR) &NfnsWriteData_EM           ,
    "InitLockNfns"              , (LPUCHAR) &InitLockNfns               ,

    // qf_nl.c
    "SysFnNL_EM_YY"             , (LPUCHAR) &SysFnNL_EM_YY              ,

    // qf_sysid.c
    "SysFnSYSID_EM_YY"          , (LPUCHAR) &SysFnSYSID_EM_YY           ,

    // qf_sysver.c
    "SysFnSYSVER_EM_YY"         , (LPUCHAR) &SysFnSYSVER_EM_YY          ,

    // qf_tc.c
    "SysFnTC_EM_YY"             , (LPUCHAR) &SysFnTC_EM_YY              ,

    // qf_tf.c
    "SysFnTF_EM_YY"             , (LPUCHAR) &SysFnTF_EM_YY              ,
    "SysFnMonTF_EM_YY"          , (LPUCHAR) &SysFnMonTF_EM_YY           ,
    "SysFnDydTF_EM_YY"          , (LPUCHAR) &SysFnDydTF_EM_YY           ,
    "SysFnDydTF1_EM_YY"         , (LPUCHAR) &SysFnDydTF1_EM_YY          ,
    "SysFnDydTF2_EM_YY"         , (LPUCHAR) &SysFnDydTF2_EM_YY          ,
    "TransferInverseFcn1_EM"    , (LPUCHAR) &TransferInverseFcn1_EM     ,

    // qf_ucs.c
    "SysFnUCS_EM_YY"            , (LPUCHAR) &SysFnUCS_EM_YY             ,
    "SysFnMonUCS_EM_YY"         , (LPUCHAR) &SysFnMonUCS_EM_YY          ,
    "SysFnDydUCS_EM_YY"         , (LPUCHAR) &SysFnDydUCS_EM_YY          ,

    // qf_vr.c
    "SysFnVR_EM_YY"             , (LPUCHAR) &SysFnVR_EM_YY              ,

    // qf_wa.c
    "SysFnWA_EM_YY"             , (LPUCHAR) &SysFnWA_EM_YY              ,

    // refcnt.c

#ifdef DEBUG
    // resdebug.c
    "_SaveObj"                  , (LPUCHAR) &_SaveObj                   ,
/// ...
    "_CheckMemStat"             , (LPUCHAR) &_CheckMemStat              ,

#endif
    // savefcn.c
    "SaveFunction"              , (LPUCHAR) &SaveFunction               ,
    "SaveFunctionCom"           , (LPUCHAR) &SaveFunctionCom            ,
    "SaveFunctionLine"          , (LPUCHAR) &SaveFunctionLine           ,
    "IsLineEmpty"               , (LPUCHAR) &IsLineEmpty                ,
    "GetLabelNums"              , (LPUCHAR) &GetLabelNums               ,
    "ErrLabelNums"              , (LPUCHAR) &ErrLabelNums               ,

    // sc_copy.c
    "CmdCopy_EM"                , (LPUCHAR) &CmdCopy_EM                 ,

    // sc_drop.c
    "CmdDrop_EM"                , (LPUCHAR) &CmdDrop_EM                 ,

    // sc_erase.c
    "CmdErase_EM"               , (LPUCHAR) &CmdErase_EM                ,

    // sc_fnov.c
    "CmdFns_EM"                 , (LPUCHAR) &CmdFns_EM                  ,

    // sc_in.c
    "CmdIn_EM"                  , (LPUCHAR) &CmdIn_EM                   ,
    "TransferInverseChr1_EM"    , (LPUCHAR) &TransferInverseChr1_EM     ,
    "TransferInverseNum1_EM"    , (LPUCHAR) &TransferInverseNum1_EM     ,
    "CmdInCopyAndTranslate_EM"  , (LPUCHAR) &CmdInCopyAndTranslate_EM   ,
    "CmdInEBCDIC2ASCII"         , (LPUCHAR) &CmdInEBCDIC2ASCII          ,

    // sc_lib.c
    "CmdLib_EM"                 , (LPUCHAR) &CmdLib_EM                  ,

    // sc_load.c
    "CmdLoad_EM"                , (LPUCHAR) &CmdLoad_EM                 ,
    "CmdXload_EM"               , (LPUCHAR) &CmdXload_EM                ,
    "CmdLoadCom_EM"             , (LPUCHAR) &CmdLoadCom_EM              ,
    "LoadWorkspace_EM"          , (LPUCHAR) &LoadWorkspace_EM           ,
    "DisplayWorkspaceStamp"     , (LPUCHAR) &DisplayWorkspaceStamp      ,
    "SendMessageLastTab"        , (LPUCHAR) &SendMessageLastTab         ,
    "ParseSavedWsFcn_EM"        , (LPUCHAR) &ParseSavedWsFcn_EM         ,
    "ParseSavedWsVar_EM"        , (LPUCHAR) &ParseSavedWsVar_EM         ,
    "LoadWorkspaceGlobal_EM"    , (LPUCHAR) &LoadWorkspaceGlobal_EM     ,
    "LoadWsGlbVarConv"          , (LPUCHAR) &LoadWsGlbVarConv           ,

    // sc_out.c
    "CmdOut_EM"                 , (LPUCHAR) &CmdOut_EM                  ,

    // sc_reset.c
    "CmdReset_EM"               , (LPUCHAR) &CmdReset_EM                ,

    // sc_save.c
    "CmdSave_EM"                , (LPUCHAR) &CmdSave_EM                 ,

    // sc_si.c
    "CmdSi_EM"                  , (LPUCHAR) &CmdSi_EM                   ,

    // sc_wsid.c
    "CmdWsid_EM"                , (LPUCHAR) &CmdWsid_EM                 ,

    // sessman.c
    "SetAttrs"                  , (LPUCHAR) &SetAttrs                   ,
    "SMWndProc"                 , (LPUCHAR) &SMWndProc                  ,

    // sort.c
    "ShellSort"                 , (LPUCHAR) &ShellSort                  ,

    // strand.c
    "InitVarStrand"             , (LPUCHAR) &InitVarStrand              ,
    "MakeList_EM_YY"            , (LPUCHAR) &MakeList_EM_YY             ,
    "CopyImmToken_EM"           , (LPUCHAR) &CopyImmToken_EM            ,

    // symtab.c
  #ifdef DEBUG
    "HshTabFrisk"               , (LPUCHAR) &HshTabFrisk                ,
  #endif
    "MaskTheHash"               , (LPUCHAR) &MaskTheHash                ,
    "HshTabDelink"              , (LPUCHAR) &HshTabDelink               ,
    "HshTabLink"                , (LPUCHAR) &HshTabLink                 ,
    "HshTabResize_EM"           , (LPUCHAR) &HshTabResize_EM            ,
    "SymTabResize_EM"           , (LPUCHAR) &SymTabResize_EM            ,
    "HshTabSplitNextEntry_EM"   , (LPUCHAR) &HshTabSplitNextEntry_EM    ,
    "FindNextFreeUsingHash_SPLIT_EM", (LPUCHAR) FindNextFreeUsingHash_SPLIT_EM ,
    "FindNextFreeUsingHTE_EM"   , (LPUCHAR) &FindNextFreeUsingHTE_EM    ,
    "SymTabLookupNameLength"    , (LPUCHAR) &SymTabLookupNameLength     ,
    "MakeSymEntry_EM"           , (LPUCHAR) &MakeSymEntry_EM            ,

    // symtrans.c
    "FcnTrans"                  , (LPUCHAR) &FcnTrans                   ,

    // syscmds.c
    "IncorrectCommand"          , (LPUCHAR) &IncorrectCommand           ,

    // sysvars.c
    "MakePermVars"              , (LPUCHAR) &MakePermVars               ,
    "MakePermCharVector"        , (LPUCHAR) &MakePermCharVector         ,
    "MakePermIntVector"         , (LPUCHAR) &MakePermIntVector          ,
    "MakePermVectorCom"         , (LPUCHAR) &MakePermVectorCom          ,
    "SymTabAppendOneSysName_EM" , (LPUCHAR) &SymTabAppendOneSysName_EM  ,
    "SymTabAppendAllSysNames_EM", (LPUCHAR) &SymTabAppendAllSysNames_EM ,
    "InitSystemNames_EM"        , (LPUCHAR) &InitSystemNames_EM         ,
    "AssignGlobalCWS"           , (LPUCHAR) &AssignGlobalCWS            ,
    "AssignBoolScalarCWS"       , (LPUCHAR) &AssignBoolScalarCWS        ,
    "AssignIntScalarCWS"        , (LPUCHAR) &AssignIntScalarCWS         ,
    "AssignRealScalarCWS"       , (LPUCHAR) &AssignRealScalarCWS        ,
    "AssignCharScalarCWS"       , (LPUCHAR) &AssignCharScalarCWS        ,
    "ValidateCharDT_EM"         , (LPUCHAR) &ValidateCharDT_EM          ,
    "ValidateInteger_EM"        , (LPUCHAR) &ValidateInteger_EM         ,
    "ValidateIntegerTest"       , (LPUCHAR) &ValidateIntegerTest        ,
    "ValidateFloat_EM"          , (LPUCHAR) &ValidateFloat_EM           ,
    "ValidateFloatTest"         , (LPUCHAR) &ValidateFloatTest          ,
    "ValidateCharVector_EM"     , (LPUCHAR) &ValidateCharVector_EM      ,
    "ValidateIntegerVector_EM"  , (LPUCHAR) &ValidateIntegerVector_EM   ,
    "VariantValidateCom_EM"     , (LPUCHAR) &VariantValidateCom_EM      ,
    "ValidSetALX_EM"            , (LPUCHAR) &ValidSetALX_EM             ,
    "ValidNdxChar"              , (LPUCHAR) &ValidNdxChar               ,
    "ValidSetCT_EM"             , (LPUCHAR) &ValidSetCT_EM              ,
    "ValidNdxCT"                , (LPUCHAR) &ValidNdxCT                 ,
    "ValidSetDM_EM"             , (LPUCHAR) &ValidSetDM_EM              ,
    "ValidNdxDM"                , (LPUCHAR) &ValidNdxDM                 ,
    "ValidNdxDT"                , (LPUCHAR) &ValidNdxDT                 ,
    "ValidSetDT_EM"             , (LPUCHAR) &ValidSetDT_EM              ,
    "ValidSetELX_EM"            , (LPUCHAR) &ValidSetELX_EM             ,
    "ValidSetFC_EM"             , (LPUCHAR) &ValidSetFC_EM              ,
    "ValidSetFEATURE_EM"        , (LPUCHAR) &ValidSetFEATURE_EM         ,
    "ValidNdxFEATURE"           , (LPUCHAR) &ValidNdxFEATURE            ,
    "SetCurrentFeatureCWS"      , (LPUCHAR) &SetCurrentFeatureCWS       ,
    "ValidSetFPC_EM"            , (LPUCHAR) &ValidSetFPC_EM             ,
    "ValidNdxFPC"               , (LPUCHAR) &ValidNdxFPC                ,
    "ValidSetIC_EM"             , (LPUCHAR) &ValidSetIC_EM              ,
    "ValidNdxIC"                , (LPUCHAR) &ValidNdxIC                 ,
    "ValidSetIO_EM"             , (LPUCHAR) &ValidSetIO_EM              ,
    "ValidNdxIO"                , (LPUCHAR) &ValidNdxIO                 ,
    "ValidSetLX_EM"             , (LPUCHAR) &ValidSetLX_EM              ,
    "ValidSetPP_EM"             , (LPUCHAR) &ValidSetPP_EM              ,
    "ValidNdxPP"                , (LPUCHAR) &ValidNdxPP                 ,
    "ValidSetPR_EM"             , (LPUCHAR) &ValidSetPR_EM              ,
    "ValidSetPW_EM"             , (LPUCHAR) &ValidSetPW_EM              ,
    "ValidNdxPW"                , (LPUCHAR) &ValidNdxPW                 ,
    "ValidSetRL_EM"             , (LPUCHAR) &ValidSetRL_EM              ,
    "ValidNdxRL"                , (LPUCHAR) &ValidNdxRL                 ,
    "ValidPostNone"             , (LPUCHAR) &ValidPostNone              ,
    "AssignDefaultSysVars"      , (LPUCHAR) &AssignDefaultSysVars       ,
    "_AssignDefaultSysVars"     , (LPUCHAR) &_AssignDefaultSysVars      ,
    "CopySysVars"               , (LPUCHAR) &CopySysVars                ,
    "DeleSysVars"               , (LPUCHAR) &DeleSysVars                ,
    "InitSysVars"               , (LPUCHAR) &InitSysVars                ,

    // tabctrl.c
    "EnumCallbackShowHide"      , (LPUCHAR) &EnumCallbackShowHide       ,
    "LclTabCtrlWndProc"         , (LPUCHAR) &LclTabCtrlWndProc          ,
    "CreateNewTabInThread"      , (LPUCHAR) &CreateNewTabInThread       ,

    // tokenize.c
    "Tokenize_EM"               , (LPUCHAR) &Tokenize_EM                ,
    "Untokenize"                , (LPUCHAR) &Untokenize                 ,

    // translate.c
    "TranslateTknTypeToTknTypeNamed", (LPUCHAR) &TranslateTknTypeToTknTypeNamed,

    // typemote.c
    "TypeDemote"                , (LPUCHAR) &TypeDemote                 ,
    "DemoteData"                , (LPUCHAR) &DemoteData                 ,
    "TypePromote_EM"            , (LPUCHAR) &TypePromote_EM             ,
    "TypePromoteGlb_EM"         , (LPUCHAR) &TypePromoteGlb_EM          ,
    "QueryPromote"              , (LPUCHAR) &QueryPromote               ,

    // ver.c
    "LclFileVersionStrW"        , (LPUCHAR) &LclFileVersionStrW         ,
    "AboutDlgProc"              , (LPUCHAR) &AboutDlgProc               ,
    "LclStaticWndProc"          , (LPUCHAR) &LclStaticWndProc           ,

    // yyfns.c
    "_YYAlloc"                  , (LPUCHAR) &_YYAlloc                   ,
    "YYCopy"                    , (LPUCHAR) &YYCopy                     ,
    "YYCopyFreeDst"             , (LPUCHAR) &YYCopyFreeDst              ,
    "YYFree"                    , (LPUCHAR) &YYFree                     ,
  #ifdef DEBUG
    "YYResIsEmpty"              , (LPUCHAR) YYResIsEmpty                ,
  #endif
    "YYCountFcnStr"             , (LPUCHAR) &YYCountFcnStr              ,
    "YYCountFcnGlb"             , (LPUCHAR) &YYCountFcnGlb              ,
    "YYIsFcnStrAxis"            , (LPUCHAR) &YYIsFcnStrAxis             ,
    "YYCopyFcn"                 , (LPUCHAR) YYCopyFcn                   ,
    "YYCopyGlbFcn_PTB"          , (LPUCHAR) YYCopyGlbFcn_PTB            ,
    "YYFreeArray"               , (LPUCHAR) YYFreeArray                 ,
    "YYFreeGlbFcn"              , (LPUCHAR) YYFreeGlbFcn                ,
  #if 0
    "IncrFcnTkn"                , (LPUCHAR) IncrFcnTkn                  ,
    "IncrFcnMem"                , (LPUCHAR) IncrFcnMem                  ,
  #endif
  #ifdef DEBUG
    "YYCheckInuse"              , (LPUCHAR) YYCheckInuse                ,
  #endif

    // C API routines
    "memset"                    , (LPUCHAR) &memset                     ,
////"_aulldiv"                  , (LPUCHAR) &_aulldiv                   ,
    "memmove"                   , (LPUCHAR) &memmove                    ,
    "lstrlenA"                  , (LPUCHAR) &lstrlenA                   ,
    "lstrlenW"                  , (LPUCHAR) &lstrlenW                   ,
#ifdef _WIN64
    "__chkstk"                  , (LPUCHAR) &__chkstk                   ,
#else
    "_chkstk"                   , (LPUCHAR) &_chkstk                    ,
#endif


    // trailer
    "<no name>"                 , (LPUCHAR) (HANDLE_PTR) -1             ,
  };

  #define START_ADDRESSES_LENGTH      ((sizeof (StartAddresses) / sizeof (StartAddresses[0])) - 1)
#endif


//***************************************************************************
//  End of File: startaddr.h
//***************************************************************************
