/*
 * FreeRTOS V202112.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the standard demo application tasks.
 * In addition to the standard demo tasks, the following tasks and tests are
 * defined and/or created within this file:
 *
 * "Fast Interrupt Test" - A high frequency periodic interrupt is generated
 * using a free running timer to demonstrate the use of the
 * configKERNEL_INTERRUPT_PRIORITY configuration constant.  The interrupt
 * service routine measures the number of processor clocks that occur between
 * each interrupt - and in so doing measures the jitter in the interrupt timing.
 * The maximum measured jitter time is latched in the ulMaxJitter variable, and
 * displayed on the LCD by the 'Check' task as described below.  The
 * fast interrupt is configured and handled in the timertest.c source file.
 *
 * "LCD" task - the LCD task is a 'gatekeeper' task.  It is the only task that
 * is permitted to access the display directly.  Other tasks wishing to write a
 * message to the LCD send the message on a queue to the LCD task instead of
 * accessing the LCD themselves.  The LCD task just blocks on the queue waiting
 * for messages - waking and displaying the messages as they arrive.
 *
 * "Check" task -  This only executes every five seconds but has the highest
 * priority so is guaranteed to get processor time.  Its main function is to
 * check that all the standard demo tasks are still operational.  Should any
 * unexpected behaviour within a demo task be discovered the 'check' task will
 * write an error to the LCD (via the LCD task).  If all the demo tasks are
 * executing with their expected behaviour then the check task writes PASS
 * along with the max jitter time to the LCD (again via the LCD task), as
 * described above.
 *
 */

/* Standard includes. */
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Library includes. */
#include "stm32f10x_it.h"
 
/* Demo app includes. */
#include "serial.h"
#include "BlockQ.h"
#include "death.h"
#include "integer.h"
#include "blocktim.h"
#include "semtest.h"
#include "PollQ.h"
#include "flash.h"
#include "comtest2.h"

/* Task priorities. */
#define mainQUEUE_POLL_PRIORITY				( tskIDLE_PRIORITY + 2 )
#define mainCHECK_TASK_PRIORITY				( tskIDLE_PRIORITY + 3 )
#define mainSEM_TEST_PRIORITY				( tskIDLE_PRIORITY + 1 )
#define mainBLOCK_Q_PRIORITY				( tskIDLE_PRIORITY + 2 )
#define mainCREATOR_TASK_PRIORITY           ( tskIDLE_PRIORITY + 3 )
#define mainFLASH_TASK_PRIORITY				( tskIDLE_PRIORITY + 1 )
#define mainCOM_TEST_PRIORITY				( tskIDLE_PRIORITY + 1 )
#define mainINTEGER_TASK_PRIORITY           ( tskIDLE_PRIORITY )

/* Constants related to the LCD. */
#define mainMAX_LINE						( 240 )
#define mainROW_INCREMENT					( 24 )
#define mainMAX_COLUMN						( 20 )
#define mainCOLUMN_START					( 319 )
#define mainCOLUMN_INCREMENT 				( 16 )

/* The maximum number of message that can be waiting for display at any one
time. */
#define mainLCD_QUEUE_SIZE					( 3 )

/* The check task uses the sprintf function so requires a little more stack. */
#define mainCHECK_TASK_STACK_SIZE			( configMINIMAL_STACK_SIZE + 50 )

/* Dimensions the buffer into which the jitter time is written. */
#define mainMAX_MSG_LEN						25

/* The time between cycles of the 'check' task. */
#define mainCHECK_DELAY						( ( TickType_t ) 5000 / portTICK_PERIOD_MS )

/* The number of nano seconds between each processor clock. */
#define mainNS_PER_CLOCK ( ( unsigned long ) ( ( 1.0 / ( double ) configCPU_CLOCK_HZ ) * 1000000000.0 ) )

/* Baud rate used by the comtest tasks. */
#define mainCOM_TEST_BAUD_RATE		( 115200 )

/* The LED used by the comtest tasks. See the comtest.c file for more
information. */
#define mainCOM_TEST_LED			( 3 )

/*-----------------------------------------------------------*/

/*
 * Configure the clocks, GPIO and other peripherals as required by the demo.
 */
static void prvSetupHardware( void );

/*
 * Retargets the C library printf function to the USART.
 */
int fputc( int ch, FILE *f );



/*-----------------------------------------------------------*/

/* The queue used to send messages to the LCD task. */
QueueHandle_t xLCDQueue;

/*test The situation when multiple tasks are being executed*/
static int task1flagrun = 0;
static int task2flagrun = 0;
static int task3flagrun = 0;
TaskHandle_t xHandleTask1;
/*-----------------------------------------*/

//void Task1_function(void * param)
//{
//	while(1)
//	{
//		task1flagrun = 1;
//		task2flagrun = 0;
//		task3flagrun = 0;
//		printf("1");
//	}
//}

/* tast4 stack full
 * generate HardFault_Handler error
 *
 */
void Task1_function(void * param)
{
	//500byte >100*4byte
	volatile char buf[500];
	int i;
	
	while(1)
	{
		task1flagrun = 1;
		task2flagrun = 0;
		task3flagrun = 0;
		printf("1");
		
		for(i = 0;i<500;i++)
		{
			buf[i] = 0;
		}
	}
}

