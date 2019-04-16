#include "LampCtl.h"
#include "LedCombo.h"
#include "Keypad.h"
#include  "LampArray.h"
#include  "CurrentSensor.h"
#include  "Message.h"
#include "semphr.h"
#include "FreeRTOS.h"
#include "task.h"
#include "DS1302.h"
#include "Env.h"
#include "app.h"
#include "VDRCoder.h"
#include "MiscDev.h"




Lamp LampCtl_Lamp[LampCtl_Max];

xSemaphoreHandle  xSemaphore_ReportFinish; 


static bool LampCtl_Need_ReCheck=false;
static bool LampCtl_DualReport_Flag=false;
static unsigned char LampCtl_UpdatePos1=0;
static unsigned char LampCtl_UpdatePos2=0;
static unsigned char LampCtl_Need_ReCheck_Pos1=0;
static unsigned char LampCtl_Need_ReCheck_Pos2=0;

static bool LampCtl_Is_BlinkOn=true;


static int Counter_Blink=0;
static int Counter_Reflash=0;
static int Counter_Report=0;
static int Counter_DualReport=0;

	
	
static int Counter_VDR_LED=0;
static bool VDR_LED_ON=false;


xQueueHandle xQueue_DualLampPos;



static unsigned char SW_ID_Navi[LampCtl_Max]=
{
	KEY1,KEY3,KEY5,KEY7,KEY9,KEY11,KEY13,KEY15,KEY17,KEY18,KEY2,KEY4,KEY6,KEY8,KEY10,KEY12,KEY14,KEY16,KEY19,KEY20
};

static unsigned char SW_ID_SIG[LampCtl_Max]=
{
	MORSE,KEY10,KEYFLASH,KEY20,KEY1,KEY3,KEY5,KEY7,KEY11,KEY13,KEY15,KEY17,KEY2,KEY4,KEY6,KEY8,KEY12,KEY14,KEY16,KEY19
	
};

static unsigned char Led_ID_Indicate_Navi[LampCtl_Max]=
{
	1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20 
};

static unsigned char Led_ID_Indicate_SIG[LampCtl_Max]=
{
	21,15,49,20 ,1,2,3,4,6,7,8,9,11,12,13,14,16,17,18,19
	
};


static unsigned char Led_ID_Warn_Navi[LampCtl_Max]=
{
	22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41
};

static unsigned char Led_ID_Warn_SIG[LampCtl_Max]=
{
	42,36,50,41,22,23,24,25,27,28,29,30,32,33,34,35,37,38,39,40
	
};

static bool Is_On_Navi[LampCtl_Max]=
{
	true,true,true,true,true,true,true,true,true,true,
	true,true,true,true,true,true,true,true,true,true
};

static bool Is_On_SIG[LampCtl_Max]=
{
	false,false,false,false,true,true,true,true,true,true,
    true,true,true,true,true,true,true,true,true,true
};

//Blink on Navigation mode is all set false;




static bool IsError[LampCtl_Max]=
{
	false,false,false,false,false,false,false,false,false,false,
	false,false,false,false,false,false,false,false,false,false
	
};

static bool ErrorACK[LampCtl_Max]=
{
	false,false,false,false,false,false,false,false,false,false,
	false,false,false,false,false,false,false,false,false,false
	
	
};


static bool IsEnable_NAVI[LampCtl_Max]=
{
	true,true,true,true,true,true,true,true,true,true,
	true,true,true,true,true,true,true,true,true,true
	
	
};


static bool IsEnable_SIG[LampCtl_Max]=
{
	true,true,true,true,true,true,true,true,true,true,
	true,true,true,true,true,true,true,true,true,true
	
	
};




void LampCtl_Navigation_Board()
{
	bool * Pt_OnStatus=ENV_Load_Lamp_OnStatus();
	bool * Pt_Enable=ENV_Load_Lamp_Enable();
	
	for(int i=0;i<LampCtl_Max;i++)
	{
		LampCtl_Lamp[i].Lamp_ID=i+1;
		LampCtl_Lamp[i].Switch_ID= SW_ID_Navi[i];
		LampCtl_Lamp[i].LED_Indicate_ID=Led_ID_Indicate_Navi[i];
		LampCtl_Lamp[i].LED_Warn_ID=Led_ID_Warn_Navi[i];
		LampCtl_Lamp[i].Is_On=Pt_OnStatus[i];//Is_On_Navi[i];// //
		LampCtl_Lamp[i].Is_Blink=false;
		LampCtl_Lamp[i].Is_Morse=false;
		LampCtl_Lamp[i].Is_Error= IsError[i];
		LampCtl_Lamp[i].ErrorACK=ErrorACK[i];
		LampCtl_Lamp[i].IsEnable=Pt_Enable[i];//IsEnable_NAVI[i];
		
	}
	
}

