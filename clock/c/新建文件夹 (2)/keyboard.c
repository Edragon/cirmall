/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：keyboard.c
* 描  述：4*4矩阵按键驱动模块
* 版本号：v1.0.0
* 备  注：
*******************************************************************************
*/

#define  _KEY_BOARD_C
#include "config.h"
#include "keyboard.h"
#include "Time.h"
 uint8 code KeyCodeMap[3] =  {0x25,  0x26,0x28   };  //数字键1、数字键2、数字键3、向上键
 static uint8 KeySta[3] =  {1, 1, 1};  //全部矩阵按键的当前状态
  uint16 pdata KeyDownTime[] ={ 0,0,0}; //每个按键按下的持续时间，单位ms

        
/* 按键驱动函数，检测按键动作，调度相应动作函数，需在主循环中调用 */
void KeyDriver()
{
    uint8 i;
    static uint8  backup[3] = {1, 1, 1};
     
    for (i=0; i<3; i++)  //循环检测按键
    {
       
            if (backup[i] != KeySta[i] )   //检测按键动作
            {
				LED=0;
                if (backup[i] != 0)           //按键按下时执行动作
                {
                    KeyAction(KeyCodeMap[i]); //调用按键动作函数
                }
                backup[i] = KeySta[i];     //刷新前一次的备份值

            }
	
					if(	KeyDownTime[0]>=1000)
					{
                    	m=0 ; //调用按键动作函数
					 	SwitchSystemSta();
						 KeyDownTime[0]=0;  
            	 	}
    }
}
/* 按键扫描函数，需在定时中断中调用，推荐调用间隔1ms */
void KeyScan()
{
    uint8 i;
    static uint8 keybuf[3] = {0xFF, 0xFF,  0xFF}; //矩阵按键扫描缓冲区
        

    //将一行的4个按键值移入缓冲区
    keybuf[0] = (keybuf[0] << 1) | KEY_IN_1;
    keybuf[1] = (keybuf[1] << 1) | KEY_IN_2;
    keybuf[2] = (keybuf[2] << 1) | KEY_IN_3;
    
    //消抖后更新按键状态
    for (i=0; i<3; i++)  //每行4个按键，所以循环4次
    {
        if ((keybuf[i] & 0x0F) == 0x00)
        {   //连续4次扫描值为0，即4*4ms内都是按下状态时，可认为按键已稳定的按下
            KeySta[i] = 0;
		   	 KeyDownTime[i]	+=1;
        }
        else if ((keybuf[i] & 0x0F) == 0x0F)
        {   //连续4次扫描值为1，即4*4ms内都是弹起状态时，可认为按键已稳定的弹起
            KeySta[i] = 1;
		    KeyDownTime[i]	=0;
        }
    }
    //执行下一次的扫描输出
 
   
  
}
