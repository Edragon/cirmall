/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����Time.c
* ��  ������19�� �๦�ܵ����� ��Ҫ�����ļ�
* �汾�ţ�v1.0.0
* ��  ע���������19��19.4��
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
bit staMute = 0;  //������־λ
uint8 AlarmHour = 0x07;  //����ʱ���Сʱ��
uint8 AlarmMin  = 0x30;  //����ʱ��ķ�����
struct sTime CurTime;    //��ǰ����ʱ��			
uint8 num=0;
uint8 m=5;
uint8 num1=0;
uint8 SetIndex=0;
uint8 pdata SetAlarmHour;    //����Сʱ�����û���
uint8 pdata SetAlarmMin;     //���ӷ��������û���
struct sTime pdata SetTime;
 void delay(uint8 ms)
 {
 	uint8 i,m,k;
	for(i=ms;i>0;i--)
		for(k=25;k>0;k--)
		for(m=14;m>0;m--);
 }

/* ��ȡ��ǰ����ʱ�䣬��ˢ��ʱ������ڵ���ʾ */
void RefreshTime()
{
   
    uint8 pdata str[9];
    GetRealTime(&CurTime);
    str[0] = (CurTime.hour>>4) + '0';   //Сʱ
    str[1] = (CurTime.hour&0xF) + '0';
    str[2] = ':';
    str[3] = (CurTime.min>>4) + '0';    //����
    str[4] = (CurTime.min&0xF) + '0';
	str[5] = ':';
	str[6] = (CurTime.sec>>4) + '0';   //Сʱ
    str[7] = (CurTime.sec&0xF) + '0';
    str[8] = '\0';
    LcdShowStr(1, 1, str);          
                      
}

/* ����ˢ�º�����ops-ˢ��ѡ�Ϊ0ʱֻ�����ڱ仯��ˢ�£���0������ˢ�� */
void RefreshDate(uint8 ops)
{
    uint8 pdata str[11];
  
   static uint8 backup=0;
    if ((backup!=CurTime.day) || (ops!=0))
    {
        str[0] = ((CurTime.year>>12) & 0xF) + '0';  //4λ�����
        str[1] = ((CurTime.year>>8) & 0xF) + '0';
        str[2] = ((CurTime.year>>4) & 0xF) + '0';
        str[3] = (CurTime.year & 0xF) + '0';
        str[4] = '-';                        //�ָ���
        str[5] = (CurTime.mon >> 4) + '0';   //�·�
        str[6] = (CurTime.mon & 0xF) + '0';
        str[7] = '-';                        //�ָ���
        str[8] = (CurTime.day >> 4) + '0';   //����
        str[9] = (CurTime.day & 0xF) + '0';
        str[10] = '\0';         //�ַ���������
        LcdShowStr(0, 0, str);  //��ʾ��Һ����
        backup = CurTime.day;   //ˢ���ϴ�����ֵ
		num1 = CurTime.week;
     
		LcdShowStr(10, 0, "week");  //��ʾ��ʾ����
      LcdShowChar(15, 0, num1);            //��ʾ����ʱ��ֵ
	  
    }
}
/* ˢ������ʱ�����ʾ */
void RefreshAlarm()
{
    uint8 pdata str[6];
    
    LcdShowStr(6, 0, "Alarm ");     //��ʾ��ʾ����
    str[0] = (AlarmHour >> 4) + '0';   //����Сʱ��
    str[1] = (AlarmHour & 0xF) + '0';
    str[2] = ':';                      //�ָ���
    str[3] = (AlarmMin >> 4) + '0';    //���ӷ�����
    str[4] = (AlarmMin & 0xF) + '0';
    str[5] = '\0';                     //�ַ���������
    LcdShowStr(6, 1, str);             //��ʾ��Һ����
}
/* ���Ӽ�غ������ִ��趨������ʱ��ʱִ������ */
void AlarmMonitor()
{
	if(	staMute==0)
	{
		  LcdShowChar(11, 1, 7);
		  
	}
	else  
	  LcdShowChar(11, 1,0x20);
	  
    if ((CurTime.hour==AlarmHour) && (CurTime.min==AlarmMin)) //���ʱ��ƥ��
    {

   			 if (!staMute)  //����Ƿ���
           		 BUZZER = ~BUZZER;  //ʵ�ַ�������������
     		else
          		 BUZZER = 1;
    }
    else
    {
     
        BUZZER = 1;
    }

}
/* ���������ڼ�������ʾ��ʾ��Һ���� */
void ShowSetDay()
{
    uint8 pdata str[10];
      
    str[0]  = ((SetTime.year>>4) & 0xF) + '0';  //2λ�����
    str[1]  = (SetTime.year & 0xF) + '0';
    str[2]  = '-';
    str[3]  = (SetTime.mon >> 4) + '0';   //�·�
    str[4]  = (SetTime.mon & 0xF) + '0';
    str[5]  = '-';
    str[6]  = (SetTime.day >> 4) + '0';   //����
    str[7]  = (SetTime.day & 0xF) + '0';
    str[8]  = '-';
   
   str[9] = '\0';
    LcdShowStr(4, 0, "Set  day");  //��ʾ��ʾ����
    LcdShowStr(1, 1, str);              //��ʾ����ʱ��ֵ
	 num1 = SetTime.week & 0x07 ; //����
	  LcdShowChar(10, 1,num1);
	
	  }
 /* ������ʱ�估������ʾ��ʾ��Һ���� */
