#ifndef __HMI_H
#define __HMI_H
#include "stdio.h"	
#include "sys.h" 



#define HMI_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_HMI_RX 			1		//使能（1）/禁止（0）串口1接收

#define KEY0 PBout(12)	// DS0
#define KEY1 PGout(9)	// DS1	
	  	
extern u8  HMI_RX_BUF[HMI_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 HMI_RX_STA;         		//接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义
void hmi_init(u32 bound);
void HMI_SendNum(USART_TypeDef* USARTx,u8 *data,u8 len,u16 Num,u8 len2);
void HMI_SendText(USART_TypeDef* USARTx,u8 *data,u8 len,u8 *text,u8 len2);
void HMISendstart(void);
void HMISendb(u8 k);	
#endif
