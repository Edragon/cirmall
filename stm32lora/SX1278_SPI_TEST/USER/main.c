#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
//#include "lcd.h"
#include "usart.h"	 
#include "timer.h"
/////////////////////////////////////
#include "radio.h"
#include "sx1276.h"
#include "sx1276-Hal.h"
#include "sx1276-Fsk.h"
#include "sx1276-LoRa.h"
/////////////////////////////////////
#include "lora.h"
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE                                 9 // Define the payload size here
 
 
 static uint16_t BufferSize = BUFFER_SIZE;			 // RF buffer size
 static uint8_t Buffer[BUFFER_SIZE];				 // RF buffer
 
 static uint8_t EnableMaster = true;				 // Master/Slave selection
 
 //tRadioDriver *Radio = NULL;
 
 const uint8_t PingMsg[] = "PING";
 const uint8_t PongMsg[] = "PONG";

 u8 send_buf[256] = {0};
 u8 send_buf_len;
 u8 rcv_buf[256] = {0};
 u16 rcv_buf_len;
 /*
  * Manages the master operation
  */
 void OnMaster( void )
 {
	 uint8_t i;
	 
	 switch( SX1276Process() )
	 {
	 case RF_RX_TIMEOUT:
		 // Send the next PING frame
		 Buffer[0] = 'P';
		 Buffer[1] = 'I';
		 Buffer[2] = 'N';
		 Buffer[3] = 'G';
		 for( i = 4; i < BufferSize; i++ )
		 {
			 Buffer[i] = i - 4;
		 }
		//Radio->SetTxPacket( Buffer, BufferSize );
		 SX1276SetTxPacket( Buffer, BufferSize );
		 break;
	 case RF_RX_DONE:
		 //Radio->GetRxPacket( Buffer, ( uint16_t* )&BufferSize );
	  	 SX1276GetRxPacket( Buffer, ( uint16_t* )&BufferSize );
		 if( BufferSize > 0 )
		 {
			 if( strncmp( ( const char* )Buffer, ( const char* )PongMsg, 4 ) == 0 )
			 {

 
				 // Send the next PING frame			
				 Buffer[0] = 'P';
				 Buffer[1] = 'I';
				 Buffer[2] = 'N';
				 Buffer[3] = 'G';
				 // We fill the buffer with numbers for the payload 
				 for( i = 4; i < BufferSize; i++ )
				 {
					 Buffer[i] = i - 4;
				 }
				 //Radio->SetTxPacket( Buffer, BufferSize );
				 SX1276SetTxPacket( Buffer, BufferSize );
			 }
			 else if( strncmp( ( const char* )Buffer, ( const char* )PingMsg, 4 ) == 0 )
			 { // A master already exists then become a slave
				 EnableMaster = false;
				// LedOff( LED_RED );
			 }
		 }			  
		 break;
	 case RF_TX_DONE:
		 // Indicates on a LED that we have sent a PING
		 //LedToggle( LED_RED );
		// Radio->StartRx( );
			SX1276StartRx();
		 break;
	 default:
		 break;
	 }
 }
 
 /*
  * Manages the slave operation
  */
 void OnSlave( void )
 {
	 uint8_t i;
 
	 switch( SX1276Process() )
	 {
	 case RF_RX_DONE:
		 //Radio->GetRxPacket( Buffer, ( uint16_t* )&BufferSize );
	 	 SX1276GetRxPacket( Buffer, ( uint16_t* )&BufferSize );
		 if( BufferSize > 0 )
		 {
			 if( strncmp( ( const char* )Buffer, ( const char* )PingMsg, 4 ) == 0 )
			 {
				 // Indicates on a LED that the received frame is a PING
				// LedToggle( LED_GREEN );
 
				// Send the reply to the PONG string
				 Buffer[0] = 'P';
				 Buffer[1] = 'O';
				 Buffer[2] = 'N';
				 Buffer[3] = 'G';
				 // We fill the buffer with numbers for the payload 
				 for( i = 4; i < BufferSize; i++ )
				 {
					 Buffer[i] = i - 4;
				 }
 
				 //Radio->SetTxPacket( Buffer, BufferSize );
				 SX1276SetTxPacket( Buffer, BufferSize );
			 }
		 }
		 break;
	 case RF_TX_DONE:
		 // Indicates on a LED that we have sent a PONG
		// LedToggle( LED_RED );
		// Radio->StartRx( );
		 SX1276StartRx();
		 break;
	 default:
		 break;
	 }
 }




 int main(void)
 {	 
	u8 key;
	u16 i=0,tmp=0;
	u8 len = 0,t = 0; 
	char *p = 0;
	delay_init();	    	 //延时函数初始化	  
  	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
	//TIM3_Int_Init(35,1);		//  1us计时
	TIM3_Int_Init(499,7199);		//	50ms计时
	TIM4_Int_Init(99,7199);		//10ms计时
	//LED_Init();
	KEY_Init();
	SX1276Init();				//SX1278初始化 包括底层引脚初始化 及 模块参数初始化(频率 分频因子等)
	SX1276StartRx();			//将全局变量RFLRState设置为lora接收模式
	Local_node.local_adr = 2;
	Local_node.lgateway = 1;

#if 0		//send
	Buffer[0] = 'P';
	Buffer[1] = 'I';
	Buffer[2] = 'N';
	Buffer[3] = 'G';
	for( i = 4; i < BufferSize; i++ )
	{
		Buffer[i] = i - 4;
	}
		   
	SX1276SetTxPacket( Buffer, BufferSize );
#endif


	while(1)
	{

#if 0
	if( EnableMaster == true )
	{
		OnMaster( );
	}
	else
	{
		OnSlave( );
	}  
#endif

#if 0		//send


	if(USART_RX_STA&0x8000)
		{					   
			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
			printf("\r\n您发送的消息为:\r\n\r\n");
			memset(send_buf,0,256);
			memcpy(send_buf,USART_RX_BUF,len);
			send_buf_len = len;	
			Usart_Send_Datas(send_buf,len);
			printf("\r\n\r\n");
			memset(USART_RX_BUF,0,200);
			USART_RX_STA=0;
			len = 0;
		}

			if(send_buf_len != 0)
			{
			SX1276SetTxPacket( send_buf, send_buf_len );
			memset(send_buf,0,256);
			send_buf_len = 0;
			}
		
	switch( SX1276Process() )
		{
		case RF_RX_TIMEOUT:
			// Send the next PING frame
			Buffer[0] = 'P';
			Buffer[1] = 'I';
			Buffer[2] = 'N';
			Buffer[3] = 'G';
			for( i = 4; i < BufferSize; i++ )
			{
				Buffer[i] = i - 4;
			}
		   
			SX1276SetTxPacket( Buffer, BufferSize );
			break;
		case RF_RX_DONE:
			
			SX1276GetRxPacket( Buffer, ( uint16_t* )&BufferSize );

			delay_ms(1000);
			delay_ms(1000);
			memset(Buffer,0,BufferSize);
			Buffer[0] = 'P';
			Buffer[1] = 'I';
			Buffer[2] = 'N';
			Buffer[3] = 'G';
			for( i = 4; i < BufferSize; i++ )
			{
				Buffer[i] = i - 4;
			}
		  
			SX1276SetTxPacket( Buffer, BufferSize );
						 
			break;
		case RF_TX_DONE:

			 //  SX1276StartRx();
#if 0
			 Buffer[0] = 'P';
			 Buffer[1] = 'I';
			 Buffer[2] = 'N';
			 Buffer[3] = 'G';
			 for( i = 4; i < BufferSize; i++ )
			 {
				 Buffer[i] = i - 4;
			 }
#endif
			//delay_ms(1000);	
			//MCU_RUN = 0;
			//delay_ms(1000);
			//MCU_RUN = 1;

#if 0
			if(send_buf_len != 0)
			{
			SX1276SetTxPacket( send_buf, send_buf_len );
			memset(send_buf,0,256);
			send_buf_len = 0;
			}
#endif			
			break;
		default:
			break;
		}
#endif


#if 0		//recive 
	switch( SX1276Process() )
			{
			case RF_RX_TIMEOUT:
				// Send the next PING frame
				Buffer[0] = 'P';
				Buffer[1] = 'I';
				Buffer[2] = 'N';
				Buffer[3] = 'G';
				for( i = 4; i < BufferSize; i++ )
				{
					Buffer[i] = i - 4;
				}
			   
				//SX1276SetTxPacket( Buffer, BufferSize );
				break;
			case RF_RX_DONE:
		    	//SX1276StartRx();			//将全局变量RFLRState设置为lora接收模式
#if 0
				SX1276GetRxPacket( Buffer, ( uint16_t* )&BufferSize );
		 if( BufferSize > 0 )
		 {
				memset(Buffer,0,BufferSize);
			  memset(RFBuffer,0,RF_BUFFER_SIZE);
		 }
#endif
				SX1276GetRxPacket( rcv_buf, &rcv_buf_len);
				printf("\r\n");
				for(t=0;t<rcv_buf_len;t++)
				{
					USART_SendData(USART1, rcv_buf[t]);//向串口1发送数据
					while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
				}
				printf("\r\n");
			//	MCU_RUN = 0;
			//	delay_ms(500);
			//	MCU_RUN = 1;
				break;
			case RF_TX_DONE:
	
				 //  SX1276StartRx();
	
				 Buffer[0] = 'P';
				 Buffer[1] = 'I';
				 Buffer[2] = 'N';
				 Buffer[3] = 'G';
				 for( i = 4; i < BufferSize; i++ )
				 {
					 Buffer[i] = i - 4;
				 }
				delay_ms(1000); 
				delay_ms(1000); 
				SX1276SetTxPacket( Buffer, BufferSize );
		
				 break;
			default:
				break;
			}

#endif
/*************************************************************************/
	key = KEY_Scan(1);
	if(key){
		switch(key){
			case KEY0_PRES:
				for(i = 0;i<3;i++){
					Terminal_SelStat = 0x0001;
					tmp = Lora_Pack_Raw(Lora_ResponseBuf,L_SELSTAT,0x01);	
					if(tmp){
						LoraMess_Forwarding(Lora_ResponseBuf,tmp);
						tmp = 0;	
						tmp = Wait_Gresponse(L_SELSTAT,100);
						if(tmp){
							printf("发送Terminal_SelStat %d 并接收到网关应答 发送次数%d\r\n",Terminal_SelStat,i+1);
							goto NEXT_STEP;
						}
						else{
							printf("发送Terminal_SelStat %d 超时未收到应答 并重新发送 已发送次数%d\r\n",Terminal_SelStat,i+1);
						}
					}
				}
				break;
			case KEY1_PRES:
				for(i = 0;i<3;i++){
				Terminal_SelStat = 0x0002;
				tmp = Lora_Pack_Raw(Lora_ResponseBuf,L_SELSTAT,0x01);	
				if(tmp)
				{
						LoraMess_Forwarding(Lora_ResponseBuf,tmp);
						tmp = 0;	
						tmp = Wait_Gresponse(L_SELSTAT,100);
						if(tmp){
							printf("发送Terminal_SelStat %d 并接收到网关应答 发送次数%d\r\n",Terminal_SelStat,i+1);
							goto NEXT_STEP;
						}
						else{
							printf("发送Terminal_SelStat %d 超时未收到应答 并重新发送 已发送次数%d\r\n",Terminal_SelStat,i+1);
						}
					}
				}
				break;
			case KEY2_PRES:
				for(i = 0;i<3;i++){
				Terminal_SelStat = 0x0003;
				tmp = Lora_Pack_Raw(Lora_ResponseBuf,L_SELSTAT,0x01);	
				if(tmp)
				{
						LoraMess_Forwarding(Lora_ResponseBuf,tmp);
						tmp = 0;	
						tmp = Wait_Gresponse(L_SELSTAT,100);
						if(tmp){
							printf("发送Terminal_SelStat %d 并接收到网关应答 发送次数%d\r\n",Terminal_SelStat,i+1);
							goto NEXT_STEP;
						}
						else{
							printf("发送Terminal_SelStat %d 超时未收到应答 并重新发送 已发送次数%d\r\n",Terminal_SelStat,i+1);
						}
					}
				}
				break;
			case KEY3_PRES:
				for(i = 0;i<3;i++){

				Terminal_SelStat = 0x0004;
				tmp = Lora_Pack_Raw(Lora_ResponseBuf,L_SELSTAT,0x01);	
				if(tmp)
				{
					
						LoraMess_Forwarding(Lora_ResponseBuf,tmp);
						tmp = 0;	
						tmp = Wait_Gresponse(L_SELSTAT,100);
						if(tmp){
							printf("发送Terminal_SelStat %d 并接收到网关应答 发送次数%d\r\n",Terminal_SelStat,i+1);
							goto NEXT_STEP;
						}
						else{
							printf("发送Terminal_SelStat %d 超时未收到应答 并重新发送 已发送次数%d\r\n",Terminal_SelStat,i+1);
						}
					}
				}

				break;
			default:
				printf("KEY SCAN Wrong\r\n");
				break;
		}
	}
/******************************************************************************/
	NEXT_STEP:switch( SX1276Process() )
	{
		case RF_RX_TIMEOUT:
			printf("RF_RX_TIMEOUT in LoraMess_Forwarding\r\n");
			break;
		case RF_RX_DONE:
			SX1276GetRxPacket( Lora_Rcv_Buf, &Lora_Rcv_Len);
			printf("lora recieve\r\n\r\n");
			Usart_Send_Datas(Lora_Rcv_Buf,Lora_Rcv_Len);
			printf("\r\n\r\n");
			tmp = Lora_Unpack_Raw(Lora_Rcv_Buf,Lora_Rcv_Len);
			if(tmp)
			{
				Lora_Analysis_Pkt(&Rcv_Lora_Pkt);
			}
			memset(Lora_Rcv_Buf,0,Lora_Rcv_Buf_Len);
			Lora_Rcv_Len = 0;
			SX1276StartRx();			//将全局变量RFLRState设置为lora接收模式	
			break;
		case RF_TX_DONE:	
			SX1276StartRx();
			break;
		default:
			//delay_ms(10);
			break;
	}
/*******************************************************************************/

			if(USART_RX_STA&0x8000)
			{					   
				len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
				if((p = strstr((char *)USART_RX_BUF,"lora send:")))
				{
					memcpy(send_buf,p+10,(USART_RX_STA&0x3FFF)-10);
					send_buf_len = (USART_RX_STA&0x3FFF)-10;
					if(send_buf_len != 0)
					{
						LoraMess_Forwarding(send_buf,send_buf_len);
						memset(send_buf,0,256);
						send_buf_len = 0;
					}
				}
				else if(p = strstr(USART_RX_BUF,"address set:"))
				{
					Local_node.local_adr= *(p+12);
					//if(tmp == *(p+12))
					{
						printf("设置节点地址: %d 完成!\r\n",Local_node.local_adr);
					}

				}
				else if(p = strstr(USART_RX_BUF,"first relay set:"))
				{
					Local_node.lfir_relay= *(p+16);
					//if(tmp == *(p+16))
					{
						printf("设置一级节点地址: %d 完成!\r\n",Local_node.lfir_relay);
					}
				}
				else if(p = strstr(USART_RX_BUF,"second relay set:"))
				{
					Local_node.lsec_relay= *(p+17);
					//if(tmp == *(p+17))
					{
						printf("设置二级节点地址: %d 完成!\r\n",Local_node.lsec_relay);
					}
				}
				else if(p = strstr(USART_RX_BUF,"gateway set:"))
				{
					Local_node.lgateway= *(p+12);
					//if(tmp == *(p+12))
					{
						printf("设置网关 %d完成!\r\n",Local_node.lgateway);
					}
				}
				else if(p = strstr(USART_RX_BUF,"show node mess"))
				{
					printf("Local Address %d\r\n",Local_node.local_adr);
					printf("First Relay %d\r\n",Local_node.lfir_relay);
					printf("Second Relay %d\r\n",Local_node.lsec_relay);
					printf("GateWay Address %d\r\n",Local_node.lgateway);
				}
				else
				{
					printf("命令无法识别请重新输入!!!\r\n");

				}
				memset(USART_RX_BUF,0,USART_REC_LEN);
				USART_RX_STA=0;
				len = 0;
				p = 0;
			}
	
/***********************************************************************************/			



	}
}


