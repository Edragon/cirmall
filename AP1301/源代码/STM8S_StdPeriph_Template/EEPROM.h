#ifndef  __EEPROM_H
#define  __EEPROM_H


#include  "system.h"

#define I2C_PageSize  8 //24C02的page大小是8 bytes

#define EE_CLK_L   GPIO_WriteLow(GPIOB,GPIO_PIN_4)//PIN_EE_SCL = 0
#define EE_CLK_H   GPIO_WriteHigh(GPIOB,GPIO_PIN_4)//PIN_EE_SCL = 1

#define EE_DATA_L  GPIO_WriteLow(GPIOB,GPIO_PIN_5)//PIN_EE_SDA = 0
#define EE_DATA_H  GPIO_WriteHigh(GPIOB,GPIO_PIN_5)//PIN_EE_SDA = 1

#define EE_DATA_Read  EEPROM_ReadInputData()


typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;


void I2C_delay(void);
void delay_ms(u8 i);
u8 EEPROM_ReadInputData(void);
bool I2C_START(void);
void I2C_STOP(void);
void I2C_Ack(void);
void I2C_NoAck(void);
bool I2C_WaitAck(void);
void I2C_SendByte(u8 SendByte);
u8 I2C_ReceiveByte(void);
bool I2C_PageWrite(u8* pBuffer,u8 WriteAddress,u8 length);
bool I2C_BufferRead(u8* pBuffer,u8 ReadAddress,u8 length);
void I2C_BufferWrite(u8* pBuffer,u8 WriteAddress,u8 length);

TestStatus Check_ID(void);
void Write_ID(void);

void Indicate_EEPROMFault(void);
void Blink_EEPROMFault(void);

#endif