void LampCtl_Signal_Board()
{
    bool * Pt_OnStatus=ENV_Load_Lamp_OnStatus();
	bool * Pt_Enable=ENV_Load_Lamp_Enable();
	
	for(int i=0;i<LampCtl_Max;i++)
	{
		LampCtl_Lamp[i].Lamp_ID=i+1;
		LampCtl_Lamp[i].Switch_ID= SW_ID_SIG[i];
		LampCtl_Lamp[i].LED_Indicate_ID=Led_ID_Indicate_SIG[i];
		LampCtl_Lamp[i].LED_Warn_ID=Led_ID_Warn_SIG[i];
		LampCtl_Lamp[i].Is_On=Pt_OnStatus[i]; //  Is_On_SIG[i];//
		LampCtl_Lamp[i].Is_Blink=((i==1)||(i==2)||(i==3))?true:false;
		LampCtl_Lamp[i].Is_Morse=(i!=0)?false:true;
		LampCtl_Lamp[i].Is_Error= IsError[i];
		LampCtl_Lamp[i].ErrorACK=ErrorACK[i];
		LampCtl_Lamp[i].IsEnable=Pt_Enable[i];//IsEnable_SIG[i];////IsEnable_SIG[i];
		
	}
	
	
	
	
}





void LampCtl_Init()
{
	
	
	LampArray_Init();
	CurrentSensor_Init();
	
	
	
	switch(ENV_Board_Type)
	{
		case Navigation_B: 
		
			LampCtl_Navigation_Board();
			break;
		case SIG_B:
			LampCtl_Signal_Board();
			
			break;
	}
	
	ENV_Load_LEDCombo_Color();
	
	LampCtl_SetLampStatus();
	
	
	
	

}


void LampCtl_SingleReport_Handler(unsigned char Pos1,unsigned char Pos2)
{
	unsigned short dualpos=0;
	

	
	dualpos=Pos1;
	dualpos<<=8;
	dualpos|=Pos2;
	xQueueSend(xQueue_DualLampPos,&dualpos,2);
	
	
}



static void LampCtl_SendVDRFrame(unsigned char Pos)
{
	unsigned char Time[6];
	
	Time[0]=DS1302_CurrentTime[2]/10+48;
	Time[1]=DS1302_CurrentTime[2]%10+48;
	Time[2]=DS1302_CurrentTime[1]/10+48;
	Time[3]=DS1302_CurrentTime[1]%10+48;
	Time[4]=DS1302_CurrentTime[0]/10+48;
	Time[5]=DS1302_CurrentTime[0]%10+48;	
		
	VDRCoder_Set_Protype();
	VDRCoder_Set_Time(Time);
	VDRCoder_Set_UAN(ENV_Board_ID,Pos);
		
	if (LampCtl_Lamp[Pos-1].Is_Error)
	{
			
		if (LampCtl_Lamp[Pos-1].ErrorACK)
		{
			VDRCoder_Set_Status_ACK(2,true);
		}
		else  
		{
			VDRCoder_Set_Status_ACK(2,false);
		}
	}
	else
	{	
		if (LampCtl_Lamp[Pos-1].Is_On)
		{ 
			VDRCoder_Set_Status_ACK(1,false);
		}
		else
		{ 
			VDRCoder_Set_Status_ACK(0,false);
		}
	}
		
	VDRCoder_Set_CheckSum();
	VDRCoder_SendMessage(1);
	VDRCoder_SendMessage(2);
	
}


static void LampCtl_ReportStatus(unsigned char Pos1, unsigned char Pos2)
{

			DS1302_ReadTime();
			if (Pos1!=0)
				LampCtl_SendVDRFrame(Pos1);
			if (Pos2!=0)
				LampCtl_SendVDRFrame(Pos2);
			
			
	
	
}


void LampCtl_ReportFullStatus(void)
{
	DS1302_ReadTime();
	
	for(int i=0;i<LampCtl_Num;i++)
	{
		LampCtl_SendVDRFrame(i+1);
		
	}
	
}

static void LampCtl_FullReport_Required(void)
{
	unsigned long Message;
	Message=Msg_Report_Full;
	Message<<=24;
		
	Send_Message(Message);
}

static void LampCtl_SingleReport_Required(unsigned char LampPos)
{
	
	unsigned long  Message;
	Message=Msg_Report_Single;
	Message<<=24;
	Message|=LampPos;
	Send_Message(Message);

}

