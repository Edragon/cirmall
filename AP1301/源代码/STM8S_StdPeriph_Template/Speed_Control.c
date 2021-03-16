#define __SPEED_CONTROL_C

#include "Speed_Control.h"
#include "Key.h"
#include "system.h"
#include "Filter.h"

u16 Duty=0;
//----------------------------------------------
//电机驱动信号，用端口产生PWM驱动信号，在中断中调用
//100us调用一次，PWM的频率定为500Hz，所以一共有200个
//100us
/*void Driver_PWM(void)
{
  static u8 level_flag=0;
  static u16 countvalue=5;
  
  
  {
    if(Fan_Speed != SPEED_STOP)
    {
      if(Duty != 0)
      {
        countvalue--;
      
        if(countvalue == 0)
        {
          if(level_flag == 0)
          {
            countvalue = Duty; //电平时间
            PIN_PWM_FAN_H;    //驱动电机(给电容充电)
            level_flag = 1;
          }
          else
          {
            countvalue = DUTY_FULL - Duty; //电平时间
            PIN_PWM_FAN_L;  //停止电机(停止电容充电)
            level_flag = 0;
          }
        }
      }
      else
      {
        PIN_PWM_FAN_L;  //停止电机(停止电容充电)
        level_flag = 0;
        countvalue=5;
      }
    }
    else
    {
      PIN_PWM_FAN_L;
      level_flag = 0;
      countvalue=5;
    }
  }
}*/
//------------------------------------------
//转速控制
/*void Process_Speed_Control(void)
{
  static u16 old_duty=0;
  
  if(Lock_Status != LOCKED)
  {
    switch(Fan_Speed)
    {
    case SPEED_STOP:
      {
        Duty = DUTY_ZERO;
        break;
      }
    case SPEED_LOW:
      {
        Duty = DUTY_L;
        break;
      }
    case SPEED_HIGH:
      {
        Duty = DUTY_H;
        break;
      } 
    }
  }
  else
  {
    Fan_Speed = SPEED_STOP;
    Duty = DUTY_ZERO;
  }
  
  if(old_duty != Duty)
  {
    old_duty = Duty;
    
    //更改占空比
    TIM2->CCR1H = (u8)(Duty >> 8);
    TIM2->CCR1L = (u8)(Duty);
  }
}*/
void Process_Speed_Control(void)
{
  if(Lock_Status != LOCKED)
  {
    switch(Fan_Speed)
    {
    case SPEED_STOP:
      {
        Vfan = SPEED_ZEROVSP;
        break;
      }
    case SPEED_LOW:
      {
        Vfan = SPEED_LOWVSP;
        break;
      }
    case SPEED_HIGH:
      {
        Vfan = SPEED_HIGHVSP;
        break;
      } 
    }
  }
  else
  {
    Fan_Speed = SPEED_STOP;
    Vfan = SPEED_ZEROVSP;
  }
  
    //更改占空比
    //TIM2->CCR1H = (u8)(Duty >> 8);
    //TIM2->CCR1L = (u8)(Duty);
  
}