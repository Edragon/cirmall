#ifndef __LORA_H
#define __LORA_H	 
#include "sys.h"

#define BusLock_Time  20		//������ʱ�� Ϊ10ms �ı���
#define GrespWait_Time 700		//�ȴ����ػ�Ӧʱ��
#define LoraNode_Base	0x01		//LoraNode �� EEprom�еĴ洢��ַ
#define Lora_ResponseBuf_Len 12
#define Lora_Rcv_Buf_Len 256



#define G_ADDRESSING	0x01		//����Ѱַ����
#define G_RESPONSE		0x03		//���ػظ�ȷ������
#define G_OPENLOCK		0x05		//���ؿ�������
#define G_QRENCODE_SHOW	0x07		//������ʾ��ά������
#define L_RESPONSE		0x04		//�ն˻ظ�ȷ������
#define L_SELSTAT		0x0A		//�ն˷���ѡ��״̬��Ϣ

typedef struct _LoraNode_
{
	u8 local_adr;		//�ڵ��ַ
	u8 lfir_relay;		//һ���м̵�ַ
	u8 lsec_relay;		//�����м̵�ַ
	u8 lgateway;			//����
	//u8 lbus_sta;			//����״̬	Ϊ1�������ߣ��ڵ㲻�ɷ������ݣ�Ϊ0��ſ����ߣ��ڵ�ɷ�������
} LoraNode;


typedef struct Lora_Packet{
	
	u8 src_adr;			//����֡�����ַ  ����һ��Ϊ0x01  �ն�Ϊ0x02~0x255
	u8 fir_relay;		//һ���м�
	u8 sec_relay;		//�����м�
	u8 target_adr;		//Ŀ���ַ
	u8 next_step;		//��Ϣ��һ����ַ  Ϊ�˹涨��Ϣ����˳����˱���
	u8 cmd;				//������
	u8 data_len;		//���ݳ���
	u8 data_buf[256];	//������	
	u8 verification;	//У����	
}Lora_Pkt;


extern LoraNode Local_node;
extern u8 Lora_Bus_Stat;
extern u8 LockTime_EN;
extern u8 GrespTime_EN;
extern u8 GrespTime_IN;
extern u16 GrespTime_Count;

extern Lora_Pkt Rcv_Lora_Pkt;
extern u16 Terminal_SelStat;

extern u8 Select_ACK;



//extern u8 Lora_SendBuf[16];
extern u8 Lora_ResponseBuf[Lora_ResponseBuf_Len];	//���ؽڵ��Ӧ�������
extern u8 Lora_SelectBuf[16];		//���ؽڵ���ѡ��״̬�������


extern u8 Lora_Rcv_Buf[Lora_Rcv_Buf_Len];
extern u16 Lora_Rcv_Len;
extern u8 Lora_Send_Buf[128];
extern u8 Lora_Send_Len;

extern u8 Local_Addr;
extern u8 Local_Fir_Relay;
extern u8 Local_Sec_Relay;
extern u8 GateWay_Addr;


u8 Local_Node_Init(void);
void Local_Node_Set(void);
u16 Lora_Unpack_Raw(u8 *src,u16 DataLen);
u8 Lora_Analysis_Pkt(Lora_Pkt *Pkt);
//u16 Lora_Pack_Raw(u8 cmd,u8 stat);
u16 Lora_Pack_Raw(u8 *buf,u8 cmd,u8 stat);
void LoraMess_Forwarding(u8 *buf,u16 len);
u8 Gate_Dress_Rcd(Lora_Pkt *Pkt);
void Gate_QRencode_Show(u8 *buf,u8 len);


//u8 Wait_Gresponse(u8 cmd);
u8 Wait_Gresponse(u8 cmd,u16 timeout);


#endif
