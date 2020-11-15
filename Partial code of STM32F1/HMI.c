#include "sys.h"
#include "hmi.h"	  
#include "delay.h"
#include "led.h"

////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif

 
 u32 time=47;
 u16 deg_flag=0;
 u16 enable=0;
 
#if EN_HMI_RX   //如果使能了接收
//串口1中断服务程序
//注意,读取USARTx->SR能避免莫名其妙的错误   	
u8 HMI_RX_BUF[HMI_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
u16 HMI_RX_STA=0;       //接收状态标记	  
  
void hmi_init(u32 bound){
  //GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能USART2，GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA.9
   
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIO  

  //Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  USART_Init(USART3, &USART_InitStructure); //初始化串口1
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
  USART_Cmd(USART3, ENABLE);                    //使能串口1 

}

void USART3_IRQHandler(void)                	//串口1中断服务程序
	{
	u8 Res;
#if SYSTEM_SUPPORT_OS 		//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
	OSIntEnter();    
#endif
	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
		Res =USART_ReceiveData(USART3);	//读取接收到的数据
		
		if((HMI_RX_STA&0x8000)==0)//接收未完成
			{
			
				if(Res==0xff)		//判断结束标志，可删除
				{
					if(Res==0xff)
					{
						if(Res==0xff)
						{
							 HMI_RX_STA|=0x8000;	//接收完成了 
						}
						else HMI_RX_STA=0;//接收错误,重新开始
					}
					else HMI_RX_STA=0;//接收错误,重新开始
				}
				
				else if(Res==0x22)//自动测量
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
						HMI_RX_STA+=1;  								//接收位置加一
						if(HMI_RX_STA>(HMI_REC_LEN-1))HMI_RX_STA=0;//接收数据错误,重新开始接收	  
					}		 
				}
			}   		 

#if SYSTEM_SUPPORT_OS 	//如果SYSTEM_SUPPORT_OS为真，则需要支持OS.
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



//10的乘方函数，用于判断位数
u32 HMI_Pow(u8 n)
{
	u32 result=1;	 
	while(n--)result*=10;    
	return result;
}	


//数值发送函数
//USART_TypeDef* USARTx： 选择串口输出，注意必须先初始化、使能串口
//u16 *data：上位机上数字位相应的改变数值代码
//u8 len：*data代码的长度。
//u16 Num：需要显示的数值，注意开头0不显示
//u8 len2：需要显示的数值的位数
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

//字符串发送函数：HMI_SendText(USART_TypeDef* USARTx,u8 *data,u8 len,u8 *text,u8 len2)
//            USART_TypeDef* USARTx： 选择串口输出，注意必须先初始化、使能该串口
//            u16 *data：上位机上文本位相应的改变数值代码，如"t0.txt="、"t1.txt="
//          	u8 len：*data代码的长度。如"t0.txt="长度为7。
//            u8 *text：需要串口输出显示的文本
//            u8 len2：需要显示的文本的长度，英文一个字符一字节、汉字一个字两个字节    

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

void HMISendb(u8 k)		         //字节发送函数
{		 
	u8 i;
	 for(i=0;i<3;i++)
	 {
	 if(k!=0)
	 	{
			USART_SendData(USART3,k);  //发送一个字节
			while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET){};//等待发送结束
		}
	 else 
	 return ;

	 } 
} 