void ShowSetTime()
{
    uint8 pdata str[6];
     
    str[0] = (SetTime.hour >> 4) + '0';  //Сʱ
    str[1] = (SetTime.hour & 0xF) + '0';
    str[2] = ':';
    str[3] = (SetTime.min >> 4) + '0';   //����
    str[4] = (SetTime.min & 0xF) + '0';
    str[5] = '\0';
    LcdShowStr(4, 0, "Set  Time");  //��ʾ��ʾ����
    LcdShowStr(1, 1, str);              //��ʾ����ʱ��ֵ
}
/* ���������Ӽ�������ʾ��ʾ��Һ���� */
void ShowSetAlarm()
{
    uint8 pdata str[6];
      
    str[0] = (SetAlarmHour >> 4) + '0';   //Сʱ
    str[1] = (SetAlarmHour & 0xF) + '0';
    str[2] = ':';
    str[3] = (SetAlarmMin >> 4) + '0';    //����
    str[4] = (SetAlarmMin & 0xF) + '0';
    str[5] = '\0';
    LcdShowStr(3, 0, "Set Alarm");  //��ʾ��ʾ����
    LcdShowStr(1, 1, str);          //��ʾ�趨����ֵ
}

 /*ʱ�����������ʱ������λ����һλ����ͷ���ۻ� */
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
						 
                        LcdShowChar(1, 1, ascii);LcdSetCursor(1,1);  break;      //��ݸ�λ����

                case 1: num= SetTime.year&0x000F;
						num++;
						if(num>9)
							num=0;
						SetTime.year = (SetTime.year&0xFFF0)|(num);
					   
                        LcdShowChar(2, 1, ascii);LcdSetCursor(2,1); break;      //��ݵ�λ����

                case 2:	num= (SetTime.mon&0xf0)>>4;
						num++;
				 		if(num>=2)
							{
								num=0;
							}
						SetTime.mon = (SetTime.mon&0x0F)|(num<<4);
						
                        LcdShowChar(4, 1, ascii);  LcdSetCursor(4,1); break;      //�·ݸ�λ����

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
						   
                        LcdShowChar(5, 1, ascii); LcdSetCursor(5,1); break;      //�·ݵ�λ����

                case 4: num=(SetTime.day&0xf0)>>4;
						num++;
						if(num>3)
						{
							num=0;
						}
						SetTime.day = (SetTime.day&0x0F)|(num<<4);
						 
                        LcdShowChar(7, 1, ascii);LcdSetCursor(7,1); break;      //���ڸ�λ����
                case 5:	num= SetTime.day&0x0f; 
						num++;
				       if(((SetTime.day&0xF0)>>4)==3)
							{
							num&=0x01;
							}
						else if(num>9)
							num=0;
						SetTime.day = (SetTime.day&0xF0)|(num);
						
                        LcdShowChar(8, 1, ascii); LcdSetCursor(8,1); break;      //���ڵ�λ����
                default:  num1 =SetTime.week&0x0f;
						  num1++;
							if(num1>6)
							{
								num1=0;
							}
							SetTime.week = (SetTime.week&0xF0)|(num1);
						 
                        LcdShowChar(10, 1, num1);
				   LcdSetCursor(10,1);	 break;      //��������
               		
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
             LcdShowChar(1,1, ascii);LcdSetCursor(1,1); break;      //Сʱ��λ����
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
                        LcdShowChar(2,1, ascii);LcdSetCursor(2,1); break;      //Сʱ��λ����

      case 2:  	 num=  (SetTime.min&0xf0)>>4;
	  				num++;
	  				if(num<=5)
						  num&=0x07;
					else 
							num=0;
				SetTime.min = (SetTime.min&0x0F)|(num<<4);
                        LcdShowChar(4,1, ascii); LcdSetCursor(4,1);  break;      //���Ӹ�λ����

      default:	 num=  SetTime.min&0x0f;
				 num++;
				if(num>9)
					num=0;		  
			   SetTime.min = (SetTime.min&0xF0)|(num);
               LcdShowChar(5,1, ascii); LcdSetCursor(5,1);num&=0x0f ; break;      //���ӵ�λ����
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
			    
				LcdShowChar(1,1, ascii);       //Сʱ��λ����
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
                          break;      //Сʱ��λ����
         case 2:  num =	( (SetAlarmMin&0xf0)>>4);
				 	num++;
		 	 if(num<=5)
						  num&=0x07;
					else num=0;
					 
					LcdShowChar(4,1, ascii);
						 LcdSetCursor(4,1);
							SetAlarmMin = (SetAlarmMin&0x0F) |(num<<4);
                       		   break;      //���Ӹ�λ����
         default:  	num = (SetAlarmMin&0x0f);
					 num++;
					if(num>9)
					num=0;
					
					  LcdShowChar(5,1, ascii);
					    LcdSetCursor(5,1);
				SetAlarmMin = (SetAlarmMin&0xF0) | num;
                          break;      //���ӵ�λ����
            }
          
        }

    }	   
