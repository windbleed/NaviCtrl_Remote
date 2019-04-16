#include "CMDParser.h"

#include "usart.h"
#include "LEDCombo.h"
#include "ControllerStatus.h"









void CMDParser_COM1_Handler()
{
	unsigned char CMDID;
	
	bool CheckSumOK=true;
	
	
	
	
	
	if(USART1_RX_BUF[0]=='$')
	{
		CheckSumOK=CMDParser_VerifyCheckSum(USART1_RX_BUF,USART1_RX_Length);
		
		if (!CheckSumOK) 
		{
			//USART_TxString(USART1,"CheckSum Error\r\n");
			
			return;
	
		}
		
		CMDID=USART1_RX_BUF[1];
		
		if((USART1_RX_BUF[1]=='S')&&(USART1_RX_BUF[2]=='Y')&&(USART1_RX_BUF[3]=='N')&&(USART1_RX_BUF[4]=='C'))
			CMDID=CMDParser_SYNC;
		
		if((USART1_RX_BUF[1]=='M')&&(USART1_RX_BUF[2]=='O')&&(USART1_RX_BUF[3]=='D')&&(USART1_RX_BUF[4]=='E'))
			CMDID=CMDParser_MODE;
		
		if((USART1_RX_BUF[3]=='A')&&(USART1_RX_BUF[4]=='L')&&(USART1_RX_BUF[5]=='R'))
			CMDID=CMDParser_ALR;
		
		switch(CMDID)
		{
			case CMDParser_SYNC:
				CMDParser_UnPack_SYNC(USART1_RX_BUF);
				break;
			case CMDParser_MODE:
				CMDParser_UnPack_MODE(USART1_RX_BUF);
				break;
			case CMDParser_ALR:
				CMDParser_UnPack_ALR(USART1_RX_BUF);
				break;

		}
		
		
		
	}
	else
	{
		//USART_TxString(USART1,"Frame Asyn fail\r\n");
	
	}
	

	
}







//$SYNC,x,ua,vb,wc,m*hh 0x0D 0x0A
//u/v/w:1: no error off 2: error off 3:no error on 4: error on
//a/b/c:0: none errorack 1: erroracked
static void CMDParser_UnPack_SYNC(unsigned char * Buf)
{
	
	//Left flash and error unpack
	unsigned char tmp=USART1_RX_BUF[8];
	
	switch(tmp)
	{
		case '1':
		{
			Status_Left_ON=false;
			Status_LeftERR_ON=false;
			break;					
		}
		case '2':
		{
			Status_Left_ON=true;
			Status_LeftERR_ON=true;
			break;					
		}
		case '3':
		{
			Status_Left_ON=true;
			Status_LeftERR_ON=false;
			break;					
		}
		case '4':
		{
			Status_Left_ON=false;
			Status_LeftERR_ON=true;
			break;					
		}
	}
	
	tmp=USART1_RX_BUF[9];
	switch(tmp)
	{
		case '0':
		{
			Status_LeftERR_Blink=true;
			
			break;					
		}
		case '1':
		{
			Status_LeftERR_Blink=false;
			break;					
		}
		
		
	}
	
	// Mid unpack
	tmp=USART1_RX_BUF[11];
	switch(tmp)
	{
		case '1':
		{
			Status_Mid_ON=false;
			Status_MidERR_ON=false;
			break;					
		}
		case '2':
		{
			Status_Mid_ON=true;
			Status_MidERR_ON=true;
			break;					
		}
		case '3':
		{
			Status_Mid_ON=true;
			Status_MidERR_ON=false;
			break;					
		}
		case '4':
		{
			Status_Mid_ON=false;
			Status_MidERR_ON=true;
			break;					
		}
	}
	
	tmp=USART1_RX_BUF[12];
	switch(tmp)
	{
		case '0':
		{
			Status_MidERR_Blink=true;
			
			break;					
		}
		case '1':
		{
			Status_MidERR_Blink=false;
			break;					
		}
		
		
	}
	
	// Right unpack
	tmp=USART1_RX_BUF[14];
	switch(tmp)
	{
		case '1':
		{
			Status_Right_ON=false;
			Status_RightERR_ON=false;
			break;					
		}
		case '2':
		{
			Status_Right_ON=true;
			Status_RightERR_ON=true;
			break;					
		}
		case '3':
		{
			Status_Right_ON=true;
			Status_RightERR_ON=false;
			break;					
		}
		case '4':
		{
			Status_Right_ON=false;
			Status_RightERR_ON=true;
			break;					
		}
	}
	
	tmp=USART1_RX_BUF[15];
	switch(tmp)
	{
		case '0':
		{
			Status_RightERR_Blink=true;
			
			break;					
		}
		case '1':
		{
			Status_RightERR_Blink=false;
			break;					
		}
		
		
	}
	
	//Mode unpack
	tmp=USART1_RX_BUF[17];
	switch(tmp)
	{
		case '0':
		{
			Status_MODE_AUTO=false;
			Status_Left_Blink=false;
			Status_Mid_Blink=false;
			Status_Right_Blink=false;
			break;					
		}
		case '1':
		{
			Status_MODE_AUTO=true;
			Status_Left_Blink=true;
			Status_Mid_Blink=true;
			Status_Right_Blink=true;
			break;					
		}
		
		
	}
	
	
	
	Status_Update();
	
	//USART_TxString(USART1,"SYNC Proceeded!\r\n");
	
}



