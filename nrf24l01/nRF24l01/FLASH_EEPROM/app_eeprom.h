#ifndef _APP_EEPROM_H
#define _APP_EEPROM_H
#ifdef __cplusplus
extern "C" {
#endif


#include "stm_flash.h"

/* 虚拟地址宏 */
//#define	SWCNT_EE_ADDR								virt_add_var_tab[0]
#define  TX_ADDRESS_EE_ADDR           virt_add_var_tab[0]
#define  BaudRate_EE_ADDR							virt_add_var_tab[2]
#define  Speed_EE_ADDR								virt_add_var_tab[4]
#define  Power_EE_ADDR								virt_add_var_tab[5]
#define  CRCMode_EE_ADDR							virt_add_var_tab[6]
#define  FreqPoint_EE_ADDR						virt_add_var_tab[7]
extern uint16_t virt_add_var_tab[NUMB_OF_VAR];
//extern uint16_t* virt_add_var_tab;
void app_ee_init(void);
void EEP_Init(void);


#ifdef __cplusplus
}
#endif

#endif
/******************* (C) COPYRIGHT 2015-2020 国源容开开发团队 *****END OF FILE****/
