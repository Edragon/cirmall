#include "lora.h"
#include "usart.h"
#include "timer.h"


#include "radio.h"
#include "sx1276.h"
#include "sx1276-Hal.h"
#include "sx1276-Fsk.h"
#include "sx1276-LoRa.h"


#include <stdio.h>
#include <string.h>
u8 Lora_Bus_Stat;				//Ϊ1 �����ߣ��˽ڵ㲻�������߷�������  Ϊ0 ������ ������˽ڵ㷢������
LoraNode Local_node;

Lora_Pkt Rcv_Lora_Pkt;			//������յ���lora֡
//u8 Lora_SendBuf[16]={0};		//lora����֡������
u8 Lora_ResponseBuf[Lora_ResponseBuf_Len]={0};	//���ؽڵ��Ӧ�������
u8 Lora_SelectBuf[16]={0};		//���ؽڵ���ѡ��״̬�������

u8 Gresponse_Flag;		//����ȷ�������ʶ 
u8 Select_ACK;


u8 Gresponse_buf[2]={0};
u8 LockTime_EN;
u8 GrespTime_EN;
u8 GrespTime_IN;
u16 GrespTime_Count = 0;
u16 Terminal_SelStat ;


u8 Local_Addr ;
u8 Local_Fir_Relay;
u8 Local_Sec_Relay;
u8 GateWay_Addr;

u8 Lora_Rcv_Buf[Lora_Rcv_Buf_Len];
u16 Lora_Rcv_Len;
u8 Lora_Send_Buf[128];
u8 Lora_Send_Len;

/*
typedef union
{
	u16 u;
	u8 s[2];
}combine;
combine OpenDoor;
*/


#if 0
#define QRCODE_Y 	80		//TFT��ά����ʾ����y

/*
u8 *QR_TestShow = "qr datashow test !qr datashow test !qr datashow test !qr datashow test !qr datashow test !qr datashow test !\
					qr datashow test !qr datashow test !qr datashow test !qr datashow test !qr datashow test !qr datashow test !\
					BJ_HTXL_CODE_V2.0";
*/

//����ά����ʾ��TFT��
//u8 *qrcode_data ��Ҫ��ʾ���ַ���ָ��
void Display_QRcode(u8 *qrcode_data)
{
	u8 i,j;
	u16 x,y,p;
	u8 qrencode_buff[12];			//���LCD ID�ַ���
	
		taskENTER_CRITICAL();			//�����ٽ���
	EncodeData((char *)qrcode_data);
	
	LCD_Fill(0,SaleGUIdev.stabarheight-1,320,SaleGUIdev.SaleGUIheight,WHITE); 
#ifdef DBG_MOD
	printf("QR size %d\r\n",m_nSymbleSize);	
#endif
	if(m_nSymbleSize*2>320)	
	{
		//LCD_ShowString(10,60,200,16,16,(u8 *)"The QR Code is too large!");//̫����ʾ����
#ifdef DBG_MOD
		printf("The QR Code is too large to show!\r\n");
#endif
		return;
	}

	for(i=0;i<10;i++)
	{
		if((m_nSymbleSize*i*2)>240)	break;
	}
	p=(i-1)*2;//���С
	x=(320-m_nSymbleSize*p)/2;
	y=QRCODE_Y;
#ifdef DBG_MOD
	printf("QR piont:%d \r\n",p);
#endif

	for(i=0;i<m_nSymbleSize;i++)
	{
		for(j=0;j<m_nSymbleSize;j++)
		{
			if(m_byModuleData[i][j]==1)
				LCD_Fill(x+p*i,y+p*j,x+p*(i+1)-1,y+p*(j+1)-1,BLUE);

		}
			
	}
	taskEXIT_CRITICAL();			//�˳��ٽ���

}
#endif




