#include "xuan.h"

#if MAVLINK

#include "sys.h"//??:????  qq:624668529
#include "mavlink_usart_fifo.h"
#include "mavlink_types.h"
#include "open_tel_mavlink.h"
#include "usart.h"

#define UART_TX_BUFFER_SIZE        200
#define UART_RX_BUFFER_SIZE        200
#include "mavlink.h"

fifo_t uart_rx_fifo, uart_tx_fifo;	
uint8_t uart_tx_buf[UART_TX_BUFFER_SIZE], uart_rx_buf[UART_RX_BUFFER_SIZE];

mavlink_system_t mavlink_system;//MAVLINK系统参数

void mavlink_init()
{
	fifo_init(&uart_tx_fifo, uart_tx_buf, UART_TX_BUFFER_SIZE);	//初始化fifo
	fifo_init(&uart_rx_fifo, uart_rx_buf, UART_RX_BUFFER_SIZE);//初始化fifo
}


/** @brief ?FIFO
  * @param fifo ?????
	*        *ch   ?????
	* @return 
	*        ????,1; ???,0
  */
uint8_t fifo_read_ch(fifo_t* fifo, uint8_t* ch)
{
	if(fifo->tail == fifo->head) return false;
	*ch = fifo->buf[fifo->tail];  
	
	if(++fifo->tail >= fifo->length) fifo->tail = 0;
  return true;
}
/** @brief ???????FIFO
  * @param fifo ??????
	*        ch   ??????
	* @return 
	*        ??,1; ????,0
  */
uint8_t fifo_write_ch(fifo_t* fifo, uint8_t ch)
{
	uint16_t h = fifo->head;
	
	if(++h >= fifo->length) h = 0;
	if(h == fifo->tail) return false;
	
	fifo->buf[fifo->head] = ch;
	fifo->head = h;
  return true;
}
/** @brief ???????????
  * @param fifo 
	* @return 
	*        ????
  *
  * @note  ??????????2?,??????
  */
uint16_t fifo_free(fifo_t* fifo)  
{
	uint16_t free;
	
	if(fifo->head >= fifo->tail) free = fifo->tail + (fifo->length - fifo->head);
	else free = fifo->tail - fifo->head;
	
  return free;
}
uint16_t fifo_used(fifo_t* fifo)
{
	uint16_t used;
	
	if(fifo->head >= fifo->tail) used = fifo->head - fifo->tail;
	else used = fifo->head + (fifo->length - fifo->tail);
	
	return used;	
}
/** @brief ??????
  * @param *fifo
  *        *buf 
  *        length
  */
void fifo_init(fifo_t* fifo, uint8_t* buf, uint16_t length)  
{
	uint16_t i;
	
	fifo->buf = buf;
	fifo->length = length;
	fifo->head = 0;
	fifo->tail = 0;
	
	for(i=0; i<length; i++) fifo->buf[i] = 0;	
}
/** @brief ??????,????
  *        
  * @note ??????????,??????,?????,??????
  */
uint8_t serial_write_buf(uint8_t* buf, uint16_t length) {
	uint16_t i;
	
	if(length == 0) return false;
  for(i = 0; length > 0; length--, i++)	{
		fifo_write_ch(&uart_tx_fifo, buf[i]);
	}	
  USART_ITConfig(USART2, USART_IT_TXE, (FunctionalState)1);
	
	return true;
}
/** @brief ?????? 
  * @return ?????
  */
uint8_t serial_read_ch(void){
	uint8_t ch;	
	fifo_read_ch(&uart_rx_fifo, &ch);	
	return ch;
}
/** @breif ????????????? 
  * @return ??????
  */
uint16_t serial_free(void){
	return fifo_free(&uart_tx_fifo);
}
uint16_t serial_available(void){
	return fifo_used(&uart_rx_fifo);
}

void USART2_IRQHandler(void)
{			
  uint8_t c;	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)//??????
  {	
		c = USART_ReceiveData(USART2);  
		fifo_write_ch(&uart_rx_fifo, c);		
    //USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
  }
  if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)//??????
  {   		
		if(fifo_read_ch(&uart_tx_fifo, &c)) 
			USART_SendData(USART2, c);     
		else 
			USART_SendData(USART2, 0x55);			
    if (fifo_used(&uart_tx_fifo) == 0) // Check if all data is transmitted . if yes disable transmitter UDRE interrupt
    {
      // Disable the EVAL_COM1 Transmit interrupt 
      USART_ITConfig(USART2, USART_IT_TXE, (FunctionalState)0);
    }
  }		
}

#endif 