extern void LampCtl_DualReport_Required(unsigned char LampPos1,unsigned char LampPos2)
{
	unsigned long  Message;
	
	unsigned short info;
	
	info=LampPos1;
	info<<=8;
	info|=LampPos2;
	
	
	Message=Msg_Report_Single;
	Message<<=24;
	Message|=info;
	
	Send_Message(Message);
	
	
	
}




static void LampCtl_ErrorHandle(bool WithCom)
{
	static bool Alarm;
	
	Alarm=false;
	for(int i=0;i<LampCtl_Num;i++)
	{
		
		LampCtl_ShowErrorLED(i+1,WithCom);
		if (LampCtl_Lamp[i].Is_Error&&(!LampCtl_Lamp[i].ErrorACK))
			Alarm=true;
		
		
	}
	
	if (Alarm) 
		Buzzer_SetAlarm();
	else 
		Buzzer_ClearAlarm();
	
	
	
}

static void LampCtl_Toggle(unsigned char Pos)
{
	unsigned char Mask_Bit=0x01;
	int Loc_Byte=(Pos-1)/5;
	int Loc_Bit=(Pos-1)%5;
	
	
	Mask_Bit<<=Loc_Bit;
		
	if((LampCtl_Is_BlinkOn)&&(LampCtl_Lamp[Pos-1].Is_On))
	{
		LampArray_Frame[Loc_Byte]|=Mask_Bit;
	}
	else 
		LampArray_Frame[Loc_Byte]&=~Mask_Bit;
		
}


static void LampCtl_ToggleBlink()
{
	for(int i=0;i<LampCtl_Num;i++)
	{
		if (LampCtl_Lamp[i].Is_Blink)
			LampCtl_Toggle(i+1);
				
	}
	LampCtl_Is_BlinkOn=!LampCtl_Is_BlinkOn;
	
}


void LampCtl_Run_M(void)
{
	
	
	unsigned char * P_Sensor;
	unsigned short dualPos;
	
	
	
	 
	if(Counter_Reflash>=1)
	{
		Counter_Reflash=0;
		 
		LampArray_Reflash();
		 
		vTaskDelay(100);
		 
		P_Sensor=CurrentSensor_GetData();
		 
		LampCtl_OnStatus_Verify(P_Sensor);
		 
		LampCtl_ErrorHandle(false);
		
	
		 
	}
	  
	if(Counter_Report>=10*10 )  //full report per 30 sec 
	{
		Counter_Report=0;
		LampCtl_ReportFullStatus();
		LEDCombo_TurnOn(LEDCombo_COM2_ID);
		VDR_LED_ON=true;
		 		 
	}
	 
	if(VDR_LED_ON)
	{
		Counter_VDR_LED++;
		if (Counter_VDR_LED>=2)
		{
			LEDCombo_TurnOff(LEDCombo_COM2_ID);
			VDR_LED_ON=false;
			Counter_VDR_LED=0;
		}
	}
	 
	if(LampCtl_DualReport_Flag)
	{
		if ((LampCtl_UpdatePos2!=FLASHLeft_Lamp_ID)&&(LampCtl_UpdatePos2!=FLASHMid_Lamp_ID)&&(LampCtl_UpdatePos2!=FLASHRight_Lamp_ID))
		{
			LampCtl_ReportStatus(LampCtl_UpdatePos1,LampCtl_UpdatePos2);
			xSemaphoreGive(xSemaphore_ReportFinish);
			LEDCombo_TurnOn(LEDCombo_COM2_ID);
			VDR_LED_ON=true;
				
		}
		else 
		{
			if(LampCtl_Is_BlinkOn)
			{
				LampCtl_ReportStatus(LampCtl_UpdatePos1,LampCtl_UpdatePos2);
				xSemaphoreGive(xSemaphore_ReportFinish);
				LEDCombo_TurnOn(LEDCombo_COM2_ID);
				VDR_LED_ON=true;
				
					
			}
			else 
			{
				LampCtl_Need_ReCheck=true;
				LampCtl_Need_ReCheck_Pos1=LampCtl_UpdatePos1;
				LampCtl_Need_ReCheck_Pos2=LampCtl_UpdatePos2;
			}
		}
		LampCtl_DualReport_Flag=false;
		//CanBeRead=false;
	}
	 
	 
	 
	if(Counter_DualReport>=0)
	{
		
		if( xSemaphoreTake(xSemaphore_ReportFinish,0 )==pdTRUE )  
		{
			if (xQueueReceive(xQueue_DualLampPos,&dualPos,0))
			{
				LampCtl_DualReport_Flag=true;
				LampCtl_UpdatePos1=(dualPos>>8)&0x00ff;
				LampCtl_UpdatePos2=dualPos&0x00ff;
			
			}
			else 
				xSemaphoreGive(xSemaphore_ReportFinish);
				
		Counter_DualReport=0;
		}
		 
	}
	 
	if (LampCtl_Need_ReCheck)
	{		 
		if(LampCtl_Is_BlinkOn)
		{
			LampCtl_ReportStatus(LampCtl_Need_ReCheck_Pos1,LampCtl_Need_ReCheck_Pos2);
			xSemaphoreGive(xSemaphore_ReportFinish);
			LampCtl_Need_ReCheck=false;
			
			LEDCombo_TurnOn(LEDCombo_COM2_ID);
			VDR_LED_ON=true;
		}
	}
	 
	if (Counter_Blink>=4)
	{
		Counter_Blink=0;
		LampCtl_ToggleBlink();
		 
	}
	 
	 
	vTaskDelay(10);
	 
	Counter_Blink++;
	Counter_Reflash++;
	Counter_Report++;
	Counter_DualReport++;
	
}

