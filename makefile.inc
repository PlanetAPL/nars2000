# /***************************************************************************
#     NARS2000 -- An Experimental APL Interpreter
#     Copyright (C) 2006-2014 Sudley Place Software
#
#     This program is free software: you can redistribute it and/or modify
#     it under the terms of the GNU General Public License as published by
#     the Free Software Foundation, either version 3 of the License, or
#     (at your option) any later version.
#
#     This program is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#     GNU General Public License for more details.
#
#     You should have received a copy of the GNU General Public License
#     along with this program.  If not, see <http://www.gnu.org/licenses/>.
# ***************************************************************************/

# Include file common to makefile.w32 and makefile.w64

!ifndef O
!error Macro O= not defined.
!endif

!ifndef WIN
!error Macro WIN= not defined.
!endif

!ifndef DEBUG
!error Macro DEBUG= not defined.
!endif

!ifndef ZIPPRG
!error Environment variable ZIPPRG= not defined.
!endif

!ifndef MAKEDEP
!error Environment variable MAKEDEP= not defined.
!endif

!ifndef MAKEPRO
!error Environment variable MAKEPRO= not defined.
!endif

!ifndef PARGEN
!error Environment variable PARGEN= not defined.
!endif

!ifndef MSVC
!error Environment variable MSVC= not defined.
!endif

!ifndef CS_DIR
!error Environment variable CS_DIR= not defined.
!endif

!ifndef MS_DIR
!error Environment variable MS_DIR= not defined.
!endif

!ifndef MPIR_DIR
!error Environment variable MPIR_DIR= not defined.
!endif

!ifndef MPFR_DIR
!error Environment variable MPFR_DIR= not defined.
!endif

!ifndef ECM_DIR
!error Environment variable ECM_DIR= not defined.
!endif

!ifndef SDK
!error Environment variable SDK= not defined.
!endif

!ifndef NARSROOT
!error Environment variable NARSROOT= not defined.
!endif

.SUFFIXES: .DEP .DEV .PRO .Y

NAME=NARS2000                   # Directory name
ENAME=NARS2000                  # Executable name
BIN=binaries\                   # Binaries directory
DEBUG=0
A=Images\                       # Images & artwork
BC=$(BIN)COM\                   # Directory for common binaries
WS=WineHQ\                      # Wine source
OLDPATH=$(PATH)                 # Save the current path
GSL_INC=$(GSL_DIR)include\      # Directory of GSL includes

QDEBUG=$(NARSROOT)qdebug\       # Directory for QDEBUG files/libraries
L98=$(L98);$(QDEBUG)

!if "$(LST)" == "1"
LIST=/Fa$(O)$(@B).lst /FAsc
!else
LIST=
!endif

!if "$(DEBUG)" == "1"
RESDEBUG=1
###CFLAGS_DEBUG=/Z7 /D "DEBUG"
CFLAGS_DEBUG=/Zi /D "DEBUG"
BUILD_DIR=Debug
DEBUG_LIBSUFFIX=d
!else
RESDEBUG=0
CFLAGS_DEBUG=/Zi
BUILD_DIR=Release
DEBUG_LIBSUFFIX=
!endif

GSL_LIB=$(GSL_DIR)libgsl$(WIN)$(DEBUG_LIBSUFFIX)-double.lib # GSL library

MS_INC   = $(MS_DIR)
MPIR_INC = $(MPIR_DIR)include^\
MPFR_INC = $(MPFR_DIR)include^\
ECM_INC  = $(ECM_DIR)include^\

!if [if not exist $(MPIR_INC)*.h  exit 1]
!error Incorrect value for MPIR_INC=$(MPIR_INC)
!endif
!if [if not exist $(MPFR_INC)*.h  exit 1]
!error Incorrect value for MPFR_INC=$(MPFR_INC)
!endif
!if [if not exist $(ECM_INC)*.h exit 1]
!error Incorrect value for ECM_INC=$(ECM_INC)
!endif

WDEF=_WIN$(WIN)
MFEXT=w$(WIN)

I98=.\;$(SDK)INCLUDE;$(MSVC)INCLUDE;$(MS_INC);$(QDEBUG);$(MPIR_INC);$(MPFR_INC);$(ECM_INC);$(GSL_INC);$(CS_DIR)include

MV=makever

## Set the following to 1 to debug linker error 0x460 as in
##   mkn LINKDEBUG=1
LINKDEBUG=0

MPIR_LIB = $(MPIR_DIR)lib\mpir$(WIN)$(DEBUG_LIBSUFFIX).lib
MPFR_LIB = $(MPFR_DIR)lib\mpfr$(WIN)$(DEBUG_LIBSUFFIX).lib
ECM_LIB  = $(ECM_DIR)lib\ecm$(WIN)$(DEBUG_LIBSUFFIX).lib

