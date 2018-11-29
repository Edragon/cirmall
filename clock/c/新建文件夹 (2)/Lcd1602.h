/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：Lcd1602.h
* 描  述：1602字符液晶驱动模块的头文件
* 版本号：v1.0.0
* 备  注：
*******************************************************************************
*/

#ifndef _LCD1602_H
#define _LCD1602_H


#ifndef _LCD1602_C

#endif
void LcdWriteCmd(uint8 cmd);
void LcdWriteDat(uint8 dat);
void InitLcd1602();
void LcdClearScreen();
void LcdOpenCursor();
void LcdCloseCursor();
void LcdSetCursor(uint8 x, uint8 y);
void LcdShowStr(uint8 x, uint8 y, uint8 *str);
void LcdShowChar(uint8 x, uint8 y, uint8 chr);

#endif
