#include "e2prom\emulation_ee.h"


uint16_t data_var = 0;
uint32_t cur_wr_address;   /* current W/R address */
uint32_t page_error = 0;

uint16_t VALID_PAGE = 0;
//uint16_t RECEIVE_DATA = 0;

//#define ERASED                  ((uint16_t)0xFFFF)     /* PAGE is empty */
//#define RECEIVE_DATA            ((uint16_t)0xEEEE)     /* PAGE is marked to receive data */
//#define VALID_PAGE              ((uint16_t)0x0000)     /* PAGE containing valid data */
FLASH_EraseInitTypeDef erase_initstruct =
{
    .TypeErase = FLASH_TYPEERASE_PAGES,
    .NbPages = 1,
    .PageAddress = PAGE0_BASE_ADDRESS
};

extern uint16_t virt_add_var_tab[NUMB_OF_VAR];

static uint16_t __ee_init(void);
static HAL_StatusTypeDef ee_format(void);
//static uint16_t ee_findvalidpage(uint8_t operation);
static uint16_t ee_verifypagefullwritevariable(uint16_t virt_addr, uint16_t data);
static uint16_t ee_pagetransfer(uint16_t virt_addr, uint16_t data);
static uint16_t init_cur_wr_address(void);
static uint16_t EE_ReadVariable_t(uint16_t virt_addr, uint16_t* data, uint32_t  old_page_address);



uint16_t EE_Init(void)
{
    uint16_t flash_status;
    flash_status = __ee_init();
    init_cur_wr_address();
    return flash_status;
}


/**
  * @brief  Restore the pages to a known good state in case of page's status
  *   corruption after a power loss.
  * @param  None.
  * @retval - Flash error code: on write Flash error
  *         - FLASH_COMPLETE: on success
  */
uint16_t __ee_init(void)
{
    uint16_t page_status[3] = {0};
    uint16_t flash_status;
    uint8_t i = 0;
    /* get page0 actual status */
    page_status[0] = (*(__IO uint16_t*)PAGE0_BASE_ADDRESS);
    /* get page1 actual status */
    page_status[1] = (*(__IO uint16_t*)PAGE1_BASE_ADDRESS);
    /* get page1 actua2 status */
    page_status[2] = (*(__IO uint16_t*)PAGE2_BASE_ADDRESS);

    for( i = 0; i < 3; i++)
    {
        if(page_status[i] != 0xffff)break;
    }
    switch(i)
    {
    case 0:
            
        VALID_PAGE = PAGE0;
        break;
    case 1:
       
        VALID_PAGE = PAGE1;
        break;
    case 2:
        
        VALID_PAGE = PAGE2;
        break;
    default:
        flash_status = ee_format();
        if(flash_status != HAL_OK)
            return flash_status;
        break;
    }
    return HAL_OK;
}


HAL_StatusTypeDef ee_format(void)
{
    HAL_StatusTypeDef status = HAL_OK;

    erase_initstruct.PageAddress = PAGE0_BASE_ADDRESS;
    status = HAL_FLASHEx_Erase(&erase_initstruct, &page_error);
    if(status != HAL_OK)
        return status;

    /* SET page0 as valid page: write VALID_PAGE at page0 base address */
    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,
                               PAGE0_BASE_ADDRESS,
                               0xfffe);
    VALID_PAGE = PAGE0;
//    RECEIVE_DATA = PAGE1;
    if(status != HAL_OK)
        return status;

    /* erase page1 */
    erase_initstruct.PageAddress = PAGE1_BASE_ADDRESS;
    status = HAL_FLASHEx_Erase(&erase_initstruct, &page_error);
    if(status != HAL_OK)
        return status;
    erase_initstruct.PageAddress = PAGE2_BASE_ADDRESS;
    status = HAL_FLASHEx_Erase(&erase_initstruct, &page_error);

    return status;
}



/**
  * @brief  Writes/upadtes variable data in EEPROM.
  * @param  VirtAddress: Variable virtual address
  * @param  Data: 16 bit data to be written
  * @retval Success or error status:
  *           - FLASH_COMPLETE: on success
  *           - PAGE_FULL: if valid page is full
  *           - NO_VALID_PAGE: if no valid page was found
  *           - Flash error code: on write Flash error
  */
uint16_t EE_WriteVariable(uint16_t virt_addr, uint16_t data)
{
    uint16_t status = 0;

    status = ee_verifypagefullwritevariable(virt_addr, data);
    if(status == PAGE_FULL)
        status = ee_pagetransfer(virt_addr, data);

    return status;
}