!if "$(DEBUG)" == "1"
CS_DLL=
CS_LIB=
!else
!if "$(WIN)" == "64"
CS_DLL=$(CS_DIR)bin\x64\crshhndl.dll
CS_LIB=$(CS_DIR)lib\x64\crshhndl.lib
!else
CS_DLL=$(CS_DIR)bin\crshhndl.dll
CS_LIB=$(CS_DIR)lib\crshhndl.lib
!endif
!endif

CFLAGS_DEBUG=$(CFLAGS_DEBUG) /D "RESDEBUG=$(RESDEBUG)" /D "_STDINT_H"

## Load all source file names
!include makefile.src

VERSION_FLAGS = /D "NTDDI_VERSION=NTDDI_WINXP" /D "_WIN32_WINNT=_WIN32_WINNT_WINXP" /D "_WIN32_IE=0x0600"
CFLAGS_WEXE = /nologo /GA /W3 /WX /Oy- /Ot $(CFLAGS_DEBUG) /D $(WDEF) /c /D _CRT_SECURE_NO_WARNINGS
CFLAGS_WEXE = $(CFLAGS_WEXE) $(VERSION_FLAGS) /D "_MBCS"
LFLAGS_WEXE = /NOLOGO /MAP /LARGEADDRESSAWARE /OPT:REF /DYNAMICBASE:NO /MANIFEST:NO /SUBSYSTEM:WINDOWS /IGNOREIDL /NXCOMPAT /DEBUG
!if "$(WIN)" == "64"
CFLAGS_WEXE = $(CFLAGS_WEXE) /GS- /D_AMD64_ /D_M_AMD64 /D_WIN64 /Gd
LFLAGS_WEXE = $(LFLAGS_WEXE) /MACHINE:X64
!endif

LIBS_WEXE = gdi32.lib           \
            kernel32.lib        \
            user32.lib          \
            version.lib         \
            comdlg32.lib        \
            shell32.lib         \
            comctl32.lib        \
            wininet.lib         \
            oldnames.lib        \
            ole32.lib           \
            uuid.lib            \
            winspool.lib        \
            advapi32.lib        \
            dbghelp.lib         \
!if "$(DEBUG)" == "1"
            qdebug32.lib        \
!endif
            libcmt.lib          \
            $(CS_LIB)           \
            $(MPIR_LIB)         \
            $(MPFR_LIB)         \
            $(ECM_LIB)          \
            $(GSL_LIB)

RCFLAGS = /nologo $(VERSION_FLAGS)
!if "$(DEBUG)" == "1"
RCFLAGS = $(RCFLAGS) /D "DEBUG"
!else
RCFLAGS = $(RCFLAGS)
!endif

RUNFLAGS =
DEFFILE = main$(WIN).def
OBJS_EXT =
!if "$(DEBUG)" == "1"
RCDEFINES = /D $(WDEF) /D BUILD_BETA
!else
RCDEFINES = /D $(WDEF)
!endif

ASM=masm
AFLAGS=/Mu /DDEBUG=$(DEBUG)

!if "$(DEBUG)" == "1"
B=$(BIN)D$(WIN)\                # Define output directory for binaries
!else
B=$(BIN)ND$(WIN)\               # Define output directory for binaries
!endif
OC=RET\COM\                     # Define output directory for common intermediate files

PDBFILE = $(B)$(ENAME).pdb

FONT=$(NARSROOT)SImPL-Medium.ttf # Default APL TrueType font


ALL:    DIR                 \
        PRO                 \
        DEP                 \
        EXE                 \
        ZIP                 \
        MAKEVER

EXE:    INITIAL_EXE         \
        $(O)$(ENAME).exe    \
        $(B)$(ENAME).exe

ZIP:    $(B)$(ENAME).zip

MAKEVER:                    \
        $(OC)$(MV).exe      \
        $(BC)$(MV).exe      \
        $(ENAME).ver

DEL:

DELPRO:
        @if exist $(O)*.pro del /Q $(O)*.pro

DELDEP:
        @if exist $(O)*.dep del /Q $(O)*.dep

DIR:
        @if not exist $(O)  mkdir $(O)
        @if not exist $(OC) mkdir $(OC)
        @if not exist $(B)  mkdir $(B)
        @if not exist $(BC) mkdir $(BC)

INITIAL_DEP:
        @echo ---.dep Files---

INITIAL_PRO:
        @echo ---.pro Files---

INITIAL_EXE:
        @echo ---.c Files-----

{}.rc{$(O)}.res:
        @Set INCLUDE=$(I98);$(O)
        @if exist $@ del $@
        @$(RC) $(RCFLAGS) $(RCDEFINES) -r -fo$@ %s

{$(MS_DIR)}.c{$(O)}.obj:
        @Set INCLUDE=$(I98);$(O)
        @if exist $@ del $@
