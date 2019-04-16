#ifndef Led_Array_Unit
#define Led_Array_Unit

#include "stm32f0xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define LedArray_LedNum 25
#define Array_Size ((LedArray_LedNum%16==0)?(LedArray_LedNum/16):(LedArray_LedNum/16+1))

static int LedArray_LightLevel;


 



static int  LedArray_GetLight(void);
static void LedArray_Disable(void);
static void LedArray_GPIOConfig(void);

static void TIM_Config(void);

extern  xSemaphoreHandle  xSemaphore_CLK;      //CLK Signal ,confirm ledarray/keypad/lamparray/currentsensor reflash working alone.
extern  unsigned short LedArray_FrameBuffer[Array_Size];

extern void LedArray_Init(void);
extern void LedArray_SetLight(int level);
extern void LedArray_SetBackLight(unsigned char level);
extern void LedArray_ReflashFrame(void);
extern void LedArray_SetFrame(unsigned short buf[]);




#endif