void LampCtl_TurnOn_LogSWI(unsigned char LampPos,bool With_Message)
{
	unsigned char Mask_Bit=0x01;
	int Loc_Byte=(LampPos-1)/4;
	int Loc_Bit=(LampPos-1)%4;
	

	if(LampCtl_Lamp[LampPos-1].IsEnable)
	{
	
		Mask_Bit<<=Loc_Bit;
	
	
	
		LampArray_Frame[Loc_Byte]|=Mask_Bit;
		
		
		LampCtl_Lamp[LampPos-1].Is_On=true;
	
		LEDCombo_TurnOn(LampCtl_Lamp[LampPos-1].LED_Indicate_ID);
	
	
	if (With_Message)
		LampCtl_SingleReport_Required(LampPos);

	
	}

	
	

	
}

void LampCtl_TurnOn(unsigned char LampPos)
{
	unsigned char Mask_Bit=0x01;
	int Loc_Byte=(LampPos-1)/4;
	int Loc_Bit=(LampPos-1)%4;
	

	if(LampCtl_Lamp[LampPos-1].IsEnable)
	{
	
		Mask_Bit<<=Loc_Bit;
	
	
	
		LampArray_Frame[Loc_Byte]|=Mask_Bit;
		
		
		LampCtl_Lamp[LampPos-1].Is_On=true;
	
		LEDCombo_TurnOn(LampCtl_Lamp[LampPos-1].LED_Indicate_ID);
	
	
	
		LampCtl_SingleReport_Required(LampPos);
		
	

	
	}

	
	

	
}


void LampCtl_TurnOff_LOGSWI(unsigned char LampPos,bool With_Message)
{
	unsigned char Mask_Bit=0x01;
	int Loc_Byte=(LampPos-1)/4;
	int Loc_Bit=(LampPos-1)%4;

	
	if(LampCtl_Lamp[LampPos-1].IsEnable)
	{
		Mask_Bit<<=Loc_Bit;
	
		LampArray_Frame[Loc_Byte]&=(~Mask_Bit);
	
		LampCtl_Lamp[LampPos-1].Is_On=false;
		LEDCombo_TurnOff(LampCtl_Lamp[LampPos-1].LED_Indicate_ID);
	
	if(With_Message)
		LampCtl_SingleReport_Required(LampPos);
	}
	

}


void LampCtl_TurnOff(unsigned char LampPos)
{
	unsigned char Mask_Bit=0x01;
	int Loc_Byte=(LampPos-1)/4;
	int Loc_Bit=(LampPos-1)%4;

	
	if(LampCtl_Lamp[LampPos-1].IsEnable)
	{
		Mask_Bit<<=Loc_Bit;
	
		LampArray_Frame[Loc_Byte]&=(~Mask_Bit);
	
		LampCtl_Lamp[LampPos-1].Is_On=false;
		LEDCombo_TurnOff(LampCtl_Lamp[LampPos-1].LED_Indicate_ID);
	

		LampCtl_SingleReport_Required(LampPos);
		
		
		
	}
	

}


unsigned char LampCtl_SearchLampByKey(unsigned char Keyvalue)
{
	for(int i=0;i<LampCtl_Max;i++)
	{
	 if (ENV_Board_Type==Navigation_B)
	 {
		 if (Keyvalue==SW_ID_Navi[i]) return i;
		 
	 }
	 else
	 {
		 if (Keyvalue==SW_ID_SIG[i]) return i;
	 }
		
	}
	
	return 0xff;
	
	
}

