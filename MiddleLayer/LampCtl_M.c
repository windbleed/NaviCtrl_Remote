#include "LampCtl_M.h"
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


static bool LampCtl_Is_BlinkOn=true;
static bool LampCtl_Current_Blink_status=true;
static unsigned char LampCtl_Status=0; 			 //0: pend message ;1: lamparray reflash; 2: check sensor ;3: single report ;4: full report


static int Counter_Blink=0;

static int Counter_Report=0;

static bool Is_Multiple_Action=false;




	
	
static int  Counter_VDR_LED=0;
static bool VDR_LED_ON=false;

static bool LampCtl_Is_Updated=false;


xQueueHandle xQueue_LampAction;



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

static void LampCtl_SetLampStatus(void)
{
	for (int i=0;i<LampCtl_Num;i++)
	{
		if (LampCtl_Lamp[i].IsEnable)
		{
			if (LampCtl_Lamp[i].Is_On)
			{
				LampCtl_TurnOn(i+1);
			
			}
			else 
			{
				LampCtl_TurnOff(i+1);
			
			}
		
			if(LampCtl_Lamp[i].Is_Error)
			{
				LEDCombo_SetBlink(LampCtl_Lamp[i].LED_Warn_ID,true);
				LEDCombo_TurnOn(LampCtl_Lamp[i].LED_Warn_ID);
			}
		}
		else
		{
			LampCtl_TurnOff(i+1);
			
		}
	}
	
	
	if (ENV_Board_Type==SIG_B)
	{
		
		if((!LampCtl_Lamp[FLASHLeft_Lamp_ID-1].IsEnable)||(!LampCtl_Lamp[FLASHRight_Lamp_ID-1].IsEnable)||(!LampCtl_Lamp[FLASHMid_Lamp_ID-1].IsEnable))
			return ;
		LampCtl_Load_FlashStatus();
	}
	
	
	
	
}

void LampCtl_Detail_Report(unsigned char LampPos,bool LampOn,bool LampError,bool ErrorACK)
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
	VDRCoder_Set_UAN(ENV_Board_ID,LampPos);
	
	if (LampError)
	{
			
		if (ErrorACK)
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
		if (LampOn)
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

static void LampCtl_SendVDRFrame(unsigned char Pos,unsigned char COMx)
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
	
    if (COMx==1)
		VDRCoder_SendMessage(2);
	else if(COMx==2)
	{
		VDRCoder_SendMessage(1);
		VDRCoder_SendMessage(2);
	}
	
}


void LampCtl_SingleReportStatus(unsigned char LampPos)
{
	DS1302_ReadTime();
	//LampCtl_SendVDRFrame(LampPos,2);
	
	LampCtl_DetailReport_Required(LampPos);
		
}


static void LampCtl_FullReport_Required(void)
{
	unsigned long Message;
	Message=Msg_Report_Full;
	Message<<=24;
		
	Send_Message_UartTask(Message);
}


static void LampCtl_DetailReport_Required(unsigned char LampPos)
{
	
	unsigned long Message=0;
	Message=Msg_Report_Detail;
	Message<<=24;
	Message|=(LampPos<<16);
	
	if (LampCtl_Lamp[LampPos-1].Is_On)
			Message|= 0x00001000;
	
	if (LampCtl_Lamp[LampPos-1].Is_Error)
		Message|= 0x00000100;
	
	if (LampCtl_Lamp[LampPos-1].ErrorACK)
		Message|= 0x00000010;
		
	Send_Message_UartTask(Message);
		
	
}


void LampCtl_FullReportStatus(void)
{
	DS1302_ReadTime();
	
	for(int i=0;i<LampCtl_Num;i++)
	{
		LampCtl_SendVDRFrame(i+1,1);
		
	}
	
}

