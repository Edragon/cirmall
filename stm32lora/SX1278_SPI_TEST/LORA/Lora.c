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
u8 Lora_Bus_Stat;				//为1 锁总线，此节点不能向总线发送数据  为0 开总线 ，允许此节点发送数据
LoraNode Local_node;

Lora_Pkt Rcv_Lora_Pkt;			//定义接收到的lora帧
//u8 Lora_SendBuf[16]={0};		//lora发送帧缓冲区
u8 Lora_ResponseBuf[Lora_ResponseBuf_Len]={0};	//本地节点回应命令缓冲区
u8 Lora_SelectBuf[16]={0};		//本地节点门选中状态命令缓冲区

u8 Gresponse_Flag;		//网关确认命令标识 
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
#define QRCODE_Y 	80		//TFT二维码显示坐标y

/*
u8 *QR_TestShow = "qr datashow test !qr datashow test !qr datashow test !qr datashow test !qr datashow test !qr datashow test !\
					qr datashow test !qr datashow test !qr datashow test !qr datashow test !qr datashow test !qr datashow test !\
					BJ_HTXL_CODE_V2.0";
*/

//将二维码显示到TFT屏
//u8 *qrcode_data 需要显示的字符串指针
void Display_QRcode(u8 *qrcode_data)
{
	u8 i,j;
	u16 x,y,p;
	u8 qrencode_buff[12];			//存放LCD ID字符串
	
		taskENTER_CRITICAL();			//进入临界区
	EncodeData((char *)qrcode_data);
	
	LCD_Fill(0,SaleGUIdev.stabarheight-1,320,SaleGUIdev.SaleGUIheight,WHITE); 
#ifdef DBG_MOD
	printf("QR size %d\r\n",m_nSymbleSize);	
#endif
	if(m_nSymbleSize*2>320)	
	{
		//LCD_ShowString(10,60,200,16,16,(u8 *)"The QR Code is too large!");//太大显示不下
#ifdef DBG_MOD
		printf("The QR Code is too large to show!\r\n");
#endif
		return;
	}

	for(i=0;i<10;i++)
	{
		if((m_nSymbleSize*i*2)>240)	break;
	}
	p=(i-1)*2;//点大小
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
	taskEXIT_CRITICAL();			//退出临界区

}
#endif




/*
lora本地节点初始化
将24cXX存储的本地节点的信息读到Local_node结构体中
*/
#if 0
u8 Local_Node_Init(void)
{
	Local_node.lfir_relay=AT24CXX_ReadOneByte(LoraNode_Base);//从EEPROM中读取本节点信息	一级节点	
	Local_node.lsec_relay=AT24CXX_ReadOneByte(LoraNode_Base+1);//二级节点
	Local_node.local_adr=AT24CXX_ReadOneByte(LoraNode_Base+2);//本节点地址
	Local_node.lgateway=AT24CXX_ReadOneByte(LoraNode_Base+3);//网关地址
	if((Local_node.lgateway != 0x01))			//利用网关地址简单检验读取是否正确 一般情况下网关地址固定为0x01
	{
#ifdef DBG_MOD
	printf("本地lora节点信息初始化失败!!!\r\n请重新设置节点信息!!!\r\n");
#endif
		Local_Node_Set();
		return 1;
	}
	printf("一级节点地址%d \r\n二级节点地址%d \r\n本地节点地址%d \r\n网关地址%d\r\n",Local_node.lfir_relay,Local_node.lsec_relay,Local_node.local_adr,Local_node.lgateway);
	return 0;
}
#endif
u8 Local_Node_Init(void)
{
#if 0
	Local_node.lfir_relay=AT24CXX_ReadOneByte(LoraNode_Base);//从EEPROM中读取本节点信息	一级节点	
	Local_node.lsec_relay=AT24CXX_ReadOneByte(LoraNode_Base+1);//二级节点
	Local_node.local_adr=AT24CXX_ReadOneByte(LoraNode_Base+2);//本节点地址
	Local_node.lgateway=AT24CXX_ReadOneByte(LoraNode_Base+3);//网关地址
	if((Local_node.lgateway != 0x01))			//利用网关地址简单检验读取是否正确 一般情况下网关地址固定为0x01
	{
#ifdef DBG_MOD
	printf("本地lora节点信息初始化失败!!!\r\n请重新设置节点信息!!!\r\n");
#endif
		Local_Node_Set();
		return 1;
	}
	printf("一级节点地址%d \r\n二级节点地址%d \r\n本地节点地址%d \r\n网关地址%d\r\n",Local_node.lfir_relay,Local_node.lsec_relay,Local_node.local_adr,Local_node.lgateway);
	return 0;
#endif

#if 0
	Local_node.lfir_relay=Local_Fir_Relay;
	Local_node.lsec_relay=Local_Sec_Relay;
	Local_node.local_adr=Local_Addr;
	Local_node.lgateway=GateWay_Addr;
#endif
	
#if 0
	if((Local_node.lgateway != 0x01))			//利用网关地址简单检验读取是否正确 一般情况下网关地址固定为0x01
	{
#ifdef DBG_MOD
	printf("本地lora节点信息初始化失败!!!\r\n请重新设置节点信息!!!\r\n");
#endif
		Local_Node_Set();
		return 1;
	}
	printf("一级节点地址%d \r\n二级节点地址%d \r\n本地节点地址%d \r\n网关地址%d\r\n",Local_node.lfir_relay,Local_node.lsec_relay,Local_node.local_adr,Local_node.lgateway);
#endif
	return 0;


}


