/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：Time.h
* 描  述：第19章 多功能电子钟 主要功能文件的头文件
* 版本号：v1.0.0
* 备  注：
*******************************************************************************
*/

#ifndef _TIME_H
#define _TIME_H
 
    
#ifndef _TIME_C
 extern bit key; 
  extern bit bakekey;
  extern uint8 m;
#endif
void SwitchSystemSta();
void RefreshTime();
void RefreshDate(uint8 ops);
void RefreshAlarm();
void AlarmMonitor();
void KeyAction(uint8 keycode);

#endif
