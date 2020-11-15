#ifndef _MLX90614_H
#define _MLX90614_H
#endif

#include "sys.h"	 
#include "stdlib.h"

void SMBus_StartBit(void);
void SMBus_StopBit(void);
u8 SMBus_SendByte(u8 Tx_buffer);
void SMBus_SendBit(u8 bit_out);
u8 SMBus_ReceiveBit(void);
u8 SMBus_ReceiveByte(u8 ack_nack);
void SMBus_Delay(u16 time);
void SMBus_Init();
u16 SMBus_ReadMemory(u8 slaveAddress, u8 command);
u8 PEC_Calculation(u8 pec[]);
float SMBus_ReadTemp(void);



