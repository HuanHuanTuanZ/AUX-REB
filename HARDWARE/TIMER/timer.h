#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//???????'?????????G????;
//ALIENTEK??STM32???
//??? ????  
//????@ALIENTEK
//???:www.openedv.com
//???u:2012/9/3
//????0
//???????c
//Copyright(C) ????????????? 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   


void TIM1_Int_Init(u16 ttt,u16 sss);//???4
void TIM4_Int_Init(u16 arr,u16 psc);
void TIM3_Int_Init(u16 arr,u16 psc);
void TIM2_Int_Init(u16 ttt,u16 sss);
void TIM5_Int_Init(u16 ttt,u16 sss);//???4

void BEEP_Init(u16 arr,u16 psc);
void IPS_Init(u16 arr,u16 psc);

void backlight(u8 light);
void beep(u16 freq,u16 time);


#endif
