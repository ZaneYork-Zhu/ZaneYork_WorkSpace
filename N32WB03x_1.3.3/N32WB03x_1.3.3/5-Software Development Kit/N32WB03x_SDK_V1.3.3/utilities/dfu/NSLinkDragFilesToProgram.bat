echo off
if exist "%1" (
	echo.
	echo Please make sure connect SWD and reset pin of chip to NSLink.
	echo This script will reset the chip and start programming...
	echo.

	.\NSpyocd\NSpyocd.exe erase --chip -M pre-reset -t n32wb031
	.\NSpyocd\NSpyocd.exe load -t n32wb031 %1%
)else (
	echo.
	echo Error! Do not double-click to run this bat!!!
	echo.  
)
PAUSE
EXIT
