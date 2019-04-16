#include "GPS_Parser.h"
#include "stdbool.h"

// with ascII to restore the time  for exp  0x31 0x32 = 12

static unsigned char GPS_UTC_Time[6]   //hhmmss.ss
={0x00,0x00,0x00,0x00,0x00,0x00}; 



static unsigned char GPS_Time_BCD[3];


unsigned char GPS_Year=0;
unsigned char GPS_Month=0;
unsigned char GPS_Day=0;
unsigned char GPS_Hour=0;
unsigned char GPS_Min=0;
unsigned char GPS_Sec=0;



static unsigned char GPS_UTC_Date[6]    //day/month/year
={0x00,0x00,0x00,0x00,0x00,0x00}; 

static unsigned char GPS_Date_BCD[3];

static unsigned char GPS_Source_Frame[100];


unsigned int Find_Useful_location(unsigned char *Frame,int comma_count)
{
	unsigned char *SearchPT=Frame;
	bool Search_End=false;
	int Found_Count=0;
	int Found_Location=0;
	int Search_Loc=0;

	while(!Search_End)
	{
		if (*SearchPT==',')
		{
			Found_Count++;
		  
			if (Found_Count==comma_count)
			{
				Search_End=true;
				Found_Location=Search_Loc;
			}
			else
			{	
				SearchPT++;
				Search_Loc++;	
			}
		}
		else
		{
			SearchPT++;
			Search_Loc++;
		}
	} 
  

	return Found_Location;
}

extern void GPS_GGA_Parse(unsigned char *Frame,unsigned char * UTC_Time)
{
	//$GPGGA,092204.999,4250.5589,S,14718.5084,E,1,04,24.4,19.7,M,,,,0000*1F
	//search the location of second comma symbol	
	
	unsigned int Time_Loc=0;
	
  
	Time_Loc=Find_Useful_location(Frame,2);
	
	if(Frame[Time_Loc-3]=='.')
		Time_Loc-=9;
	else if(Frame[Time_Loc-4]=='.')
		Time_Loc-=10;
	else Time_Loc=0;
	
	if (Time_Loc!=0)
	{
		for(int i=0;i<6;i++)
		UTC_Time[i]=Frame[Time_Loc++];
	}
	
}



extern void GPS_RMC_Parse(unsigned char *Frame,unsigned char * UTC_Time,unsigned char * UTC_Date)
{
	
	//$GPRMC,024813.640,A,3158.4608,N,11848.3737,E,10.05,324.27,150706,,,A*50
	int Time_Loc=0;
	unsigned int Date_Loc=0;
	
	Time_Loc=Find_Useful_location(Frame,2);
  
	if(Frame[Time_Loc-3]=='.')
		Time_Loc-=9;
	else if(Frame[Time_Loc-4]=='.')
		Time_Loc-=10;
	else Time_Loc=0;
	
	if (Time_Loc!=0)
	{
		for(int i=0;i<6;i++)
		UTC_Time[i]=Frame[Time_Loc++];
	}
	
	Date_Loc=Find_Useful_location(Frame,10);
	
	if(Frame[Date_Loc-1]!=',')
		Date_Loc-=6;
	else Date_Loc=0;
	
	if (Date_Loc!=0)
	{
		for(int i=0;i<6;i++)
		UTC_Date[i]=Frame[Date_Loc++];
	}
	
}

void GPS_GGA_Parse_BCD(unsigned char * Buf)
{
	
		
	GPS_GGA_Parse(Buf,GPS_UTC_Time);
	
	GPS_Time_BCD[0]=(GPS_UTC_Time[0]-0x30)*16+(GPS_UTC_Time[1]-0x30);   //hour
	GPS_Time_BCD[1]=(GPS_UTC_Time[2]-0x30)*16+(GPS_UTC_Time[3]-0x30);   //min
	GPS_Time_BCD[2]=(GPS_UTC_Time[4]-0x30)*16+(GPS_UTC_Time[5]-0x30);   //second
	
	
	
	GPS_Hour=GPS_Time_BCD[0];
	GPS_Min=GPS_Time_BCD[1];
	GPS_Sec=GPS_Time_BCD[2];
	
	
}



void  GPS_RMC_Parse_BCD(unsigned char * Buf)
{
	
	
	
	GPS_RMC_Parse(Buf,GPS_UTC_Time,GPS_UTC_Date);
	
	GPS_Time_BCD[0]=(GPS_UTC_Time[0]-0x30)*16+(GPS_UTC_Time[1]-0x30);   //hour
	GPS_Time_BCD[1]=(GPS_UTC_Time[2]-0x30)*16+(GPS_UTC_Time[3]-0x30);   //min
	GPS_Time_BCD[2]=(GPS_UTC_Time[4]-0x30)*16+(GPS_UTC_Time[5]-0x30);   //second
	
	
	GPS_Date_BCD[0]=(GPS_UTC_Date[0]-0x30)*16+(GPS_UTC_Date[1]-0x30);    //day/month/year
	GPS_Date_BCD[1]=(GPS_UTC_Date[2]-0x30)*16+(GPS_UTC_Date[3]-0x30);    //day/month/year
	GPS_Date_BCD[2]=(GPS_UTC_Date[4]-0x30)*16+(GPS_UTC_Date[5]-0x30);    //day/month/year
	
	GPS_Hour=GPS_Time_BCD[0];
	GPS_Min=GPS_Time_BCD[1];
	GPS_Sec=GPS_Time_BCD[2];
	
	GPS_Year=GPS_Date_BCD[2];
	GPS_Month=GPS_Date_BCD[1];
	GPS_Day=GPS_Date_BCD[0];

	
}




