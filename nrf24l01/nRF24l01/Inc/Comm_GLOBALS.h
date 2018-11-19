#ifndef __COMM_GLOBALS_H
#define __COMM_GLOBALS_H
#include "stm32f1xx_hal.h"
#include "NRF24L01/bsp_NRF24L01.h"
#ifdef GLOBALS 
#define EXT
#else
#define EXT extern 
#endif
//#define SYSCLK 8 //系统频率72M
EXT uint8_t  Rec_Data [100];//应答报文
EXT uint8_t  NRF_Tx_Data [100];
EXT uint8_t  USART_TX_BUF[100];
EXT uint8_t  RF_Mode;
EXT uint8_t  USART_RX_CNT;
EXT uint8_t  NRF_RX_CNT;
EXT uint8_t  TX_ADDRESS[5];//ID号 
EXT uint32_t BaudRate;
EXT uint16_t Speed;
EXT uint16_t Power;
EXT uint16_t CRCMode;
EXT uint16_t FreqPoint;

#endif