/* �л�ϵͳ����״̬ */
void SwitchSystemSta()
{

    if (staSystem == E_NORMAL)  //���������л���ʱ������
    {	
		
		if(m==0)
		{
           staSystem = E_SET_Day;
        	SetTime.year = CurTime.year;  //��ǰʱ�俽����ʱ�����û�������
        	SetTime.mon  = CurTime.mon;
        	SetTime.day  = CurTime.day;
        	SetTime.week = CurTime.week;
        	LcdClearScreen();  //Һ������
        	ShowSetDay();     //��ʾ����ʱ��	
        	SetIndex = 0;    //�������������һ�𽫹����������ߵ�λ����
		SetRightShift();
        	 LcdOpenCursor();   //�������  
			 delay(1);
    	}
	}
  else  if((m==1)&&(staSystem == E_SET_Day))
		{
			 staSystem = E_SET_TIME;
        	SetTime.hour = CurTime.hour;
        	SetTime.min  = CurTime.min;
        	SetTime.sec  = CurTime.sec;
       
        	LcdClearScreen();  //Һ������
        	ShowSetTime();     //��ʾ����ʱ��
		
        	SetIndex = 0;    //�������������һ�𽫹����������ߵ�λ����
  	SetRightShift();
        	 LcdOpenCursor();   //������� 
			  delay(1);
    	}
  else if ((m==2)&&(staSystem == E_SET_TIME))  //ʱ�������л�����������
    {
         staSystem = E_SET_ALARM;
        SetTime.sec = 0;          //�����㣬��������ʱ����0�뿪ʼ��ʱ
        SetRealTime(&SetTime);    //�趨ʱ��д��ʵʱʱ��
        SetAlarmHour = AlarmHour; //��ǰ����ֵ���������û�����
        SetAlarmMin  = AlarmMin;
        LcdClearScreen();  //Һ������
		 ShowSetAlarm();    //��ʾ��������		 
		 SetIndex = 0;    //�������������һ�𽫹����������ߵ�λ����
      	SetRightShift();
         LcdOpenCursor();   //������� 
		   delay(1);
    }  	
  else if ((m==3)&&(staSystem ==E_SET_ALARM)) //ʱ�������л�����������
     {
	 	staSystem = E_NORMAL;
	    AlarmHour = SetAlarmHour;  //�趨������ֵд������ʱ��
        AlarmMin  = SetAlarmMin;
   	 LcdCloseCursor();  //�رչ�� 
	   delay(1);
    LcdClearScreen();  //Һ������
    RefreshTime();   //ˢ�µ�ǰʱ��
    RefreshDate(1);  //����ˢ��������ʾ
    RefreshTemp(1);  //����ˢ���¶���ʾ
    }

}

/* �����������������ݼ���ִ����Ӧ�Ĳ�����keycode-�������� */
void KeyAction(uint8 keycode)
{
 
    if (keycode ==0x25)  //������������л�����λ
    {
	
	 
	   	m++;
		  SwitchSystemSta();
       	
    }
    else if (keycode == 0x26)  //�������л�����λ
    {
		 if (staSystem == E_NORMAL)  //���������л���ʱ������
	 {
 	   key= !key;
	    bakekey=key;
	  }
			SetIndex ++;
        SetRightShift();
	
    }
    else if (keycode == 0x28)  //������ֵ
    {
		  if (staSystem == E_NORMAL)  //���������л���ʱ������
		  {
		  	staMute=!staMute;	
		  }
		 InputSetNumber();	
		
    }
   
 }	
