#ifndef _EMULATION_EE_H
#define _EMULATION_EE_H

/* Includes ------------------------------------------------------------------*/
/* 这里使用的stm32f103tb，所以包含stm32f1xx.h，如果使用的是不是f1系列，则应修改为相应的头文件包含 */
#include "stm32f1xx.h"

/* Exported constants --------------------------------------------------------*/
/* low-density(16 to 32k) and medium-density(64 to 128k) device,page size = 1kbyte */
/* 页大小，根据片上flash大小来设定该值，低中密度的也大小 = 1kbytes */
#define PAGE_SIZE  (uint16_t)0x400 

/* high-density(256 to 512k) and XL-density(512 - 1024k),page size = 2kbyte */
//#define PAGE_SIZE  (uint16_t)0x800  


///* EEPROM start address in Flash,对应page61 */
#define EEPROM_START_ADDRESS    ((uint32_t)0x0800F400) /* EEPROM emulation start address:
                                                  after 64KByte of used Flash memory */
/* EEPROM start address in Flash,对应page254 */
//#define EEPROM_START_ADDRESS    ((uint32_t)0x0807F400) /* EEPROM emulation start address:
//																										after 64KByte of used Flash memory */
/* Variables' number，用户根据要保存的数据量来确定 */
#define NUMB_OF_VAR             ((uint8_t)0x8)


/* Pages 0 and 1 base and end addresses */
#define PAGE0_BASE_ADDRESS      ((uint32_t)(EEPROM_START_ADDRESS + 0x000))
#define PAGE0_END_ADDRESS       ((uint32_t)(EEPROM_START_ADDRESS + (PAGE_SIZE - 1)))

#define PAGE1_BASE_ADDRESS      ((uint32_t)(EEPROM_START_ADDRESS + PAGE_SIZE))
#define PAGE1_END_ADDRESS       ((uint32_t)(EEPROM_START_ADDRESS + (2 * PAGE_SIZE - 1)))

#define PAGE2_BASE_ADDRESS      ((uint32_t)(EEPROM_START_ADDRESS + (2 * PAGE_SIZE )))
#define PAGE2_END_ADDRESS       ((uint32_t)(EEPROM_START_ADDRESS + (3 * PAGE_SIZE - 1)))

/* Used Flash pages for EEPROM emulation */
#define PAGE0                   ((uint16_t)0x0000)
#define PAGE1                   ((uint16_t)0x0001)
#define PAGE2                   ((uint16_t)0x0002)
/* No valid page define */
#define NO_VALID_PAGE           ((uint16_t)0x00AB)

/* Page status definitions */
#define ERASED                  ((uint16_t)0xFFFF)     /* PAGE is empty */
//#define RECEIVE_DATA            ((uint16_t)0xEEEE)     /* PAGE is marked to receive data */
//#define VALID_PAGE              ((uint16_t)0x0000)     /* PAGE containing valid data */

/* Valid pages in read and write defines */
#define READ_FROM_VALID_PAGE    ((uint8_t)0x00)
#define WRITE_IN_VALID_PAGE     ((uint8_t)0x01)

/* Page full define */
#define PAGE_FULL               ((uint8_t)0x80)


///* Exported types ------------------------------------------------------------*/
///* Exported macro ------------------------------------------------------------*/
///* Exported functions ------------------------------------------------------- */
//uint16_t ee_init(void);
//uint16_t EE_ReadVariable(uint16_t virt_addr, uint16_t* data);
//uint16_t EE_WriteVariable(uint16_t virt_addr, uint16_t data);
//#define NumbOfVar               ((uint8_t)0x18)

/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
uint16_t EE_Init(void);
uint16_t EE_ReadVariable(uint16_t VirtAddress, uint16_t* Data);
uint16_t EE_WriteVariable(uint16_t VirtAddress, uint16_t Data);

#endif
