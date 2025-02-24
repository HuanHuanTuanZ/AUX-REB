/*************************************************************
*	name: usb_hw.c
*	mail:findstr@sina.com
*************************************************************/
#include "stm32f10x.h"
#include "usbreg.h"

#ifndef 	  	_USB_CORE_H__
#define	  		_USB_CORE_H__
void 	USB_EndPoint0(DWORD event);
void	USB_ResetCore(void);

/* USB端点数据结构*/
typedef struct _USB_EP_DATA {
  BYTE  *pData;
  WORD   Count;
} USB_EP_DATA;

#endif
