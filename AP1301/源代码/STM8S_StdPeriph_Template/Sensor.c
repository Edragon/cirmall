#define __SENSOR_C

#include "Sensor.h"
#include "Key.h"
#include "system.h"
#include "Filter.h"
#include "Speed_Control.h"


static u16 TurnBad_Count;
u16 t[3]={60,20,30};
static u16 TurnGood_Count = 30;

u8 Backup_Air_Condition;

u8 Ro=1;  //气体传感器基准值
//static u16 Rs=0;  //Rs:传感器当前阻值
u8 VOCActive = 0;
u16 VOC_Ratio=100;

//气体传感器基准值更新以及饱和判断计时器
//所谓饱和，就是传感器在很长的一段时间内测得的
//结果都是变坏或者没变;如果出现饱和，就以当前值为
//新的基准值
//static u16 cnt_vocturngood=0;
//static u16 cnt_vocturnbad=0;
u16 cnt_vocturngood=0;
u16 cnt_vocturnbad=0;

u16 cnt_vocsensorfault=0; //气体传感器故障计时

//----------------------------------------------
//VOC初始化计时
void VOC_StartupCounting(void)
{
  static u16 j=0;
  
  if(Power_Status != OFF)
  {
    j++;
    if(j >= 180) //3min.
    {
      j = 0;
      if(VOCActive == 0)
      {
        VOCActive = 1;
      }
    }
  }
  else
  {
    j = 0;
    VOCActive = 0;
  }
}
//------------------------------------------
//AD采样通道选择
void ADC1_Channel_Sel(ADC1_Channel_TypeDef ADC1_Channel)
{
  AD_WaitStop();
  ADC1->CSR &= (u8)(~ADC1_CSR_CH);
  ADC1->CSR |= (u8)(ADC1_Channel);
  AD_Start();
}
//------------------------------------------
//AD采样处理
void Process_AD(void)
{
  static u16 cnt_ad;
  static u8 Channel=0;
  
  //if(Fan_Speed != SPEED_STOP)
  //{
    if(Channel == AD_CHANNEL_VFAN)
    {
      AD_WaitStop();
      Process_VFan();
    }
    else
    {
      AD_WaitStop();
      Process_VOCSensor();
    }
    
    if(Func_Cnt_Is_Ok(cnt_ad,10)) //每10ms换一次转换通道
    {
      Func_Cnt_Get(cnt_ad);
          
      Channel++;
      if(Channel > 1)
      {
        Channel = 0;
      }
           
      switch(Channel) //改变AD采样通道
      {
      case AD_CHANNEL_VFAN: ADC1_Channel_Sel(ADC1_CHANNEL_6);break;
                   
      case AD_CHANNEL_VOC: ADC1_Channel_Sel(ADC1_CHANNEL_5);break;    
      }
    }
    
    Estimate_AirCondition();
  //}
  //else
  //{
    //Channel = 0;
  //}
}
//----------------------------------------------
//VOC气体传感器程序
void Process_VOCSensor(void)
{
  //u8 ad_h,ad_l;
  static u16 Sum_VOCValue=0,VOC_Value=0,i=0;
  static u16 Rs=0;  //Rs:传感器当前阻值
  static u16 cnt_voc;
  //static u8 activeflag=0;
  
  if((ADC1->CSR & 0x80) != 0)
  {
    ADC1->CSR &= 0x7f;
  
    i = ADC1_GetConversionValue();
    
    AD_Start();
    
    Sum_VOCValue = Sum_VOCValue - VOC_Value + i; //采样数据处理
    VOC_Value = Sum_VOCValue >> 5;
  }
  
  //if(VOCActive != 0)  //气体传感器激活
  {
    if(Func_Cnt_Is_Ok(cnt_voc,2000))
    {
      Func_Cnt_Get(cnt_voc);
      
      //if(VOC_Value > 10 && VOC_Value < 1000)  //正常值  
      {
        //计算当前电阻值
        Rs = (u16)((u32)(1024 - VOC_Value)*100)/VOC_Value;
        //Rs = (u16)((u32)(1024 - VOC_Value)*47)/(10*VOC_Value);
        
        if(Ro != 0)
        {
          VOC_Ratio = (Rs*100)/Ro;  //计算相对变化
        }
        else
        {
          Ro = 2;
          VOC_Ratio = (Rs*100)/Ro;  //计算相对变化
        }
      }
    }
  }
  
  
  //if(VOCActive != 0)  //气体传感器激活
  {
     //if(VOC_Value > 10 && VOC_Value < 1000)  //正常值  
      {
        if(Rs > Ro)  //如果10分钟内测得的值都比基准值好，就更新基准值
        {
          if(Func_nSec_Cnt_Is_Ok(cnt_vocturngood,300))  //10分钟
          {
            Func_nSec_Cnt_Get(cnt_vocturngood);
            
            Ro = Rs;  //更新基准值
          }
          
          Func_nSec_Cnt_Get(cnt_vocturnbad);
        }
        else //如果测得的值没变或者变坏，就计时
        {
          if(Func_nSec_Cnt_Is_Ok(cnt_vocturnbad,600)) //30minute，认为饱和
          {
            Func_nSec_Cnt_Get(cnt_vocturnbad);
            Ro = Rs;  //更新基准值
          }
          
          Func_nSec_Cnt_Get(cnt_vocturngood);
        }
        
        Func_nSec_Cnt_Get(cnt_vocsensorfault);
      }
      //else  //传感器故障
      //{
        //if(Func_nSec_Cnt_Is_Ok(cnt_vocsensorfault,30)) //1分钟计时
        //{
            //VOC_Ratio = 100; //气体传感器失效，给一个固定的值(好值)
        //}
      //}
    
  }
}
//------------------------------------------
//空气质量评估
void Estimate_AirCondition(void)
{
  if(VOCActive != 0)  //VOC气体传感器激活
  {
    if(VOC_Ratio < 65)
    {
      Es_Air_Condition = Normal;
    } 
    else if(VOC_Ratio < 85)
    {
      Es_Air_Condition = Good;
    }
    else
    {
      Es_Air_Condition = Excellent;
    }
  }  
}
//----------------------------------------------
//空气质量判定
void Process_RealAirQuality(void)
{
  static u16 cnt_air=0,cnt_TurnBad=0;
  
    if(VOCActive != 0)
    {
      if(Es_Air_Condition < Air_Condition) //空气变好
        {
          if(TurnGood_Count != 0)
          {
            if(Func_Cnt_Is_Ok(cnt_air,1000))
            {
              Func_Cnt_Get(cnt_air);
              TurnGood_Count--;
            }
            if(TurnGood_Count == 0)
            {
              
              if(Air_Condition != Excellent)
              {
                Air_Condition--;
                
              }
              TurnGood_Count = t[Air_Condition-1];
            
            }
          }         
        }
        else if(Es_Air_Condition == Air_Condition) //空气
        {
          TurnGood_Count = t[Air_Condition-1];
        }
        else if(Es_Air_Condition > Air_Condition) //空气变差
        {
		  if(Backup_Air_Condition < Es_Air_Condition)
          	Backup_Air_Condition = Es_Air_Condition;
          
        }
        
       if(Backup_Air_Condition !=0 &&  
          Air_Condition != Backup_Air_Condition && TurnBad_Count==0)
       {
         TurnBad_Count = 2;
       }
        
       if(TurnBad_Count != 0)
       {
         if(Func_Cnt_Is_Ok(cnt_TurnBad,1000))
         {
           Func_Cnt_Get(cnt_TurnBad);
           TurnBad_Count--;
         } 
            
         TurnGood_Count = t[Air_Condition-1]; //只要空气变差，就不能计时了
           
         if(TurnBad_Count == 0)
         {
           if(Air_Condition != Bad)
           {
             Air_Condition ++;
           }
           if(Air_Condition == Backup_Air_Condition)
           {
             Backup_Air_Condition = 0;
           } 
         }
       } 
    }
  
}
//----------------------------------------------
//VOC气体传感器程序
void Process_VFan(void)
{
  static u16 Sum_VFanValue=0,VFan_Value=0,i=0;
  static u16 Target_Vfan=0;
  
  static u16 cnt_pi=0;
  static u16 old_duty=0;
  static u16 duty=0;
  
  if((ADC1->CSR & 0x80) != 0)
  {
    ADC1->CSR &= 0x7f;
  
    i = ADC1_GetConversionValue();
    
    AD_Start();
    
    Sum_VFanValue = Sum_VFanValue - VFan_Value + i; //采样数据处理
    VFan_Value = Sum_VFanValue >> 5;
  }
  
  if(Vfan == 0)
  {
    duty = 0;
  }
  else
  {
    Target_Vfan = (u16)(Vfan*87);
    
    if(Func_Cnt_Is_Ok(cnt_pi,10))
    {
      Func_Cnt_Get(cnt_pi);
      
      if(VFan_Value < Target_Vfan)
      {
        duty++;
        if(duty >= 100)
        {
          duty = 100;
        }
      }
      else
      {
        if(duty != 0)
        {
          duty--;
        }
        else
        {
          duty = 0;
        }
      }
    }
  }
  
  if(old_duty != duty)
  {
    old_duty = (u16)duty;
    TIM2->CCR1H = (u8)(duty >> 8);
    TIM2->CCR1L = (u8)(duty);  
  }
}

