/*************************************************************
*	name: usb_hw.c
*	mail:findstr@sina.com
* 	(c) findstr	2012 3/9
*************************************************************/
#include "type.h"
#include "usb.h"

#ifndef	_USB_HW_H__
#define	_USB_HW_H__

void USB_Init(void);
void USB_Connect(BOOL turnon);
//DWORD USB_ReadEP(DWORD EPnum,BYTE * pData);
DWORD USB_ReadEP(DWORD EPnum,BYTE * pData,DWORD cnt);
DWORD USB_WriteEP(DWORD EPNum , BYTE * pData,DWORD cnt);
void USB_SetStallEP(DWORD	EPNum);
void USB_ClrStallEP(DWORD	EPNum);
void USB_SetAddress(DWORD	addr);
void USB_DisableEP(DWORD EPNum);
void USB_EnableEP(DWORD EPNum);
void EP_ResetEP(DWORD EPNum);
void USB_ResetEP(DWORD EPNum);
void USB_ConfigEP (USB_ENDPOINT_DESCRIPTOR * pEPD);
#endif
