#include "common.h"
//#include "lcd.h"
#include "XUI.h"
#include "usart.h"


void XUI_tool1()
{
	statusbar();	
	XUI_str(52,2,64,16,(u8*)"计时器1",16,1,PC,BC);	
	if(ESC==1)
	{
		ESC=0;
		time_use1=!time_use1;
	}
	if(ESC>=20)
	{
		ESC=0;
		time_use1=0;
		time_10ms=0;
	}
	XUI_num0(0,34,time_10ms/360000%24,2,32,1,PC,BC);
	XUI_char(29,31,':',32,1,FC,BC);
	XUI_num0(42,34,time_10ms/6000%60,2,32,1,PC,BC);
	XUI_char(71,31,':',32,1,FC,BC);
	XUI_num0(84,34,time_10ms/100%60,2,32,1,PC,BC);
	XUI_char(113,31,':',32,1,FC,BC);
	XUI_num0(126,34,time_10ms%100,2,32,1,PC,BC);
	
}

void XUI_tool2()
{
	statusbar();	
	XUI_str(52,2,64,16,(u8*)"计时器2",16,1,PC,BC);	
	if(ESC==1)
	{
		ESC=0;
		time_use2=!time_use2;
	}
	if(ESC>=20)
	{
		ESC=0;
		time_use2=0;
		time_10ms2=0;
	}
	XUI_num0(0,34,time_10ms2/360000%24,2,32,1,PC,BC);
	XUI_char(29,31,':',32,1,FC,BC);
	XUI_num0(42,34,time_10ms2/6000%60,2,32,1,PC,BC);
	XUI_char(71,31,':',32,1,FC,BC);
	XUI_num0(84,34,time_10ms2/100%60,2,32,1,PC,BC);
	XUI_char(113,31,':',32,1,FC,BC);
	XUI_num0(126,34,time_10ms2%100,2,32,1,PC,BC);
}

void XUI_tool3()
{
	statusbar();	
	XUI_str(52,2,64,16,(u8*)"计时器3",16,1,PC,BC);	
	if(ESC==1)
	{
		ESC=0;
		time_use3=!time_use3;
	}
	if(ESC>=20)
	{
		ESC=0;
		time_use3=0;
		time_10ms3=0;
	}
	XUI_num0(0,34,time_10ms3/360000%24,2,32,1,PC,BC);
	XUI_char(29,31,':',32,1,FC,BC);
	XUI_num0(42,34,time_10ms3/6000%60,2,32,1,PC,BC);
	XUI_char(71,31,':',32,1,FC,BC);
	XUI_num0(84,34,time_10ms3/100%60,2,32,1,PC,BC);
	XUI_char(113,31,':',32,1,FC,BC);
	XUI_num0(126,34,time_10ms3%100,2,32,1,PC,BC);	
}

#include "math.h"
extern u32 second;
void clock()
{
	u8 i;
	char temp[20];
	XUI_circle(80+40,40,39,PC);//画外圈
	XUI_circle(80+40,40,3,PC);//画内圈
	for(i=0;i<12;i++)XUI_line(80+sin(i*30*pi/180)*39+40,40+cos(i*30*pi/180)*39,80+sin(i*30*pi/180)*34+40,40+cos(i*30*pi/180)*34,PC);//画刻度
	XUI_num8(74+40,2,12,2,1,RC,BC);//画时钟12
	XUI_num8(77+40,70,6,1,1,RC,BC);//画时钟6
	XUI_num8(43+40,36,9,1,1,RC,BC);//画时钟9
	XUI_num8(111+40,36,3,1,1,RC,BC);//画时钟3
	XUI_line(80+40,40,40+80+sin((60-second%60)*6*pi/180+pi)*30,40+cos((60-second%60)*6*pi/180+pi)*30,BLUE8);//画秒针
	XUI_line(80+40,40,40+80+sin((60-(second/60)%60)*6*pi/180+pi)*25,40+cos((60-(second/60)%60)*6*pi/180+pi)*25,GREEN8);//画分针
	XUI_line(80+40,40,40+80+sin((12-(second/3600)%12)*30*pi/180+pi)*20,40+cos((12-(second/3600)%12)*30*pi/180+pi)*20,BLACK8);//画时针
	
	XUI_str(0,0,100,16,(u8*)"BOOT_Time",16,1,PC,BC);//显示时间
	sprintf(temp,"%02d:%02d:%02d",(second/3600)%12,(second/60)%60,second%60);//打印时间到字符串
	XUI_str(0,16,100,16,(u8*)temp,16,1,PC,BC);//显示时间
}












