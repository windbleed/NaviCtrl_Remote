#include "ControllerStatus.h"
#include "LEDCombo.h"


bool Status_Left_ON=false;
bool Status_Left_Blink=false;
bool Status_LeftERR_ON=false;
bool Status_LeftERR_Blink=false;
bool Status_Mid_ON=false;
bool Status_Mid_Blink=false;
bool Status_MidERR_ON=false;
bool Status_MidERR_Blink=false;
bool Status_Right_ON=false;
bool Status_Right_Blink=false;
bool Status_RightERR_ON=false;
bool Status_RightERR_Blink=false;
bool Status_MODE_AUTO=false;

static void Status_SetLED(unsigned char pos, bool ON, bool Blink)
{
	if (Blink)
		 LEDCombo_SetBlink(pos,true);
	else
		 LEDCombo_SetBlink(pos,false);
	
	if (ON)
			LEDCombo_TurnOn(pos);
	else
			LEDCombo_TurnOff(pos);
}




void Status_Set_Mode(bool ModeAuto)
{
	if (ModeAuto)
	{
		Status_SetLED(LEDCombo_Auto,true,false);
		Status_SetLED(LEDCombo_Manual,false,false);
		Status_Left_Blink=true;
		Status_Mid_Blink=true;
		Status_Right_Blink=true;
	}
	else
	{
		Status_SetLED(LEDCombo_Auto,false,false);
		Status_SetLED(LEDCombo_Manual,true,false);
		Status_Left_Blink=false;
		Status_Mid_Blink=false;
		Status_Right_Blink=false;
	}
	
}

void Status_Update()
{
	Status_Set_Mode(Status_MODE_AUTO);
	Status_SetLED(LEDCombo_Left,Status_Left_ON,Status_Left_Blink);
	Status_SetLED(LEDCombo_LeftError,Status_LeftERR_ON,Status_LeftERR_Blink);
	Status_SetLED(LEDCombo_Right,Status_Right_ON,Status_Right_Blink);
	Status_SetLED(LEDCombo_RightError,Status_RightERR_ON,Status_RightERR_Blink);
	Status_SetLED(LEDCombo_Mid,Status_Mid_ON,Status_Mid_Blink);
	Status_SetLED(LEDCombo_MidError,Status_MidERR_ON,Status_MidERR_Blink);
	
}

