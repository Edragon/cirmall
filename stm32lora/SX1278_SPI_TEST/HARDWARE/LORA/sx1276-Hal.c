/*
 * THE FOLLOWING FIRMWARE IS PROVIDED: (1) "AS IS" WITH NO WARRANTY; AND 
 * (2)TO ENABLE ACCESS TO CODING INFORMATION TO GUIDE AND FACILITATE CUSTOMER.
 * CONSEQUENTLY, SEMTECH SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 * 
 * Copyright (C) SEMTECH S.A.
 */
/*! 
 * \file       sx1276-Hal.c
 * \brief      SX1276 Hardware Abstraction Layer
 *
 * \version    2.0.B2 
 * \date       Nov 21 2012
 * \author     Miguel Luis
 *
 * Last modified by Miguel Luis on Jun 19 2013
 */
#include <stdint.h>
#include <stdbool.h> 

//#include "platform.h"

//#if defined( USE_SX1276_RADIO )

//#include "ioe.h"
#include "spi.h"
#include "sx1276-Hal.h"




#if 0
/*!
 * SX1276 RESET I/O definitions
 */
#if defined( STM32F4XX ) || defined( STM32F2XX ) 
#define RESET_IOPORT                                GPIOB
#define RESET_PIN                                   GPIO_Pin_11
#elif defined( STM32F429_439xx )
#define RESET_IOPORT                                GPIOB
#define RESET_PIN                                   GPIO_Pin_11
#else
#define RESET_IOPORT                                GPIOB
#define RESET_PIN                                   GPIO_Pin_11
#endif

/*!
 * SX1276 SPI NSS I/O definitions
 */
#if defined( STM32F4XX ) || defined( STM32F2XX )
#define NSS_IOPORT                                  GPIOA
#define NSS_PIN                                     GPIO_Pin_4
#elif defined( STM32F429_439xx )
#define NSS_IOPORT                                  GPIOA
#define NSS_PIN                                     GPIO_Pin_4
#else
#define NSS_IOPORT                                  GPIOA
#define NSS_PIN                                     GPIO_Pin_4
#endif

/*!
 * SX1276 DIO pins  I/O definitions
 */
#if defined( STM32F4XX ) || defined( STM32F2XX ) 
#define DIO0_IOPORT                                 GPIOB
#define DIO0_PIN                                    GPIO_Pin_10
#elif defined( STM32F429_439xx )
#define DIO0_IOPORT                                 GPIOB
#define DIO0_PIN                                    GPIO_Pin_10
#else
#define DIO0_IOPORT                                 GPIOA
#define DIO0_PIN                                    GPIO_Pin_0
#endif

#if defined( STM32F4XX ) || defined( STM32F2XX )
#define DIO1_IOPORT                                 GPIOB
#define DIO1_PIN                                    GPIO_Pin_8
#elif defined( STM32F429_439xx )
#define DIO1_IOPORT                                 GPIOB
#define DIO1_PIN                                    GPIO_Pin_7
#else
#define DIO1_IOPORT                                 GPIOB
#define DIO1_PIN                                    GPIO_Pin_0
#endif

#if defined( STM32F4XX ) || defined( STM32F2XX ) 
#define DIO2_IOPORT                                 GPIOA
#define DIO2_PIN                                    GPIO_Pin_2
#elif defined( STM32F429_439xx )
#define DIO2_IOPORT                                 GPIOA
#define DIO2_PIN                                    GPIO_Pin_2
#else
#define DIO2_IOPORT                                 GPIOC
#define DIO2_PIN                                    GPIO_Pin_5
#endif

#if defined( STM32F4XX ) || defined( STM32F2XX ) || defined( STM32F429_439xx )
#define DIO3_IOPORT                                 
#define DIO3_PIN                                    RF_DIO3_PIN
#else
#define DIO3_IOPORT                                 
#define DIO3_PIN                                    RF_DIO3_PIN
#endif

#if defined( STM32F4XX ) || defined( STM32F2XX ) || defined( STM32F429_439xx )
#define DIO4_IOPORT                                 
#define DIO4_PIN                                    RF_DIO4_PIN
#else
#define DIO4_IOPORT                                 
#define DIO4_PIN                                    RF_DIO4_PIN
#endif

