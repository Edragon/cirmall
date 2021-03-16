#ifndef __SYSTEM_H
#define __SYSTEM_H

//#include "stm8s_gpio.h"
#include "stm8s.h"

/*
typedef signed long  s32;
typedef signed short s16;
typedef signed char  s8;
typedef unsigned long  u32;
typedef unsigned short u16;
typedef unsigned char u8;
typedef char BOOL;
*/
//--------------------------------------
typedef const struct
{
  GPIO_TypeDef * port;
  
  GPIO_Pin_TypeDef pin;
}PIN_TypeDef;


#define on      1
#define off     0
//--------------------------------------
//端口定义
//三色灯
#define TRILED1_on       GPIO_WriteHigh(GPIOC,GPIO_PIN_6)//GPIOC->ODR |= GPIO_PIN_6
#define TRILED1_off      GPIO_WriteLow(GPIOC,GPIO_PIN_6)//GPIOC->ODR &= (~GPIO_PIN_6)
#define TRILED2_on       GPIO_WriteHigh(GPIOC,GPIO_PIN_5)//GPIOC->ODR |= GPIO_PIN_5
#define TRILED2_off      GPIO_WriteLow(GPIOC,GPIO_PIN_5)//GPIOC->ODR &= (~GPIO_PIN_5)
#define TRILED3_on       GPIO_WriteHigh(GPIOC,GPIO_PIN_3)//GPIOC->ODR |= GPIO_PIN_3
#define TRILED3_off      GPIO_WriteLow(GPIOC,GPIO_PIN_3)//GPIOC->ODR &= (~GPIO_PIN_3)
//负离子
#define PIN_ANION1(x)    GPIO_WriteBit(GPIOC,GPIO_PIN_7,x) //
#define PIN_ANION2(x)    GPIO_WriteBit(GPIOD,GPIO_PIN_3,x) //
#define PIN_ANION1_H     GPIO_WriteHigh(GPIOC,GPIO_PIN_7)//GPIOB->ODR |= 0x10
#define PIN_ANION1_L     GPIO_WriteLow(GPIOC,GPIO_PIN_7)//GPIOB->ODR &= (~0x10)
#define PIN_ANION2_H     GPIO_WriteHigh(GPIOD,GPIO_PIN_3)//GPIOB->ODR |= 0x20
#define PIN_ANION2_L     GPIO_WriteLow(GPIOD,GPIO_PIN_3)//GPIOB->ODR &= (~0x20)
//PWM_Vfan
#define PIN_PWM_FAN(x)   GPIO_WriteBit(GPIOD,GPIO_PIN_4,x)

//GAS_POWER
#define PIN_GAS_POWER(x)     GPIO_WriteBit(GPIOA,GPIO_PIN_2,(~x&0x01)) //低电平有效

//--------------------------------------
#ifdef __MAIN_C
   u16 Func_Count = 0;
   u16 Func_nSec_Count = 0;
   u8 Anion=1;
   u8 SystemFault_EEPROM=0;
#else
   extern u16 Func_Count;
   extern u16 Func_nSec_Count;
   extern u8 Anion;
   extern u8 SystemFault_EEPROM;
#endif

#define Func_Cnt_Get(x)        (x = Func_Count)
#define Func_Cnt_Inc()         (Func_Count++)
#define Func_Cnt_Is_Ok(x,t)    ( ((u16)(Func_Count - (u16)x) >= (u16)t) ? 1:0 )
   
#define Func_nSec_Cnt_Get(x)        (x = Func_nSec_Count)        
#define Func_nSec_Cnt_Inc()         (Func_nSec_Count++)     
#define Func_nSec_Cnt_Is_Ok(x, t)   (((u16)(Func_nSec_Count - (u16)x) >= (u16)t)? 1 : 0) 


#endif