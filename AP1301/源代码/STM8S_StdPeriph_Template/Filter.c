#define __FILTER_C

#include "system.h"
#include "Filter.h"
#include "sensor.h"
#include "Key.h"
#include "EEPROM.h"
//---------------------------------------
//�ϵ��ȡ��ʱ���������ж��Ƿ���Ծ��������ס��Ʒ
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
      
    if(ACCSUM != PARAMETER.DATAEEPROM.Acc_Sum)  //У��ͶԲ���
    {
      error = 1;
    }
  }
  else
  {
    error = 1;
  }
  
  if(error == 1) //��һ��У�����
  {
    //������ʼ��
    PARAMETER.DATAEEPROM.ReplaceFilter_Counter = 0;
    Save_Parameter();
    
    PARAMETER.DATAEEPROM.HeaderCode1 = 0; //ͷ������
    PARAMETER.DATAEEPROM.Acc_Sum = 0;  //У�������
    
    //�ٴζ���
    Read_Parameter();
    
    if(PARAMETER.DATAEEPROM.HeaderCode1 == 0xaa)//�ٴ�У��
    {
      for(i=0;i<5;i++)
      {
        ACCSUM += PARAMETER.eeprom_map[i];
      }
      
      if(ACCSUM != PARAMETER.DATAEEPROM.Acc_Sum) //У�����Ȼ�Բ���
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
    //������ʼ��
    PARAMETER.DATAEEPROM.ReplaceFilter_Counter = 0;
    
    SystemFault_EEPROM = 1; //EEPROM����
  }
  
  Lock_Status = UNLOCKED;  //��Ʒ״̬��ʼ��
  
  //�ж��Ƿ��˿�����ס��Ʒ�Ľ׶�
  if(PARAMETER.DATAEEPROM.ReplaceFilter_Counter >= LockTime)
  {
    Lock_Status = LOCKED; //����
  }
}
//---------------------------------------
//������ʱ����ʱ�����ж��е���-����Ϊ��λ��ʱ
void DurationCounting(void)
{
  static u16 j=0;
  
  if(Lock_Status == UNLOCKED) //�����Ժ󣬾Ͳ��ټ�ʱ�������Ժ�����¼�����ʱ 
  {
    if(Power_Status != OFF)
    {
      //��ʱ����ʱ
      PARAMETER.DATAEEPROM.ReplaceFilter_Counter++;
        
      j++;
      if(j >= WRITETIME)  //WRITETIME��дһ��EEPROM
      {
        j = 0;
        Time_To_Write = 1;
      }
    }
  }
}
//---------------------------------------
//������
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
//д����
void Save_Parameter(void)
{
  u8 i;
  
  //����׼��
  PARAMETER.DATAEEPROM.HeaderCode1 = 0xaa; //ͷ��
  PARAMETER.DATAEEPROM.Acc_Sum = 0;
  
  for(i=0;i<5;i++)
  {
    PARAMETER.DATAEEPROM.Acc_Sum += PARAMETER.eeprom_map[i];
  }
  
  //д����
  /*FLASH_Unlock(FLASH_MEMTYPE_DATA);
 
  for(i=0;i<6;i++)
  {
    FLASH_ProgramByte(FLASHSTARTADDRESS+i,PARAMETER.eeprom_map[i]);
  }
  
  FLASH_Lock(FLASH_MEMTYPE_DATA);*/
  I2C_BufferWrite(PARAMETER.eeprom_map,0,6);
}
//---------------------------------------
//����������
void Process_Filter(void)
{
  if(Lock_Status != LOCKED)
  {
    if(Power_Status != OFF)
    {
      if(PARAMETER.DATAEEPROM.ReplaceFilter_Counter >= LockTime)
      {
        Lock_Status = LOCKED;  //��Ʒ��ס
        Save_Parameter();
        Anion = 0;
        //PIN_GAS_POWER(off);
        Fan_Speed = SPEED_STOP;
        Power_Status = OFF;
      }
      
      if(Time_To_Write == 1)  //��ʱдEEPROM  
      { 
        Time_To_Write = 0;
        Save_Parameter();
      }      
    } 
  } 
}
//---------------------------------------
//������������˸
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