
#include "xuan.h"

#if USB

/*************************************************************
*	name: usb_hw.c
*	mail:findstr@sina.com
* 	(c) findstr	2012 3/15
*************************************************************/
#include "stm32f10x.h"
#include "usbreg.h"
#include "usbcore.h"
#include "usbuser.h"
#include "usbcfg.h"
#include "usb.h"
#include "usb_hw.h"
#include "usbdesc.h"
#include "hid.h"
#include "hiduser.h"

#include "stdio.h"


#pragma diag_suppress 111,1441

//用来指示USB设备的状态
static WORD	USB_DeviceStatus;
//用来存储设备的地址
static BYTE	USB_DeviceAddress;
//用来存储USB当前使用的设备的配置
BYTE	USB_Configuration;
//此配置使用端点
static DWORD USB_EndPointMask;
//用于标志此端点是否已经被停止0~15依次代表15个端点
static DWORD USB_EndPointHalt;
//此配置使用的接口数
static BYTE  USB_NumInterfaces;
//每个接口可用的当前接口可替换值
//static BYTE  USB_AltSetting[USB_IF_NUM];


/*用于临时存储控制传输时的数据包*/
USB_SETUP_PACKET SetupPacket;

/*用于向主机发送数据的EP0的数据结构*/
USB_EP_DATA EP0Data;
/*EP0Buf用于向USB发送数据时用的缓冲区*/
BYTE	EP0Buf[USB_MAX_PACKET0];

/*功能:复位USB的一些数据标志
 *参数:无
 *返回值:无
 */
void	USB_ResetCore(void)
{
	//默认为总线供电,因为我们的USB现在不都是插在电脑上才能工作的么&^_^
	USB_DeviceStatus 	=	USB_POWER;	
	//在枚举之初地址当然是0
	USB_DeviceAddress	=	0;
	//配置描述符的标识从1开始,这里也先置为0
	USB_Configuration	=	0;
	//目前使用的是端点０
	USB_EndPointMask	= 	0x00010001;
	//没有停止的端点
	USB_EndPointHalt	=	0x00000000;
}

/*功能:建立阶段,读取建立数据包
 *参数:无
 *返回值:无
 */
void USB_SetupStage(void)
{
	 USB_ReadEP(0x00,(BYTE*)&SetupPacket,0);
}
/*功能:建立阶段,In握手包
 *参数:无
 *返回值:无
 */
void USB_StatusInStage(void)
{
	 USB_WriteEP(0x80,NULL,0);
}
/*功能:建立阶段,Out握手包
 *参数:无
 *返回值:无
 */
void USB_StatusOutStage(void)
{
//	 USB_ReadEP(0x00,EP0Buf);
		 USB_ReadEP(0x00,EP0Buf,0);
}
/*功能:数据In阶段
 *参数:无
 *返回值:无
 */
void USB_DataInStage(void)
{
	DWORD	cnt;
	//先计算引次要发送多少数据
	if(EP0Data.Count > USB_MAX_PACKET0)
		cnt	= USB_MAX_PACKET0;
	else
		cnt	= EP0Data.Count;
	//这里写端点却主机读,则将Dir位置1
   	cnt	= USB_WriteEP(0x80,EP0Data.pData,cnt);
	EP0Data.pData	+=	cnt;
	EP0Data.Count	-=	cnt;
}
/*功能:数据Out阶段
 *参数:无
 *返回值:无
 */
void USB_DataOutStage(void)
{
	 DWORD	cnt;

//	 cnt	=	USB_ReadEP(0x00,EP0Data.pData);
	 cnt	=	USB_ReadEP(0x00,EP0Data.pData,0);
	 EP0Data.pData+=cnt;
	 EP0Data.Count-=cnt;
}
/*功能:获取USB设备的状态
 *参数:无 
 *返回值:TRUE	--->成功
 *		 FALSE	--->错误
 */
