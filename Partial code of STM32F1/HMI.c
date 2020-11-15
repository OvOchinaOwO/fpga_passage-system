#include "sys.h"
#include "hmi.h"	  
#include "delay.h"
#include "led.h"

////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif

 
 u32 time=47;
 u16 deg_flag=0;
 u16 enable=0;
 
#if EN_HMI_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 HMI_RX_BUF[HMI_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 HMI_RX_STA=0;       //����״̬���	  
  
void hmi_init(u32 bound){
  //GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ��USART2��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO  

  //Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART3, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���1 

}

void USART3_IRQHandler(void)                	//����1�жϷ������
	{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntEnter();    
#endif
	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		{
		Res =USART_ReceiveData(USART3);	//��ȡ���յ�������
		
		if((HMI_RX_STA&0x8000)==0)//����δ���
			{
			
				if(Res==0xff)		//�жϽ�����־����ɾ��
				{
					if(Res==0xff)
					{
						if(Res==0xff)
						{
							 HMI_RX_STA|=0x8000;	//��������� 
						}
						else HMI_RX_STA=0;//���մ���,���¿�ʼ
					}
					else HMI_RX_STA=0;//���մ���,���¿�ʼ
				}
				
				else if(Res==0x22)//�Զ�����
				{
					enable=1;
				}
				else if(Res==0x18)//0x75
				{
					time+=80;
					if(time>=495)
					{
						deg_flag=!deg_flag;
						time=3;
					
					}
					else if(time<=3)
					{
						deg_flag=!deg_flag;
						time=495;
			
					}
					delay_ms(50);
									
					
				}
				else if(Res==0x08)
				{
					time-=80;
					if(time>=495)
					{
						deg_flag=!deg_flag;
						time=3;
					
					}
					else if(time<=3)
					{
						deg_flag=!deg_flag;
						time=495;
			
					}
					delay_ms(50);
					
				}
				else if(Res==0x12)
				{
					time+=20;
					if(time>=495)
					{
						deg_flag=!deg_flag;
						time=3;
					
					}
					else if(time<=3)
					{
						deg_flag=!deg_flag;
						time=495;
			
					}
					delay_ms(50);
					
				}
				else if(Res==0x02)
				{
					time-=20;
					if(time>=495)
					{
						deg_flag=!deg_flag;
						time=3;
					
					}
					else if(time<=3)
					{
						deg_flag=!deg_flag;
						time=495;
			
					}
					delay_ms(50);
					
				}
				else if(Res==0x15)
				{
					time+=5;
					if(time>=495)
					{
						deg_flag=!deg_flag;
						time=3;
					
					}
					else if(time<=3)
					{
						deg_flag=!deg_flag;
						time=495;
			
					}
					delay_ms(50);
					
					
				}
				else if(Res==0x05)
				{
					time-=5;
					if(time>=495)
					{
						deg_flag=!deg_flag;
						time=3;
					
					}
					else if(time<=3)
					{
						deg_flag=!deg_flag;
						time=495;
			
					}
					delay_ms(50);
					
				}
				else if(Res==0x11)
				{
					time+=1;
					if(time>=495)
					{
						deg_flag=!deg_flag;
						time=3;
					
					}
					else if(time<=3)
					{
						deg_flag=!deg_flag;
						time=495;
			
					}
					delay_ms(50);
					
					
				}
				else if(Res==0x01)
				{
					time-=1;
					if(time>=495)
					{
						deg_flag=!deg_flag;
						time=3;
					
					}
					else if(time<=3)
					{
						deg_flag=!deg_flag;
						time=495;
			
					}
					delay_ms(50);
					
				}
				else	{
						printf("%c",Res);
						HMI_RX_BUF[HMI_RX_STA&0X3FFF]=Res;
						HMI_RX_STA+=1;  								//����λ�ü�һ
						if(HMI_RX_STA>(HMI_REC_LEN-1))HMI_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
					}		 
				}
			}   		 

#if SYSTEM_SUPPORT_OS 	//���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS.
	OSIntExit();  											 
#endif
} 
#endif	

void HMISendstart(void)
	{
	 	delay_ms(250);
		HMISendb(0xff);
		delay_ms(200);
	}



//10�ĳ˷������������ж�λ��
u32 HMI_Pow(u8 n)
{
	u32 result=1;	 
	while(n--)result*=10;    
	return result;
}	


//��ֵ���ͺ���
//USART_TypeDef* USARTx�� ѡ�񴮿������ע������ȳ�ʼ����ʹ�ܴ���
//u16 *data����λ��������λ��Ӧ�ĸı���ֵ����
//u8 len��*data����ĳ��ȡ�
//u16 Num����Ҫ��ʾ����ֵ��ע�⿪ͷ0����ʾ
//u8 len2����Ҫ��ʾ����ֵ��λ��
void HMI_SendNum(USART_TypeDef* USARTx,u8 *data,u8 len,u16 Num,u8 len2)
{
		u8 t;
 for(t=0;t<len;t++)
        {	
            while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);
            USART_SendData(USARTx,data[t]); 					
        }
for(t=0;t<len2;t++)	
{
           while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);
           USART_SendData(USARTx,'0'+(Num/HMI_Pow(len2-t-1))%10); 


}
				
            while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);
            USART_SendData(USARTx,0XFF);
              
       	
            while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);
            USART_SendData(USARTx,0XFF);
             
       	
            while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);
            USART_SendData(USARTx,0XFF);

}

//�ַ������ͺ�����HMI_SendText(USART_TypeDef* USARTx,u8 *data,u8 len,u8 *text,u8 len2)
//            USART_TypeDef* USARTx�� ѡ�񴮿������ע������ȳ�ʼ����ʹ�ܸô���
//            u16 *data����λ�����ı�λ��Ӧ�ĸı���ֵ���룬��"t0.txt="��"t1.txt="
//          	u8 len��*data����ĳ��ȡ���"t0.txt="����Ϊ7��
//            u8 *text����Ҫ���������ʾ���ı�
//            u8 len2����Ҫ��ʾ���ı��ĳ��ȣ�Ӣ��һ���ַ�һ�ֽڡ�����һ���������ֽ�    

void HMI_SendText(USART_TypeDef* USARTx,u8 *data,u8 len,u8 *text,u8 len2)
{
		u8 t;
    for(t=0;t<len;t++)
        {	
            while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);
            USART_SendData(USARTx,data[t]); 					
        }
				    while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);
            USART_SendData(USARTx,0X22); 
   for(t=0;t<len2;t++)	
       {
           while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);
           USART_SendData(USARTx,text[t]); 


       }
			 			while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);
            USART_SendData(USARTx,0X22); 
				
            while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);
            USART_SendData(USARTx,0XFF);
              
       	
            while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);
            USART_SendData(USARTx,0XFF);
             
       	
            while(USART_GetFlagStatus(USARTx,USART_FLAG_TC)!=SET);
            USART_SendData(USARTx,0XFF);




}

void HMISendb(u8 k)		         //�ֽڷ��ͺ���
{		 
	u8 i;
	 for(i=0;i<3;i++)
	 {
	 if(k!=0)
	 	{
			USART_SendData(USART3,k);  //����һ���ֽ�
			while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET){};//�ȴ����ͽ���
		}
	 else 
	 return ;

	 } 
} 