void LampCtl_MultiReportStatus()
{
	if (LampCtl_Lamp[FLASHMid_Lamp_ID-1].Is_Blink)
	{
		if(LampCtl_Current_Blink_status==true)
		{
			DS1302_ReadTime();
						
			//LampCtl_SendVDRFrame(FLASHLeft_Lamp_ID,2);
			//LampCtl_SendVDRFrame(FLASHMid_Lamp_ID,2);
			//LampCtl_SendVDRFrame(FLASHRight_Lamp_ID,2);
			LampCtl_DetailReport_Required(FLASHLeft_Lamp_ID);
			LampCtl_DetailReport_Required(FLASHMid_Lamp_ID);
			LampCtl_DetailReport_Required(FLASHRight_Lamp_ID);
			
			Is_Multiple_Action=false;
		}
	}
	else
	{
			DS1302_ReadTime();
			//LampCtl_SendVDRFrame(FLASHLeft_Lamp_ID,1);
			//LampCtl_SendVDRFrame(FLASHMid_Lamp_ID,1);
			//LampCtl_SendVDRFrame(FLASHRight_Lamp_ID,1);
			LampCtl_DetailReport_Required(FLASHLeft_Lamp_ID);
			LampCtl_DetailReport_Required(FLASHMid_Lamp_ID);
			LampCtl_DetailReport_Required(FLASHRight_Lamp_ID);
			Is_Multiple_Action=false;
		
	}
	
}


void LampCtl_HandleAction(unsigned char CMD,unsigned char LampPos)  //CMD 0x00 is turn off 0x01 is turn on
{
	
	if(CMD==0x01)
	{
		LampCtl_Lamp[LampPos-1].Is_On=true;
		LampCtl_Is_Updated=true;
	
		//LEDCombo_TurnOn(LampCtl_Lamp[LampPos-1].LED_Indicate_ID);
				
	}
	else if(CMD==0x00)
	{

		LampCtl_Lamp[LampPos-1].Is_On=false;
		LampCtl_Is_Updated=true;
			
		//LEDCombo_TurnOff(LampCtl_Lamp[LampPos-1].LED_Indicate_ID);
			
			
	}
	else if(CMD==0x10)
	{
		switch(LampPos)
		{
			case 0x00:
			{
				LampCtl_Lamp[FLASHLeft_Lamp_ID-1].Is_On=false;
				LampCtl_Lamp[FLASHMid_Lamp_ID-1].Is_On=false;
				LampCtl_Lamp[FLASHRight_Lamp_ID-1].Is_On=false;
				Is_Multiple_Action=true;
				break;
				
				
			}
			case 0x01:
			{
				LampCtl_Lamp[FLASHLeft_Lamp_ID-1].Is_On=true;
				LampCtl_Lamp[FLASHMid_Lamp_ID-1].Is_On=false;
				LampCtl_Lamp[FLASHRight_Lamp_ID-1].Is_On=false;
				Is_Multiple_Action=true;
				break;
				
				
			}
			case 0x02:
			{
				LampCtl_Lamp[FLASHLeft_Lamp_ID -1].Is_On=false;
				LampCtl_Lamp[FLASHMid_Lamp_ID -1].Is_On=true;
				LampCtl_Lamp[FLASHRight_Lamp_ID -1].Is_On=false;
				Is_Multiple_Action=true;
				break;
				
				
			}
			case 0x03:
			{
				LampCtl_Lamp[FLASHLeft_Lamp_ID -1].Is_On=false;
				LampCtl_Lamp[FLASHMid_Lamp_ID-1].Is_On=false;
				LampCtl_Lamp[FLASHRight_Lamp_ID-1].Is_On=true;
				Is_Multiple_Action=true;
				break;
				
				
			}
			case 0x04:
			{
				LampCtl_Lamp[FLASHLeft_Lamp_ID-1].Is_On=true;
				LampCtl_Lamp[FLASHMid_Lamp_ID-1].Is_On=true;
				LampCtl_Lamp[FLASHRight_Lamp_ID-1].Is_On=false;
				Is_Multiple_Action=true;
				break;
				
				
			}
			case 0x05:
			{
				LampCtl_Lamp[FLASHLeft_Lamp_ID-1].Is_On=false;
				LampCtl_Lamp[FLASHMid_Lamp_ID-1].Is_On=true;
				LampCtl_Lamp[FLASHRight_Lamp_ID-1].Is_On=true;
				Is_Multiple_Action=true;
				break;
				
				
			}
		}
		
	}

}


