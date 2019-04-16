#include "stm32f0xx.h"
#include "LedArray.h"




#define OE1          GPIO_Pin_5   	//GPIOA
#define NSS1         GPIO_Pin_2   	//GPIOA
#define SCLK1        GPIO_Pin_3  		//GPIOA
#define LEDS1        GPIO_Pin_4   	//GPIOA


static int LedArray_LightLevel;  		//4,3,2,1,0 total 5 light level.







unsigned short LedArray_FrameBuffer[Array_Size]=
 {0xffff,0xffff};


void LedArray_Init()
{

   LedArray_GPIOConfig();
   LedArray_SetLight(1);
      
}


void LedArray_SetFrame(unsigned short buf[])
{
	static unsigned char  i=0;
	
	for(;i<Array_Size;i++)
	{
		LedArray_FrameBuffer[i]=buf[i];
		
	}
}


void LedArray_GPIOConfig()
{
	//NSS1(PA2) : latch / SCLK1(PA3) : shift_clk / OE1(PA5): Enable / LEDS1(PA4):  Serial_Data_In
	
	// Set NSS1
	GPIO_InitTypeDef GPIO_InitStruchture;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	
	GPIO_InitStruchture.GPIO_Pin = NSS1;
	GPIO_InitStruchture.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruchture.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruchture.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruchture.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStruchture);
	
	GPIO_ResetBits(GPIOA, NSS1);
	//GPIO_SetBits(GPIOA, NSS1);
	
	//Set OE1
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	
	GPIO_InitStruchture.GPIO_Pin = OE1;
	GPIO_InitStruchture.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruchture.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruchture.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruchture.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStruchture);
	
	GPIO_SetBits(GPIOA, OE1);
	
	
	TIM_Config();
	
	
	//Set SCLK1
		
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	
	GPIO_InitStruchture.GPIO_Pin = SCLK1 ;
	GPIO_InitStruchture.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruchture.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruchture.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruchture.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStruchture);
	
	GPIO_SetBits(GPIOA, SCLK1);
	
	
	//Set LEDS1
		
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	
	GPIO_InitStruchture.GPIO_Pin = LEDS1 ;
	GPIO_InitStruchture.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruchture.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruchture.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruchture.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(GPIOA, &GPIO_InitStruchture);
	
	GPIO_SetBits(GPIOA, LEDS1);
	
	
	
}


static void TIM_Config(void)
{
	unsigned short TimerPeriod =0;
	unsigned short  Channel2Pulse=0;
	
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIOA, GPIOB and GPIOE Clocks enable */
  RCC_AHBPeriphClockCmd( RCC_AHBPeriph_GPIOB, ENABLE);
  

  /* GPIOB Configuration: Channel 2N and 3N as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOB, &GPIO_InitStructure); 

  GPIO_PinAFConfig(GPIOB, GPIO_PinSource0, GPIO_AF_2); 

  
  /* TIM1 Configuration ---------------------------------------------------
   Generate 7 PWM signals with 4 different duty cycles:
   TIM1 input clock (TIM1CLK) is set to APB2 clock (PCLK2)    
    => TIM1CLK = PCLK2 = SystemCoreClock
   TIM1CLK = SystemCoreClock, Prescaler = 0, TIM1 counter clock = SystemCoreClock
   SystemCoreClock is set to 48 MHz for STM32F0xx devices
   
   The objective is to generate 7 PWM signal at 17.57 KHz:
     - TIM1_Period = (SystemCoreClock / 17570) - 1
   The channel 1 and channel 1N duty cycle is set to 50%
   The channel 2 and channel 2N duty cycle is set to 37.5%
   The channel 3 and channel 3N duty cycle is set to 25%
   The channel 4 duty cycle is set to 12.5%
   The Timer pulse is calculated as follows:
     - ChannelxPulse = DutyCycle * (TIM1_Period - 1) / 100
   
   Note: 
    SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f0xx.c file.
    Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
    function to update SystemCoreClock variable value. Otherwise, any configuration
    based on this variable will be incorrect. 
  ----------------------------------------------------------------------- */
  /* Compute the value to be set in ARR regiter to generate signal frequency at 17.57 Khz 17570*/
  TimerPeriod = (SystemCoreClock / 1000 ) - 1;
 
  /* Compute CCR2 value to generate a duty cycle at 37.5%  for channel 2 and 2N */
  Channel2Pulse = (uint16_t) (((uint32_t) 990 * (TimerPeriod - 1)) / 1000);
 

  /* TIM1 clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);
  
  /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  /* Channel 1, 2,3 and 4 Configuration in PWM mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = Channel2Pulse;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;


  TIM_OC2Init(TIM1, &TIM_OCInitStructure);

  

  /* TIM1 counter enable */
  TIM_Cmd(TIM1, ENABLE);

  /* TIM1 Main Output Enable */
  TIM_CtrlPWMOutputs(TIM1, ENABLE);
}


