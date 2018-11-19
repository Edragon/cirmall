#ifndef __USART_H__
#define __USART_H__
#include "stm32f1xx_hal.h"
#define RT_USING_UART1 
//#define RT_USING_UART3 
#define USART_REC_LEN  			100  	//定义最大接收字节数 
extern struct stm32_uart uart1;
extern TIM_HandleTypeDef htim2;
//extern 	struct stm32_uart uart2;
//extern struct stm32_uart uart3;
extern uint8_t  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern volatile  uint16_t USART_RX_STA;          		//接收状态标记	
void MX_TIM2_Init(void);
int rt_hw_usart_init(void);
int set_usart_BaudRate(struct stm32_uart *uart,uint32_t USARTx_BaudRate);
void serial_tx(struct stm32_uart *uart, const uint8_t *data, int length);
#endif
