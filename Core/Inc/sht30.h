#ifndef __SHT30_H__
#define __SHT30_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "ens160.h"

#define SDA1_IN()  {GPIOC->CRL&=0XFFFFF0FF;GPIOC->CRL|=(uint32_t)8<<8;}
#define SDA1_OUT() {GPIOC->CRL&=0XFFFFF0FF;GPIOC->CRL|=(uint32_t)3<<8;}

//IO操作函数	 
#define IIC_SCL1    PCout(3) //SCL
#define IIC_SDA1    PCout(2) //SDA	 
#define READ_SDA1   PCin(2)  //输入SDA 

void IIC1_Init(void);                //初始化IIC的IO口				 
void IIC1_Start(void);				//发送IIC开始信号
void IIC1_Stop(void);	  			//发送IIC停止信号
void IIC1_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t IIC1_Read_B(unsigned char ack);//IIC读取一个字节
uint8_t IIC1_Wait_Ack(void); 				//IIC等待ACK信号
void IIC1_Ack(void);					//IIC发送ACK信号
void IIC1_NAck(void);				//IIC不发送ACK信号

void IIC1_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC1_Read_One_Byte(uint8_t daddr,uint8_t addr);
uint32_t  IIC1_Read_Byte(uint8_t ack,uint8_t* recvdata,uint8_t num);

uint8_t SHT40_ReadPdata(uint8_t ack,uint8_t *arr,uint16_t* T,uint16_t* H );
extern uint32_t SHT45_Data1[2];
void SHT30_Init(void);


/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __I2C_H__ */

