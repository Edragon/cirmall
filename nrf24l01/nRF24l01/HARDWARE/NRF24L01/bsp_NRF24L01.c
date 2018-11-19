/* ����ͷ�ļ� ----------------------------------------------------------------*/
#include "NRF24L01/bsp_NRF24L01.h"
#include "Comm_GLOBALS.h"
/* ˽�����Ͷ��� --------------------------------------------------------------*/
/* ˽�к궨�� ----------------------------------------------------------------*/
/* ˽�б��� ------------------------------------------------------------------*/
SPI_HandleTypeDef hspi_NRF24L01;
//const uint8_t TX_ADDRESS[TX_ADR_WIDTH]= {0X01,0X02,0X03,0X04,0X01}; //���͵�ַ
extern uint8_t RF_Mode;
/* ��չ���� ------------------------------------------------------------------*/
/* ˽�к���ԭ�� --------------------------------------------------------------*/
/* ������ --------------------------------------------------------------------*/

/**
  * ��������: ����FLASH��ʼ��
  * �������: huart�����ھ������ָ��
  * �� �� ֵ: ��
  * ˵    ��: �ú�����HAL���ڲ�����
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
  * ��������: SPI����ϵͳ����ʼ��
  * �������: hspi��SPI�������ָ��
  * �� �� ֵ: ��
  * ˵    ��: �ú�����HAL���ڲ�����
  */
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    if(hspi->Instance==NRF24L01_SPIx)
    {
        /* SPI����ʱ��ʹ�� */
        NRF24L01_SPIx_RCC_CLK_ENABLE();
        /* GPIO����ʱ��ʹ�� */
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
  * ��������: SPI����ϵͳ������ʼ��
  * �������: hspi��SPI�������ָ��
  * �� �� ֵ: ��
  * ˵    ��: �ú�����HAL���ڲ�����
  */
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{

    if(hspi->Instance==NRF24L01_SPIx)
    {
        /* SPI����ʱ�ӽ��� */
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
  * ��������: ������Flash��ȡд��һ���ֽ����ݲ�����һ���ֽ�����
  * �������: byte������������
  * �� �� ֵ: uint8_t�����յ�������
  * ˵    ������
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
  * ��������: ���24L01�Ƿ����
  * �������: ��
  * �� �� ֵ: 0���ɹ�;1��ʧ��
  * ˵    ������
  */
uint8_t NRF24L01_Check(void)
{
    uint8_t buf[5]= {0XA5,0XA5,0XA5,0XA5,0XA5};
    uint8_t i;
    NRF24L01_Write_Buf(NRF_WRITE_REG+TX_ADDR,buf,5);//д��5���ֽڵĵ�ַ.
    NRF24L01_Read_Buf(TX_ADDR,buf,5); //����д��ĵ�ַ
    for(i=0; i<5; i++)if(buf[i]!=0XA5)break;
    if(i!=5)return 1;//���24L01����
    return 0;		 //��⵽24L01
}
/**
  * ��������: SPIд����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ����reg:ָ���Ĵ�����ַ
  *
  */
uint8_t NRF24L01_Write_Cmd(uint8_t Cmd)
{
    uint8_t status;
    NRF24L01_SPI_CS_ENABLE();                 //ʹ��SPI����
    status =SPIx_ReadWriteByte(&hspi_NRF24L01,Cmd);//���ͼĴ�����
    NRF24L01_SPI_CS_DISABLE();                 //��ֹSPI����
    return(status);       			//����״ֵ̬
}
/**
  * ��������: SPIд�Ĵ���
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ����reg:ָ���Ĵ�����ַ
  *
  */
uint8_t NRF24L01_Write_Reg(uint8_t reg,uint8_t value)
{
    uint8_t status;
    NRF24L01_SPI_CS_ENABLE();                 //ʹ��SPI����
    status =SPIx_ReadWriteByte(&hspi_NRF24L01,reg);//���ͼĴ�����
    SPIx_ReadWriteByte(&hspi_NRF24L01,value);      //д��Ĵ�����ֵ
    NRF24L01_SPI_CS_DISABLE();                 //��ֹSPI����
    return(status);       			//����״ֵ̬
}

/**
  * ��������: ��ȡSPI�Ĵ���ֵ
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ����reg:Ҫ���ļĴ���
  *
  */
uint8_t NRF24L01_Read_Reg(uint8_t reg)
{
    uint8_t reg_val;
    NRF24L01_SPI_CS_ENABLE();          //ʹ��SPI����
    SPIx_ReadWriteByte(&hspi_NRF24L01,reg);   //���ͼĴ�����
    reg_val=SPIx_ReadWriteByte(&hspi_NRF24L01,NOP);//��ȡ�Ĵ�������
    NRF24L01_SPI_CS_DISABLE();          //��ֹSPI����
    return(reg_val);           //����״ֵ̬
}

/**
  * ��������: ��ָ��λ�ö���ָ�����ȵ�����
  * �������: ��
  * �� �� ֵ: �˴ζ�����״̬�Ĵ���ֵ
  * ˵    ������
  *
  */
uint8_t NRF24L01_Read_Buf(uint8_t reg,uint8_t *pBuf,uint8_t len)
{
    uint8_t status,uint8_t_ctr;

    NRF24L01_SPI_CS_ENABLE();           //ʹ��SPI����
    status=SPIx_ReadWriteByte(&hspi_NRF24L01,reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
    for(uint8_t_ctr=0; uint8_t_ctr<len; uint8_t_ctr++)
    {
        pBuf[uint8_t_ctr]=SPIx_ReadWriteByte(&hspi_NRF24L01,NOP);//��������
    }
    NRF24L01_SPI_CS_DISABLE();       //�ر�SPI����
    return status;        //���ض�����״ֵ̬
}

/**
  * ��������: ��ָ��λ��дָ�����ȵ�����
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ����reg:�Ĵ���(λ��)  *pBuf:����ָ��  len:���ݳ���
  *
  */
uint8_t NRF24L01_Write_Buf(uint8_t reg, uint8_t *pBuf, uint8_t len)
{
    uint8_t status,uint8_t_ctr;
    NRF24L01_SPI_CS_ENABLE();          //ʹ��SPI����
    status = SPIx_ReadWriteByte(&hspi_NRF24L01,reg);//���ͼĴ���ֵ(λ��),����ȡ״ֵ̬
    for(uint8_t_ctr=0; uint8_t_ctr<len; uint8_t_ctr++)
    {
        SPIx_ReadWriteByte(&hspi_NRF24L01,*pBuf++); //д������
    }
    NRF24L01_SPI_CS_DISABLE();       //�ر�SPI����
    return status;          //���ض�����״ֵ̬
}

/**
  * @brief :���TX������
  * @param :��
  * @note  :��
  * @retval:��
  */
void NRF24L01_Flush_Tx_Fifo ( void )
{

    NRF24L01_Write_Cmd( FLUSH_TX );	//��TX FIFO����

}

/**
  * @brief :���RX������
  * @param :��
  * @note  :��
  * @retval:��
  */
void NRF24L01_Flush_Rx_Fifo( void )
{

    NRF24L01_Write_Cmd( FLUSH_RX );	//��RX FIFO����
}
/**
  * @brief :����ʹ����һ������
  * @param :��
  * @note  :��
  * @retval:��
  */
void NRF24L01_Reuse_Tx_Payload( void )
{
    NRF24L01_Write_Cmd( REUSE_TX_PL);		//����ʹ����һ������
}
/**
  * @brief :NRF24L01�ղ���
  * @param :��
  * @note  :��
  * @retval:��
  */
void NRF24L01_Nop( void )
{
    NRF24L01_Write_Cmd( NOP );		//�ղ�������
}

/**
  * @brief :NRF24L01��״̬�Ĵ���
  * @param :��
  * @note  :��
  * @retval:RF24L01״̬
  */
uint8_t NRF24L01_Read_Status_Register( void )
{
    uint8_t Status;

    Status = NRF24L01_Read_Reg(STATUS);  //��ȡ״̬�Ĵ�����ֵ

    return Status;
}

/**
  * @brief :NRF24L01���ж�
  * @param :
           @IRQ_Source:�ж�Դ
  * @note  :��
  * @retval:�����״̬�Ĵ�����ֵ
  */
uint8_t NRF24L01_Clear_IRQ_Flag( uint8_t IRQ_Source )
{
    uint8_t btmp = 0;

    IRQ_Source &= ( 1 << RX_DR ) | ( 1 << TX_DS ) | ( 1 << MAX_RT );	//�жϱ�־����
    btmp = NRF24L01_Read_Status_Register( );			//��״̬�Ĵ���
    NRF24L01_CE_LOW();
    NRF24L01_Write_Reg(NRF_WRITE_REG + STATUS,IRQ_Source | btmp);	//д״̬�Ĵ�������
    NRF24L01_CE_HIGH();
    return ( NRF24L01_Read_Status_Register( ));			//����״̬�Ĵ���״̬
}

/**
  * @brief :��RF24L01�ж�״̬
  * @param :��
  * @note  :��
  * @retval:�ж�״̬
  */
uint8_t RF24L01_Read_IRQ_Status( void )
{
    return ( NRF24L01_Read_Status_Register( ) & (( 1 << RX_DR ) | ( 1 << TX_DS ) | ( 1 << MAX_RT )));	//�����ж�״̬
}

/**
 * @brief :��FIFO�����ݿ��
 * @param :��
 * @note  :��
 * @retval:���ݿ��
 */
uint8_t NRF24L01_Read_Top_Fifo_Width( void )
{
    uint8_t btmp;
    btmp = NRF24L01_Read_Reg( R_RX_PL_WID );	//������
    return btmp;
}

/**
 * @brief :�����յ�������
 * @param :��
 * @note  :��
 * @retval:
          @pRxBuf:���ݴ�ŵ�ַ�׵�ַ
 */
uint8_t NRF24L01_Read_Rx_Payload( uint8_t *pRxBuf )
{
    uint8_t Width, PipeNum;
    PipeNum = ( NRF24L01_Read_Reg( STATUS ) >> 1 ) & 0x07;	//������״̬
    Width = NRF24L01_Read_Top_Fifo_Width();		//���������ݸ���
    NRF24L01_Read_Buf(RD_RX_PLOAD,pRxBuf,Width);//��ȡ����
    NRF24L01_Write_Cmd(FLUSH_RX);//���RX FIFO�Ĵ���
    return Width;
}

/**
 * @brief :�������ݣ���Ӧ��
 * @param :
 *			@pTxBuf:�������ݵ�ַ
 *			@len:����
 * @note  :һ�β�����32���ֽ�
 * @retval:��
 */
void NRF24L01_Write_Tx_Payload_Ack( uint8_t *pTxBuf, uint8_t len )
{
    uint8_t btmp;
    uint8_t length = ( len > 32 ) ? 32 : len;		//���ݳ����Լ32 ��ֻ����32��

    NRF24L01_Flush_Tx_Fifo( );		//��TX FIFO
    NRF24L01_CE_LOW();
    NRF24L01_Write_Buf(WR_TX_PLOAD,pTxBuf,length);//д���ݵ�TX BUF  32���ֽ�
    NRF24L01_CE_HIGH();
}

/**
 * @brief :�������ݣ�����Ӧ��
 * @param :
 *			@pTxBuf:�������ݵ�ַ
 *			@len:����
 * @note  :һ�β�����32���ֽ�
 * @retval:��
 */
void NRF24L01_Write_Tx_Payload_NoAck( uint8_t *pTxBuf, uint8_t len )
{
    if( len > 32 || len == 0 )
    {
        return ;		//���ݳ��ȴ���32 ���ߵ���0 ��ִ��
    }
    NRF24L01_CE_LOW();
    NRF24L01_Write_Buf(WR_TX_PLOAD,pTxBuf,len);//д���ݵ�TX BUF  32���ֽ�
    NRF24L01_CE_HIGH();//��������
}

/**
 * @brief :�ڽ���ģʽ����TX FIFOд����(��ACK)
 * @param :
 *			@pData:���ݵ�ַ
 *			@len:����
 * @note  :һ�β�����32���ֽ�
 * @retval:��
 */
void NRF24L01_Write_Tx_Payload_InAck( uint8_t *pData, uint8_t len )
{
    uint8_t btmp;

    len = ( len > 32 ) ? 32 : len;		//���ݳ��ȴ���32����ֻд32���ֽ�
    NRF24L01_CE_LOW();
    NRF24L01_Write_Buf(W_ACK_PLOAD,pData,len);//д���ݵ�TX BUF  32���ֽ�
    NRF24L01_CE_HIGH();
}

/**
 * @brief :���÷��͵�ַ
 * @param :
 *			@pAddr:��ַ��ŵ�ַ
 *			@len:����
 * @note  :��
 * @retval:��
 */
void NRF24L01_Set_TxAddr( uint8_t *pAddr, uint8_t len )
{
    len = ( len > 5 ) ? 5 : len;					//��ַ���ܴ���5���ֽ�
    NRF24L01_CE_LOW();
    NRF24L01_Write_Buf( NRF_WRITE_REG+TX_ADDR, pAddr, len );	//д��ַ
    NRF24L01_CE_HIGH();
}

/**
 * @brief :���ý���ͨ����ַ
 * @param :
 *			@PipeNum:ͨ��
 *			@pAddr:��ַ����ŵ�ַ
 *			@Len:����
 * @note  :ͨ��������5 ��ַ���Ȳ�����5���ֽ�
 * @retval:��
 */
void NRF24L01_Set_RxAddr( uint8_t PipeNum, uint8_t *pAddr, uint8_t Len )
{
    Len = ( Len > 5 ) ? 5 : Len;
    PipeNum = ( PipeNum > 5 ) ? 5 : PipeNum;		//ͨ��������5 ��ַ���Ȳ�����5���ֽ�
    NRF24L01_CE_LOW();
    NRF24L01_Write_Buf( NRF_WRITE_REG+RX_ADDR_P0 + PipeNum, pAddr, Len );	//д���ַ
    NRF24L01_CE_HIGH();
}

/**
 * @brief :����ͨ���ٶ�
 * @param :
 *			@Speed:�ٶ�
 * @note  :��
 * @retval:��
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
 * @brief :���ù���
 * @param :
 *			@Speed:�ٶ�
 * @note  :��
 * @retval:��
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
 * @brief :����ģʽ
 * @param :
 *			@Mode:ģʽ����ģʽ�����ģʽ
 * @note  :��
 * @retval:��
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
    NRF24L01_CE_HIGH();//��������
}

/**
 * @brief :����CRC ģʽ
 * @param :
 *			@Mode:��0��-8 λ CRC У��
							��1��-16 λ CRC У��
 * @note  :��
 * @retval:��
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
    NRF24L01_CE_HIGH();//��������
}
/**
 * @brief :����Ƶ��
 * @param :
 *			@FreqPoint:Ƶ�����ò���
 * @note  :ֵ������127
 * @retval:��
 */
void RF24LL01_Write_Hopping_Point( uint8_t FreqPoint )
{
    NRF24L01_CE_LOW();
    NRF24L01_Write_Reg( NRF_WRITE_REG+ RF_CH, FreqPoint & 0x7F );
    NRF24L01_CE_HIGH();//��������
}
/**
  * @brief :NRF24L01����һ������
  * @param :
  *			@txbuf:�����������׵�ַ
  *			@Length:�������ݳ���
  * @note  :��
  * @retval:
  *			MAX_TX���ﵽ����ط�����
  *			TX_OK���������
  *			0xFF:����ԭ��
  */
uint8_t NRF24L01_TxPacket( uint8_t *txbuf, uint8_t Length )
{
    uint8_t l_Status = 0;
    uint16_t l_MsTimes = 0;
    NRF24L01_CE_LOW();
    NRF24L01_Write_Buf( WR_TX_PLOAD_NACK, txbuf, Length );	//д���ݵ�TX BUF 32�ֽ�  TX_PLOAD_WIDTH
    NRF24L01_CE_HIGH();//��������
    while( 0 != NRF24L01_IRQ_PIN_READ())
    {
        HAL_Delay( 1 );
        if( 20 == l_MsTimes++ )						//20ms��û�з��ͳ� �������³�ʼ���豸
        {
            NRF24L01_Init();
            break;
        }
    }
    l_Status = NRF24L01_Read_Reg(STATUS);						//��״̬�Ĵ���
    NRF24L01_CE_LOW();
    NRF24L01_Write_Reg( NRF_WRITE_REG+STATUS, l_Status );						//���TX_DS��MAX_RT�жϱ�־
    NRF24L01_CE_HIGH();
    if( l_Status & MAX_TX )	//�ﵽ����ط�����
    {
        NRF24L01_Write_Cmd( FLUSH_TX );	//���TX FIFO�Ĵ� ��
        return MAX_TX;
    }
    if( l_Status & TX_OK )	//�������
    {
        return TX_OK;
    }

    return 0xFF;	//����ԭ����ʧ��
}

/**
  * @brief :NRF24L01��������
  * @param :
  *			@rxbuf:�������ݴ�ŵ�ַ
  * @note  :��
  * @retval:
  *			0:�������
  *			1:û�н��յ�����
  */
uint8_t NRF24L01_RxPacket( uint8_t *rxbuf )
{
    uint8_t l_Status = 0, l_RxLength = 0;
    l_Status = NRF24L01_Read_Reg( STATUS );		//��״̬�Ĵ���
    NRF24L01_Write_Reg(NRF_WRITE_REG+ STATUS,l_Status );		//���жϱ�־
    if( l_Status & RX_OK)	//���յ�����
    {
        l_RxLength = NRF24L01_Read_Reg( R_RX_PL_WID );		//��ȡ���յ������ݸ���
        NRF24L01_Read_Buf( RD_RX_PLOAD,rxbuf,l_RxLength );	//���յ�����
        NRF24L01_Write_Cmd( FLUSH_RX );				//���RX FIFO
        return l_RxLength;
    }
    return 0;				//û���յ�����
}
/**
  * ��������: �ú�����ʼ��NRF24L01
  * �������: ��
  * �� �� ֵ: ��
  * ˵    ������
  *
  */
void NRF24L01_Init(void)
{
    NRF24L01_CE_LOW();

    NRF24L01_Write_Reg( NRF_WRITE_REG+DYNPD, ( 1 << 0 ) ); 	//ʹ��ͨ��0��̬���ݳ���
    NRF24L01_Write_Reg( NRF_WRITE_REG+FEATRUE, 0x07 );

    NRF24L01_Write_Reg( NRF_WRITE_REG+EN_AA, 0x00);   		//��ֹ�Զ�Ӧ��
    NRF24L01_Write_Reg( NRF_WRITE_REG+SETUP_RETR,0x00);   //��ֹ�����Զ��ط�
    NRF24L01_Write_Reg( NRF_WRITE_REG+EN_RXADDR,0x01 );		//ʹ��ͨ��0����
    NRF24L01_Write_Reg( NRF_WRITE_REG+SETUP_AW, AW_5BYTES );//��ַ��� 5���ֽ�
//    NRF24L01_Write_Reg( NRF_WRITE_REG+RF_CH, FreqPoint );   //���ù���ͨ��Ƶ��
//    NRF24L01_Write_Reg( NRF_WRITE_REG+RF_SETUP,0x0f);  //����TX�������,0db����,2Mbps,���������濪��
//    NRF24L01_Write_Reg( NRF_WRITE_REG+CONFIG,0x0f);    //���û�������ģʽ�Ĳ���;PWR_UP,EN_CRC,16BIT_CRC,����ģʽ,����������
    NRF24L01_Set_TxAddr(  TX_ADDRESS, 5);
    NRF24L01_Set_RxAddr( 0,TX_ADDRESS, 5 );
    RF24LL01_Write_Hopping_Point(FreqPoint);
    NRF24L01_Set_Speed(  Speed );
    NRF24L01_Set_Power(  Power );
    RF24L01_Set_CRCMode(  CRCMode );
    RF24L01_Set_Mode(  MODE_RX );
    NRF24L01_CE_HIGH();//CEΪ��,10us����������
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

/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/