__inline BOOL	USB_GetStatus(void)
{
	DWORD	n,m;
	
	switch(SetupPacket.bmRequestType.BM.Recipient)
	{
		//接收端是设备
		case REQUEST_TO_DEVICE:
			//返回设备状态给他
			EP0Data.pData = (BYTE *)&USB_DeviceStatus;
			//将状态信息发送给主机
			USB_DataInStage();
			break;
		//接收端是接口
		case REQUEST_TO_INTERFACE:
			/*配置描述符的标识从1开始,并且请求的接口号不能大于接口的数目,因为接口数目从0开始
			 *因为我们接口描述符中的接口号是一个字节所以这里wIndex中的数据中低字节有效
			 */
			if((USB_Configuration !=0)&&(SetupPacket.wIndex.WB.L < USB_NumInterfaces))
			{	
				//清0两个字节,因为根据USB协议此处必须返回0
				*((__packed WORD *)EP0Buf) = 0;	
				EP0Data.pData = EP0Buf;
				//发送给主机
				USB_DataInStage();
			}
			//此接口出现了错误
			else
				return	FALSE;
			break;
		case REQUEST_TO_ENDPOINT:
			//端点号高1位0方向,低4位为端点号
			n	= SetupPacket.wIndex.WB.L & 0x8f;
			//m的高16位代表in端点的标志,低16位代表out端点的标志
			m	= (n&0x80)?(1<<16 )<< (n&0x0f) :(1<<n);
			//如果配置标识不为0,或配置标识为0,但是是端点0时,才算正常
			if((USB_Configuration !=0)||((n&0x0f)==0)&&(USB_EndPointMask & m))
			{
				//查看此端点是否已经停用
				*((__packed WORD *)EP0Buf) = (USB_EndPointHalt &m )?1:0;
				EP0Data.pData = EP0Buf;
				//将数据发送给主机
				USB_DataInStage();
			}
			//说明配置描述符出了问题
			else
				return	FALSE;
			break;
		default:
			return	FALSE;
	}	
  	return TRUE;
}
/*功能:设置/清除USB设备的特征
 *参数:sc
 			0----->清除
			1----->设置
 *返回值:TRUE	--->成功
 *		 FALSE	--->错误
 */
__inline BOOL USB_SetClrFeature(DWORD	sc)
{
	DWORD	n,m;
	switch(SetupPacket.bmRequestType.BM.Recipient)
	{
		//接收端是设备,则清除或设置设备的特性
		case REQUEST_TO_DEVICE:
			if(SetupPacket.wValue.W == USB_FEATURE_REMOTE_WAKEUP)
			{
				if(sc)
				{
					printf("设置设备远程唤醒特性\r\n");
					//设置USB状态为使能远程唤醒
					USB_DeviceStatus |= USB_GETSTATUS_REMOTE_WAKEUP;
					/*stm32硬件本身就支持远程唤醒,这里就不用设置了
					 *当然,软件支不支持在于对中断屏蔽位的设置
					 */
				}	
				else
				{
					printf("清除设备远程唤醒特性\r\n");
					USB_DeviceStatus &= ~USB_GETSTATUS_REMOTE_WAKEUP;
					/*stm32硬件本身就支持远程唤醒,这里就不用设置了
					 *当然,软件支不支持在于对中断屏蔽位的设置
					 */
				}
			}
			//不支持的请求
			else
			{
				printf("不支持的请求\r\n");
				return FALSE;
			}
			break;
		//接收端是接口
		case REQUEST_TO_INTERFACE:
			//接口不支持清除特性
			printf("清楚设置接口特征不被USB2.0支持\r\n");
			return 	FALSE;
		//接收端是端点
		case REQUEST_TO_ENDPOINT:
			//取得端点号
			n = SetupPacket.wIndex.WB.L & 0x8F;
//			printf("请求的端点是:");
//			printhex(n);
//			printf("USB_Configuration:");
//			printhex(USB_Configuration);
//			printf("USB_EndPointMask");
//			printhex(USB_EndPointMask);
//			printf("\r\n");
			//将端点转化成与EndPointMask一样的格式
			m =	(n & 0x80)?((1<<16) << (n&0x0f)) : (1<<n);
			/*根据USB协议来说,
			 *地址状态下只有端点0才能使用,
			 *配置状态下可用
			 *不知道他这句是不是bug
			 *原文件引用:
			 *This request is valid when the device is in the Address state;
			 *references to interfaces or to endpoints other than endpoint zero 
			 *shall cause the device to respond with a Request Error
			 */
			 /*	此处认为只用配置状态下的非０端点才能清楚特征 ，至于地址状态下没有处理
			  *
			  */
			if((USB_Configuration !=0) && ((n&0x0f)!=0) && (USB_EndPointMask & m))
			{
				//设置/清楚 某个端点stall状态
				if(SetupPacket.wValue.W == USB_FEATURE_ENDPOINT_STALL)
				{
					//设置
					if(sc)
					{
						printf("设置端点STALL\r\n");
						//设置请求的端点为stall
						USB_SetStallEP(n);
						//更新USB的状态位
						USB_EndPointHalt |= m;	
					}
					//清楚
					else
					{
						printf("清除端点%dSTALL\r\n", n);
						USB_ClrStallEP(n);
						USB_EndPointHalt &= ~m;
					}

				}
				//未定义的请求
				else
				{
					printf("不正确的端点请求\r\n");
					return FALSE;
				}
			}
			//不正确的请求时机
			else
			{
				printf("不正确的请求时机\r\n");
				return	FALSE;
			}
			break;
		//未定义的接收端类型
		default:
				printf("未定义的接收端类型\r\n");
				printf("接收端为:");
				printf("%x", SetupPacket.bmRequestType.BM.Recipient);
				printf("\r\n");
				return	FALSE;
			
	}
	return TRUE;
}
/*功能:用于向主机发送描述符
 *参数:无 
 *返回值:TRUE	--->成功
 *		 FALSE	--->发生了错误
 */
