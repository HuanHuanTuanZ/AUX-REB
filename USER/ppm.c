#include "common.h"
#include "gpio.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------PPM�ź����------------------------------------------------------/-----------------*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ppm_enable() // ����PPM�ź����
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, (FunctionalState)1);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	TIM_Cmd(TIM2, (FunctionalState)1);
}
void ppm_disable() // �ر�PPM�ź����
{
	TIM_Cmd(TIM2, (FunctionalState)0);
}

u8 ppm_mode = 1;   // 1�����룬0�����
u16 ppm_value[8];  // ppm����ֵ
u8 ppm_sta = 0;	   // ppm����������
u8 ppm_status = 0; // ppm����״̬

u16 ppm_sum = 0;
u8 ppm_count = 0;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*---------------------------------PPM1�ź����-----------------------------------------------------------------------*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TIM2_IRQHandler(void) // TIM32    20ms
{
	if (ppm_mode == 0) // PPM���ģʽ
	{
		if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) //??TIM3?????"??
		{
			ppm_count++;
			switch (ppm_count)
			{
			case 1:
				ppm = 1;
				ppm_sum += TIM2->ARR = 400;
				break;
			case 2:
				ppm = 0;
				ppm_sum += TIM2->ARR = CH_out[0] - 400;
				break;
			case 3:
				ppm = 1;
				ppm_sum += TIM2->ARR = 400;
				break;
			case 4:
				ppm = 0;
				ppm_sum += TIM2->ARR = CH_out[1] - 400;
				break;
			case 5:
				ppm = 1;
				ppm_sum += TIM2->ARR = 400;
				break;
			case 6:
				ppm = 0;
				ppm_sum += TIM2->ARR = CH_out[2] - 400;
				break;
			case 7:
				ppm = 1;
				ppm_sum += TIM2->ARR = 400;
				break;
			case 8:
				ppm = 0;
				ppm_sum += TIM2->ARR = CH_out[3] - 400;
				break;
			case 9:
				ppm = 1;
				ppm_sum += TIM2->ARR = 400;
				break;
			case 10:
				ppm = 0;
				ppm_sum += TIM2->ARR = CH_out[4] - 400;
				break;
			case 11:
				ppm = 1;
				ppm_sum += TIM2->ARR = 400;
				break;
			case 12:
				ppm = 0;
				ppm_sum += TIM2->ARR = CH_out[5] - 400;
				break;
			case 13:
				ppm = 1;
				ppm_sum += TIM2->ARR = 400;
				break;
			case 14:
				ppm = 0;
				ppm_sum += TIM2->ARR = CH_out[6] - 400;
				break;
			case 15:
				ppm = 1;
				ppm_sum += TIM2->ARR = 400;
				break;
			case 16:
				ppm = 0;
				ppm_sum += TIM2->ARR = CH_out[7] - 400;
				break;
			case 17:
				ppm = 1;
				ppm_sum += TIM2->ARR = 400;
				break;
			case 18:
				ppm = 0;
				TIM2->ARR = 20000 - ppm_sum;
				ppm_sum = 0;
				ppm_count = 0;
				break;
			default:
				break;
			}
			TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //??TIMx??????
		}
	}
	else
	{
		if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
		{ // ��ʱ������ʧ�ܻ�δ��⵽PPM�ź�
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
		if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET) // ����1���������¼�
		{
			if (ppm_sta)
			{
				u16 temp = TIM_GetCapture1(TIM2);
				if (temp > 500 && temp < 2500) // �ж���Χ
				{
					ppm_value[ppm_sta - 1] = temp + 1;
					if (ppm_value[ppm_sta - 1] < 1000)
						ppm_value[ppm_sta - 1] = 1000;
					if (ppm_sta < 8)
						ppm_sta++;
					else
						ppm_sta = 0;
					ppm_status = 5; // �ɹ���⵽PPM�źű�־
				}
				else
					ppm_sta = 0;
			}
			if (ppm_sta == 0) // ��δ��ʼ����
			{
				if (TIM_GetCapture1(TIM2) > 3000) // ���ʱ�����5000us��
					ppm_sta = 1;				  // ��ʼ����
			}
			TIM_SetCounter(TIM2, 0); // ���������
		}
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1 | TIM_IT_Update); // ����жϱ�־λ
	}
}

/////////////////////////////////////////////////////////////////////////

// ��ʱ��2ͨ��1���벶������

void ppm_Cap_Init(u16 arr, u16 psc)
{
	TIM_ICInitTypeDef TIM2_ICInitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, (FunctionalState)1);  // ʹ��TIM2ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, (FunctionalState)1); // ʹ��GPIOAʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	  // PA0 ���֮ǰ����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // PA0 ����
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA, GPIO_Pin_0); // PA0 ����

	// ��ʼ����ʱ��2 TIM2
	TIM_TimeBaseStructure.TIM_Period = arr;						// �趨�������Զ���װֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc;					// Ԥ��Ƶ��
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		// ����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);				// ����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	// ��ʼ��TIM2���벶�����
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_1;				 // CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	 // �����ز���
	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // ӳ�䵽TI1��
	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;			 // ���������Ƶ,����Ƶ
	TIM2_ICInitStructure.TIM_ICFilter = 0x00;						 // IC1F=0000 ���������˲��� ���˲�
	TIM_ICInit(TIM2, &TIM2_ICInitStructure);

	// �жϷ����ʼ��
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;				// TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	// ��ռ���ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;			// �����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = (FunctionalState)1; // IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);								// ����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_ITConfig(TIM2, TIM_IT_Update | TIM_IT_CC1, (FunctionalState)1); // ��������ж� ,����CC1IE�����ж�
	TIM_Cmd(TIM2, (FunctionalState)1);									// ʹ�ܶ�ʱ��5
}
