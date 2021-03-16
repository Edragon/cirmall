#define __EEPROM_C


#include "EEPROM.h"
#include "Key.h"
#include "Filter.h"

#define IDAddress            255    //Ҫд��ID��Flash��ַ
#define Zone_DSP_ID          0X77      //Zone DSP��ID

u8 ZoneDSP_ID[1];

volatile TestStatus IDProgramStatus;

void I2C_delay(void)
{	
   u16 i=25; 
   while(i) 
   { 
     i--; 
   } 
}

void delay_ms(u8 i)//��ʱi����
{
  u16 j;
  for ( ; i>0; i--)
  {
    for (j=2700; j>0; j--)
    {
    }    
  }
}
//******************************************************************************
/*��DATA�˿�����*/
u8 EEPROM_ReadInputData(void)
{
  u8 temp;
  
  //PIN_PD_EE_SDA = 0;
  //PIN_PD_EE_SDA = 0;
  GPIO_Init(GPIOB,GPIO_PIN_5,GPIO_MODE_IN_FL_NO_IT);
  temp = (u8)GPIO_ReadInputPin(GPIOB,GPIO_PIN_5);
  //PIN_PD_EE_SDA = 1;
  //PIN_PD_EE_SDA = 1;
  GPIO_Init(GPIOB,GPIO_PIN_5,GPIO_MODE_OUT_OD_HIZ_SLOW);
  
  return temp;
}
//******************************************************************************
/*��������*/
bool I2C_START(void)
{
  EE_DATA_H;
  EE_CLK_H;
  I2C_delay();
  //if(!EE_DATA_Read)return FALSE;	//SDA��Ϊ�͵�ƽ������æ,�˳�
  EE_DATA_L;
  I2C_delay();
  //if(EE_DATA_Read) return FALSE;	//SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
  EE_DATA_L;
  I2C_delay();
  return TRUE;
}
//******************************************************************************
/*ֹͣ����*/
void I2C_STOP(void)
{
  EE_CLK_L;
  I2C_delay();
  EE_DATA_L;
  I2C_delay();
  EE_CLK_H;
  I2C_delay();
  EE_DATA_H;
  I2C_delay();
}
//******************************************************************************
/*Ӧ���ź�*/
void I2C_Ack(void)
{
  EE_CLK_L;
  I2C_delay();
  EE_DATA_L;
  I2C_delay();
  EE_CLK_H;
  I2C_delay();
  EE_CLK_L;
  I2C_delay();
}
//******************************************************************************
/*��Ӧ���ź�*/
void I2C_NoAck(void)
{
  EE_CLK_L;
  I2C_delay();
  EE_DATA_H;
  I2C_delay();
  EE_CLK_H;
  I2C_delay();
  EE_CLK_L;
  I2C_delay();
}
//******************************************************************************
/*�ȴ�Ӧ�𣬷���Ϊ:1 ��ACK,0 ��ACK*/
bool I2C_WaitAck(void) 	 
{
  EE_CLK_L;
  I2C_delay();
  EE_DATA_H;			
  I2C_delay();
  EE_CLK_H;
  I2C_delay();
  if(EE_DATA_Read)
  {
    EE_CLK_L;
    return FALSE;
  }
  EE_CLK_L;
  return TRUE;
}
//******************************************************************************
/*�������ݣ����ݴӸ�λ����λ*/
void I2C_SendByte(u8 SendByte) 
{
  u8 i=8;
  while(i--)
  {
    EE_CLK_L;
    //I2C_delay();
    if(SendByte&0x80)
    EE_DATA_H;  
    else 
    EE_DATA_L;   
    I2C_delay();
    EE_CLK_H;
    I2C_delay();
    SendByte<<=1;
    //I2C_delay();
  }
  EE_CLK_L;
}
//******************************************************************************
/*�������ݣ����ݴӸ�λ����λ*/
u8 I2C_ReceiveByte(void)  
{ 
  u8 i=8;
  u8 ReceiveByte=0;

  EE_DATA_H;				
  while(i--)
  {
    ReceiveByte<<=1;      
    EE_CLK_L;
    I2C_delay();
    EE_CLK_H;
    I2C_delay();	
    if(EE_DATA_Read)
    {
      ReceiveByte|=0x01;
    }
  }
  EE_CLK_L;
  return ReceiveByte;
}
//******************************************************************************
/*д��һҳ���ݣ�ע�ⲻ�ܿ�ҳд��Ҳ������಻�ܳ���8���ֽڣ��������ڱ�ҳ��*/
//д��1������      ��д�������ַ   ��д���ַ       ��д�볤��     
bool I2C_PageWrite(u8* pBuffer, u8 WriteAddress,   u8 length )
{
  if(!I2C_START())
  {
    return FALSE;
  } 
  //I2C_SendByte(((WriteAddress & 0x0700) >>7) | 0xa0 & 0xFFFE);//���ø���ʼ��ַ+������ַ
  I2C_SendByte(0xa0);
  if(!I2C_WaitAck())
  {
    I2C_STOP();
    return FALSE;
  }
  I2C_SendByte(WriteAddress);   //д�����ݵĵ�ַ      
  I2C_WaitAck();	
	  
  while(length--)
  {
    I2C_SendByte(* pBuffer);
    I2C_WaitAck();
    pBuffer++;
  }
  I2C_STOP();
  //ע�⣺��Ϊ����Ҫ�ȴ�EEPROMд�꣬���Բ��ò�ѯ����ʱ��ʽ(10ms)
  delay_ms(10);
  return TRUE;
}
//******************************************************************************
/*��ҳд*/
void I2C_BufferWrite(u8* pBuffer,u8 WriteAddress,u8 length)
{
    u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;
    Addr  = WriteAddress % I2C_PageSize;      //д���ַ�ǿ�ʼҳ�ĵڼ�λ
    count = I2C_PageSize - Addr;	     //�ڿ�ʼҳҪд��ĸ���
    NumOfPage   =  length / I2C_PageSize;     //Ҫд���ҳ��
    NumOfSingle =  length % I2C_PageSize;     //����һҳ�ĸ���

    if(Addr == 0)         //д���ַ��ҳ�Ŀ�ʼ 
    {
      if(NumOfPage == 0)  //����С��һҳ 
      {
        I2C_PageWrite(pBuffer,WriteAddress,NumOfSingle);   //д����һҳ������
      }
      else		            //���ݴ��ڵ���һҳ  
      {
        while(NumOfPage)//Ҫд���ҳ��
        {
          I2C_PageWrite(pBuffer,WriteAddress,I2C_PageSize);//дһҳ������
          WriteAddress += I2C_PageSize;
          pBuffer += I2C_PageSize;
	  NumOfPage--;		
        }
        if(NumOfSingle!=0)//ʣ������С��һҳ
        {
          I2C_PageWrite(pBuffer,WriteAddress,NumOfSingle); //д����һҳ������
        }
      }
    }

    else                  //д���ַ����ҳ�Ŀ�ʼ 
    {
      if(NumOfPage== 0)   //����С��һҳ 
      {
        if(length > count) //���Ҫд���ֽ������ڵ�ǰҳʣ�µ��ֽ���
        {
          I2C_PageWrite(pBuffer,WriteAddress,count);   //����д�굱ǰҳʣ�µ�(count)�ֽ�
          //������һҳдʣ�µ�(length - count)�ֽ�
          I2C_PageWrite((u8*)(pBuffer+count), (WriteAddress + count) , (length - count));
        }
        else  //����Ҫд���ֽ���С�ڵ�ǰҳʣ�µ��ֽ���
        {
          I2C_PageWrite(pBuffer,WriteAddress,NumOfSingle); //��ֱ��д����
        }
      }
      else        //���ݴ��ڵ���һҳ
      {
        length -= count;
        NumOfPage = length / I2C_PageSize;  //���¼���Ҫд���ҳ��
        NumOfSingle = length % I2C_PageSize;  //���¼��㲻��һҳ�ĸ���	
      
        if(count != 0)
        { 
          I2C_PageWrite(pBuffer,WriteAddress,count);      //����ʼ�Ŀռ�д��һҳ
          WriteAddress += count;
          pBuffer += count;
         } 
      
        while(NumOfPage--)  //Ҫд���ҳ��
        {
          I2C_PageWrite(pBuffer,WriteAddress,I2C_PageSize);//дһҳ������
          WriteAddress +=  I2C_PageSize;
          pBuffer      +=  I2C_PageSize; 
        }
        if(NumOfSingle != 0)//ʣ������С��һҳ
        {
          I2C_PageWrite(pBuffer,WriteAddress,NumOfSingle); //д����һҳ������ 
        }
      }
    } 
}
//******************************************************************************
/*��һ�����ݣ����Կ�ҳ�������ɶ�256���ֽ�*/      
bool I2C_BufferRead(u8* pBuffer,u8 ReadAddress,u8 length)
{		
  if(!I2C_START())
  {
    return FALSE;
  }
  I2C_SendByte(0xa0);//������ַ 
  if(!I2C_WaitAck())
  {
    I2C_STOP(); 
    return FALSE;
  }
  I2C_SendByte(ReadAddress);   //��ȡ���ݵĵ�ַ      
  I2C_WaitAck();
  I2C_START();
  I2C_SendByte(0xa1);  
  I2C_WaitAck();
  while(length)
  {
    *pBuffer = I2C_ReceiveByte();
    if(length == 1)
    {
      I2C_NoAck();
    }
    else
    {
      I2C_Ack(); 
    }
    pBuffer++;
    length--;
  }
  I2C_STOP();
  //delay_ms(10);
  return TRUE;
}
//******************************************************************************
/*����Ƿ��Ѿ�д��ID*/
TestStatus Check_ID(void)
{
  I2C_BufferRead(ZoneDSP_ID,IDAddress,1);
   
  if(ZoneDSP_ID[0] != Zone_DSP_ID)
  {
    IDProgramStatus = FAILED;
  }
  else
  {
    IDProgramStatus = PASSED;
  }
  return IDProgramStatus;
}
//******************************************************************************
/*д��ID*/
void Write_ID(void)
{
  ZoneDSP_ID[0] = Zone_DSP_ID;
  
  I2C_BufferWrite(ZoneDSP_ID,IDAddress,1);
}
//----------------------------------------------
//EEPROM����ָʾ-��������ָʾ�Ժ󣬵���˸30��
/*extern u8 Working_Mode;

void Indicate_EEPROMFault(void)
{
  static u16 cnt_indicateeeprom=0;
  //static u8 blinkflag_motor=1;
  
  if(blinkeeprom != 0)
  {
    if(Func_nSec_Cnt_Is_Ok(cnt_indicateeeprom,30))
    {
      blinkeeprom = 0; 
      if(Working_Mode == Manual_Mode)
      {
        LED_Fan_Speed_Status = ON;
      }
      else
      {
        LED_Fan_Speed_Status = OFF;
      }
    }
    
    Blink_EEPROMFault();
  }
  else
  {
    Func_nSec_Cnt_Get(cnt_indicateeeprom);
  }
}*/
//----------------------------------------------
//SPEED����˸������ָʾEEPROM����
/*void Blink_EEPROMFault(void)
{
  static u16 cnt_blink_speed;
  static u8 blink_speed = 0;
  
  if(Func_Cnt_Is_Ok(cnt_blink_speed, 500)) //0.5s
  {
    Func_Cnt_Get(cnt_blink_speed);
    if(blink_speed == 0)
    {
      blink_speed = 1;
      LED_Fan_Speed_Status = ON;
    }
    else
    {
      blink_speed = 0;
      LED_Fan_Speed_Status = OFF;
    }
  }
}*/
