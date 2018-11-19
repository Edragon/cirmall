/* 包含头文件 ----------------------------------------------------------------*/
#include "stm_flash.h"
/**
  * 函数功能: 从指定地址开始写入指定长度的数据
  * 输入参数: WriteAddr:起始地址(此地址必须为2的倍数!!)
  *           pBuffer:数据指针
  *           NumToWrite:半字(16位)数(就是要写入的16位数据的个数.)
  * 返 回 值: 无
  * 说    明：无
  */
void STMFLASH_Write ( uint16_t virt_addr, uint16_t * pBuffer, uint16_t NumToWrite )
{
	  uint16_t i;
		uint16_t data;
    HAL_FLASH_Unlock();//解锁
    for(i = 0; i < NumToWrite; i++)
    {
        data = pBuffer[i] ;
			  EE_WriteVariable( virt_addr, data);
				virt_addr += 1;
    }
    HAL_FLASH_Lock();//上锁
}

/**
  * 函数功能: 从指定地址开始读出指定长度的数据
  * 输入参数: ReadAddr:起始地址
  *           pBuffer:数据指针
  *           NumToRead:半字(16位)数
  * 返 回 值: 无
  * 说    明：无
  */
void STMFLASH_Read (uint16_t virt_addr, uint16_t *pBuffer, uint16_t NumToRead )
{
    uint16_t i;
		uint16_t data;
    for(i = 0; i < NumToRead; i++)
    {
			EE_ReadVariable( virt_addr, &data);
        pBuffer[i] = data; //读取2个字节.
			virt_addr += 1;
    }
}

/******************* (C) COPYRIGHT 2015-2020 国源容开开发团队 *****END OF FILE****/
