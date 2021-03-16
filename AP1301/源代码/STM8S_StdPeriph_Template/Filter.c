#define __FILTER_C

#include "system.h"
#include "Filter.h"
#include "sensor.h"
#include "Key.h"
#include "EEPROM.h"
//---------------------------------------
//上电读取计时参数，并判断是否可以警告或者锁住产品
void Parameter_Init(void)
{
  u8 ACCSUM=0;
  u8 i=0;
  u8 error=0;
  
  Read_Parameter();
  
  if(PARAMETER.DATAEEPROM.HeaderCode1 == 0xaa)
  {
    for(i=0;i<5;i++)
    {
      ACCSUM += PARAMETER.eeprom_map[i];
    }
      
    if(ACCSUM != PARAMETER.DATAEEPROM.Acc_Sum)  //校验和对不上
    {
      error = 1;
    }
  }
  else
  {
    error = 1;
  }
  
  if(error == 1) //第一次校验错误
  {
    //参数初始化
    PARAMETER.DATAEEPROM.ReplaceFilter_Counter = 0;
    Save_Parameter();
    
    PARAMETER.DATAEEPROM.HeaderCode1 = 0; //头码清零
    PARAMETER.DATAEEPROM.Acc_Sum = 0;  //校验和清零
    
    //再次读数
    Read_Parameter();
    
    if(PARAMETER.DATAEEPROM.HeaderCode1 == 0xaa)//再次校验
    {
      for(i=0;i<5;i++)
      {
        ACCSUM += PARAMETER.eeprom_map[i];
      }
      
      if(ACCSUM != PARAMETER.DATAEEPROM.Acc_Sum) //校验和仍然对不上
      {
        error = 2;
      }
      else
      {
        error = 0;
      }
    }
    else
    {
      error = 2;
    }
  }
  
  if(error == 2)
  {
    //参数初始化
    PARAMETER.DATAEEPROM.ReplaceFilter_Counter = 0;
    
    SystemFault_EEPROM = 1; //EEPROM报错
  }
  
  Lock_Status = UNLOCKED;  //产品状态初始化
  
  //判断是否到了可以锁住产品的阶段
  if(PARAMETER.DATAEEPROM.ReplaceFilter_Counter >= LockTime)
  {
    Lock_Status = LOCKED; //锁定
  }
}
//---------------------------------------
//寿命计时器计时，在中断中调用-以秒为单位计时
void DurationCounting(void)
{
  static u16 j=0;
  
  if(Lock_Status == UNLOCKED) //锁定以后，就不再计时，解锁以后才重新继续计时 
  {
    if(Power_Status != OFF)
    {
      //计时器计时
      PARAMETER.DATAEEPROM.ReplaceFilter_Counter++;
        
      j++;
      if(j >= WRITETIME)  //WRITETIME秒写一次EEPROM
      {
        j = 0;
        Time_To_Write = 1;
      }
    }
  }
}
//---------------------------------------
//读参数
void Read_Parameter(void)
{
  /*u8 i;
  
  FLASH_Unlock(FLASH_MEMTYPE_DATA);
  
  for(i=0;i<6;i++)
  {
    PARAMETER.eeprom_map[i] = FLASH_ReadByte(FLASHSTARTADDRESS+i);
  }
  
  FLASH_Lock(FLASH_MEMTYPE_DATA);*/
  
  I2C_BufferRead(PARAMETER.eeprom_map,0,6);
}
//---------------------------------------
//写参数
void Save_Parameter(void)
{
  u8 i;
  
  //数据准备
  PARAMETER.DATAEEPROM.HeaderCode1 = 0xaa; //头码
  PARAMETER.DATAEEPROM.Acc_Sum = 0;
  
  for(i=0;i<5;i++)
  {
    PARAMETER.DATAEEPROM.Acc_Sum += PARAMETER.eeprom_map[i];
  }
  
  //写数据
  /*FLASH_Unlock(FLASH_MEMTYPE_DATA);
 
  for(i=0;i<6;i++)
  {
    FLASH_ProgramByte(FLASHSTARTADDRESS+i,PARAMETER.eeprom_map[i]);
  }
  
  FLASH_Lock(FLASH_MEMTYPE_DATA);*/
  I2C_BufferWrite(PARAMETER.eeprom_map,0,6);
}
//---------------------------------------
//过滤网处理
void Process_Filter(void)
{
  if(Lock_Status != LOCKED)
  {
    if(Power_Status != OFF)
    {
      if(PARAMETER.DATAEEPROM.ReplaceFilter_Counter >= LockTime)
      {
        Lock_Status = LOCKED;  //产品锁住
        Save_Parameter();
        Anion = 0;
        //PIN_GAS_POWER(off);
        Fan_Speed = SPEED_STOP;
        Power_Status = OFF;
      }
      
      if(Time_To_Write == 1)  //定时写EEPROM  
      { 
        Time_To_Write = 0;
        Save_Parameter();
      }      
    } 
  } 
}
//---------------------------------------
//过滤网警告闪烁
void Blink_ReplaceFilter(void)
{
  static u16 cnt_blink_replacefilter=0;
  static u8 blink_replacefilter=0;
    
  if(Func_Cnt_Is_Ok(cnt_blink_replacefilter, 500)) //0.5s
  {
    Func_Cnt_Get(cnt_blink_replacefilter);
    if(blink_replacefilter == 0)
    {
      blink_replacefilter = 1;
      LED_Air_Red();
    }
    else
    {
      blink_replacefilter = 0;
      LED_Air_OFF();
    }
  }
}