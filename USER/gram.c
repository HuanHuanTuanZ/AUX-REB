

#include "ips.h"
#include "lcd.h"
#include "gram.h"










//»­¾ØÐÎ
void Gram_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u8 color)
{
	Gram_DrawLine(x1,y1,x2,y1,color);
	Gram_DrawLine(x1,y1,x1,y2,color);
	Gram_DrawLine(x1,y2,x2,y2,color);
	Gram_DrawLine(x2,y1,x2,y2,color);
}


void Gram_fill_circle(int16 x0,u16 y0,u16 r,u16 color)
{											  
	u32 i;
	u32 imax = ((u32)r*707)/1000+1;
	u32 sqmax = (u32)r*(u32)r+(u32)r/2;
	u32 x=r;
	Gram_draw_hline(x0-r,y0,2*r,color);
	for (i=1;i<=imax;i++) 
	{
		if ((i*i+x*x)>sqmax)// draw lines from outside  
		{
 			if (x>imax) 
			{
				Gram_draw_hline(x0-i+1,y0+x,2*(i-1),color);
				Gram_draw_hline(x0-i+1,y0-x,2*(i-1),color);
			}
			x--;
		}
		Gram_draw_hline(x0-x,y0+i,2*x,color);
		Gram_draw_hline(x0-x,y0-i,2*x,color);
	}
}  



