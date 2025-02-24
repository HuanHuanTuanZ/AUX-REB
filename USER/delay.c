#include "delay.h"




void delay_us(__IO u32 nCount)
{
  for(; nCount != 0; nCount--);
} 

void delay_ms(u16 ms)
{
	u32 i=ms*12000;
  for(;i!=0;i--);
} 