/*
通过调试串口设置本地节点的基础信息并存入EEPROM
*/
void Local_Node_Set(void)
{
	u8 i=4,tmp = 0;	
	char *p = 0;
	printf("请设置节点信息 命令格式如下:!!!\r\n");
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
				printf("设置节点地址 为%d \r\n",tmp);
#endif
				Local_Addr = *(p+12);
				//if(tmp == *(p+12))
				{
					printf("设置节点地址: %d 完成!\r\n",Local_Addr);
					i--;
					memset(USART_RX_BUF,0,USART_RX_STA&0x3FFF);
					USART_RX_STA = 0x00;
				}
#if 0
				else
				{
					printf("设置节点地址失败 请重新设置!!!\r\n");
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
				printf("设置一级节点 为%d \r\n",tmp);
#endif
				Local_Fir_Relay = *(p+16);
				//if(tmp == *(p+16))
				{
					printf("设置一级节点地址: %d 完成!\r\n",Local_Fir_Relay);
					i--;
					memset(USART_RX_BUF,0,USART_RX_STA&0x3FFF);
					USART_RX_STA = 0x00;
				}
#if 0
				else
				{
					printf("设置一级节点地址失败 请重新设置!!!\r\n");
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
				printf("设置二级节点地址 为%d \r\n",tmp);
#endif
				Local_Sec_Relay = *(p+17);
				//if(tmp == *(p+17))
				{
					printf("设置二级节点地址: %d 完成!\r\n",Local_Sec_Relay);
					i--;
					memset(USART_RX_BUF,0,USART_RX_STA&0x3FFF);
					USART_RX_STA = 0x00;
				}
#if 0
				else
				{
					printf("设置二级节点地址失败 请重新设置!!!\r\n");
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
				printf("设置网关地址 为%d \r\n",tmp);
#endif
				GateWay_Addr = *(p+12);
				//if(tmp == *(p+12))
				{
					printf("设置网关完成!\r\n");
					i--;
					memset(USART_RX_BUF,0,USART_RX_STA&0x3FFF);
					USART_RX_STA = 0x00;
				}
#if 0				
				else
				{
					printf("设置网关失败 请重新设置!!!\r\n");
					memset(USART1_RX_BUF,0,USART1_RX_STA&0x3FFF);
					USART1_RX_STA = 0x00;
				}
#endif
			}
			else
			{
				printf("命令无法识别请重新输入!!!\r\n");
				memset(USART_RX_BUF,0,USART_RX_STA&0x3FFF);
				USART_RX_STA = 0x00;
				p = 0;
			}
			
		}
	}
	printf("本地节点设置完成!\r\n");
}


/*
 解包原始数据包，分析数据包是否符合规定格式，及长度，校验等是
 否正确。若正确返回数据长度（格式中的有效数据长度），无效返回0.
 提取有效数据到Rcv_Lora_Pkt结构体中
 入口：接收的整个数据包的首地址及接收到的数据长度。 (u8 *src,u16 DataLen)
 */
