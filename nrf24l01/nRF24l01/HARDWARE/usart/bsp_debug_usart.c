/* 包含头文件 ----------------------------------------------------------------*/
#include "usart/bsp_debug_usart.h"
#include "NRF24L01/bsp_NRF24L01.h"
#include "Comm_GLOBALS.h"
void _Error_Handler(char * file, int line);
TIM_HandleTypeDef htim2;
uint8_t USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
volatile uint16_t USART_RX_STA = 0;     //接收状态标记
/* STM32 uart driver */
struct stm32_uart
{
    UART_HandleTypeDef huart;
    IRQn_Type irq;
};

static int stm32_putc(struct stm32_uart *uart, char c)
{
    if(uart == 0)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    while (__HAL_UART_GET_FLAG(&uart->huart, UART_FLAG_TXE) == RESET);
    uart->huart.Instance->DR = c;
    return 1;
}

static int stm32_getc(struct stm32_uart *uart)
{
    int ch;
    if(uart == 0)
    {
        _Error_Handler(__FILE__, __LINE__);
    }
    ch = -1;
    if (__HAL_UART_GET_FLAG(&uart->huart, UART_FLAG_RXNE) != RESET)
    {
        ch = uart->huart.Instance->DR & 0xff;
    }
    return ch;
}


/**
 * Uart common interrupt process. This need add to uart ISR.
 *
 * @param serial serial device
 */
static void uart_isr(struct stm32_uart *uart)
{
    int ch = -1;
    if(uart == 0)
    {
        _Error_Handler(__FILE__, __LINE__);
    }

    if ((__HAL_UART_GET_FLAG(&uart->huart, UART_FLAG_RXNE) != RESET) && (__HAL_UART_GET_IT_SOURCE(&uart->huart, UART_IT_RXNE) != RESET))
    {
        if(uart->huart.Instance == USART1 )
        {
            ch = stm32_getc(uart) ;
            if (ch == -1) return;
            if(USART_RX_STA > USART_REC_LEN)USART_RX_STA = 0;
            USART_RX_BUF[USART_RX_STA++] = ch;
            __HAL_TIM_SET_COUNTER(&htim2, 0); // 清零定时器计数
            HAL_TIM_Base_Start_IT(&htim2);
        }
        else if(uart->huart.Instance == USART2 )
        {

        }
        else if(uart->huart.Instance == USART3 )
        {

        }
        __HAL_UART_CLEAR_FLAG(&uart->huart, UART_FLAG_RXNE);
    }
}

#if defined(RT_USING_UART1)
/* UART1 device driver structure */
struct stm32_uart uart1 =
{
    {USART1},
    USART1_IRQn
};
void USART1_IRQHandler(void)
{
    uart_isr(&uart1);
}
#endif /* RT_USING_UART1 */

#if defined(RT_USING_UART2)
/* UART1 device driver structure */
struct stm32_uart uart2 =
{
    {USART2},
    USART2_IRQn
};

void USART2_IRQHandler(void)
{
    uart_isr(&uart2);
}
#endif /* RT_USING_UART2 */

#if defined(RT_USING_UART3)
/* UART1 device driver structure */
struct stm32_uart uart3 =
{
    {USART3},
    USART3_IRQn
};

void USART3_IRQHandler(void)
{
    uart_isr(&uart3);

}
#endif /* RT_USING_UART2 */
int set_usart_BaudRate(struct stm32_uart *uart,uint32_t USARTx_BaudRate)
{
    if(uart->huart.Instance == USART1)
    {
        uart->huart.Init.BaudRate = USARTx_BaudRate;
    }
    else if(uart->huart.Instance == USART2)
    {
        uart->huart.Init.BaudRate = USARTx_BaudRate;
    }
    else if(uart->huart.Instance == USART3)
    {
        uart->huart.Init.BaudRate = USARTx_BaudRate;
    }
    HAL_UART_Init(&uart->huart);
		return 0;
}

