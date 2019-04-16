#ifndef VDRCoder_Unit
#define VDRCoder_Unit
#include "stdbool.h"

#define  Lamp_ON_Normal         "ON "
#define  Lamp_Off_Normal        "OFF"
#define  Lamp_Error             "ERR"

#define  BoardID                0x01
#define  VDRCoder_Frame_Len     (33)


typedef struct VDR_Frame
{
	unsigned char Start_Symbol;						//'$'
	unsigned char TalkerID[2];                    	//Company ID "ZH"
	unsigned char CMD[3];                  		 	//"ALR"
	unsigned char Separator_1;  				  	//','
	unsigned char TIMER[9];                       	//"hhmmss.ss" last ss=00
	unsigned char Separator_2;  					//','
	unsigned char UAN[3];                         	//Unique Alarm Number 000~999 BCD type 
	unsigned char Separator_3;  					//','
	unsigned char Condition;                      	// 'A'-- threshold exceeded  'V'--not exceeded
	unsigned char Separator_4;  
	unsigned char ACK;							  	// 'A' -- acknowledeged      'V'--not acknowledeged
	unsigned char Separator_5; 
	unsigned char Description[3];                 	// 'ON '/'OFF'/'ERR'
	unsigned char End_Symbol;          				//'*'
	unsigned char CheckSum[2];                         //Xor bytes from start to end symbol  but not include .
	unsigned char Char_Return;                      //0x0d
	unsigned char Char_LineFeed;                     //0x0a
		
}VDR_Frame;


extern void VDRCoder_Set_Protype(void);
extern void VDRCoder_Set_Time(unsigned char tm[6]);
extern void VDRCoder_Set_UAN(unsigned char Boardid,unsigned char LampPos);
extern void VDRCoder_Set_Status_ACK(unsigned char status,bool ACK);     //status = 0:lamp off OK /1: lamp On OK/2:lamp error
extern void VDRCoder_Set_CheckSum(void);

extern void VDRCoder_SendMessage(unsigned char COM);






#endif
