#include "Env.h"
#include "M24C02.h"
#include "LampCtl_M.h"


unsigned char ENV_Board_ID;                    // 1 byte  in 24c02

unsigned char ENV_Board_Type;                  // 1 byte  in 24c02

static bool ENV_Lamp_OnStatus[ENV_LampNum];

bool ENV_Lamp_Enable[ENV_LampNum];

unsigned char ENV_LEDCombo_Index[ENV_LEDCombo_Ctl_Num];	

unsigned char ENV_LEDCombo_Color[ENV_LEDCombo_Ctl_Num];				

unsigned short ENV_Key_FlashStatus;

static unsigned char ENV_Lamp_OnStatus_Compressed[ENV_LampCompressed_Num];   	//3  bytes in 24c02

static unsigned char  ENV_Lamp_Enable_Compressed[ENV_LampCompressed_Num];		//3  bytes in 24c02


void ENV_Config_Board(void)  //before send to user, config boardid/boardtype/onstatus/enable/ledcolor
{
	ENV_Load_BoardID();
	ENV_Load_BoardType();
	
}

extern void ENV_Save_BoardID(unsigned char boardid)
{
	M24C02_WriteOneByte(ENV_Board_ID_Offset,boardid);
}

extern unsigned char ENV_Load_BoardID(void)
{
	ENV_Board_ID=M24C02_ReadOneByte(ENV_Board_ID_Offset);
	return ENV_Board_ID;
}


extern void ENV_Save_Board_Type(unsigned char boardtype)
{
	M24C02_WriteOneByte(ENV_Board_Type_Offset,boardtype);
}

extern unsigned char ENV_Load_BoardType(void)
{
	ENV_Board_Type=M24C02_ReadOneByte(ENV_Board_Type_Offset);
	return ENV_Board_Type;
}


extern void ENV_Save_Lamp_OnStatus(bool * OnStatus)  //save Lamp On status when power down flag,
{
	unsigned char BitMask=0x01;
	unsigned char BitShiftNum=0;
	
	
	//Convert to compressed status table for less number byte save to eeprom
	
	for(int i=0;i<ENV_LampCompressed_Num ;i++)
	{
		BitShiftNum=((i<2)?8:(ENV_LampNum %8));
				
		for(int BitPos=0;BitPos<BitShiftNum;BitPos++)
		{
			
			if(OnStatus[i*8+BitPos])
			ENV_Lamp_OnStatus_Compressed[i]|=BitMask;
			else ENV_Lamp_OnStatus_Compressed[i]&=~BitMask;
			
			
			BitMask=((BitPos==7)?0x01:BitMask<<1);
						
		}
		M24C02_WriteOneByte(i+ENV_Lamp_OnStatus_Offset,ENV_Lamp_OnStatus_Compressed[i]);
		
	
	}
	
}

extern void ENV_Save_Lamp_OnStatus_Compressed(unsigned char * pOnstatus)
{
	for(int i=0;i<ENV_LampCompressed_Num ;i++)
	{
		
		M24C02_WriteOneByte(i+ENV_Lamp_OnStatus_Offset,pOnstatus[i]);
			
	}
	
}


extern bool * ENV_Load_Lamp_OnStatus(void)  //Load Lamp On Status when reset. BYTE 0 BIT 0 IS CHANNEL 1: BYTE 0 BIT 7  CHANNEL 8
{										    //BYTE 1 BIT 0 IS CH9 :BYTE2 BIT 0 CH 17 :BYTE2 BIT 3  IS CH20
	unsigned char BitMask=0x01;
	unsigned char BitShiftNum=0;
	
	for(int i=0;i<ENV_LampCompressed_Num;i++)
	{
		
		BitShiftNum=((i<2)?8:(ENV_LampNum %8));
		
		ENV_Lamp_OnStatus_Compressed[i]=M24C02_ReadOneByte(i+ENV_Lamp_OnStatus_Offset);
		
		for(int BitPos=0;BitPos<BitShiftNum;BitPos++)
		{
			if((ENV_Lamp_OnStatus_Compressed[i]&BitMask)!=0)
				ENV_Lamp_OnStatus[8*i+BitPos]=true;
			else
				ENV_Lamp_OnStatus[8*i+BitPos]=false;
			
			BitMask=((BitPos==7)?0x01:BitMask<<1);
			
		}
	}	
	
	return ENV_Lamp_OnStatus;
}









