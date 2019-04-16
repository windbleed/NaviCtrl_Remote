#include "app.h"

#include "LEDCombo.h"
#include "Keypad_M.h"
#include "TX_Framer.h"
#include "ControllerStatus.h"


#include "main.h"
#include "usart.h"





static unsigned char level=1;


 bool  IsTestMode=false;




void Handle_Key_Pressed(unsigned char Msg_Info)
{

	unsigned char KeyID=Msg_Info;
	
	
	if (IsTestMode)
	{
		if (KeyID==Morse_Release_Key)
		{
			
			LEDCombo_EnterNormalMode();
			
			IsTestMode=false;
			
			LEDCombo_TurnOff(LEDCombo_Test);
			
			vTaskResume(UartHandle);
			
			
		}
		return;
		
	}
	
	switch(KeyID)
	{
		case KEYDIM:
		{
				level=LEDCombo_GetLight();
				level=((level==3)?0:(level+1));
				LEDCombo_SetLight(level);
				LEDCombo_TurnOn(LEDCombo_Dimmer);
			  
				
				break;
				
		}
		case KEYTest :
		{				
					//LampCtl_SaveEnv();
					
			vTaskSuspend(UartHandle);
					
			LEDCombo_TurnOn(LEDCombo_Test);
					
			IsTestMode=true;
			
			LEDCombo_EnterTestMode();
					
			break;
				
		}
		
		case KEYDianDong :
		{				
		
			if(!Status_MODE_AUTO)
			{
				LEDCombo_TurnOn(LEDCombo_DianDong);
					
				TXFramer_SendSimKey(0x01,SIMKey_DianDong);
			}
					
			break;
				
		}
		case KEYLeft  :
		{				
							
	
			TXFramer_SendSimKey(0x01,SIMKey_FlashLeft);
					
			break;
				
		}
		case KEYMid :
		{				
		
					
	
			TXFramer_SendSimKey(0x01,SIMKey_FlashMid);
					
			break;
				
		}
		case KEYRight :
		{				
						
	
			TXFramer_SendSimKey(0x01,SIMKey_FlashRight);
					
			break;
				
		}
		case KEYMode :
		{				
		
		 Status_MODE_AUTO=!Status_MODE_AUTO;
			 
			 
		 Status_Set_Mode(Status_MODE_AUTO);
	
			TXFramer_SendSimKey(0x01,SIMKey_Mode);
			
			vTaskDelay(100);
			
			//TXFramer_SendSync();
			
			//vTaskDelay(100);
					
			break;
				
		}
		
		
		
		case Morse_Release_Key:
		{
			if(LEDCombo_GetStatus(LEDCombo_DianDong)==true)			
				TXFramer_SendSimKey(0x01,SIMKey_MorseRelease);
			
			LEDCombo_TurnOff(LEDCombo_Dimmer);
			LEDCombo_TurnOff(LEDCombo_Test);
			LEDCombo_TurnOff(LEDCombo_DianDong);
			break;
				
		}
		
		
			
			
	}
		
					
}
