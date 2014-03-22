@echo off
REM @php wincver.php version.rc

Set NMAKE=NMAKE
call w32

Set DEL_SW=
FOR %%G IN (%*) DO (IF /I "%1" == "del" Set DEL_SW=1)

%NMAKE% /nologo /f makefile.ind "WIN=32" "DEBUG=0" "NMAKE_SWS=%*" "DEL_SW=%DEL_SW%"
Set DEL_SW=
Set NMAKE=