u16 Lora_Unpack_Raw(u8 *src,u16 DataLen)		
{	      
	u8 i = 0;
    u8 verify = 0;
		
	if((src[0] == 0x68) && (src[DataLen-1] == 0x16))     //初步判断包头包尾是否合规
	{
		for(i = 1; i < DataLen-2; i++)
			verify += src[i];
		if(verify == src[DataLen - 2])				//比较校验是否正确
		{
			Rcv_Lora_Pkt.src_adr = src[1];			//取出命令帧发起地址
			Rcv_Lora_Pkt.fir_relay = src[2];		//取出一级中继数值(中继数值其实为节点地址)
			Rcv_Lora_Pkt.sec_relay = src[3];		//取出二级中继数值
			Rcv_Lora_Pkt.target_adr = src[4];		//取出本帧目标地址
			Rcv_Lora_Pkt.next_step = src[5];		//取出下一步地址
			Rcv_Lora_Pkt.cmd = src[6];				//取出命令标识
			Rcv_Lora_Pkt.data_len = src[7];			//取出本帧数据长度标识
			memcpy(Rcv_Lora_Pkt.data_buf,&src[8],Rcv_Lora_Pkt.data_len);	//取出数据区内容
			Rcv_Lora_Pkt.verification = src[DataLen - 2];	//取出校验码
			return Rcv_Lora_Pkt.data_len;		//返回数据区长度
		}
		else
		{
#ifdef DBG_MOD
		printf("Lora接收数据校验出错!!!\r\n");
#endif
				//校验出错
			return 0;	
		}
	}
	else
	{
#ifdef DBG_MOD
	printf("Lora接收数据包头包尾出错!!!\r\n");
#endif
			//包头包尾不合规
	return 0;	
	}
}


