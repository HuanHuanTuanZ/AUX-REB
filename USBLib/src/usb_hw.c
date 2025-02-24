#include "xuan.h"

#if USB
/*************************************************************
*	name: usb_hw.c
*	mail:findstr@sina.com
* 	(c) findstr	2012 3/9
*************************************************************/
#include "stm32f10x.h"

#include "usbreg.h"
#include "usbuser.h"
#include "usbcore.h"
#include "usb_hw.h"
#include "stdio.h"
#include "usbcfg.h"

/*端点缓冲区的开始地址
 *因为每个缓冲块都需要一个端点描术表
 *而所有的端点描述表放在，USB缓冲区的首部
 *此地址是相对于USB缓冲区的地址，我认为加上Offset更好些
 *这里使用2个端点
 *端点0与端点1
 *此时EP_BUF_ADDR指向缓冲区的内容
 */
#define	EP_BUF_ADDR	(sizeof(EP_BUF_DSCR)*USB_EP_NUM)

/*USB缓冲区首地址包括缓冲区描述表,绝对地址*/
EP_BUF_DSCR	* pBUF_DSCR = (EP_BUF_DSCR *) USB_PMA_ADDR;

/*端点空闲缓冲区地址 
 *用于指示目前为止USB缓冲区中还没有分配的空闲地址的首地址*/
WORD	FreeBufAddr;

	
/*功能:用于初始化USB的时钟等部分
 *参数:无
 *返回值：无
 */
void USB_Init(void)
{
	//printf("进入USB_Init，进行初始化\r\n");
	//使能USB时钟
	RCC->APB1ENR |= (1<<23);

	//使能USB中断
	/*因为USB低优先级中断的中断号为20，而NVIC——IPRX
	 *寄存器用四位来存储中断优先级，所以20/4＝5 ,
	 *然后使能第20位中断*/
	NVIC->IP[5] |=0x10;
	NVIC->ISER[0]|=(1<<20);
}
/*功能:用于复位USB模块	  
 *参数:无
 *返回值：无
 */
/*现在以我的水平还搞不懂双缓冲为何物，所以先不搞^-^*/
/*一些资料：
  *USB低优先级中断(通道20):可由所有USB事件触发(正确传输,USB复位等).
  *USB高优先级中断(通道19):仅能由同步和双缓冲批量传输事件触发,目的是保证最大的传输速率.
  *USB唤醒中断(通道42)：由USB挂起模式的唤醒事件触发.  OTG_FS_WKUP唤醒
  *
  *复位要执行的内容可以参见rm0008 21.4.2节
  */
extern const BYTE USB_DeviceDescriptor[];
void USB_Reset(void)
{
	//printf("USB_Reset\r\n");
	/*复位了嘛，那所有以前产生的中断都没有用了，清了完事！*/
	ISTR=0;

	/*通过设置CNTR来控制stm32的USB模块的工作方式
	 *所有的USB事件中断都是在低优先级中断(通道20)上处理的
	 *好吧就先使能这么多吧，先跑起来再说！
	 */
	CNTR= 		CNTR_CTRM	|	// 使能正确传输中断
		 	CNTR_RESETM 	|	//使能复位中断
			CNTR_SUSPM	|	//使能挂起中断
			CNTR_WKUPM	;	//使能唤醒中断
	
	FreeBufAddr	=	EP_BUF_ADDR; //此时FreeBuff指向第一个缓冲区首地址(不包括描述符表)，相对地址

	BTABLE	= 0x00;				 //设置缓冲区描述表的位置仍是相对地址
	
	/*为端点0设置缓冲区及各种控制位*/
	pBUF_DSCR->ADDR_TX = FreeBufAddr;
	FreeBufAddr += USB_MAX_PACKET0;		//端点0设置为8个字节，一般控制数据为8个字节
	pBUF_DSCR->ADDR_RX = FreeBufAddr;
	FreeBufAddr += USB_MAX_PACKET0;
	
	/*在count_Rx字段中10~14bit用来表示缓冲区字节的快数
	 *而15bit用来表示块的大小
	 *0---2byte
	 *1---1byte
	 *我们这里使用了8个字节，bit15为0,所以应该((8<<10)>>1)即8<<9;
	 *至于count_Rx我们在发送时再来赋值
	 */
	pBUF_DSCR->COUNT_RX= USB_MAX_PACKET0 << 9;	
	/*设置端点0为控制端点，接收缓冲区有效
	 *低四位代表端点地址
	 */
	EPxREG(0) = EP_CONTROL | EP_RX_VALID;

	/*使能USB模块,并设置USB地址为0，以响应枚举*/
	DADDR = DADDR_EF | 0;
	//printf("Leave USB_Reset\r\n");
}

