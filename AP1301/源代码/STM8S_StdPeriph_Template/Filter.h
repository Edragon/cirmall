#ifndef __FILTER_H
#define __FILTER_H

#include "system.h"

//#define   TEST    //��ʱ����ʹ��

#ifndef   TEST  //����ʹ��

#define  LockTime      ((u32)5184000)  //1440*60*60
#define  WRITETIME      600 //10minute

#else

#define  LockTime      ((u32)180)  //3*60
#define  WRITETIME      10 //10second

#endif

#define FLASHSTARTADDRESS  0x4000   

typedef enum
{  UNLOCKED = 0,
   LOCKED
}ControlStatus;

typedef struct{
  u8  HeaderCode1; //ͷ��1  ��ַ0
  u32 ReplaceFilter_Counter;  //������������ʱ�� ��ַ1-4
  u8 Acc_Sum;  //�ۼӺ� ��ַ5
}Dummy_EEPROM;

typedef union {
   Dummy_EEPROM  DATAEEPROM;
   u8 eeprom_map[6];
}UNION_EEPROM;

#ifdef __FILTER_C
   UNION_EEPROM   PARAMETER;
   u8 Time_To_Write;
   ControlStatus Lock_Status = UNLOCKED;
#else
   extern UNION_EEPROM  PARAMETER;
   extern u8 Time_To_Write;
   extern ControlStatus Lock_Status;
#endif  

void Parameter_Init(void);
void DurationCounting(void);
void Read_Parameter(void);
void Save_Parameter(void);
void Process_Filter(void);
void Blink_ReplaceFilter(void);

#endif