/**
  * @brief  Returns the last stored variable data, if found, which correspond to
  *   the passed virtual address
  * @param  VirtAddress: Variable virtual address
  * @param  Data: Global variable contains the read variable value
  * @retval Success or error status:
  *           - 0: if variable was found
  *           - 1: if the variable was not found
  *           - NO_VALID_PAGE: if no valid page was found.
  */
uint16_t EE_ReadVariable(uint16_t virt_addr, uint16_t* data)
{
    uint16_t validpage = PAGE0;
    uint16_t address_value = 0x5555, read_status = 1;
    uint32_t address = 0x08010000, page_start_address = 0x08010000;

//    validpage = ee_findvalidpage(READ_FROM_VALID_PAGE);
    validpage = VALID_PAGE;
    if(validpage == NO_VALID_PAGE)
        return NO_VALID_PAGE;

    page_start_address = (uint32_t)(EEPROM_START_ADDRESS + (uint32_t)(validpage * PAGE_SIZE));

    address = cur_wr_address - 2;

    while(address > (page_start_address + 2))
    {
        address_value = (*(__IO uint16_t*)address);

        if(address_value == virt_addr)
        {
            *data = (*(__IO uint16_t*)(address - 2));

            read_status = 0;
            break;
        } else {
            address = address - 4;
        }
    }

    return read_status;
}


/**
  * @brief  Verify if active page is full and Writes variable in EEPROM.
  * @param  VirtAddress: 16 bit virtual address of the variable
  * @param  Data: 16 bit data to be written as variable value
  * @retval Success or error status:
  *           - FLASH_COMPLETE: on success
  *           - PAGE_FULL: if valid page is full
  *           - NO_VALID_PAGE: if no valid page was found
  *           - Flash error code: on write Flash error
  */
static uint16_t ee_verifypagefullwritevariable(uint16_t virt_addr, uint16_t data)
{
    uint16_t status = HAL_OK;
    uint16_t validpage = PAGE0;
    uint32_t page_end_address = 0x080107FF;

    /* get valid page for write operation */
//    validpage = ee_findvalidpage(WRITE_IN_VALID_PAGE);
    validpage = VALID_PAGE;
    if(validpage == NO_VALID_PAGE)
        return NO_VALID_PAGE;

    /* get the valid page end address */
    page_end_address = (uint32_t)((EEPROM_START_ADDRESS - 2) + (uint32_t)((1 + validpage) * PAGE_SIZE));
    /* GET the active page address starting from begining */
    while(cur_wr_address < page_end_address) {
        if((*(__IO uint32_t*)cur_wr_address) == 0xFFFFFFFF)
        {
            status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,
                                       cur_wr_address,
                                       data);
            if(status != HAL_OK)
                return status;

            status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,
                                       cur_wr_address + 2,
                                       virt_addr);
            cur_wr_address = cur_wr_address + 4;
            return status;
        } else
        {
            cur_wr_address += 4;
        }
    }

    return PAGE_FULL;
}

/**
  * @brief  Find valid Page for write or read operation
  * @param  Operation: operation to achieve on the valid page.
  *   This parameter can be one of the following values:
  *     @arg READ_FROM_VALID_PAGE: read operation from valid page
  *     @arg WRITE_IN_VALID_PAGE: write operation from valid page
  * @retval Valid page number (PAGE0 or PAGE1) or NO_VALID_PAGE in case
  *   of no valid page was found
  */
//uint16_t ee_findvalidpage(uint8_t operation)
//{
//    uint16_t page_status0 = 6, page_status1 = 6, page_status2 = 6;

//    /* get page0 actual status */
//    page_status0 = (*(__IO uint16_t*)PAGE0_BASE_ADDRESS);

//    /* get page1 actual status */
//    page_status1 = (*(__IO uint16_t*)PAGE1_BASE_ADDRESS);

//    /* get page1 actual status */
//    page_status2 = (*(__IO uint16_t*)PAGE2_BASE_ADDRESS);
/* write or read operation */
//    switch(operation)
//    {
//    case WRITE_IN_VALID_PAGE:
//        return VALID_PAGE;
//    case READ_FROM_VALID_PAGE:

//    default:
//        return PAGE0;
//    }
//}


