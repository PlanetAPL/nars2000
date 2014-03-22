@echo off

REM Capture the output filename
Set OUTFILE=%1

REM Skip over it
shift

REM Run through all the filenames
FOR %%G IN (%*) DO (call :subroutine %%G)

Set OUTFILE=
exit /b

:subroutine
echo !if exist ($(O)%1)   >> %OUTFILE%
echo !    include $(O)%1  >> %OUTFILE%
echo !endif               >> %OUTFILE%

REM Echo a progress char
<nul (set/p foo=.)
