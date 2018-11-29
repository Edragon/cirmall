#ifndef __LORA_H
#define __LORA_H	 
#include "sys.h"

#define BusLock_Time  20		//锁总线时间 为10ms 的倍数
#define GrespWait_Time 700		//等待网关回应时间
#define LoraNode_Base	0x01		//LoraNode 在 EEprom中的存储地址
#define Lora_ResponseBuf_Len 12
#define Lora_Rcv_Buf_Len 256



#define G_ADDRESSING	0x01		//网关寻址命令
#define G_RESPONSE		0x03		//网关回复确认命令
#define G_OPENLOCK		0x05		//网关开锁命令
#define G_QRENCODE_SHOW	0x07		//网关显示二维码命令
#define L_RESPONSE		0x04		//终端回复确认命令
#define L_SELSTAT		0x0A		//终端发送选定状态信息

typedef struct _LoraNode_
{
	u8 local_adr;		//节点地址
	u8 lfir_relay;		//一级中继地址
	u8 lsec_relay;		//二级中继地址
	u8 lgateway;			//网关
	//u8 lbus_sta;			//总线状态	为1则锁总线，节点不可发送数据，为0则放开总线，节点可发送数据
} LoraNode;


typedef struct Lora_Packet{
	
	u8 src_adr;			//命令帧发起地址  网关一般为0x01  终端为0x02~0x255
	u8 fir_relay;		//一级中继
	u8 sec_relay;		//二级中继
	u8 target_adr;		//目标地址
	u8 next_step;		//信息下一级地址  为了规定信息传播顺序定义此变量
	u8 cmd;				//命令码
	u8 data_len;		//数据长度
	u8 data_buf[256];	//数据区	
	u8 verification;	//校验码	
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
extern u8 Lora_ResponseBuf[Lora_ResponseBuf_Len];	//本地节点回应命令缓冲区
extern u8 Lora_SelectBuf[16];		//本地节点门选中状态命令缓冲区


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
