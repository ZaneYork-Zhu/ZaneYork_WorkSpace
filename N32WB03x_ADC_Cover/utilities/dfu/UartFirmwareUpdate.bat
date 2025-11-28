echo off

:: APP.bin is the update firmware
:: --app_start_address is the address of update firmware
:: --app_version is the version of update firmware
:: --serial_port is the serial port number                
set app_bin=.\Image\APP1.bin
set app_start_address=0x1004000
set app_version=0x01020304
set serial_baudrat=1000000
set force_update=True
set serial_port=COM4


echo.
echo Please connect serial chip RX to PA4 and serial chip TX to PA5.
echo Please note that this tool is not compatible with the COM port of NSLink.
echo Please test it with other serial chips (such as CH340).
echo Please confirm the %serial_port% is the right serial port you connected.
echo Please reset the chip to enter bootloader mode.
echo Waiting to respond from bootloader mode of the chip...
.\NSUtil\NSUtil.exe ius serial %app_bin%                                       ^
                               --app_start_address %app_start_address%         ^
							   --app_version %app_version%                     ^
							   --serial_port %serial_port%                     ^
							   --serial_baudrate %serial_baudrat%              ^
							   --force_update %force_update%              


							 
PAUSE
EXIT							 