/* 【Includes】 ------------------------------------------------------------------*/
#include "i2c.h"

/* 【function】 ------------------------------------------------------------------*/
void delay_us(uint32_t nus)
{		
	uint32_t temp;	    	 
	SysTick->LOAD=nus*9; 				//时间加载	  		 
	SysTick->VAL=0x00;        				//清空计数器
	SysTick->CTRL=0x01 ;      				//开始倒数 	 
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//等待时间到达   
	SysTick->CTRL=0x00;      	 			//关闭计数器
	SysTick->VAL =0X00;       				//清空计数器	 
}
void delay_ms(uint32_t nms)
{	 		  	  
	uint32_t temp;		   
	SysTick->LOAD=(uint32_t)nms*9000;//时间加载(SysTick->LOAD为24bit)
	SysTick->VAL =0x00;           //清空计数器
	SysTick->CTRL=0x01 ;          //开始倒数  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));//等待时间到达   
	SysTick->CTRL=0x00;       //关闭计数器
	SysTick->VAL =0X00;       //清空计数器	  	    
} 

/* 【Ini】 ------------------------------------------------------------------*/
//初始化IIC
void IIC_Init(void)
{					     
  GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	//IIC1
	GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_SET);
	
	//IIC2
	GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_SET);

	//IIC3
	GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_SET);
}

/* IIC ------------------------------------------------------------------*/
//产生IIC起始信号
void IIC_Start(uint8_t x)
{
	SDA_OUT(x);     //sda线输出
	IIC_SDA(x,1);	  	  
	IIC_SCL(x,1);
	delay_us(4);
	IIC_SDA(x,0);//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL(x,0);//钳住I2C总线，准备发送或接收数据 

}	  
//产生IIC停止信号
void IIC_Stop(uint8_t x)
{
	SDA_OUT(x);//sda线输出
	IIC_SCL(x,0);
	IIC_SDA(x,0);//STOP:when CLK is high DATA change form low to high
	delay_us(4);
	IIC_SCL(x,1); 
	delay_us(4);	
	IIC_SDA(x,1);//发送I2C总线结束信号
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t IIC_Wait_Ack(uint8_t x)
{
	uint8_t ucErrTime=0;
	SDA_IN(x);      //SDA设置为输入  
	IIC_SDA(x,1);delay_us(1);	   
	IIC_SCL(x,1);delay_us(1);	 
	while(READ_SDA(x))
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop(x);
			return 1;
		}
	}
	IIC_SCL(x,0);//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void IIC_Ack(uint8_t x)
{
	IIC_SCL(x,0);
	SDA_OUT(x);
	IIC_SDA(x,0);
	delay_us(2);
	IIC_SCL(x,1);
	delay_us(2);
	IIC_SCL(x,0);
}
//不产生ACK应答		    
void IIC_NAck(uint8_t x)
{
	IIC_SCL(x,0);
	SDA_OUT(x);
	IIC_SDA(x,1);
	delay_us(2);
	IIC_SCL(x,1);
	delay_us(2);
	IIC_SCL(x,0);
}					 				     
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(uint8_t txd,uint8_t x)
{                        
  uint8_t t;   
	SDA_OUT(x); 	    
	IIC_SCL(x,0);//拉低时钟开始数据传输
	for(t=0;t<8;t++)
	{              
			//IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC_SDA(x,1);
		else
			IIC_SDA(x,0);
		txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL(x,1);
		delay_us(2); 
		IIC_SCL(x,0);	
		delay_us(2);
	}	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t IIC_Read_Byte(unsigned char ack,uint8_t x)
{
	unsigned char i,receive=0;
	SDA_IN(x);//SDA设置为输入
   for(i=0;i<8;i++ )
	{
		IIC_SCL(x,0); 
		delay_us(2);
		IIC_SCL(x,1);
		receive<<=1;
		if(READ_SDA(x))receive++;   
		delay_us(1); 
   }					 
	if (!ack)
			IIC_NAck(x);//发送nACK
	else
			IIC_Ack(x); //发送ACK   
	return receive;
}

