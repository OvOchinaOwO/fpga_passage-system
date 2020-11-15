
#ifndef _1115_H_
#define _1115_H_

/**********ADDR宏定义***************/
#define ADDRESS_0  0x90   //ADDR PIN ->GND
#define ADDRESS_1  0x92   //ADDR PIN ->VDD
#define ADDRESS_2  0x94   //ADDR PIN ->SDA
#define ADDRESS_3  0x96   //ADDR PIN ->SCL



/************POINTER REGISTER*****************/
#define Pointer_0  0x00   //Convertion register
#define Pointer_1  0x01   //Config register
#define Pointer_2  0x02   //Lo_thresh register
#define Pointer_3  0x03   //Hi_thresh register

/************CONFIG REGISTER*****************/
#define OS_0   0x0000
#define OS_1   0x8000

#define MUX_0  0x0000  //AINp=AIN0, AINn=AIN1
#define MUX_1  0x1000  //AINp=AIN0, AINn=AIN3
#define MUX_2  0x2000  //AINp=AIN1, AINn=AIN3
#define MUX_3  0x3000  //AINp=AIN2, AINn=AIN3
#define MUX_4  0x4000  //AINp=AIN0 , AINn=GND
#define MUX_5  0x5000  //AINp=AIN1, AINn=GND
#define MUX_6  0x6000  //AINp=AIN2, AINn=GND
#define MUX_7  0x7000  //AINp=AIN3, AINn=GND

#define PGA_0  0x0000   //FS=6.144V
#define PGA_1  0x0200   //FS=4.096V
#define PGA_2  0x0400  //FS=2.048V
#define PGA_3  0x0600  //FS=1.024V
#define PGA_4  0x0800  //FS=0.512V
#define PGA_5  0x0A00  //FS=0.256V
#define PGA_6  0x0C00  //FS=0.256V
#define PGA_7  0x0E00  //FS=0.256V

#define MODE_0 0x0000
#define MODE_1 0x0100

#define DR_0   0x0000   //Data Rate = 8
#define DR_1   0x0020   //Data Rate = 16
#define DR_2   0x0040   //Data Rate = 32
#define DR_3   0x0060   //Data Rate = 64
#define DR_4   0x0080   //Data Rate = 128
#define DR_5   0x00A0   //Data Rate = 250
#define DR_6   0x00C0   //Data Rate = 475
#define DR_7   0x00E0   //Data Rate = 860

#define COMP_MODE_0  0x0000
#define COMP_MODE_1  0x0010

#define COMP_POL_0   0x0000
#define COMP_POL_1   0x0008

#define COMP_LAT_0   0x0000
#define COMP_LAT_1   0x0040

#define COMP_QUE_0   0x0000
#define COMP_QUE_1   0x0001
#define COMP_QUE_2   0x0002
#define COMP_QUE_3   0x0003
void __no_operation(void);
void PA2OUT(void);
void PA3IN(void);
void PA3OUT(void);
void Delay(void);
void Delay_1ms(void);
void Start(void);
void Stop(void);
void ACK(void);
void NACK(void);
unsigned char Check(void);
void Write_1_Byte(unsigned char DataByte);
unsigned char Write_N_Byte(unsigned char *writebuffer,unsigned char n);
unsigned char Read_1_Byte(void);
void Read_N_Byte(unsigned int *readbuff,unsigned char n);
void InitADS1115(int x);
void WriteWord(void);
void ReadWord(void);
void ADS1115(int a);
void ShowInt(long int x);
void adsfilter(int b);
extern unsigned int Config;
extern unsigned char Writebuff[4],Readbuff[3];
extern unsigned int Result[2]; 
extern int D_ADS; //转换的数字量
extern float VIN_DAS; //输入的电压值
extern unsigned char  t_DAS; 
extern int  Format[5];  //转换的数字量转换成十进制


#endif /* 1115_H_ */