/*
将解析的数据包进行分析，并根据命令执行相关动作
*/
u8 Lora_Analysis_Pkt(Lora_Pkt *Pkt)
{
	u8 tmp8 = 0 , i = 0;
	u16 tmp = 0;
	if((Pkt->fir_relay == Local_node.local_adr) && (Pkt->next_step == Local_node.local_adr))		//如果接收到的lora帧中一级地址和本地地址相同，则本节点作为一级中继转发此帧(帧数据next_step需变更为二级中继或目的地址)
	{
		if(Pkt->sec_relay != 0)			//如果lora帧中的二级地址不为空，则此帧需要二级中继转发，且此帧中的next_step需要更换为二级中继地址
		{
			printf("本节点%d 收到一级中继信息 转发至二级节点%d \r\n",Local_node.local_adr,Pkt->sec_relay);	
			Lora_Rcv_Buf[5] = Pkt->sec_relay;		//next_step更换为下级地址		Lora_Rcv_Buf[5]为一帧中NEXT STEP位置
			Lora_Rcv_Buf[Lora_Rcv_Len-2] = 0;					//倒数第二个字节为校验位  此处需重新对校验位进行计算
			for(i = 1;i<Lora_Rcv_Len-2;i++){
				Lora_Rcv_Buf[Lora_Rcv_Len-2] += Lora_Rcv_Buf[i];
			}
			LoraMess_Forwarding(Lora_Rcv_Buf,Lora_Rcv_Len);			//消息发送给下一级  阻塞方式发送直到消息发送成功(即在总线空闲状态下消息发送完成)
		}
		else if(Pkt->sec_relay == 0x00)			//如果二级中继地址为0x00 即没有二级中继
		{
		
			if((Pkt->target_adr >= 0x01) && (Pkt->target_adr < 0xFF))		
			{
				printf("本节点%d 收到一级中继信息 无二级节点 转发至目标地址%d\r\n",Local_node.local_adr,Pkt->target_adr);
				Lora_Rcv_Buf[5] = Pkt->target_adr;		//next_step更换为下级地址		
				Lora_Rcv_Buf[Lora_Rcv_Len-2] = 0;					//倒数第二个字节为校验位  此处需重新对校验位进行计算
				for(i = 1;i<Lora_Rcv_Len-2;i++){
					Lora_Rcv_Buf[Lora_Rcv_Len-2] += Lora_Rcv_Buf[i];
				}
				LoraMess_Forwarding(Lora_Rcv_Buf,Lora_Rcv_Len);			//消息发送给下一级
			}
		}
	}
	else if((Pkt->sec_relay== Local_node.local_adr) && (Pkt->next_step == Local_node.local_adr))		//若lora帧中的二级地址和本地地址相同并且next_step与本地地址相同，则本地终端作为二级中继转发此消息
	{
		if((Pkt->target_adr >= 0x01) && (Pkt->target_adr < 0xFF))		//判断目标地址合法0x02~0xFE
		{
			printf("本节点%d 收到二级中继信息 转发至目标地址%d\r\n",Local_node.local_adr,Pkt->target_adr);
			Lora_Rcv_Buf[5] = Pkt->target_adr;		//next_step更换为下级地址
			Lora_Rcv_Buf[Lora_Rcv_Len-2] = 0;					//倒数第二个字节为校验位  此处需重新对校验位进行计算
			for(i = 1;i<Lora_Rcv_Len-2;i++){
				Lora_Rcv_Buf[Lora_Rcv_Len-2] += Lora_Rcv_Buf[i];
			}
			LoraMess_Forwarding(Lora_Rcv_Buf,Lora_Rcv_Len);			//消息发送给下一级
		}
	}
	else if((Pkt->target_adr == Local_node.local_adr) && (Pkt->next_step == Local_node.local_adr))		//如果lora帧中的目标地址与本地地址相同，则本节点解析帧命令及帧数据进行相应动作			
	{
		printf("本节点%d 收到执行指令\r\n");
		//Lora_Bus_Stat = 0;
		switch(Pkt->cmd)
		{
			case G_ADDRESSING:
					printf("寻址指令\r\n");
					tmp8 = Gate_Dress_Rcd(Pkt);		//记录网关传递过来的路径信息(一级中继和二级中继地址)
					if(tmp8 == 0)
					{
						tmp = Lora_Pack_Raw(Lora_ResponseBuf,L_RESPONSE,0x01);	//组包确认消息
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
							printf("收到选中状态命令的网关回复~\r\n");
#endif									
								}
							break;
						default :
#ifdef DBG_MOD
							printf("收到无法识别的网关回应命令\r\n");
#endif
							break;
					}
				break;
			case G_QRENCODE_SHOW:
#ifdef DBG_MOD
					printf("收到网关发送来的二维码消息并准备显示：\r\n");
					printf("%s\r\n",Pkt->data_buf);
#endif			
					//Gate_QRencode_Show(Pkt->data_buf,Pkt->data_len);
					//SPB_Page = 0x02;
						tmp = Lora_Pack_Raw(Lora_ResponseBuf,L_RESPONSE,0x01);	//组包确认消息
						if(tmp)
						{
							LoraMess_Forwarding(Lora_ResponseBuf,tmp);
						}
				break;
			case G_OPENLOCK:
#ifdef DBG_MOD
			printf("收到开锁命令:\r\n");
#endif	
			tmp = Lora_Pack_Raw(Lora_ResponseBuf,L_RESPONSE,0x01);	//组包确认消息
			if(tmp)
			{
				LoraMess_Forwarding(Lora_ResponseBuf,tmp);
			}

#if 0
					g74HC595Data[0] = Rcv_Lora_Pkt.data_buf[0];
					g74HC595Data[1] |= (Rcv_Lora_Pkt.data_buf[1] & 0x03);
					DO_OUT();
					delay_ms(300);		//开锁延时300ms		
					g74HC595Data[0] = 0x00;
					g74HC595Data[1] &= 0xFC;
					DO_OUT();		//关闭所有锁(即断电)
					
#ifdef DBG_MOD
	printf("打开柜门号 %x %x\r\n",g74HC595Data[0],g74HC595Data[1]);		
#endif
#endif
			break;
			default:
#ifdef DBG_MOD
	printf("无定义的命令!!!\r\n");			
#endif
				break;
		}
	}
	else
	{
#ifdef DBG_MOD
	printf("接收到无用的lora节点信息!!!\r\n");	
#endif
	}

//memset(Lora_Rcv_Buf,0,USART2_REC_LEN);
memset(Pkt,0,sizeof(Rcv_Lora_Pkt));
	return 0;
}

