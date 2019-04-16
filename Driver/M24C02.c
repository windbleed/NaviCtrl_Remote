#include "M24C02.h"
#include "stm32f0xx.h"
#include "freertos.h"
#include "task.h"

void M24C02_SDA_IN(void)
{
    
    GPIO_InitTypeDef GPIO_InitStructure;
   
    RCC_AHBPeriphClockCmd(M24C02_RCC,ENABLE);

    GPIO_InitStructure.GPIO_Pin = M24C02_SDA;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(M24C02_PORT,&GPIO_InitStructure);

}
void M24C02_SDA_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
   
    RCC_AHBPeriphClockCmd(M24C02_RCC,ENABLE);

    GPIO_InitStructure.GPIO_Pin =  M24C02_SDA;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(M24C02_PORT,&GPIO_InitStructure);

} 


void M24C02_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruchture;
	
	RCC_AHBPeriphClockCmd(M24C02_RCC, ENABLE);
	
	GPIO_InitStruchture.GPIO_Pin = M24C02_NSS | M24C02_SDA | M24C02_SCL;
	GPIO_InitStruchture.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruchture.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruchture.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruchture.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(M24C02_PORT, &GPIO_InitStruchture);
	
	GPIO_SetBits(M24C02_PORT, M24C02_SDA | M24C02_SCL);
	GPIO_ResetBits(M24C02_PORT,M24C02_NSS);
	
	
}

static void M24C02_Start(void)
{
	M24C02_SDA_OUT();
	GPIO_SetBits(M24C02_PORT,M24C02_SDA);
	GPIO_SetBits(M24C02_PORT,M24C02_SCL);
	__NOP();
	GPIO_ResetBits(M24C02_PORT,M24C02_SDA);
	__NOP();
	GPIO_ResetBits(M24C02_PORT,M24C02_SCL);
	
}

static void M24C02_Stop(void)
{
	M24C02_SDA_OUT();
	GPIO_ResetBits(M24C02_PORT,M24C02_SCL);
	GPIO_ResetBits(M24C02_PORT,M24C02_SDA);
	__NOP();
	GPIO_SetBits(M24C02_PORT,M24C02_SCL);
	GPIO_SetBits(M24C02_PORT,M24C02_SDA);
	__NOP();
}

static bool M24C02_Wait_ACK(void)
{
	unsigned char ucErrTime=0;
	M24C02_SDA_IN();
	
	GPIO_SetBits(M24C02_PORT,M24C02_SDA);
	__NOP();
	GPIO_SetBits(M24C02_PORT,M24C02_SCL);
	__NOP();
	
	while(GPIO_ReadInputDataBit(M24C02_PORT,M24C02_SDA)!=0)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			M24C02_Stop();
			return false;
		}
	
	}
	GPIO_ResetBits(M24C02_PORT,M24C02_SCL);
	return true;
		
	
	
	
}

static void M24C02_Ack(void)
{
	GPIO_ResetBits(M24C02_PORT,M24C02_SCL);
	M24C02_SDA_OUT();
	GPIO_ResetBits(M24C02_PORT,M24C02_SDA);
	__NOP();
	GPIO_SetBits(M24C02_PORT,M24C02_SCL);
	__NOP();
	GPIO_ResetBits(M24C02_PORT,M24C02_SCL);
}

static void M24C02_NAck(void)
{
	GPIO_ResetBits(M24C02_PORT,M24C02_SCL);
	M24C02_SDA_OUT();
	GPIO_SetBits(M24C02_PORT,M24C02_SDA);
	__NOP();
	GPIO_SetBits(M24C02_PORT,M24C02_SCL);
	__NOP();
	GPIO_ResetBits(M24C02_PORT,M24C02_SCL);
	
		
	
}

void M24C02_Send_Byte(unsigned char txd)
{
	unsigned char t;
	M24C02_SDA_OUT();
	GPIO_ResetBits(M24C02_PORT,M24C02_SCL);
	for(t=0;t<8;t++)
	{
		if((txd&0x80)>>7) GPIO_SetBits(M24C02_PORT,M24C02_SDA);
		else   GPIO_ResetBits(M24C02_PORT,M24C02_SDA);
		txd<<=1;
		__NOP();
		GPIO_SetBits(M24C02_PORT,M24C02_SCL);
		__NOP();
		GPIO_ResetBits(M24C02_PORT,M24C02_SCL);
		__NOP();
		
	}
	
}

unsigned char M24C02_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	M24C02_SDA_IN();
	for(i=0;i<8;i++)
	{
		GPIO_ResetBits(M24C02_PORT,M24C02_SCL);
		__NOP();
		GPIO_SetBits(M24C02_PORT,M24C02_SCL);
		receive<<=1;
		if(GPIO_ReadInputDataBit(M24C02_PORT,M24C02_SDA)!=0) receive++;
		__NOP();
		
		
	}
	if(ack) M24C02_Ack();
	else M24C02_NAck();
	return receive;
	
}

unsigned char M24C02_ReadOneByte(unsigned int ReadAddr)
{
	unsigned char temp=0;
	M24C02_Start();
	M24C02_Send_Byte(0xA0+((ReadAddr/256)<<1));
	M24C02_Wait_ACK();
	M24C02_Send_Byte(ReadAddr%256);
	M24C02_Wait_ACK();
	M24C02_Start();
	M24C02_Send_Byte(0xA1);
	M24C02_Wait_ACK();
	temp=M24C02_Read_Byte(0);
	M24C02_Stop();
	return temp;
	
	
}

void M24C02_WriteOneByte(unsigned int WriteAddr,unsigned char Data)
{
	M24C02_Start();
	M24C02_Send_Byte(0xA0+((WriteAddr/256)<<1));
	M24C02_Wait_ACK();
	M24C02_Send_Byte(WriteAddr%256);
	M24C02_Wait_ACK();
	M24C02_Send_Byte(Data);
	M24C02_Wait_ACK();
	M24C02_Stop();
	//vTaskDelay(5);
	
	__NOP();
	__NOP();
	
}

void M24C02_WriteLenByte(unsigned int WriteAddr, unsigned char * WriteBuf,unsigned int Len)
{
	while(Len--)
	{
		M24C02_WriteOneByte(WriteAddr,*WriteBuf);
		WriteAddr++;
		WriteBuf++;
	}
}

void M24C02_ReadLenByte(unsigned int ReadAddr,unsigned char * ReadBuf,unsigned int Len)
{
	while(Len--)
	{
		*ReadBuf++=M24C02_ReadOneByte(ReadAddr++);
		
	}
		
}

bool M24C02_Check(void)
{
	unsigned char temp;
	temp=M24C02_ReadOneByte(255);
	if(temp==0x55) return true;
	else
	{
		M24C02_WriteOneByte(255,0x55);
		temp=M24C02_ReadOneByte(255);
		if (temp==0x55) return true;
		
	}
	return false;
}




 void M24C02_Format()
{
	for (int i=0;i<255;i++)
	{
		M24C02_WriteOneByte(i,0xff);
	}
}















