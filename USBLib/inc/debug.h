#ifndef	_DEBUG_H__
#define	_DEBUG_H__

#include <stm32f10x_type.h>
/*如果定义了_DEBUG_宏则定义用于调试的USART输出的一系列函数*/	
extern  void PrintS(u8 *str);
extern  void PrintHex(u8 ch);

#ifdef _DEBUG_

#define	printf(a)	PrintS(a)
#define	printhex(a) PrintHex(a)
#else
#define	printf(a)
#define	printhex(a)
#endif

#endif
