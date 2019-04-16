#ifndef Keypad_Unit
#define Keypad_Unit

#include "stdbool.h"

#define Keypad_Num  7 


#define   KEYMode  					7
#define   KEYDIM	   				6
#define   KEYTest  					5
#define   KEYDianDong     	4
#define   KEYRight     			3
#define   KEYMid    				2
#define   KEYLeft     			1


#define   Morse_Release_Key 0x66


static unsigned short Keypad_Logic[Keypad_Num];

static bool Keypad_IsValid;
static unsigned char Keypad_KeyVal;

  
static void Keypad_GPIOConfig(void);
static unsigned char Keypad_ScanKey(void); 
           	
				
				
				
extern unsigned char Keypad_ReadKey(void);     								 //scan key and make Keypad_IsValid true, save key number in Keypad_KeyValval
extern void Keypad_Init(void);




#endif
