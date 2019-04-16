#include "TX_Framer.h"
#include "usart.h"






static unsigned char TXFramer_Str[15];



void TXFramer_SendSync(void)
{
	unsigned char checksum_1,checksum_2;
	unsigned char tmp;
	
	TXFramer_Str[0]=START_SYMBOL;
	TXFramer_Str[1]='S';
	TXFramer_Str[2]='Y';
	TXFramer_Str[3]='N';
	TXFramer_Str[4]='C';
	TXFramer_Str[5]=',';
	TXFramer_Str[6]='1';
	TXFramer_Str[7]=END_SYMBOL;
	
	tmp=TXFramer_CalChecksum(TXFramer_Str,8);
	checksum_1=((tmp/16)<10)?(tmp/16+'0'):(tmp/16-10+'A');
	checksum_2=((tmp%16)<10)?(tmp%16+'0'):(tmp%16-10+'A');
	
	TXFramer_Str[8]=checksum_1;
	TXFramer_Str[9]=checksum_2;
	
	TXFramer_Str[10]=0x0d;
	TXFramer_Str[11]=0x0a;
	
	USART1_TxStringByLen(TXFramer_Str,12);
	
	
	
}



// $ 0x02 0x01 0xXX 0xYY *hh 0x0D 0x0A (XX Mainboard ID, YY SIMKey NUM)
void TXFramer_SendSimKey(unsigned char BId,unsigned char Keynum)
{
	unsigned char checksum_1,checksum_2;
	unsigned char tmp;
	
	TXFramer_Str[0]=START_SYMBOL;
	TXFramer_Str[1]=0x02;
	TXFramer_Str[2]=0x01;
	TXFramer_Str[3]=BId;
	TXFramer_Str[4]=Keynum;
	TXFramer_Str[5]=END_SYMBOL;
	
	tmp=TXFramer_CalChecksum(TXFramer_Str,6);
	checksum_1=((tmp/16)<10)?(tmp/16+'0'):(tmp/16-10+'A');
	checksum_2=((tmp%16)<10)?(tmp%16+'0'):(tmp%16-10+'A');
	
	TXFramer_Str[6]=checksum_1;
	TXFramer_Str[7]=checksum_2;
	
	TXFramer_Str[8]=0x0d;
	TXFramer_Str[9]=0x0a;
	
	USART1_TxStringByLen(TXFramer_Str,10);
	
}


unsigned char TXFramer_CalChecksum(unsigned char *str,int length)
{
	
	unsigned char CheckSum_Verify=str[1];

	
	
	for(int i=2;i<length-1;i++)
	{
		CheckSum_Verify^=str[i];
			
		
	}
	return CheckSum_Verify;
}