static void LampCtl_SetLampStatus(void)
{
	for (int i=0;i<LampCtl_Num;i++)
	{
		if (LampCtl_Lamp[i].IsEnable)
		{
			if (LampCtl_Lamp[i].Is_On)
			{
				LampCtl_TurnOn_LogSWI(i+1,false);
			
			}
			else 
			{
				LampCtl_TurnOff_LOGSWI(i+1,false);
			
			}
		
			if(LampCtl_Lamp[i].Is_Error)
			{
				LEDCombo_SetBlink(LampCtl_Lamp[i].LED_Warn_ID,true);
				LEDCombo_TurnOn(LampCtl_Lamp[i].LED_Warn_ID);
			}
		}
		else
		{
			LampCtl_TurnOff_LOGSWI(i+1,false);
			
		}
	}
	
	
	if (ENV_Board_Type==SIG_B)
	{
		
		if((!LampCtl_Lamp[FLASHLeft_Lamp_ID-1].IsEnable)||(!LampCtl_Lamp[FLASHRight_Lamp_ID-1].IsEnable)||(!LampCtl_Lamp[FLASHMid_Lamp_ID-1].IsEnable))
			return ;
		LampCtl_Load_FlashStatus();
	}
	
	
	
	
}



void LampCtl_SetErrorACK(void)
{
	
	
	for(int i=0;i<LampCtl_Num;i++)
	{
		if (LampCtl_Lamp[i].Is_Error)
		{
			LampCtl_Lamp[i].ErrorACK=true;
			
		}
	}
	
	
	
	
}


void LampCtl_SetSingleErrorACK(unsigned char LampPos)
{
	if (LampCtl_Lamp[LampPos-1].Is_Error)
	{
		LampCtl_Lamp[LampPos-1].ErrorACK=true;
			
	}
	
	
	
}




static void LampCtl_ShowErrorLED(unsigned char Pos,bool WithCom)
{
	if (LampCtl_Lamp[Pos-1].Is_Error)
		{
			if (!LEDCombo_GetStatus(LampCtl_Lamp[Pos-1].LED_Warn_ID))
			{
			
				if (LampCtl_Lamp[Pos-1].ErrorACK)
				{
					LEDCombo_SetBlink(LampCtl_Lamp[Pos-1].LED_Warn_ID,false);
					
				}
				else  
				{
					LEDCombo_SetBlink(LampCtl_Lamp[Pos-1].LED_Warn_ID,true);
					//Buzzer_SetAlarm();
				}
			
				LEDCombo_TurnOn(LampCtl_Lamp[Pos-1].LED_Warn_ID);
								
			}
			else if (LampCtl_Lamp[Pos-1].ErrorACK)
			{
				LEDCombo_SetBlink(LampCtl_Lamp[Pos-1].LED_Warn_ID,false);
				LEDCombo_TurnOn(LampCtl_Lamp[Pos-1].LED_Warn_ID);
				
			}
			
		}
	else
	{
		LEDCombo_TurnOff(LampCtl_Lamp[Pos-1].LED_Warn_ID);
		
		
	}
	

		
	
	
}


void LampCtl_TestMode(void)
{
	/*
	
	if (M24C02_Check())
		printf("M24C02 Check OK \r\n");
	else 
		printf("M24C02 Check Fail \r\n");
	
	DS1302_ReadTime();
	vTaskDelay(10);
	printf("Current Data is:%d/%d/%d \r\n",DS1302_CurrentTime[6],DS1302_CurrentTime[4],DS1302_CurrentTime[3]);
	printf("        Time is:%d/%d/%d \r\n",DS1302_CurrentTime[2],DS1302_CurrentTime[1],DS1302_CurrentTime[0]);
	
	
	LampCtl_SaveEnv();
	
	*/
	
	// store current 54 ledcombo on status
	LEDCombo_SaveCurrent_OnStatus();
	
	//light all the ledcombo no including the Disable Lamps's indicate and warn ledCombo
	//Note this was some difference between SIG_B and Navigation_B
		
	LEDCombo_LightOn_NoneLampRelative();
	
	for(int i=0;i<LampCtl_Num;i++)
	{
		if (LampCtl_Lamp[i].IsEnable)
		{
			LEDCombo_SetBlink(LampCtl_Lamp[i].LED_Warn_ID,false);
			LEDCombo_TurnOn(LampCtl_Lamp[i].LED_Indicate_ID);
			LEDCombo_TurnOn(LampCtl_Lamp[i].LED_Warn_ID);
		}
		
		
	}

}

