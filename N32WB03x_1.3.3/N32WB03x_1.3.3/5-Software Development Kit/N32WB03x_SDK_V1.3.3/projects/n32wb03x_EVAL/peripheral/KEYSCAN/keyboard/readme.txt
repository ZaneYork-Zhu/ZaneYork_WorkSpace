
1. Function description
    This is a sample to the hardware IRC module output NEC IR protocol.

2. Use environment
    Software development environment: KEIL MDK-ARM V5.30.0.0
    Hardware environment: Developed based on the development board N32WB031_STB

3. Instructions for use
    System Configuration;
        1. Clock source:
                    HSE=32M, HSI=64M, AHB=64M, APB1=32M, APB2=64M, ADC CLK=HSE/8, DMA CLK=64M
        2. Port configuration:
                    PB4 as the IRC tx pin.

    Instructions:
        1. Download after compiling and reset to rum the program, check the wave of PB4.
		2. We can connect the IR led board to PB4 than check the data receive from IR receiver.
		3. Or we can connect the logic analyzer to PB4 and check the wave it output. 

4. Matters needing attention

