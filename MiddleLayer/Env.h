#ifndef ENV_Unit
#define ENV_Unit

#include "stdbool.h"

#define ENV_LampNum  20

#define ENV_LEDCombo_Ctl_Num 22

#define ENV_LampCompressed_Num  ((ENV_LampNum%8==0)?(ENV_LampNum/8):(ENV_LampNum/8+1))

#define ENV_LEDComboCompressed_Ctl_Num ((ENV_LEDCombo_Ctl_Num%8==0)?(ENV_LEDCombo_Ctl_Num/8):(ENV_LEDCombo_Ctl_Num/8+1))

#define ENV_Board_ID_Offset 0

#define ENV_Board_Type_Offset 1

#define ENV_Lamp_OnStatus_Offset 2

#define ENV_Lamp_Enable_Offset 5

#define ENV_Key_FlashStatus_Offset 8

#define ENV_LEDCombo_Color_Offset 10




extern unsigned char ENV_Board_ID;                    // 1 byte  in 24c02

extern unsigned char ENV_Board_Type;                  // 1 byte  in 24c02

static bool ENV_Lamp_OnStatus[ENV_LampNum];

extern bool ENV_Lamp_Enable[ENV_LampNum];

extern unsigned char ENV_LEDCombo_Index[ENV_LEDCombo_Ctl_Num];	

extern unsigned char ENV_LEDCombo_Color[ENV_LEDCombo_Ctl_Num];				



static unsigned char ENV_Lamp_OnStatus_Compressed[ENV_LampCompressed_Num];   	//3  bytes in 24c02

static unsigned char  ENV_Lamp_Enable_Compressed[ENV_LampCompressed_Num];		//3  bytes in 24c02

extern unsigned short ENV_Key_FlashStatus;											//2 bytes in 24C02



extern void ENV_Config_Board(void);  //before send to user, config boardid/boardtype/onstatus/enable/ledcolor/flash_status

extern void ENV_Save_BoardID(unsigned char boardid);
extern unsigned char ENV_Load_BoardID(void);


extern void ENV_Save_Board_Type(unsigned char boardtype);
extern unsigned char ENV_Load_BoardType(void);


extern void ENV_Save_Lamp_OnStatus(bool * OnStatus);  //save Lamp On status when power down flag,
extern bool * ENV_Load_Lamp_OnStatus(void);  //Load Lamp On Status when reset.
extern void ENV_Save_Lamp_OnStatus_Compressed(unsigned char * pOnstatus);


extern  void ENV_Save_Lamp_Enable(bool * Lamp_Enable);
extern  bool * ENV_Load_Lamp_Enable(void);
extern  void ENV_Save_Lamp_Enable_Compressed(unsigned char * pEnable);


extern void ENV_Save_LEDCombo_Color(unsigned char * Color);
extern unsigned char * ENV_Load_LEDCombo_Color(void);

extern void ENV_Save_Key_FlashStatus(unsigned short status);
extern unsigned short ENV_Load_Key_FlashStatus(void);









#endif
