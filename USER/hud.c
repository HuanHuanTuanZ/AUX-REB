//#include "open_tel_mavlink.h"
#include "common.h"
//#include "lcd.h"
#include "XUI.h"
#include "usart.h"
#include "math.h"

#define pi 3.141592f


void XUI_yaw(u16 yaw,u8 c1,u8 c2)
{
	u8 b=0,i=0;
	int16 x=0,temp=0;
	b=yaw%30;
	for(i=0;i<11;i++)
	{
//		int16 x2;
	  x=30-b+30*i-11;		
//		x2=x-12; 
//		if(x2>=-20&&x2<=180)XUI_line(x2,11,x2,14,WHITE8);
//		x2=x-6; 
//		if(x2>=-20&&x2<=180)XUI_line(x2,11,x2,14,WHITE8);
//		x2=x+6; 
//		if(x2>=-20&&x2<=180)XUI_line(x2,11,x2,14,WHITE8);
//		x2=x+12; 
//		if(x2>=-20&&x2<=180)XUI_line(x2,11,x2,14,WHITE8);		
	  if(x>=-20&&x<=180)
		{
			//XUI_line(x,9,x,14,WHITE8);
			temp=yaw-b-60+30*i;
			if(temp<0)temp+=360;
			else if(temp>=360)temp-=360;
			//if(x>=62&&x<=98)continue;
			if     (temp==0)  XUI_char8(x-3,0,'N',1,c2,0);
			else if(temp==90) XUI_char8(x-3,0,'E',1,c2,0);
			else if(temp==180)XUI_char8(x-3,0,'S',1,c2,0);
			else if(temp==270)XUI_char8(x-3,0,'W',1,c2,0);
			else XUI_num8(x-len(temp)*3,0,temp,len(temp),1,c1,0);		
		}		
	}
	XUI_fill(80-len(yaw)*3-4,0,80+len(yaw)*3+1,12,c2);
	XUI_rectangle(80-len(yaw)*3-4,0,80+len(yaw)*3+1,12,c1);
	XUI_num(80-len(yaw)*3,0,yaw,len(yaw),12,1,c1,0);
}


void XUI_speed( int16 speed,u8 c1,u8 c2)
{
	u8 i=0;
	int16 b=0,y=0,temp=0;
	b=speed%20;
	for(i=0;i<11;i++)
	{
		u16 y2;
		y=100-b-100+20*i-60;
		y=80-y;
		y2=y-8; 
		if(y2>=20&&y2<=60)XUI_line(36,y2,40,y2,c1);
		y2=y-4; 
		if(y2>=20&&y2<=60)XUI_line(36,y2,40,y2,c1);	
		y2=y+4; 
		if(y2>=20&&y2<=60)XUI_line(36,y2,40,y2,c1);	
		y2=y+8; 
		if(y2>=20&&y2<=60)XUI_line(36,y2,40,y2,c1);		
	  if(y>=20&&y<=60)
		{	
			XUI_line(32,y,40,y,c1);					
			temp=speed-b-100+20*i;							
			if(temp<0){temp=-temp;XUI_char8(24,y-4,'-',1,c1,0);}//RAM_ShowChar(97,320-y-6,'-',12);}
			else XUI_num8(30-len(temp)*6,y-4,temp,len(temp),1,c1,0);	
		}		
	}
	
	XUI_fill(0,34,29,45,c2);
	XUI_rectangle(0,34,29,45,c1);
	XUI_num(14-len(speed)*3,34,speed,len(speed),12,1,c1,0);
}

void XUI_alt(int16 alt,u8 c1,u8 c2)
{
	u8 i=0;
	int16 b=0,y=0,temp=0;
	b=alt%20;
	for(i=0;i<11;i++)
	{
		u16 y2;
		y=100-b-100+20*i-60;
		y=80-y;
		y2=y-8; 
		if(y2>=20&&y2<=60)XUI_line(121,y2,125,y2,c1);
		y2=y-4; 
		if(y2>=20&&y2<=60)XUI_line(121,y2,125,y2,c1);	
		y2=y+4; 
		if(y2>=20&&y2<=60)XUI_line(121,y2,125,y2,c1);	
		y2=y+8; 
		if(y2>=20&&y2<=60)XUI_line(121,y2,125,y2,c1);		
	  if(y>=20&&y<=60)
		{	
			XUI_line(121,y,129,y,c1);					
			temp=alt-b-100+20*i;							
			if(temp<0){temp=-temp;XUI_char8(130,y-4,'-',1,c1,0);XUI_num8(135,y-4,temp,len(temp),1,c1,0);}//RAM_ShowChar(97,320-y-6,'-',12);}
			else XUI_num8(130,y-4,temp,len(temp),1,c1,0);
		}		
	}	
	
	XUI_fill(131,34,159,45,c2);	
	XUI_rectangle(131,34,159,45,c1);	
	if(abs(alt)<100)
	{
		char temp[10];
		sprintf(temp,"%d",alt);
		XUI_str(132+(alt>=0?5:0),34,100,12,(u8*)temp,12,1,c1,0);
	}
	else 
	{
		XUI_num(145-len(alt)*3,34,alt,len(alt),12,1,c1,0);
	}
}


