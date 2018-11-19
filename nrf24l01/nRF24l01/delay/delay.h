#ifndef _DELAY_H
#define _DELAY_H

#include "stm32f1xx_hal.h"
void delay_init(uint8_t SYSCLK);
void delay_ms(uint32_t nms);
void delay_us(uint32_t nus);
#endif

