echo off

set filename=NSLinkReadChip.bin

echo.
echo Please make sure connect SWD and reset pin of chip to NSLink.
echo This script will reset the chip and start reading...
echo.

.\NSpyocd\NSpyocd.exe  cmd -t n32wb031 -M pre-reset -c savemem 0x1000000 0x40000 %filename%

echo Please check the readback file %filename% if read success.

PAUSE
EXIT
