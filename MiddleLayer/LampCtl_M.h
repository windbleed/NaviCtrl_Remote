#ifndef LampCtl_Unit
#define LampCtl_Unit



#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "stdbool.h"

#define Navigation_B   0x01
#define SIG_B          0x02

#define BoardType        SIG_B                      // This is board configuration.

#define LampCtl_Max    20

#define LampCtl_Num   ((BoardType==Navigation_B)?20:20)


#define Auto_LED_ID   					45
#define Manual_LED_ID  					46
#define DIMMER_LED_ID   				51
#define MUTE_LED_ID     				52
#define TEST_LED_ID    					53
#define DD_LED_ID						54

#define FLASHLeft_Indicate_LED_ID   	15
#define FlASHLeft_Warn_LED_ID           36
#define FLASHMid_Indicate_LED_ID        49
#define FLASHMid_Warn_LED_ID			50
#define FLASHRight_Indicate_LED_ID      20
#define FLASHRight_Warn_LED_ID			41
#define MORSE_Indicate_LED_ID  			21
#define MORSE_Warn_LED_ID				42

#define MORSE_Lamp_ID					1
#define FLASHLeft_Lamp_ID               2
#define FLASHMid_Lamp_ID 				3
#define FLASHRight_Lamp_ID              4

#define Msg_LampOff                    0x00
#define Msg_LampOn                     0x01
#define Msg_FlashSet				   0x10

#define Lamp_Line_Num                   5




typedef struct lamp
{
	unsigned char Lamp_ID;                    //(1~20)
	unsigned char Switch_ID;                  //(1~27)
	unsigned char LED_Indicate_ID;            //(1~54)
	unsigned char LED_Warn_ID;                //(1~54)
	bool          Is_On;
	bool          Is_Blink;
	bool          Is_Morse;
	bool          Is_Error;	
	bool          ErrorACK;
	bool          IsEnable;
	
}Lamp;






extern xQueueHandle xQueue_LampAction;


extern Lamp LampCtl_Lamp[LampCtl_Max];


extern void LampCtl_Init(void);

extern void LampCtl_Run(void);

static void LampCtl_Reflash(void);

static void LampCtl_Check_Sensor(unsigned char * Buf);

static void LampCtl_SingleReportStatus(unsigned char LampPos);

extern  void LampCtl_FullReportStatus(void);

static void LampCtl_FullReport_Required(void);


extern void LampCtl_TurnOn(unsigned char LampPos);

extern void LampCtl_TurnOff(unsigned char LampPos);

extern void LampCtl_SetFlash(unsigned char FlashStatus);

extern unsigned char LampCtl_SearchLampByKey(unsigned char Keyvalue);

extern void LampCtl_SetErrorACK(void);

extern void LampCtl_SetSingleErrorACK(unsigned char LampPos);



extern void LampCtl_SaveEnv(void);

static void LampCtl_Load_FlashStatus(void);

static void LampCtl_Save_FlashStatus(void);

static void LampCtl_SetLampStatus(void);

static void LampCtl_SendVDRFrame(unsigned char Pos,unsigned char COM);

extern void LampCtl_Detail_Report(unsigned char LampPos,bool LampOn,bool LampError,bool ErrorACK);

static void LampCtl_DetailReport_Required(unsigned char LampPos);

extern  void LampCtl_FlashLight_Manual(void);

extern  void LampCtl_FlashLight_Auto(void);

extern  void LampCtl_SetLedCombo_Color(unsigned char * pColor);

extern void LampCtl_NormalMode(void);

extern void LampCtl_TestMode(void);

#endif
