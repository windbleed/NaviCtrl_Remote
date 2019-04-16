#ifndef DS1302_Unit
#define DS1302_Unit

#include "stm32f0xx.h"


//相对应的IO口配置
#define DS1302_PORT           GPIOB
#define DS1302_RCC            RCC_AHBPeriph_GPIOB

#define DS1302_SCK_PIN        GPIO_Pin_2        //时钟
#define DS1302_IO_PIN         GPIO_Pin_13        //双向IO口，
#define DS1302_CE_PIN         GPIO_Pin_14        //片选使能，当需要读写的时候，置高位


//存放时间 with bcd code type to restore time ,for exp. 0x12 means 12 
typedef struct _time{ 

    unsigned char second;
    unsigned char minute;
    unsigned char hour;
    unsigned char date;
    unsigned char month;
    unsigned char week;
    unsigned char year;

}My_Time;

extern unsigned char  DS1302_CurrentTime[7];

extern unsigned char DS1302_Year;
extern unsigned char DS1302_Month;
extern unsigned char DS1302_Day;
extern unsigned char DS1302_Hour;
extern unsigned char DS1302_Min;
extern unsigned char DS1302_Sec;

extern void DS1302_Init(void);
extern void DS1302_ReadTime(void);
extern void DS1302_WriteTime(unsigned char Time[3]); //秒 、分 、时
extern void DS1302_WriteDate(unsigned char Date[4]); //日  、月  、周 、年 




static void DS1302_gpio_init(void);
static void DS1302_writebyte(unsigned char  Data);					//写一个字节; byte是保留字，不能作为变量
static void DS1302_writedata(unsigned char Addr,unsigned char Data);	//给某地址写数据,data是c51内部的关键字，表示将变量定义在数据存储区，故此处用data_;
static unsigned char   DS1302_readbyte(void);							//读一个字节
static unsigned char   DS1302_readdata(unsigned char  Addr);			//读取某寄存器数据;




#endif
