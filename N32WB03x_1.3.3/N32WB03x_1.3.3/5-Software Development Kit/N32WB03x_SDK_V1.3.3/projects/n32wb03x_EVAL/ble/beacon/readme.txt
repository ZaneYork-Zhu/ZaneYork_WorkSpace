1. Function description

    Bluetooth BLE Beacon routine, you can scan san scan the BLE adv in beacon mode(not connect able).
    UUID: 0x10,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff".
    Major and Minor will change every 5 sencond with a randrom number. RSSI is -50 dBm.

2. Operating environment
    Software development environment: KEIL MDK-ARM V5.30.0.0
    Hardware environment: Developed based on the development board N32WB031_STB

3. Instructions

    System configuration;
        1. Clock source: HSI=64M, AHB=64M, APB1=32M, APB2=64M
        2. Port configuration:
            LED1: PB0
            LED2: PA6
            KEY1: PB1
            KEY2: PB2
        3. Log print: DEMO board PB6(TX), baud rate: 115200, 8 data bits, 1 stop bit, no parity bit, no hardware flow control

    usage method:
        1. Burn to development board after compilation
        2. Power on development board and check the log.
        3. Connect the BLE device with the name above.

4. Precautions
    Since the program will enter the sleep mode, the SWD will not be accessible. Press the RESET button to execute the program burning step within one second.
    
    
5. firmware authentication feature test step:
    1. Define ENABLE_FIRMWARE_AUTH in project preprocessor.
    2. Build and program firmware in development board A, it run as expect and printf demo string on USART1(PB6) .
    3. Read firmware from firmware in development board A (run utilities\dfu\JLink\ReadAll.bat).
    4. Flash all All.bin to development board B, it will not able to run as expect and printf the demo string.
    5. erase and program the All.bin into development board A, it will run as expect and printf demo string.