
#include "timer.h"
#include "common.h"
extern u8 beep_flag;


int16 beep_queue_index[20][2];
u8    beep_queue_seq=0;

u8    beep_queue_flag=0;//队列就绪标志
	
//蜂鸣器队列注册
//倒序注册
void beep_queue_regedit(u16 freq,u16 time)
{
	++beep_queue_seq;
	beep_queue_index[beep_queue_seq][0]=freq;
	beep_queue_index[beep_queue_seq][1]=time;
}

void beep_queue_operation()
{
	if(beep_flag)beep_flag--;
	else 
	{
		beep_queue_flag=0;	
		if(beep_queue_seq==0)beep(0,0);
		else
		{
			if(beep_queue_flag==0)
			{
				beep_queue_flag=1;
				beep(beep_queue_index[beep_queue_seq][0],beep_queue_index[beep_queue_seq][1]);
				beep_queue_seq--;		
			}
		}		
	}	
}

void beep_positive()
{
	beep_queue_regedit(1400,10);
	beep_queue_regedit(0,4);	
	beep_queue_regedit(1000,10);
}

void beep_negative()
{
	beep_queue_regedit(1000,10);
	beep_queue_regedit(0,4);	
	beep_queue_regedit(1400,10);
}



const u16 beep_freq_map[21]={262,294,330,349,392,440,494,523,587,659,698,784,880,988,1046,1175,1318,1397,1568,1760,1976};
u16 beep_freq_temp=0;
	
void beep(u16 freq,u16 time)
{
	if(beep_freq_temp!=freq)
	{
		beep_freq_temp=freq;	
		if(beep_enable)
		{					
			if(freq==0)
			{			
				//TIM_SetCompare3(TIM4,0);	
				TIM_Cmd(TIM4, DISABLE);  //使能TIM4
			}
			else if(freq<21)
			{
				BEEP_Init(1000000/beep_freq_map[freq]-1,CPU_SPEED/2-1);	//84M/84=1Mhz的计数频率,重装载值500，所以PWM频率为 1M/500=2Khz.
				TIM_SetCompare3(TIM4,500000/beep_freq_map[freq]);
			}
			else
			{
				BEEP_Init(1000000/freq-1,CPU_SPEED/2-1);	//84M/84=1Mhz的计数频率,重装载值500，所以PWM频率为 1M/500=2Khz.
				TIM_SetCompare3(TIM4,5000*100/freq);
			}
		}			
		else TIM_SetCompare3(TIM4,0);			
	}
	beep_flag=time;
}



void BEEP_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//使能定时器3时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
	
	GPIO_PinRemapConfig(GPIO_Remap_TIM4, ENABLE); //Timer3部分重映射  TIM4_CH2->PB5    
 
   //设置该引脚为复用输出功能,输出TIM4 CH2的PWM脉冲波形	GPIOB.5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO
 
   //初始化TIM4
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM4 Channel2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM4 OC2

	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIM4在CCR2上的预装载寄存器
 
	TIM_Cmd(TIM4, ENABLE);  //使能TIM4
	
	
	TIM_SetCompare3(TIM4,0);	

}


void backlight(u8 light)
{
	TIM_SetCompare4(TIM4,light*10);	
}

void IPS_Init(u16 arr,u16 psc)
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//使能定时器3时钟
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  | RCC_APB2Periph_AFIO, ENABLE);  //使能GPIO外设和AFIO复用功能模块时钟
	
	GPIO_PinRemapConfig(GPIO_Remap_TIM4, ENABLE); //Timer3部分重映射  TIM4_CH2->PB5    
 
   //设置该引脚为复用输出功能,输出TIM4 CH2的PWM脉冲波形	GPIOB.5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //TIM_CH2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO
 
   //初始化TIM4
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	
	//初始化TIM4 Channel2 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
	
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM4 OC2
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIM4在CCR2上的预装载寄存器
	
	TIM_Cmd(TIM4, ENABLE);  //使能TIM4
	TIM_SetCompare4(TIM4,0);	
}