void XUI_line_roll0(int16 x1,int16 y1,int16 x2,int16 y2,int16 roll)
{
	static int16 x=0;
	static int16 y=0;
	static int16 xx=0;
	static int16 yy=0;
  static int16 x0=80;
	static int16 y0=40;
	roll+=180;
	x=x0+cos(acos((x1-x0)/(sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0))))
	+roll*pi/180)*sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
	if(y1>y0)	
	{
		y=y0+sin(acos((x1-x0)/(sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0))))+roll*pi/180)*sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));	
		x=160-x;
	}
	else y=y0+sin(acos((x1-x0)/(sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0))))+roll*pi/180)*sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));		
	xx=x0+cos(acos((x2-x0)/(sqrt((x2-x0)*(x2-x0)+(y2-y0)*(y2-y0))))+roll*pi/180)*sqrt((x2-x0)*(x2-x0)+(y2-y0)*(y2-y0));
	if(y2>y0)
	{	
		yy=y0+sin(acos((x2-x0)/(sqrt((x2-x0)*(x2-x0)+(y2-y0)*(y2-y0))))+roll*pi/180)*sqrt((x2-x0)*(x2-x0)+(y2-y0)*(y2-y0));		
		xx=160-xx;	
	}
	else yy=y0+sin(acos((x2-x0)/(sqrt((x2-x0)*(x2-x0)+(y2-y0)*(y2-y0))))+roll*pi/180)*sqrt((x2-x0)*(x2-x0)+(y2-y0)*(y2-y0));		
  XUI_line(x,y,xx,yy,WHITE8);	
}


void XUI_line_roll(int16 x1,int16 y1,int16 x2,int16 y2,int16 roll,u8 c)
{
	static int16 x=0;
	static int16 y=0;
	static int16 xx=0;
	static int16 yy=0;
  static int16 x0=80;
	static int16 y0=60;
	roll+=180;
	x=x0+cos(acos((x1-x0)/(sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0))))
	+roll*pi/180)*sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
	if(y1>y0)	
	{
		y=y0+sin(acos((x1-x0)/(sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0))))+roll*pi/180)*sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));	
		x=160-x;
	}
	else y=y0+sin(acos((x1-x0)/(sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0))))+roll*pi/180)*sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));		
	xx=x0+cos(acos((x2-x0)/(sqrt((x2-x0)*(x2-x0)+(y2-y0)*(y2-y0))))+roll*pi/180)*sqrt((x2-x0)*(x2-x0)+(y2-y0)*(y2-y0));
	if(y2>y0)
	{	
		yy=y0+sin(acos((x2-x0)/(sqrt((x2-x0)*(x2-x0)+(y2-y0)*(y2-y0))))+roll*pi/180)*sqrt((x2-x0)*(x2-x0)+(y2-y0)*(y2-y0));		
		xx=160-xx;	
	}
	else yy=y0+sin(acos((x2-x0)/(sqrt((x2-x0)*(x2-x0)+(y2-y0)*(y2-y0))))+roll*pi/180)*sqrt((x2-x0)*(x2-x0)+(y2-y0)*(y2-y0));		
  XUI_line(x,y,xx,yy,c);	
}


void XUI_roll(int16 roll,u8 c)
{	
	//roll=-roll;
	XUI_line_roll(80,15,80,20,roll,c);
	XUI_line_roll(80,15,80,20,roll+10,c);
	XUI_line_roll(80,15,80,20,roll+20,c);
	XUI_line_roll(80,15,80,20,roll+30,c);
	XUI_line_roll(80,15,80,20,roll+40,c);
	XUI_line_roll(80,15,80,20,roll-10,c);
	XUI_line_roll(80,15,80,20,roll-20,c);
	XUI_line_roll(80,15,80,20,roll-30,c);
	XUI_line_roll(80,15,80,20,roll-40,c);
}