void LampCtl_NormalMode(void)
{
	LEDCombo_LoadCurrent_Onstatus();
}




extern void LampCtl_SaveEnv(void)
{
	if(ENV_Board_Type==Navigation_B)
	{
		for(int i=0;i<LampCtl_Max;i++)
		{
			Is_On_Navi[i]=LampCtl_Lamp[i].Is_On;
			IsEnable_NAVI[i]=LampCtl_Lamp[i].IsEnable;
		}
		ENV_Save_Lamp_OnStatus(Is_On_Navi);
		
	}
	else
	{
		for(int i=0;i<LampCtl_Max;i++)
		{
			Is_On_SIG[i]=LampCtl_Lamp[i].Is_On;
			IsEnable_SIG[i]=LampCtl_Lamp[i].IsEnable;
		}
		ENV_Save_Lamp_OnStatus(Is_On_SIG);
	
		LampCtl_Save_FlashStatus();
	}
}




void LampCtl_Load_FlashStatus(void)
{
	unsigned short status=ENV_Load_Key_FlashStatus();
	unsigned char MainStatus=(status>>8)&0xff;
	unsigned char SubStatus=status&0xff;
	
	FlashLamp_Status=MainStatus;
	FlashLamp_Status_Pre=SubStatus;
	
	switch(MainStatus)
	{
		case 0x00:
		{
			LampCtl_TurnOff_LOGSWI(FLASHLeft_Lamp_ID,false);
			LampCtl_TurnOff_LOGSWI(FLASHMid_Lamp_ID,false);
			LampCtl_TurnOff_LOGSWI(FLASHRight_Lamp_ID,false);
			LEDCombo_TurnOn(Auto_LED_ID );
			LEDCombo_TurnOff(Manual_LED_ID );
			break;
			
		}
		case 0x01:
		{
			LampCtl_TurnOn_LogSWI(FLASHLeft_Lamp_ID,false);
			LampCtl_TurnOff_LOGSWI(FLASHRight_Lamp_ID,false);
			LampCtl_TurnOff_LOGSWI(FLASHMid_Lamp_ID,false);
			LEDCombo_TurnOn(Auto_LED_ID);
			LEDCombo_TurnOff(Manual_LED_ID);
			break;
			
		}
		case 0x02:
		{
			LampCtl_TurnOff_LOGSWI(FLASHLeft_Lamp_ID,false);
			LampCtl_TurnOff_LOGSWI(FLASHRight_Lamp_ID,false);
			LampCtl_TurnOn_LogSWI(FLASHMid_Lamp_ID,false);
			LEDCombo_TurnOn(Auto_LED_ID);
			LEDCombo_TurnOff(Manual_LED_ID);
			break;
			
		}
		case 0x03:
		{
			LampCtl_TurnOff_LOGSWI(FLASHLeft_Lamp_ID,false);
			LampCtl_TurnOn_LogSWI(FLASHRight_Lamp_ID,false);
			LampCtl_TurnOff_LOGSWI(FLASHMid_Lamp_ID,false);
			LEDCombo_TurnOn(Auto_LED_ID);
			LEDCombo_TurnOff(Manual_LED_ID);
			break;
			
		}
		case 0x04:
		{
			LampCtl_TurnOn_LogSWI(FLASHLeft_Lamp_ID,false);
			LampCtl_TurnOff_LOGSWI(FLASHRight_Lamp_ID,false);
			LampCtl_TurnOn_LogSWI(FLASHMid_Lamp_ID,false);
			LEDCombo_TurnOn(Auto_LED_ID);
			LEDCombo_TurnOff(Manual_LED_ID);
			break;
			
		}
		case 0x05:
		{
			LampCtl_TurnOff_LOGSWI(FLASHLeft_Lamp_ID,false);
			LampCtl_TurnOn_LogSWI(FLASHRight_Lamp_ID,false);
			LampCtl_TurnOn_LogSWI(FLASHMid_Lamp_ID,false);
			LEDCombo_TurnOn(Auto_LED_ID);
			LEDCombo_TurnOff(Manual_LED_ID);
			break;
			
		}
		case 0x06:
		{
			LampCtl_TurnOff_LOGSWI(FLASHLeft_Lamp_ID,false);
			LampCtl_TurnOff_LOGSWI(FLASHRight_Lamp_ID,false);
			LampCtl_TurnOff_LOGSWI(FLASHMid_Lamp_ID,false);
			
			LEDCombo_TurnOn(Manual_LED_ID);
			LEDCombo_TurnOff(Auto_LED_ID);
			
			LEDCombo_TurnOff(FLASHLeft_Indicate_LED_ID);
			LEDCombo_TurnOff(FLASHRight_Indicate_LED_ID );
			LEDCombo_TurnOff(FLASHMid_Indicate_LED_ID);
			break;
			
		}
		case 0x07:
		{
			LampCtl_TurnOff_LOGSWI(FLASHLeft_Lamp_ID,false);
			LampCtl_TurnOff_LOGSWI(FLASHRight_Lamp_ID,false);
			LampCtl_TurnOff_LOGSWI(FLASHMid_Lamp_ID,false);
			
			LEDCombo_TurnOn(Manual_LED_ID);
			LEDCombo_TurnOff(Auto_LED_ID);
			
			LEDCombo_TurnOn(FLASHLeft_Indicate_LED_ID);
			LEDCombo_TurnOff(FLASHRight_Indicate_LED_ID );
			LEDCombo_TurnOff(FLASHMid_Indicate_LED_ID);
			break;
			
		}
		case 0x08:
		{
			LampCtl_TurnOff_LOGSWI(FLASHLeft_Lamp_ID,false);
			LampCtl_TurnOff_LOGSWI(FLASHRight_Lamp_ID,false);
			LampCtl_TurnOff_LOGSWI(FLASHMid_Lamp_ID,false);
			
			LEDCombo_TurnOn(Manual_LED_ID);
			LEDCombo_TurnOff(Auto_LED_ID);
			
			LEDCombo_TurnOff(FLASHLeft_Indicate_LED_ID);
			LEDCombo_TurnOff(FLASHRight_Indicate_LED_ID );
			LEDCombo_TurnOn(FLASHMid_Indicate_LED_ID);
			break;
			
		}
		case 0x09:
		{
			LampCtl_TurnOff_LOGSWI(FLASHLeft_Lamp_ID,false);
			LampCtl_TurnOff_LOGSWI(FLASHRight_Lamp_ID,false);
			LampCtl_TurnOff_LOGSWI(FLASHMid_Lamp_ID,false);
			
			LEDCombo_TurnOn(Manual_LED_ID);
			LEDCombo_TurnOff(Auto_LED_ID);
			
			LEDCombo_TurnOff(FLASHLeft_Indicate_LED_ID);
			LEDCombo_TurnOn(FLASHRight_Indicate_LED_ID );
			LEDCombo_TurnOff(FLASHMid_Indicate_LED_ID);
			break;
			
		}
		case 0x10:
		{
			LampCtl_TurnOff_LOGSWI(FLASHLeft_Lamp_ID,false);
			LampCtl_TurnOff_LOGSWI(FLASHRight_Lamp_ID,false);
			LampCtl_TurnOff_LOGSWI(FLASHMid_Lamp_ID,false);
			
			LEDCombo_TurnOn(Manual_LED_ID);
			LEDCombo_TurnOff(Auto_LED_ID);
			
			LEDCombo_TurnOn(FLASHLeft_Indicate_LED_ID);
			LEDCombo_TurnOff(FLASHRight_Indicate_LED_ID );
			LEDCombo_TurnOn(FLASHMid_Indicate_LED_ID);
			break;
			
		}
		case 0x11:
		{
			LampCtl_TurnOff_LOGSWI(FLASHLeft_Lamp_ID,false);
			LampCtl_TurnOff_LOGSWI(FLASHRight_Lamp_ID,false);
			LampCtl_TurnOff_LOGSWI(FLASHMid_Lamp_ID,false);
			
			LEDCombo_TurnOn(Manual_LED_ID);
			LEDCombo_TurnOff(Auto_LED_ID);
			
			LEDCombo_TurnOff(FLASHLeft_Indicate_LED_ID);
			LEDCombo_TurnOn(FLASHRight_Indicate_LED_ID );
			LEDCombo_TurnOn(FLASHMid_Indicate_LED_ID);
			break;
			
		}
		case 0x12:  											//will not excute  because env status was not be saved the enter diandong status 
		{
			LampCtl_TurnOff_LOGSWI(FLASHLeft_Lamp_ID,false);
			LampCtl_TurnOff_LOGSWI(FLASHRight_Lamp_ID,false);
			LampCtl_TurnOff_LOGSWI(FLASHMid_Lamp_ID,false);
			
			LEDCombo_TurnOn(Manual_LED_ID);
			LEDCombo_TurnOff(Auto_LED_ID);
			
		
			break;
			
		}
		
		
	}

	
	
}

