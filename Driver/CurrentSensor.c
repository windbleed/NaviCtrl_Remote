#include "CurrentSensor.h"
#include "stm32f0xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "LedArray.h"



#define CurrentSensor_SCLK    GPIO_Pin_0    //PF0
#define CurrentSensor_NSS4   GPIO_Pin_13   //PC13
#define CurrentSensor_KeyS6  GPIO_Pin_6    //PB6


unsigned char CurrentSensor_Frame[CurrentSensor_Buf_Num];

unsigned char * CurrentSensor_GetData(void)
{
	 CurrentSensor_ScanKey();
	
	return CurrentSensor_Frame;
	
}


void CurrentSensor_Init()
{
	GPIO_InitTypeDef GPIO_InitStruchture;
	
	
	//SCK: PF0 /NSS4: PC13/ KeyS6 : PB6
	
	//SCK
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
	
	GPIO_InitStruchture.GPIO_Pin = CurrentSensor_SCLK;
	GPIO_InitStruchture.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruchture.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruchture.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruchture.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOF, &GPIO_InitStruchture);
	
	GPIO_SetBits(GPIOF,CurrentSensor_SCLK);
	
	//NSS
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	
	GPIO_InitStruchture.GPIO_Pin = CurrentSensor_NSS4;
	GPIO_InitStruchture.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruchture.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruchture.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruchture.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOC, &GPIO_InitStruchture);
	
	GPIO_SetBits(GPIOC,CurrentSensor_NSS4);
	
	//DS

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);//设置GPIO 时钟
    GPIO_InitStruchture.GPIO_Pin = CurrentSensor_KeyS6 ;
    GPIO_InitStruchture.GPIO_Mode = GPIO_Mode_IN;		//设置管脚为输入模式
    GPIO_InitStruchture.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruchture.GPIO_PuPd = GPIO_PuPd_UP;		//设置为上拉输入
    GPIO_Init(GPIOB, &GPIO_InitStruchture);
	
	GPIO_SetBits(GPIOB, CurrentSensor_KeyS6);
	
	
}



void CurrentSensor_LoadData()
{
	GPIO_SetBits(GPIOF,CurrentSensor_SCLK);
	GPIO_ResetBits(GPIOC, CurrentSensor_NSS4);
	__NOP();
	__NOP();
	
}

void CurrentSensor_ShiftData()
{
	unsigned char ShiftMask=0x01;
	
	GPIO_SetBits(GPIOC, CurrentSensor_NSS4);

	__NOP();
	
	if(GPIO_ReadInputDataBit(GPIOB,CurrentSensor_KeyS6)!=0)
		CurrentSensor_Frame[0]|=ShiftMask;
	else CurrentSensor_Frame[0]&=~ShiftMask;
	

	for(int count=1;count<(CurrentSensor_Num+4*5);count++)  //4 key null each io board(5),
	{
		
		GPIO_ResetBits(GPIOF,CurrentSensor_SCLK);
		__NOP();
		GPIO_SetBits(GPIOF,CurrentSensor_SCLK);
		
		ShiftMask=((ShiftMask==0x80)?0x01:ShiftMask<<1);
		
		if(GPIO_ReadInputDataBit(GPIOB,CurrentSensor_KeyS6)!=0)
			CurrentSensor_Frame[count/8]|=ShiftMask;
		else CurrentSensor_Frame[count/8]&=~ShiftMask;
			 
		
		
		
	}
	
	
	
}






void   CurrentSensor_ScanKey()              //scan is only scan input and store in Keypad_Array also scan  auto key input 
{
	//Enter Critical Area	
	if( xSemaphoreTake(xSemaphore_CLK,portMAX_DELAY )==pdTRUE )  
	{
		CurrentSensor_LoadData();
		CurrentSensor_ShiftData();
		xSemaphoreGive(xSemaphore_CLK);
	}
	//Exit Critical Area
	
	
	
}






