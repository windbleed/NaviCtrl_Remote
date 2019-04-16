#include "MiscDev.h"
#include "stm32f0xx.h"
#include "LEDCombo.h"
#include "LampCtl_M.h"





void Misc_Init(void)
{
	Buzzer_Init();
	Sensor_Init();
	Fan_Init();
	
	PwrMain_Init();
	PwrSub_Init();
	PwrLack_Init();
}


void Buzzer_Init(void)
{
	
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(Buzzer_RCC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = Buzzer_Gpio_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	
	GPIO_Init(Buzzer_Port, &GPIO_InitStructure);
	
	GPIO_SetBits(Buzzer_Port,Buzzer_Gpio_Pin);
}


void Buzzer_SetAlarm(void)
{
	//GPIO_ResetBits(Buzzer_Port,Buzzer_Gpio_Pin);
	
}


void Buzzer_ClearAlarm(void)
{
	GPIO_SetBits(Buzzer_Port,Buzzer_Gpio_Pin);
}


void Sensor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable the GPIOA Clock */
    RCC_AHBPeriphClockCmd(Sensor_RCC, ENABLE);

    /* Configure PA.7 (ADC Channel7) in analog mode */
    GPIO_InitStructure.GPIO_Pin  = Sensor_Gpio_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_Speed= GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(Sensor_Port, &GPIO_InitStructure);
	
	Sensor_ADC_Init();
	
	
}



unsigned short Sensor_GetTemp(void)
{
	unsigned short  ADC1ConvertedVoltage = 0;
	unsigned short 	ADC1ConvertedValue = 0;
	
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
    
    /* Get ADC1 converted data */
    ADC1ConvertedValue =ADC_GetConversionValue(ADC1);
    
    /* Compute the voltage */
    ADC1ConvertedVoltage = (ADC1ConvertedValue *3300)/0xFFF;
	
	return  ADC1ConvertedVoltage;
}





static void Sensor_ADC_Init(void)
{
	ADC_InitTypeDef     ADC_InitStructure;
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	ADC_DeInit(ADC1);
  
	/* Initialize ADC structure */
	ADC_StructInit(&ADC_InitStructure);
  
	/* Configure the ADC1 in continuous mode with a resolution equal to 12 bits  */
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; 
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
	ADC_Init(ADC1, &ADC_InitStructure); 
  
	/* Convert the ADC1 Channel 7 (PA7) with 239.5 Cycles as sampling time */ 

	ADC_ChannelConfig(ADC1, ADC_Channel_7 , ADC_SampleTime_239_5Cycles);


	/* ADC Calibration */
	ADC_GetCalibrationFactor(ADC1);
  
	/* Enable the ADC peripheral */
	ADC_Cmd(ADC1, ENABLE);     
  
	/* Wait the ADRDY flag */
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY)); 
  
	/* ADC1 regular Software Start Conv */ 
	ADC_StartOfConversion(ADC1);
}


void Fan_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(Fan_RCC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = Fan_Gpio_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	
	GPIO_Init(Fan_Port, &GPIO_InitStructure);
	
	GPIO_SetBits(Fan_Port,Fan_Gpio_Pin);	
}


void Fan_Start(void)
{
	GPIO_ResetBits(Fan_Port,Buzzer_Gpio_Pin);
}

void Fan_Stop(void)
{
	GPIO_SetBits(Fan_Port,Buzzer_Gpio_Pin);
}

void PwrMain_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
   
    RCC_AHBPeriphClockCmd(PwrMain_RCC,ENABLE);

    GPIO_InitStructure.GPIO_Pin = PwrMain_Gpio_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(PwrMain_Port,&GPIO_InitStructure);
}


void PwrSub_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
   
    RCC_AHBPeriphClockCmd(PwrSub_RCC,ENABLE);

    GPIO_InitStructure.GPIO_Pin = PwrSub_Gpio_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(PwrSub_Port,&GPIO_InitStructure);
}

void PwrLack_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
   
    RCC_AHBPeriphClockCmd(PwrLack_RCC,ENABLE);

    GPIO_InitStructure.GPIO_Pin = PwrLack_Gpio_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(PwrLack_Port,&GPIO_InitStructure);
}



void Pwr_Check_Status(void)
{
	if(GPIO_ReadInputDataBit(PwrMain_Port,PwrMain_Gpio_Pin)!=0)	//low level activity
	{
		LEDCombo_TurnOn(LEDCombo_Pwr_Main);
	}
	else
		LEDCombo_TurnOff(LEDCombo_Pwr_Main);
	
	
	if(GPIO_ReadInputDataBit(PwrSub_Port,PwrSub_Gpio_Pin)!=0) //low level activity
	{
		LEDCombo_TurnOn(LEDCombo_Pwr_Sub);
	}
	else
		LEDCombo_TurnOff(LEDCombo_Pwr_Sub);
	
	if(GPIO_ReadInputDataBit(PwrLack_Port,PwrLack_Gpio_Pin)!=0) //high level activity
	{
		PwrLack_Handle();
	}
	
}

void PwrLack_Handle(void)
{
	//LampCtl_SaveEnv();
}


