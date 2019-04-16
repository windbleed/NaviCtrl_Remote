#ifndef Keypad_Unit
#define Keypad_Unit

#include "stdbool.h"

#define Keypad_Num  27 

#define Keypad_ArrayNum  ((Keypad_Num%8==0)?Keypad_Num/8:Keypad_Num/8+1)

#define   AUTO		27  
#define   DIMMER    26
#define   INCHING   25
#define   TEST      24
#define   MUTE      23
#define   KEYFLASH  22
#define   KEY20     21
#define   KEY19     20
#define   KEY18     19
#define   KEY17     18
#define   KEY16     17
#define   KEY15     16
#define   KEY14     15
#define   KEY13     14
#define   KEY12     13
#define   KEY11     12
#define   MORSE     11
#define   KEY10     10
#define   KEY9     	9
#define   KEY8     	8
#define   KEY7     	7
#define   KEY6     	6
#define   KEY5     	5
#define   KEY4     	4
#define   KEY3     	3
#define   KEY2     	2
#define   KEY1     	1


static unsigned char Keypad_Logic[Keypad_Num];
static unsigned char Keypad_Array[Keypad_ArrayNum];
static bool Keypad_IsValid;
static unsigned char Keypad_KeyVal;

static unsigned char  Keypad_ConvertKeyVal(void);   
static void Keypad_GPIOConfig(void);
static unsigned char Keypad_ScanKey(void); 
             	
				
				
				
extern unsigned char Keypad_ReadKey(void);     								 //scan key and make Keypad_IsValid true, save key number in Keypad_KeyValval
extern void Keypad_Init(void);




#endif
