#ifndef CMDParser_Unit
#define CMDParser_Unit

#include "stdbool.h"

#define CMDParser_SYNC           		0x01
#define CMDParser_ALR					0x02
#define CMDParser_MODE	 				0x03



		








extern void CMDParser_COM1_Handler(void);


static void CMDParser_UnPack_SYNC(unsigned char * Buf);
static void CMDParser_UnPack_ALR(unsigned char * Buf);
static void CMDParser_UnPack_MODE(unsigned char * BUF);



static bool CMDParser_VerifyCheckSum(unsigned char *Buf, unsigned char Length);
static unsigned char CMDParser_GetOriginalChecksum(unsigned char P1,unsigned char P2);














#endif
