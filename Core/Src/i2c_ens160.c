#include "i2c_ens160.h"

void Init_ENS160(void)
{
	IIC_Start(CHANNEL_ENS160);
	IIC_Send_Byte(ENS160_ADDR_W,CHANNEL_ENS160);
	IIC_Wait_Ack(CHANNEL_ENS160);
	IIC_Send_Byte(0x10,CHANNEL_ENS160);
	IIC_Wait_Ack(CHANNEL_ENS160);
	IIC_Send_Byte(0x02,CHANNEL_ENS160);
	IIC_Wait_Ack(CHANNEL_ENS160);
	IIC_Stop(CHANNEL_ENS160);
}


uint16_t Get_TVOC(void)
{
	uint16_t voc=0;
	uint8_t tempvoc1=0;
	uint8_t tempvoc2=0;

	
	IIC_Start(CHANNEL_ENS160);
	IIC_Send_Byte(ENS160_ADDR_W,CHANNEL_ENS160);
	IIC_Wait_Ack(CHANNEL_ENS160);
	IIC_Send_Byte(0x22,CHANNEL_ENS160);
	IIC_Wait_Ack(CHANNEL_ENS160);
	IIC_Stop(CHANNEL_ENS160);
	delay_us(10);
	IIC_Start(CHANNEL_ENS160);
	IIC_Send_Byte(ENS160_ADDR_R,CHANNEL_ENS160);
	IIC_Wait_Ack(CHANNEL_ENS160);
	tempvoc1 = IIC_Read_Byte(1,CHANNEL_ENS160);
	tempvoc2 = IIC_Read_Byte(0,CHANNEL_ENS160);
	IIC_NAck(CHANNEL_ENS160);
	voc = tempvoc2<<8;
	voc |= tempvoc1;
	return voc;
}

uint16_t Get_ECO2(void)
{
	uint16_t voc=0;
	uint8_t tempvoc1=0;
	uint8_t tempvoc2=0;

	
	IIC_Start(CHANNEL_ENS160);
	IIC_Send_Byte(ENS160_ADDR_W,CHANNEL_ENS160);
	IIC_Wait_Ack(CHANNEL_ENS160);
	IIC_Send_Byte(0x24,CHANNEL_ENS160);
	IIC_Wait_Ack(CHANNEL_ENS160);
	IIC_Stop(CHANNEL_ENS160);
	delay_us(10);
	IIC_Start(CHANNEL_ENS160);
	IIC_Send_Byte(ENS160_ADDR_R,CHANNEL_ENS160);
	IIC_Wait_Ack(CHANNEL_ENS160);
	tempvoc1 = IIC_Read_Byte(1,CHANNEL_ENS160);
	tempvoc2 = IIC_Read_Byte(0,CHANNEL_ENS160);
	IIC_NAck(CHANNEL_ENS160);
	voc = tempvoc2<<8;
	voc |= tempvoc1;
	return voc;
}

void ens160_readdata(uint16_t *tvoc,uint16_t *co2)
{
	if(USE_ENS160)
	{	
		uint8_t buffer[4]={0};

		IIC_Start(CHANNEL_ENS160);
		IIC_Send_Byte(ENS160_ADDR_W,CHANNEL_ENS160);
		IIC_Wait_Ack(CHANNEL_ENS160);
		IIC_Send_Byte(0x22,CHANNEL_ENS160);
		IIC_Wait_Ack(CHANNEL_ENS160);
		IIC_Stop(CHANNEL_ENS160);
		delay_us(10);
		IIC_Start(CHANNEL_ENS160);
		IIC_Send_Byte(ENS160_ADDR_R,CHANNEL_ENS160);
		IIC_Wait_Ack(CHANNEL_ENS160);
		buffer[0] = IIC_Read_Byte(1,CHANNEL_ENS160);
		buffer[1] = IIC_Read_Byte(1,CHANNEL_ENS160);
		buffer[2] = IIC_Read_Byte(1,CHANNEL_ENS160);
		buffer[3] = IIC_Read_Byte(0,CHANNEL_ENS160);	
		IIC_Stop(CHANNEL_ENS160);
	
		*tvoc = (uint16_t)((buffer[1]<<8|buffer[0])*4.1);
		*co2  = buffer[3]<<8|buffer[2];
		
		if(PRINTTEST2)
		{
			printf_U3("ens160:tvoc=%d, co2=%d\r\n",*tvoc,*co2);
		}					
//		if(voc==0) voc=1;
		
//		*tvoc=(uint16_t)(voc*4.1*data_X[6]);	//ÐÞÕýÏµÊý
//		*co2=(uint16_t)((*co2)*data_X[5]);			
//		*tvoc = (uint16_t)(voc*4.1);	
//		*co2  = (*co2);			
		
				
//		sensor_data.sensor_16bit[TVOC] = *tvoc;
//		sensor_data.sensor_16bit[CO2]  = *co2;				
//		hmi_sethmivalue[TVOC_H+POSADD]	= sensor_data.sensor_8bit[TVOC_L];
//		hmi_sethmivalue[TVOC_L+POSADD]  = sensor_data.sensor_8bit[TVOC_H];	
//		hmi_sethmivalue[CO2_H+POSADD]		= sensor_data.sensor_8bit[CO2_L];
//		hmi_sethmivalue[CO2_L+POSADD]   = sensor_data.sensor_8bit[CO2_H];			
	}
}