void XUI_ground(int16 roll,int16 pitch,u8 c1,u8 c2)
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
	x1=cos(a)*100-sin(a)*b;
//	x2=-cos(a)*100-sin(a)*b;	
	y1=sin(a)*100+cos(a)*b;
//	y2=-sin(a)*100+cos(a)*b;	

	k=tan(a);
	b=y1-k*x1;
	
	if(c1==c2)
	{
#if USE_animation == 0
		XUI_clear(BLACK8);
#else
		for(x=0;x<160;x++)
			XUI_line_l(x,0,80,BLACK8);		
#endif
	}
	else 
	{
#if USE_animation == 0
		XUI_clear(c2);
#endif
		if(roll>0&&roll<90)
		{
			for(x=80;x>=-80;x--)
			{
				y=k*x+b;
				if(y<=-40)break;
				if(y>=40)y=40;
				XUI_line_l(x+80,0,y+40,c1);
			}
#if USE_animation == 1				
			for(x=-80;x<80;x++)
			{
				y=k*x+b;
				if(y>=40)break;			
				if(y<=-40)y=-40;
				XUI_line_l(x+80,y+40,80,c2);
			}
#endif 			
		}
		if(roll<=0&&roll>-90)
		{
			for(x=-80;x<80;x++)
			{
				y=k*x+b;
				if(y>=40)y=40;
				if(y<=-40)break;
				XUI_line_l(x+80,0,y+40,c1);
			}
#if USE_animation == 1	
			for(x=80;x>=-80;x--)
			{
				y=k*x+b;
				if(y>=40)break;			
				if(y<=-40)y=-40;
				XUI_line_l(x+80,y+40,80,c2);
			}
#endif			
		}
		else 	if(roll==-90)
		{
			for(x=-80;x<x1;x++)
			{			
				XUI_line_l(x+80,0,80,c1);
			}
#if USE_animation == 1	
			for(x=x1;x<160;x++)
			{			
				XUI_line_l(x+80,0,80,c2);
			}
#endif			
		}
		else 	if(roll==90)
		{
			for(x=80;x>=x1;x--)
			{			
				XUI_line_l(x+80,0,80,c1);
			}
#if USE_animation == 1				
			for(x=-80;x<x1;x++)
			{			
				XUI_line_l(x+80,0,80,c2);
			}
#endif			
		}
		else 	if(roll<-90&&roll>-180)
		{
			for(x=-80;x<80;x++)
			{
				y=k*x+b;
				if(y>=40)break;			
				if(y<=-40)y=-40;
				XUI_line_l(x+80,y+40,80,c1);
			}
#if USE_animation == 1				
			for(x=80;x>=-80;x--)
			{
				y=k*x+b;
				if(y<=-40)break;
				if(y>=40)y=40;
				XUI_line_l(x+80,0,y+40,c2);
			}
#endif			
		}
		else 	if(roll>90&&roll<180)
		{
			for(x=80;x>=-80;x--)
			{
				y=k*x+b;
				if(y>=40)break;			
				if(y<=-40)y=-40;
				XUI_line_l(x+80,y+40,80,c1);
			}
#if USE_animation == 1			
			for(x=-80;x<80;x++)
			{
				y=k*x+b;
				if(y>=40)y=40;
				if(y<=-40)break;
				XUI_line_l(x+80,0,y+40,c2);
			}
#endif			
		}
		else 	if(roll==180)
		{
			for(x=-80;x<80;x++)
				XUI_line_l(x+80,y1+40,80,c1);
#if USE_animation == 1				
			for(x=-80;x<80;x++)
				XUI_line_l(x+80,0,y1+39,c2);
#endif
		}
	}
}

void XUI_pitch(int16 pitch,int16 roll,u8 c1,u8 c2)
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
				XUI_line(60,40+angle[i],99,40+angle[i],c1);
				XUI_num8(54,40+angle[i]-4,0,1,1,c2,0);
				XUI_num8(100,40+angle[i]-4,0,1,1,c2,0);
			}
			else 
			{
				XUI_line(70,40+angle[i],89,40+angle[i],c1);
				XUI_num8(58,40+angle[i]-4,abs(i-9)*10,2,1,c2,0);
				XUI_num8(89,40+angle[i]-4,abs(i-9)*10,2,1,c2,0);
			}

		}	
	}
}
	