extern  void ENV_Save_Lamp_Enable(bool * Lamp_Enable)
{
	unsigned char BitMask=0x01;
	unsigned char BitShiftNum=0;
	
	
	//Convert to compressed status table for less number byte save to eeprom
	
	for(int i=0;i<ENV_LampCompressed_Num ;i++)
	{
		BitShiftNum=((i<2)?8:(ENV_LampNum %8));
				
		for(int BitPos=0;BitPos<BitShiftNum;BitPos++)
		{
			
			if(Lamp_Enable[i*8+BitPos])
			ENV_Lamp_Enable_Compressed[i]|=BitMask;
			else ENV_Lamp_Enable_Compressed[i]&=~BitMask;
			
			
			BitMask=((BitPos==7)?0x01:BitMask<<1);
						
		}
		M24C02_WriteOneByte(i+ENV_Lamp_Enable_Offset,ENV_Lamp_Enable_Compressed[i]);
		
	
	}
	
}

extern  void ENV_Save_Lamp_Enable_Compressed(unsigned char * pEnable)
{
	for(int i=0;i<ENV_LampCompressed_Num ;i++)
	{
		
		M24C02_WriteOneByte(i+ENV_Lamp_Enable_Offset,pEnable[i]);
			
	}
}

extern  bool * ENV_Load_Lamp_Enable(void)
{
	unsigned char BitMask=0x01;
	unsigned char BitShiftNum=0;
	
	for(int i=0;i<ENV_LampCompressed_Num;i++)
	{
		
		BitShiftNum=((i<2)?8:(ENV_LampNum %8));
		
		ENV_Lamp_Enable_Compressed[i]=M24C02_ReadOneByte(i+ENV_Lamp_Enable_Offset);
		
		for(int BitPos=0;BitPos<BitShiftNum;BitPos++)
		{
			if((ENV_Lamp_Enable_Compressed[i]&BitMask)!=0)
				ENV_Lamp_Enable[8*i+BitPos]=true;
			else
				ENV_Lamp_Enable[8*i+BitPos]=false;
			
			BitMask=((BitPos==7)?0x01:BitMask<<1);
			
		}
	}
	return ENV_Lamp_Enable;
}


extern void ENV_Save_LEDCombo_Color(unsigned char * Color)
{
	for(int i=0;i<LampCtl_Num ;i++)
		M24C02_WriteOneByte(i+ENV_LEDCombo_Color_Offset,Color[i]);
	
}

extern unsigned char * ENV_Load_LEDCombo_Color(void)
{
	for(int i=0;i<LampCtl_Num ;i++)
		
	ENV_LEDCombo_Color[i]=M24C02_ReadOneByte(i+ENV_LEDCombo_Color_Offset);//LampCtl_Lamp[i].LED_Indicate_ID
	
	LampCtl_SetLedCombo_Color(ENV_LEDCombo_Color);
		
	return ENV_LEDCombo_Color;
}


extern void ENV_Save_Key_FlashStatus(unsigned short status)
{
	unsigned char main_status=(status>>8)&0xff;
	unsigned char sub_status=status&0xff;
	
	M24C02_WriteOneByte(ENV_Key_FlashStatus_Offset,main_status);
	M24C02_WriteOneByte(ENV_Key_FlashStatus_Offset+1,sub_status);
}
extern unsigned short ENV_Load_Key_FlashStatus(void)
{
	unsigned char main_status=M24C02_ReadOneByte(ENV_Key_FlashStatus_Offset);
	unsigned char sub_status=M24C02_ReadOneByte(ENV_Key_FlashStatus_Offset+1);
	
	ENV_Key_FlashStatus=main_status;
	ENV_Key_FlashStatus<<=8;
	ENV_Key_FlashStatus|=sub_status;
	
	return ENV_Key_FlashStatus;
	
}

