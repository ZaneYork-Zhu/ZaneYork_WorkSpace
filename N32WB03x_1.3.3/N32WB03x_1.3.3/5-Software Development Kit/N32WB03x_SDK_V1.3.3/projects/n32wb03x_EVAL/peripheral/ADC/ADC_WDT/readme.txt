
1. Function description
    1. ADC analog watchdog feature work at channel 1 which is PB10.
    2. Enable the ADC continuous mode, analog watchdog and interrupt feature.
    3. The analog watchdog flag will set and active ADC interrupt when the ADC value highter than high threshold value, 
        or lower than the low threshold value.

2. Use environment
    Software development environment: KEIL MDK-ARM V5.30.0.0
    Hardware environment: Developed based on the development board N32WB031_STB

3. Instructions for use
    System Configuration;
        1. Clock source:
                    HSE=32M, HSI=64M, AHB=64M, APB1=32M, APB2=64M, ADC CLK=HSE/8
        2. Port configuration:
                    PB10 is selected as analog function, ADC conversion channel 1 (low voltage channel)
        3. ADC:
                    ADC configuration: continuous conversion, continuous mode, 10-bit data, enable analog watchdog and interrupt mode
                    conversion channel 1 is the analog voltage data of PB10,
                   
        4. Log print: DEMO board PB6(TX), baud rate: 115200, 8 data bits, 1 stop bit, no parity bit, no hardware flow control
    Instructions:
        1. Download after compiling and reset to rum the program, check conversion result at log.
        2. By changing the voltage of the PB10 pin, when the voltage over the threshold, it set the analog watchdog state bit(AWD_F) and active ADC interrupt.

4. Matters needing attention
