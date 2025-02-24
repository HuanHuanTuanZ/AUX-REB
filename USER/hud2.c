


//#include "open_tel_mavlink.h"
#include "common.h"
//#include "lcd.h"
#include "XUI.h"
#include "usart.h"
#include "math.h"

#define pi 3.141592f




int16 x_center=0;
int16 y_center=0;

int16 x_min=0;
int16 y_min=0;
int16 x_max=0;
int16 y_max=0;

float distance(int16 x1,int16 y1,int16 x2,int16 y2)
{
	return sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
}

void XUI_line_roll_circle(int16 x1,int16 y1,int16 x2,int16 y2,int16 roll,u8 c)
{
	static int16 x=0;
	static int16 y=0;
	static int16 xx=0;
	static int16 yy=0;
  int16 x0=x_center;
	int16 y0=y_center;
	float a;
	float dis=distance(x1,y1,x0,y0);
	roll+=180;
	
	a=acos((x1-x0)/dis)+roll*pi/180;
	x=x0+cos(a)*dis;
	if(y1>y0)x=2*x0-x;
	y=y0+sin(a)*dis*(y1<y0?1:-1);		
	
	dis=distance(x2,y2,x0,y0);
	a=acos((x2-x0)/dis)+roll*pi/180;
	xx=x0+cos(a)*dis;
	if(y2>y0)xx=2*x0-xx;
	yy=y0+sin(a)*dis*(y2<y0?1:-1);		
	XUI_line(x,y,xx,yy,c);	
}


void XUI_roll_circle(int16 roll,u8 c)
{	
	XUI_line_roll_circle(40,4,40,9,roll,c);
	XUI_line_roll_circle(40,4,40,9,roll+15,c);
	XUI_line_roll_circle(40,4,40,9,roll+30,c);
	XUI_line_roll_circle(40,4,40,9,roll+45,c);
	XUI_line_roll_circle(40,4,40,9,roll+60,c);
	XUI_line_roll_circle(40,4,40,9,roll-15,c);
	XUI_line_roll_circle(40,4,40,9,roll-30,c);
	XUI_line_roll_circle(40,4,40,9,roll-45,c);
	XUI_line_roll_circle(40,4,40,9,roll-60,c);
}


void XUI_pitch_circle(int16 pitch,int16 roll,u8 c1,u8 c2)
{
	int16 angle[19];
	u8 i=0;	
	for(i=0;i<19;i++)
	{
		angle[i]=-90+i*10+pitch;
		if(angle[i]>=-15&&angle[i]<=15)
		{
			if(i==9)
			{
				XUI_line(20,40+angle[i],59,40+angle[i],c1);
				XUI_num8(14,40+angle[i]-4,0,1,1,c2,0);
				XUI_num8(60,40+angle[i]-4,0,1,1,c2,0);
			}
			else 
			{
				XUI_line(30,40+angle[i],49,40+angle[i],c1);
				XUI_num8(18,40+angle[i]-4,abs(i-9)*10,2,1,c2,0);
				XUI_num8(49,40+angle[i]-4,abs(i-9)*10,2,1,c2,0);
			}
		}	
	}
}

