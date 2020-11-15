#include "adc.h"
#include "delay.h"

#define SCL              0x40
#define SDA              0x80

//#define SCL_SEL          P5SEL &= ~SCL
#define SCL_DDR_OUT  	 PA2OUT()//P5DIR |= SCL       // 设置为输出
//#define SCL_OUT 	 PA2OUT()
#define SCL_H 		GPIO_SetBits(GPIOA,GPIO_Pin_2)
#define SCL_L	 	GPIO_ResetBits(GPIOA,GPIO_Pin_2)

//#define SDA_SEL          P5SEL &= ~SDA
#define SDA_DDR_OUT   	 PA3OUT() //P5DIR |= SDA        // 设置为输出
#define SDA_DDR_IN               PA3IN()  //P5DIR &=  ~SDA      // 设置为输入
//#define SDA_OUT 	 PA3OUT()
#define SDA_IN                   GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3);   // 读取 输入电平
#define SDA_H    	 GPIO_SetBits(GPIOA,GPIO_Pin_3)
#define SDA_L	  	 GPIO_ResetBits(GPIOA,GPIO_Pin_3)

#define TRUE   1
#define FALSE  0
#define DA     -1

/*************ADDR Initial********************/
#define ADDRESS    ADDRESS_0   //ADDR PIN ->GND
#define ADDRESS_W  ADDRESS|0x00  //写地址
#define ADDRESS_R  ADDRESS|0x01    //读地址

/*************Config Initial*********************/
#define OS         OS_1
//#define MUX        MUX_6      //AINp=AIN0, AINn=GND
#define PGA        PGA_0     //FS=6.144V
#define DR         DR_7       //Data Rate = 860
#define COMP_QUE   COMP_QUE_3


unsigned int Config;
unsigned char Writebuff[4],Readbuff[3];
unsigned int Result[2]; 
int D_ADS; //转换的数字量
float VIN_DAS; //输入的电压值
unsigned char  t_DAS; 
int  Format[5]={0};  //转换的数字量转换成十进制

/*******************************************
函数名称：delay
功    能：延时约15us的时间
参    数：无
返回值  ：无
********************************************/
void Delay(void)
{
	delay_us(15);
}	

/******************延时1ms的时间*********************/
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
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能A端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 		 //浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  //初始化GPIOA3	
}
void PA3OUT()
{
 	GPIO_InitTypeDef  GPIO_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能A端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  //初始化GPIOA3	
}
void PA2OUT()
{
 	GPIO_InitTypeDef  GPIO_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能A端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	 
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  //初始化GPIOA3	
}
/*******************************************
函数名称：Start
功    能：完成IIC的起始条件操作
参    数：无
返回值  ：无
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
函数名称：Stop
功    能：完成IIC的终止条件操作
参    数：无
返回值  ：无
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
函数名称：ACK
功    能：完成IIC的主机应答操作
参    数：无
返回值  ：无
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
函数名称：NACK
功    能：完成IIC的主机无应答操作
参    数：无
返回值  ：无
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

/**********检查应答信号函数******************/
/*如果返回值为1则证明有应答信号，反之没有*/
/*******************************************
函数名称：Check
功    能：检查从机的应答操作
参    数：无
返回值  ：从机是否有应答：1--有，0--无
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
    slaveack = SDA_IN  //读入SDA数值
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
                
       //         P6OUT=0xf6;  // 1 4 位亮   
                 
		if(Check())
		{
			writebuffer ++;                                  
       //                 P6OUT=0x79;  // 2 8 位亮                
		}                         
		else
		{ 
                  
        //         P6OUT=0xeb;  // 3 5 位亮                 
			Stop();
			return 2 ;
		}      
	}  
      
  //    P6OUT=0x7a;  // 1 8 位亮  
      
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
	SDA_DDR_IN; //端口为输入
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
			NACK();  //不连续读字节
		else 
			ACK(); //连续读字节
	}
	Stop();

}


/*****************初始化******************/
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
     
  //   P6OUT=0x7f;  // 最高位亮
          
     Start();   //写入4个字节
   
     do
     {

          t=Write_N_Byte(Writebuff,4);
     }while(t==0);
   
  //   P6OUT=0x1f;   //  最高三位亮
}

void ReadWord(void)
{
    int t;
    Start();  //写入2个字节
    
  //  P6OUT=0xfc;  //    低两位亮  

    do
    {
      t=Write_N_Byte(Readbuff,2);
    }while(t==0);

    Start();   //写入2个字节

   // P6OUT=0xf8;  //    低三位亮   
    
    do
    {
      t=Write_N_Byte(&Readbuff[2],1);
    }while(t==0);
    
  //   P6OUT=0xf1;  //    第2—4 位 亮    
     
    Read_N_Byte(Result,2);  //读出2个字节

}

/***************ADS1115********************/
 
void ADS1115(int a)
{  


     
    InitADS1115(a);
    

  //   P6OUT=0xf0;       //低四位亮  
 
 /*************************************程序可能卡这里下方*******************/      
    
    WriteWord(); 
    
     
 //   P6OUT=0xfe;       // 最低位亮
    
    delay_ms(1);
    ReadWord();
    
  /*************************************程序可能卡这里上方*******************/     
   
    //   P6OUT=0x07;    // 高五位亮  
	
    D_ADS=Result[0]*256+Result[1];  //转换的数字量
    t_DAS=0;
    ShowInt(D_ADS);    //转换的数字量转换成十进制
    VIN_DAS=6.144*D_ADS/32768;  //输入的电压值
}
void ad1115test()
{
    WriteWord();
    Delay_1ms();
    ReadWord();
    D_ADS=Result[0]*256+Result[1];  //转换的数字量
    t_DAS=0;
    ShowInt(D_ADS);    //转换的数字量转换成十进制
    VIN_DAS=6.144*D_ADS/32768;  //输入的电压值
}
//用递归方法将正整数x转换为10进制格式
void ShowInt(long int x)
{
	if(x)
	{
          Format[t_DAS]= x%10;
          t_DAS  ++;
          ShowInt(x/10);
	}
}
void adsfilter(int b)//中位值平均滤波法   
{
  char count,i,j;
  char No=10;             //   一共采集次数  
  float value_buf[10],temp;    //  每次AD值 所存放数组
  float sum=0;
  

 
  
  for (count=0;count<No;count++)
  {
    
    Delay();
    

   
    ADS1115(b);
  

   
    
    value_buf[count]=VIN_DAS;    
  }

  

   //排序算法
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

//去掉最大最小值值取平均
  for (count=1;count<No-1;count++)
  {
    sum+=value_buf[count];
 }
  VIN_DAS=sum/(No-2);
  
// 避免悬空、接地产生12.87V电压
  if(((VIN_DAS-12.287)<=0.01)&&((12.287-VIN_DAS)<=0.01))
	VIN_DAS=0;  
  
  
  //sum=sum*1.492-35.55;//-0.0016*sum*sum;
  //sum=(uint)sum;
  

}


