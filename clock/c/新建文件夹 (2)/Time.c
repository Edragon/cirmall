/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：Time.c
* 描  述：第19章 多功能电子钟 主要功能文件
* 版本号：v1.0.0
* 备  注：详情见第19章19.4节
*******************************************************************************
*/

#define  _TIME_C
#include "config.h"
#include "DS1302.h"
#include "Lcd1602.h"
#include "Time.h"
#include "main.h"
#define  ascii	 (num+'0')
bit	 key=0;
 bit bakekey=1;
bit staMute = 0;  //静音标志位
uint8 AlarmHour = 0x07;  //闹钟时间的小时数
uint8 AlarmMin  = 0x30;  //闹钟时间的分钟数
struct sTime CurTime;    //当前日期时间			
uint8 num=0;
uint8 m=5;
uint8 num1=0;
uint8 SetIndex=0;
uint8 pdata SetAlarmHour;    //闹钟小时数设置缓冲
uint8 pdata SetAlarmMin;     //闹钟分钟数设置缓冲
struct sTime pdata SetTime;
 void delay(uint8 ms)
 {
 	uint8 i,m,k;
	for(i=ms;i>0;i--)
		for(k=25;k>0;k--)
		for(m=14;m>0;m--);
 }

/* 获取当前日期时间，并刷新时间和星期的显示 */
void RefreshTime()
{
   
    uint8 pdata str[9];
    GetRealTime(&CurTime);
    str[0] = (CurTime.hour>>4) + '0';   //小时
    str[1] = (CurTime.hour&0xF) + '0';
    str[2] = ':';
    str[3] = (CurTime.min>>4) + '0';    //分钟
    str[4] = (CurTime.min&0xF) + '0';
	str[5] = ':';
	str[6] = (CurTime.sec>>4) + '0';   //小时
    str[7] = (CurTime.sec&0xF) + '0';
    str[8] = '\0';
    LcdShowStr(1, 1, str);          
                      
}

/* 日期刷新函数，ops-刷新选项：为0时只当日期变化才刷新，非0则立即刷新 */
void RefreshDate(uint8 ops)
{
    uint8 pdata str[11];
  
   static uint8 backup=0;
    if ((backup!=CurTime.day) || (ops!=0))
    {
        str[0] = ((CurTime.year>>12) & 0xF) + '0';  //4位数年份
        str[1] = ((CurTime.year>>8) & 0xF) + '0';
        str[2] = ((CurTime.year>>4) & 0xF) + '0';
        str[3] = (CurTime.year & 0xF) + '0';
        str[4] = '-';                        //分隔符
        str[5] = (CurTime.mon >> 4) + '0';   //月份
        str[6] = (CurTime.mon & 0xF) + '0';
        str[7] = '-';                        //分隔符
        str[8] = (CurTime.day >> 4) + '0';   //日期
        str[9] = (CurTime.day & 0xF) + '0';
        str[10] = '\0';         //字符串结束符
        LcdShowStr(0, 0, str);  //显示到液晶上
        backup = CurTime.day;   //刷新上次日期值
		num1 = CurTime.week;
     
		LcdShowStr(10, 0, "week");  //显示提示标题
      LcdShowChar(15, 0, num1);            //显示设置时间值
	  
    }
}
/* 刷新闹钟时间的显示 */
void RefreshAlarm()
{
    uint8 pdata str[6];
    
    LcdShowStr(6, 0, "Alarm ");     //显示提示标题
    str[0] = (AlarmHour >> 4) + '0';   //闹钟小时数
    str[1] = (AlarmHour & 0xF) + '0';
    str[2] = ':';                      //分隔符
    str[3] = (AlarmMin >> 4) + '0';    //闹钟分钟数
    str[4] = (AlarmMin & 0xF) + '0';
    str[5] = '\0';                     //字符串结束符
    LcdShowStr(6, 1, str);             //显示到液晶上
}
/* 闹钟监控函数，抵达设定的闹钟时间时执行闹铃 */
void AlarmMonitor()
{
	if(	staMute==0)
	{
		  LcdShowChar(11, 1, 7);
		  
	}
	else  
	  LcdShowChar(11, 1,0x20);
	  
    if ((CurTime.hour==AlarmHour) && (CurTime.min==AlarmMin)) //检查时间匹配
    {

   			 if (!staMute)  //检查是否静音
           		 BUZZER = ~BUZZER;  //实现蜂鸣器断续鸣叫
     		else
          		 BUZZER = 1;
    }
    else
    {
     
        BUZZER = 1;
    }

}
/* 将设置日期及标题提示显示到液晶上 */
void ShowSetDay()
{
    uint8 pdata str[10];
      
    str[0]  = ((SetTime.year>>4) & 0xF) + '0';  //2位数年份
    str[1]  = (SetTime.year & 0xF) + '0';
    str[2]  = '-';
    str[3]  = (SetTime.mon >> 4) + '0';   //月份
    str[4]  = (SetTime.mon & 0xF) + '0';
    str[5]  = '-';
    str[6]  = (SetTime.day >> 4) + '0';   //日期
    str[7]  = (SetTime.day & 0xF) + '0';
    str[8]  = '-';
   
   str[9] = '\0';
    LcdShowStr(4, 0, "Set  day");  //显示提示标题
    LcdShowStr(1, 1, str);              //显示设置时间值
	 num1 = SetTime.week & 0x07 ; //星期
	  LcdShowChar(10, 1,num1);
	
	  }
 /* 将设置时间及标题提示显示到液晶上 */
