#include "Keypad.h"

#include "stm32f0xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "LedArray.h"



#define  Key_SCLK    GPIO_Pin_0   //GPIOF
#define  Key_NSS     GPIO_Pin_14   //GPIOC

#define  Key_DS      GPIO_Pin_7   //GPIOB




static unsigned char Keypad_KeyVal=0;

static bool  Keypad_IsValid=false;

static unsigned char Keypad_Array[Keypad_ArrayNum]= {0xff,0xff,0xff,0xff};

static unsigned char Keypad_ReadKey_Status=0x00; //0x00 is start to read / 0x01 is wait for key up /02 is finish key reading


static unsigned char Keypad_Logic[Keypad_Num]=

	{AUTO,DIMMER,INCHING,TEST,MUTE,KEYFLASH,KEY20,KEY19,KEY18,KEY17,KEY16,KEY15,KEY14,KEY13,KEY12,KEY11,
	 MORSE,KEY10,KEY9,KEY8,KEY7,KEY6,KEY5,KEY4,KEY3,KEY2,KEY1};
	 
unsigned char  Keypad_Morse[Keypad_Num]=
	{
		false,false,false,false,false,false,false,false,false,false,true,
		false,false,false,false,false,false,false,false,false,false,false,
		true,false,true,false,false};
		




void Keypad_GPIOConfig()
{
	GPIO_InitTypeDef GPIO_InitStruchture;
	
	
	//Key_SCK: PF0 /Key_NSS: PC14/ Key_DS : PB7
	
	//SCK
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOF, ENABLE);
	
	GPIO_InitStruchture.GPIO_Pin = Key_SCLK;
	GPIO_InitStruchture.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruchture.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruchture.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruchture.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOF, &GPIO_InitStruchture);
	
	GPIO_SetBits(GPIOF,Key_SCLK);
	
	//NSS
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	
	GPIO_InitStruchture.GPIO_Pin = Key_NSS;
	GPIO_InitStruchture.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruchture.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruchture.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruchture.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOC, &GPIO_InitStruchture);
	
	GPIO_SetBits(GPIOC,Key_NSS);
	
	//DS

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);//设置GPIO 时钟
    GPIO_InitStruchture.GPIO_Pin = Key_DS ;
    GPIO_InitStruchture.GPIO_Mode = GPIO_Mode_IN;		//设置管脚为输入模式
    GPIO_InitStruchture.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruchture.GPIO_PuPd = GPIO_PuPd_UP;		//设置为上拉输入
    GPIO_Init(GPIOB, &GPIO_InitStruchture);
	
	GPIO_SetBits(GPIOB, Key_DS);
	
	
}

void Keypad_Debounce()
{
	vTaskDelay(10);
}

void Keypad_LoadData()
{
	GPIO_SetBits(GPIOF,Key_SCLK);
	GPIO_ResetBits(GPIOC, Key_NSS);
	__NOP();
	__NOP();
	
}



void Keypad_ShiftData()
{
	unsigned char ShiftMask=0x01;
	
	GPIO_SetBits(GPIOC, Key_NSS);

	__NOP();
	
	if(GPIO_ReadInputDataBit(GPIOB,Key_DS)!=0)
		Keypad_Array[0]|=ShiftMask;
	else Keypad_Array[0]&=~ShiftMask;
	

	for(int count=1;count<Keypad_Num+5;count++)  //5 key null at first,
	{
		
		GPIO_ResetBits(GPIOF,Key_SCLK);
		__NOP();
		GPIO_SetBits(GPIOF,Key_SCLK);
		
		ShiftMask=((ShiftMask==0x80)?0x01:ShiftMask<<1);
		
		if(GPIO_ReadInputDataBit(GPIOB,Key_DS)!=0)
			Keypad_Array[count/8]|=ShiftMask;
		else Keypad_Array[count/8]&=~ShiftMask;
			 
		
		
		
	}
	
	
	
	
}

unsigned char  Keypad_ScanKey()              //scan is only scan input and store in Keypad_Array also scan  auto key input 
{
	//Enter Critical Area	
	if( xSemaphoreTake(xSemaphore_CLK,portMAX_DELAY )==pdTRUE )  
	{
		Keypad_LoadData();
		Keypad_ShiftData();
		xSemaphoreGive(xSemaphore_CLK);
	}
	//Exit Critical Area
	return Keypad_ConvertKeyVal();
	
	
}

unsigned char  Keypad_ConvertKeyVal()                
{
	int KeyPoint=0;
	int Num_KeyPressed=0;
	unsigned char ByteMask=0x01;
	int j=0;
	
	
	//check if only one bit was 0 in array, if not wrong ,keyval =0, if OK, keyval= position  of this bit.

	
	
	for( int i=0;i<Keypad_ArrayNum;i++)
	{	

		if (i==0) 
			{j=5;ByteMask<<=5;}                  //omit the first 5 counts '0' input
		else 
			{j=0;}
			
	
		for(;j<8;j++)
		{
		
		
			if ((Keypad_Array[i]&ByteMask)==0) 
			{
				KeyPoint=8*i+j;
				Num_KeyPressed++;
			}	
			
			ByteMask=((ByteMask==0x80)?0x01:ByteMask<<1);
			
			
		}
	
	
	}
		
	if ( Num_KeyPressed==1)         //one key pressed
	{			
			Keypad_KeyVal=KeyPoint-5;
			return  Keypad_Logic[Keypad_KeyVal];
			
	}
	else if( Num_KeyPressed>1)    //more key pressed
		return 255;
	else 
		return 0;                 //no key pressed 
	

}




unsigned char  Keypad_ReadKey()     //readkey return the pressed keyvalue  
{
	static unsigned  char  KeyNum;
	static unsigned  char  KeyNum_ForUp;
	static unsigned  char   Old_Key;
	
	
	
	switch(Keypad_ReadKey_Status)
	{
		case 0x00:
		{		
			Keypad_IsValid=false;
			KeyNum=Keypad_ScanKey();
				
			if((KeyNum!=0)&&(KeyNum!=255))
			{
				
				if (Keypad_Morse[KeyNum-1])
				{
					Keypad_ReadKey_Status=0x03;
				}
				else 
				{
					Keypad_ReadKey_Status=0x01;
					vTaskDelay(100);
				}
					
			}
			else
			{
				Keypad_ReadKey_Status=0x00;
			}
			
	
				
			break;
		}	
		case 0x01:
		{
			Keypad_IsValid=false;
					
			KeyNum_ForUp=Keypad_ScanKey();
				
			
			if(KeyNum_ForUp==0)
				Keypad_ReadKey_Status=0x02;
			
			else Keypad_ReadKey_Status=0x01;
			

			
			break;
			
			
		}
		
		case 0x02:
		{
			Keypad_IsValid=true;
			Old_Key=KeyNum;
			Keypad_ReadKey_Status=0x00;
			
	
			
	    break;
		}
		
		
		case 0x03:
		{
			Keypad_IsValid=true;
			Old_Key=KeyNum;
			Keypad_ReadKey_Status=0x04;
			
	        
			
	    break;	
		}
		case 0x04:
		{
			KeyNum=Keypad_ScanKey();
			if (KeyNum!=Old_Key)
				Keypad_ReadKey_Status=0x00;
			else
				Keypad_ReadKey_Status=0x03;
			
			break;
		}
				
				
	}
	
	
	if (Keypad_IsValid) return Old_Key;
		else return 0;
	
	
	

	
}

	
extern void Keypad_Init(void)
{
	 Keypad_GPIOConfig();	
}


