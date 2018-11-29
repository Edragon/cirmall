/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.h
* 描  述：第19章 多功能电子钟 工程主文件的头文件
* 版本号：v1.0.0
* 备  注：
*******************************************************************************
*/

#ifndef _MAIN_H
#define _MAIN_H

enum eStaSystem {  //系统运行状态枚举
    E_NORMAL,E_SET_Day, E_SET_TIME, E_SET_ALARM
};

#ifndef _MAIN_C

extern enum eStaSystem staSystem;
#endif
  void WriteROM();
void RefreshTemp(uint8 ops);
void ConfigTimer0(uint16 ms);

#endif
