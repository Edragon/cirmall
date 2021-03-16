#ifndef  __KEY_H
#define  __KEY_H

#include "stm8s_gpio.h"
#include "system.h"

#define KEY_STATE_0  0
#define KEY_STATE_1  1
#define KEY_STATE_2  2
#define KEY_STATE_3  3
#define KEY_STATE_4  4

#define NO_KEY           0
#define SPEED            1
#define FILTER_RESET     2


#define SPEED_STOP       0
#define SPEED_LOW        1
#define SPEED_HIGH       2

#define OFF  0
#define ON   1


#ifdef __KEY_C
   u8 Key=0;
   u8 Fan_Speed=SPEED_HIGH;
   u8 Power_Status = 1;
   u8 Manufature=0;
#else
   extern u8 Key;
   extern u8 Fan_Speed;
   extern u8 Power_Status;
   extern u8 Manufature;
#endif
   
void Read_Key(void);
void Process_Key(void);
void Process_Key_SPEED(void);
void Process_Key_FILTER_RESET(void);
void Process_Anion(void);
void Check_Manufature(void);
void Process_Manufature(void);
void GPIO_WriteBit(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef PortPins, u8 BitVal);

#endif