u8 hud_type=1;
void hud()
{
	char temp[20];

	x_center=80;
	y_center=40;
	x_min=48;
	y_min=12;
	x_max=111;
	y_max=67;	

	
	XUI_ground(hud_roll,hud_pitch,LIGHTBLUE8,BROWN8);
	XUI_yaw(hud_yaw,WHITE8,DARKBLUE8);	
	XUI_alt(hud_alt,WHITE8,DARKBLUE8);
	XUI_speed(hud_groundspeed,WHITE8,DARKBLUE8);	
	if(ESC)
	{
		ESC=0;
		hud_type=!hud_type;
		save16(31,hud_type);
	}
	

	
	if(hud_type==1)
	{
		XUI_roll(hud_roll,WHITE8);
		XUI_pitch(hud_pitch,hud_roll,WHITE8,GREEN8);
		XUI_line(76,42,80,38,GREEN8);
		XUI_line(80,38,84,42,GREEN8);		
	}	
	else XUI_3d(3.1416f-hud_pitch*pi/180.0,0,3.1416f-hud_roll*pi/180.0f,40,WHITE8);	
	
	XUI_string8(0,8,(u8*)"Gps",1,RED8,BC);	
	XUI_num8(20,8,hud_gps,2,1,GREEN8,0);
	XUI_string8(120,8,(u8*)"Hsd",1,RED8,BC);	
	XUI_num8(140,8,crsf_vertical_speed,2,1,GREEN8,0);
	XUI_num(74,67,fps,2,12,1,WHITE8,0);
	sprintf(temp,"%-2.1fV",hud_bat/10.0);
	XUI_string8(0,63,(u8*)temp,1,RED8,BC);	
	sprintf(temp,"%4.1fA",hud_cur/10.0);
	XUI_string8(130,63,(u8*)temp,1,RED8,BC);	
	sprintf(temp,"%-7.6fE",crsf_longitude/10000000.0);
	XUI_string8(0,71,(u8*)temp,1,WHITE8,BC);	
	sprintf(temp,"%9.6fN",crsf_latitude/10000000.0);
	XUI_string8(98,71,(u8*)temp,1,WHITE8,BC);	

}


u8 hud_type2=1;
void hud2()
{
	char temp[20];

	x_center=80;
	y_center=40;
	x_min=48;
	y_min=12;
	x_max=111;
	y_max=67;	

	XUI_ground(hud_roll,hud_pitch,GREEN8,GREEN8);
	XUI_yaw(hud_yaw,GREEN8,BLACK8);	
	XUI_alt(hud_alt,GREEN8,BLACK8);
	XUI_speed(hud_groundspeed,GREEN8,BLACK8);	
	
	if(ESC)
	{
		ESC=0;
		hud_type2=!hud_type2;
		save16(32,hud_type2);
	}
	
	if(hud_type2==1)
	{
		XUI_roll(hud_roll,GREEN8);
		XUI_pitch(hud_pitch,hud_roll,GREEN8,GREEN8);
		XUI_line(76,42,80,38,GREEN8);
		XUI_line(80,38,84,42,GREEN8);		
	}	
	else XUI_3d(3.1416f-hud_pitch/100.0f,0,3.1416f-hud_roll/100.0f,40,GREEN8);	
	
	XUI_string8(0,8,(u8*)"Gps",1,GREEN8,BLACK8);	
	XUI_num8(20,8,hud_gps,2,1,GREEN8,0);
	XUI_string8(120,8,(u8*)"Hsd",1,GREEN8,BC);	
	XUI_num8(140,8,crsf_vertical_speed,2,1,GREEN8,0);
	XUI_num(74,67,fps,2,12,1,GREEN8,0);
	sprintf(temp,"%-2.1fV",hud_bat/10.0);
	XUI_string8(0,63,(u8*)temp,1,GREEN8,BC);	
	sprintf(temp,"%4.1fA",hud_cur/10.0);
	XUI_string8(130,63,(u8*)temp,1,GREEN8,BC);	
	sprintf(temp,"%-7.6fE",crsf_longitude/10000000.0);
	XUI_string8(0,71,(u8*)temp,1,GREEN8,BC);	
	sprintf(temp,"%9.6fN",crsf_latitude/10000000.0);
	XUI_string8(98,71,(u8*)temp,1,GREEN8,BC);	
//XUI_num(40,61,crsf_baro_altitude,5,12,1,PC,0);
}