/*
lora���ؽڵ��ʼ��
��24cXX�洢�ı��ؽڵ����Ϣ����Local_node�ṹ����
*/
#if 0
u8 Local_Node_Init(void)
{
	Local_node.lfir_relay=AT24CXX_ReadOneByte(LoraNode_Base);//��EEPROM�ж�ȡ���ڵ���Ϣ	һ���ڵ�	
	Local_node.lsec_relay=AT24CXX_ReadOneByte(LoraNode_Base+1);//�����ڵ�
	Local_node.local_adr=AT24CXX_ReadOneByte(LoraNode_Base+2);//���ڵ��ַ
	Local_node.lgateway=AT24CXX_ReadOneByte(LoraNode_Base+3);//���ص�ַ
	if((Local_node.lgateway != 0x01))			//�������ص�ַ�򵥼����ȡ�Ƿ���ȷ һ����������ص�ַ�̶�Ϊ0x01
	{
#ifdef DBG_MOD
	printf("����lora�ڵ���Ϣ��ʼ��ʧ��!!!\r\n���������ýڵ���Ϣ!!!\r\n");
#endif
		Local_Node_Set();
		return 1;
	}
	printf("һ���ڵ��ַ%d \r\n�����ڵ��ַ%d \r\n���ؽڵ��ַ%d \r\n���ص�ַ%d\r\n",Local_node.lfir_relay,Local_node.lsec_relay,Local_node.local_adr,Local_node.lgateway);
	return 0;
}
#endif
u8 Local_Node_Init(void)
{
#if 0
	Local_node.lfir_relay=AT24CXX_ReadOneByte(LoraNode_Base);//��EEPROM�ж�ȡ���ڵ���Ϣ	һ���ڵ�	
	Local_node.lsec_relay=AT24CXX_ReadOneByte(LoraNode_Base+1);//�����ڵ�
	Local_node.local_adr=AT24CXX_ReadOneByte(LoraNode_Base+2);//���ڵ��ַ
	Local_node.lgateway=AT24CXX_ReadOneByte(LoraNode_Base+3);//���ص�ַ
	if((Local_node.lgateway != 0x01))			//�������ص�ַ�򵥼����ȡ�Ƿ���ȷ һ����������ص�ַ�̶�Ϊ0x01
	{
#ifdef DBG_MOD
	printf("����lora�ڵ���Ϣ��ʼ��ʧ��!!!\r\n���������ýڵ���Ϣ!!!\r\n");
#endif
		Local_Node_Set();
		return 1;
	}
	printf("һ���ڵ��ַ%d \r\n�����ڵ��ַ%d \r\n���ؽڵ��ַ%d \r\n���ص�ַ%d\r\n",Local_node.lfir_relay,Local_node.lsec_relay,Local_node.local_adr,Local_node.lgateway);
	return 0;
#endif

#if 0
	Local_node.lfir_relay=Local_Fir_Relay;
	Local_node.lsec_relay=Local_Sec_Relay;
	Local_node.local_adr=Local_Addr;
	Local_node.lgateway=GateWay_Addr;
#endif
	
#if 0
	if((Local_node.lgateway != 0x01))			//�������ص�ַ�򵥼����ȡ�Ƿ���ȷ һ����������ص�ַ�̶�Ϊ0x01
	{
#ifdef DBG_MOD
	printf("����lora�ڵ���Ϣ��ʼ��ʧ��!!!\r\n���������ýڵ���Ϣ!!!\r\n");
#endif
		Local_Node_Set();
		return 1;
	}
	printf("һ���ڵ��ַ%d \r\n�����ڵ��ַ%d \r\n���ؽڵ��ַ%d \r\n���ص�ַ%d\r\n",Local_node.lfir_relay,Local_node.lsec_relay,Local_node.local_adr,Local_node.lgateway);
#endif
	return 0;


}


