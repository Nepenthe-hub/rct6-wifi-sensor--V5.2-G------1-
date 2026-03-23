/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "ens160.h"

/* USER CODE BEGIN 0 */
#include "am2320.h"
#include "string.h"

/* USER CODE END 0 */


/* USER CODE BEGIN 1 */

//初始化IIC
void IIC_Init(void)
{					     
  GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOC_CLK_ENABLE();

	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_SET);
}
//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	delay_us(4);	
	IIC_SDA=1;//发送I2C总线结束信号
						   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	IIC_SDA=1;delay_us(1);	   
	IIC_SCL=1;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}
//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	SDA_OUT(); 	    
	IIC_SCL=0;//拉低时钟开始数据传输
	for(t=0;t<8;t++)
	{              
			//IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC_SDA=1;
		else
			IIC_SDA=0;
		txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
	}	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}


void Init_ENS160_sof(void)
{
	IIC_Init();
	IIC_Start();
	IIC_Send_Byte(ENS160_ADDR_W);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x10);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x02);
	IIC_Wait_Ack();
	IIC_Stop();
}


uint16_t Get_TVOC(void)
{
	uint16_t voc=0;
	uint8_t tempvoc1=0;
	uint8_t tempvoc2=0;

	
	IIC_Start();
	IIC_Send_Byte(ENS160_ADDR_W);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x22);
	IIC_Wait_Ack();
	IIC_Stop();
	delay_us(10);
	IIC_Start();
	IIC_Send_Byte(ENS160_ADDR_R);
	IIC_Wait_Ack();
	tempvoc1 = IIC_Read_Byte(1);
	tempvoc2 = IIC_Read_Byte(0);
	IIC_NAck();
	voc = tempvoc2<<8;
	voc |= tempvoc1;
	return voc;
}

uint16_t Get_ECO2(void)
{
	uint16_t voc=0;
	uint8_t tempvoc1=0;
	uint8_t tempvoc2=0;

	
	IIC_Start();
	IIC_Send_Byte(ENS160_ADDR_W);
	IIC_Wait_Ack();
	IIC_Send_Byte(0x24);
	IIC_Wait_Ack();
	IIC_Stop();
	delay_us(10);
	IIC_Start();
	IIC_Send_Byte(ENS160_ADDR_R);
	IIC_Wait_Ack();
	tempvoc1 = IIC_Read_Byte(1);
	tempvoc2 = IIC_Read_Byte(0);
	IIC_NAck();
	voc = tempvoc2<<8;
	voc |= tempvoc1;
	return voc;
}

/* USER CODE END 1 */
