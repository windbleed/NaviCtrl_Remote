#ifndef LED_Combo_Unit
#define LED_Combo_Unit
#include "stdbool.h"

#define  LEDCombo_ColorNum 8
#define  LEDCombo_Num  12
#define  LEDCombo_Status_Compressed_Num   ((LEDCombo_Num%8==0)?(LEDCombo_Num/8):(LEDCombo_Num/8+1))

#define LEDCombo_LeftError		1
#define LEDCombo_Left		    2
#define LEDCombo_MidError		3
#define LEDCombo_Mid		    4
#define LEDCombo_RightError		5
#define LEDCombo_Right		    6
#define LEDCombo_DianDong       7
#define LEDCombo_Test			8
#define LEDCombo_Dimmer			9

#define LEDCombo_Manual			10
#define LEDCombo_Auto			11
#define LEDCombo_COM_ID 		12

#define LEDCOmbo_LightLevel_Address	  0x01


static unsigned char  LEDCombo_REDVector;
static unsigned char  LEDCombo_GREENVector;
static unsigned char  LEDCombo_BlueVector;

static unsigned char  LEDCombo_ColorName[LEDCombo_ColorNum][5];

static unsigned char  LEDCombo_Layout[LEDCombo_Num][3];

static bool  LEDCombo_OnStatus[LEDCombo_Num];

static bool  LEDCombo_OnStatus_BackUp[LEDCombo_Num];

static bool LEDCombo_IsBlink[LEDCombo_Num];






static unsigned char  LEDCombo_ColorConfig[LEDCombo_Num];


static void LEDCombo_SetupColor(int Comb_Offset,int Color);


extern unsigned char Status_Table_Compressed[LEDCombo_Status_Compressed_Num];


extern unsigned char LEDCombo_LightLevel;


static void LEDCombo_RestoreDefault(void);



extern void LEDCombo_Init(void);
extern void LEDCombo_ConfigColor(int Combo_Offset,unsigned char Color);
extern void LEDCombo_TurnOn(int Comb_Offset);
extern void LEDCombo_TurnOff(int Comb_Offset);


extern void LEDCombo_SaveStatusToEEPROM(void);
extern void LEDCombo_GetStatusFromEEPROM(void);

extern bool LEDCombo_GetStatus(int Comb_Offset);
extern void LEDCombo_SetStatus(int Comb_Offset,bool Status);
extern void LEDCombo_SetBlink(unsigned char Pos, bool Value);

extern void LEDCombo_Reflash(void);
extern void LEDCombo_SetLight(unsigned char level);
extern unsigned char LEDCombo_GetLight(void);

extern void LEDCombo_SaveCurrent_OnStatus(void);

extern void LEDCombo_LoadCurrent_Onstatus(void);

extern void LEDCombo_LightOn_NoneLampRelative(void);

extern void	LEDCombo_EnterTestMode(void);

extern void	LEDCombo_EnterNormalMode(void);


extern void LEDCombo_SaveCurrentLightlevel(void);

extern void LEDCombo_InitCurrentLightlevel(void);



#endif
