#include "adc.h"
#include "delay.h"

#define SCL              0x40
#define SDA              0x80

//#define SCL_SEL          P5SEL &= ~SCL
#define SCL_DDR_OUT  	 PA2OUT()//P5DIR |= SCL       // ����Ϊ���
//#define SCL_OUT 	 PA2OUT()
#define SCL_H 		GPIO_SetBits(GPIOA,GPIO_Pin_2)
#define SCL_L	 	GPIO_ResetBits(GPIOA,GPIO_Pin_2)

//#define SDA_SEL          P5SEL &= ~SDA
#define SDA_DDR_OUT   	 PA3OUT() //P5DIR |= SDA        // ����Ϊ���
#define SDA_DDR_IN               PA3IN()  //P5DIR &=  ~SDA      // ����Ϊ����
//#define SDA_OUT 	 PA3OUT()
#define SDA_IN                   GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3);   // ��ȡ �����ƽ
#define SDA_H    	 GPIO_SetBits(GPIOA,GPIO_Pin_3)
#define SDA_L	  	 GPIO_ResetBits(GPIOA,GPIO_Pin_3)

#define TRUE   1
#define FALSE  0
#define DA     -1

/*************ADDR Initial********************/
#define ADDRESS    ADDRESS_0   //ADDR PIN ->GND
#define ADDRESS_W  ADDRESS|0x00  //д��ַ
#define ADDRESS_R  ADDRESS|0x01    //����ַ

/*************Config Initial*********************/
#define OS         OS_1
//#define MUX        MUX_6      //AINp=AIN0, AINn=GND
#define PGA        PGA_0     //FS=6.144V
#define DR         DR_7       //Data Rate = 860
#define COMP_QUE   COMP_QUE_3


unsigned int Config;
unsigned char Writebuff[4],Readbuff[3];
unsigned int Result[2]; 
int D_ADS; //ת����������
float VIN_DAS; //����ĵ�ѹֵ
unsigned char  t_DAS; 
int  Format[5]={0};  //ת����������ת����ʮ����

/*******************************************
�������ƣ�delay
��    �ܣ���ʱԼ15us��ʱ��
��    ������
����ֵ  ����
********************************************/
void Delay(void)
{
	delay_us(15);
}	

/******************��ʱ1ms��ʱ��*********************/
void Delay_1ms(void)
{
	delay_ms(1);
}
void __no_operation()
{
	delay_us(5);	
}
void PA3IN()
{
 	GPIO_InitTypeDef  GPIO_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��A�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 //��������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  //��ʼ��GPIOA3	
}
void PA3OUT()
{
 	GPIO_InitTypeDef  GPIO_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��A�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  //��ʼ��GPIOA3	
}
void PA2OUT()
{
 	GPIO_InitTypeDef  GPIO_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��A�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  //��ʼ��GPIOA3	
}
/*******************************************
�������ƣ�Start
��    �ܣ����IIC����ʼ��������
��    ������
����ֵ  ����
********************************************/
void Start(void)
{
        SCL_H;
        SDA_H;
        Delay();
	SDA_L;
        Delay();
	SCL_L;
        Delay();
}

/*******************************************
�������ƣ�Stop
��    �ܣ����IIC����ֹ��������
��    ������
����ֵ  ����
********************************************/
void Stop(void)
{ 
        SDA_L;
        Delay();
        SCL_H;
        Delay();
	SDA_H;
        Delay();
}


/*******************************************
�������ƣ�ACK
��    �ܣ����IIC������Ӧ�����
��    ������
����ֵ  ����
********************************************/
void ACK(void)
{
        SDA_L;
        __no_operation();__no_operation();
        SCL_H;
        Delay();
        SCL_L;
	__no_operation();__no_operation();
	SDA_H;
        Delay();
}

/*******************************************
�������ƣ�NACK
��    �ܣ����IIC��������Ӧ�����
��    ������
����ֵ  ����
********************************************/
void NACK(void)
{
	SDA_H;
	__no_operation();__no_operation();
	SCL_H;
	Delay();
	SCL_L;
	__no_operation();__no_operation();
	SDA_L;   
	Delay();       
}

/**********���Ӧ���źź���******************/
/*�������ֵΪ1��֤����Ӧ���źţ���֮û��*/
/*******************************************
�������ƣ�Check
��    �ܣ����ӻ���Ӧ�����
��    ������
����ֵ  ���ӻ��Ƿ���Ӧ��1--�У�0--��
********************************************/
unsigned char Check(void)
{
    unsigned char slaveack;
    
    SDA_H;
   __no_operation();__no_operation();
    SCL_H;
   __no_operation();__no_operation();
    SDA_DDR_IN;
   __no_operation();__no_operation();
    slaveack = SDA_IN  //����SDA��ֵ
    SCL_L;
    Delay();
    SDA_DDR_OUT;
    if(slaveack)    return FALSE;
    else            return TRUE;
}

/***************Write a Byte****************/
void Write_1_Byte(unsigned char DataByte)
{
	int i;

	for(i=0;i<8;i++)
	{
		if(DataByte&0x80)  //if((DataByte<<i)&0x80)
		  SDA_H;
		else
		  SDA_L;
        Delay();
		SCL_H;
        Delay();
        SCL_L;
        Delay();
        DataByte <<= 1;
	}
        SDA_H;
       __no_operation();__no_operation();
}

