/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：config.h
* 描  述：第19章 多功能电子钟 工程配置头文件
* 版本号：v1.0.0
* 备  注：
*******************************************************************************
*/

#ifndef _CONFIG_H
#define _CONFIG_H

/* 通用头文件 */
#include <reg52.h>
#include <intrins.h>

/* 数据类型定义 */
typedef  signed    char    int8;    // 8位有符号整型数
typedef  signed    int     int16;   //16位有符号整型数
typedef  signed    long    int32;   //32位有符号整型数
typedef  unsigned  char    uint8;   // 8位无符号整型数
typedef  unsigned  int     uint16;  //16位无符号整型数
typedef  unsigned  long    uint32;  //32位无符号整型数

/* 全局运行参数定义 */
#define SYS_MCLK   (11059200/12)  //系统主时钟频率，即振荡器频率÷12

/* IO引脚分配定义 */
sbit KEY_IN_1  = P1^5;  //矩阵按键的扫描输入引脚1
sbit KEY_IN_2  = P1^6;  //矩阵按键的扫描输入引脚2
sbit KEY_IN_3  = P1^7;  //矩阵按键的扫描输入引脚3

#define LCD1602_DB  P0   //1602液晶数据端口
sbit LCD1602_RS = P1^1;  //1602液晶指令/数据选择引脚
sbit LCD1602_RW = P1^2;  //1602液晶读写引脚
sbit LCD1602_E  = P1^3;  //1602液晶使能引脚

sbit DS1302_CE = P2^4;  //DS1302片选引脚
sbit DS1302_CK = P2^2;  //DS1302通信时钟引脚
sbit DS1302_IO = P2^3;  //DS1302通信数据引脚



sbit BUZZER = P1^0;  //蜂鸣器控制引脚

sbit IO_18B20 = P2^0;  //DS18B20通信引脚

 sbit LED=P2^5;


#endif
