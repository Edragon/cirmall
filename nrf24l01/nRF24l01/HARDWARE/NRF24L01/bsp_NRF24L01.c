/* 包含头文件 ----------------------------------------------------------------*/
#include "NRF24L01/bsp_NRF24L01.h"
#include "Comm_GLOBALS.h"
/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
/* 私有变量 ------------------------------------------------------------------*/
SPI_HandleTypeDef hspi_NRF24L01;
//const uint8_t TX_ADDRESS[TX_ADR_WIDTH]= {0X01,0X02,0X03,0X04,0X01}; //发送地址
extern uint8_t RF_Mode;
/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
/* 函数体 --------------------------------------------------------------------*/

/**
  * 函数功能: 串行FLASH初始化
  * 输入参数: huart：串口句柄类型指针
  * 返 回 值: 无
  * 说    明: 该函数被HAL库内部调用
*/
void NRF24L01_SPI_Init(void)
{
    hspi_NRF24L01.Instance = NRF24L01_SPIx;
    hspi_NRF24L01.Init.Mode = SPI_MODE_MASTER;
    hspi_NRF24L01.Init.Direction = SPI_DIRECTION_2LINES;
    hspi_NRF24L01.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi_NRF24L01.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi_NRF24L01.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi_NRF24L01.Init.NSS = SPI_NSS_SOFT;
    hspi_NRF24L01.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
    hspi_NRF24L01.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi_NRF24L01.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi_NRF24L01.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi_NRF24L01.Init.CRCPolynomial = 10;
    HAL_SPI_Init(&hspi_NRF24L01);
    __HAL_SPI_ENABLE(&hspi_NRF24L01);
}

