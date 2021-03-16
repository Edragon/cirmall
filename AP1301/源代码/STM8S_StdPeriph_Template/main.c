/**
  ******************************************************************************
  * @file    Project/main.c 
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    18-November-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 
#define __MAIN_C

/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "system.h"
#include "sensor.h"
#include "Key.h"
#include "Speed_Control.h"
#include "Filter.h"
#include "EEPROM.h"
/* Private defines -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void CLK_Configuration(void);
void GPIO_Configuration(void);
void TIM4_Configuration(void);
void TIM2_Configuration(void);
void AD_Configuration(void);
void FLASH_Configuration(void);
/* Private functions ---------------------------------------------------------*/
//u8 wData[10]={8,2,3,4,5,6,7,8,9,10};
//u8 rData[10];

void main(void)
{
  CLK_Configuration();
  
  GPIO_Configuration();
  
  Check_Manufature();
  
  TIM4_Configuration();  //定时时钟基准
  
  TIM2_Configuration();  //风扇PWM
  
  AD_Configuration();
  
  Parameter_Init();
  
  PIN_GAS_POWER(on); //传感器电源一直给
  
  /* Infinite loop */
  while (1)
  {
    if(Manufature != 0)
    {
      Process_Manufature();
    }
    else
    {
      Process_Key();
    
      Process_LED();
      
      Process_Speed_Control();
      
      Process_AD();
      
      Process_RealAirQuality();
      
      Process_Anion();
      
      Process_Filter();
    }
  }
}

//------------------------------------------
//系统时钟配置
void CLK_Configuration(void)
{
    CLK->CKDIVR = 0x00; //16MHz
    //CLK->PCKENR1 |= 0x10; //Peripheral Clock-TIM4
}

//------------------------------------------
//输入输出配置
void GPIO_Configuration(void)
{
  //PC7:NEGA1 PD3:NEGA2
  GPIO_Init(GPIOC,GPIO_PIN_7,GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(GPIOD,GPIO_PIN_3,GPIO_MODE_OUT_PP_LOW_SLOW);
  
  //PC6:TriLED_1  PC5:TriLED_2  PC3:TriLED_3
  GPIO_Init(GPIOC,GPIO_PIN_6,GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(GPIOC,GPIO_PIN_5,GPIO_MODE_OUT_PP_LOW_SLOW);
  GPIO_Init(GPIOC,GPIO_PIN_3,GPIO_MODE_OUT_PP_LOW_SLOW);
  
  //PD2:KEY_FILTER_RESET  PC4:KEY_SPEED
  GPIO_Init(GPIOD,GPIO_PIN_2,GPIO_MODE_IN_FL_NO_IT);
  GPIO_Init(GPIOC,GPIO_PIN_4,GPIO_MODE_IN_FL_NO_IT);
  
  //PB4:EE_SCL  PB5:EE_SDA
  GPIO_Init(GPIOB,GPIO_PIN_4,GPIO_MODE_OUT_OD_HIZ_SLOW);
  GPIO_Init(GPIOB,GPIO_PIN_5,GPIO_MODE_OUT_OD_HIZ_SLOW);
  
  //PA2:GAS_POWER
  GPIO_Init(GPIOA,GPIO_PIN_2,GPIO_MODE_OUT_PP_HIGH_SLOW); //低电平有效
}

//------------------------------------------
//------------------------------------------
//100us时钟基准
void TIM4_Configuration(void)
{
  //TIM4_TimeBaseInit(0x04, 100);  // Time base configuration 
  TIM4->PSCR = 0x04; //16
  TIM4->ARR = (100-1);  //100
  
  TIM4_ARRPreloadConfig(ENABLE);
    
  TIM4_ClearFlag(TIM4_FLAG_UPDATE);  // Clear TIM4 update flag 
  
  TIM4_ITConfig(TIM4_IT_UPDATE, ENABLE); //Enable update interrupt 
  
  enableInterrupts();  // enable interrupts 

  TIM4_Cmd(ENABLE);  // Enable TIM4 
} 
//------------------------------------------
//PWM_Vfan风扇电压控制  PWM模式 10kHz载频
void TIM2_Configuration(void)
{
  TIM2_DeInit();
  
  TIM2_TimeBaseInit(TIM2_PRESCALER_16,99); //载频10kHz
  
  //PWM1模式，输出使能，初始化占空比0%，有效电平为高电平
  TIM2_OC1Init(TIM2_OCMODE_PWM1,TIM2_OUTPUTSTATE_ENABLE,0,TIM2_OCPOLARITY_HIGH);
  
  TIM2_OC1PreloadConfig(ENABLE);
  
  TIM2_ARRPreloadConfig(ENABLE);
  
  TIM2_Cmd(ENABLE); //TIM2 enable counter
}
//------------------------------------------
//AD配置
void AD_Configuration(void)
{
  /*
  ADC1->TDRL = 0x60;  //disable schmitt trigger
  ADC1->CR1 |= 0x40;  //f(adc)=f(master)/8 
  ADC1->CR2 |= 0x08;   //数据右对齐
  //ADC1->CSR = 6;      //通道选择，初始化为AD_Vfan-AIN6
  ADC1->CSR = 5;    //通道选择，GAS-AIN5
  ADC1->CR1 |= ADC1_CR1_ADON; //start AD conversion
  ADC1->CR1 |= ADC1_CR1_ADON; //启动转换必须第二次使用写指令来置位寄存器的位
  */
  
  ADC1_DeInit();
  
  //单次采样，通道6，数据右对齐
  ADC1_ConversionConfig(ADC1_CONVERSIONMODE_SINGLE,ADC1_CHANNEL_6,ADC1_ALIGN_RIGHT);
  
  //f(adc)=f(master)/8
  ADC1_PrescalerConfig(ADC1_PRESSEL_FCPU_D8);
  
  //disable schmitt trigger
  //ADC1_SchmittTriggerConfig(ADC1_SCHMITTTRIG_ALL,DISABLE);
  //ADC1_SchmittTriggerConfig(ADC1_SCHMITTTRIG_CHANNEL5,DISABLE);
  //ADC1_SchmittTriggerConfig(ADC1_SCHMITTTRIG_CHANNEL6,DISABLE);
  
  //Enable the ADC1 peripheral 
  ADC1->CR1 |= ADC1_CR1_ADON;
  
  AD_Start();
  //ADC1_StartConversion(); //初始化AD启动需要写两次ADC1->CR1，以后只写一次就可以了
}
//------------------------------------------
//FLASH配置-EEPROM
void FLASH_Configuration(void)
{
  //Define FLASH programming time 
  FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD);

  //FLASH_Unlock(FLASH_MEMTYPE_PROG);
  //Wait until Flash Program area unlocked flag is set
  //while (FLASH_GetFlagStatus(FLASH_FLAG_PUL) == RESET)
  //{}

  //Unlock flash data eeprom memory 
  FLASH_Unlock(FLASH_MEMTYPE_DATA);
  //Wait until Data EEPROM area unlocked flag is set
  //while (FLASH_GetFlagStatus(FLASH_FLAG_DUL) == RESET)
  //{}
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
