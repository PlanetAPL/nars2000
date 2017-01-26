REM ************************************************************************
REM This batch file is used to customize the NARS2000 build environment
REM   to your local system.  Please change these lines as necessary to
REM   reflect how you organize your system.
REM ************************************************************************


REM Directory of the root of the NARS2000 project
Set NARSROOT=R:\NARS2000^\

REM Path to Doctor Dump SDK (formerly known as CrashServer) root
Set CS_DIR=R:\DoctorDumpSDK^\

REM Path and name of zip program (7-zip)
Set ZIPPRG="%ProgramFiles%\7-Zip\7z"

REM Path to Cygwin directory
Set CYGWIN_DIR=C:\Cygwin64^\

REM Directory of SDK ("%ProgramFiles(x86)%\Windows Kits\8.1")
Set SDK=U:^\

REM Directory of MSVC compiler ("%ProgramFiles(x86)%\Microsoft Visual Studio 12.0")
Set MSVC=V:\VC^\
Set VSINSTALLDIR=V:^\
Set VCINSTALLDIR=%MSVC%

