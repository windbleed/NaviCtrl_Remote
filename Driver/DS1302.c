#include "DS1302.h"
#include "FreeRTOS.h"
#include "task.h"
unsigned char  DS1302_CurrentTime[7];

unsigned char DS1302_Year=0;
unsigned char DS1302_Month=0;
unsigned char DS1302_Day=0;
unsigned char DS1302_Hour=0;
unsigned char DS1302_Min=0;
unsigned char DS1302_Sec=0;


My_Time TIME = {0};  				//显示时间的结构体

static unsigned char Init_time[] = {0x00,0x59,0x23,0x30,0x11,0x12,0x18};

unsigned char  read[] = {0x81,0x83,0x85,0x87,0x89,0x8b,0x8d};//读秒、分、时、日、月、周、年的寄存器地址 
unsigned char  write[] = {0x80,0x82,0x84,0x86,0x88,0x8a,0x8c};//写秒、分、时、日、月、周、年的寄存器地址


static void DS1302_gpio_init(void)
{
	    
    //开启GPIOB的时钟  
   
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHBPeriphClockCmd(DS1302_RCC, ENABLE);
    
    //设置GPIO的基本参数  
    GPIO_InitStruct.GPIO_Pin = DS1302_SCK_PIN | DS1302_CE_PIN ;  
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;    //这两个普通端口设为推挽输出  
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;    //输出速度50MHz 
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;	
    GPIO_Init(DS1302_PORT, &GPIO_InitStruct);  

    GPIO_InitStruct.GPIO_Pin = DS1302_IO_PIN;         //这里最好设成开漏，当然也可以普通推挽，但是需要后面一直切换输入输出模式
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;     //开漏输出，需要接上拉，不需要切换输入输出了。
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;    //输出速度50MHz 
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;	
    GPIO_Init(DS1302_PORT, &GPIO_InitStruct);
	
	GPIO_ResetBits(DS1302_PORT,DS1302_SCK_PIN|DS1302_CE_PIN|DS1302_IO_PIN );
}

static void DS1302_writebyte(unsigned char  Data)					//写一个字节; 
{
	unsigned char i = 0;  
    GPIO_ResetBits(DS1302_PORT,DS1302_SCK_PIN); //ds1302clk=0  
	__NOP();
  
    for(i = 0;i < 8;i ++)  
        {  
            GPIO_ResetBits(DS1302_PORT,DS1302_SCK_PIN);   //ds1302clk=0;  
            if(Data&0x01)  
                GPIO_SetBits(DS1302_PORT,DS1302_IO_PIN);  
            else                                           //ds1302dat=(dat&0x01)  
                GPIO_ResetBits(DS1302_PORT,DS1302_IO_PIN); 
				
            __NOP(); 
			
            GPIO_SetBits(DS1302_PORT,DS1302_SCK_PIN);    //发送一位数据，clk上升沿,//ds1302clk=1  
			
            Data >>= 1;  
            __NOP();
        }    
	
}

static void DS1302_writedata(unsigned char Addr,unsigned char Data) 	//给某地址写数据,data是c51内部的关键字，表示将变量定义在数据存储区，故此处用data_;
{
	GPIO_ResetBits(DS1302_PORT,DS1302_CE_PIN); //只有在rst为高电平的时候才能进行数据传输  
    GPIO_ResetBits(DS1302_PORT,DS1302_SCK_PIN); //只有clk为低电平的时候，rst才能被置为高电平  
    //ds1302rst=0;  
    //ds1302clk=0;  
    __NOP();                     //略微延时  
    GPIO_SetBits(DS1302_PORT,DS1302_CE_PIN);   //clk = 0之后，这里将rst拉高，准备传送数据  
    //ds1302rst=1;  
    __NOP();                     //时间大约2us  
    DS1302_writebyte(Addr);             //先发地址  
    DS1302_writebyte(Data);             //然后发数据  
    GPIO_ResetBits(DS1302_PORT,DS1302_SCK_PIN); //拉低clk，以备下一次数据发送
    GPIO_ResetBits(DS1302_PORT,DS1302_CE_PIN); 	//disable 总线 
    //ds1302clk=0;  
    //ds1302rst=0;  
    __NOP();  

	
}

