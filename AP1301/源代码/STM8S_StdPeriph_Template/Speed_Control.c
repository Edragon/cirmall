#define __SPEED_CONTROL_C

#include "Speed_Control.h"
#include "Key.h"
#include "system.h"
#include "Filter.h"

u16 Duty=0;
//----------------------------------------------
//��������źţ��ö˿ڲ���PWM�����źţ����ж��е���
//100us����һ�Σ�PWM��Ƶ�ʶ�Ϊ500Hz������һ����200��
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
            countvalue = Duty; //��ƽʱ��
            PIN_PWM_FAN_H;    //�������(�����ݳ��)
            level_flag = 1;
          }
          else
          {
            countvalue = DUTY_FULL - Duty; //��ƽʱ��
            PIN_PWM_FAN_L;  //ֹͣ���(ֹͣ���ݳ��)
            level_flag = 0;
          }
        }
      }
      else
      {
        PIN_PWM_FAN_L;  //ֹͣ���(ֹͣ���ݳ��)
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
//ת�ٿ���
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
    
    //����ռ�ձ�
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
  
    //����ռ�ձ�
    //TIM2->CCR1H = (u8)(Duty >> 8);
    //TIM2->CCR1L = (u8)(Duty);
  
}