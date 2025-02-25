#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK精英STM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 


#define ppm             PBout(10)

#define menu_up         PBin(6)
#define menu_down       PBin(3)
#define menu_left       PBin(4)
#define menu_right      PAin(15)
#define menu_ok         PBin(5)
#define menu_esc        PBin(2)  //PB12 ---> PB2


#define ch7        PBin(2)
#define ch8        PCin(15)
#define ch9        PCin(14)
#define ch10       PCin(13)


#define sport_dir       PBout(7)


void Gpio_Init(void);//初始化
void ch7_init(void);
		 				    
#endif
