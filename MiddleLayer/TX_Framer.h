#ifndef TXFramer_Unit
#define TXFramer_Unit

#define   START_SYMBOL    '$'
#define   END_SYMBOL			'*'

#define   SIMKey_FlashLeft   		10
#define   SIMKey_FlashMid    		22
#define   SIMKey_FlashRight  		21
#define   SIMKey_DianDong				25
#define   SIMKey_Mode						27
#define   SIMKey_MorseRelease   0x66


extern void 					TXFramer_SendSync(void);
extern void 					TXFramer_SendSimKey(unsigned char BId,unsigned char Keynum);
extern unsigned char 	TXFramer_CalChecksum(unsigned char *str,int length);
#endif