void LampCtl_Reflash(void)
{
	unsigned char Mask_Bit=0x01;
		
	for(int i=0;i<LampCtl_Num;i++)
	{
		Mask_Bit=0x01;
		
		if(!LampCtl_Lamp[i].Is_Blink)
		{
			if(LampCtl_Lamp[i].Is_On)
			{
				Mask_Bit<<=(i%Lamp_Line_Num);
		
				LampArray_Frame[i/Lamp_Line_Num]|=Mask_Bit;
			}
			else
			{
				Mask_Bit<<=(i%Lamp_Line_Num);
	
				LampArray_Frame[i/Lamp_Line_Num]&=(~Mask_Bit);
			}
		}
		else 
		{
			if(LampCtl_Lamp[i].Is_On)
			{
				if(LampCtl_Is_BlinkOn)
				{
					Mask_Bit<<=(i%Lamp_Line_Num);
		
					LampArray_Frame[i/Lamp_Line_Num]|=Mask_Bit;
					
					if(!LEDCombo_GetStatus(LampCtl_Lamp[i].LED_Indicate_ID))
					
						LEDCombo_TurnOn(LampCtl_Lamp[i].LED_Indicate_ID);
				}
				else
				{
					Mask_Bit<<=(i%Lamp_Line_Num);
	
					LampArray_Frame[i/Lamp_Line_Num]&=(~Mask_Bit);
					
					//if(LEDCombo_GetStatus(LampCtl_Lamp[i].LED_Indicate_ID))
					 LEDCombo_TurnOff(LampCtl_Lamp[i].LED_Indicate_ID);
					
				}
					
				
			}
			else
			{
				Mask_Bit<<=(i%Lamp_Line_Num);
	
				LampArray_Frame[i/Lamp_Line_Num]&=(~Mask_Bit);
				
				//if(LEDCombo_GetStatus(LampCtl_Lamp[i].LED_Indicate_ID))
				LEDCombo_TurnOff(LampCtl_Lamp[i].LED_Indicate_ID);
					
				
			}
			
		}
			
	}
	
	LampCtl_Current_Blink_status=LampCtl_Is_BlinkOn;
	
	if(Counter_Blink>=3)
	{
		Counter_Blink=0;
		
		LampCtl_Is_BlinkOn=!LampCtl_Is_BlinkOn;
	}
	else
	{
		Counter_Blink++;
	}
	
	
	LampArray_Reflash();
	
	
	
	
}