/*
ͨ�����Դ������ñ��ؽڵ�Ļ�����Ϣ������EEPROM
*/
void Local_Node_Set(void)
{
	u8 i=4,tmp = 0;	
	char *p = 0;
	printf("�����ýڵ���Ϣ �����ʽ����:!!!\r\n");
	printf("address set: \r\n");
	printf("first relay set: \r\n");
	printf("second relay set: \r\n");
	printf("gateway set: \r\n");
	while(i)
	{
		if(USART_RX_STA & 0x8000)
		{
			if(p = strstr(USART_RX_BUF,"address set:"))
			{
#if 0
				AT24CXX_WriteOneByte(LoraNode_Base+2,*(p+12));
				tmp = AT24CXX_ReadOneByte(LoraNode_Base+2);
				printf("���ýڵ��ַ Ϊ%d \r\n",tmp);
#endif
				Local_Addr = *(p+12);
				//if(tmp == *(p+12))
				{
					printf("���ýڵ��ַ: %d ���!\r\n",Local_Addr);
					i--;
					memset(USART_RX_BUF,0,USART_RX_STA&0x3FFF);
					USART_RX_STA = 0x00;
				}
#if 0
				else
				{
					printf("���ýڵ��ַʧ�� ����������!!!\r\n");
					memset(USART1_RX_BUF,0,USART1_RX_STA&0x3FFF);
					USART1_RX_STA = 0x00;
				}
#endif
			}
			else if(p = strstr(USART_RX_BUF,"first relay set:"))
			{
#if 0			
				AT24CXX_WriteOneByte(LoraNode_Base,*(p+16));
				tmp = AT24CXX_ReadOneByte(LoraNode_Base);
				printf("����һ���ڵ� Ϊ%d \r\n",tmp);
#endif
				Local_Fir_Relay = *(p+16);
				//if(tmp == *(p+16))
				{
					printf("����һ���ڵ��ַ: %d ���!\r\n",Local_Fir_Relay);
					i--;
					memset(USART_RX_BUF,0,USART_RX_STA&0x3FFF);
					USART_RX_STA = 0x00;
				}
#if 0
				else
				{
					printf("����һ���ڵ��ַʧ�� ����������!!!\r\n");
					memset(USART1_RX_BUF,0,USART1_RX_STA&0x3FFF);
					USART1_RX_STA = 0x00;
				}				
#endif
			}
			else if(p = strstr(USART_RX_BUF,"second relay set:"))
			{
#if 0
				AT24CXX_WriteOneByte(LoraNode_Base+1,*(p+17));
				tmp = AT24CXX_ReadOneByte(LoraNode_Base+1);
				printf("���ö����ڵ��ַ Ϊ%d \r\n",tmp);
#endif
				Local_Sec_Relay = *(p+17);
				//if(tmp == *(p+17))
				{
					printf("���ö����ڵ��ַ: %d ���!\r\n",Local_Sec_Relay);
					i--;
					memset(USART_RX_BUF,0,USART_RX_STA&0x3FFF);
					USART_RX_STA = 0x00;
				}
#if 0
				else
				{
					printf("���ö����ڵ��ַʧ�� ����������!!!\r\n");
					memset(USART1_RX_BUF,0,USART1_RX_STA&0x3FFF);
					USART1_RX_STA = 0x00;
				}
#endif
			}
			else if(p = strstr(USART_RX_BUF,"gateway set:"))
			{
#if 0			
				AT24CXX_WriteOneByte(LoraNode_Base+3,*(p+12));
				tmp = AT24CXX_ReadOneByte(LoraNode_Base+3);
				printf("�������ص�ַ Ϊ%d \r\n",tmp);
#endif
				GateWay_Addr = *(p+12);
				//if(tmp == *(p+12))
				{
					printf("�����������!\r\n");
					i--;
					memset(USART_RX_BUF,0,USART_RX_STA&0x3FFF);
					USART_RX_STA = 0x00;
				}
#if 0				
				else
				{
					printf("��������ʧ�� ����������!!!\r\n");
					memset(USART1_RX_BUF,0,USART1_RX_STA&0x3FFF);
					USART1_RX_STA = 0x00;
				}
#endif
			}
			else
			{
				printf("�����޷�ʶ������������!!!\r\n");
				memset(USART_RX_BUF,0,USART_RX_STA&0x3FFF);
				USART_RX_STA = 0x00;
				p = 0;
			}
			
		}
	}
	printf("���ؽڵ��������!\r\n");
}


/*
 ���ԭʼ���ݰ����������ݰ��Ƿ���Ϲ涨��ʽ�������ȣ�У�����
 ����ȷ������ȷ�������ݳ��ȣ���ʽ�е���Ч���ݳ��ȣ�����Ч����0.
 ��ȡ��Ч���ݵ�Rcv_Lora_Pkt�ṹ����
 ��ڣ����յ��������ݰ����׵�ַ�����յ������ݳ��ȡ� (u8 *src,u16 DataLen)
 */
