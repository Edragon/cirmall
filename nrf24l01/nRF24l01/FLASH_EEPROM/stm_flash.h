#ifndef __STMFLASH_H__
#define __STMFLASH_H__

/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "e2prom\emulation_ee.h"
/* ��չ���� ------------------------------------------------------------------*/
/* �������� ------------------------------------------------------------------*/
void STMFLASH_Write( uint16_t WriteAddr, uint16_t * pBuffer, uint16_t NumToWrite );		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read( uint16_t ReadAddr, uint16_t * pBuffer, uint16_t NumToRead );   	//��ָ����ַ��ʼ����ָ�����ȵ�����

#endif /* __STMFLASH_H__ */

/******************* (C) COPYRIGHT 2015-2020 ��Դ�ݿ������Ŷ� *****END OF FILE****/
