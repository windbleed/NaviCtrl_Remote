
#include "stdio.h"
#include "usart.h"
#include "semphr.h"
#include "task.h"

#define USART_Retry_Max_Count 5



static xSemaphoreHandle  USART1_BUFFER_OP=NULL;


unsigned char  USART1_RX_BUF[USART_BUF_LEN];

static unsigned int USART1_RX_STA=0;

static bool USART1_RX_Frame_Start=false;

static bool USART1_RX_Busy=false;

static unsigned int USART1_Retry_Count=0;

static unsigned int USART1_OverTimer=0;




bool USART1_Com_Status=false;

int USART1_RX_Length=0;


xSemaphoreHandle USART1_ReceivedSemaphore=NULL;

//xSemaphoreHandle USART1_BufferReady=NULL;



//串口1配置函数
void USART1_Config()
{
	NVIC_InitTypeDef    NVIC_InitStructure;
	USART_InitTypeDef	USART_InitStructure;
	GPIO_InitTypeDef	GPIO_InitStructure;
	
	//开启串口1和GPIOA的时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	//PA9(TX),PA10(RX)引脚配置
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//USART1_RTS pin config
	RCC_AHBPeriphClockCmd(USART1_RTS_RCC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = USART1_RTS_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;	
	GPIO_Init(USART1_RTS_PORT, &GPIO_InitStructure);
	
	USART1_Set_RTS_RX();
	
	
	//打开引脚和串口1和连接
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);
	
	
		
	//清除串口配置
	USART_DeInit(USART1);
	
	//配置串口
	USART_InitStructure.USART_BaudRate = USART1_BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		//8个数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;				//无奇偶校验
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//开启接收和发送
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控制	
	USART_Init(USART1, &USART_InitStructure);	//初始化串口
	
		
	//Enable the USART1 Interrupt
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);  	//开启串口接收中断
	USART_Cmd(USART1, ENABLE);						//启用串口
	
	

	 USART1_BUFFER_OP=xSemaphoreCreateBinary();
	 
	xSemaphoreGive(USART1_BUFFER_OP);
	//USART1_BufferReady=xSemaphoreCreateCounting(1,0);
	
}







void USART_TxByte(USART_TypeDef* USARTx,unsigned char  dat)
{
  USART_SendData(USARTx,dat);
  while (USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET);
}

//重定义fputc函数发送到串口1
int fputc(int ch, FILE *f)
{
	USART1_Set_RTS_TX();
	 
	USART_SendData(USART1, (uint8_t) ch);

	/* 等待发送结束 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
	{}

	USART1_Set_RTS_RX();

	return ch;
}


int fgetc(FILE *f)
{
	/* 等待串口1输入数据 */
	while (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);

	return (int)USART_ReceiveData(USART1);
}


static void USART1_Set_RTS_TX(void)
{
	GPIO_ResetBits(USART1_RTS_PORT,USART1_RTS_Pin);
}

static void USART1_Set_RTS_RX(void)
{
	GPIO_SetBits(USART1_RTS_PORT,USART1_RTS_Pin);
	
}






bool USART_TxString(USART_TypeDef* USARTx,unsigned char *string)
{

	
		if(USARTx==USART1)
		{
			//xSemaphoreTake(USART1_BUFFER_OP,portMAX_DELAY);
			
			while (USART1_RX_Busy) 
			{
				USART1_OverTimer++;
				
				if (USART1_OverTimer>=10)
				{
					USART1_RX_Busy=false;
					USART1_RX_Frame_Start=false;
					USART1_RX_Length=0;
					USART1_RX_STA=0;
					//USART1_Com_Status=true;
					USART1_Retry_Count=0;
					USART1_OverTimer=0;
					
				}
				else
				{
					vTaskDelay(150);
				}
			
			}
		//	xSemaphoreGive(USART1_BUFFER_OP);
			
			USART1_Set_RTS_TX();
		}

	

		while(*string != '\0')
	 
			USART_TxByte(USARTx,*string++);
	
		
	if(USARTx==USART1)
		USART1_Set_RTS_RX();
	
	
	return true;
	

}


bool USART_TxStringByLen(USART_TypeDef* USARTx,unsigned char *string,int length)
{
	unsigned char *tp=string;
	
	
	
		if(USARTx==USART1)
		{
			while (USART1_RX_Busy) 
			{}
			USART1_Set_RTS_TX();
		}


		for(int i=0;i<length;i++)
		USART_TxByte(USARTx,*tp++);
	
	
	if(USARTx==USART1)
		USART1_Set_RTS_RX();

	
	return true;
	
}


