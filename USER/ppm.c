#include "common.h"
#include "gpio.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------PPM信号输出------------------------------------------------------/-----------------*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

u16 ppm_value[8];  // ppm输入值
u8 ppm_sta = 0;	   // ppm解析计数器
u8 ppm_status = 0; // ppm输入状态

u16 ppm_sum = 0;
u8 ppm_count = 0;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------PPM定时器-----------------------------------------------------------------------*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TIM2_IRQHandler(void) // TIM32    20ms
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{ // 超时，解析失败或未检测到PPM信号
		u8 i;
		ppm_sta = 0;
		if (ppm_status)
		{
		}
		else
		{
			for (i = 0; i < 8; i++)
				ppm_value[i] = 1500;
		}
	}
	if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET) // 捕获1发生捕获事件
	{
		if (ppm_sta)
		{
			u16 temp = TIM_GetCapture1(TIM2);
			if (temp > 500 && temp < 2500) // 判定范围
			{
				ppm_value[ppm_sta - 1] = temp + 1;
				if (ppm_value[ppm_sta - 1] < 1000)
					ppm_value[ppm_sta - 1] = 1000;
				if (ppm_sta < 8)
					ppm_sta++;
				else
					ppm_sta = 0;
				ppm_status = 5; // 成功检测到PPM信号标志
			}
			else
				ppm_sta = 0;
		}
		if (ppm_sta == 0) // 还未开始解析
		{
			if (TIM_GetCapture1(TIM2) > 3000) // 如果时间大于5000us，
				ppm_sta = 1;				  // 开始解析
		}
		TIM_SetCounter(TIM2, 0); // 清除计数器
	}
	TIM_ClearITPendingBit(TIM2, TIM_IT_CC1 | TIM_IT_Update); // 清除中断标志位
}

/////////////////////////////////////////////////////////////////////////

// 定时器2通道1输入捕获配置

void ppm_Cap_Init(u16 arr, u16 psc)
{
	TIM_ICInitTypeDef TIM2_ICInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, (FunctionalState)1);  // 使能TIM2时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, (FunctionalState)1); // 使能GPIOA时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	  // PA0 清除之前设置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // PA0 输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA, GPIO_Pin_0); // PA0 下拉

	// 初始化定时器2 TIM2
	TIM_TimeBaseStructure.TIM_Period = arr;						// 设定计数器自动重装值
	TIM_TimeBaseStructure.TIM_Prescaler = psc;					// 预分频器
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		// 设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);				// 根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	// 初始化TIM2输入捕获参数
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_1;				 // CC1S=01 	选择输入端 IC1映射到TI1上
	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	 // 上升沿捕获
	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // 映射到TI1上
	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;			 // 配置输入分频,不分频
	TIM2_ICInitStructure.TIM_ICFilter = 0x00;						 // IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM2, &TIM2_ICInitStructure);

	// 中断分组初始化
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;				// TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	// 先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			// 从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = (FunctionalState)1; // IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);								// 根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_ITConfig(TIM2, TIM_IT_Update | TIM_IT_CC1, (FunctionalState)1); // 允许更新中断 ,允许CC1IE捕获中断
	TIM_Cmd(TIM2, (FunctionalState)1);									// 使能定时器5
}
