/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������19�� �๦�ܵ����� �������ļ�
* �汾�ţ�v1.0.0
* ��  ע���������19��19.4��
*******************************************************************************
*/

#define  _MAIN_C
#include "config.h"
#include "Lcd1602.h"
#include "keyboard.h"
#include "DS1302.h"
#include "DS18B20.h"
#include "Time.h"
#include "main.h"

bit flag2s = 0;    //2s��ʱ��־λ
bit flag200ms = 0; //200ms��ʱ��־
uint8 T0RH = 0;    //T0����ֵ�ĸ��ֽ�
uint8 T0RL = 0;    //T0����ֵ�ĵ��ֽ�
enum eStaSystem staSystem = E_NORMAL;  //ϵͳ����״̬
 uint8 code table[]={0x00,0x00,0x00,0x1F,0x00,0x00,0x00,0x00,

0x00,0x00,0x0E,0x00,0x00,0x1F,0x00,0x00, 

0x00,0x00,0x0E,0x00,0x0E,0x00,0x1F,0x00,

0x00,0x1F,0x15,0x15,0x1B,0x11,0x1F,0x00,

0x00,0x1E,0x08,0x1E,0x0A,0x0A,0x0A,0x1F,

0x00,0x04,0x00,0x1F,0x0A,0x11,0x11,0x00,

0x00,0x1F,0x11,0x11,0x1F,0x11,0x11,0x1F,

0x00,0x04,0x0E,0x0E,0x0E,0x1F,0x00,0x0E };	 

void main()
{
	uint8 ledsj=0;
    EA = 1;           //�����ж�
    ConfigTimer0(1);  //����T0��ʱ1ms
    InitDS1302();     //��ʼ��ʵʱʱ��ģ��
    InitLcd1602();    //��ʼ��Һ��ģ��
	LED=0;
	WriteROM();
    Start18B20();     //�����״��¶�ת��
     LcdShowStr(2, 0,"Zhao Wen jie");
	  LcdShowStr(0, 1,"Qichuangnaozhong");
   while (!flag2s);  //�ϵ����ʱ2��
    flag2s = 0;
   LcdClearScreen();  //Һ������
   RefreshDate(1);   //����ˢ��������ʾ
   RefreshTemp(1);   //����ˢ���¶���ʾ
   RefreshTime();   //ˢ�µ�ǰʱ��

    
    while (1)  //������ѭ��
    {
        KeyDriver();      //ִ�а�������
		
        if (flag200ms)    //ÿ��200msִ�����·�֧
        {
            flag200ms = 0;
		
           
           if (staSystem == E_NORMAL)  //��������ʱˢ��������ʾ
            {
			   	AlarmMonitor();  //�������
			 	if(key==0)
			 		{
					if(bakekey==0)
							{
								LcdClearScreen();  //Һ����
				 				RefreshDate(1);   //����ˢ��������ʾ
     							RefreshTemp(1);   //����ˢ���¶���ʾ
								bakekey=1;
							}
				 		 if(bakekey==1)
				 			{
								RefreshTime();   //ˢ�µ�ǰʱ��
			 					RefreshDate(0);   //����ˢ������
							}
						else
						{
							;;
						}		   
					}
				else
			 		{
					 		LcdClearScreen();  //Һ������
			    	 		RefreshAlarm();
					}   
			
            }
       }
      if (flag2s)  //ÿ��2sִ�����·�֧
        {
			flag2s = 0;
			if (staSystem == E_NORMAL)  //��������ʱˢ��������ʾ
				{
		 			ledsj++;
					if(ledsj>=30)
						{
							ledsj=0;
							LED=1;
						}
            	
             		RefreshTemp(0);
           	  	  }
        } 
    }	 
}
/* �¶�ˢ�º�������ȡ��ǰ�¶Ȳ�������Ҫˢ��Һ����ʾ��
** ops-ˢ��ѡ�Ϊ0ʱֻ���¶ȱ仯��ˢ�£���0������ˢ�� */
void RefreshTemp(uint8 ops)
{
    int16 temp;
    uint8 pdata str[8];
    static int16 backup = 0;
    
    Get18B20Temp(&temp); //��ȡ��ǰ�¶�ֵ16bit
    Start18B20();        //������һ��ת��
    temp >>= 4;          //����4bitС��λ
    if ((backup!=temp) || (ops!=0)) //����Ҫˢ��Һ����ʾ
    {
		if((temp>>7)&0x1f)
			{
			  LcdShowChar(12, 1, '-');
			  temp=(uint8)temp<<5;
			  temp=~(temp)+1;
			}

        str[0] = (temp/10) + '0';  //ʮλתΪASCII��
        str[1] = (temp%10) + '0';  //��λתΪASCII��       
        str[2] = 'C';
        str[3] = '\0';             //�ַ���������
        LcdShowStr(13, 1, str);    //��ʾ��Һ����
        backup = temp;             //ˢ���ϴ��¶�ֵ
    }
}
/* ���ò�����T0��ms-T0��ʱʱ�� */
void ConfigTimer0(uint16 ms)
{
    uint32 tmp;
    
    tmp = (SYS_MCLK*ms)/1000; //��������ļ���ֵ
    tmp = 65536 - tmp;        //���㶨ʱ������ֵ
    tmp = tmp + 33;           //�����ж���Ӧ��ʱ��ɵ����   
    T0RH = (uint8)(tmp>>8);   //��ʱ������ֵ���Ϊ�ߵ��ֽ�
    T0RL = (uint8)tmp;
    TMOD &= 0xF0;   //����T0�Ŀ���λ
    TMOD |= 0x01;   //����T0Ϊģʽ1
    TH0 = T0RH;     //����T0����ֵ
    TL0 = T0RL;
    ET0 = 1;        //ʹ��T0�ж�
    TR0 = 1;        //����T0
}
/* T0�жϷ�������ʵ��ϵͳ��ʱ�Ͱ���ɨ�� */
void InterruptTimer0() interrupt 1
{
    static uint8 tmr2s = 0;
    static uint8 tmr200ms = 0;
    
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
    tmr200ms++;  //��ʱ200ms
    if (tmr200ms >= 200)
    {
        tmr200ms = 0;
        flag200ms = 1;
        tmr2s++;  //��ʱ2s
        if (tmr2s >= 10)
        {
            tmr2s = 0;
            flag2s = 1;
        }
    }
    KeyScan();   //ִ�а���ɨ��
}
  void WriteROM()
  {
  uint8 i=0;
   LcdWriteCmd(0x40);
   for(i=0;i<64;i++)
   {
	LcdWriteDat(table[i]);
   }
   } 