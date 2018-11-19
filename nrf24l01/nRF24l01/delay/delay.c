#include "delay.h"
static uint32_t fac_us = 0;							//us延时倍乘数


void delay_init(uint8_t SYSCLK)
{
    /* 系统滴答定时器时钟源 */
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    fac_us = SYSCLK;						//不论是否使用OS,fac_us都需要使用
}

void delay_us(uint32_t nus)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD;				//LOAD的值
    ticks = nus * fac_us; 						//需要的节拍数
    told = SysTick->VAL;        				//刚进入时的计数器值
    while(1)
    {
        tnow = SysTick->VAL;
        if(tnow != told)
        {
            if(tnow < told)tcnt += told - tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
            else tcnt += reload - tnow + told;
            told = tnow;
            if(tcnt >= ticks)break;			//时间超过/等于要延迟的时间,则退出.
        }
    };
}

//延时nms
//nms:要延时的ms数
void delay_ms(uint32_t nms)
{
    uint32_t i;
    for(i = 0; i < nms; i++) delay_us(1000);
}

//		char Time2[15] = "";
//    while (1) {
//        wdogFeed();//喂狗
//			__HAL_TIM_SET_COUNTER(&htim2, 0); // 清零定时器计数
//			delay_ms(10);
//			sprintf(Time2, "%d",__HAL_TIM_GET_COUNTER(&htim2));
//      LCD12864_Display_Words(0, 0,(unsigned char*)Time2);
//			delay_ms(1000);
//    }


































