/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "stm_flash.h"
/**
  * ��������: ��ָ����ַ��ʼд��ָ�����ȵ�����
  * �������: WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ2�ı���!!)
  *           pBuffer:����ָ��
  *           NumToWrite:����(16λ)��(����Ҫд���16λ���ݵĸ���.)
  * �� �� ֵ: ��
  * ˵    ������
  */
void STMFLASH_Write ( uint16_t virt_addr, uint16_t * pBuffer, uint16_t NumToWrite )
{
	  uint16_t i;
		uint16_t data;
    HAL_FLASH_Unlock();//����
    for(i = 0; i < NumToWrite; i++)
    {
        data = pBuffer[i] ;
			  EE_WriteVariable( virt_addr, data);
				virt_addr += 1;
    }
    HAL_FLASH_Lock();//����
}

/**
  * ��������: ��ָ����ַ��ʼ����ָ�����ȵ�����
  * �������: ReadAddr:��ʼ��ַ
  *           pBuffer:����ָ��
  *           NumToRead:����(16λ)��
  * �� �� ֵ: ��
  * ˵    ������
  */
void STMFLASH_Read (uint16_t virt_addr, uint16_t *pBuffer, uint16_t NumToRead )
{
    uint16_t i;
		uint16_t data;
    for(i = 0; i < NumToRead; i++)
    {
			EE_ReadVariable( virt_addr, &data);
        pBuffer[i] = data; //��ȡ2���ֽ�.
			virt_addr += 1;
    }
}

/******************* (C) COPYRIGHT 2015-2020 ��Դ�ݿ������Ŷ� *****END OF FILE****/
