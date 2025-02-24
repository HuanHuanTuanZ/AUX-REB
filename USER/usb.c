#include "common.h"

#if USB
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------USBÐÅºÅÊä³ö-------------------------------------------------/----------------------*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

u8 sendbuf[13];
void Get_USB_Buffer()
{
	u8 i;
	for(i=0;i<13;i++)
		sendbuf[i] = 0;

	for(i=0;i<8;i++)
		sendbuf[i]=(CH_out[i]-1000)/4;

	if(CH_out[0]>1750||CH_out[1]>1750)sendbuf[8]=0;
	if(CH_out[0]<1250||CH_out[1]>1750)sendbuf[8]=0;
	if(CH_out[0]>1750||CH_out[1]>1750)sendbuf[8]=0;
	if(CH_out[0]>1750||CH_out[1]>1750)sendbuf[8]=0;
	
	for(i=0;i<8;i++)
		if(CH_out[8+i]>1750)sendbuf[9]|=0x01<<i;
	for(i=0;i<8;i++)
		if(CH_out[16+i]>1750)sendbuf[10]|=0x01<<i;	
	for(i=0;i<8;i++)
		if(CH_out[24+i]>1750)sendbuf[11]|=0x01<<i;		
	for(i=0;i<8;i++)
		if(CH_out[i]>1750)sendbuf[12]|=0x01<<i;	
}
void USB_out()
{
	Get_USB_Buffer();
	USB_WriteEP(0x81,sendbuf,13); 	
}

#endif