void XUI_line_l2(int16 x, int16 sy,int16 ey,u8 color)
{
	int16 dis=abs(ey-sy);
	int16 t=0;
	if(sy>ey)
	{
		ey=sy;
		sy-=dis;
	}
	for(t=0;t<=dis;t++)
	{
		XUI_point2(x,sy+t,color);//»­µã 
	}
}


void XUI_ground2(int16 roll,int16 pitch,u8 c1,u8 c2)
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
	x1=cos(a)*100-sin(a)*b;
//	x2=-cos(a)*100-sin(a)*b;	
	y1=sin(a)*100+cos(a)*b;
//	y2=-sin(a)*100+cos(a)*b;	

	k=tan(a);
	b=y1-k*x1;
	
	if(c1==c2)
	{
#if USE_animation == 0
		XUI_clear(BLACK8);
#else
		for(x=0;x<160;x++)
			XUI_line_l2(x,0,80,BLACK8);		
#endif
	}
	else 
	{
#if USE_animation == 0
		XUI_clear(c2);
#endif
		if(roll>0&&roll<90)
		{
			for(x=80;x>=-80;x--)
			{
				y=k*x+b;
				if(y<=-40)break;
				if(y>=40)y=40;
				XUI_line_l2(x+80,0,y+40,c1);
			}
#if USE_animation == 1				
			for(x=-80;x<80;x++)
			{
				y=k*x+b;
				if(y>=40)break;			
				if(y<=-40)y=-40;
				XUI_line_l2(x+80,y+40,80,c2);
			}
#endif 			
		}
		if(roll<=0&&roll>-90)
		{
			for(x=-80;x<80;x++)
			{
				y=k*x+b;
				if(y>=40)y=40;
				if(y<=-40)break;
				XUI_line_l2(x+80,0,y+40,c1);
			}
#if USE_animation == 1	
			for(x=80;x>=-80;x--)
			{
				y=k*x+b;
				if(y>=40)break;			
				if(y<=-40)y=-40;
				XUI_line_l2(x+80,y+40,80,c2);
			}
#endif			
		}
		else 	if(roll==-90)
		{
			for(x=-80;x<x1;x++)
			{			
				XUI_line_l2(x+80,0,80,c1);
			}
#if USE_animation == 1	
			for(x=x1;x<160;x++)
			{			
				XUI_line_l2(x+80,0,80,c2);
			}
#endif			
		}
		else 	if(roll==90)
		{
			for(x=80;x>=x1;x--)
			{			
				XUI_line_l2(x+80,0,80,c1);
			}
#if USE_animation == 1				
			for(x=-80;x<x1;x++)
			{			
				XUI_line_l2(x+80,0,80,c2);
			}
#endif			
		}
		else 	if(roll<-90&&roll>-180)
		{
			for(x=-80;x<80;x++)
			{
				y=k*x+b;
				if(y>=40)break;			
				if(y<=-40)y=-40;
				XUI_line_l2(x+80,y+40,80,c1);
			}
#if USE_animation == 1				
			for(x=80;x>=-80;x--)
			{
				y=k*x+b;
				if(y<=-40)break;
				if(y>=40)y=40;
				XUI_line_l2(x+80,0,y+40,c2);
			}
#endif			
		}
		else 	if(roll>90&&roll<180)
		{
			for(x=80;x>=-80;x--)
			{
				y=k*x+b;
				if(y>=40)break;			
				if(y<=-40)y=-40;
				XUI_line_l2(x+80,y+40,80,c1);
			}
#if USE_animation == 1			
			for(x=-80;x<80;x++)
			{
				y=k*x+b;
				if(y>=40)y=40;
				if(y<=-40)break;
				XUI_line_l2(x+80,0,y+40,c2);
			}
#endif			
		}
		else 	if(roll==180)
		{
			for(x=-80;x<80;x++)
				XUI_line_l2(x+80,y1+40,80,c1);
#if USE_animation == 1				
			for(x=-80;x<80;x++)
				XUI_line_l2(x+80,0,y1+39,c2);
#endif
		}
	}
}