!if "$(WIN)" == "64"
        @$(CC) $(CFLAGS_WEXE) /Fo$@ /D WIN32 /D WIN64 /D __x86_64__ /D "UNICODE" $(LIST) %s
!else
        @$(CC) $(CFLAGS_WEXE) /Fo$@ /D WIN32                        /D "UNICODE" $(LIST) %s
!endif

{}.y{$(O)}.c:
        @Set INCLUDE=$(I98);$(O)
        $(PARGEN) -o$(O)$(@B).c -v -r all -l %s

{}.c{$(O)}.obj:
        @Set INCLUDE=$(I98);$(O)
        @if exist $@ del $@
        @$(CC) $(CFLAGS_WEXE) /Fo$@ $(LIST) %s

{}.cpp{$(O)}.obj:
        @Set INCLUDE=$(I98);$(O)
        @if exist $@ del $@
        @$(CC) $(CFLAGS_WEXE) /Fo$@ $(LIST) %s

{}.c{$(OC)}.obj:
        @Set INCLUDE=$(I98);$(O)
        @if exist $@ del $@
        @$(CC) $(CFLAGS_WEXE) /Fo$@ $(LIST) %s

{$(O)}.c{$(O)}.obj:
        @Set INCLUDE=$(I98);$(O)
        @if exist $@ del $@
        @$(CC) $(CFLAGS_WEXE) /Fo$@ $(LIST) %s

{$(WS)}.c{$(O)}.obj:
        @Set INCLUDE=$(WS).;$(WS)Include;$(WS)Include\Wine;$(WS)Include\msvcrt;.;$(O);$(SDK)INCLUDE
        @if exist $@ del $@
!if "$(WIN)" == "64"
        @$(CC) $(CFLAGS_WEXE) /Fo$@ /D __x86_64__ /D "UNICODE" $(LIST) %s
!else
        @$(CC) $(CFLAGS_WEXE) /Fo$@               /D "UNICODE" $(LIST) %s
!endif

####################### DEPENDENCY GENERATION #################

DEP:    DIR                     \
        INITIAL_DEP             \
        $(SRC:???=dep)          \
        $(SRC_PARSE:???=dep)    \
        $(O)main.dev            \
        $(O)ver.dev             \
        $(O)version.dev

{}.c{$(O)}.dep:
        @echo $(@F)
        @php $(MAKEDEP) -s$(O)=$$(O) %s > $@

{}.cpp{$(O)}.dep:
        @echo $(@F)
        @php $(MAKEDEP) -s$(O)=$$(O) %s > $@

{$(O)}.c{$(O)}.dep:
        @echo $(@F)
        @php $(MAKEDEP) -s$(O)=$$(O) %s > $@

{$(WS)}.c{$(O)}.dep:
        @echo $(@F)
        @php $(MAKEDEP) -s$(O)=$$(O),$(WS)=$$(WS) %s > $@

{$(MS_DIR)}.c{$(O)}.dep:
        @echo $(@F)
        @php $(MAKEDEP) -s$(O)=$$(O),$(MS_DIR)=$$(MS_DIR) %s > $@

{}.rc{$(O)}.dev:
        @echo $(@F)
        @php $(MAKEDEP) -s$(O)=$$(O) %s > $@

!include $(O)dep.txt


####################### PROTOTYPE GENERATION #################

PRO:    DIR                     \
        INITIAL_PRO             \
        $(SRC:???=pro)          \
        $(SRC_PARSE:???=pro)

{}.c{$(O)}.pro:
        @echo $(@F)
        @php $(MAKEPRO) %s $@

{}.cpp{$(O)}.pro:
        @echo $(@F)
        @php $(MAKEPRO) %s $@

{$(O)}.c{$(O)}.pro:
        @echo $(@F)
        @php $(MAKEPRO) %s $@

{$(WS)}.c{$(O)}.pro:
        @echo $(@F)
        @php $(MAKEPRO) %s $@

{$(MS_DIR)}.c{$(O)}.pro:
        @echo $(@F)
        @php $(MAKEPRO) %s $@


########################## UTILITY ROUTINES ##########################

$(O)dbgbrk.obj:         dbgbrk.asm
        @if exist $@ del $@
        @$(ML) /DIS_$(WIN) /c /W3 /nologo $(ASMOPTS) /Fo $@ %s


############################# MAIN APP ROUTINES ######################

ICONS=$(A)About.ico     \
      $(A)CC.ico        \
      $(A)Close.ico     \
      $(A)Custom.ico    \
      $(A)DB.ico        \
      $(A)FE.ico        \
      $(A)ME.ico        \
      $(A)MF.ico        \
      $(A)NARS.ico      \
      $(A)PM.ico        \
      $(A)SM.ico        \
      $(A)VE.ico        \
      $(A)WC.ico        \
      $(A)LineCont.bmp  \
      $(A)Workspace.bmp \
      $(A)Edit.bmp      \
      $(A)Objects.bmp

