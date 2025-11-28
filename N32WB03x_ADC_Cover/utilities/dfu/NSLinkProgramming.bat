
echo off


set NSUtil_path=.\NSUtil\NSUtil.exe

set output_file=Temp

set masterboot_bin=Image\MasterBoot
set bootsetting_bin=Image\bootsetting

set public_key_file=.\keys\public_key.bin
set output_bootsetting=.\Image\bootsetting.bin

set app1_bin=Image\APP1
set bank1_bin=.\Image\APP1.bin
set bank1_start_address=0x1004000
set bank1_version=0x00000001
set bank1_activation=yes

::set app2_bin=Image\APP2
::set bank2_bin=.\Image\APP2.bin
::set bank2_start_address=0x1020000
::set bank2_version=0x00000001
::set bank2_activation=no

::set image_update_bin=Image\ImageUpdate

echo.
echo Please make sure connect SWD and reset pin of chip to NSLink.
echo This script will reset the chip and start programming...
echo.

if exist %output_bootsetting% (del %output_bootsetting%)

if defined public_key_file (

	if not defined app2_bin (
	
		%NSUtil_path% bootsetting generate %output_bootsetting%                                        ^
												--public_key_file %public_key_file%                           ^
												--bank1_start_address %bank1_start_address%                   ^
												--bank1_version %bank1_version%                               ^
												--bank1_bin %bank1_bin%                                       ^
												--bank1_activation %bank1_activation%
	)
	if defined app2_bin (
	
		%NSUtil_path% bootsetting generate %output_bootsetting%                                         ^
												--public_key_file %public_key_file%                           ^
												--bank1_start_address %bank1_start_address%                   ^
												--bank1_version %bank1_version%                               ^
												--bank1_bin %bank1_bin%                                       ^
												--bank1_activation %bank1_activation%                         ^
												--bank2_start_address %bank2_start_address%                   ^
												--bank2_version %bank2_version%                               ^
												--bank2_bin %bank2_bin%                                       ^
												--bank2_activation %bank2_activation%
	)
)


if defined bootsetting_bin (

	if defined app1_bin if defined app2_bin if defined image_update_bin  (
	
        %NSUtil_path% pack mergebin %output_file%                    ^
								 --masterboot %masterboot_bin%       ^
								 --bootsetting %bootsetting_bin%     ^
								 --app1 %app1_bin%                   ^
								 --app2 %app2_bin%                   ^
								 --imageupdate %image_update_bin%                
										 
										 
	)

	if defined app1_bin if defined app2_bin if not defined image_update_bin (
	
        %NSUtil_path% pack mergebin %output_file%                    ^
								 --masterboot %masterboot_bin%       ^
								 --bootsetting %bootsetting_bin%     ^
								 --app1 %app1_bin%                   ^
								 --app2 %app2_bin%              
										 
										 
	)
	
	
	if defined app1_bin if not defined app2_bin if not defined image_update_bin (
	
        %NSUtil_path% pack mergebin %output_file%                    ^
								 --masterboot %masterboot_bin%       ^
								 --bootsetting %bootsetting_bin%     ^
								 --app1 %app1_bin%             
										 
	)
)

.\NSpyocd\NSpyocd.exe erase --chip -M pre-reset -t n32wb031
.\NSpyocd\NSpyocd.exe load -t n32wb031 %output_file%.hex
echo Program Finish!!!
del %output_file%.hex /f
del %output_file%.bin /f 
PAUSE
EXIT