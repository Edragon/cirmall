#ifndef __STMFLASH_H__
#define __STMFLASH_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "e2prom\emulation_ee.h"
/* 扩展变量 ------------------------------------------------------------------*/
/* 函数声明 ------------------------------------------------------------------*/
void STMFLASH_Write( uint16_t WriteAddr, uint16_t * pBuffer, uint16_t NumToWrite );		//从指定地址开始写入指定长度的数据
void STMFLASH_Read( uint16_t ReadAddr, uint16_t * pBuffer, uint16_t NumToRead );   	//从指定地址开始读出指定长度的数据

#endif /* __STMFLASH_H__ */

/******************* (C) COPYRIGHT 2015-2020 国源容开开发团队 *****END OF FILE****/