void ShowSetTime()
{
    uint8 pdata str[6];
     
    str[0] = (SetTime.hour >> 4) + '0';  //小时
    str[1] = (SetTime.hour & 0xF) + '0';
    str[2] = ':';
    str[3] = (SetTime.min >> 4) + '0';   //分钟
    str[4] = (SetTime.min & 0xF) + '0';
    str[5] = '\0';
    LcdShowStr(4, 0, "Set  Time");  //显示提示标题
    LcdShowStr(1, 1, str);              //显示设置时间值
}
/* 将设置闹钟及标题提示显示到液晶上 */
void ShowSetAlarm()
{
    uint8 pdata str[6];
      
    str[0] = (SetAlarmHour >> 4) + '0';   //小时
    str[1] = (SetAlarmHour & 0xF) + '0';
    str[2] = ':';
    str[3] = (SetAlarmMin >> 4) + '0';    //分钟
    str[4] = (SetAlarmMin & 0xF) + '0';
    str[5] = '\0';
    LcdShowStr(3, 0, "Set Alarm");  //显示提示标题
    LcdShowStr(1, 1, str);          //显示设定闹钟值
}

 /*时间或闹钟设置时，设置位右移一位，到头后折回 */
void SetRightShift( )  
{
	
    if (staSystem == E_SET_Day)
    {
	  	  if(	SetIndex==7)
			SetIndex=0;
        switch (SetIndex)
        {
            case 0:  LcdSetCursor(1, 1);     break;  
            case 1:    LcdSetCursor(2, 1);   break;
            case 2:  	 LcdSetCursor(4, 1);  break;  
            case 3:  LcdSetCursor(5, 1);  	 break;
            case 4:  	 LcdSetCursor(7, 1); break;
			case 5:  LcdSetCursor(8, 1); 	 break;
  			default:	LcdSetCursor(10, 1);		break;
	     
        }
		
    }
    else if (staSystem == E_SET_TIME)
    {
			if(	SetIndex==4)
			SetIndex=0;
        switch (SetIndex)
        {
            case 0:  LcdSetCursor(1,1);	 break;
            case 1:  LcdSetCursor(2,1); 	break;
            case 2:    LcdSetCursor(4,1);	break;
            default:  LcdSetCursor(5,1); break;
        }
	
    }
    else if (staSystem == E_SET_ALARM)
    {
		if(	SetIndex==4)
			SetIndex=0;
        switch (SetIndex)
        {
            case 0:    LcdSetCursor(1,1);	break;
            case 1:   LcdSetCursor(2,1);	 break;
            case 2:   LcdSetCursor(4,1); 	break;
             default:   LcdSetCursor(5,1); break;
        }
		
    }
}
void InputSetNumber()
{
    

   if (staSystem == E_SET_Day)
        {
            switch (SetIndex)
            {
                case 0:	 num= (SetTime.year&0x00f0)>>4;
						 num++;
						 if(num>9)
						 	num=0;
						 SetTime.year = (SetTime.year&0xFF0F)|(num<<4);
						 
                        LcdShowChar(1, 1, ascii);LcdSetCursor(1,1);  break;      //年份高位数字

                case 1: num= SetTime.year&0x000F;
						num++;
						if(num>9)
							num=0;
						SetTime.year = (SetTime.year&0xFFF0)|(num);
					   
                        LcdShowChar(2, 1, ascii);LcdSetCursor(2,1); break;      //年份低位数字

                case 2:	num= (SetTime.mon&0xf0)>>4;
						num++;
				 		if(num>=2)
							{
								num=0;
							}
						SetTime.mon = (SetTime.mon&0x0F)|(num<<4);
						
                        LcdShowChar(4, 1, ascii);  LcdSetCursor(4,1); break;      //月份高位数字

                case 3: num= SetTime.mon&0x0f;
						num++;
						if(	((SetTime.mon&0xF0)>>4)==1)
						 {
							if(num>2)
							{
								num=0;
							}
						  }
						  else if(num>9)
						  	num=0;
						SetTime.mon = (SetTime.mon&0xF0)|(num);
						   
                        LcdShowChar(5, 1, ascii); LcdSetCursor(5,1); break;      //月份低位数字

                case 4: num=(SetTime.day&0xf0)>>4;
						num++;
						if(num>3)
						{
							num=0;
						}
						SetTime.day = (SetTime.day&0x0F)|(num<<4);
						 
                        LcdShowChar(7, 1, ascii);LcdSetCursor(7,1); break;      //日期高位数字
                case 5:	num= SetTime.day&0x0f; 
						num++;
				       if(((SetTime.day&0xF0)>>4)==3)
							{
							num&=0x01;
							}
						else if(num>9)
							num=0;
						SetTime.day = (SetTime.day&0xF0)|(num);
						
                        LcdShowChar(8, 1, ascii); LcdSetCursor(8,1); break;      //日期低位数字
                default:  num1 =SetTime.week&0x0f;
						  num1++;
							if(num1>6)
							{
								num1=0;
							}
							SetTime.week = (SetTime.week&0xF0)|(num1);
						 
                        LcdShowChar(10, 1, num1);
				   LcdSetCursor(10,1);	 break;      //星期数字
               		
            }
		
           }
   
 else if (staSystem == E_SET_TIME)
		 {
		    switch (SetIndex)
			{
	 case 0: num=	(SetTime.hour&0xf0)>>4;
			   num++;
			   if(num<3)
			  	 {
			  		 num&=0x03;
			   	}
			   else
			   		 num=0;
			 SetTime.hour = (SetTime.hour&0x0f)|(num<<4);
             LcdShowChar(1,1, ascii);LcdSetCursor(1,1); break;      //小时高位数字
     case 1: 		num= SetTime.hour&0x0f;
					   num++;
					  if  (((SetTime.hour&0xF0)>>4)==2)
					  {
					 		 if(num<4)
					  			{
					  				num&=0x07;
					 			 }
							  else num=0;
					  }
					  else if(num>9)
					     num=0;
				SetTime.hour = (SetTime.hour&0xF0)|(num);
                        LcdShowChar(2,1, ascii);LcdSetCursor(2,1); break;      //小时低位数字

      case 2:  	 num=  (SetTime.min&0xf0)>>4;
	  				num++;
	  				if(num<=5)
						  num&=0x07;
					else 
							num=0;
				SetTime.min = (SetTime.min&0x0F)|(num<<4);
                        LcdShowChar(4,1, ascii); LcdSetCursor(4,1);  break;      //分钟高位数字

      default:	 num=  SetTime.min&0x0f;
				 num++;
				if(num>9)
					num=0;		  
			   SetTime.min = (SetTime.min&0xF0)|(num);
               LcdShowChar(5,1, ascii); LcdSetCursor(5,1);num&=0x0f ; break;      //分钟低位数字
			  }
		}
  else if (staSystem == E_SET_ALARM)
        {
            switch (SetIndex)
            {
          case 0: 	num = (SetAlarmHour & 0xf0)>>4;
			  
				 num++;
				  if(num<3)
			  	 {
			  		 num&=0x03;						 
			  	 }
			   else
			   {
			    num=0;
				}
			    
				LcdShowChar(1,1, ascii);       //小时高位数字
				  LcdSetCursor(1,1);
				SetAlarmHour = (SetAlarmHour&0x0F) | (num<<4);
                break;  
          case 1:	num= SetAlarmHour&0x0f;
		          
					  	num++;
					   if  (((SetAlarmHour&0xF0)>>4)==2)
					  {
					 		 if(num<4)
					  			{
					  				num&=0x07;
					 			 }
							  else num=0;
					  }
					else if(num>9)
					num=0;
					
					LcdShowChar(2,1, ascii);
					LcdSetCursor(2,1); 
				 SetAlarmHour = (SetAlarmHour&0xF0)| num;
                          break;      //小时低位数字
         case 2:  num =	( (SetAlarmMin&0xf0)>>4);
				 	num++;
		 	 if(num<=5)
						  num&=0x07;
					else num=0;
					 
					LcdShowChar(4,1, ascii);
						 LcdSetCursor(4,1);
							SetAlarmMin = (SetAlarmMin&0x0F) |(num<<4);
                       		   break;      //分钟高位数字
         default:  	num = (SetAlarmMin&0x0f);
					 num++;
					if(num>9)
					num=0;
					
					  LcdShowChar(5,1, ascii);
					    LcdSetCursor(5,1);
				SetAlarmMin = (SetAlarmMin&0xF0) | num;
                          break;      //分钟低位数字
            }
          
        }

    }	   