/*功能:复位一个端点
 *参数:	端点号
 *				EPNum:bit3~bit0	----> 端点号
 * 				EPNum:bit7		----> 端点方向
 *
 *返回值:无
 */
 /*其实就是将下一个要发送的数据包变成DATA0*/
 void	EP_Reset(DWORD	EPNum)
 {
 	DWORD	num,var;
   	//printf("EP_Reset\r\n");
	/*获得端点号,低四位为端点号*/
	num = EPNum & 0x0F;
	var = EPxREG(num);
	/*如果bit7为1则将此端点的发送toggle置为0，
	 *否则将此端点的接收toggle置为0
	 *因为数据总是从data0数据包开始发送的
	 */
	 if(EPNum & 0x80)
	 	EPxREG(num) = var & (EP_MASK | EP_DTOG_TX);/*输入端点*/
	 else
	 	EPxREG(num) = var & (EP_MASK | EP_DTOG_RX);/*输出端点*/
	 
 }
 /*功能：连接或断开USB功能
  *参数：true -->连接USB
  *      false-->关闭USB
  *返回值:无
  */
void USB_Connect(BOOL turnon)
{
	/*需要注意一点的事，所有的USB寄存器尽量用＝而不要用与或非
	 *在编程手册上有明确表明，这样可能会导至出一些问题*/
	//printf("进入连接USB程序\r\n");
	 /*将USB强制复位*/
	CNTR = CNTR_FRES;
	//printf("test1\r\n");
	/*因为刚连接所以应该跟才启动一样，将所有没有处理的中断给清理掉*/
	ISTR=0;
	//printf("test2\r\n");
	if(turnon)
	{
		//printf("test3\r\n");
		/*使能GPIOA,然后将PA.8置低,使USB
		*的D+加1.5K上接电阻,使USB集线器识别了高速设备
		*这样才能让USB识别
		*/
		RCC->APB2ENR |= (1 << 2);                 	 /* enable clock for GPIOA */
		GPIOA->CRH &= ~(0x0f << 0 * 4);                /* clear port PA8 */
		GPIOA->CRH |=  (0x03 << 0 * 4);                /* PA6 General purpose output open-drain, max speed 50 MHz */
		GPIOA->BRR  =  (   1 << 8    );                /* reset PA8  (set to low) */	 	
		/*经过调试发现，这个语句的本意是：复位USB模块
		*然后在此使能CNTR_RESETM即复位中断标志
		*至于端点0的初始化留在USB低优先级中进行处理
		*当然，我们也只开了低优先级中断^_^!*/
		CNTR = CNTR_RESETM;	 /*此处只使能了复位中断，*/	 
	}
	else
		CNTR = CNTR_FRES | CNTR_PDWN ;/*复位并关闭*/
	

}
/*功能:设置端点状态
 *参数:EPnum --->端点号
 *     stat  --->要设置的状态值
 *返回值:无
 */
void USB_ConfigEP (USB_ENDPOINT_DESCRIPTOR * pEPD)
{
 	DWORD num,val;
	
	//取得端点号
	num = pEPD->bEndpointAddress & 0xf;

	val = pEPD->wMaxPacketSize;
	//如果是IN端点
	if(pEPD->bEndpointAddress & USB_ENDPOINT_DIRECTION_MASK)
	{
		//此处我只想说pBUF_DSCR是指针,剩下的就是语法问题了
		(pBUF_DSCR + num)->ADDR_TX = FreeBufAddr;
		/*取2的倍数,因为缓冲区都是字对齐的,注意此处如果大于1023会出现浪费现象
		 *因为USB_COUNTn_TX只能接收bit0~bit9
		 */
		val = (val + 1)& ~1;
	}
	//输出端点
	else
	{
		(pBUF_DSCR + num)->ADDR_RX = FreeBufAddr;
		/*因为USB_COUNTn_RX中存储只用bit10~bit14,如果BLSIZE=0(即块大小为2字节),那么只能是0~62个字节
		 *所以如果大于62,则应将块大小设置为BLSIZE=1(即32个字节)
		 */
		if(val > 62	)
		{
			//块大小为32,则大小应该为32的倍数
			val = (val +31)&~31;
			/*关于此计算公式,参见rm0008,21,5.3节
			 *(val >> 5)<<10 == val <<5
			 */
			(pBUF_DSCR + num)->COUNT_RX = ((val << 5)-1) | 0x8000;
		}
		else
		{
			val = (val + 1) & ~1;
			(pBUF_DSCR + num)->COUNT_RX = val << 9;
		}
	}
	//修正空闲区域的起始地址
	FreeBufAddr += val ;

	switch(pEPD->bmAttributes & USB_ENDPOINT_TYPE_MASK)
	{
		//控制端点
		case USB_ENDPOINT_TYPE_CONTROL:
			val = EP_CONTROL;
			break;
		//同步端点
		case USB_ENDPOINT_TYPE_ISOCHRONOUS:
			val = EP_ISOCHRONOUS;
			break;
		//块传输
		case USB_ENDPOINT_TYPE_INTERRUPT:
			val = EP_INTERRUPT;
			break;
		default:
			//printf("出错了,未识别的端点类型\r\n");
			break;
	}
	val |= num;
	//设置端点寄存器
	EPxREG(num) = val;
 }
