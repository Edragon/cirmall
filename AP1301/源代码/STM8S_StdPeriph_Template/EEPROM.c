#define __EEPROM_C


#include "EEPROM.h"
#include "Key.h"
#include "Filter.h"

#define IDAddress            255    //要写入ID的Flash地址
#define Zone_DSP_ID          0X77      //Zone DSP的ID

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

void delay_ms(u8 i)//延时i毫秒
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
/*读DATA端口数据*/
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
/*启动总线*/
bool I2C_START(void)
{
  EE_DATA_H;
  EE_CLK_H;
  I2C_delay();
  //if(!EE_DATA_Read)return FALSE;	//SDA线为低电平则总线忙,退出
  EE_DATA_L;
  I2C_delay();
  //if(EE_DATA_Read) return FALSE;	//SDA线为高电平则总线出错,退出
  EE_DATA_L;
  I2C_delay();
  return TRUE;
}
//******************************************************************************
/*停止总线*/
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
/*应答信号*/
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
/*非应答信号*/
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
/*等待应答，返回为:1 有ACK,0 无ACK*/
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
/*发送数据，数据从高位到低位*/
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
/*接收数据，数据从高位到低位*/
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
/*写入一页数据，注意不能跨页写，也就是最多不能超过8个字节，而且是在本页内*/
//写入1串数据      待写入数组地址   待写入地址       待写入长度     
bool I2C_PageWrite(u8* pBuffer, u8 WriteAddress,   u8 length )
{
  if(!I2C_START())
  {
    return FALSE;
  } 
  //I2C_SendByte(((WriteAddress & 0x0700) >>7) | 0xa0 & 0xFFFE);//设置高起始地址+器件地址
  I2C_SendByte(0xa0);
  if(!I2C_WaitAck())
  {
    I2C_STOP();
    return FALSE;
  }
  I2C_SendByte(WriteAddress);   //写入数据的地址      
  I2C_WaitAck();	
	  
  while(length--)
  {
    I2C_SendByte(* pBuffer);
    I2C_WaitAck();
    pBuffer++;
  }
  I2C_STOP();
  //注意：因为这里要等待EEPROM写完，可以采用查询或延时方式(10ms)
  delay_ms(10);
  return TRUE;
}
//******************************************************************************
/*跨页写*/
void I2C_BufferWrite(u8* pBuffer,u8 WriteAddress,u8 length)
{
    u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;
    Addr  = WriteAddress % I2C_PageSize;      //写入地址是开始页的第几位
    count = I2C_PageSize - Addr;	     //在开始页要写入的个数
    NumOfPage   =  length / I2C_PageSize;     //要写入的页数
    NumOfSingle =  length % I2C_PageSize;     //不足一页的个数

    if(Addr == 0)         //写入地址是页的开始 
    {
      if(NumOfPage == 0)  //数据小于一页 
      {
        I2C_PageWrite(pBuffer,WriteAddress,NumOfSingle);   //写少于一页的数据
      }
      else		            //数据大于等于一页  
      {
        while(NumOfPage)//要写入的页数
        {
          I2C_PageWrite(pBuffer,WriteAddress,I2C_PageSize);//写一页的数据
          WriteAddress += I2C_PageSize;
          pBuffer += I2C_PageSize;
	  NumOfPage--;		
        }
        if(NumOfSingle!=0)//剩余数据小于一页
        {
          I2C_PageWrite(pBuffer,WriteAddress,NumOfSingle); //写少于一页的数据
        }
      }
    }

    else                  //写入地址不是页的开始 
    {
      if(NumOfPage== 0)   //数据小于一页 
      {
        if(length > count) //如果要写的字节数大于当前页剩下的字节数
        {
          I2C_PageWrite(pBuffer,WriteAddress,count);   //就先写完当前页剩下的(count)字节
          //再在下一页写剩下的(length - count)字节
          I2C_PageWrite((u8*)(pBuffer+count), (WriteAddress + count) , (length - count));
        }
        else  //否则要写的字节数小于当前页剩下的字节数
        {
          I2C_PageWrite(pBuffer,WriteAddress,NumOfSingle); //就直接写好了
        }
      }
      else        //数据大于等于一页
      {
        length -= count;
        NumOfPage = length / I2C_PageSize;  //重新计算要写入的页数
        NumOfSingle = length % I2C_PageSize;  //重新计算不足一页的个数	
      
        if(count != 0)
        { 
          I2C_PageWrite(pBuffer,WriteAddress,count);      //将开始的空间写满一页
          WriteAddress += count;
          pBuffer += count;
         } 
      
        while(NumOfPage--)  //要写入的页数
        {
          I2C_PageWrite(pBuffer,WriteAddress,I2C_PageSize);//写一页的数据
          WriteAddress +=  I2C_PageSize;
          pBuffer      +=  I2C_PageSize; 
        }
        if(NumOfSingle != 0)//剩余数据小于一页
        {
          I2C_PageWrite(pBuffer,WriteAddress,NumOfSingle); //写少于一页的数据 
        }
      }
    } 
}
//******************************************************************************
/*读一串数据，可以跨页读，最大可读256个字节*/      
bool I2C_BufferRead(u8* pBuffer,u8 ReadAddress,u8 length)
{		
  if(!I2C_START())
  {
    return FALSE;
  }
  I2C_SendByte(0xa0);//器件地址 
  if(!I2C_WaitAck())
  {
    I2C_STOP(); 
    return FALSE;
  }
  I2C_SendByte(ReadAddress);   //读取数据的地址      
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
/*检测是否已经写入ID*/
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
/*写入ID*/
void Write_ID(void)
{
  ZoneDSP_ID[0] = Zone_DSP_ID;
  
  I2C_BufferWrite(ZoneDSP_ID,IDAddress,1);
}
//----------------------------------------------
//EEPROM故障指示-调出故障指示以后，灯闪烁30秒
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
//SPEED灯闪烁，用于指示EEPROM故障
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
