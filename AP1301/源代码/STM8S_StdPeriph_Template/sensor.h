#ifndef __SENSOR_H
#define __SENSOR_H

#include "stm8s.h"
#include "system.h"

#define  AD_Start()        ADC1_StartConversion()//ADC1->CR1 |= 0x01 //start ad convert
#define  AD_WaitStop()     while((ADC1->CSR&0x80) == 0)  //�ȴ�ADת������

#define  AD_CHANNEL_VFAN   0
#define  AD_CHANNEL_VOC    1

#define Excellent 1
#define Good 2
#define Normal 3
#define Bad 4

//*******************************************************************
//����ɫLED�ƶ���

#define LED_Air_Red()    TRILED1_off;TRILED2_off;TRILED3_on  //��ɫ
#define LED_Air_Purple() TRILED1_on;TRILED2_off;TRILED3_on   //��ɫ
#define LED_Air_Orange() TRILED1_off;TRILED2_on;TRILED3_on   //��ɫ
#define LED_Air_Green()  TRILED1_off;TRILED2_on;TRILED3_off  //��ɫ
#define LED_Air_Blue()   TRILED1_on;TRILED2_off;TRILED3_off  //��ɫ
#define LED_Air_OFF()    TRILED1_off;TRILED2_off;TRILED3_off //Ϩ��
/*
#define LED_Air_Red()    GPIO_WriteLow(GPIOC,GPIO_PIN_3);\
                         GPIO_WriteLow(GPIOC,GPIO_PIN_4);\
                         GPIO_WriteHigh(GPIOC,GPIO_PIN_6) //��ɫ

#define LED_Air_Purple() GPIO_WriteHigh(GPIOC,GPIO_PIN_3);\
                         GPIO_WriteLow(GPIOC,GPIO_PIN_4);\
                         GPIO_WriteHigh(GPIOC,GPIO_PIN_6) //��ɫ

#define LED_Air_Orange() GPIO_WriteLow(GPIOC,GPIO_PIN_3);\
                         GPIO_WriteHigh(GPIOC,GPIO_PIN_4);\
                         GPIO_WriteHigh(GPIOC,GPIO_PIN_6) //��ɫ

#define LED_Air_Green()  GPIO_WriteLow(GPIOC,GPIO_PIN_3);\
                         GPIO_WriteHigh(GPIOC,GPIO_PIN_4);\
                         GPIO_WriteLow(GPIOC,GPIO_PIN_6) //��ɫ

#define LED_Air_Blue()   GPIO_WriteHigh(GPIOC,GPIO_PIN_3);\
                         GPIO_WriteLow(GPIOC,GPIO_PIN_4);\
                         GPIO_WriteLow(GPIOC,GPIO_PIN_6) //��ɫ

#define LED_Air_OFF()    GPIO_WriteLow(GPIOC,GPIO_PIN_3);\
                         GPIO_WriteLow(GPIOC,GPIO_PIN_4);\
                         GPIO_WriteLow(GPIOC,GPIO_PIN_6) //Ϩ��
*/

#ifdef __SENSOR_C
  u8 Air_Condition = 3; //ʵ�ʿ���������ʼ��Ϊ��
  u8 Es_Air_Condition = 2; //������������
#else
  extern u8 Air_Condition;
  extern u8 Es_Air_Condition;
#endif


void VOC_StartupCounting(void);
void ADC1_Channel_Sel(ADC1_Channel_TypeDef ADC1_Channel);
void Process_AD(void);
void Process_VOCSensor(void);
void Estimate_AirCondition(void);
void Process_RealAirQuality(void);
void Process_VFan(void);
void Process_LED(void);
void Blink_Blue(void);

#endif