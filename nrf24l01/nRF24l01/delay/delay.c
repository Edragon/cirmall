#include "delay.h"
static uint32_t fac_us = 0;							//us��ʱ������


void delay_init(uint8_t SYSCLK)
{
    /* ϵͳ�δ�ʱ��ʱ��Դ */
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    fac_us = SYSCLK;						//�����Ƿ�ʹ��OS,fac_us����Ҫʹ��
}

void delay_us(uint32_t nus)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD;				//LOAD��ֵ
    ticks = nus * fac_us; 						//��Ҫ�Ľ�����
    told = SysTick->VAL;        				//�ս���ʱ�ļ�����ֵ
    while(1)
    {
        tnow = SysTick->VAL;
        if(tnow != told)
        {
            if(tnow < told)tcnt += told - tnow;	//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
            else tcnt += reload - tnow + told;
            told = tnow;
            if(tcnt >= ticks)break;			//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
        }
    };
}

//��ʱnms
//nms:Ҫ��ʱ��ms��
void delay_ms(uint32_t nms)
{
    uint32_t i;
    for(i = 0; i < nms; i++) delay_us(1000);
}

//		char Time2[15] = "";
//    while (1) {
//        wdogFeed();//ι��
//			__HAL_TIM_SET_COUNTER(&htim2, 0); // ���㶨ʱ������
//			delay_ms(10);
//			sprintf(Time2, "%d",__HAL_TIM_GET_COUNTER(&htim2));
//      LCD12864_Display_Words(0, 0,(unsigned char*)Time2);
//			delay_ms(1000);
//    }


