MANIFEST=$(ENAME).exe.manifest$(WIN)

## Cygwin-specific files
$(O)getreent.obj:       $(@B).c         $(GETREENT_DEP)

## FP-specific files
$(O)dtoa.obj:           $(@B).c         $(DTOA_DEP)

## Bison-generated files
$(O)cs_parse.c:         $(@B).y
$(O)cs_parse.obj:       $(O)$(@B).c     $(CS_PARSE_DEP)
$(O)fh_parse.c:         $(@B).y
$(O)fh_parse.obj:       $(O)$(@B).c     $(FH_PARSE_DEP)
$(O)pl_parse.c:         $(@B).y
$(O)pl_parse.obj:       $(O)$(@B).c     $(PL_PARSE_DEP)
$(O)pn_parse.c:         $(@B).y
$(O)pn_parse.obj:       $(O)$(@B).c     $(PN_PARSE_DEP)

## WineHQ-specific files
$(O)editctrl.obj:       $(WS)$(@B).c    $(EDITCTRL_DEP)     $(WS)local.h

## Msieve-specific files
$(O)mp.obj:             $(MS_DIR)$(@B).c    $(MP_DEP)           $(MS_DIR)mp.h
$(O)prime_delta.obj:    $(MS_DIR)$(@B).c    $(PRIME_DELTA_DEP)  $(MS_DIR)common.h
$(O)smallfact.obj:      $(MS_DIR)$(@B).c    $(SMALLFACT_DEP)    $(MS_DIR)common.h
$(O)squfof.obj:         $(MS_DIR)$(@B).c    $(SQUFOF_DEP)       $(MS_DIR)common.h
$(O)tinyqs.obj:         $(MS_DIR)$(@B).c    $(TINYQS_DEP)       $(MS_DIR)common.h

#ifdef MPQS_ENABLED
## Msieve-MPQS-specific files
$(O)gf2.obj:            $(MS_DIR)$(@B).c    $(GF2_DEP)          $(MS_DIR)mpqs.h
$(O)lanczos.obj:        $(MS_DIR)$(@B).c    $(LANCZOS_DEP)      $(MS_DIR)lanczos.h
$(O)lanczos_pre.obj:    $(MS_DIR)$(@B).c    $(LANCZOS_PRE_DEP)  $(MS_DIR)lanczos.h
$(O)lanczos_matmul0.obj: $(MS_DIR)$(@B).c   $(LANCZOS_MATMUL0_DEP)  $(MS_DIR)lanczos.h
$(O)lanczos_matmul1.obj: $(MS_DIR)$(@B).c   $(LANCZOS_MATMUL1_DEP)  $(MS_DIR)lanczos.h
$(O)lanczos_matmul2.obj: $(MS_DIR)$(@B).c   $(LANCZOS_MATMUL2_DEP)  $(MS_DIR)lanczos.h
$(O)lanczos_vv.obj:     $(MS_DIR)$(@B).c    $(LANCZOS_VV_DEP)   $(MS_DIR)lanczos.h
$(O)matmul_util.obj:    $(MS_DIR)$(@B).c    $(MATMUL_UTIL_DEP)  $(MS_DIR)lanczos.h
$(O)mpqs.obj:           $(MS_DIR)$(@B).c    $(MPQS_DEP)         $(MS_DIR)mpqs.h
$(O)prime_sieve.obj:    $(MS_DIR)$(@B).c    $(PRIME_SIEVE_DEP)  $(MS_DIR)common.h
##$(O)relation.obj:       $(MS_DIR)$(@B).c    $(RELATION_DEP)     $(MS_DIR)mpqs.h
$(O)sieve.obj:          $(MS_DIR)$(@B).c    $(SIEVE_DEP)        $(MS_DIR)mpqs.h
$(O)sqrt.obj:           $(MS_DIR)$(@B).c    $(SQRT_DEP)         $(M_DIRS)mpqs.h
#endif

$(MS_DIR)common.h:      $(MS_DIR)msieve.h
$(MS_DIR)mp.h:          $(MS_DIR)util.h