void Task2_function(void * param)
{
	while(1)
	{
		task1flagrun = 0;
		task2flagrun = 1;
		task3flagrun = 0;
		printf("2");
	}
}

/*tast2 delete task*/
//void Task2_function(void * param)
//{
//	int i = 0;
//	while(1)
//	{
//		task1flagrun = 0;
//		task2flagrun = 1;
//		task3flagrun = 0;
//		printf("2");
//		
//		if(i++ == 100)
//		{
//			// delete task1
//			vTaskDelete(xHandleTask1);
//		}
//		if(i == 200)
//		{
//			// delete self
//			vTaskDelete(NULL);
//		}
//	}
//}

void Task3_function(void * param)
{
	while(1)
	{
		task1flagrun = 0;
		task2flagrun = 0;
		task3flagrun = 1;
		printf("3");
	}
}

void TaskGeneric_function(void * param)
{
	int val = (int)param;
	while(1)
	{
		printf("%d",val);
	}
}

StackType_t xTask3_stackbuffer[100];
StaticTask_t xTask3_TCB;
StackType_t xIdleTaskStack[100];
StaticTask_t xIdleTask_TCB;


void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, 
																		StackType_t **ppxIdleTaskStackBuffer,
																		uint32_t *pulIdleTaskStackSize )
{
	*ppxIdleTaskStackBuffer = xIdleTaskStack;
	*ppxIdleTaskTCBBuffer = &xIdleTask_TCB;
	*pulIdleTaskStackSize = 100;
}

/*-----------------------------------------------------------*/

int main( void )
{

#ifdef DEBUG
  debug();
#endif

	prvSetupHardware();

	printf("hello world !!");
	/* Start the scheduler. */
/* tast1 priority tast
 * if Raise the priority of the task1--->1==>2
 *
 * will only task1 is run*/
//	xTaskCreate(Task1_function,"Task1",100,NULL,2,&xHandleTask1);
//	xTaskCreate(Task2_function,"Task2",100,NULL,1,NULL);
//	xTaskCreateStatic(Task3_function,"Task3",100,NULL,1,xTask3_stackbuffer,&xTask3_TCB);
	
/* tast2 task delete tast
 * will in task2 delete task1
 *
 **/
//	xTaskCreate(Task1_function,"Task1",100,NULL,1,&xHandleTask1);
//	xTaskCreate(Task2_function,"Task2",100,NULL,1,NULL);
//	xTaskCreateStatic(Task3_function,"Task3",100,NULL,1,xTask3_stackbuffer,&xTask3_TCB);

/* tast3 task generic tast
 * will generic 
 *
 **/
//	xTaskCreate(TaskGeneric_function,"Task4",100,(void *)4,1,NULL);
//	xTaskCreate(TaskGeneric_function,"Task5",100,(void *)5,1,NULL);

/* tast4 stack tast
 * 
 *
 * */
	xTaskCreate(Task1_function,"Task1",100,NULL,1,&xHandleTask1);
	xTaskCreate(Task2_function,"Task2",100,NULL,1,NULL);
	xTaskCreateStatic(Task3_function,"Task3",100,NULL,1,xTask3_stackbuffer,&xTask3_TCB);
		
	
	
	
	vTaskStartScheduler();

	/* Will only get here if there was not enough heap space to create the
	idle task. */
	return 0;
}
/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* Start with the clocks in their expected state. */
	RCC_DeInit();

	/* Enable HSE (high speed external clock). */
	RCC_HSEConfig( RCC_HSE_ON );

	/* Wait till HSE is ready. */
	while( RCC_GetFlagStatus( RCC_FLAG_HSERDY ) == RESET )
	{
	}

	/* 2 wait states required on the flash. */
	*( ( unsigned long * ) 0x40022000 ) = 0x02;

	/* HCLK = SYSCLK */
	RCC_HCLKConfig( RCC_SYSCLK_Div1 );

	/* PCLK2 = HCLK */
	RCC_PCLK2Config( RCC_HCLK_Div1 );

	/* PCLK1 = HCLK/2 */
	RCC_PCLK1Config( RCC_HCLK_Div2 );

	/* PLLCLK = 8MHz * 9 = 72 MHz. */
	RCC_PLLConfig( RCC_PLLSource_HSE_Div1, RCC_PLLMul_9 );

	/* Enable PLL. */
	RCC_PLLCmd( ENABLE );

	/* Wait till PLL is ready. */
	while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
	{
	}

	/* Select PLL as system clock source. */
	RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK );

	/* Wait till PLL is used as system clock source. */
	while( RCC_GetSYSCLKSource() != 0x08 )
	{
	}

	/* Enable GPIOA, GPIOB, GPIOC, GPIOD, GPIOE and AFIO clocks */
	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC
							| RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE );

	/* SPI2 Periph clock enable */
	RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2, ENABLE );


	/* Set the Vector Table base address at 0x08000000 */
	NVIC_SetVectorTable( NVIC_VectTab_FLASH, 0x0 );

	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

	/* Configure HCLK clock as SysTick clock source. */
	SysTick_CLKSourceConfig( SysTick_CLKSource_HCLK );
	
	SerialPortInit(115200);
}
/*-----------------------------------------------------------*/



#ifdef  DEBUG
/* Keep the linker happy. */
void assert_failed( unsigned char* pcFile, unsigned long ulLine )
{
	for( ;; )
	{
	}
}
#endif
