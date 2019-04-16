
#include "stm32f0xx.h"
#include "stdio.h"

#include "FreeRTOS.h"
#include "task.h"

#include "LedCombo.h"
#include "Keypad_M.h"

#include "M24C02.h"

#include "semphr.h"
#include "usart.h"
#include "app.h"



#include "CMDParser.h"
#include "TX_Framer.h"




#include "main.h"

#define Sync_Normal_Time_Interval      200
#define Sync_KeyPressed_Time_Interval   20



xTaskHandle LedHandle,KeyHandle,UartHandle;



int main(void)
{
	
	 
	
	
	 	 
	USART1_ReceivedSemaphore=xSemaphoreCreateCounting(1,0);
	 
	 
	
	
	RCC_ClocksTypeDef RCC_Clocks;

	
	RCC_GetClocksFreq(&RCC_Clocks);  									//获取系统时钟频率
	
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);	//将SysTick定时器的中断频率设置1ms，即FreeRTOS的时间片为1ms
	
	USART1_Config();

	Keypad_Init();
	
	M24C02_Init();
	
	LEDCombo_Init();
	
	
	

	
		
   

	//创建任务
	xTaskCreate(led_task, ( signed char * )"led",128, NULL, 4, &LedHandle);
		
	xTaskCreate(key_task, ( signed char * )"KeyScan", 128, NULL, 3, &KeyHandle);
		
	xTaskCreate(USART1_task, ( signed char * )"COM1", 128, NULL, 1, &UartHandle);
	
	//xTaskCreate(Event_Handle_task, ( signed char * )"MessageLoop", 36, NULL, 2, &KeyMessageHandle);
	

	
	vTaskStartScheduler();
	
	while (1)
	{

	}
	
	
}





void led_task(void *para)
{
	while (1)
	{
		//if(!IsTestMode)
					
			LEDCombo_Reflash();

					
	    vTaskDelay(10);
		
		
	}

		
}





void key_task(void *para)
{

	unsigned char temp=0;

	static int Max_Count=1000;

	static int Sync_Count=0;


	
	while (1)
	{
		temp=Keypad_ReadKey();
		
		if ((temp>0))
		{
			
			Handle_Key_Pressed(temp);
		
			Max_Count=Sync_KeyPressed_Time_Interval;
			Sync_Count=0;
	
			
		}
		
	
		
		
			if (++Sync_Count==Max_Count)
			{
				TXFramer_SendSync();
				Sync_Count=0;
				Max_Count=Sync_Normal_Time_Interval;
			}

		
		 vTaskDelay(10);
	
	}

		 
		 
			
}


void USART1_task(void *para)
{
	const uint8_t *COMPILED_DATE=__DATE__;//获得编译日期
	const uint8_t *COMPILED_TIME=__TIME__;//获得编译时间
	static int 			COM1_LED_Counter=0;
	static bool     COM1_LED_ON=false;
		
	
	
	printf("		FreeRTOS 7.6.0 for STM32F030\r\n");
	printf("		Build date: ");
	printf((char *)COMPILED_DATE);
	printf(" ");
	printf((char *)COMPILED_TIME);
	printf("\r\n");
	printf("		Device Vendor: NJ ZeHai Electrical Equipment Co.,Ltd.\r\n");
	
	


	TXFramer_SendSync();
	
	while(1)
	{
		 if(!IsTestMode)
		{
				
			if( xSemaphoreTake(USART1_ReceivedSemaphore, 10 )==pdTRUE )  //portMAX_DELAY
			{
				LEDCombo_TurnOn(LEDCombo_COM_ID);
				//LEDCombo_TurnOn(LEDCombo_Dimmer);
				COM1_LED_ON=true;
			
				CMDParser_COM1_Handler();
			
			//LEDCombo_TurnOff(LEDCombo_COM1_ID);
			
			
			}
		
			if(COM1_LED_ON)
				COM1_LED_Counter++;
				
		
			if((COM1_LED_Counter>=25)&&COM1_LED_ON)//&&LEDCombo_GetStatus(LEDCombo_COM1_ID))
			{
				LEDCombo_TurnOff(LEDCombo_COM_ID);
				COM1_LED_Counter=0;
				COM1_LED_ON=false;
			}
			
			if (!USART1_Com_Status)
			{
				LEDCombo_ConfigColor(LEDCombo_COM_ID,0);
				LEDCombo_TurnOn(LEDCombo_COM_ID);
			}
			else
			{
				LEDCombo_ConfigColor(LEDCombo_COM_ID,1);
			}
			
		}
		
		vTaskDelay(10);
	}

		
}
/*
void Event_Handle_task(void *para)
{
	
	while (1)
	{
		unsigned  long pxMessage;
	
		if( xQueueReceive( xQueue, &( pxMessage ),  10  ))
		{
			
			Handle_Message( pxMessage);

    }

			
	  	vTaskDelay(10);
		
		
	}
	
}
*/



//内存分配失败的钩子函数，configUSE_MALLOC_FAILED_HOOK定义为1时会调用
//void vApplicationMallocFailedHook( void )
//{
//	/* vApplicationMallocFailedHook() will only be called if
//	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
//	function that will get called if a call to pvPortMalloc() fails.
//	pvPortMalloc() is called internally by the kernel whenever a task, queue,
//	timer or semaphore is created.  It is also called by various parts of the
//	demo application.  If heap_1.c or heap_2.c are used, then the size of the
//	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
//	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
//	to query the size of free heap space that remains (although it does not
//	provide information on how the remaining heap might be fragmented). */
//	taskDISABLE_INTERRUPTS();
//	for( ;; );
//}
///*-----------------------------------------------------------*/

//空闲任务钩子函数，configUSE_IDLE_HOOK定义为1时需要定义这个函数
//void vApplicationIdleHook( void )
//{
//	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
//	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
//	task.  It is essential that code added to this hook function never attempts
//	to block in any way (for example, call xQueueReceive() with a block time
//	specified, or call vTaskDelay()).  If the application makes use of the
//	vTaskDelete() API function (as this demo application does) then it is also
//	important that vApplicationIdleHook() is permitted to return to its calling
//	function, because it is the responsibility of the idle task to clean up
//	memory allocated by the kernel to any task that has since been deleted. */
//}
///*-----------------------------------------------------------*/

//堆栈溢出钩子函数，configCHECK_FOR_STACK_OVERFLOW定义为1或2时会调用
void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
///*-----------------------------------------------------------*/

//时钟嘀嗒钩子函数，在SysTick定时器中断处理函数中会调用这个函数，所以此函数在调用FreeRTOS API时需要注意安全
//configUSE_TICK_HOOK被定义为1时会调用
//void vApplicationTickHook( void )
//{
//	/* This function will be called by each tick interrupt if
//	configUSE_TICK_HOOK is set to 1 in FreeRTOSConfig.h.  User code can be
//	added here, but the tick hook is called from an interrupt context, so
//	code must not attempt to block, and only the interrupt safe FreeRTOS API
//	functions can be used (those that end in FromISR()). */
//}
///*-----------------------------------------------------------*/


