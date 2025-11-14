
1. Function description

    Bluetooth Secure Connection and AES routine, user can connect this demo with PIN code "123456" in LESC mode.
    The AES demo task will run ECB, CBC and CCM test, and keep repeating the CBC test every second.

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
        3. Log print: DEMO board PB1(TX), baud rate: 115200, 8 data bits, 1 stop bit, no parity bit, no hardware flow control
        4. Bluetooth BLE name: NS_LESC
        5. LED1 lights up when it is turned on, LED2 indicates the connection status, and lights up when Bluetooth is connected
        6. BLE Data: USART1 PB6(TX) and PB7(RX), baud rate: 115200, 8 data bits, 1 stop bit, no parity bit, no hardware flow control

    usage method:
        1. Burn to development board after compilation
        2. Power on development board and check the log.
        3. Connect the master device, pair and bond the device with PIN code "123456", send data via USART1 each other.
        4. change the log of AES test.

4. Precautions
    Since the program will enter the sleep mode, the SWD will not be accessible. Press the RESET button to execute the program burning step within one second.
    
    
5. Cover a slave project to using LESC pairing.   
        1. add n32wb03x_host.lib file.
        2. change symbol_g15.obj to symbol_g15_central.obj in Misc control. (change for all target)
        3. remove lib_att.c.
        4. set define as below:
                #define BOND_STORE_ENABLE           1
                #define SEC_PARAM_LESC              1
                #define SEC_PARAM_IO_CAPABILITIES   GAP_IO_CAP_DISPLAY_ONLY
                #define SEC_PARAM_IKEY              GAP_KDIST_IDKEY
        5. add define as below: 
                #define CFG_ECCH        (1)
                #define BLE_APP_SEC_CON (1)
        6. save LTK at gapc_bond_ind_handler -> GAPC_LTK_EXCH. (1.3.2 alread changed)
        7. change NO_AUTH change to SEC_CON at app_rdtss_add_rdts.
        8. input PINGCODE "123456" when pairing.

  
6. firmware authentication feature test step:
    1. Define ENABLE_FIRMWARE_AUTH in project preprocessor.
    2. Build and program firmware in development board A, it run as expect and printf demo string on PB1 .
    3. Read firmware from firmware in development board A (run utilities\dfu\JLink\ReadAll.bat).
    4. Flash all All.bin to development board B, it will not able to run as expect and printf the demo string.
    5. erase and program the All.bin into development board A, it will run as expect and printf demo string.
    