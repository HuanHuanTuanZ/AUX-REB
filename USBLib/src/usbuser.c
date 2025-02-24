

#include "xuan.h"

#if USB

/*************************************************************
*	name: usb_hw.c
*	mail:findstr@sina.com
* 	(c) findstr	2012 3/10
*************************************************************/
#include "stm32f10x.h"
#include "usbreg.h"
#include "usbcfg.h"
#include "usbcore.h"

#include "usbuser.h" 
#include "stdio.h"
#include "usb_hw.h"

#include "key.h"

/*使用此宏来自动生成端点函数
 *但是有一点值得失考的是，USB_EP_EVENT 为0x07
 *即，当n>3时，就会直接返回为NULL，不知道keil的库意欲何为
 */
//#define	P_EP(n)	((USB_EP_EVENT & (1<< (n))) ? USB_EndPoint##n:NULL)
#define	P_EP(n)	((USB_EP_EVENT & (n)||(!n)) ? USB_EndPoint##n : NULL)

/*使用P_EP(n)来生成端点的函数名	\
 *只用端点0 -->用来枚举
 *和端点1   -->用来传输数据
 */
void (* const USB_P_EP[16])(DWORD event)=
{
	P_EP(0),
	P_EP(1)
} ;

void USB_EndPoint1 (DWORD event)
{
	if (event == USB_EVT_IN) {
//		printf("端点1USB_EVT_IN\r\n");
// 		if (key_press_status()) {
// 			u8 buf[2]={0}; 
// 			USB_WriteEP(0x81,buf,2);
// 		}
	}
	else
	{
		u8 buf[4]={0};  
//		printf("端点1USB_EVT_OUT\r\n");
		USB_ReadEP(0x01,buf,4);	
	}
}

#endif