//------------------------------------------
//三色LED灯显示处理
void Process_LED(void)
{
  if(Lock_Status != LOCKED)
  {
    if(VOCActive != 0)
    {
      if(Power_Status != OFF)
      {
        switch(Air_Condition)         
        {                              
        //case Bad:
          //{
            //LED_Air_Red(); 
            //break;
          //}
        case Normal:
          {
            LED_Air_Purple();
            break;
          }
        case Good:
          {
            LED_Air_Green();
            break;
          }
        case Excellent:
          {
            LED_Air_Blue();  
            break;
          }
        }
      }
      else
      {
        LED_Air_OFF();
      }
    }
    else
    {
      if(Power_Status != OFF)
      {
        //Blink_Blue();
        LED_Air_Purple();
      }
      else
      {
        LED_Air_OFF();
      }
    }
  }
  else
  {
    //Blink_ReplaceFilter();
    LED_Air_Red();
  }
}

//------------------------------------------
//
void Blink_Blue(void)
{
  static u16 cnt_blinkled;
  static u8 flag_blinkled=0;
  
  if(Func_Cnt_Is_Ok(cnt_blinkled,500))
  {
    Func_Cnt_Get(cnt_blinkled);
    
    if(flag_blinkled != 0)
    {
      flag_blinkled = 0;
      LED_Air_OFF();
    }
    else
    {
      flag_blinkled = 1;
      LED_Air_Blue();
    }
  }
}