int rt_hw_usart_init(void)
{
    struct stm32_uart *uart;
#if defined(RT_USING_UART1)
    uart = &uart1;
    uart->huart.Instance = USART1;
    uart->huart.Init.BaudRate = BaudRate;
    uart->huart.Init.WordLength = UART_WORDLENGTH_8B;
    uart->huart.Init.StopBits = UART_STOPBITS_1;
    uart->huart.Init.Parity = UART_PARITY_NONE;
    uart->huart.Init.Mode = UART_MODE_TX_RX;
    uart->huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uart->huart.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&uart->huart);
    /* enable interrupt */
    __HAL_UART_ENABLE_IT(&uart->huart, USART_IT_RXNE);


#endif /* RT_USING_UART1 */

#if defined(RT_USING_UART2)
    uart = &uart2;
    uart->huart.Instance = USART2;
    uart->huart.Init.BaudRate = BaudRate;
    uart->huart.Init.WordLength = UART_WORDLENGTH_8B;
    uart->huart.Init.StopBits = UART_STOPBITS_1;
    uart->huart.Init.Parity = UART_PARITY_NONE;
    uart->huart.Init.Mode = UART_MODE_TX_RX;
    uart->huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uart->huart.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&uart->huart);
    /* enable interrupt */
    __HAL_UART_ENABLE_IT(&uart->huart, USART_IT_RXNE);
    

#endif /* RT_USING_UART2 */

#if defined(RT_USING_UART3)
    uart = &uart3;
    uart->huart.Instance = USART3;
    uart->huart.Init.BaudRate = BaudRate;
    uart->huart.Init.WordLength = UART_WORDLENGTH_8B;
    uart->huart.Init.StopBits = UART_STOPBITS_1;
    uart->huart.Init.Parity = UART_PARITY_NONE;
    uart->huart.Init.Mode = UART_MODE_TX_RX;
    uart->huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uart->huart.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&uart->huart);
    /* enable interrupt */
    __HAL_UART_ENABLE_IT(&uart->huart, USART_IT_RXNE);

#endif /* RT_USING_UART3 */
    MX_TIM2_Init();
    return 0;
}



