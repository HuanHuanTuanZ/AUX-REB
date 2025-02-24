#include "sbus.h"
#include "delay.h"
#include "gpio.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "dma.h"

#define DMA_USE 0
u8 packet[25];
extern u16 CH_out[16];

void sbus_init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);			//

	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  // 子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);							  // 根据指定的参数初始化VIC寄存器
	// USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;										// 串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_9b;						// 字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_2;							// 2个停止位
	USART_InitStructure.USART_Parity = USART_Parity_Even;							// 无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Tx;									// 收发模式
	USART_Init(USART3, &USART_InitStructure);										//
	USART_Cmd(USART3, ENABLE);														//
}


u8 sbus_sta = 0;
u16 sbus_value[16];
u8 sbus_buf[25];
u8 sbus_status = 0;

void sbus_unpackage()
{
	u8 i;
	u16 value_temp[16];
	value_temp[0] = ((sbus_buf[1] | sbus_buf[2] << 8) & 0x07FF);
	value_temp[1] = ((sbus_buf[2] >> 3 | sbus_buf[3] << 5) & 0x07FF);
	value_temp[2] = ((sbus_buf[3] >> 6 | sbus_buf[4] << 2 | sbus_buf[5] << 10) & 0x07FF);
	value_temp[3] = ((sbus_buf[5] >> 1 | sbus_buf[6] << 7) & 0x07FF);
	value_temp[4] = ((sbus_buf[6] >> 4 | sbus_buf[7] << 4) & 0x07FF);
	value_temp[5] = ((sbus_buf[7] >> 7 | sbus_buf[8] << 1 | sbus_buf[9] << 9) & 0x07FF);
	value_temp[6] = ((sbus_buf[9] >> 2 | sbus_buf[10] << 6) & 0x07FF);
	value_temp[7] = ((sbus_buf[10] >> 5 | sbus_buf[11] << 3) & 0x07FF);

	value_temp[8] = ((sbus_buf[12] | sbus_buf[13] << 8) & 0x07FF);
	value_temp[9] = ((sbus_buf[13] >> 3 | sbus_buf[14] << 5) & 0x07FF);
	value_temp[10] = ((sbus_buf[14] >> 6 | sbus_buf[15] << 2 | sbus_buf[16] << 10) & 0x07FF);
	value_temp[11] = ((sbus_buf[16] >> 1 | sbus_buf[17] << 7) & 0x07FF);
	value_temp[12] = ((sbus_buf[17] >> 4 | sbus_buf[18] << 4) & 0x07FF);
	value_temp[13] = ((sbus_buf[18] >> 7 | sbus_buf[19] << 1 | sbus_buf[20] << 9) & 0x07FF);
	value_temp[14] = ((sbus_buf[20] >> 2 | sbus_buf[21] << 6) & 0x07FF);
	value_temp[15] = ((sbus_buf[21] >> 5 | sbus_buf[22] << 3) & 0x07FF);
	for (i = 0; i < 16; i++)
	{
		value_temp[i] = (value_temp[i] + 1408) / 1.6;
		if (value_temp[i] >= 500 && value_temp[i] <= 2500)
		{
			sbus_value[i] = value_temp[i];
			sbus_status = 5;
		}
	}
}

void USART1_IRQHandler(void) // 串口1中断服务程序
{
	u8 res;
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) //??????
	{
		res = USART_ReceiveData(USART1); // 读取数据
		if (sbus_sta)
		{
			sbus_buf[sbus_sta] = res;
			if (sbus_sta < 24)
				sbus_sta++;
			else
			{
				sbus_sta = 0;
				sbus_unpackage();
			}
		}
		if (sbus_sta == 0)
		{
			if (res == 0x0f)
			{
				sbus_buf[sbus_sta] = res;
				sbus_sta = 1;
			}
		}
	}
}