void XUI_ground_circle(int16 roll,int16 pitch,u8 c1,u8 c2)
{
	float a;
	float k;
	int16 b;
	int16 y1;
//	int16 y2;
	int16 x1;
//	int16 x2;
	int16 x,y;
	b=pitch;	
	a=roll*3.1415926/180.0;	
	x1=cos(a)*40-sin(a)*b;
	y1=sin(a)*40+cos(a)*b;
	k=tan(a);
	b=y1-k*x1;
	
	if(c1==c2)
	{
//#if USE_animation == 0
//		XUI_clear(BLACK8);
//#else
//		for(x=0;x<160;x++)
//			XUI_line_l(x,0,80,BLACK8);		
//#endif
	}
	else 
	{
#if USE_animation == 0
		XUI_fill(0,0,79,79,c2);	
#endif
		
		if(roll>0&&roll<90)
		{
			for(x=40;x>=-40;x--)
			{
				y=k*x+b;
				if(y<=-40)break;
				if(y>=40)y=40;
				XUI_line_l(x+40,0,y+40,c1);
			}
#if USE_animation == 1				
			for(x=-40;x<=40;x++)
			{
				y=k*x+b;
				if(y>=40)break;			
				if(y<=-40)y=-40;
				XUI_line_l(x+40,y+40,80,c2);
			}
#endif 			
		}
		if(roll<=0&&roll>-90)
		{
			for(x=-40;x<=40;x++)
			{
				y=k*x+b;
				if(y>=40)y=40;
				if(y<=-40)break;
				XUI_line_l(x+40,0,y+40,c1);
			}
#if USE_animation == 1	
			for(x=40;x>=-40;x--)
			{
				y=k*x+b;
				if(y>=40)break;			
				if(y<=-40)y=-40;
				XUI_line_l(x+40,y+40,80,c2);
			}
#endif			
		}
		else if(roll==-90)
		{
			for(x=-40;x<x1;x++)
			{			
				XUI_line_l(x+40,0,80,c1);
			}
#if USE_animation == 1	
			for(x=x1;x<80;x++)
			{			
				XUI_line_l(x+40,0,80,c2);
			}
#endif			
		}
		else 	if(roll==90)
		{
			for(x=40;x>=x1;x--)
			{			
				XUI_line_l(x+40,0,80,c1);
			}
#if USE_animation == 1				
			for(x=-40;x<x1;x++)
			{			
				XUI_line_l(x+40,0,80,c2);
			}
#endif			
		}
		else 	if(roll<-90&&roll>-180)
		{
			for(x=-40;x<=40;x++)
			{
				y=k*x+b;
				if(y>=40)break;			
				if(y<=-40)y=-40;
				XUI_line_l(x+40,y+40,80,c1);
			}
#if USE_animation == 1				
			for(x=40;x>=-40;x--)
			{
				y=k*x+b;
				if(y<=-40)break;
				if(y>=40)y=40;
				XUI_line_l(x+40,0,y+40,c2);
			}
#endif			
		}
		else 	if(roll>90&&roll<180)
		{
			for(x=40;x>=-40;x--)
			{
				y=k*x+b;
				if(y>=40)break;			
				if(y<=-40)y=-40;
				XUI_line_l(x+40,y+40,80,c1);
			}
#if USE_animation == 1			
			for(x=-40;x<=40;x++)
			{
				y=k*x+b;
				if(y>=40)y=40;
				if(y<=-40)break;
				XUI_line_l(x+40,0,y+40,c2);
			}
#endif			
		}
		else 	if(roll==180)
		{
			for(x=-40;x<40;x++)
				XUI_line_l(x+40,y1+40,80,c1);
#if USE_animation == 1				
			for(x=-40;x<=40;x++)
				XUI_line_l(x+40,0,y1+39,c2);
#endif
		}
		
		XUI_line_l(0,0,79,WHITE8);
		XUI_line_l(80,0,79,WHITE8);
		for(x=-39;x<=39;x++)
		{
			u8 y=sqrt(39*39-x*x);
			XUI_line_l(x+40,0,40-y,WHITE8);
			XUI_line_l(x+40,y+40,79,WHITE8);
		}	
	}
}