/*功能:设置端点状态
 *参数:EPnum --->端点号
 *     stat  --->要设置的状态值
 *返回值:无
 */
 void EP_Status(DWORD EPNum,DWORD stat)
 {
 	DWORD num,var;
	
	/*取得端点号*/
	num = EPNum & 0x0f;
	var = EPxREG(num);
	/*此处用了一点小技巧,因为端点寄存器是写1反转,所以想设置相应的值只有使用异或*/
	if(EPNum & 0x80)	//输入端点
		EPxREG(num)=(var ^ (stat & EP_STAT_TX)) & (EP_MASK | EP_STAT_TX);
	else				//输出端点
		EPxREG(num)=(var ^ (stat & EP_STAT_RX)) & (EP_MASK | EP_STAT_RX);

 }
/*功能:复位端点
 *参数：EPNum 	bit0~bit3为端点号
 			  	bit7	 为端点方向
 *返回值:无
 */
void USB_ResetEP(DWORD EPNum)
{
	EP_Reset(EPNum);
}
/*功能:设置端点为stall
 *参数：EPNum 	bit0~bit3为端点号
 			  	bit7	 为端点方向
 *返回值:无
 */
void USB_SetStallEP(DWORD	EPNum)
{
	EP_Status(EPNum,EP_TX_STALL | EP_RX_STALL);
}
/*功能:设置端点为enable
 *参数：EPNum 	bit0~bit3为端点号
 			  	bit7	 为端点方向
 *返回值:无
 */
void USB_EnableEP(DWORD EPNum)
{
	EP_Status(EPNum,EP_TX_VALID | EP_RX_VALID);
}

/*功能:设置端点为disable
 *参数：EPNum 	bit0~bit3为端点号
 			  	bit7	 为端点方向
 *返回值:无
 */
void USB_DisableEP(DWORD EPNum)
{
	EP_Status(EPNum,EP_TX_DIS | EP_RX_DIS);
}

/*功能:清除端点的stall状态
 *参数：EPNum 	bit0~bit3为端点号
 			  	bit7	 为端点方向
 *返回值:无
 */
void USB_ClrStallEP(DWORD	EPNum)
{
	EP_Status(EPNum,EP_TX_VALID | EP_RX_VALID);
}
/*功能:设置USB地址
 *参数：addr要设置的地址
 *返回值:无
 */
void USB_SetAddress(DWORD	addr)
{
	//DADDR的高1位是用来使能USB模块的
	DADDR = DADDR_EF | addr;
}
/*功能:用于读端点缓冲区
 *参数:EPnum --->端点号
 *     pData --->用于接收从端点缓冲区中读到的数据
 *		此函数有些问题,应该加入一个参数显示缓冲区有多大
 *返回值:读到的字节数
 */
DWORD USB_ReadEP(DWORD EPnum,BYTE * pData,DWORD cnt)
{

	//DWORD num,cnt,*pv,n;
DWORD num,*pv,n;	
	//得到端点号    
	num = EPnum & 0xf;

	//取得些端点的缓冲区地址
	pv=(DWORD *)(USB_PMA_ADDR + 2* ((pBUF_DSCR + num)->ADDR_RX));
	//COUNT_RX低10位存放的是缓冲区的数据
	cnt=(pBUF_DSCR + num)->COUNT_RX & EP_COUNT_MASK;
	for(n=0;n<(cnt+1)/2;n++)
	{
		/*pakced关键字用于单字节对齐,这在USB数据结构中的结构体中尤为重要
		 *因为stm32访问时使用32位,而USB访问时使用16位,所以pData为WORD,而
		 *pv为DWORD型指针
		 */
	 	*((__packed WORD *)pData)=*pv++;
		/*这里pData为单字节指针所以,还是加2而不是加4*/
		pData+=2;
	}
	/*OK,现在我们的端点又可以接收数据了,设置为VALID*/
	EP_Status (EPnum,EP_RX_VALID);
   	return cnt;
}

