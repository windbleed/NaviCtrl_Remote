#include "Keypad_M.h"
#include "stm32f0xx.h"
#include "FreeRTOS.h"
#include "task.h"



#define  Key_1    		GPIO_Pin_8   	//GPIOA
#define  Key_2     		GPIO_Pin_12   //GPIOA
#define  Key_3      	GPIO_Pin_11   //GPIOA
#define  Key_4    		GPIO_Pin_15   //GPIOA
#define  Key_5     		GPIO_Pin_3   	//GPIOB
#define  Key_6      	GPIO_Pin_4   	//GPIOB
#define  Key_7      	GPIO_Pin_5   	//GPIOB

#define Port_Key_1_4 	GPIOA
#define Port_Key_5_7 	GPIOB

#define RCC_Key_1_4		RCC_AHBPeriph_GPIOA
#define RCC_Key_5_7		RCC_AHBPeriph_GPIOB




static bool  Keypad_IsValid=false;

static unsigned short Keypad_Logic[Keypad_Num]={Key_1,Key_2,Key_3,Key_4,Key_5,Key_6,Key_7};

static unsigned char Keypad_ReadKey_Status=0x00; //0x00 is start to read / 0x01 is wait for key up /02 is finish key reading



		




void Keypad_GPIOConfig()
{
	GPIO_InitTypeDef GPIO_InitStruchture;
	
	
		
	
	//Key1~4

    RCC_AHBPeriphClockCmd(RCC_Key_1_4, ENABLE);			//设置GPIO 时钟
    GPIO_InitStruchture.GPIO_Pin = Key_1|Key_2|Key_3|Key_4;
    GPIO_InitStruchture.GPIO_Mode = GPIO_Mode_IN;		//设置管脚为输入模式
    GPIO_InitStruchture.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruchture.GPIO_PuPd = GPIO_PuPd_UP;		//设置为上拉输入
    GPIO_Init(Port_Key_1_4, &GPIO_InitStruchture);
	
		GPIO_SetBits(Port_Key_1_4, Key_1);
		GPIO_SetBits(Port_Key_1_4, Key_2);
		GPIO_SetBits(Port_Key_1_4, Key_3);
		GPIO_SetBits(Port_Key_1_4, Key_4);
	
	//Key5~7
		RCC_AHBPeriphClockCmd(RCC_Key_5_7, ENABLE);			//设置GPIO 时钟
    GPIO_InitStruchture.GPIO_Pin = Key_5|Key_6|Key_7;
    GPIO_InitStruchture.GPIO_Mode = GPIO_Mode_IN;		//设置管脚为输入模式
    GPIO_InitStruchture.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruchture.GPIO_PuPd = GPIO_PuPd_UP;		//设置为上拉输入
    GPIO_Init(Port_Key_5_7, &GPIO_InitStruchture);
	
		GPIO_SetBits(Port_Key_5_7, Key_5);
		GPIO_SetBits(Port_Key_5_7, Key_6);
		GPIO_SetBits(Port_Key_5_7, Key_7);
	
	
}

void Keypad_Debounce()
{
		vTaskDelay(10);
}






unsigned char  Keypad_ScanKey()              //scan is only scan input and store in Keypad_Array also scan  auto key input 
{

	for(int i=0;i<4;i++)
	{
		if(GPIO_ReadInputDataBit(Port_Key_1_4,Keypad_Logic[i])==0)
			return i+1;
	}

	for(int i=4;i<7;i++)
	{
		if(GPIO_ReadInputDataBit(Port_Key_5_7,Keypad_Logic[i])==0)
			return i+1;
	}
	
	return 0;
	
}




unsigned char  Keypad_ReadKey()     //readkey return the pressed keyvalue  if morse  pressed  only return once until key released while return release virtual value;
{
	static unsigned  char  KeyNum;
	static unsigned  char  KeyNum_ForUp;
	static unsigned  char  Old_Key;
	
	
	
	switch(Keypad_ReadKey_Status)
	{
		case 0x00:
		{		
			Keypad_IsValid=false;
			KeyNum=Keypad_ScanKey();
				
			if((KeyNum!=0)&&(KeyNum!=255))
			{				
				if ((KeyNum==KEYDianDong)||(KeyNum==KEYTest)||(KeyNum==KEYDIM))  //morse key goto status 3
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
			//vTaskDelay(10);
	        
			
	    break;	
		}
		case 0x04:
		{
			KeyNum=Keypad_ScanKey();
			
			if (KeyNum==Old_Key)
			{
				Keypad_IsValid=false;
				Keypad_ReadKey_Status=0x04;
				
			}
			else if(KeyNum==0x00)
			{
				Keypad_IsValid=true;
				Old_Key=Morse_Release_Key;
				Keypad_ReadKey_Status=0x00;
			}
			else
			{
				//Keypad_IsValid=false;
				
				Keypad_ReadKey_Status=0x04;
			}
			
			break;
		}
				
				
	}
	
	
	if (Keypad_IsValid) 
	{	
		Keypad_IsValid=false;
		
		return Old_Key;
	}
		else return 0;
	
	

	
}



	
extern void Keypad_Init(void)
{
	 Keypad_GPIOConfig();	
}