u8 hud_type3=0;
void draw_yaw(int16 yaw)
{
	u8 i;
	float a;
	
	x_center=120;
	y_center=40;
	
	XUI_circle(120,40,39,BLACK8);	
	if(hud_type3==0||hud_type3==2)
	{
		for(i=0;i<24;i++)
		{
			a=(i*15)*pi/180;
			if(i%2==0)continue;
			XUI_line(80+sin(a)*38+40,40+cos(a)*38,80+sin(a)*35+40,40+cos(a)*35,BLACK8);//»­¿Ì¶È
		}
		for(i=0;i<12;i++)
		{
			a=(i*30)*pi/180;
			XUI_line(80+sin(a)*38+40,40+cos(a)*38,80+sin(a)*32+40,40+cos(a)*32,BLACK8);//»­¿Ì¶È	
		}
		
		XUI_char(117,8,'N',12,1,RED8,0);
		XUI_char(88,34,'W',12,1,RED8,0);
		XUI_char(117,60,'S',12,1,RED8,0);
		XUI_char(146,34,'E',12,1,RED8,0);
		XUI_line_roll_circle(120,20,105,55,yaw,BLACK8);
		XUI_line_roll_circle(120,20,134,55,yaw,BLACK8);
		XUI_line_roll_circle(105,55,120,40,yaw,BLACK8);
		XUI_line_roll_circle(134,55,120,40,yaw,BLACK8);
	}
	else if(hud_type3==1||hud_type3==3)
	{
		for(i=0;i<24;i++)
		{
			a=(i*15+yaw)*pi/180;
			if(i%2==0)continue;	
			XUI_line(80+sin(a)*38+40,40+cos(a)*38,80+sin(a)*35+40,40+cos(a)*35,BLACK8);//»­¿Ì¶È
		}
		for(i=0;i<12;i++)
		{
			a=(i*30+yaw)*pi/180;
			XUI_line(80+sin(a)*38+40,40+cos(a)*38,80+sin(a)*32+40,40+cos(a)*32,BLACK8);//»­¿Ì¶È	
		}
		XUI_line_roll_circle(120,20,105,55,0,BLACK8);
		XUI_line_roll_circle(120,20,134,55,0,BLACK8);
		XUI_line_roll_circle(105,55,120,40,0,BLACK8);
		XUI_line_roll_circle(134,55,120,40,0,BLACK8);		
		
		XUI_char_center(120+sin((yaw+180)*pi/180)*26,40+cos((yaw+180)*pi/180)*26,'N',12,1,RED8,0);
		XUI_char_center(120+sin((yaw+270)*pi/180)*26,40+cos((yaw+270)*pi/180)*26,'W',12,1,RED8,0);
		XUI_char_center(120+sin((yaw+0)*  pi/180)*26,40+cos((yaw+0)*  pi/180)*26,'S',12,1,RED8,0);
		XUI_char_center(120+sin((yaw+90)* pi/180)*26,40+cos((yaw+90)* pi/180)*26,'E',12,1,RED8,0);
	}
	XUI_num(120-len(yaw)*3,50,yaw,len(yaw),12,1,BLACK8,0);
}

void draw_attitude(int16 roll,int16 pitch)
{
	x_center=40;
	y_center=40;
	
	XUI_ground_circle(roll,pitch,LIGHTBLUE8,GREEN8);	
	if(hud_type3==0||hud_type3==1)
	{
		XUI_roll_circle(roll,WHITE8);
		XUI_pitch_circle(pitch,roll,WHITE8,WHITE8);
		XUI_line(8,40,25,40,RED8);
		XUI_line(55,40,72,40,RED8);			
		XUI_line(36,42,40,38,RED8);
		XUI_line(40,38,44,42,RED8);	
		XUI_line(40,10,37,14,RED8);	
		XUI_line(40,10,43,14,RED8);	
		XUI_line(37,14,43,14,RED8);	
		XUI_circle(40,40,39,BLACK8);
		XUI_num8(39-len(roll)*3-(roll<0?3:0),15,roll,len(roll),1,BLACK8,0);
	}
	else if(hud_type3==2||hud_type3==3)
	{
		XUI_3d(3.1416f-hud_pitch/100.0f,0,3.1416f-hud_roll/100.0f,40,BLACK8);	
		XUI_circle(40,40,39,BLACK8);
	}
	
}
	


void hud3()
{
	char temp[20];
	
	x_min=0;
	y_min=0;
	x_max=80;
	y_max=80;		

	XUI_fill(80,0,159,79,WHITE8);	

	draw_attitude(hud_roll,hud_pitch);
	
	draw_yaw(hud_yaw);
	
	XUI_string8(0,8,(u8*)"TX",1,BLACK8,0);	
	XUI_num8(0,0,tx_signal,3,1,BLACK8,0);
	
	XUI_string8(141,0,(u8*)"Gps",1,BLACK8,0);	
	XUI_num8(147,8,hud_gps,2,1,BLACK8,0);

	sprintf(temp,"%-2.1fV",hud_bat/10.0);
	XUI_string8(68,72,(u8*)temp,1,BLACK8,0);		
	
	sprintf(temp,"%4.1fM",hud_alt/10.0f);
	XUI_string8(62,0,(u8*)temp,1,BLACK8,0);	
	
	XUI_string8(147,64,(u8*)"Gs",1,BLACK8,0);	
	XUI_num8(141,72,crsf_ground_speed,3,1,BLACK8,0);
	
	XUI_string8(0,64,(u8*)"Hz",1,BLACK8,0);	
	XUI_num8(0,72,fps,2,1,BLACK8,0);
	
	if(ESC)
	{
		ESC=0;
		hud_type3++;
		if(hud_type3>3)hud_type3=0;
		save16(33,hud_type3);
	}
}