void LedArray_SetBackLight(unsigned char level)
{
	
	unsigned short TimerPeriod =0;
	unsigned short  Channel2Pulse=0;
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	int PWM_Rate=0;
	
	
//Disble PWM ant TIM
	
	

  /* TIM1 Main Output Enable */
  TIM_CtrlPWMOutputs(TIM1, DISABLE);
	  /* TIM1 counter enable */
  TIM_Cmd(TIM1, DISABLE);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , DISABLE);
	
	TimerPeriod = (SystemCoreClock / 1000 ) - 1;
 
  /* Compute CCR2 value to generate a duty cycle at 37.5%  for channel 2 and 2N */
	
	switch(level)
	{
		case 0: PWM_Rate=999;break;
		case 1: PWM_Rate=900;break;
		case 2: PWM_Rate=500;break;
		case 3: PWM_Rate=10;break;
		//case 4: PWM_Rate=10; break;
				
			
	}
	
  Channel2Pulse = (uint16_t) (((uint32_t)PWM_Rate * (TimerPeriod - 1)) / 1000);
 

   RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);
  
  /* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  /* Channel 1, 2,3 and 4 Configuration in PWM mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = Channel2Pulse;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Set;


  TIM_OC2Init(TIM1, &TIM_OCInitStructure);
	
	
	 TIM_Cmd(TIM1, ENABLE);

  /* TIM1 Main Output Enable */
  TIM_CtrlPWMOutputs(TIM1, ENABLE);
	
	
}





void LedArray_SetLight(int level)
{
		if ((level>=1)&&(level<=4))
		 LedArray_LightLevel=level;
		
}

int  LedArray_GetLight(void)
{
	return LedArray_LightLevel;
}


void Clr_Data()
{
	
	GPIO_SetBits(GPIOA, OE1);
	GPIO_ResetBits(GPIOA,NSS1);
	
	
}

void LedArray_Disable(void)
{
	Clr_Data();
}


void Latch_Data()
{
	

	
	GPIO_ResetBits(GPIOA,NSS1);
	__NOP();
	

	GPIO_SetBits(GPIOA,NSS1);
	__NOP();


 GPIO_ResetBits(GPIOA,NSS1);
	__NOP();
	
	//GPIO_ResetBits(GPIOA, OE1);

	
	
}

void Shift_Byte(unsigned short Data, int  Null_Count)
{
	unsigned short SendData;
	int i=0;
	
	//GPIO_ResetBits(GPIOB, OE1);
	

	
	SendData=Data;
	for( ;i<Null_Count;i++)
		SendData=SendData<<1;
	
	for(;i<16;i++)
	{
	  
		if((SendData&0x8000)!=0)
			GPIO_ResetBits(GPIOA,LEDS1);
		else  
			GPIO_SetBits(GPIOA,LEDS1);
	
		GPIO_ResetBits(GPIOA,SCLK1);
		__NOP();
		
	
		GPIO_SetBits(GPIOA,SCLK1);
		__NOP();
	
		
		SendData=SendData<<1;
	
	}
	

		

	
}

void Shift_Data()
{
	
	
	if (LedArray_LedNum%16!=0)
		Shift_Byte(LedArray_FrameBuffer[LedArray_LedNum/16],(16-(LedArray_LedNum%16)));
     

	
	for(int count=LedArray_LedNum/16-1;count>=0;count--)
	{
		Shift_Byte(LedArray_FrameBuffer[count],0);
	}
	

	
}




void LedArray_ReflashFrame()
{
	
	//static portTickType xLastWakeTime;  
   
	
	if (LedArray_LightLevel<=4)             
	{
		//Clr_Data();													// clear shift register means output high Z.
		
		
		//vTaskDelayUntil( &xLastWakeTime,20);//*xFrequency);
		//vTaskDelay(15);
	
		//Enter Critical Section
		{
		//	if( xSemaphoreTake(xSemaphore_CLK,portMAX_DELAY )==pdTRUE )  
			{
				TIM_CtrlPWMOutputs(TIM1,DISABLE);
				Shift_Data();
				Latch_Data();				
				TIM_CtrlPWMOutputs(TIM1, ENABLE);
			}
			
			//vTaskDelayUntil( &xLastWakeTime,LedArray_LightLevel);//*xFrequency);
			//vTaskDelay(LedArray_LightLevel);
			//Clr_Data();		
			//xSemaphoreGive(xSemaphore_CLK);
		}          
		//Exit Critical Section
		
	}
	
}