u16 Lora_Unpack_Raw(u8 *src,u16 DataLen)		
{	      
	u8 i = 0;
    u8 verify = 0;
		
	if((src[0] == 0x68) && (src[DataLen-1] == 0x16))     //�����жϰ�ͷ��β�Ƿ�Ϲ�
	{
		for(i = 1; i < DataLen-2; i++)
			verify += src[i];
		if(verify == src[DataLen - 2])				//�Ƚ�У���Ƿ���ȷ
		{
			Rcv_Lora_Pkt.src_adr = src[1];			//ȡ������֡�����ַ
			Rcv_Lora_Pkt.fir_relay = src[2];		//ȡ��һ���м���ֵ(�м���ֵ��ʵΪ�ڵ��ַ)
			Rcv_Lora_Pkt.sec_relay = src[3];		//ȡ�������м���ֵ
			Rcv_Lora_Pkt.target_adr = src[4];		//ȡ����֡Ŀ���ַ
			Rcv_Lora_Pkt.next_step = src[5];		//ȡ����һ����ַ
			Rcv_Lora_Pkt.cmd = src[6];				//ȡ�������ʶ
			Rcv_Lora_Pkt.data_len = src[7];			//ȡ����֡���ݳ��ȱ�ʶ
			memcpy(Rcv_Lora_Pkt.data_buf,&src[8],Rcv_Lora_Pkt.data_len);	//ȡ������������
			Rcv_Lora_Pkt.verification = src[DataLen - 2];	//ȡ��У����
			return Rcv_Lora_Pkt.data_len;		//��������������
		}
		else
		{
#ifdef DBG_MOD
		printf("Lora��������У�����!!!\r\n");
#endif
				//У�����
			return 0;	
		}
	}
	else
	{
#ifdef DBG_MOD
	printf("Lora�������ݰ�ͷ��β����!!!\r\n");
#endif
			//��ͷ��β���Ϲ�
	return 0;	
	}
}


