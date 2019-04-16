#ifndef GPSParser_Unit
#define GPSParser_Unit

static unsigned char GPS_UTC_Time[6];  //Hour/Min/Second each 2 bytes  

static unsigned char GPS_UTC_Date[6];  //Year/Month/Day each 2bytes


static unsigned char GPS_Time_BCD[3];  //HH:MM:SS 
static unsigned char GPS_Date_BCD[3];  //DD:MM:YY

extern unsigned char GPS_Year;
extern unsigned char GPS_Month;
extern unsigned char GPS_Day;
extern unsigned char GPS_Hour;
extern unsigned char GPS_Min;
extern unsigned char GPS_Sec;




static unsigned char GPS_Source_Frame[100];

static  void GPS_GGA_Parse(unsigned char *Frame,unsigned char * UTC_Time);

static  void GPS_RMC_Parse(unsigned char *Frame,unsigned char * UTC_Time,unsigned char * UTC_Data);

extern void GPS_GGA_Parse_BCD(unsigned char * Buf);

extern void  GPS_RMC_Parse_BCD(unsigned char * Buf);





#endif