## NARS-specific files
$(O)afofns.obj:         $(@B).c         $(AFOFNS_DEP)
$(O)assign.obj:         $(@B).c         $(ASSIGN_DEP)
$(O)axisfns.obj:        $(@B).c         $(AXISFNS_DEP)
$(O)bjhash.obj:         $(@B).c         $(BJHASH_DEP)
$(O)common.obj:         $(@B).c         $(COMMON_DEP)
$(O)convert.obj:        $(@B).c         $(CONVERT_DEP)
$(O)crash.obj:          $(@B).c         $(CRASH_DEP)
!if "$(DEBUG)" == "0"
$(O)crashserver.obj:    $(@B).cpp       $(CRASHSERVER_DEP)
!endif
$(O)cs_proc.obj:        $(@B).c         $(CS_PROC_DEP)
$(O)customize.obj:      $(@B).c         $(CUSTOMIZE_DEP)
$(O)debug.obj:          $(@B).c         $(DEBUG_DEP)
$(O)dictionary.obj:     $(@B).c         $(DICTIONARY_DEP)
$(O)dispdbg.obj:        $(@B).c         $(DISPDBG_DEP)
$(O)display.obj:        $(@B).c         $(DISPLAY_DEP)
$(O)editfcn.obj:        $(@B).c         $(EDITFCN_DEP)
$(O)editmat.obj:        $(@B).c         $(EDITMAT_DEP)
$(O)editvec.obj:        $(@B).c         $(EDITVEC_DEP)
$(O)errmsg.obj:         $(@B).c         $(ERRMSG_DEP)
$(O)except.obj:         $(@B).c         $(EXCEPT_DEP)
$(O)execdfn.obj:        $(@B).c         $(EXECDFN_DEP)
$(O)execfns.obj:        $(@B).c         $(EXECFNS_DEP)
$(O)execmfn.obj:        $(@B).c         $(EXECMFN_DEP)
$(O)fastbool.obj:       $(@B).c         $(FASTBOOL_DEP)
$(O)fh_proc.obj:        $(@B).c         $(FH_PROC_DEP)
$(O)fmtspec.obj:        $(@B).c         $(FMTSPEC_DEP)
$(O)free.obj:           $(@B).c         $(FREE_DEP)
$(O)getfns.obj:         $(@B).c         $(GETFNS_DEP)
$(O)goto.obj:           $(@B).c         $(GOTO_DEP)
$(O)hungarian_dbl.obj:  $(@B).c         $(HUNGARIAN_DBL_DEP)
$(O)hungarian_int.obj:  $(@B).c         $(HUNGARIAN_INT_DEP)
$(O)hungarian_rat.obj:  $(@B).c         $(HUNGARIAN_RAT_DEP)
$(O)hungarian_vfp.obj:  $(@B).c         $(HUNGARIAN_VFP_DEP)
$(O)identfns.obj:       $(@B).c         $(IDENTFNS_DEP)
$(O)immexec.obj:        $(@B).c         $(IMMEXEC_DEP)
$(O)inifile.obj:        $(@B).c         $(INIFILE_DEP)
$(O)iniparser.obj:      $(@B).c         $(INIPARSER_DEP)
$(O)initdata.obj:       $(@B).c         $(INITDATA_DEP)
$(O)main.obj:           $(@B).c         $(MAIN_DEP)
$(O)malloc.obj:         $(@B).c         $(MALLOC_DEP)
$(O)mpfns.obj:          $(@B).c         $(MPFNS_DEP)
$(O)mpifns.obj:         $(@B).c         $(MPIFNS_DEP)
$(O)perfmon.obj:        $(@B).c         $(PERFMON_DEP)
$(O)pf_bar.obj:         $(@B).c         $(PF_BAR_DEP)
$(O)pf_circle.obj:      $(@B).c         $(PF_CIRCLE_DEP)
$(O)pf_colonbar.obj:    $(@B).c         $(PF_COLONBAR_DEP)
$(O)pf_comma.obj:       $(@B).c         $(PF_COMMA_DEP)
$(O)pf_cslope.obj:      $(@B).c         $(PF_CSLOPE_DEP)
$(O)pf_cstile.obj:      $(@B).c         $(PF_CSTILE_DEP)
$(O)pf_cstar.obj:       $(@B).c         $(PF_CSTAR_DEP)
$(O)pf_darrow.obj:      $(@B).c         $(PF_DARROW_DEP)
$(O)pf_dcaret.obj:      $(@B).c         $(PF_DCARET_DEP)
$(O)pf_deltastile.obj:  $(@B).c         $(PF_DELTASTILE_DEP)
$(O)pf_domino.obj:      $(@B).c         $(PF_DOMINO_DEP)
$(O)pf_dotdot.obj:      $(@B).c         $(PF_DOTDOT_DEP)
$(O)pf_dshoe.obj:       $(@B).c         $(PF_DSHOE_DEP)
$(O)pf_dstile.obj:      $(@B).c         $(PF_DSTILE_DEP)
$(O)pf_dtack.obj:       $(@B).c         $(PF_DTACK_DEP)
$(O)pf_dtackjot.obj:    $(@B).c         $(PF_DTACKJOT_DEP)
$(O)pf_epsilon.obj:     $(@B).c         $(PF_EPSILON_DEP)
$(O)pf_epsund.obj:      $(@B).c         $(PF_EPSUND_DEP)
$(O)pf_equal.obj:       $(@B).c         $(PF_EQUAL_DEP)
$(O)pf_equalund.obj:    $(@B).c         $(PF_EQUALUND_DEP)
$(O)pf_index.obj:       $(@B).c         $(PF_INDEX_DEP)
$(O)pf_iota.obj:        $(@B).c         $(PF_IOTA_DEP)
$(O)pf_iotaund.obj:     $(@B).c         $(PF_IOTAUND_DEP)
$(O)pf_lcaret.obj:      $(@B).c         $(PF_LCARET_DEP)
$(O)pf_lcaretund.obj:   $(@B).c         $(PF_LCARETUND_DEP)
$(O)pf_lshoe.obj:       $(@B).c         $(PF_LSHOE_DEP)
$(O)pf_lshoeund.obj:    $(@B).c         $(PF_LSHOEUND_DEP)
$(O)pf_ltack.obj:       $(@B).c         $(PF_LTACK_DEP)
$(O)pf_ndcaret.obj:     $(@B).c         $(PF_NDCARET_DEP)
$(O)pf_nequal.obj:      $(@B).c         $(PF_NEQUAL_DEP)
$(O)pf_nucaret.obj:     $(@B).c         $(PF_NUCARET_DEP)
$(O)pf_pi.obj:          $(@B).c         $(PF_PI_DEP)
$(O)pf_plus.obj:        $(@B).c         $(PF_PLUS_DEP)
$(O)pf_qdot.obj:        $(@B).c         $(PF_QDOT_DEP)
$(O)pf_query.obj:       $(@B).c         $(PF_QUERY_DEP)
$(O)pf_rcaret.obj:      $(@B).c         $(PF_RCARET_DEP)
$(O)pf_rcaretund.obj:   $(@B).c         $(PF_RCARETUND_DEP)
$(O)pf_rho.obj:         $(@B).c         $(PF_RHO_DEP)
$(O)pf_root.obj:        $(@B).c         $(PF_ROOT_DEP)
$(O)pf_rshoe.obj:       $(@B).c         $(PF_RSHOE_DEP)
$(O)pf_rshoeund.obj:    $(@B).c         $(PF_RSHOEUND_DEP)
$(O)pf_rtack.obj:       $(@B).c         $(PF_RTACK_DEP)
$(O)pf_section.obj:     $(@B).c         $(PF_SECTION_DEP)
$(O)pf_slash.obj:       $(@B).c         $(PF_SLASH_DEP)
$(O)pf_slope.obj:       $(@B).c         $(PF_SLOPE_DEP)
$(O)pf_squad.obj:       $(@B).c         $(PF_SQUAD_DEP)
$(O)pf_star.obj:        $(@B).c         $(PF_STAR_DEP)
$(O)pf_stile.obj:       $(@B).c         $(PF_STILE_DEP)
$(O)pf_tilde.obj:       $(@B).c         $(PF_TILDE_DEP)
$(O)pf_times.obj:       $(@B).c         $(PF_TIMES_DEP)
$(O)pf_uarrow.obj:      $(@B).c         $(PF_UARROW_DEP)
$(O)pf_ucaret.obj:      $(@B).c         $(PF_UCARET_DEP)
$(O)pf_ushoe.obj:       $(@B).c         $(PF_USHOE_DEP)
$(O)pf_ustile.obj:      $(@B).c         $(PF_USTILE_DEP)
$(O)pf_utack.obj:       $(@B).c         $(PF_UTACK_DEP)
$(O)pf_utackjot.obj:    $(@B).c         $(PF_UTACKJOT_DEP)
$(O)pl_proc.obj:        $(@B).c         $(PL_PROC_DEP)
$(O)pn_proc.obj:        $(@B).c         $(PN_PROC_DEP)
$(O)po_cmiddot.obj:     $(@B).c         $(PO_CMIDDOT_DEP)
$(O)po_dicircle.obj:    $(@B).c         $(PO_DICIRCLE_DEP)
$(O)po_didtack.obj:     $(@B).c         $(PO_DIDTACK_DEP)
$(O)po_dieresis.obj:    $(@B).c         $(PO_DIERESIS_DEP)
$(O)po_dijot.obj:       $(@B).c         $(PO_DIJOT_DEP)
$(O)po_ditilde.obj:     $(@B).c         $(PO_DITILDE_DEP)
$(O)po_dot.obj:         $(@B).c         $(PO_DOT_DEP)
$(O)po_jot.obj:         $(@B).c         $(PO_JOT_DEP)
$(O)po_jotdot.obj:      $(@B).c         $(PO_JOTDOT_DEP)
$(O)po_multiset.obj:    $(@B).c         $(PO_MULTISET_DEP)
$(O)po_slash.obj:       $(@B).c         $(PO_SLASH_DEP)
$(O)po_slope.obj:       $(@B).c         $(PO_SLOPE_DEP)
$(O)po_variant.obj:     $(@B).c         $(PO_VARIANT_DEP)
$(O)primfns.obj:        $(@B).c         $(PRIMFNS_DEP)
$(O)primspec.obj:       $(@B).c         $(PRIMSPEC_DEP)
$(O)progbar.obj:        $(@B).c         $(PROGBAR_DEP)
$(O)qf_a.obj:           $(@B).c         $(QF_A_DEP)
$(O)qf_at.obj:          $(@B).c         $(QF_AT_DEP)
$(O)qf_av.obj:          $(@B).c         $(QF_AV_DEP)
$(O)qf_cr.obj:          $(@B).c         $(QF_CR_DEP)
$(O)qf_dl.obj:          $(@B).c         $(QF_DL_DEP)
$(O)qf_dr.obj:          $(@B).c         $(QF_DR_DEP)
$(O)qf_ea.obj:          $(@B).c         $(QF_EA_DEP)
$(O)qf_ec.obj:          $(@B).c         $(QF_EC_DEP)
$(O)qf_em.obj:          $(@B).c         $(QF_EM_DEP)
$(O)qf_error.obj:       $(@B).c         $(QF_ERROR_DEP)
$(O)qf_es.obj:          $(@B).c         $(QF_ES_DEP)
$(O)qf_et.obj:          $(@B).c         $(QF_ET_DEP)
$(O)qf_ex.obj:          $(@B).c         $(QF_EX_DEP)
$(O)qf_fmt.obj:         $(@B).c         $(QF_FMT_DEP)
$(O)qf_fx.obj:          $(@B).c         $(QF_FX_DEP)
$(O)qf_lc.obj:          $(@B).c         $(QF_LC_DEP)
$(O)qf_mf.obj:          $(@B).c         $(QF_MF_DEP)
$(O)qf_nc.obj:          $(@B).c         $(QF_NC_DEP)
$(O)qf_nfns.obj:        $(@B).c         $(QF_NFNS_DEP)
$(O)qf_nl.obj:          $(@B).c         $(QF_NL_DEP)
$(O)qf_sysid.obj:       $(@B).c         $(QF_SYSID_DEP)
$(O)qf_sysver.obj:      $(@B).c         $(QF_SYSVER_DEP)
$(O)qf_tc.obj:          $(@B).c         $(QF_TC_DEP)
$(O)qf_tf.obj:          $(@B).c         $(QF_TF_DEP)
$(O)qf_ts.obj:          $(@B).c         $(QF_TS_DEP)
$(O)qf_ucs.obj:         $(@B).c         $(QF_UCS_DEP)
$(O)qf_vr.obj:          $(@B).c         $(QF_VR_DEP)
$(O)qf_wa.obj:          $(@B).c         $(QF_WA_DEP)
$(O)rebar.obj:          $(@B).c         $(REBAR_DEP)
$(O)refcnt.obj:         $(@B).c         $(REFCNT_DEP)
!if "$(RESDEBUG)" == "1"
$(O)resdebug.obj:       $(@B).c         $(RESDEBUG_DEP)
!endif
$(O)savefcn.obj:        $(@B).c         $(SAVEFCN_DEP)
$(O)sc_common.obj:      $(@B).c         $(SC_COMMON_DEP)
$(O)sc_copy.obj:        $(@B).c         $(SC_COPY_DEP)
$(O)sc_drop.obj:        $(@B).c         $(SC_DROP_DEP)
$(O)sc_erase.obj:       $(@B).c         $(SC_ERASE_DEP)
$(O)sc_fnov.obj:        $(@B).c         $(SC_FNOV_DEP)
$(O)sc_in.obj:          $(@B).c         $(SC_IN_DEP)
$(O)sc_inascii.obj:     $(@B).c         $(SC_INASCII_DEP)
$(O)sc_lib.obj:         $(@B).c         $(SC_LIB_DEP)
$(O)sc_load.obj:        $(@B).c         $(SC_LOAD_DEP)
$(O)sc_out.obj:         $(@B).c         $(SC_OUT_DEP)
$(O)sc_reset.obj:       $(@B).c         $(SC_RESET_DEP)
$(O)sc_save.obj:        $(@B).c         $(SC_SAVE_DEP)
$(O)sc_si.obj:          $(@B).c         $(SC_SI_DEP)
$(O)sc_wsid.obj:        $(@B).c         $(SC_WSID_DEP)
$(O)sessman.obj:        $(@B).c         $(SESSMAN_DEP)
$(O)sort.obj:           $(@B).c         $(SORT_DEP)
$(O)statusbar.obj:      $(@B).c         $(STATUSBAR_DEP)
$(O)strand.obj:         $(@B).c         $(STRAND_DEP)
$(O)symtab.obj:         $(@B).c         $(SYMTAB_DEP)
$(O)symtrans.obj:       $(@B).c         $(SYMTRANS_DEP)
$(O)syscmds.obj:        $(@B).c         $(SYSCMDS_DEP)
$(O)sysvars.obj:        $(@B).c         $(SYSVARS_DEP)
$(O)tabctrl.obj:        $(@B).c         $(TABCTRL_DEP)
$(O)tokenize.obj:       $(@B).c         $(TOKENIZE_DEP)
$(O)translate.obj:      $(@B).c         $(TRANSLATE_DEP)
$(O)typemote.obj:       $(@B).c         $(TYPEMOTE_DEP)
$(O)updates.obj:        $(@B).c         $(UPDATES_DEP)
$(O)ver.obj:            $(@B).c         $(VER_DEP)
$(O)webcolors.obj:      $(@B).c         $(WEBCOLORS_DEP)
$(O)yyfns.obj:          $(@B).c         $(YYFNS_DEP)

