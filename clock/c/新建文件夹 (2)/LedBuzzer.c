/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：LedBuzzer.c
* 描  述：点阵LED、数码管、独立LED和无源蜂鸣器的驱动模块
* 版本号：v1.0.0
* 备  注：
*******************************************************************************
*/

#define  _LED_BUZZER_C
#include "config.h"
#include "LedBuzzer.h"
bit staBuzzer = 0; //蜂鸣器状态控制位，1-鸣叫、0-关闭


/* LED初始化函数，初始化IO、配置定时器 */
void InitLed()
{
    //初始化IO口
    P0 = 0xFF;
 
    //配置T2作为动态扫描定时
    T2CON = 0x00;  //配置T2工作在16位自动重载定时器模式
    RCAP2H = ((65536-SYS_MCLK/1500)>>8);  //配置重载值，每秒产生1500次中断，
    RCAP2L = (65536-SYS_MCLK/1500);       //以使刷新率达到100Hz无闪烁的效果
    TH2 = RCAP2H;  //设置初值等于重载值
    TL2 = RCAP2L;
    ET2 = 1;       //使能T2中断
    PT2 = 1;       //设置T2中断为高优先级
    TR2 = 1;       //启动T2
}


/* T2中断服务函数，LED动态扫描、蜂鸣器控制 */
void InterruptTimer2() interrupt 5
{
    
    TF2 = 0;  //清零T2中断标志
  
    //由蜂鸣器状态位控制蜂鸣器
    if (staBuzzer == 1)
        BUZZER = ~BUZZER;  //蜂鸣器鸣叫
    else
        BUZZER = 1;        //蜂鸣器静音
}