void LampCtl_Run()
{
	static unsigned short Message_LampAction=0;
	static unsigned char Message_Cmd=0;
	static unsigned char Message_LampPos;
	
	unsigned char * P_Sensor;
	
	switch(LampCtl_Status)
	{
		case 0:                                    //pending turn on /off  lamp message
		{
			if (xQueueReceive(xQueue_LampAction,&Message_LampAction,0))
			{
				Message_Cmd=(unsigned char)((Message_LampAction&0xff00)>>8);
				Message_LampPos=(unsigned char)(Message_LampAction&0x00ff);
				LampCtl_HandleAction(Message_Cmd,Message_LampPos);
				
				//LampCtl_Is_Updated=true;
				LampCtl_Status=1;
				
			}
			else
			{
				
				LampCtl_Status=1;
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
			
			break;
		}
		case 1:                                     //Reflash lamparray
		{
		
			LampCtl_Reflash();
			
			vTaskDelay(100);
			
			LampCtl_Status=2;
		 				
			break;
		}
		case 2:										//Check Sensor
		{
			P_Sensor=CurrentSensor_GetData();
		 
			LampCtl_Check_Sensor(P_Sensor);
			
			if(LampCtl_Is_Updated)
			{
				LampCtl_Status=3;
				LampCtl_Is_Updated=false;
			}			
			else if(Is_Multiple_Action)
			{
				
				LampCtl_Status=5;
			
				
			}
			else if(Counter_Report>=10*10*4)
			{
				Counter_Report=0;
				LampCtl_Status=4;
			}
			
			else
			{
				Counter_Report++;
				LampCtl_Status=0;
			}
			break;
		}
		case 3:											//Single Report
		{
			LEDCombo_TurnOn(LEDCombo_COM2_ID);
			
			VDR_LED_ON=true;
			
			LampCtl_SingleReportStatus(Message_LampPos);
			
			if(Counter_Report>=10*10*4)
			{
				Counter_Report=0;
				LampCtl_Status=4;
			}
			else
			{
				Counter_Report++;
				LampCtl_Status=0;
			}
			
			break;
		}
		case 4:											//Full report
		{
			LEDCombo_TurnOn(LEDCombo_COM2_ID);
			VDR_LED_ON=true;
			
			//LampCtl_FullReportStatus();
			LampCtl_FullReport_Required();
			LampCtl_Status=0;
			break;
		}
		
		case 5:											//Full report
		{
			LEDCombo_TurnOn(LEDCombo_COM2_ID);
			VDR_LED_ON=true;
			
		
			LampCtl_MultiReportStatus();
			LampCtl_Status=0;
			break;
		}
	}
	
}

// verify FLASHRight_Indicate_LED_ID  bits with Lamp[i].Is_On , if OK ,Reset Is_Error, if fail ,Set Is_Error;
// Mirror bits layout In16 In17 In18 In19 In20 0 0 0   In11 In12 In13 In14 In15 0 0 0  In6 In7 In8 In9 In10 0 0 0   In1 In2 In3 In4 In5 0 0 0
// byte loc (i-1)/5       bit loc 8-(i-1)%5
// This function will be changed while light sensor driver being changed
//hardware V4
// verify FLASHRight_Indicate_LED_ID  bits with Lamp[i].Is_On , if OK ,Reset Is_Error, if fail ,Set Is_Error;
// Mirror bits layout In17 In18 In19 In20 0 0 0 0  In13 In14 In 15 In16 0 0 0 0 In9 In10 In11 In12 0 0 0 0   In5 In6 In7 In8 0 0 0 0 In1 In2 In3 In4 0 0 0 0
// byte loc (i-1)/4       bit loc 8-(i-1)%4


void LampCtl_Check_Sensor(unsigned char * Buf)
{
	unsigned char Bit_Mask=0x01;
	bool  OnStatus_Mirror=false;

	bool  Alarm;
	
	Alarm=false;
	
	for(int i=0;i<LampCtl_Num;i++)
	{
		Bit_Mask=(0x01<<(7-(i%Lamp_Line_Num)));
		
		OnStatus_Mirror=(Buf[i/Lamp_Line_Num]&Bit_Mask)==0?true:false;
		
		if(!LampCtl_Lamp[i].Is_Blink)
		{
			if ((LampCtl_Lamp[i].Is_On)==OnStatus_Mirror)
			{
				LampCtl_Lamp[i].Is_Error=false;
				LampCtl_Lamp[i].ErrorACK=false;
				LEDCombo_TurnOff(LampCtl_Lamp[i].LED_Warn_ID);
			}
			else
			{
				if(LampCtl_Lamp[i].ErrorACK)
				{
					LampCtl_Lamp[i].Is_Error=true;
					LEDCombo_SetBlink(LampCtl_Lamp[i].LED_Warn_ID,false);
					LEDCombo_TurnOn(LampCtl_Lamp[i].LED_Warn_ID);
					
					
					
					
				}
				else
				{
					LampCtl_Lamp[i].Is_Error=true;
					
					LEDCombo_SetBlink(LampCtl_Lamp[i].LED_Warn_ID,true);
					
					if (!LEDCombo_GetStatus(LampCtl_Lamp[i].LED_Warn_ID))
					{
						LEDCombo_TurnOn(LampCtl_Lamp[i].LED_Warn_ID);
					
					}
			
				}
								
			}
			
		}
		else
		{
			if ((LampCtl_Lamp[i].Is_On&LampCtl_Current_Blink_status)==OnStatus_Mirror)
			{
				
				if((LampCtl_Current_Blink_status&&LampCtl_Lamp[i].Is_On) ||(!LampCtl_Lamp[i].Is_On))
				{
					LampCtl_Lamp[i].ErrorACK=false;
					LampCtl_Lamp[i].Is_Error=false;
					LEDCombo_TurnOff(LampCtl_Lamp[i].LED_Warn_ID);
					
				}
				else if (!LampCtl_Current_Blink_status&&LampCtl_Lamp[i].Is_On)
				{
					if(LampCtl_Lamp[i].Is_Error&&LampCtl_Lamp[i].ErrorACK)
						LEDCombo_TurnOn(LampCtl_Lamp[i].LED_Warn_ID);
					else 
						LEDCombo_TurnOff(LampCtl_Lamp[i].LED_Warn_ID);
				}
				
			}
			
			
			else
			{
				
				
				if(((LampCtl_Current_Blink_status)&&(LampCtl_Lamp[i].Is_On))||(!LampCtl_Lamp[i].Is_On))
				{
					if(LampCtl_Lamp[i].ErrorACK)
					{
						LampCtl_Lamp[i].Is_Error=true;
						LEDCombo_SetBlink(LampCtl_Lamp[i].LED_Warn_ID,false);
						LEDCombo_TurnOn(LampCtl_Lamp[i].LED_Warn_ID);
						
					}
					else
					{
						LampCtl_Lamp[i].Is_Error=true;
						LEDCombo_SetBlink(LampCtl_Lamp[i].LED_Warn_ID,true);
						
						if (!LEDCombo_GetStatus(LampCtl_Lamp[i].LED_Warn_ID))
						{
							LEDCombo_TurnOn(LampCtl_Lamp[i].LED_Warn_ID);
					
						}
					}
				}
				
			}
		}
		
		if (LampCtl_Lamp[i].Is_Error&&(!LampCtl_Lamp[i].ErrorACK))
			Alarm=true;
			
	}
	
	if (Alarm) 
		Buzzer_SetAlarm();
	else 
		Buzzer_ClearAlarm();
	
	
}



void LampCtl_TurnOn(unsigned char LampPos)
{
	
	unsigned short LampAction_Message=0;
	
	LampAction_Message=Msg_LampOn;
    LampAction_Message<<=8;
    LampAction_Message|=LampPos;	
	
	
	if(LampCtl_Lamp[LampPos-1].IsEnable)
	{
		LEDCombo_TurnOn(LampCtl_Lamp[LampPos-1].LED_Indicate_ID);
		xQueueSend(xQueue_LampAction,&LampAction_Message,1);
		
	}

	
	
}



void LampCtl_TurnOff(unsigned char LampPos)
{
	unsigned short LampAction_Message=0;
	
	LampAction_Message=Msg_LampOff ;
    LampAction_Message<<=8;
    LampAction_Message|=LampPos;	
	
	
	if(LampCtl_Lamp[LampPos-1].IsEnable)
	{
		LEDCombo_TurnOff(LampCtl_Lamp[LampPos-1].LED_Indicate_ID);
		
		xQueueSend(xQueue_LampAction,&LampAction_Message,1);
		
	}
	

}


void LampCtl_SetFlash(unsigned char FlashStatus)
{
	unsigned short LampAction_Message=0;
	
	
	
	if((LampCtl_Lamp[FLASHMid_Lamp_ID-1].IsEnable)&&(LampCtl_Lamp[FLASHRight_Lamp_ID-1].IsEnable)&&(LampCtl_Lamp[FLASHLeft_Lamp_ID-1].IsEnable))
	{
		//LEDCombo_TurnOff(LampCtl_Lamp[LampPos-1].LED_Indicate_ID);
		
		LampAction_Message=Msg_FlashSet ;
		LampAction_Message<<=8;
		LampAction_Message|=FlashStatus;	
		
		
		switch(FlashLamp_Status)
		{
			case 0x00:
			{
				LEDCombo_TurnOff(FLASHLeft_Indicate_LED_ID);
				LEDCombo_TurnOff(FLASHMid_Indicate_LED_ID);
				LEDCombo_TurnOff(FLASHRight_Indicate_LED_ID);
				break;
			}
			case 0x01:
			{
				LEDCombo_TurnOn(FLASHLeft_Indicate_LED_ID);
				LEDCombo_TurnOff(FLASHMid_Indicate_LED_ID);
				LEDCombo_TurnOff(FLASHRight_Indicate_LED_ID);
				break;
			}
			case 0x02:
			{
				LEDCombo_TurnOff(FLASHLeft_Indicate_LED_ID);
				LEDCombo_TurnOn(FLASHMid_Indicate_LED_ID);
				LEDCombo_TurnOff(FLASHRight_Indicate_LED_ID);
				break;
			}
			case 0x03:
			{
				LEDCombo_TurnOff(FLASHLeft_Indicate_LED_ID);
				LEDCombo_TurnOff(FLASHMid_Indicate_LED_ID);
				LEDCombo_TurnOn(FLASHRight_Indicate_LED_ID);
				break;
			}
			case 0x04:
			{
				LEDCombo_TurnOn(FLASHLeft_Indicate_LED_ID);
				LEDCombo_TurnOn(FLASHMid_Indicate_LED_ID);
				LEDCombo_TurnOff(FLASHRight_Indicate_LED_ID);
				break;
			}
			case 0x05:
			{
				LEDCombo_TurnOff(FLASHLeft_Indicate_LED_ID);
				LEDCombo_TurnOn(FLASHMid_Indicate_LED_ID);
				LEDCombo_TurnOn(FLASHRight_Indicate_LED_ID);
				break;
			}
			
			
		}
		
		
		xQueueSend(xQueue_LampAction,&LampAction_Message,1);
		
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


void LampCtl_SaveEnv(void)
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
			LampCtl_TurnOff(FLASHLeft_Lamp_ID);
			LampCtl_TurnOff(FLASHMid_Lamp_ID);
			LampCtl_TurnOff(FLASHRight_Lamp_ID);
			LEDCombo_TurnOn(Auto_LED_ID );
			LEDCombo_TurnOff(Manual_LED_ID );
			break;
			
		}
		case 0x01:
		{
			LampCtl_TurnOn(FLASHLeft_Lamp_ID);
			LampCtl_TurnOff(FLASHRight_Lamp_ID);
			LampCtl_TurnOff(FLASHMid_Lamp_ID);
			LEDCombo_TurnOn(Auto_LED_ID);
			LEDCombo_TurnOff(Manual_LED_ID);
			break;
			
		}
		case 0x02:
		{
			LampCtl_TurnOff(FLASHLeft_Lamp_ID);
			LampCtl_TurnOff(FLASHRight_Lamp_ID);
			LampCtl_TurnOn(FLASHMid_Lamp_ID);
			LEDCombo_TurnOn(Auto_LED_ID);
			LEDCombo_TurnOff(Manual_LED_ID);
			break;
			
		}
		case 0x03:
		{
			LampCtl_TurnOff(FLASHLeft_Lamp_ID);
			LampCtl_TurnOn(FLASHRight_Lamp_ID);
			LampCtl_TurnOff(FLASHMid_Lamp_ID);
			LEDCombo_TurnOn(Auto_LED_ID);
			LEDCombo_TurnOff(Manual_LED_ID);
			break;
			
		}
		case 0x04:
		{
			LampCtl_TurnOn(FLASHLeft_Lamp_ID);
			LampCtl_TurnOff(FLASHRight_Lamp_ID);
			LampCtl_TurnOn(FLASHMid_Lamp_ID);
			LEDCombo_TurnOn(Auto_LED_ID);
			LEDCombo_TurnOff(Manual_LED_ID);
			break;
			
		}
		case 0x05:
		{
			LampCtl_TurnOff(FLASHLeft_Lamp_ID);
			LampCtl_TurnOn(FLASHRight_Lamp_ID);
			LampCtl_TurnOn(FLASHMid_Lamp_ID);
			LEDCombo_TurnOn(Auto_LED_ID);
			LEDCombo_TurnOff(Manual_LED_ID);
			break;
			
		}
		case 0x06:
		{
			LampCtl_TurnOff(FLASHLeft_Lamp_ID);
			LampCtl_TurnOff(FLASHRight_Lamp_ID);
			LampCtl_TurnOff(FLASHMid_Lamp_ID);
			
			LEDCombo_TurnOn(Manual_LED_ID);
			LEDCombo_TurnOff(Auto_LED_ID);
			
			LEDCombo_TurnOff(FLASHLeft_Indicate_LED_ID);
			LEDCombo_TurnOff(FLASHRight_Indicate_LED_ID );
			LEDCombo_TurnOff(FLASHMid_Indicate_LED_ID);
			break;
			
		}
		case 0x07:
		{
			LampCtl_TurnOff(FLASHLeft_Lamp_ID);
			LampCtl_TurnOff(FLASHRight_Lamp_ID);
			LampCtl_TurnOff(FLASHMid_Lamp_ID);
			
			LEDCombo_TurnOn(Manual_LED_ID);
			LEDCombo_TurnOff(Auto_LED_ID);
			
			LEDCombo_TurnOn(FLASHLeft_Indicate_LED_ID);
			LEDCombo_TurnOff(FLASHRight_Indicate_LED_ID );
			LEDCombo_TurnOff(FLASHMid_Indicate_LED_ID);
			break;
			
		}
		case 0x08:
		{
			LampCtl_TurnOff(FLASHLeft_Lamp_ID);
			LampCtl_TurnOff(FLASHRight_Lamp_ID);
			LampCtl_TurnOff(FLASHMid_Lamp_ID);
			
			LEDCombo_TurnOn(Manual_LED_ID);
			LEDCombo_TurnOff(Auto_LED_ID);
			
			LEDCombo_TurnOff(FLASHLeft_Indicate_LED_ID);
			LEDCombo_TurnOff(FLASHRight_Indicate_LED_ID );
			LEDCombo_TurnOn(FLASHMid_Indicate_LED_ID);
			break;
			
		}
		case 0x09:
		{
			LampCtl_TurnOff(FLASHLeft_Lamp_ID);
			LampCtl_TurnOff(FLASHRight_Lamp_ID);
			LampCtl_TurnOff(FLASHMid_Lamp_ID);
			
			LEDCombo_TurnOn(Manual_LED_ID);
			LEDCombo_TurnOff(Auto_LED_ID);
			
			LEDCombo_TurnOff(FLASHLeft_Indicate_LED_ID);
			LEDCombo_TurnOn(FLASHRight_Indicate_LED_ID );
			LEDCombo_TurnOff(FLASHMid_Indicate_LED_ID);
			break;
			
		}
		case 0x10:
		{
			LampCtl_TurnOff(FLASHLeft_Lamp_ID);
			LampCtl_TurnOff(FLASHRight_Lamp_ID);
			LampCtl_TurnOff(FLASHMid_Lamp_ID);
			
			LEDCombo_TurnOn(Manual_LED_ID);
			LEDCombo_TurnOff(Auto_LED_ID);
			
			LEDCombo_TurnOn(FLASHLeft_Indicate_LED_ID);
			LEDCombo_TurnOff(FLASHRight_Indicate_LED_ID );
			LEDCombo_TurnOn(FLASHMid_Indicate_LED_ID);
			break;
			
		}
		case 0x11:
		{
			LampCtl_TurnOff(FLASHLeft_Lamp_ID);
			LampCtl_TurnOff(FLASHRight_Lamp_ID);
			LampCtl_TurnOff(FLASHMid_Lamp_ID);
			
			LEDCombo_TurnOn(Manual_LED_ID);
			LEDCombo_TurnOff(Auto_LED_ID);
			
			LEDCombo_TurnOff(FLASHLeft_Indicate_LED_ID);
			LEDCombo_TurnOn(FLASHRight_Indicate_LED_ID );
			LEDCombo_TurnOn(FLASHMid_Indicate_LED_ID);
			break;
			
		}
		case 0x12:  											//will not excute  because env status was not be saved the enter diandong status 
		{
			LampCtl_TurnOff(FLASHLeft_Lamp_ID);
			LampCtl_TurnOff(FLASHRight_Lamp_ID);
			LampCtl_TurnOff(FLASHMid_Lamp_ID);
			
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


void LampCtl_FlashLight_Manual(void)
{
	LampCtl_Lamp[FLASHLeft_Lamp_ID-1].Is_Blink=false;
	LampCtl_Lamp[FLASHMid_Lamp_ID-1].Is_Blink=false;
	LampCtl_Lamp[FLASHRight_Lamp_ID-1].Is_Blink=false;
}

void LampCtl_FlashLight_Auto(void)
{
	LampCtl_Lamp[FLASHLeft_Lamp_ID-1].Is_Blink=true;
	LampCtl_Lamp[FLASHMid_Lamp_ID-1].Is_Blink=true;
	LampCtl_Lamp[FLASHRight_Lamp_ID-1].Is_Blink=true;
}

void LampCtl_SetLedCombo_Color(unsigned char * pColor)
{
	for(int i=0;i<LampCtl_Num;i++,pColor++)
	{
		LEDCombo_ConfigColor(LampCtl_Lamp[i].LED_Indicate_ID,*pColor);
		
	}
	
	
}

