__inline BOOL	USB_GetDescriptor()
{
	BYTE 	* pD;
	DWORD	len,n;

	switch (SetupPacket.bmRequestType.BM.Recipient)
	{
		//发给设备的请求
		 case REQUEST_TO_DEVICE:
		 	switch(SetupPacket.wValue.WB.H)
			{
				//获取设备描述符
				case USB_DEVICE_DESCRIPTOR_TYPE:
				  	printf("获取设备描述符\r\n");
					EP0Data.pData = (BYTE *)USB_DeviceDescriptor;
					len	= USB_DEVICE_DESC_SIZE;
					break;
				//获取配置描述符
				case USB_CONFIGURATION_DESCRIPTOR_TYPE:
				  	printf("获取USB配置描述符\r\n");
					pD = (BYTE *)USB_ConfigDescriptor;
				/* 引用USB协议:The range of values used for a
				 * descriptor index is from 0 to one less than 
				 * the number of descriptors of that type implemented by the device
				 * 配置描述符中的标识是从1开始计的,但是,请求中的值是标识值为配置描述符
				 */
				 /*由于可能有几个配置描述符所以可能需要描述一下指针*/
					for(n=0;n!= SetupPacket.wValue.WB.L;n++)
					{
						if(((USB_CONFIGURATION_DESCRIPTOR *)pD)-> bLength != 0)
						{
							pD+=((USB_CONFIGURATION_DESCRIPTOR *)pD)->wTotalLength;
						}
					}
					//如果请求的描述符为空则返回stall
					if(((USB_CONFIGURATION_DESCRIPTOR *)pD)->bLength == 0)
						return	FALSE;
					//设置要传送的指针
					EP0Data.pData = pD;
					//虽然配置描述符中多了一个字节,但是wTotalLength中没有算上,所以这里不用管,直接用就行了
					len = ((USB_CONFIGURATION_DESCRIPTOR*)pD)->wTotalLength;
				  	break;
				//字符串描述符
				case USB_STRING_DESCRIPTOR_TYPE:
				  	printf("获取字符串描述符\r\n");
					EP0Data.pData = (BYTE *)USB_StringDescriptor + SetupPacket.wValue.WB.L;
					len = ((USB_STRING_DESCRIPTOR *)EP0Data.pData)->bLength;
					break;
				default:
					return	FALSE;
			}
			break;
		//请求接收端接口
		case REQUEST_TO_INTERFACE:
			printf("请求接收端接口\r\n");
			switch(SetupPacket.wValue.WB.H)
			{
				//Hid 描述符
				case HID_HID_DESCRIPTOR_TYPE:  
					printf("请求HID描\r\n");
					break;
				case HID_REPORT_DESCRIPTOR_TYPE:
					printf("请求报告描述符\r\n");
					//这里只有一个报告描述符,报告描述符的索引是从0开始计,跟数组是一样的,如是不是0则返回Stall
					if(SetupPacket.wIndex.WB.L != 0)
						return FALSE;
					EP0Data.pData = (BYTE *)HID_ReportDescriptor;
					printf("报告描述符请求的字节:");
					printf("%x", EP0Data.Count);
					printf("报告描述符本来的长度:");
					printf("%x", HID_ReportDescSize);
					printf("\r\n");
					len = HID_ReportDescSize;
					break;
				default:
					printf("未识别的请求!!!###,类型是:");
					printf("%x", SetupPacket.wValue.WB.H);
					printf("\r\n");
					break;
			}
			break;
		default:
			printf("USB_GetDescriptor false\r\n");
			return	FALSE;
 	}

	printf("获取描述符成功\r\n");
	//将数据发送给主机
	if(EP0Data.Count > len)
		EP0Data.Count = len;
	USB_DataInStage();
	return 	TRUE;
}
/*功能:设置配置请求
 *参数:无
 *返回值:TRUE 	--- >成功
 *		 FALSE	--- >失败
 */
