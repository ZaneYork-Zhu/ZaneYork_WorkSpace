echo off

set JLink_path=.\JLink\JLink_V632\JLink.exe
set JLink_script_path=.\JLink\JLink_Script\loadDragFile.jlink

if exist "%1" (
echo start erase and program. 
echo r > %JLink_script_path%
echo erase >> %JLink_script_path%
echo loadfile %1 0x01000000 >> %JLink_script_path%
echo r >> %JLink_script_path%
echo g >> %JLink_script_path%
echo q >> %JLink_script_path%
%JLink_path% -Device N32WB031 -If SWD -Speed 4000 -commanderscript %JLink_script_path%
del %JLink_script_path% /f
) else (
echo. 
echo Error! Do not double-click to run this bat!!!
echo.  )
PAUSE
EXIT