/*
�����������ݰ����з���������������ִ����ض���
*/
u8 Lora_Analysis_Pkt(Lora_Pkt *Pkt)
{
	u8 tmp8 = 0 , i = 0;
	u16 tmp = 0;
	if((Pkt->fir_relay == Local_node.local_adr) && (Pkt->next_step == Local_node.local_adr))		//������յ���lora֡��һ����ַ�ͱ��ص�ַ��ͬ���򱾽ڵ���Ϊһ���м�ת����֡(֡����next_step����Ϊ�����м̻�Ŀ�ĵ�ַ)
	{
		if(Pkt->sec_relay != 0)			//���lora֡�еĶ�����ַ��Ϊ�գ����֡��Ҫ�����м�ת�����Ҵ�֡�е�next_step��Ҫ����Ϊ�����м̵�ַ
		{
			printf("���ڵ�%d �յ�һ���м���Ϣ ת���������ڵ�%d \r\n",Local_node.local_adr,Pkt->sec_relay);	
			Lora_Rcv_Buf[5] = Pkt->sec_relay;		//next_step����Ϊ�¼���ַ		Lora_Rcv_Buf[5]Ϊһ֡��NEXT STEPλ��
			Lora_Rcv_Buf[Lora_Rcv_Len-2] = 0;					//�����ڶ����ֽ�ΪУ��λ  �˴������¶�У��λ���м���
			for(i = 1;i<Lora_Rcv_Len-2;i++){
				Lora_Rcv_Buf[Lora_Rcv_Len-2] += Lora_Rcv_Buf[i];
			}
			LoraMess_Forwarding(Lora_Rcv_Buf,Lora_Rcv_Len);			//��Ϣ���͸���һ��  ������ʽ����ֱ����Ϣ���ͳɹ�(�������߿���״̬����Ϣ�������)
		}
		else if(Pkt->sec_relay == 0x00)			//��������м̵�ַΪ0x00 ��û�ж����м�
		{
		
			if((Pkt->target_adr >= 0x01) && (Pkt->target_adr < 0xFF))		
			{
				printf("���ڵ�%d �յ�һ���м���Ϣ �޶����ڵ� ת����Ŀ���ַ%d\r\n",Local_node.local_adr,Pkt->target_adr);
				Lora_Rcv_Buf[5] = Pkt->target_adr;		//next_step����Ϊ�¼���ַ		
				Lora_Rcv_Buf[Lora_Rcv_Len-2] = 0;					//�����ڶ����ֽ�ΪУ��λ  �˴������¶�У��λ���м���
				for(i = 1;i<Lora_Rcv_Len-2;i++){
					Lora_Rcv_Buf[Lora_Rcv_Len-2] += Lora_Rcv_Buf[i];
				}
				LoraMess_Forwarding(Lora_Rcv_Buf,Lora_Rcv_Len);			//��Ϣ���͸���һ��
			}
		}
	}
	else if((Pkt->sec_relay== Local_node.local_adr) && (Pkt->next_step == Local_node.local_adr))		//��lora֡�еĶ�����ַ�ͱ��ص�ַ��ͬ����next_step�뱾�ص�ַ��ͬ���򱾵��ն���Ϊ�����м�ת������Ϣ
	{
		if((Pkt->target_adr >= 0x01) && (Pkt->target_adr < 0xFF))		//�ж�Ŀ���ַ�Ϸ�0x02~0xFE
		{
			printf("���ڵ�%d �յ������м���Ϣ ת����Ŀ���ַ%d\r\n",Local_node.local_adr,Pkt->target_adr);
			Lora_Rcv_Buf[5] = Pkt->target_adr;		//next_step����Ϊ�¼���ַ
			Lora_Rcv_Buf[Lora_Rcv_Len-2] = 0;					//�����ڶ����ֽ�ΪУ��λ  �˴������¶�У��λ���м���
			for(i = 1;i<Lora_Rcv_Len-2;i++){
				Lora_Rcv_Buf[Lora_Rcv_Len-2] += Lora_Rcv_Buf[i];
			}
			LoraMess_Forwarding(Lora_Rcv_Buf,Lora_Rcv_Len);			//��Ϣ���͸���һ��
		}
	}
	else if((Pkt->target_adr == Local_node.local_adr) && (Pkt->next_step == Local_node.local_adr))		//���lora֡�е�Ŀ���ַ�뱾�ص�ַ��ͬ���򱾽ڵ����֡���֡���ݽ�����Ӧ����			
	{
		printf("���ڵ�%d �յ�ִ��ָ��\r\n");
		//Lora_Bus_Stat = 0;
		switch(Pkt->cmd)
		{
			case G_ADDRESSING:
					printf("Ѱַָ��\r\n");
					tmp8 = Gate_Dress_Rcd(Pkt);		//��¼���ش��ݹ�����·����Ϣ(һ���м̺Ͷ����м̵�ַ)
					if(tmp8 == 0)
					{
						tmp = Lora_Pack_Raw(Lora_ResponseBuf,L_RESPONSE,0x01);	//���ȷ����Ϣ
						if(tmp)
						{
							LoraMess_Forwarding(Lora_ResponseBuf,tmp);
						}
					}
					else
					{
						return 1;
					}
				break;
			case G_RESPONSE:
					//Gresponse_Flag = 1;
					//Gresponse_buf[0] = Rcv_Lora_Pkt.data_buf[0];
					//Gresponse_buf[1] = Rcv_Lora_Pkt.data_buf[1];
					switch(Rcv_Lora_Pkt.data_buf[0])
					{
						case L_SELSTAT:
								if(Rcv_Lora_Pkt.data_buf[1] == 0x01)
								{
										Select_ACK = 1;
#ifdef DBG_MOD
							printf("�յ�ѡ��״̬��������ػظ�~\r\n");
#endif									
								}
							break;
						default :
#ifdef DBG_MOD
							printf("�յ��޷�ʶ������ػ�Ӧ����\r\n");
#endif
							break;
					}
				break;
			case G_QRENCODE_SHOW:
#ifdef DBG_MOD
					printf("�յ����ط������Ķ�ά����Ϣ��׼����ʾ��\r\n");
					printf("%s\r\n",Pkt->data_buf);
#endif			
					//Gate_QRencode_Show(Pkt->data_buf,Pkt->data_len);
					//SPB_Page = 0x02;
						tmp = Lora_Pack_Raw(Lora_ResponseBuf,L_RESPONSE,0x01);	//���ȷ����Ϣ
						if(tmp)
						{
							LoraMess_Forwarding(Lora_ResponseBuf,tmp);
						}
				break;
			case G_OPENLOCK:
#ifdef DBG_MOD
			printf("�յ���������:\r\n");
#endif	
			tmp = Lora_Pack_Raw(Lora_ResponseBuf,L_RESPONSE,0x01);	//���ȷ����Ϣ
			if(tmp)
			{
				LoraMess_Forwarding(Lora_ResponseBuf,tmp);
			}

#if 0
					g74HC595Data[0] = Rcv_Lora_Pkt.data_buf[0];
					g74HC595Data[1] |= (Rcv_Lora_Pkt.data_buf[1] & 0x03);
					DO_OUT();
					delay_ms(300);		//������ʱ300ms		
					g74HC595Data[0] = 0x00;
					g74HC595Data[1] &= 0xFC;
					DO_OUT();		//�ر�������(���ϵ�)
					
#ifdef DBG_MOD
	printf("�򿪹��ź� %x %x\r\n",g74HC595Data[0],g74HC595Data[1]);		
#endif
#endif
			break;
			default:
#ifdef DBG_MOD
	printf("�޶��������!!!\r\n");			
#endif
				break;
		}
	}
	else
	{
#ifdef DBG_MOD
	printf("���յ����õ�lora�ڵ���Ϣ!!!\r\n");	
#endif
	}

//memset(Lora_Rcv_Buf,0,USART2_REC_LEN);
memset(Pkt,0,sizeof(Rcv_Lora_Pkt));
	return 0;
}

