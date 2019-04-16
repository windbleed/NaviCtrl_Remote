#ifndef Main_Unit
#define Main_Unit

#include "FreeRTOS.h"

#include "task.h"


extern xTaskHandle LedHandle,KeyHandle,UartHandle,KeyMessageHandle;


void led_config(void);

void led_task(void *para);
void lamp_task(void *para);
void  key_task(void *para);
void  USART1_task(void *para);
void  USART2_task(void *para);
void  USART3_task(void *para);
void  Event_Handle_task(void *para);

#endif
