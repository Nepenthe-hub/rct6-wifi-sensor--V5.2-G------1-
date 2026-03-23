#ifndef __I2C_H__
#define __I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "tim.h"
#include <string.h>
#include <stdio.h>

#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射

#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 

#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 
#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 
#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 
#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define CHANNEL_SHT40		1
#define CHANNEL_AM2320	1
#define CHANNEL_ENS160	2
#define CHANNEL_EEPROM	3
//x:IIC channel	

#define SDA_IN(x)			do{if(x>2) SDA_IN3(); else if(x>1) SDA_IN2(); else SDA_IN1();}while(0)
#define SDA_OUT(x)		do{if(x>2) SDA_OUT3();else if(x>1) SDA_OUT2();else SDA_OUT1();}while(0)

#define IIC_SCL(x,y)	((x>2)?(IIC_SCL3=y):((x>1)?(IIC_SCL2=y):(IIC_SCL1=y ) ) )
#define IIC_SDA(x,y)	((x>2)?(IIC_SDA3=y):((x>1)?(IIC_SDA2=y):(IIC_SDA1=y ) ) )
#define READ_SDA(x)		((x>2)?(READ_SDA3): ((x>1)?(READ_SDA2): (READ_SDA1 ) ) )

//CHANNEL_SHT40 PC2 PC3
#define SDA_IN1()  do{GPIOC->CRL&=0XFFFFF0FF;GPIOC->CRL|=(uint32_t)8<<8;}while(0) 
#define SDA_OUT1() do{GPIOC->CRL&=0XFFFFF0FF;GPIOC->CRL|=(uint32_t)3<<8;}while(0) 
#define IIC_SCL1    PCout(3) //SCL PA12 
#define IIC_SDA1    PCout(2) //SDA	PA11 
#define READ_SDA1   PCin(2)  //输入SDA 

//CHANNEL_ENS160 PC0 PC1
#define SDA_IN2()  do{GPIOC->CRL&=0XFFFFFFF0;GPIOC->CRL|=(uint32_t)8<<0;}while(0) 
#define SDA_OUT2() do{GPIOC->CRL&=0XFFFFFFF0;GPIOC->CRL|=(uint32_t)3<<0;}while(0) 
#define IIC_SCL2    PCout(1) //SCL PB3 
#define IIC_SDA2    PCout(0) //SDA PB4  
#define READ_SDA2   PCin(0)  //输入SDA 

//CHANNEL_EEPROM PB7 PC6
#define SDA_IN3()  do{GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(uint32_t)8<<28;}while(0) 
#define SDA_OUT3() do{GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(uint32_t)3<<28;}while(0) 
#define IIC_SCL3    PBout(6) //SCL PC1 
#define IIC_SDA3    PBout(7) //SDA PC0 	 
#define READ_SDA3   PBin(7)  //输入SDA 

//IIC所有操作函数	 
void IIC_Init(void);

void IIC_Start(uint8_t x);				//发送IIC开始信号
void IIC_Stop(uint8_t x);	  			//发送IIC停止信号
uint8_t IIC_Wait_Ack(uint8_t x); 				//IIC等待ACK信号
void IIC_Ack(uint8_t x);					//IIC发送ACK信号
void IIC_NAck(uint8_t x);				//IIC不发送ACK信号

void IIC_Send_Byte(uint8_t txd,uint8_t x);			//IIC发送一个字节
uint8_t IIC_Read_Byte(unsigned char ack,uint8_t x);//IIC读取一个字节

/* USER CODE END Private defines */


void delay_us(uint32_t nus);
void delay_ms(uint32_t nms);	
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __I2C_H__ */