static unsigned char   DS1302_readdata(unsigned char  Addr)			//读取某寄存器数据;
{
	unsigned char i=0;  
    unsigned char Return_dat=0x00;  
    GPIO_ResetBits(DS1302_PORT,DS1302_CE_PIN);            //ds1302rst=0;  
    GPIO_ResetBits(DS1302_PORT,DS1302_SCK_PIN);           //ds1302clk=0;  
    __NOP();                                              //略微延时2us  
    GPIO_SetBits(DS1302_PORT,DS1302_CE_PIN);              //ds1302rst=1;  
    __NOP();                                				//时间要大约3us  
    DS1302_writebyte(Addr);                         		//先写寄存器的地址  
    for(i=0;i<8;i++)  
    {  
        GPIO_SetBits(DS1302_PORT,DS1302_SCK_PIN);       //ds1302clk=1; 
        
        Return_dat >>= 1;  
        __NOP();
        GPIO_ResetBits(DS1302_PORT,DS1302_SCK_PIN);     //ds1302clk=0;//拉低时钟线，以便于数据的读入  
        if(GPIO_ReadInputDataBit(DS1302_PORT,DS1302_IO_PIN)==1)    //数据线此时为高电平  
        {Return_dat = Return_dat|0x80;}  
    }  
      __NOP(); 
      GPIO_ResetBits(DS1302_PORT,DS1302_CE_PIN);        //ds1302rst=0;disable总线  
      return Return_dat;  
	
}


void DS1302_Init(void)
{
     unsigned char i = 0;
    
    DS1302_gpio_init();  //端口初始化
	DS1302_writedata(0x8e,0x00); //撤销写保护，允许写入数据,0x8e,0x00

        for(i = 0;i<7;i++)
        {
            DS1302_writedata(write[i],Init_time[i]);
        }
    
         DS1302_writedata(0x8e,0x80);//打开写保护功能，防止干扰造成的数据写入。
}

void DS1302_WriteTime(unsigned char Time[3]) //秒 、分 、时
{
	
	DS1302_writedata(0x8e,0x00); //撤销写保护，允许写入数据,0x8e,0x00

        for(int i = 0;i<3;i++)
        {
            DS1302_writedata(write[i],Time[i]);
        }
    
         DS1302_writedata(0x8e,0x80);//打开写保护功能，防止干扰造成的数据写入。
	
	
}

void DS1302_WriteDate(unsigned char Date[4]) //日  、月  、周 、年 
{
	
	DS1302_writedata(0x8e,0x00); //撤销写保护，允许写入数据,0x8e,0x00

        for(int i = 3;i<7;i++)
        {
            DS1302_writedata(write[i],Date[i-3]);
        }
    
         DS1302_writedata(0x8e,0x80);//打开写保护功能，防止干扰造成的数据写入。
	
	
}








unsigned char  BCDToHex(unsigned char Data)
{
	unsigned char tmp;
	
	tmp=(Data/16)*10+(Data%16);
	return tmp;
}



void DS1302_ReadTime(void)   //读取时间
{
      unsigned char i;
  
	  
      for(i = 0;i<7;i++)
      {
         DS1302_CurrentTime[i] = BCDToHex(DS1302_readdata(read[i]));
		     //vTaskDelay(10);
      }
	  
	  DS1302_Year=DS1302_CurrentTime[6];
	  DS1302_Month=DS1302_CurrentTime[4];
	  DS1302_Day=DS1302_CurrentTime[3];
	  
	  DS1302_Hour=DS1302_CurrentTime[2];
	  DS1302_Min=DS1302_CurrentTime[1];
	  DS1302_Sec=DS1302_CurrentTime[0];
	  
	  	  
	  
	  
}