#if defined( STM32F4XX ) || defined( STM32F2XX ) || defined( STM32F429_439xx )
#define DIO5_IOPORT                                 
#define DIO5_PIN                                    RF_DIO5_PIN
#else
#define DIO5_IOPORT                                 
#define DIO5_PIN                                    RF_DIO5_PIN
#endif

#if defined( STM32F4XX ) || defined( STM32F2XX ) || defined( STM32F429_439xx )
#define RXTX_IOPORT                                 
#define RXTX_PIN                                    FEM_CTX_PIN
#else
#define RXTX_IOPORT                                 
#define RXTX_PIN                                    FEM_CTX_PIN
#endif

#endif





void SX1276InitIo( void )
{

#if 0
	GPIO_InitTypeDef GPIO_InitStructure;
#if defined( STM32F4XX ) || defined( STM32F2XX ) || defined( STM32F429_439xx )
    RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB |
                            RCC_AHB1Periph_GPIOG, ENABLE );
#else
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                            RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE );
#endif

#if defined( STM32F4XX ) || defined( STM32F2XX ) || defined( STM32F429_439xx )
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
#else
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
#endif
    
    // Configure NSS as output
    GPIO_WriteBit( NSS_IOPORT, NSS_PIN, Bit_SET );
    GPIO_InitStructure.GPIO_Pin = NSS_PIN;
    GPIO_Init( NSS_IOPORT, &GPIO_InitStructure );

    // Configure radio DIO as inputs
#if defined( STM32F4XX ) || defined( STM32F2XX ) || defined( STM32F429_439xx )
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
#else
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
#endif

    // Configure DIO0
    GPIO_InitStructure.GPIO_Pin =  DIO0_PIN;
    GPIO_Init( DIO0_IOPORT, &GPIO_InitStructure );
    
    // Configure DIO1
    GPIO_InitStructure.GPIO_Pin =  DIO1_PIN;
    GPIO_Init( DIO1_IOPORT, &GPIO_InitStructure );
    
    // Configure DIO2
    GPIO_InitStructure.GPIO_Pin =  DIO2_PIN;
    GPIO_Init( DIO2_IOPORT, &GPIO_InitStructure );
    
    // REAMARK: DIO3/4/5 configured are connected to IO expander

    // Configure DIO3 as input
    
    // Configure DIO4 as input
    
    // Configure DIO5 as input
#endif

/////////lora SPI2 init////////////////////////////////////////////////////////////////////////////////////////////////////
	  GPIO_InitTypeDef GPIO_InitStructure;
		SPI_InitTypeDef  SPI_InitStructure;
	  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA, ENABLE );//PORTB PORTD时钟使能 
	  RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1,	ENABLE );//SPI2时钟使能   
//lora cs init
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;  // PB12 推挽  LORA_CS
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//推挽输出
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);
	  GPIO_SetBits(GPIOA,GPIO_Pin_4);
	  LORA_CS=1;			  //LORA不选中
//lora dio0	init    dio1 dio2 init but no use just for compile
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_3|GPIO_Pin_6; //初始化 RA_IRQ-->GPIOD.12
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PD12 设置成输入，下拉
	  GPIO_Init(GPIOB, &GPIO_InitStructure); //初始化 GPIOD.12

//lora reset init
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;	// PB13 推挽  LORA_rst
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //推挽输出
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_SetBits(GPIOB,GPIO_Pin_11);
		LORA_RST=1;				//LORA不选中
//lora spi init
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //PB13/14/15复用推挽输出 
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOB
	
	  GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);	//PB13/14/15上拉
	
	  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	  SPI_InitStructure.SPI_Mode = SPI_Mode_Master; 	  //设置SPI工作模式:设置为主SPI
	  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; 	  //设置SPI的数据大小:SPI发送接收8位帧结构

	  //SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;	  //串行同步时钟的空闲状态为高电平
	  //SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	  //串行同步时钟的第二个跳变沿（上升或下降）数据被采样

	  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;	  //串行同步时钟的空闲状态为高电平
	  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	  //串行同步时钟的第二个跳变沿（上升或下降）数据被采样


	  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; 	  //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;		  //定义波特率预分频的值:波特率预分频值为16
	  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;  //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	  SPI_InitStructure.SPI_CRCPolynomial = 7;	  //CRC值计算的多项式
	  SPI_Init(SPI1, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	
	  SPI_Cmd(SPI1, ENABLE); //使能SPI外设
	  
	  SPI1_ReadWriteByte(0xff);//启动传输 

	//SPI2_SetSpeed(SPI_BaudRatePrescaler_2);//设置为18M时钟,高速模式 



}