void LampCtl_Save_FlashStatus(void)
{
	
	unsigned char MainStatus=FlashLamp_Status;
	unsigned char SubStatus=FlashLamp_Status_Pre;
	unsigned short status=MainStatus;
	status<<=8;
	status|=SubStatus;
	
	ENV_Save_Key_FlashStatus(status);
	
	
}



// verify FLASHRight_Indicate_LED_ID  bits with Lamp[i].Is_On , if OK ,Reset Is_Error, if fail ,Set Is_Error;
// Mirror bits layout In16 In17 In18 In19 In20 0 0 0   In11 In12 In13 In14 In15 0 0 0  In6 In7 In8 In9 In10 0 0 0   In1 In2 In3 In4 In5 0 0 0
// byte loc (i-1)/5       bit loc 8-(i-1)%5
// This function will be changed while light sensor driver being changed
//hardware V4
// verify FLASHRight_Indicate_LED_ID  bits with Lamp[i].Is_On , if OK ,Reset Is_Error, if fail ,Set Is_Error;
// Mirror bits layout In17 In18 In19 In20 0 0 0 0  In13 In14 In 15 In16 0 0 0 0 In9 In10 In11 In12 0 0 0 0   In5 In6 In7 In8 0 0 0 0 In1 In2 In3 In4 0 0 0 0
// byte loc (i-1)/4       bit loc 8-(i-1)%4



