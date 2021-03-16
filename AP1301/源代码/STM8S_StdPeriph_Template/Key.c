#define __KEY_C

#include "Key.h"
#include "system.h"
#include "sensor.h"
#include "Filter.h"
#include "EEPROM.h"

u8 key_first=0;
u8 key_cout_start=0;
u8 Key_Status_Continue_3s=0;
u8 Key_Status_Continue_5s=0;


PIN_TypeDef PIN[2]=
{
  {GPIOC,GPIO_PIN_4}, //speed
  {GPIOD,GPIO_PIN_2}  //reset
};


//------------------------------------------
//按键读取
void Read_Key(void)
{
  static u8 i,key_state = KEY_STATE_0;
  static u16 key_time = 0;
  static u16  cnt_key;
  u8 key_temp;
  
  if(Func_Cnt_Is_Ok(cnt_key,10))
  {
    Func_Cnt_Get(cnt_key);
    
    switch(key_state)
    {
    case KEY_STATE_0: //读键
      {
        for(i=0;i<2;i++)
        {
          key_temp = GPIO_ReadInputPin(PIN[i].port,PIN[i].pin);
          if(key_temp == 0)
          {
            key_state = KEY_STATE_1;
            break;
          }
          else
          {
            key_state = KEY_STATE_0;
            Key = NO_KEY;
          }
        }
        break;
      }
    case KEY_STATE_1: //一次消抖
      {
        key_temp = GPIO_ReadInputPin(PIN[i].port,PIN[i].pin); 
        if(key_temp == 0)
        {
          key_state = KEY_STATE_2;
        }
        else
        {
          key_state = KEY_STATE_0;
          Key = NO_KEY;
        }
        break;
      }
    case KEY_STATE_2: //二次消抖，确认按键
      {
        key_temp = GPIO_ReadInputPin(PIN[i].port,PIN[i].pin); 
        if(key_temp == 0) //确实有键按下
        {
          Key = i+1;  //确认按键
          key_state = KEY_STATE_3;
          key_first = 1;
          
          key_cout_start = 1; //3s计时可以开始
        }
        else
        {
          key_state = KEY_STATE_0;
          Key = NO_KEY;
        }
        break;
      }
    case KEY_STATE_3: //等待按键释放
      {
        key_first = 0;
	key_temp = GPIO_ReadInputPin(PIN[i].port,PIN[i].pin); 
        if(key_temp != 0) //按键已释放
        {
          Key_Status_Continue_3s = 0;
          Key_Status_Continue_5s = 0;
          key_time = 0;
          key_state = KEY_STATE_0;
        }
        else// if(Key == FILTER_RESET)
        {
          if(key_cout_start == 1)
          {
            if(++key_time >= 300)
            {
              key_time = 0;
              Key_Status_Continue_3s = 1; //3s时间到
            }
          }
        }
        break;
      }
    }
  }
}

//------------------------------------------
//按键处理
void Process_Key(void)
{
  Read_Key();
  
  if(Key != NO_KEY)
  {
    switch (Key)
    {
    case SPEED:   //转速调节
      {
        Process_Key_SPEED();
        break;
      }
    case FILTER_RESET:    //过滤网重置
      {
        Process_Key_FILTER_RESET();
        break;
      }
    }
  }
}

//------------------------------------------
//转速按键处理
void Process_Key_SPEED(void)
{
  if(Lock_Status != LOCKED)
  {
    if(key_first != 0)
    {
      key_first = 0;
      
      PIN_GAS_POWER(on);
      
      if(Fan_Speed != SPEED_HIGH)
      {
        Fan_Speed = SPEED_HIGH;
        Anion = 1;
        //PIN_GAS_POWER(on);
      }
      else
      {
        Fan_Speed = SPEED_LOW;
        Anion = 1;
        //PIN_GAS_POWER(on);
      }
      
      Power_Status = ON;
    }
    else if(Key_Status_Continue_3s != 0)
    {
      Key_Status_Continue_3s = 0;
      
      Power_Status = OFF;
      
      Fan_Speed = SPEED_STOP;
      Anion = 0;
      //PIN_GAS_POWER(off);
    }
  }
}
//------------------------------------------
//过滤网按键处理
void Process_Key_FILTER_RESET(void)
{
  
  if(Key_Status_Continue_3s != 0)
  {
    Key_Status_Continue_3s = 0;
    
    if(Lock_Status == LOCKED)
    {
      Lock_Status = UNLOCKED;
      
      PARAMETER.DATAEEPROM.ReplaceFilter_Counter = 0;
      Save_Parameter();
      
      Power_Status = OFF;
    }
  }
}
//------------------------------------------
//判定是否是生产测试
void Check_Manufature(void)
{
  if(!GPIO_ReadInputPin(GPIOD,GPIO_PIN_2))
  {
    delay_ms(50); //延时10ms消抖
    if(!GPIO_ReadInputPin(GPIOD,GPIO_PIN_2))
    {
      Manufature = 1;
    }
    else
    {
      Manufature = 0;
    }
  }
  else
  {
    Manufature = 0;
  }
}
//------------------------------------------
//生产测试
void Process_Manufature(void)
{
  static u16 cnt_mt;
  static u8 i=0;
  
  if(Func_Cnt_Is_Ok(cnt_mt,500))
  {
    Func_Cnt_Get(cnt_mt);
    
    switch(i)
    {
    case 0:
      {
        i++;
        LED_Air_Red();
        break;
      }
    case 1:
      {
        i++;
        LED_Air_Green();
        break;
      }
    case 2:
      {
        i++;
        LED_Air_Blue();
        break;
      }
    case 3:
      {
        i++;
        LED_Air_Purple();
        break;
      }
    case 4:
      {
        i = 0;
        Manufature = 0;
        break;
      }
    }
  }
}
//------------------------------------------
//负离子功能
void Process_Anion(void)
{
  if(Anion != 0)
  {
    PIN_ANION1(on);
    PIN_ANION2(on); 
  }
  else
  {
    PIN_ANION1(off);
    PIN_ANION2(off);
  }
}
//------------------------------------------
//端口位处理
void GPIO_WriteBit(GPIO_TypeDef* GPIOx, GPIO_Pin_TypeDef PortPins, u8 BitVal)
{
  if(BitVal != 0)
  {
    GPIOx->ODR |= (uint8_t)PortPins;
  }
  else
  {
    GPIOx->ODR &= (uint8_t)(~PortPins);
  }
}