uint16_t init_cur_wr_address(void)
{
    uint16_t validpage = PAGE0;
    uint32_t page_endaddress;

    /* get valid page for write opteration */
//    validpage = ee_findvalidpage(WRITE_IN_VALID_PAGE);
    validpage = VALID_PAGE;
    /* check if there is no valid page */
    if(validpage == NO_VALID_PAGE)
    {
        cur_wr_address = (uint32_t)(EEPROM_START_ADDRESS + (uint32_t)(validpage * PAGE_SIZE));
        return NO_VALID_PAGE;
    }

    cur_wr_address = (uint32_t)(EEPROM_START_ADDRESS + (uint32_t)(validpage * PAGE_SIZE));
    page_endaddress = (uint32_t)((EEPROM_START_ADDRESS - 2) + (uint32_t)((1 + validpage) * PAGE_SIZE));

    while(cur_wr_address < page_endaddress)
    {
        if((*(__IO uint32_t*)cur_wr_address) == 0xFFFFFFFF) {
            return HAL_OK;
        } else {
            cur_wr_address = cur_wr_address + 4;
        }
    }

    return PAGE_FULL;
}

static uint16_t EE_ReadVariable_t(uint16_t virt_addr, uint16_t* data, uint32_t  old_page_address)
{
    uint16_t address_value = 0x5555, read_status = 1;
    uint32_t address = 0x08010000, page_start_address = 0x08010000;
    page_start_address = old_page_address;
    address = (uint32_t)((old_page_address - 2) + (uint32_t) PAGE_SIZE);

    while(address > (page_start_address + 2))
    {
        address_value = (*(__IO uint16_t*)address);

        if(address_value == virt_addr)
        {
            *data = (*(__IO uint16_t*)(address - 2));

            read_status = 0;
            break;
        } else {
            address = address - 4;
        }
    }

    return read_status;
}
/**
  * @brief  Transfers last updated variables data from the full Page to
  *   an empty one.
  * @param  VirtAddress: 16 bit virtual address of the variable
  * @param  Data: 16 bit data to be written as variable value
  * @retval Success or error status:
  *           - FLASH_COMPLETE: on success
  *           - PAGE_FULL: if valid page is full
  *           - NO_VALID_PAGE: if no valid page was found
  *           - Flash error code: on write Flash error
  */
uint16_t ee_pagetransfer(uint16_t virt_addr, uint16_t data)
{
    uint16_t status = HAL_OK;
    uint32_t new_page_address = 0x080103FF, old_page_address = 0x08010000;
    uint16_t validpage = PAGE0, varidx = 0;
    uint16_t ee_status = 0, read_status = 0;
    uint16_t page_num = 0xffff;

    validpage = VALID_PAGE;
    if(validpage == PAGE1)
    {
        new_page_address = PAGE2_BASE_ADDRESS;
        old_page_address = PAGE1_BASE_ADDRESS;
        VALID_PAGE = PAGE2;
    }
    else if(validpage == PAGE2)
    {
        new_page_address = PAGE0_BASE_ADDRESS;
        old_page_address = PAGE2_BASE_ADDRESS;
        VALID_PAGE = PAGE0;
    }
    else if(validpage == PAGE0)
    {
        new_page_address = PAGE1_BASE_ADDRESS;
        old_page_address = PAGE0_BASE_ADDRESS;
        VALID_PAGE = PAGE1;
    }
    else
    {
        return NO_VALID_PAGE;
    }
    page_num = (*(__IO uint16_t*)old_page_address);
    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,
                               new_page_address,
                               page_num - 1);
    if(status != HAL_OK)
        return status;

    init_cur_wr_address();
    ee_status = ee_verifypagefullwritevariable(virt_addr, data);
    if(ee_status != HAL_OK)
        return ee_status;

    for(varidx = 0; varidx < NUMB_OF_VAR; varidx++)
    {
        if(virt_add_var_tab[varidx] != virt_addr)
        {
            read_status = EE_ReadVariable_t(virt_add_var_tab[varidx], &data_var, old_page_address);
            if(read_status != 0x1)
            {
                ee_status = ee_verifypagefullwritevariable(virt_add_var_tab[varidx], data_var);
                if(ee_status != HAL_OK)
                    return ee_status;;
            }
        }
    }

    /* erase the old page: set oldpage status to ERASED status */
    erase_initstruct.PageAddress = old_page_address;
    status = HAL_FLASHEx_Erase(&erase_initstruct, &page_error);
    if(status != HAL_OK)
        return status;


    return status;
}

/************************* END FILE OF EMULATION_EE***************************/