main.rc:                                $(MAIN_RCDEP) $(ICONS) $(MANIFEST)
ver.rc:                                 $(VER_RCDEP)

$(O)main.res:           $(@B).rc        $(MAIN_RCDEP)
        @Set INCLUDE=$(I98);$(O)
        @if exist $@ del $@
        @$(RC) $(RCFLAGS) $(RCDEFINES) -r -fo$@ %s

$(BC)$(ENAME).hlp:      $(ENAME).rtf
        $(HLP) -o $@ -xr %s

$(WS)local.h:           defines.h       \
                        editctrl.h      \
                        resource.h      \
                        enums.h

# Build the Win32/Win64 version
OBJS=   $(SRC:???=obj)                  \
        $(SRC_PARSE:???=obj)            \
        $(O)dbgbrk.obj                  \
        $(O)main.res

$(O)$(ENAME).exe:       $(OBJS)             \
                        $(DEFFILE)          \
                        $(MPIR_LIB)         \
                        $(MPFR_LIB)         \
                        $(ECM_LIB)          \
                        $(GSL_LIB)          \
                        $(CS_DLL)           \
                        $(CS_LIB)           \
                        makefile.inc        \
                        makefile.$(MFEXT)
        @echo >NUL @<<$(O)main.crf
$(OBJS)
/OUT:$(O)$(ENAME).exe
/MAP:$(O)$(ENAME).map
$(LIBS_WEXE) $(LIBS_EXT)
/DEF:$(DEFFILE)
/PDB:$(PDBFILE)
/NODEFAULTLIB
!if "$(LINKDEBUG)" == "1"
/VERBOSE:LIB
!endif
<<KEEP
        @echo ---.exe File----
        @Set LIB=$(L98)
        @if exist $@ del $@