#if 0
void SX1276SetReset( uint8_t state )
{
    GPIO_InitTypeDef GPIO_InitStructure;

    if( state == RADIO_RESET_ON )
    {
        // Set RESET pin to 0
        GPIO_WriteBit( RESET_IOPORT, RESET_PIN, Bit_RESET );

        // Configure RESET as output
#if defined( STM32F4XX ) || defined( STM32F2XX ) || defined( STM32F429_439xx )
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
#else

        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
#endif        
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Pin = RESET_PIN;
        GPIO_Init( RESET_IOPORT, &GPIO_InitStructure );
    }
    else
    {
#if FPGA == 0    
        // Configure RESET as input
#if defined( STM32F4XX ) || defined( STM32F2XX ) || defined( STM32F429_439xx )
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
#else
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
#endif        
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Pin =  RESET_PIN;
        GPIO_Init( RESET_IOPORT, &GPIO_InitStructure );
#else
        GPIO_WriteBit( RESET_IOPORT, RESET_PIN, Bit_RESET );
#endif
    }
}
#endif


void SX1276Write( uint8_t addr, uint8_t data )
{
    SX1276WriteBuffer( addr, &data, 1 );
}

void SX1276Read( uint8_t addr, uint8_t *data )
{
    SX1276ReadBuffer( addr, data, 1 );
}

void SX1276WriteBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;

    //NSS = 0;
    //GPIO_WriteBit( NSS_IOPORT, NSS_PIN, Bit_RESET );
	LORA_CS = 0;
    //SpiInOut( addr | 0x80 );
	SPI1_ReadWriteByte(addr | 0x80);
	for( i = 0; i < size; i++ )
    {
        //SpiInOut( buffer[i] );
		SPI1_ReadWriteByte(buffer[i]);

	}

    //NSS = 1;
    //GPIO_WriteBit( NSS_IOPORT, NSS_PIN, Bit_SET );
	LORA_CS = 1;
}

void SX1276ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    uint8_t i;

    //NSS = 0;
    //GPIO_WriteBit( NSS_IOPORT, NSS_PIN, Bit_RESET );
	LORA_CS = 0;
    //SpiInOut( addr & 0x7F );
	SPI1_ReadWriteByte(addr & 0x7F);

    for( i = 0; i < size; i++ )
    {
        //buffer[i] = SpiInOut( 0 );
		buffer[i] = SPI1_ReadWriteByte(0);

	}

    //NSS = 1;
    //GPIO_WriteBit( NSS_IOPORT, NSS_PIN, Bit_SET );
	LORA_CS = 1;
}

void SX1276WriteFifo( uint8_t *buffer, uint8_t size )
{
    SX1276WriteBuffer( 0, buffer, size );
}

void SX1276ReadFifo( uint8_t *buffer, uint8_t size )
{
    SX1276ReadBuffer( 0, buffer, size );
}

#if 0
inline uint8_t SX1276ReadDio0( void )
{
    return GPIO_ReadInputDataBit( DIO0_IOPORT, DIO0_PIN );
}

inline uint8_t SX1276ReadDio1( void )
{
    return GPIO_ReadInputDataBit( DIO1_IOPORT, DIO1_PIN );
}

inline uint8_t SX1276ReadDio2( void )
{
    return GPIO_ReadInputDataBit( DIO2_IOPORT, DIO2_PIN );
}

inline uint8_t SX1276ReadDio3( void )
{
    return IoePinGet( RF_DIO3_PIN );
}

inline uint8_t SX1276ReadDio4( void )
{
    return IoePinGet( RF_DIO4_PIN );
}

inline uint8_t SX1276ReadDio5( void )
{
    return IoePinGet( RF_DIO5_PIN );
}
#endif


void SX1276WriteRxTx( uint8_t txEnable )
{
#if 0	
    if( txEnable != 0 )
    {
        IoePinOn( FEM_CTX_PIN );
        IoePinOff( FEM_CPS_PIN );
    }
    else
    {
        IoePinOff( FEM_CTX_PIN );
        IoePinOn( FEM_CPS_PIN );
    }
#endif
}

//#endif // USE_SX1276_RADIO
