#include "common.h"
#include "XUI.h"

int16  XUI_line_inty_outx(int16 sx, int16 sy, int16 ex, int16 ey,int16 inty)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=ex-sx; //计算坐标增量 
	delta_y=ey-sy; 
	uRow=sx; 
	uCol=sy; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{ 
		if(uCol==inty)return uRow;
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
	return 32767;
} 

void swap(int16 *v1,int16 *v2)
{
	int16 temp;
	temp=*v1;
	*v1=*v2;
	*v2=temp;
}

void sort(int16 *x,int16 *y)  //三者坐标从到到小排序
{
	if(y[0]<y[1])//y0<y1     
	{
		swap(&y[0],&y[1]);//y0>y1   60  30  90
		swap(&x[0],&x[1]);
		if(y[1]<y[2])   //y2>y0
		{
			swap(&y[1],&y[2]); //    60 90 30
			swap(&x[1],&x[2]);
			if(y[0]<y[1])//
			{
				swap(&y[0],&y[1]);//
				swap(&x[0],&x[1]);//
			}		
		}
	}
	else
	{
		if(y[1]<y[2])
		{
			swap(&y[1],&y[2]);	
			swap(&x[1],&x[2]);//
		}
		if(y[0]<y[1])
		{
			swap(&y[0],&y[1]);
			swap(&x[0],&x[1]);//
		}
	}
}

#include "math.h"
float a=0;  
void triangle()
{
	float x=0;
	float y=0;
	int16 i=0;
//	int16 value=0;
	int16 ratio=0;
	
	ratio=(CH_out[0]-1500)/5.0;
	
	if(ratio>=0)y=(ratio*2+100)/100.0;
	else if(ratio<0)y=(300+2*ratio)/300.0;

	
	XUI_rectangle(60,0,159,79,PC);
	XUI_line(110,0,110,79,SC);
	XUI_line(60,40,159,40,SC);	
	for(i=0;i<100;i++)
	{
		x=(i-50)/50.0;
		if(x>=0)XUI_point(i+60,40-pow(x,y)*40,PC);
		else    XUI_point(i+60,40+pow(-x,y)*40,PC);
	}

//	for(i=0;i<100;i++)
//	{
//		x=i/100.0;
//		XUI_point(i+60,80-pow(x,y)*80,PC);
//		
//	}	
	
//	int16 i=0;   //扫描Y值
//	int16 x1,x2;  //左右边界点
//	int16 x[3]={80,12,30,},y[3]={20,40,60};  //三角形坐标
//	
//	/////////////////显示坐标
////	for(i=0;i<3;i++)
////	{
////		XUI_num(0+30*i,0,x[i],3,12,1,PC,BC);
////		XUI_num(80+30*i,0,y[i],3,12,1,PC,BC);
////	}
//	/////////产生动态坐标/////////////
//	a+=0.05;
//	x[0]=sin(a)*30+30;
//	y[0]=cos(a)*20+30;
//	x[1]=sin(1.2*a-pi/2)*60+110;
//	y[1]=cos(1.2*a-pi/2)*30+40;	
//	x[2]=sin(-a*2.7)*50+80;
//	y[2]=cos(-a*2.7)*15+60;
//	
//	sort(x,y);																				//从大到小排序
//	for(i=y[2];i<y[0];i++) 														//从下之上扫描
//	{
//		if(i<=y[1])             												//拐点之前
//		{
//			x1=XUI_line_inty_outx(x[2],y[2],x[1],y[1],i);	//输入Y,返回X1
//			x2=XUI_line_inty_outx(x[2],y[2],x[0],y[0],i);	//输入Y,返回X2
//		}
//		else                   													//拐点之后
//		{
//			x1=XUI_line_inty_outx(x[1],y[1],x[0],y[0],i);	//输入Y,返回X1
//			x2=XUI_line_inty_outx(x[2],y[2],x[0],y[0],i);	//输入Y,返回X2
//		}
//		XUI_line_horizontal(x1,x2,i,PC;		         	//画水平线
//	}
	XUI_num(0,60,fps,2,12,1,PC,BC);                 //显示帧率
}