/***************Write N Byte****************/
unsigned char Write_N_Byte(unsigned char *writebuffer,unsigned char n)
{
	int i;
        

    
	for(i=0;i<n;i++)
	{
		Write_1_Byte(*writebuffer);  
                
       //         P6OUT=0xf6;  // 1 4 λ��   
                 
		if(Check())
		{
			writebuffer ++;                                  
       //                 P6OUT=0x79;  // 2 8 λ��                
		}                         
		else
		{ 
                  
        //         P6OUT=0xeb;  // 3 5 λ��                 
			Stop();
			return 2 ;
		}      
	}  
      
  //    P6OUT=0x7a;  // 1 8 λ��  
      
	Stop();
	return TRUE;
}

/***************Read a Byte****************/
unsigned char Read_1_Byte(void)
{
    unsigned char data=0,FLAG, i;
	
    for(i=0;i<8;i++)
    {
        
	SDA_H;
	Delay();
        SCL_H;
	SDA_DDR_IN; //�˿�Ϊ����
	Delay();
        FLAG=SDA_IN;
        data <<= 1;
	if( FLAG)
	   data |= 0x01;
	SDA_DDR_OUT;
        SCL_L;
        Delay();
    }
    return data;
}

/***************Read N Byte****************/
void Read_N_Byte(unsigned int*readbuff,unsigned char n)
{
	unsigned char i;
	
	for(i=0;i<n;i++)
	{
		readbuff[i]=Read_1_Byte();
		if(i==n-1)
			NACK();  //���������ֽ�
		else 
			ACK(); //�������ֽ�
	}
	Stop();

}


/*****************��ʼ��******************/
void InitADS1115(int x)
{
    int MU;
  //  SCL_SEL;
   // SDA_SEL;
    SCL_DDR_OUT;
    SDA_DDR_OUT;
    SDA_H;
    switch(x)
    {
    case 0:MU=0x4000;break;
    case 1:MU=0x5000;break;
    case 2:MU=0x6000;break;
    case 3:MU=0x7000;break;
    }
    Config = OS+MU+PGA+DR+COMP_QUE;
    Writebuff[0]=ADDRESS_W;
    Writebuff[1]=Pointer_1;
    Writebuff[2]=Config/256;
    Writebuff[3]=Config%256;
        
    Readbuff[0]=ADDRESS_W;
    Readbuff[1]=Pointer_0;
        
    Readbuff[2]=ADDRESS_R;

}

/***************Write a Word***********************/
void WriteWord(void)
{
     int t=0;
     
  //   P6OUT=0x7f;  // ���λ��
          
     Start();   //д��4���ֽ�
   
     do
     {

          t=Write_N_Byte(Writebuff,4);
     }while(t==0);
   
  //   P6OUT=0x1f;   //  �����λ��
}

void ReadWord(void)
{
    int t;
    Start();  //д��2���ֽ�
    
  //  P6OUT=0xfc;  //    ����λ��  

    do
    {
      t=Write_N_Byte(Readbuff,2);
    }while(t==0);

    Start();   //д��2���ֽ�

   // P6OUT=0xf8;  //    ����λ��   
    
    do
    {
      t=Write_N_Byte(&Readbuff[2],1);
    }while(t==0);
    
  //   P6OUT=0xf1;  //    ��2��4 λ ��    
     
    Read_N_Byte(Result,2);  //����2���ֽ�

}

/***************ADS1115********************/
 
void ADS1115(int a)
{  


     
    InitADS1115(a);
    

  //   P6OUT=0xf0;       //����λ��  
 
 /*************************************������ܿ������·�*******************/      
    
    WriteWord(); 
    
     
 //   P6OUT=0xfe;       // ���λ��
    
    delay_ms(1);
    ReadWord();
    
  /*************************************������ܿ������Ϸ�*******************/     
   
    //   P6OUT=0x07;    // ����λ��  
	
    D_ADS=Result[0]*256+Result[1];  //ת����������
    t_DAS=0;
    ShowInt(D_ADS);    //ת����������ת����ʮ����
    VIN_DAS=6.144*D_ADS/32768;  //����ĵ�ѹֵ
}
void ad1115test()
{
    WriteWord();
    Delay_1ms();
    ReadWord();
    D_ADS=Result[0]*256+Result[1];  //ת����������
    t_DAS=0;
    ShowInt(D_ADS);    //ת����������ת����ʮ����
    VIN_DAS=6.144*D_ADS/32768;  //����ĵ�ѹֵ
}
//�õݹ鷽����������xת��Ϊ10���Ƹ�ʽ
void ShowInt(long int x)
{
	if(x)
	{
          Format[t_DAS]= x%10;
          t_DAS  ++;
          ShowInt(x/10);
	}
}
void adsfilter(int b)//��λֵƽ���˲���   
{
  char count,i,j;
  char No=10;             //   һ���ɼ�����  
  float value_buf[10],temp;    //  ÿ��ADֵ ���������
  float sum=0;
  

 
  
  for (count=0;count<No;count++)
  {
    
    Delay();
    

   
    ADS1115(b);
  

   
    
    value_buf[count]=VIN_DAS;    
  }

  

   //�����㷨
  for (j=0;j<No;j++)
  {
    for (i=0;i<No-j;i++)
    {
      if (value_buf[i]>value_buf[i+1])
      {
        temp=value_buf[i];
        value_buf[i]=value_buf[i+1];
        value_buf[i+1]=temp;
      }      
    }
  }

//ȥ�������Сֵֵȡƽ��
  for (count=1;count<No-1;count++)
  {
    sum+=value_buf[count];
 }
  VIN_DAS=sum/(No-2);
  
// �������ա��ӵز���12.87V��ѹ
  if(((VIN_DAS-12.287)<=0.01)&&((12.287-VIN_DAS)<=0.01))
	VIN_DAS=0;  
  
  
  //sum=sum*1.492-35.55;//-0.0016*sum*sum;
  //sum=(uint)sum;
  

}


