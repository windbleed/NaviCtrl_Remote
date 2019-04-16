#ifndef M24C02_Unit
#define M24C02_Unit

#include "stdbool.h"

#define M24C02_PORT   GPIOA
#define M24C02_RCC    RCC_AHBPeriph_GPIOA

#define M24C02_SDA GPIO_Pin_6 //PA6
#define M24C02_SCL GPIO_Pin_5 //PA5
#define M24C02_NSS  GPIO_Pin_7 //PA7



void M24C02_SDA_OUT(void);

void M24C02_SDA_IN(void);

void M24C02_Init(void);

static void M24C02_Start(void);

static void M24C02_Stop(void);

static bool M24C02_Wait_ACK(void);

static void M24C02_Ack(void);

static void M24C02_NAck(void);

static void M24C02_Send_Byte(unsigned char txd);

static unsigned char M24C02_Read_Byte(unsigned char ack);

extern unsigned char M24C02_ReadOneByte(unsigned int ReadAddr);

extern void M24C02_WriteOneByte(unsigned int WriteAddr,unsigned char Data);

static  void M24C02_WriteLenByte(unsigned int WriteAddr, unsigned char * WriteBuf,unsigned int Len);

static  void M24C02_ReadLenByte(unsigned int ReadAddr,unsigned char * ReadBuf,unsigned int Len);

extern void M24C02_Format(void);


bool M24C02_Check(void);





























#endif

