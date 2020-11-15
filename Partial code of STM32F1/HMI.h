#ifndef __HMI_H
#define __HMI_H
#include "stdio.h"	
#include "sys.h" 



#define HMI_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_HMI_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����

#define KEY0 PBout(12)	// DS0
#define KEY1 PGout(9)	// DS1	
	  	
extern u8  HMI_RX_BUF[HMI_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 HMI_RX_STA;         		//����״̬���	
//����봮���жϽ��գ��벻Ҫע�����º궨��
void hmi_init(u32 bound);
void HMI_SendNum(USART_TypeDef* USARTx,u8 *data,u8 len,u16 Num,u8 len2);
void HMI_SendText(USART_TypeDef* USARTx,u8 *data,u8 len,u8 *text,u8 len2);
void HMISendstart(void);
void HMISendb(u8 k);	
#endif
