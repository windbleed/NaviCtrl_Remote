#include "LampArray.h"
#include "stm32f0xx.h"
#include "LedArray.h"
#include "FreeRTOS.h"
#include "task.h"


//NSS3(PB9) : latch / SCLK0(PF0) : shift_clk / OE2(PA1): Enable / SDI0(PB8):  Serial_Data_In

#define LampArray_NSS3     GPIO_Pin_9    //PB9
#define LampArray_SCLK0    GPIO_Pin_0    //PF0
#define LampArray_OE2      GPIO_Pin_1    //PA1
#define LampArray_SDI0     GPIO_Pin_8    //PB8


unsigned char  LampArray_Frame[LampArray_Buf_Num]                 //set 1 is turn on  the lamp , set 0 is turn off the lamp
={0x00,0x00,0x00,0x00};                                 //Lamp 1/2/3/4/5 is in buf[0] 
														//bit7/6/5 no use 
														//bit4:Lamp5/bit3: Lamp4/bit2: Lamp3/bit1: Lamp2/bit0: Lamp1/
											
void LampArray_Clr_Data()
{
	GPIO_SetBits(GPIOA, LampArray_OE2);
}


void LampArray_Shift_Byte(unsigned char Data)
{
	unsigned char SendData;
	
	//GPIO_ResetBits(GPIOB, OE1);
	
	SendData=Data;
	
	
	for(int i=0;i<8;i++)
	{
	  
		if((SendData&0x80)!=0)
			GPIO_SetBits(GPIOB,LampArray_SDI0 );
		else  
			GPIO_ResetBits(GPIOB,LampArray_SDI0);
	
		GPIO_ResetBits(GPIOF,LampArray_SCLK0);
		__NOP();
		
	
		GPIO_SetBits(GPIOF,LampArray_SCLK0);
		__NOP();
	
		
		SendData=SendData<<1;
	
	}
		

	
}


void LampArray_Shift_Data()
{
	 
	
	for(int count=LampArray_Buf_Num-1;count>=0;count--)
	{
		LampArray_Shift_Byte(LampArray_Frame[count]);
	}
	
	
}
		
		
void LampArray_Latch_Data()
{
	GPIO_ResetBits(GPIOB,LampArray_NSS3);
	__NOP();
	

	GPIO_SetBits(GPIOB,LampArray_NSS3);
	__NOP();

		
	GPIO_ResetBits(GPIOA, LampArray_OE2);
	
	

}



void LampArray_Reflash(void)
{
		
		//vTaskDelay(100);
		
		//Enter Critical Section
		{
			if( xSemaphoreTake(xSemaphore_CLK,portMAX_DELAY )==pdTRUE )  
			{
				
				LampArray_Shift_Data();
				LampArray_Latch_Data();				
			}
			
			//vTaskDelayUntil( &xLastWakeTime,LedArray_LightLevel);//*xFrequency);
			//vTaskDelay(20);
			//LampArray_Clr_Data();		
			xSemaphoreGive(xSemaphore_CLK);
		}     
		//Exit Critical Section		
		
		
		
}

void LampArray_SetFrame(unsigned char * NewFrame)
{
	for(int i=0;i<LampArray_Buf_Num;i++)
	{
		LampArray_Frame[i]=NewFrame[i];
	}
}


extern  void LampArray_Init(void)
{
		//NSS3(PB9) : latch / SCLK0(PF0) : shift_clk / OE2(PA1): Enable / SDI0(PB8):  Serial_Data_In
	
	// Set SDI0&NSS3
	GPIO_InitTypeDef GPIO_InitStruchture;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	
	GPIO_InitStruchture.GPIO_Pin = LampArray_SDI0 | LampArray_NSS3;
	GPIO_InitStruchture.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruchture.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruchture.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruchture.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOB, &GPIO_InitStruchture);
	
	GPIO_SetBits(GPIOB, LampArray_SDI0 | LampArray_NSS3);
	
	//Set OE2
		
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	
	GPIO_InitStruchture.GPIO_Pin = LampArray_OE2;
	GPIO_InitStruchture.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruchture.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruchture.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruchture.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStruchture);
	
	GPIO_SetBits(GPIOA, LampArray_OE2);
	//Set SCLK0
		
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
	
	GPIO_InitStruchture.GPIO_Pin = LampArray_SCLK0;
	GPIO_InitStruchture.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruchture.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruchture.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruchture.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOF, &GPIO_InitStruchture);
	
	GPIO_SetBits(GPIOF, LampArray_SCLK0);
	
	
}