/*功能:用于写端点缓冲区
 *参数:EPNum --->端点号
 *     pData --->指向要发送的数据缓冲区
 *	   cnt	 --->要写入的字节数
 *返回值:写入的字节数
 */
 DWORD USB_WriteEP(DWORD EPNum , BYTE * pData,DWORD cnt)
 {
 	DWORD num,*pv,n;

	num = EPNum & 0x0f;

  	//pv=(DWORD *)(USB_PMA_ADDR + 2*((pBUF_DSCR+num)->ADDR_TX));
	 pv=(DWORD *)(USB_PMA_ADDR + 2*((pBUF_DSCR+num)->ADDR_TX));

	/*此处应该判断要写入的数据是否超量了,可能会产一个隐藏bug*/
   	for(n=0;n<(cnt + 1)/2;n++)
	{
		*pv++=*((__packed WORD*)pData);
		pData+=2;
	}
	//OK,现在USB发送缓冲区中已经有东西了,可以响应主机了
	(pBUF_DSCR+num)->COUNT_TX=cnt;
	EP_Status(EPNum,EP_TX_VALID);
	return 	cnt;
 }

/*功能:USB挂起
 *参数：无
 *返回值:无
 */
void USB_Suspend(void)
{
	//printf("进入挂起中断\r\n");
	//强制挂起
	CNTR |= CNTR_FSUSP;	
	//进入低功耗模式
	CNTR |= CNTR_LPMODE;
}

/*功能:USB唤醒
 *参数：无
 *返回值:无
 */
void USB_WakeUp(void)
{
 	GPIOE->BRR |=1 ;            /* Turn On Suspend LED */
	//printf("进入唤醒中断\r\n");
	//唤醒了,当然得把这一位给清了
	CNTR &=  ~CNTR_FSUSP;	
	//USB的唤醒事件会复位此位,我们这里不用管
	//CNTR &= ~CNTR_LPMODE;
}
/*功能:USB低优先级中断服务程序
 *参数：无
 *返回值:无
 */
 void USB_LP_CAN1_RX0_IRQHandler(void)
 {
 	DWORD	istr;
	DWORD 	num,var;

	istr=ISTR;	//取得中断标志位
	/*USB复位中断的处理*/
	if(istr & ISTR_RESET)
	{
		//复位设备
		USB_Reset();
		//复位与USB协议有关的数据
		USB_ResetCore();
		ISTR = ~ISTR_RESET;	/*已经处理完复位中断了，清楚复位中断标志*/
	}
	/*USB挂起中断*/
/**********************************************************************/
	if (istr & ISTR_SUSP) {
		USB_Suspend();
		ISTR = ~ISTR_SUSP;
	}

	/* USB Wakeup */
	if (istr & ISTR_WKUP) {
		USB_WakeUp();
		ISTR = ~ISTR_WKUP;
	}

//******************************************************************
	/*端点接中发送中断处理*/
	while((istr = ISTR) & ISTR_CTR) 
	{
		//清楚中断标志位
		ISTR = ~ISTR_CTR;
		//取得发生中断的端点号
		num=istr & ISTR_EP_ID;
		//取得端点寄存器的值		
		var = EPxREG(num);
		//正确的接收传输
		if(var & EP_CTR_RX )
		{
			//清0正确接收标志位
		//	printf("端点号为:");
		//	printhex((u8)num);
			EPxREG(num) = var & ~EP_CTR_RX & EP_MASK;
			//调用相应的端点进行处理
			if(USB_P_EP[num])
			{
				//如果是控制传输,则使有USG_EVT_SETUP
				if(var & EP_SETUP)
					USB_P_EP[num](USB_EVT_SETUP);
				else
					//否则就是普通的输出包
					USB_P_EP[num](USB_EVT_OUT);
			}
		}
		//产生的是发送成功中断
		if(var & EP_CTR_TX)
		{
			//清楚中断标志位
			EPxREG(num) = var & ~EP_CTR_TX & EP_MASK;
			//调用对应的中断函数进行处理
			if(USB_P_EP[num])
			{
				//USB发送成功
				USB_P_EP[num](USB_EVT_IN);	
			}
		}

	}
 }
#endif