void HAL_UART_MspInit(UART_HandleTypeDef *uartHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct;
    if (uartHandle->Instance == USART1)
    {
        /* USER CODE BEGIN USART1_MspInit 0 */

        /* USER CODE END USART1_MspInit 0 */
        /* USART1 clock enable */
        __HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**USART1 GPIO Configuration
        PA9     ------> USART1_TX
        PA10     ------> USART1_RX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* USART1 interrupt Init */
        HAL_NVIC_SetPriority(USART1_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
        /* USER CODE BEGIN USART1_MspInit 1 */

        /* USER CODE END USART1_MspInit 1 */
    }
    else if (uartHandle->Instance == USART2)
    {
        /* USER CODE BEGIN USART2_MspInit 0 */

        /* USER CODE END USART2_MspInit 0 */
        /* USART2 clock enable */
        __HAL_RCC_USART2_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**USART2 GPIO Configuration
        PA2     ------> USART2_TX
        PA3     ------> USART2_RX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_2;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* USART2 interrupt Init */
        HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
        /* USER CODE BEGIN USART2_MspInit 1 */

        /* USER CODE END USART2_MspInit 1 */
    }
    else if (uartHandle->Instance == USART3)
    {
        /* USER CODE BEGIN USART3_MspInit 0 */

        /* USER CODE END USART3_MspInit 0 */
        /* USART3 clock enable */
        __HAL_RCC_USART3_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**USART3 GPIO Configuration
        PB10     ------> USART3_TX
        PB11     ------> USART3_RX
        */
        GPIO_InitStruct.Pin = GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_11;
        GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* USART3 interrupt Init */
        HAL_NVIC_SetPriority(USART3_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(USART3_IRQn);
        /* USER CODE BEGIN USART3_MspInit 1 */

        /* USER CODE END USART3_MspInit 1 */
    }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *uartHandle)
{

    if (uartHandle->Instance == USART1)
    {
        /* USER CODE BEGIN USART1_MspDeInit 0 */

        /* USER CODE END USART1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_USART1_CLK_DISABLE();

        /**USART1 GPIO Configuration
        PA9     ------> USART1_TX
        PA10     ------> USART1_RX
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9 | GPIO_PIN_10);

        /* USART1 interrupt Deinit */
        HAL_NVIC_DisableIRQ(USART1_IRQn);
        /* USER CODE BEGIN USART1_MspDeInit 1 */

        /* USER CODE END USART1_MspDeInit 1 */
    }
    else if (uartHandle->Instance == USART2)
    {
        /* USER CODE BEGIN USART2_MspDeInit 0 */

        /* USER CODE END USART2_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_USART2_CLK_DISABLE();

        /**USART2 GPIO Configuration
        PA2     ------> USART2_TX
        PA3     ------> USART2_RX
        */
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2 | GPIO_PIN_3);

        /* USART2 interrupt Deinit */
        HAL_NVIC_DisableIRQ(USART2_IRQn);
        /* USER CODE BEGIN USART2_MspDeInit 1 */

        /* USER CODE END USART2_MspDeInit 1 */
    }
    else if (uartHandle->Instance == USART3)
    {
        /* USER CODE BEGIN USART3_MspDeInit 0 */

        /* USER CODE END USART3_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_USART3_CLK_DISABLE();

        /**USART3 GPIO Configuration
        PB10     ------> USART3_TX
        PB11     ------> USART3_RX
        */
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10 | GPIO_PIN_11);

        /* USART3 interrupt Deinit */
        HAL_NVIC_DisableIRQ(USART3_IRQn);
        /* USER CODE BEGIN USART3_MspDeInit 1 */

        /* USER CODE END USART3_MspDeInit 1 */
    }
}
/**
  * 函数功能: 重定义putc函数，这样可以使用printf函数从串口1打印输出
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
int fputc(int ch_debug, FILE *f)
{
#if defined(RT_USING_UART1)
    return stm32_putc(&uart1, ch_debug);
#endif /* RT_USING_UART1 */
}
/**
  * 函数功能: 重定向c库函数getchar,scanf到DEBUG_USARTx
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明：无
  */
int fgetc(FILE * f)
{
#if defined(RT_USING_UART1)
    return stm32_getc(&uart1);
#endif /* RT_USING_UART1 */
}

void serial_tx(struct stm32_uart *uart, const uint8_t *data, int length)
{
    while (length)
    {
        stm32_putc(uart, *data++);
        length --;
    }
}
/* TIM4 init function */
void MX_TIM2_Init(void)
{
    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 7;//1M
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 5000;//5ms
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    HAL_TIM_Base_Init(&htim2);
}
/**
  * 函数功能: 基本定时器硬件初始化配置
  * 输入参数: htim_base：基本定时器句柄类型指针
  * 返 回 值: 无
  * 说    明: 该函数被HAL库内部调用
  */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{

    if(htim_base->Instance == TIM2)
    {
        __HAL_RCC_TIM2_CLK_ENABLE();
        HAL_NVIC_EnableIRQ(TIM2_IRQn);
        HAL_NVIC_SetPriority(TIM2_IRQn, 4, 0);
    }
    else if(htim_base->Instance == TIM3)
    {
        __HAL_RCC_TIM3_CLK_ENABLE();
        HAL_NVIC_EnableIRQ(TIM3_IRQn);
        HAL_NVIC_SetPriority(TIM3_IRQn, 4, 0);
    }
}

/**
  * 函数功能: 基本定时器硬件反初始化配置
  * 输入参数: htim_base：基本定时器句柄类型指针
  * 返 回 值: 无
  * 说    明: 该函数被HAL库内部调用
  */
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* htim_base)
{
    if(htim_base->Instance == TIM2)
    {
        __HAL_RCC_TIM2_CLK_DISABLE();

        HAL_NVIC_DisableIRQ(TIM2_IRQn);
    }
    else if(htim_base->Instance == TIM3)
    {
        __HAL_RCC_TIM3_CLK_DISABLE();

        HAL_NVIC_DisableIRQ(TIM3_IRQn);
    }
}
void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim2);
}

