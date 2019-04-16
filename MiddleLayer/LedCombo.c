#include "LedCombo.h"
#include "FreeRTOS.h"
#include "task.h"
#include "LedArray.h"
#include "ControllerStatus.h"
#include "M24C02.h"

#define  LEDCombo_BlinkFreq 50


//static unsigned char  LEDCombo_ColorName[LEDCombo_ColorNum][5]
//	={"RED","GREEN","BLUE","YELLO","PURLE","CYAN","WHITE","BLACK"};
	
static unsigned char  LEDCombo_REDVector=0x9a;  		//0b10011010
static unsigned char  LEDCombo_GREENVector=0x56;    	//0b01010110
static unsigned char  LEDCombo_BLUEVector=0x2e;	        //0b00101110
	
	

static unsigned char   LEDCombo_Layout[LEDCombo_Num][3]
	={
		{1, 2, 3 },	{4,5,6  },{7,8,9},{10,11,12},		{13,14,15},		{16,17,18},
		{19},				{20},			{21},		{22},					{23},					{24,25},

	};
	  
	  



static unsigned char  LEDCombo_ColorConfig[LEDCombo_Num]
	 ={0x00,0x00,0x00,0x06,0x00,0x01,0x06,0x01,0x01,0x01,0x01,0x01
	  
	  };

static bool LEDCombo_IsBlink[LEDCombo_Num]
	={
	
	false,false ,false,false,false,false,false,false,false,false,false,false
	
	
	};
	



static bool  LEDCombo_OnStatus[LEDCombo_Num]
	={
		false,false,false,false,false,false,false,false,false,false,false,false
	 };
	
	 
unsigned char LEDCombo_LightLevel=0;
	

		
	  
//static unsigned short LEDCombo_Buf[Array_Size];	 

static bool LEDCombo_Is_BlinkOn=true;
 
	  
void LEDCombo_SetBit(unsigned char BitData,int Offset)  //BitData= 1:  set 0 to light led on 
{
	int Array_Index=((Offset%16==0)?(Offset/16-1):(Offset/16));
	unsigned short  Byte_Mask=((Offset%16==0)?0x8000:(0x8000>>(16-Offset%16)));
	
	if (BitData==0)  LedArray_FrameBuffer[Array_Index]=LedArray_FrameBuffer[Array_Index]|Byte_Mask;
	 else LedArray_FrameBuffer[Array_Index]=LedArray_FrameBuffer[Array_Index]&(~Byte_Mask);
	 
	
		
}	
	  
void LEDCombo_SetupColor(int Comb_Offset,int Color)  //Combo_Offset :1~12, Color:0x00~0x07
{
	
	
	unsigned char  ColorMask=0x80;

	
	// Check led numbers in according combo 
	
	int length=0;
	
	for(int i=0;i<3;i++)
	{
		if (LEDCombo_Layout[Comb_Offset-1][i]!=0)  
		{
			length++;
		}
	}
	
	
	//Set bit on LedArray Buffer
	
	if (length==1)
	{
		//set only one led light on according ledArray buff location  
		
		if (Color!=7)  LEDCombo_SetBit(0x01,LEDCombo_Layout[Comb_Offset-1][0]);
		else LEDCombo_SetBit(0x00,LEDCombo_Layout[Comb_Offset-1][0]);
		
	}
    else
	{
		for(int i=0;i<Color;i++)
	        ColorMask=ColorMask>>1;
		
		if (LEDCombo_Layout[Comb_Offset-1][0]!=0) 
		  LEDCombo_SetBit(LEDCombo_REDVector&ColorMask,LEDCombo_Layout[Comb_Offset-1][0]);
		if (LEDCombo_Layout[Comb_Offset-1][1]!=0) 
		  LEDCombo_SetBit(LEDCombo_GREENVector&ColorMask,LEDCombo_Layout[Comb_Offset-1][1]);
	  	if (LEDCombo_Layout[Comb_Offset-1][2]!=0) 
		  LEDCombo_SetBit(LEDCombo_BLUEVector&ColorMask,LEDCombo_Layout[Comb_Offset-1][2]);
		
	}

}