#if 0
/*
�������
����������������ֽ����ݳ���
cmdΪ�ն�Ҫ���͵�������
statΪ���յ�ָ���ִ��״̬
*/
u16 Lora_Pack_Raw(u8 cmd,u8 stat)
{
	u8 verify = 0,i = 0;
	u16 len=0;
	memset(Lora_SendBuf,0,16);
	Lora_SendBuf[0] = 0x68;
	Lora_SendBuf[1] = Local_node.local_adr;

	//Lora_SendBuf[2] = Local_node.lfir_relay;
	//Lora_SendBuf[3] = Local_node.lsec_relay;

	Lora_SendBuf[2] = Local_node.lsec_relay;		//��Ϣ����·��һ���м̺Ͷ����м���Ҫ�����´���
	Lora_SendBuf[3] = Local_node.lfir_relay;

	Lora_SendBuf[4] = Local_node.lgateway;
	if(Local_node.lsec_relay != 0x00)
	{
		Lora_SendBuf[5] = Local_node.lsec_relay;
	}
	else if(Local_node.lfir_relay != 0x00)
	{
		Lora_SendBuf[5] = Local_node.lfir_relay;
	}
	else
	{
		Lora_SendBuf[5] = Local_node.lgateway;
	}
	Lora_SendBuf[6] = cmd;


	switch(cmd)
	{
		case L_RESPONSE:
			Lora_SendBuf[7] = 0x02;		//���ݳ���Ϊ2
			Lora_SendBuf[8] = Rcv_Lora_Pkt.cmd;		//��������Ϊ���յ�����������
			Lora_SendBuf[9] = stat;			//ִ�н��յ�������Ľ�� statΪ1��ʾ����ִ������ statΪ2���ǽ���ִ���쳣
			for(i=1;i<10;i++)
			{
				verify += Lora_SendBuf[i];
			}
			Lora_SendBuf[10] = verify;
			Lora_SendBuf[11] = 0x16;
			len = 12;
			break;
		case L_SELSTAT:
			Lora_SendBuf[7] = 0x02;		//���ݳ���Ϊ2
			Lora_SendBuf[8] = Terminal_SelStat & 0x00FF;		//ȡ��8λ D7~D0 ������ӵ�1~8��
			Lora_SendBuf[9] = Terminal_SelStat >> 8;			//ȡ��8λ D7~D0 ������ӵ�9~16��
			for(i=1;i<10;i++)
			{
				verify += Lora_SendBuf[i];
			}
			Lora_SendBuf[10] = verify;
			Lora_SendBuf[11] = 0x16;
			len = 12;
			break;

		default:
			len = 0;
			break;
	}
	return len;
}
#endif

