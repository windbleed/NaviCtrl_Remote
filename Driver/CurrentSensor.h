#ifndef CurrentSensor_Unit
#define CurrentSensor_Unit

#define CurrentSensor_Num 20
#define CurrentSensor_Buf_Num  CurrentSensor_Num/4

extern unsigned char CurrentSensor_Frame[CurrentSensor_Buf_Num];

extern unsigned char * CurrentSensor_GetData(void);

extern void CurrentSensor_Init(void);

static void   CurrentSensor_ScanKey(void);



#endif