__inline BOOL USB_SetConfiguration(void)
{
	USB_COMMON_DESCRIPTOR * pD;
	DWORD	alt,n,m;
	printf("设置配置\r\n");
	//配置值不能为0
	if(SetupPacket.wValue.WB.L)
	{
		pD=(USB_COMMON_DESCRIPTOR *)USB_ConfigDescriptor;
		//循环到配置描述符中的最后一个端点描述符
		while(pD->bLength)
		{
			 switch(pD->bDescriptorType)
			 {
			 	//此时是配置描述符
			 	case USB_CONFIGURATION_DESCRIPTOR_TYPE:	
					//如果此配置描述符是主机要设置的配置描述符
					if(((USB_CONFIGURATION_DESCRIPTOR * )pD)->bConfigurationValue == SetupPacket.wValue.WB.L)
					{
						//保存起此时,设备所用的配置值
						USB_Configuration = SetupPacket.wValue.WB.L;
						//保存起此时，设备所用的配置的接口数目
						USB_NumInterfaces = ((USB_CONFIGURATION_DESCRIPTOR*)pD)->bNumInterfaces;
						//清０每个接口可替换设置值
						for(n=0;n<USB_IF_NUM;n++)
						{
							//USB_AltSetting[n]=0;	
						}	
						//我们将所有的以前可能使用的端点禁用，如果需要，在端点描述符中再使能就行了
						//这样如果更改了配置就保证了,不会使用到以前配置使用,而本次配置没有使用的端点
						//可能是为了防止干扰,猜的^-^!
						for(n=1;n<16;n++)
						{
							if(USB_EndPointMask & (1<<n))
								USB_DisableEP(n);
							if(USB_EndPointMask & ((1<<16)<<n))
								USB_DisableEP(n|0x80);
						}
						//还没有枚举到端点因此这里只使用的是端点0
						USB_EndPointMask = 0x00010001;
						//刚配置,当然没有停止的端点啊,全新的&_^
						USB_EndPointHalt = 0x00000000;
						/*在复位时,我们默认是总线供电,现在开始正式运行以前要因为主机会根据我们的配
						 *置描述符进行调整,一旦配置描述符返回后,配置描述符就生效了,所以这里必须来依据实际
						 *来重新设置
						 */
						if(((USB_CONFIGURATION_DESCRIPTOR *)pD)->bmAttributes & USB_CONFIG_SELF_POWERED)
							USB_DeviceStatus |= USB_GETSTATUS_SELF_POWERED;
						else
							USB_DeviceStatus &= ~USB_GETSTATUS_SELF_POWERED;
					}
					//否则略过,下一个配置描述符
					else
					{
						(BYTE *)pD += ((USB_CONFIGURATION_DESCRIPTOR *)pD)->wTotalLength;
						continue;		
					}
					break;
				//下面是接口描述符
				case USB_INTERFACE_DESCRIPTOR_TYPE:
					alt = ((USB_INTERFACE_DESCRIPTOR *)pD)->bAlternateSetting;
					break;
				case USB_ENDPOINT_DESCRIPTOR_TYPE:
					//我们仅仅需要处理多个子接口中的第一个就行了,(如果有子接口的话,没有的话刚好也是0)
					if(alt == 0)
					{
						//得到此端点
						n = ((USB_ENDPOINT_DESCRIPTOR *)pD)->bEndpointAddress & 0x8f;
						//将端点号转化成bit以存储在USB_EndPointMask中去代表我们使用了此端点
						m = (n&0x80)?((1<<16)<<(n&0x0f)):(1<<n);
						USB_EndPointMask |= m;
						//配置端点
						USB_ConfigEP((USB_ENDPOINT_DESCRIPTOR  *)pD);
						//使能端点
						USB_EnableEP(n);
					    //复位端点,使端点处于确定的端点
						USB_ResetEP(n);
					}
					break;
			 }
		(BYTE *)pD += pD->bLength;
		}	
	}
	//如果请求的配置为0,则我们将所有的配置恢复到地址状态时的配置
	else
	{
		USB_Configuration = 0;
		/*这里不包括0端点*/
		for(n=1;n<16;n++)
		{
			//如果上一个配置使用了此端点,则disable it
			if(USB_EndPointMask & (1<<n)) //OUT端点
				 USB_DisableEP(n);
			if(USB_EndPointMask & (1<<16)<<n)  //IN端点
				 USB_DisableEP(n | 0x80);
		}	
		//清除使用的全部端点,(当然0端点还是除外的)
		USB_EndPointMask = 0x00010001;
		USB_EndPointHalt = 0x00000000;
	}
	//我们根据USB_Configuration的值来查看我们到底时否找到了所需要的值
	if(USB_Configuration == SetupPacket.wValue.WB.L)
	{	
		printf("设置配置成功\r\n");
		return	TRUE;
	}
	else
	{
		printf("设置配置失败\r\n");
		return	FALSE;
	}
}