/**
  * 函数功能: SPI外设系统级初始化
  * 输入参数: hspi：SPI句柄类型指针
  * 返 回 值: 无
  * 说    明: 该函数被HAL库内部调用
  */
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    if(hspi->Instance==NRF24L01_SPIx)
    {
        /* SPI外设时钟使能 */
        NRF24L01_SPIx_RCC_CLK_ENABLE();
        /* GPIO外设时钟使能 */
        NRF24L01_SPI_GPIO_ClK_ENABLE();
        NRF24L01_SPI_CS_CLK_ENABLE();
        NRF24L01_CE_CLK_ENABLE();
        NRF24L01_IRQ_CLK_ENABLE();

        GPIO_InitStruct.Pin = NRF24L01_SPI_SCK_PIN|NRF24L01_SPI_MOSI_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(NRF24L01_SPI_GPIO_PORT, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = NRF24L01_SPI_MISO_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(NRF24L01_SPI_GPIO_PORT, &GPIO_InitStruct);

        HAL_GPIO_WritePin(NRF24L01_SPI_CS_PORT, NRF24L01_SPI_CS_PIN, GPIO_PIN_SET);
        GPIO_InitStruct.Pin = NRF24L01_SPI_CS_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        HAL_GPIO_Init(NRF24L01_SPI_CS_PORT, &GPIO_InitStruct);

        HAL_GPIO_WritePin(NRF24L01_CE_PORT, NRF24L01_CE_PIN, GPIO_PIN_RESET);
        GPIO_InitStruct.Pin = NRF24L01_CE_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        HAL_GPIO_Init(NRF24L01_CE_PORT, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = NRF24L01_IRQ_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        HAL_GPIO_Init(NRF24L01_IRQ_PORT, &GPIO_InitStruct);
        /* EXTI interrupt init*/
        HAL_NVIC_SetPriority(NRF24L01_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(NRF24L01_IRQn);
    }
}

/**
  * 函数功能: SPI外设系统级反初始化
  * 输入参数: hspi：SPI句柄类型指针
  * 返 回 值: 无
  * 说    明: 该函数被HAL库内部调用
  */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{

    if(hspi->Instance==NRF24L01_SPIx)
    {
        /* SPI外设时钟禁用 */
        NRF24L01_SPIx_RCC_CLK_DISABLE();

        /**SPI3 GPIO Configuration
        PF11     ------> SPI3_NSS
        PB3      ------> SPI3_SCK
        PB4      ------> SPI3_MISO
        PB5      ------> SPI3_MOSI
        */
        HAL_GPIO_DeInit(NRF24L01_SPI_GPIO_PORT, NRF24L01_SPI_SCK_PIN|NRF24L01_SPI_MISO_PIN|NRF24L01_SPI_MOSI_PIN);
        HAL_GPIO_DeInit(NRF24L01_SPI_CS_PORT, NRF24L01_SPI_CS_PIN);
        HAL_GPIO_DeInit(NRF24L01_CE_PORT, NRF24L01_CE_PIN);
        HAL_GPIO_DeInit(NRF24L01_IRQ_PORT, NRF24L01_IRQ_PIN);
    }
}

/**
  * 函数功能: 往串行Flash读取写入一个字节数据并接收一个字节数据
  * 输入参数: byte：待发送数据
  * 返 回 值: uint8_t：接收到的数据
  * 说    明：无
  */
uint8_t SPIx_ReadWriteByte(SPI_HandleTypeDef* hspi,uint8_t byte)
{
    uint8_t d_read,d_send=byte;
    if(HAL_SPI_TransmitReceive(hspi,&d_send,&d_read,1,0xFF)!=HAL_OK)
    {
        d_read=0xFF;
    }
    return d_read;
}

/**
  * 函数功能: 检测24L01是否存在
  * 输入参数: 无
  * 返 回 值: 0，成功;1，失败
  * 说    明：无
  */
uint8_t NRF24L01_Check(void)
{
    uint8_t buf[5]= {0XA5,0XA5,0XA5,0XA5,0XA5};
    uint8_t i;
    NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//写入5个字节的地址.
    NRF24L01_Read_Buf(TX_ADDR,buf,5); //读出写入的地址
    for(i=0; i<5; i++)if(buf[i]!=0XA5)break;
    if(i!=5)return 1;//检测24L01错误
    return 0;		 //检测到24L01
}
/**
  * 函数功能: SPI写命令
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：reg:指定寄存器地址
  *
  */
uint8_t NRF24L01_Write_Cmd(uint8_t Cmd)
{
    uint8_t status;
    NRF24L01_SPI_CS_ENABLE();                 //使能SPI传输
    status =SPIx_ReadWriteByte(&hspi_NRF24L01,Cmd);//发送寄存器号
    NRF24L01_SPI_CS_DISABLE();                 //禁止SPI传输
    return(status);       			//返回状态值
}
/**
  * 函数功能: SPI写寄存器
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：reg:指定寄存器地址
  *
  */
uint8_t NRF24L01_Write_Reg(uint8_t reg,uint8_t value)
{
    uint8_t status;
    NRF24L01_SPI_CS_ENABLE();                 //使能SPI传输
    status =SPIx_ReadWriteByte(&hspi_NRF24L01,reg);//发送寄存器号
    SPIx_ReadWriteByte(&hspi_NRF24L01,value);      //写入寄存器的值
    NRF24L01_SPI_CS_DISABLE();                 //禁止SPI传输
    return(status);       			//返回状态值
}

/**
  * 函数功能: 读取SPI寄存器值
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：reg:要读的寄存器
  *
  */
uint8_t NRF24L01_Read_Reg(uint8_t reg)
{
    uint8_t reg_val;
    NRF24L01_SPI_CS_ENABLE();          //使能SPI传输
    SPIx_ReadWriteByte(&hspi_NRF24L01,reg);   //发送寄存器号
    reg_val=SPIx_ReadWriteByte(&hspi_NRF24L01,NOP);//读取寄存器内容
    NRF24L01_SPI_CS_DISABLE();          //禁止SPI传输
    return(reg_val);           //返回状态值
}

/**
  * 函数功能: 在指定位置读出指定长度的数据
  * 输入参数: 无
  * 返 回 值: 此次读到的状态寄存器值
  * 说    明：无
  *
  */
uint8_t NRF24L01_Read_Buf(uint8_t reg,uint8_t *pBuf,uint8_t len)
{
    uint8_t status,uint8_t_ctr;

    NRF24L01_SPI_CS_ENABLE();           //使能SPI传输
    status=SPIx_ReadWriteByte(&hspi_NRF24L01,reg);//发送寄存器值(位置),并读取状态值
    for(uint8_t_ctr=0; uint8_t_ctr<len; uint8_t_ctr++)
    {
        pBuf[uint8_t_ctr]=SPIx_ReadWriteByte(&hspi_NRF24L01,NOP);//读出数据
    }
    NRF24L01_SPI_CS_DISABLE();       //关闭SPI传输
    return status;        //返回读到的状态值
}

/**
  * 函数功能: 在指定位置写指定长度的数据
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：reg:寄存器(位置)  *pBuf:数据指针  len:数据长度
  *
  */
uint8_t NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
    uint8_t status,uint8_t_ctr;
    NRF24L01_SPI_CS_ENABLE();          //使能SPI传输
    status = SPIx_ReadWriteByte(&hspi_NRF24L01,reg);//发送寄存器值(位置),并读取状态值
    for(uint8_t_ctr=0; uint8_t_ctr<len; uint8_t_ctr++)
    {
        SPIx_ReadWriteByte(&hspi_NRF24L01,*pBuf++); //写入数据
    }
    NRF24L01_SPI_CS_DISABLE();       //关闭SPI传输
    return status;          //返回读到的状态值
}

/**
  * @brief :清空TX缓冲区
  * @param :无
  * @note  :无
  * @retval:无
  */
void NRF24L01_Flush_Tx_Fifo ( void )
{

    NRF24L01_Write_Cmd( FLUSH_TX );	//清TX FIFO命令

}

/**
  * @brief :清空RX缓冲区
  * @param :无
  * @note  :无
  * @retval:无
  */
void NRF24L01_Flush_Rx_Fifo( void )
{

    NRF24L01_Write_Cmd( FLUSH_RX );	//清RX FIFO命令
}
/**
  * @brief :重新使用上一包数据
  * @param :无
  * @note  :无
  * @retval:无
  */
void NRF24L01_Reuse_Tx_Payload( void )
{
    NRF24L01_Write_Cmd( REUSE_TX_PL);		//重新使用上一包命令
}
/**
  * @brief :NRF24L01空操作
  * @param :无
  * @note  :无
  * @retval:无
  */
void NRF24L01_Nop( void )
{
    NRF24L01_Write_Cmd( NOP );		//空操作命令
}

/**
  * @brief :NRF24L01读状态寄存器
  * @param :无
  * @note  :无
  * @retval:RF24L01状态
  */
uint8_t NRF24L01_Read_Status_Register( void )
{
    uint8_t Status;

    Status = NRF24L01_Read_Reg(STATUS);  //读取状态寄存器的值

    return Status;
}

/**
  * @brief :NRF24L01清中断
  * @param :
           @IRQ_Source:中断源
  * @note  :无
  * @retval:清除后状态寄存器的值
  */
uint8_t NRF24L01_Clear_IRQ_Flag( uint8_t IRQ_Source )
{
    uint8_t btmp = 0;

    IRQ_Source &= ( 1 << RX_DR ) | ( 1 << TX_DS ) | ( 1 << MAX_RT );	//中断标志处理
    btmp = NRF24L01_Read_Status_Register( );			//读状态寄存器
    NRF24L01_CE_LOW();
    NRF24L01_Write_Reg(NRF_WRITE_REG + STATUS,IRQ_Source | btmp);	//写状态寄存器命令
    NRF24L01_CE_HIGH();
    return ( NRF24L01_Read_Status_Register( ));			//返回状态寄存器状态
}

/**
  * @brief :读RF24L01中断状态
  * @param :无
  * @note  :无
  * @retval:中断状态
  */
uint8_t RF24L01_Read_IRQ_Status( void )
{
    return ( NRF24L01_Read_Status_Register( ) & (( 1 << RX_DR ) | ( 1 << TX_DS ) | ( 1 << MAX_RT )));	//返回中断状态
}

/**
 * @brief :读FIFO中数据宽度
 * @param :无
 * @note  :无
 * @retval:数据宽度
 */
uint8_t NRF24L01_Read_Top_Fifo_Width( void )
{
    uint8_t btmp;
    btmp = NRF24L01_Read_Reg( R_RX_PL_WID );	//读数据
    return btmp;
}

/**
 * @brief :读接收到的数据
 * @param :无
 * @note  :无
 * @retval:
          @pRxBuf:数据存放地址首地址
 */
uint8_t NRF24L01_Read_Rx_Payload( uint8_t *pRxBuf )
{
    uint8_t Width, PipeNum;
    PipeNum = ( NRF24L01_Read_Reg( STATUS ) >> 1 ) & 0x07;	//读接收状态
    Width = NRF24L01_Read_Top_Fifo_Width();		//读接收数据个数
    NRF24L01_Read_Buf(RD_RX_PLOAD,pRxBuf,Width);//读取数据
    NRF24L01_Write_Cmd(FLUSH_RX);//清除RX FIFO寄存器
    return Width;
}

/**
 * @brief :发送数据（带应答）
 * @param :
 *			@pTxBuf:发送数据地址
 *			@len:长度
 * @note  :一次不超过32个字节
 * @retval:无
 */
void NRF24L01_Write_Tx_Payload_Ack( uint8_t *pTxBuf, uint8_t len )
{
    uint8_t btmp;
    uint8_t length = ( len > 32 ) ? 32 : len;		//数据长达大约32 则只发送32个

    NRF24L01_Flush_Tx_Fifo( );		//清TX FIFO
    NRF24L01_CE_LOW();
    NRF24L01_Write_Buf(WR_TX_PLOAD,pTxBuf,length);//写数据到TX BUF  32个字节
    NRF24L01_CE_HIGH();
}

/**
 * @brief :发送数据（不带应答）
 * @param :
 *			@pTxBuf:发送数据地址
 *			@len:长度
 * @note  :一次不超过32个字节
 * @retval:无
 */
void NRF24L01_Write_Tx_Payload_NoAck( uint8_t *pTxBuf, uint8_t len )
{
    if( len > 32 || len == 0 )
    {
        return ;		//数据长度大于32 或者等于0 不执行
    }
    NRF24L01_CE_LOW();
    NRF24L01_Write_Buf(WR_TX_PLOAD,pTxBuf,len);//写数据到TX BUF  32个字节
    NRF24L01_CE_HIGH();//启动发送
}

/**
 * @brief :在接收模式下向TX FIFO写数据(带ACK)
 * @param :
 *			@pData:数据地址
 *			@len:长度
 * @note  :一次不超过32个字节
 * @retval:无
 */
void NRF24L01_Write_Tx_Payload_InAck( uint8_t *pData, uint8_t len )
{
    uint8_t btmp;

    len = ( len > 32 ) ? 32 : len;		//数据长度大于32个则只写32个字节
    NRF24L01_CE_LOW();
    NRF24L01_Write_Buf(W_ACK_PLOAD,pData,len);//写数据到TX BUF  32个字节
    NRF24L01_CE_HIGH();
}

/**
 * @brief :设置发送地址
 * @param :
 *			@pAddr:地址存放地址
 *			@len:长度
 * @note  :无
 * @retval:无
 */
void NRF24L01_Set_TxAddr( uint8_t *pAddr, uint8_t len )
{
    len = ( len > 5 ) ? 5 : len;					//地址不能大于5个字节
    NRF24L01_CE_LOW();
    NRF24L01_Write_Buf( NRF_WRITE_REG+TX_ADDR, pAddr, len );	//写地址
    NRF24L01_CE_HIGH();
}

/**
 * @brief :设置接收通道地址
 * @param :
 *			@PipeNum:通道
 *			@pAddr:地址存肥着地址
 *			@Len:长度
 * @note  :通道不大于5 地址长度不大于5个字节
 * @retval:无
 */
void NRF24L01_Set_RxAddr( uint8_t PipeNum, uint8_t *pAddr, uint8_t Len )
{
    Len = ( Len > 5 ) ? 5 : Len;
    PipeNum = ( PipeNum > 5 ) ? 5 : PipeNum;		//通道不大于5 地址长度不大于5个字节
    NRF24L01_CE_LOW();
    NRF24L01_Write_Buf( NRF_WRITE_REG+RX_ADDR_P0 + PipeNum, pAddr, Len );	//写入地址
    NRF24L01_CE_HIGH();
}

/**
 * @brief :设置通信速度
 * @param :
 *			@Speed:速度
 * @note  :无
 * @retval:无
 */
void NRF24L01_Set_Speed( nRf24l01SpeedType Speed )
{
    uint8_t btmp = 0;

    btmp = NRF24L01_Read_Reg( RF_SETUP );
    btmp &= ~( ( 1<<5 ) | ( 1<<3 ) );

    if( Speed == SPEED_250K )		//250K
    {
        btmp |= ( 1<<5 );
    }
    else if( Speed == SPEED_1M )   //1M
    {
        btmp &= ~( ( 1<<5 ) | ( 1<<3 ) );
    }
    else if( Speed == SPEED_2M )   //2M
    {
        btmp |= ( 1<<3 );
    }
    NRF24L01_CE_LOW();
    NRF24L01_Write_Reg(NRF_WRITE_REG+ RF_SETUP, btmp );
    NRF24L01_CE_HIGH();
}

/**
 * @brief :设置功率
 * @param :
 *			@Speed:速度
 * @note  :无
 * @retval:无
 */
void NRF24L01_Set_Power( nRf24l01PowerType Power )
{
    uint8_t btmp;

    btmp = NRF24L01_Read_Reg( RF_SETUP ) & ~0x07;
    switch( Power )
    {
    case POWER_F18DBM:
        btmp |= PWR_18DB;
        break;
    case POWER_F12DBM:
        btmp |= PWR_12DB;
        break;
    case POWER_F6DBM:
        btmp |= PWR_6DB;
        break;
    case POWER_0DBM:
        btmp |= PWR_0DB;
        break;
    default:
        break;
    }
    NRF24L01_CE_LOW();
    NRF24L01_Write_Reg(NRF_WRITE_REG+ RF_SETUP, btmp );
    NRF24L01_CE_HIGH();
}

/**
 * @brief :设置模式
 * @param :
 *			@Mode:模式发送模式或接收模式
 * @note  :无
 * @retval:无
 */
void RF24L01_Set_Mode( nRf24l01ModeType Mode )
{
    uint8_t controlreg = 0;
    controlreg = NRF24L01_Read_Reg( CONFIG );

    if( Mode == MODE_TX )
    {
        RF_Mode = MODE_TX;
        controlreg &= ~( 1<< PRIM_RX );
        controlreg |= ( 1<< PWR_UP );
    }
    else if( Mode == MODE_RX )
    {

        controlreg |= ( 1<< PRIM_RX );
        controlreg |= ( 1<< PWR_UP );
        RF_Mode = MODE_RX;
    }
    else
    {
        controlreg &= ~( 1<< PWR_UP );
    }

    NRF24L01_CE_LOW();
    NRF24L01_Write_Reg( NRF_WRITE_REG+CONFIG, controlreg );
    NRF24L01_CE_HIGH();//启动发送
}

/**
 * @brief :设置CRC 模式
 * @param :
 *			@Mode:‘0’-8 位 CRC 校验
							‘1’-16 位 CRC 校验
 * @note  :无
 * @retval:无
 */
void RF24L01_Set_CRCMode( nRf24l01CRCModeType CRCMode )
{
    uint8_t controlreg = 0;
    controlreg = NRF24L01_Read_Reg( CONFIG );

    if( CRCMode == MODE_8 )
    {
        controlreg &= ~( 1<< CRCO );
        controlreg |= ( 1<< EN_CRC );
    }
    else  if( CRCMode == MODE_16 )
    {
        controlreg |= ( 1<< CRCO );
        controlreg |= ( 1<< EN_CRC );
    }
    else
    {
        controlreg &= ~( 1<< EN_CRC );
    }
    NRF24L01_CE_LOW();
    NRF24L01_Write_Reg( NRF_WRITE_REG+CONFIG, controlreg );
    NRF24L01_CE_HIGH();//启动发送
}
/**
 * @brief :设置频率
 * @param :
 *			@FreqPoint:频率设置参数
 * @note  :值不大于127
 * @retval:无
 */
void RF24LL01_Write_Hopping_Point( uint8_t FreqPoint )
{
    NRF24L01_CE_LOW();
    NRF24L01_Write_Reg( NRF_WRITE_REG+ RF_CH, FreqPoint & 0x7F );
    NRF24L01_CE_HIGH();//启动发送
}
/**
  * @brief :NRF24L01发送一次数据
  * @param :
  *			@txbuf:待发送数据首地址
  *			@Length:发送数据长度
  * @note  :无
  * @retval:
  *			MAX_TX：达到最大重发次数
  *			TX_OK：发送完成
  *			0xFF:其他原因
  */
uint8_t NRF24L01_TxPacket( uint8_t *txbuf, uint8_t Length )
{
    uint8_t l_Status = 0;
    uint16_t l_MsTimes = 0;
    NRF24L01_CE_LOW();
    NRF24L01_Write_Buf( WR_TX_PLOAD_NACK, txbuf, Length );	//写数据到TX BUF 32字节  TX_PLOAD_WIDTH
    NRF24L01_CE_HIGH();//启动发送
    while( 0 != NRF24L01_IRQ_PIN_READ())
    {
        HAL_Delay( 1 );
        if( 20 == l_MsTimes++ )						//20ms还没有发送成 功，重新初始化设备
        {
            NRF24L01_Init();
            break;
        }
    }
    l_Status = NRF24L01_Read_Reg(STATUS);						//读状态寄存器
    NRF24L01_CE_LOW();
    NRF24L01_Write_Reg( NRF_WRITE_REG+STATUS, l_Status );						//清除TX_DS或MAX_RT中断标志
    NRF24L01_CE_HIGH();
    if( l_Status & MAX_TX )	//达到最大重发次数
    {
        NRF24L01_Write_Cmd( FLUSH_TX );	//清除TX FIFO寄存 器
        return MAX_TX;
    }
    if( l_Status & TX_OK )	//发送完成
    {
        return TX_OK;
    }

    return 0xFF;	//其他原因发送失败
}

/**
  * @brief :NRF24L01接收数据
  * @param :
  *			@rxbuf:接收数据存放地址
  * @note  :无
  * @retval:
  *			0:接收完成
  *			1:没有接收到数据
  */
uint8_t NRF24L01_RxPacket( uint8_t *rxbuf )
{
    uint8_t l_Status = 0, l_RxLength = 0;
    l_Status = NRF24L01_Read_Reg( STATUS );		//读状态寄存器
    NRF24L01_Write_Reg(NRF_WRITE_REG+ STATUS,l_Status );		//清中断标志
    if( l_Status & RX_OK)	//接收到数据
    {
        l_RxLength = NRF24L01_Read_Reg( R_RX_PL_WID );		//读取接收到的数据个数
        NRF24L01_Read_Buf( RD_RX_PLOAD,rxbuf,l_RxLength );	//接收到数据
        NRF24L01_Write_Cmd( FLUSH_RX );				//清除RX FIFO
        return l_RxLength;
    }
    return 0;				//没有收到数据
}
/**
  * 函数功能: 该函数初始化NRF24L01
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  *
  */
void NRF24L01_Init(void)
{
    NRF24L01_CE_LOW();

    NRF24L01_Write_Reg( NRF_WRITE_REG+DYNPD, ( 1 << 0 ) ); 	//使能通道0动态数据长度
    NRF24L01_Write_Reg( NRF_WRITE_REG+FEATRUE, 0x07 );

    NRF24L01_Write_Reg( NRF_WRITE_REG+EN_AA, 0x00);   		//禁止自动应答
    NRF24L01_Write_Reg( NRF_WRITE_REG+SETUP_RETR,0x00);   //禁止建立自动重发
    NRF24L01_Write_Reg( NRF_WRITE_REG+EN_RXADDR,0x01 );		//使能通道0接收
    NRF24L01_Write_Reg( NRF_WRITE_REG+SETUP_AW, AW_5BYTES );//地址宽度 5个字节
//    NRF24L01_Write_Reg( NRF_WRITE_REG+RF_CH, FreqPoint );   //设置工作通道频率
//    NRF24L01_Write_Reg( NRF_WRITE_REG+RF_SETUP,0x0f);  //设置TX发射参数,0db增益,2Mbps,低噪声增益开启
//    NRF24L01_Write_Reg( NRF_WRITE_REG+CONFIG,0x0f);    //配置基本工作模式的参数;PWR_UP,EN_CRC,16BIT_CRC,接收模式,开启所有中
    NRF24L01_Set_TxAddr(  TX_ADDRESS, 5);
    NRF24L01_Set_RxAddr( 0,TX_ADDRESS, 5 );
    RF24LL01_Write_Hopping_Point(FreqPoint);
    NRF24L01_Set_Speed(  Speed );
    NRF24L01_Set_Power(  Power );
    RF24L01_Set_CRCMode(  CRCMode );
    RF24L01_Set_Mode(  MODE_RX );
    NRF24L01_CE_HIGH();//CE为高,10us后启动发送
    HAL_Delay(1);
}
void NRF24L01_IRQHandler(void)
{
    /* USER CODE BEGIN EXTI0_IRQn 0 */

    /* USER CODE END EXTI0_IRQn 0 */
    if(MODE_RX == RF_Mode) HAL_GPIO_EXTI_IRQHandler(NRF24L01_IRQ_PIN);
    __HAL_GPIO_EXTI_CLEAR_IT(NRF24L01_IRQ_PIN);
    /* USER CODE BEGIN EXTI0_IRQn 1 */

    /* USER CODE END EXTI0_IRQn 1 */
}

/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/

