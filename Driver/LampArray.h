#ifndef LampArray_Unit
#define LampArray_Unit

#define LampArray_LampNum      20
#define LampArray_Buf_Num      LampArray_LampNum/4 

extern  unsigned char  LampArray_Frame[LampArray_Buf_Num];

extern  void LampArray_Reflash(void);

extern  void LampArray_SetFrame(unsigned char * NewFrame);

extern  void LampArray_Init(void);

#endif