/*功能:用于处理端点0的输入和输出请求函数
 *参数: USB_EVT_SETUP 枚举时上位机发送的数据包
 *      USB_EVT_OUT	  普通传输时,上位机发送的数据包
 *		USB_EVT_IN	  普通传输时,上位机请求发送数据
 *返回值:无
 */
void USB_EndPoint0 (DWORD event)
{
//	printf("进入端点0处理函数\r\n");
	switch (event)
	{
		//控制传输
		case USB_EVT_SETUP:
		//	printf("进入建立阶段\r\n");
			//首先读取主机发来的请求
			USB_SetupStage();
		   	//设置EP0的数据结构
			EP0Data.Count = SetupPacket.wLength;
			//识别是何请求
//			printf("请求是");
//			printhex(SetupPacket.bRequest);
//			printf("    被请求对象:");
//			printhex(SetupPacket.bmRequestType.B);
//			printf("\r\n");
			//bRequestType.bit5~bit6代表请求类型
			switch(SetupPacket.bmRequestType.BM.Type)
			{
				//标准请求
				case REQUEST_STANDARD:
					switch(SetupPacket.bRequest)
					{
					//获取USB状态
					case USB_REQUEST_GET_STATUS:
						 //如果状态失败则停止0x81端点
					 	if(!USB_GetStatus())
						{
							printf("获取状态失败\r\n");
							goto	stall_i;
						}
						break;
					case USB_REQUEST_CLEAR_FEATURE:
						//如果清除特征失败则停止0x81端点
						if(!USB_SetClrFeature(0))
						{
							printf("清楚特征失败\r\n");
							goto stall_i;	
						}
						//成功了握一下手OK
						USB_StatusInStage();	
						break;
					case USB_REQUEST_SET_FEATURE:
						//如果状态失败则停止0x81端点
						if(!USB_SetClrFeature(1))
						{
							printf("设置特征失败\r\n");
							goto stall_i;	
						}	
						//成功了握一下手OK
						USB_StatusInStage();
						break;
					case USB_REQUEST_SET_ADDRESS:
						switch(SetupPacket.bmRequestType.BM.Recipient)
						{
							//只支持向设备设置地址
							/*USB协议指出必须在改地址前完成检测并对此信息包进行响应
							 *即USB的设置地址阶段必须在主机接收到0长度数据包后才能进行
							 *当主机接收到状态阶段的0数据包时将会产生接收中断
							 *所以放到了USB_EVT_IN处*/
							case REQUEST_TO_DEVICE:
								printf("设置地址\r\n");
								USB_DeviceAddress = 0x80 | SetupPacket.wValue.WB.L;
								USB_StatusInStage();
						   		break;
							default:
								printf("未识别的接收端,接收端是");
								printf("%x", SetupPacket.bmRequestType.BM.Recipient);
								printf("\r\n");
								goto	stall_i;
						}
						break;
					//获取设置描述符
					case USB_REQUEST_GET_DESCRIPTOR:
						if(!USB_GetDescriptor())
							goto	stall_i;
						break;
					//设置配置请求
					case USB_REQUEST_SET_CONFIGURATION:
						switch(SetupPacket.bmRequestType.BM.Recipient)
						{
							//对设备
						 	case REQUEST_TO_DEVICE:
								if(!USB_SetConfiguration())
								{
									//如果失败则对主机返回stall
									goto stall_i;
								}
								//完成了请求,握个手OK
								USB_StatusInStage();
								break;
							//不支持对其他类型请求
							default:
								goto stall_i;
						}
						break;
					case USB_REQUEST_GET_INTERFACE:
						printf("请求接口\r\n");
						break;
					default:
						printf("未识别的请求,请求代码是");
						printf("%x", SetupPacket.bRequest);
						printf("\r\n");
						break;
					}
				break;
				//类请求
				case REQUEST_CLASS:
					printf("类请求:");
					printf("%x", SetupPacket.bmRequestType.B);
					printf("\r\n");
					switch (SetupPacket.bmRequestType.BM.Recipient)
					{
						//对接口请求
						case REQUEST_TO_INTERFACE:
//							printhex( SetupPacket.wIndex.WB.L);
							if(SetupPacket.wIndex.WB.L == 0x00)
							{
								switch(SetupPacket.bRequest)
								{
									case HID_REQUEST_GET_REPORT:
										printf("获取报告描述符\r\n");
										break;
									case HID_REQUEST_SET_REPORT:
										printf("设置报告描述符\r\n");
										break;
									case HID_REQUEST_GET_IDLE:
										printf("获取空闲率\r\n");
										break;
									case HID_REQUEST_SET_IDLE:
										printf("设置空闲\r\n");
										break;
								    case HID_REQUEST_GET_PROTOCOL:
										printf("查询报告协议是否在活动\r\n");
										break;				
									case HID_REQUEST_SET_PROTOCOL:
										printf("SetReprot\r\n");
										break;	
									default:
										printf("不支持的HID请求\r\n");
										break;
						   		}
							}
							break;
						default:
							printf("不是REQUEST_TO_INTERFACE\r\n");
							break;
					}
					break;
				//产商请求
				case REQUEST_VENDOR:
					printf("REQUEST_VENDOR\r\n");
					 break;
				//未定义请求
				default:
					printf("未定义请求\r\n");
		stall_i:
					USB_SetStallEP(0x80);
					break;
			}
			break;
		//接收成功
		case USB_EVT_OUT:
			printf("USB_EVT_OUT\r\n");
			if(SetupPacket.bmRequestType.BM.Dir ==0)
			{
				printf(" 请求是:");
				printf("%x", SetupPacket.bRequest);
				printf("\r\n");
			}
			//返回握手
			else
			{
				USB_StatusOutStage();
			}
			break;
		//发送成功
		case USB_EVT_IN:
		//	printf("USB_EVT_IN\r\n");
			if(SetupPacket.bmRequestType.BM.Dir == 1)
				USB_DataInStage();
			/*USB协议指出必须在改地址前完成检测并对此信息包进行响应
			 *即USB的设置地址阶段必须在主机接收到0长度数据包后才能进行
			 *当主机接收到状态阶段的0数据包时将会产生接收中断
			 *所以放到了USB_EVT_IN处*/
			 //原keil库没有加if判断,怀疑为bug,这里判断是否是由于设置地址的状态阶段成功而触发的中断
			 else// if(SetupPacket.bRequest == USB_REQUEST_SET_ADDRESS)
			 {
				if(USB_DeviceAddress & 0x80)
				{
					USB_DeviceAddress &= 0x7f;
					USB_SetAddress(USB_DeviceAddress);
				}		 	
			 }
			break;
	}

}

#endif


