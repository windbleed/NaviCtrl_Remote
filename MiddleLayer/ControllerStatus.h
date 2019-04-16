#ifndef ControllerStatus_Unit
#define ControllerStatus_Unit

#include "stdbool.h"


extern bool Status_Left_ON;
extern bool Status_Left_Blink;
extern bool Status_LeftERR_ON;
extern bool Status_LeftERR_Blink;
extern bool Status_Mid_ON;
extern bool Status_Mid_Blink;
extern bool Status_MidERR_ON;
extern bool Status_MidERR_Blink;
extern bool Status_Right_ON;
extern bool Status_Right_Blink;
extern bool Status_RightERR_ON;
extern bool Status_RightERR_Blink;
extern bool Status_MODE_AUTO;
	

extern void Status_Set_Mode(bool ModeAuto);

extern void Status_Update(void);

static void Status_SetLED(unsigned char pos, bool ON, bool Blink);







#endif