void LEDCombo_ConfigColor(int Combo_Offset,unsigned char Color)
{
	LEDCombo_ColorConfig[Combo_Offset-1]=Color;
	
}


void LEDCombo_TurnOn(int Comb_Offset)
{
	
	LEDCombo_SetupColor(Comb_Offset,LEDCombo_ColorConfig[Comb_Offset-1]);
	LEDCombo_SetStatus(Comb_Offset,true);
	//vTaskDelay(1);  // for check crash
		
}

void LEDCombo_TurnOff(int Comb_Offset)
{
	LEDCombo_SetupColor(Comb_Offset,0x07);
	LEDCombo_SetStatus(Comb_Offset,false);
		
}
	
bool LEDCombo_GetStatus(int Comb_Offset)
{
	return LEDCombo_OnStatus[Comb_Offset-1];
}

void LEDCombo_SetStatus(int Comb_Offset,bool Status)
{
	
	LEDCombo_OnStatus[Comb_Offset-1]=Status;
	
}
	
void LEDCombo_RestoreDefault(void)
{
	for(int i=0;i<LEDCombo_Num;i++)
	{
		if(LEDCombo_OnStatus[i])
			LEDCombo_TurnOn(i+1);
		else LEDCombo_TurnOff(i+1);
	}
}

void LEDCombo_Init(void)
{
	LedArray_Init();
	LEDCombo_RestoreDefault();
	
	LEDCombo_InitCurrentLightlevel();
	
	LEDCombo_SetLight(LEDCombo_LightLevel);
}
	
	
void LEDCombo_SetBlink(unsigned char Pos, bool Value)
{
	LEDCombo_IsBlink[Pos-1]=Value;
	
}



void LEDCombo_Toggle(unsigned char Pos)
{
		
	if((LEDCombo_Is_BlinkOn)&&(LEDCombo_OnStatus[(Pos-1)]))
		LEDCombo_SetupColor(Pos,LEDCombo_ColorConfig[Pos-1]);
	else 
		LEDCombo_SetupColor(Pos,0x07 );
		
}


void LEDCombo_Scan_Toggle(void)
{
	for(int i=0;i<LEDCombo_Num;i++)
	{
		if (LEDCombo_IsBlink[i])
			LEDCombo_Toggle(i+1);
				
	}
	LEDCombo_Is_BlinkOn=!LEDCombo_Is_BlinkOn;
		
}



void LEDCombo_Reflash(void)
{
static int counter=0;
	
	if (counter>=LEDCombo_BlinkFreq)
	{
		LEDCombo_Scan_Toggle();
		counter=0;
	}
	else
		 counter++;
		
		
	LedArray_ReflashFrame();
	
	
}
	
	
	


void LEDCombo_SetLight(unsigned char level)
{
	LedArray_SetBackLight(level);
	LEDCombo_LightLevel=level;
	LEDCombo_SaveCurrentLightlevel();
	
}

unsigned char LEDCombo_GetLight()
{
	return LEDCombo_LightLevel;
}


void	LEDCombo_EnterTestMode()
{
	
	for( int i=1;i<=LEDCombo_Num;i++)
	{
		LEDCombo_SetBlink(i,false);
		LEDCombo_TurnOn(i);
	}
	
}

void	LEDCombo_EnterNormalMode()
{
	for( int i=1;i<=LEDCombo_Num;i++)
	{
		
		LEDCombo_TurnOff(i);
	}
	Status_Update();
}





void LEDCombo_SaveCurrentLightlevel()
{
	
	M24C02_WriteOneByte(LEDCOmbo_LightLevel_Address,LEDCombo_LightLevel);

}

void LEDCombo_InitCurrentLightlevel(void)
{
	LEDCombo_LightLevel=M24C02_ReadOneByte(LEDCOmbo_LightLevel_Address);
	
	if (LEDCombo_LightLevel>=4) 
		
		LEDCombo_LightLevel=0;
	

}


