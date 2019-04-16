#ifndef Misc_Unit
#define Misc_Unit

#include "stdbool.h"


#define Buzzer_Port			GPIOF
#define Buzzer_Gpio_Pin		GPIO_Pin_1
#define Buzzer_RCC			RCC_AHBPeriph_GPIOF

#define Sensor_Port			GPIOA
#define Sensor_Gpio_Pin		GPIO_Pin_7
#define Sensor_RCC          RCC_AHBPeriph_GPIOA

#define Fan_Port			GPIOA
#define Fan_Gpio_Pin		GPIO_Pin_11
#define Fan_RCC				RCC_AHBPeriph_GPIOA


#define PwrMain_Port		GPIOA
#define PwrMain_Gpio_Pin	GPIO_Pin_15
#define PwrMain_RCC			RCC_AHBPeriph_GPIOA

#define PwrSub_Port			GPIOB
#define PwrSub_Gpio_Pin		GPIO_Pin_3
#define PwrSub_RCC			RCC_AHBPeriph_GPIOB

#define PwrLack_Port		GPIOB
#define PwrLack_Gpio_Pin	GPIO_Pin_4
#define PwrLack_RCC			RCC_AHBPeriph_GPIOB




extern void Misc_Init(void);

static void Buzzer_Init(void);
extern void Buzzer_SetAlarm(void);
extern void Buzzer_ClearAlarm(void);

static void Sensor_Init(void);
static void Sensor_ADC_Init(void);
extern unsigned short Sensor_GetTemp(void);

static void Fan_Init(void);
extern void Fan_Start(void);
extern void Fan_Stop(void);
 
static void PwrMain_Init(void);
static void PwrSub_Init(void);
static void PwrLack_Init(void);
 
extern void Pwr_Check_Status(void);
static void PwrLack_Handle(void);




#endif