bool USART1_TxStringByLen(unsigned char * string,int length)
{
	
			xSemaphoreTake(USART1_BUFFER_OP,portMAX_DELAY);
			
			while (USART1_RX_Busy) 
			{
				USART1_OverTimer++;
				
				if (USART1_OverTimer>=10)
				{
					USART1_RX_Busy=false;
					USART1_RX_Frame_Start=false;
					USART1_RX_Length=0;
					USART1_RX_STA=0;
					//USART1_Com_Status=true;
					USART1_Retry_Count=0;
					USART1_OverTimer=0;
					
				}
				else
				{
					vTaskDelay(150);
				}
			
			}
			xSemaphoreGive(USART1_BUFFER_OP);
	  //
		
			
		if(++USART1_Retry_Count>USART_Retry_Max_Count)
		{
			USART1_Com_Status=false;
		}
			
		if(!USART1_Com_Status)
		{
			//set com led color to red
		}
		
	
		USART1_Set_RTS_TX();
	
		for(int i=0;i<length;i++)
		USART_TxByte(USART1,*string++);

 
	USART1_Set_RTS_RX();
			
			
			
	
	return true;
}









//USART1_IRQHandler

void USART1_IRQHandler(void)
{
	unsigned char Res;
	long  xHigherPriorityTaskWoken;
	
	if(USART_GetFlagStatus(USART1,USART_FLAG_ORE))	//溢出中断
    USART1->ICR |= 1 << 3;           				//空读清除ORE位
	
	if( USART_GetFlagStatus(USART1,USART_FLAG_RXNE)!=RESET)
	{
		
		//xSemaphoreTake(USART1_BUFFER_OP,portMAX_DELAY);
		
		USART1_RX_Busy=true;
		
		Res=USART_ReceiveData(USART1);
		
		if (Res=='$')
		{
			
			USART1_RX_Frame_Start=true;
		}
		
		if(USART1_RX_Frame_Start)
		{	
			if((USART1_RX_STA&0x8000)==0)  // rx not end
			{
				if(USART1_RX_STA&0x4000)     // rx 0x0d last time
				{
					if (Res!=0x0a) 
					{
					USART1_RX_STA&=~0x4000;
					USART1_RX_BUF[USART1_RX_STA&0x3fff]=Res;  //not rx 0x0d this time, rx[buf]<-data
					USART1_RX_STA++; 
						
					}	
				//USART1_RX_STA=0;  // if not rx 0x0a current  then wrong rebegin
					else 
					{
					USART1_RX_STA|=0x8000;      // else mask to succeed rx
					USART1_RX_STA--; 
					}
				}
				else                                   //not rx ox0d last time
				{
					if(Res==0x0d) 
					{
					USART1_RX_STA|=0x4000; // rx 0x0d this time then mask to rx 0x0d 
					USART1_RX_BUF[USART1_RX_STA&0x3fff]=Res;  //not rx 0x0d this time, rx[buf]<-data
					USART1_RX_STA++;                          //lenght++
					}
					else
					{
						USART1_RX_BUF[USART1_RX_STA&0x3fff]=Res;  //not rx 0x0d this time, rx[buf]<-data
						USART1_RX_STA++;                          //lenght++
						if(USART1_RX_STA>(USART_BUF_LEN-1))       //rx buf full?
						USART1_RX_STA=0;                        //rebegan rx
					}
				}
			}
	
	
			if ((USART1_RX_STA&0x8000)&&(USART1_ReceivedSemaphore!=NULL))
			{
				USART1_RX_Busy=false;
				USART1_RX_Frame_Start=false;
				USART1_RX_Length=USART1_RX_STA&0x3fff;
				USART1_RX_STA=0;
				USART1_Com_Status=true;
				USART1_Retry_Count=0;
				
				//xSemaphoreGive(USART1_BUFFER_OP);
				
				xSemaphoreGiveFromISR(USART1_ReceivedSemaphore,&xHigherPriorityTaskWoken);
				
				
				
				portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		
			}
			
			else
			{
			
				//xSemaphoreGive(USART1_BUFFER_OP);
				
			
				
			}
			
	  }
	  else
	  {			
			USART1_RX_Busy=false;
			
			//xSemaphoreGive(USART1_BUFFER_OP);
							
			return;
	  }
	}
	
}
		









	