/* 切换系统运行状态 */
void SwitchSystemSta()
{

    if (staSystem == E_NORMAL)  //正常运行切换到时间设置
    {	
		
		if(m==0)
		{
           staSystem = E_SET_Day;
        	SetTime.year = CurTime.year;  //当前时间拷贝到时间设置缓冲区中
        	SetTime.mon  = CurTime.mon;
        	SetTime.day  = CurTime.day;
        	SetTime.week = CurTime.week;
        	LcdClearScreen();  //液晶清屏
        	ShowSetDay();     //显示设置时间	
        	SetIndex = 0;    //与接下来的右移一起将光标设在最左边的位置上
		SetRightShift();
        	 LcdOpenCursor();   //开启光标  
			 delay(1);
    	}
	}
  else  if((m==1)&&(staSystem == E_SET_Day))
		{
			 staSystem = E_SET_TIME;
        	SetTime.hour = CurTime.hour;
        	SetTime.min  = CurTime.min;
        	SetTime.sec  = CurTime.sec;
       
        	LcdClearScreen();  //液晶清屏
        	ShowSetTime();     //显示设置时间
		
        	SetIndex = 0;    //与接下来的右移一起将光标设在最左边的位置上
  	SetRightShift();
        	 LcdOpenCursor();   //开启光标 
			  delay(1);
    	}
  else if ((m==2)&&(staSystem == E_SET_TIME))  //时间设置切换到闹钟设置
    {
         staSystem = E_SET_ALARM;
        SetTime.sec = 0;          //秒清零，即当设置时间后从0秒开始走时
        SetRealTime(&SetTime);    //设定时间写入实时时钟
        SetAlarmHour = AlarmHour; //当前闹钟值拷贝到设置缓冲区
        SetAlarmMin  = AlarmMin;
        LcdClearScreen();  //液晶清屏
		 ShowSetAlarm();    //显示设置闹钟		 
		 SetIndex = 0;    //与接下来的右移一起将光标设在最左边的位置上
      	SetRightShift();
         LcdOpenCursor();   //开启光标 
		   delay(1);
    }  	
  else if ((m==3)&&(staSystem ==E_SET_ALARM)) //时间设置切换到闹钟设置
     {
	 	staSystem = E_NORMAL;
	    AlarmHour = SetAlarmHour;  //设定的闹钟值写入闹钟时间
        AlarmMin  = SetAlarmMin;
   	 LcdCloseCursor();  //关闭光标 
	   delay(1);
    LcdClearScreen();  //液晶清屏
    RefreshTime();   //刷新当前时间
    RefreshDate(1);  //立即刷新日期显示
    RefreshTemp(1);  //立即刷新温度显示
    }

}

/* 按键动作函数，根据键码执行相应的操作，keycode-按键键码 */
void KeyAction(uint8 keycode)
{
 
    if (keycode ==0x25)  //向左键，向左切换设置位
    {
	
	 
	   	m++;
		  SwitchSystemSta();
       	
    }
    else if (keycode == 0x26)  //，向右切换设置位
    {
		 if (staSystem == E_NORMAL)  //正常运行切换到时间设置
	 {
 	   key= !key;
	    bakekey=key;
	  }
			SetIndex ++;
        SetRightShift();
	
    }
    else if (keycode == 0x28)  //调整数值
    {
		  if (staSystem == E_NORMAL)  //正常运行切换到时间设置
		  {
		  	staMute=!staMute;	
		  }
		 InputSetNumber();	
		
    }
   
 }	