/*
�������
����������������ֽ����ݳ���
cmdΪ�ն�Ҫ���͵�������
statΪ���յ�ָ���ִ��״̬
*/
u16 Lora_Pack_Raw(u8 *buf,u8 cmd,u8 stat)
{
	u8 verify = 0,i = 0;
	u16 len=0;
	memset(buf,0,Lora_ResponseBuf_Len);		//�ݶ����з��Ͱ��ĳ���Ϊ12�ֽ�
	buf[0] = 0x68;
	buf[1] = Local_node.local_adr;

	//Lora_SendBuf[2] = Local_node.lfir_relay;
	//Lora_SendBuf[3] = Local_node.lsec_relay;

	buf[2] = Local_node.lsec_relay;		//��Ϣ����·��һ���м̺Ͷ����м���Ҫ�����´���
	buf[3] = Local_node.lfir_relay;

	buf[4] = Local_node.lgateway;
	if(Local_node.lsec_relay != 0x00)
	{
		buf[5] = Local_node.lsec_relay;
	}
	else if(Local_node.lfir_relay != 0x00)
	{
		buf[5] = Local_node.lfir_relay;
	}
	else
	{
		buf[5] = Local_node.lgateway;
	}
	buf[6] = cmd;


	switch(cmd)
	{
		case L_RESPONSE:
			buf[7] = 0x02;		//���ݳ���Ϊ2
			buf[8] = Rcv_Lora_Pkt.cmd;		//��������Ϊ���յ�����������  ������Ҫȷ��֮ǰ��Rcv_Lora_Pkt��û�б����
			buf[9] = stat;			//ִ�н��յ�������Ľ�� statΪ1��ʾ����ִ������ statΪ2���ǽ���ִ���쳣
			for(i=1;i<10;i++)
			{
				verify += buf[i];
			}
			buf[10] = verify;
			buf[11] = 0x16;
#if 0
			buf[12] = 0x00;		//����4��0��ʾ�ſ�����
			buf[13] = 0x00;
			buf[14] = 0x00;
			buf[15] = 0x00;
#endif
			len = 12;
			break;
		case L_SELSTAT:
			buf[7] = 0x02;		//���ݳ���Ϊ2
			buf[8] = Terminal_SelStat & 0x00FF;		//ȡ��8λ D7~D0 ������ӵ�1~8��
			buf[9] = Terminal_SelStat >> 8;			//ȡ��8λ D7~D0 ������ӵ�9~16��
			for(i=1;i<10;i++)
			{
				verify += buf[i];
			}
			buf[10] = verify;
			buf[11] = 0x16;
#if 0
			buf[12] = 0x00;		//����4��0��ʾ�ſ�����
			buf[13] = 0x00;
			buf[14] = 0x00;
			buf[15] = 0x00;
#endif
			len = 12;
			break;

		default:
			len = 0;
			break;
	}
	return len;
}








/*
lora��Ϣ����
u8 *buf���������ĵ�ַָ��
u16 len���ͳ���
u16 �����ѷ����ֽ���
*/
void LoraMess_Forwarding(u8 *buf,u16 len)
{
	u8 i = 1;

		if(1)		//������Ϊ����״̬���ڵ�ɷ�����Ϣ
		{
		SX1276SetTxPacket(buf,len);
		while(i)
		{

		switch( SX1276Process() )
					{
					case RF_RX_TIMEOUT:
						printf("RF_RX_TIMEOUT in LoraMess_Forwarding\r\n");
						break;
					case RF_RX_DONE:
						//SX1276StartRx();			//��ȫ�ֱ���RFLRState����Ϊlora����ģʽ
#if 0
						SX1276GetRxPacket( Buffer, ( uint16_t* )&BufferSize );
				 if( BufferSize > 0 )
				 {
						memset(Buffer,0,BufferSize);
					  memset(RFBuffer,0,RF_BUFFER_SIZE);
				 }

						SX1276GetRxPacket( rcv_buf, &rcv_buf_len);
						printf("\r\n");
						for(t=0;t<rcv_buf_len;t++)
						{
							USART_SendData(USART1, rcv_buf[t]);//�򴮿�1��������
							while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
						}
						printf("\r\n");
					//	MCU_RUN = 0;
					//	delay_ms(500);
					//	MCU_RUN = 1;
#endif
						printf("RF_RX_DONE in LoraMess_Forwarding\r\n");	
						break;
					case RF_TX_DONE:
						i = 0;	
						 SX1276StartRx();
#if 0			
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
#endif
						
						 break;
					default:
						//delay_ms(10);
						break;
					}

		}
#if 0
		else 
		{
			delay_ms(200);
			i++;
#ifdef DBG_MOD
		printf("Lora���߷�æ!!!\r\n200ms��� %d �γ�����Ϣ�ط�^^^^^^^\r\n",i);
#endif
		}
#endif
	}
}

/*
��¼���ؽڵ��·����Ϣ��һ���м̺Ͷ����м̵ĵ�ַ
*Pkt Ϊ���յ����ط��͹�����Ѱַ�������ݰ�
��ȡ�ɹ�����0  ��ȡʧ�ܷ���1

����һ��:���ؽڵ�ĵ�ַ�����ص�ַ��Ҫͨ�����ڽ������û���ר������ʵ��
*/
u8 Gate_Dress_Rcd(Lora_Pkt *Pkt)
{
#if 0
	u8 tmp = 0;
	AT24CXX_WriteOneByte(LoraNode_Base,Pkt->fir_relay);	
	AT24CXX_WriteOneByte(LoraNode_Base+1,Pkt->sec_relay);
	tmp = AT24CXX_ReadOneByte(LoraNode_Base);
	if(tmp != Pkt->fir_relay)
		return 1;
	tmp = AT24CXX_ReadOneByte(LoraNode_Base+1);
	if(tmp != Pkt->sec_relay)
		return 1;
	return 0;
#endif


	Local_node.lfir_relay= Pkt->fir_relay;
	Local_node.lsec_relay= Pkt->sec_relay;

	printf("�����ؽ��յ�Ѱַ��Ϣ\r\nһ���м̵�ַ:%d\r\n�����м̵�ַ:%d\r\n",Local_node.lfir_relay,Local_node.lsec_relay);
	return 0;	
}