bool LampCtl_OnStatus_Verify(unsigned char * Buf)
{
	unsigned char Bit_Mask=0x01;
	bool  OnStatus_Mirror=false;
	bool  Result_Verify=true;
	
	
	for(int i=0;i<LampCtl_Num;i++)
	{
		Bit_Mask=(0x01<<(7-(i%4)));
		
		OnStatus_Mirror=(Buf[i/4]&Bit_Mask)==0?true:false;
		
		if (!LampCtl_Lamp[i].Is_Blink)
		{
		
			if ((LampCtl_Lamp[i].Is_On)==OnStatus_Mirror)
			{
				if((LampCtl_Lamp[i].Is_Error)&&LampCtl_Lamp[i].Is_On)
				{
			
					LampCtl_Lamp[i].ErrorACK=false;
				}
					LampCtl_Lamp[i].Is_Error=false;
			
			}
			else 
			{
			
				LampCtl_Lamp[i].Is_Error=true;
				Result_Verify=false;
			
			}
		}
		else 
		{
			if ((LampCtl_Is_BlinkOn)&&(LampCtl_Lamp[i].Is_On))
			{
				if (OnStatus_Mirror==LampCtl_Lamp[i].Is_On)
				{
					if(LampCtl_Lamp[i].Is_Error)
					{
			
						LampCtl_Lamp[i].ErrorACK=false;
					}
					LampCtl_Lamp[i].Is_Error=false;
				}
				else 
				{
			
					LampCtl_Lamp[i].Is_Error=true;
					Result_Verify=false;
			
				}
								
			}
			else if((!LampCtl_Is_BlinkOn)&&(LampCtl_Lamp[i].Is_On))
			{
				
				
			}
			else if(!LampCtl_Lamp[i].Is_On)
			{
				if (OnStatus_Mirror==LampCtl_Lamp[i].Is_On)
				{
					
					LampCtl_Lamp[i].Is_Error=false;
				}
				else 
				{
			
					LampCtl_Lamp[i].Is_Error=true;
					Result_Verify=false;
			
				}
				
			}
				
		}
	
		
	}
	
	
	
	
	return Result_Verify;
	
	
}


void LampCtl_FlashLight_Manual(void)
{
	LampCtl_Lamp[FLASHLeft_Lamp_ID].Is_Blink=false;
	LampCtl_Lamp[FLASHMid_Lamp_ID].Is_Blink=false;
	LampCtl_Lamp[FLASHRight_Lamp_ID].Is_Blink=false;
}

void LampCtl_FlashLight_Auto(void)
{
	LampCtl_Lamp[FLASHLeft_Lamp_ID].Is_Blink=true;
	LampCtl_Lamp[FLASHMid_Lamp_ID].Is_Blink=true;
	LampCtl_Lamp[FLASHRight_Lamp_ID].Is_Blink=true;
}



extern  void LampCtl_SetLedCombo_Color(unsigned char * pColor)
{
	for(int i=0;i<LampCtl_Num;i++,pColor++)
	{
		LEDCombo_ConfigColor(LampCtl_Lamp[i].LED_Indicate_ID,*pColor);
		
	}
	
	
}

















