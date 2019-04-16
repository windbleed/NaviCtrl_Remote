#ifndef Message_Unit
#define Message_Unit

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define Event_Save_Status     0x01
#define Event_Format_EEPROM   0x02
#define Event_LightOn         0x03


#define Msg_Event_Mask       0xff000000
#define MSG_Info_Mask        0x0000ffff

#define MSG_ID_Mask			 		 0x00ff0000
#define MSG_On_Mask          0x0000f000
#define MSG_ERROR_Mask  	 	 0x00000f00
#define MSG_ACK_Mask         0x000000f0


#define Msg_Key_Input        0x01
#define Msg_Report_Single    0x02
#define Msg_Error_Ack        0x03
#define Msg_COM              0x04
#define Msg_Report_Full      0x05
#define Msg_CFG_ENV			 		 0x06
#define Msg_RemoteCtl	     	 0x07
#define Msg_TimeCorrect		   0x08
#define Msg_Report_Detail    0x09







extern xQueueHandle xQueue;

extern void Send_Message(unsigned long pxMessage);

extern void Send_Message_UartTask(unsigned long pxMessage);

extern void Handle_Message(unsigned long pxMessage);



#endif