/*
��ʾ���ش��ݹ����Ķ�ά����Ϣ(������250�ֽ�)
u8 *bufΪRcv_Lora_Pkt�ṹ���е�data_bufָ��
u8 len ΪRcv_Lora_Pkt�ṹ���е�data_len
*/
void Gate_QRencode_Show(u8 *buf,u8 len)
{
#if 0
	u8 *tmp = 0;
	tmp =(u8 *)malloc(len * sizeof(u8));
	if(tmp == 0)
		return 1;
	memset(tmp,0,len);
	memcpy(tmp,buf,len);
	Display_QRcode(tmp);	
	free(tmp);
	return 0;
#endif

	//Display_QRcode(buf);

}



#if 0
/*
�ȴ����ط���ȷ������
u8 cmd 
u16 timeout �ȴ����ػ�Ӧ��ʱ
�ڹ涨ʱ�����յ���ȷ��Ӧ������ ����1 ���� ���� 0
*/
u8 Wait_Gresponse(u8 cmd)
{
	u16 tmp= 0;
	GrespTime_EN = 1;
	GrespTime_Count = 0;
	GrespTime_IN = 1;
	Gresponse_Flag = 0;
	memset(Gresponse_buf,0,2);
	while(GrespTime_IN)		
	{
		if(USART2_RX_STA & (1<<15))
		{
			tmp = Lora_Unpack_Raw(Lora_Rcv_Buf,USART2_RX_STA&0x3FFF);
				if(tmp)
				{
					Lora_Analysis_Pkt(&Rcv_Lora_Pkt);
					if(Gresponse_Flag == 1)
					{
						if((Gresponse_buf[0]== cmd) &&(Gresponse_buf[1]==0x01))
							return 1;
					}
				}
		}
	}
	return 0;
}
#endif

/*
�ȴ����ط���ȷ������
u8 cmd 
u16 timeout �ȴ����ػ�Ӧ��ʱΪ10ms�ı���
�ڹ涨ʱ�����յ���ȷ��Ӧ������ ����1 ���� ���� 0
*/
u8 Wait_Gresponse(u8 cmd,u16 timeout)
{
	u16 tmp= 0;
	GrespTime_EN = 1;
	GrespTime_Count = timeout;
	GrespTime_IN = 1;
	Gresponse_Flag = 0;
	memset(Gresponse_buf,0,2);
	TIM_Cmd(TIM4,ENABLE);
	while(GrespTime_IN)
	{
			switch( SX1276Process() )
			{
				case RF_RX_TIMEOUT:
					printf("RF_RX_TIMEOUT in LoraMess_Forwarding\r\n");
					break;
				case RF_RX_DONE:
					SX1276GetRxPacket( Lora_Rcv_Buf, &Lora_Rcv_Len);
					tmp = Lora_Unpack_Raw(Lora_Rcv_Buf,Lora_Rcv_Len);
					if(tmp)
					{
						Lora_Analysis_Pkt(&Rcv_Lora_Pkt);
						switch(cmd){
							case L_SELSTAT:
								if(Select_ACK == 1){
									Select_ACK = 0;
									memset(Lora_Rcv_Buf,0,Lora_Rcv_Buf_Len);
									GrespTime_EN = 0;
									GrespTime_IN = 0;
									TIM_Cmd(TIM4,DISABLE);	
									return 1;
								}
								break;
							default:

								break;

						}
					//	memset(Lora_Rcv_Buf,0,Lora_Rcv_Buf_Len);
						
					}

							SX1276StartRx();			//��ȫ�ֱ���RFLRState����Ϊlora����ģʽ	
							break;
						case RF_TX_DONE:
							 SX1276StartRx();
							 break;
						default:
							//delay_ms(100);
							break;
						}
						//delay_ms(10);
						//timeout--;


	}

	return 0;

}