#if 0
/*
组包函数
返回整组命令包的字节数据长度
cmd为终端要发送的命令码
stat为接收到指令的执行状态
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

	Lora_SendBuf[2] = Local_node.lsec_relay;		//信息返回路径一级中继和二级中继需要交换下次序
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
			Lora_SendBuf[7] = 0x02;		//数据长读为2
			Lora_SendBuf[8] = Rcv_Lora_Pkt.cmd;		//数据内容为接收到的网关命令
			Lora_SendBuf[9] = stat;			//执行接收到的命令的结果 stat为1表示接收执行正常 stat为2表是接收执行异常
			for(i=1;i<10;i++)
			{
				verify += Lora_SendBuf[i];
			}
			Lora_SendBuf[10] = verify;
			Lora_SendBuf[11] = 0x16;
			len = 12;
			break;
		case L_SELSTAT:
			Lora_SendBuf[7] = 0x02;		//数据长度为2
			Lora_SendBuf[8] = Terminal_SelStat & 0x00FF;		//取低8位 D7~D0 代表柜子的1~8格
			Lora_SendBuf[9] = Terminal_SelStat >> 8;			//取高8位 D7~D0 代表柜子的9~16格
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
组包函数
返回整组命令包的字节数据长度
cmd为终端要发送的命令码
stat为接收到指令的执行状态
*/
u16 Lora_Pack_Raw(u8 *buf,u8 cmd,u8 stat)
{
	u8 verify = 0,i = 0;
	u16 len=0;
	memset(buf,0,Lora_ResponseBuf_Len);		//暂定所有发送包的长度为12字节
	buf[0] = 0x68;
	buf[1] = Local_node.local_adr;

	//Lora_SendBuf[2] = Local_node.lfir_relay;
	//Lora_SendBuf[3] = Local_node.lsec_relay;

	buf[2] = Local_node.lsec_relay;		//信息返回路径一级中继和二级中继需要交换下次序
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
			buf[7] = 0x02;		//数据长读为2
			buf[8] = Rcv_Lora_Pkt.cmd;		//数据内容为接收到的网关命令  所以需要确保之前的Rcv_Lora_Pkt并没有被清除
			buf[9] = stat;			//执行接收到的命令的结果 stat为1表示接收执行正常 stat为2表是接收执行异常
			for(i=1;i<10;i++)
			{
				verify += buf[i];
			}
			buf[10] = verify;
			buf[11] = 0x16;
#if 0
			buf[12] = 0x00;		//连续4个0表示放开总线
			buf[13] = 0x00;
			buf[14] = 0x00;
			buf[15] = 0x00;
#endif
			len = 12;
			break;
		case L_SELSTAT:
			buf[7] = 0x02;		//数据长度为2
			buf[8] = Terminal_SelStat & 0x00FF;		//取低8位 D7~D0 代表柜子的1~8格
			buf[9] = Terminal_SelStat >> 8;			//取高8位 D7~D0 代表柜子的9~16格
			for(i=1;i<10;i++)
			{
				verify += buf[i];
			}
			buf[10] = verify;
			buf[11] = 0x16;
#if 0
			buf[12] = 0x00;		//连续4个0表示放开总线
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
lora信息发送
u8 *buf待发送区的地址指针
u16 len发送长度
u16 返回已发送字节数
*/
void LoraMess_Forwarding(u8 *buf,u16 len)
{
	u8 i = 1;

		if(1)		//若总线为空闲状态，节点可发送信息
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
						//SX1276StartRx();			//将全局变量RFLRState设置为lora接收模式
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
							USART_SendData(USART1, rcv_buf[t]);//向串口1发送数据
							while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET);//等待发送结束
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
		printf("Lora总线繁忙!!!\r\n200ms后第 %d 次尝试消息重发^^^^^^^\r\n",i);
#endif
		}
#endif
	}
}

/*
记录本地节点的路由信息及一级中继和二级中继的地址
*Pkt 为接收到网关发送过来的寻址命令数据包
存取成功返回0  存取失败返回1

另外一点:本地节点的地址和网关地址需要通过串口进行设置或者专门命令实现
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

	printf("从网关接收到寻址信息\r\n一级中继地址:%d\r\n二级中继地址:%d\r\n",Local_node.lfir_relay,Local_node.lsec_relay);
	return 0;	
}


/*
显示网关传递过来的二维码信息(不超过250字节)
u8 *buf为Rcv_Lora_Pkt结构体中的data_buf指针
u8 len 为Rcv_Lora_Pkt结构体中的data_len
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
等待网关返回确认命令
u8 cmd 
u16 timeout 等待网关回应限时
在规定时间内收到正确的应答命令 返回1 否则 返回 0
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
等待网关返回确认命令
u8 cmd 
u16 timeout 等待网关回应限时为10ms的倍数
在规定时间内收到正确的应答命令 返回1 否则 返回 0
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

							SX1276StartRx();			//将全局变量RFLRState设置为lora接收模式	
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


