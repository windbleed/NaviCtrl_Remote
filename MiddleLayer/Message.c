#include "Message.h"
#include "app.h"

//This Unit was system event handler, Response on multiple message
// such as key message [turn on/off lamps\manul|auto switch\mute control\test]
// report message [ send lamp status frame]\ACK message [acknowledge UAN  ]\remote controler [time correct\lamp control]


xQueueHandle xQueue;



void Send_Message(unsigned long pxMessage)
{
	if( xQueue != 0 ) 
	{
       
		xQueueSend( xQueue, ( void * ) &pxMessage,  0 );
												
	}
}
				
void Handle_Message(unsigned long pxMessage)
{
	unsigned short 	Msg_Event=(pxMessage&Msg_Event_Mask)>>24;
	
	unsigned short  Msg_Info=(pxMessage&MSG_Info_Mask);
	
	
	switch(Msg_Event)
	{
		case Msg_Key_Input:
		{
			Handle_Key_Pressed(Msg_Info);
			break;
		}					
		
	}
		
	
}




