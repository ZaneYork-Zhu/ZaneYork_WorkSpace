1. Function description

    1. Bluetooth BLE device service (dis) routine, you can view device service data after connecting the device
    2. Low power timer feature example, setup a low power timer to blink the LED1 every 100ms.
    3. RTC feature example, setup RTC and RTC alarm interrupt.
    4. LPUART feature example, setup LPUART working with LSI in 9600 baud rate and loopback the data input. 
    5. GPIO and EXIT as button detection example, setuo GPIO as input to trigger EXTI interrupt.

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
        4. Bluetooth BLE name: NS_LPT

    usage method:
        1. Burn to development board after compilation
        2. Power on development board and check the log.
        3. LED 1 is blinking every 100 ms.

4. Precautions
    Since the program will enter the sleep mode, the SWD will not be accessible. Press the RESET button to execute the program burning step within one second.