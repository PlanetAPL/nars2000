REM ************************************************************************
REM This batch file establishes the environment vars for building NARS2000
REM   from source code.
REM ************************************************************************


REM Call local customizations
call wcustomize

REM The following environment vars should not need to change as they
REM   reflect the normal directory structure under each of the programs
REM   after they have been installed.

REM CygWin flag to suppress DOS file warning message
Set CYGWIN=nodosfilewarning

REM Path and name of parser generator (bison)
Set PARGEN=%CYGWIN_DIR%bin\bison

REM Path and name of makedep program
Set MAKEDEP=%NARSROOT%makedep.php

REM Path and name of makepro program
Set MAKEPRO=%NARSROOT%makepro.php

if not defined ORIGPATH Set ORIGPATH=%PATH%

Set MS_DIR=%NARSROOT%msieve^\
Set MPC_DIR=%NARSROOT%mpc^\
Set MPIR_DIR=%NARSROOT%mpir^\
Set MPFR_DIR=%NARSROOT%mpfr^\
Set GSL_DIR=%NARSROOT%gsl^\
Set ECM_DIR=%NARSROOT%ecm^\

if not exist .\SImPL-Medium.ttf copy /Y ..\SImPL-Medium.ttf . >NUL

if "%1" == "32" goto W32
:W64
REM Path to mspdbNNN.dll (x64 build uses the 32-bit MSPDB)
Set MSPDB=%MSVC%bin^\

REM Path to library files (x64 uses the 64-bit libraries)
Set LIB=%SDK%Lib\winv6.3\um\x64;%SDK%Lib\winv6.3\ucrt\x64;%MSVC%lib\amd64

REM Path to various executables
Set PATH=%MSVC%bin\x86_amd64;%SDK%Bin\x86;%MSPDB%;%NARSROOT%qdebug;%ORIGPATH%

goto START

:W32
REM Path to mspdbNNN.dll
Set MSPDB=%MSVC%bin^\

REM Path to library files (x86 build uses the 32-bit libraries)
Set LIB=%SDK%Lib\winv6.3\um\x86;%SDK%Lib\winv6.3\ucrt\x86;%MSVC%lib

REM Path to various executables
Set PATH=%MSVC%bin;%SDK%bin\x86;%MSPDB%;%NARSROOT%qdebug;%ORIGPATH%

:START
Set _NT_SYMBOL_PATH=srv*C:\Symbols*http://msdl.microsoft.com/download/symbols
Set SDK_INC=%SDK%include^\
Set INCLUDE=%MSVC%include;%SDK_INC%um;%SDK_INC%shared;%SDK_INC%ucrt;%SDK_INC%winrt

if /i "%2" == "start" start V:\Common7\IDE\WDExpress.exe .\NARS2000.sln /useenv