static void CMDParser_UnPack_ALR_Left(unsigned char A_1st,unsigned char A_2nd,unsigned char ON)
{
	if (A_1st=='A')
		Status_LeftERR_ON=true;
	else
		Status_LeftERR_ON=false;
	
	if(A_2nd=='A')
		Status_LeftERR_Blink=false;
	else
		Status_LeftERR_Blink=true;
	
	if (ON=='N')
		Status_Left_ON=true;
	else
		Status_Left_ON=false;
	

}

static void CMDParser_UnPack_ALR_Mid(unsigned char A_1st,unsigned char A_2nd,unsigned char ON)
{
	if (A_1st=='A')
		Status_MidERR_ON=true;
	else
		Status_MidERR_ON=false;
	
	if(A_2nd=='A')
		Status_MidERR_Blink=false;
	else
		Status_MidERR_Blink=true;
	
	if (ON=='N')
		Status_Mid_ON=true;
	else
		Status_Mid_ON=false;
	

}

static void CMDParser_UnPack_ALR_Right(unsigned char A_1st,unsigned char A_2nd,unsigned char ON)
{
	if (A_1st=='A')
		Status_RightERR_ON=true;
	else
		Status_RightERR_ON=false;
	
	if(A_2nd=='A')
		Status_RightERR_Blink=false;
	else
		Status_RightERR_Blink=true;
	
	if (ON=='N')
		Status_Right_ON=true;
	else
		Status_Right_ON=false;
	

}

//$ZHALR,hhmmss.ss,xxx,A,A,ccc*hh 0x0D 0x0A
// xxx: 1st x-boardid  2nd xx 1~20 channel,leftflash is 02 midflash is 03 rightflash is 04
//first A field if A ERR if V No ERR,
//second A field if A ERR ACKED if V None ACKED
//ccc is ON OFF ERR, ERR will be delete because of first A field same meaning,here 
//proceed the same as OFF
static void CMDParser_UnPack_ALR(unsigned char * Buf)
{

	
	unsigned char tmp;
	
	tmp=Buf[18];
	
	//printf("tmp [18] is %d\r\n",tmp);
	
	if (tmp=='0')
	{	
		tmp=Buf[19];
		
		switch(tmp)
		{
			case '2': 
			{
				CMDParser_UnPack_ALR_Left(Buf[21],Buf[23],Buf[26]);
				break;
			
			}
			case '3': 
			{
				CMDParser_UnPack_ALR_Mid(Buf[21],Buf[23],Buf[26]);
				break;
			
			}
			case '4': 
			{
				CMDParser_UnPack_ALR_Right(Buf[21],Buf[23],Buf[26]);
				break;
			
			}
		}
		
		
		
		Status_Update();
		
		
		

		
	}
	else 
	{
				USART_TxString(USART1,"Flash channel error!\r\n");
				return;
		
	}
		
		
	
	
		//USART_TxString(USART1,"ALR Proceeded!\r\n");
}

///	$MODE,x,m*hh 0x0D 0x0A 
//  x boardid, m 0:manual 1:auto
static void CMDParser_UnPack_MODE(unsigned char * BUF)
{
	
	if (BUF[8]=='0')
		Status_MODE_AUTO=false;
	else 
		Status_MODE_AUTO=true;
	
	Status_Update();
	
	
	
		//USART_TxString(USART1,"MODE Proceeded!\r\n");
}
	


static bool CMDParser_VerifyCheckSum(unsigned char *Buf, unsigned char Length)
{
	unsigned char CheckSum_Verify=Buf[1];
	unsigned char CheckSum_Buf=0;
	
	CheckSum_Buf=CMDParser_GetOriginalChecksum(Buf[Length-2],Buf[Length-1]);
	
	for(int i=2;i<Length-3;i++)
	{
		CheckSum_Verify^=Buf[i];
			
		
	}
	if (CheckSum_Verify==CheckSum_Buf)
		return true;
	else 
		return false;
	
}


static unsigned char CMDParser_GetOriginalChecksum(unsigned char P1,unsigned char P2)
{
	unsigned char CheckSum_High=0;
	unsigned char CheckSum_Low=0;
	
	
	if ((P1>='0')&&(P1<='9'))
		CheckSum_High=P1-'0';
	else if((P1>='A')&&(P1<='F'))
		CheckSum_High=P1-'A'+0x0a;
	
	if ((P2>='0')&&(P2<='9'))
		CheckSum_Low=P2-'0';
	else if((P2>='A')&&(P2<='F'))
		CheckSum_Low=P2-'A'+0x0a;

	return(CheckSum_High*16+CheckSum_Low);
	
	
	
} 



