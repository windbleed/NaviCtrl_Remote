#include "VDRCoder.h"
#include "usart.h"



static VDR_Frame   VDRCoder_Frame;


extern void VDRCoder_Set_Protype(void)
{
	VDRCoder_Frame.Start_Symbol='$';
	
	VDRCoder_Frame.TalkerID[0]='Z';
	VDRCoder_Frame.TalkerID[1]='H';
	
	VDRCoder_Frame.CMD[0]='A';
	VDRCoder_Frame.CMD[1]='L';
	VDRCoder_Frame.CMD[2]='R';
	
	
	VDRCoder_Frame.Separator_1=',';
	VDRCoder_Frame.Separator_2=',';
	VDRCoder_Frame.Separator_3=',';
	VDRCoder_Frame.Separator_4=',';
	VDRCoder_Frame.Separator_5=',';
	
	VDRCoder_Frame.End_Symbol='*';
	
	VDRCoder_Frame.Char_Return=0x0d;
	VDRCoder_Frame.Char_LineFeed=0x0a;
	

}
extern void VDRCoder_Set_Time(unsigned char tm[6])
{
	
	VDRCoder_Frame.TIMER[0]=tm[0];
	VDRCoder_Frame.TIMER[1]=tm[1];
	VDRCoder_Frame.TIMER[2]=tm[2];
	VDRCoder_Frame.TIMER[3]=tm[3];
	VDRCoder_Frame.TIMER[4]=tm[4];
	VDRCoder_Frame.TIMER[5]=tm[5];
	VDRCoder_Frame.TIMER[6]='.';
	VDRCoder_Frame.TIMER[7]='0';
	VDRCoder_Frame.TIMER[8]='0';

	
	
}

void VDRCoder_Set_UAN(unsigned char Boardid,unsigned char LampPos)
{
		
	VDRCoder_Frame.UAN[0]=Boardid+48;
	VDRCoder_Frame.UAN[1]=LampPos/10+48;
	VDRCoder_Frame.UAN[2]=LampPos%10+48;
	
}

void VDRCoder_Set_Status_ACK(unsigned char status,bool ACK)     //status = 0:lamp off OK /1: lamp On OK/2:lamp error
{
	if (status==2)
	{
		VDRCoder_Frame.Condition='A';
		VDRCoder_Frame.ACK=ACK?'A':'V';
		VDRCoder_Frame.Description[0]='E';
		VDRCoder_Frame.Description[1]='R';
		VDRCoder_Frame.Description[2]='R';	
	}
	else if (status==0)
	{
		VDRCoder_Frame.Condition='V';
		VDRCoder_Frame.ACK=ACK?'A':'V';
		VDRCoder_Frame.Description[0]='O';
		VDRCoder_Frame.Description[1]='F';
		VDRCoder_Frame.Description[2]='F';
		
	}
	else 
	{
		VDRCoder_Frame.Condition='V';
		VDRCoder_Frame.ACK=ACK?'A':'V';
		VDRCoder_Frame.Description[0]='O';
		VDRCoder_Frame.Description[1]='N';
		VDRCoder_Frame.Description[2]=' ';
	}
}

void VDRCoder_Set_CheckSum()
{
	unsigned char tmp;
	
	tmp='Z'^'H'^'A'^'L'^'R'^',';
	
	tmp^=VDRCoder_Frame.TIMER[0]^VDRCoder_Frame.TIMER[1]^VDRCoder_Frame.TIMER[2]^VDRCoder_Frame.TIMER[3];
	tmp^=VDRCoder_Frame.TIMER[4]^VDRCoder_Frame.TIMER[5]^VDRCoder_Frame.TIMER[6]^VDRCoder_Frame.TIMER[7]^VDRCoder_Frame.TIMER[8];
	
	tmp^=VDRCoder_Frame.UAN[0]^VDRCoder_Frame.UAN[1]^VDRCoder_Frame.UAN[2];
	tmp^=VDRCoder_Frame.Condition^VDRCoder_Frame.ACK^VDRCoder_Frame.Description[0]^VDRCoder_Frame.Description[1]^VDRCoder_Frame.Description[2];
	
	VDRCoder_Frame.CheckSum[0]=((tmp/16)<10)?(tmp/16+'0'):(tmp/16-10+'A');
	
	
	VDRCoder_Frame.CheckSum[1]=((tmp%16)<10)?(tmp%16+'0'):(tmp%16-10+'A');
	
	
}


extern void VDRCoder_SendMessage(unsigned char COM)
{
	void * PMessage=&VDRCoder_Frame;
	
	if(COM==1)
	{
		USART_TxStringByLen(USART1,PMessage,VDRCoder_Frame_Len);
	}
	else if(COM==2)
	{
		USART_TxStringByLen(USART2,PMessage,VDRCoder_Frame_Len);
	}
	
}