!if "$(LINKDEBUG)" == "1"
        @$(LD) $(LFLAGS_WEXE) @$(O)main.crf
!else
        @$(LD) $(LFLAGS_WEXE) @$(O)main.crf >NUL
!endif
        @del $(O)$(@B).exp
        @del $(O)$(@B).lib
        @copy $(O)$(ENAME).map $(B) >NUL

$(B)$(ENAME).zip:       $(O)$(ENAME).exe LICENSE* $(FONT) $(CS_DLL)
        @echo ---.zip File----
        @copy $(O)$(ENAME).exe $(ENAME).exe >NUL
        @$(ZIPPRG) a -tzip $(B)$(ENAME).zip $(ENAME).EXE LICENSE* $(FONT) $(CS_DLL) >NUL
        @del $(ENAME).exe
        @dir /ON $(B)$(ENAME).zip|grep "$(ENAME)\."

$(B)$(ENAME).exe:       $(O)$(ENAME).exe
        @copy %s $@ >NUL
        @dir /ON $(B)$(ENAME).exe|grep "$(ENAME)\."


############################# MAKEVER EXECUTABLE #####################

$(OC)$(MV).obj:         $(@B).c

$(OC)$(MV).exe:         $(OC)$(MV).obj
        @echo >NUL @<<$(OC)$(MV).crf
$**
/OUT:$(OC)$(MV).exe
/MAP:$(OC)$(MV).map
kernel32.lib user32.lib version.lib libcmt.lib
/DEF:$(MV).def
/NODEFAULTLIB
<<KEEP
        @Set LIB=$(L98)
        @if exist $@ del $@
        @$(LD) $(LFLAGS_WEXE) @$(OC)$(MV).crf
        @del $(OC)$(@B).exp
        @del $(OC)$(@B).lib

$(BC)$(MV).exe:         $(OC)$(MV).exe
        @copy    %s     $@ >NUL

$(ENAME).ver:           $(O)$(ENAME).exe
        @$(OC)$(MV).exe $(O)$(ENAME).exe

