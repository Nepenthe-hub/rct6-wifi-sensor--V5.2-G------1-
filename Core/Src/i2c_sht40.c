#include "i2c_sht40.h"

void sht40_readdata(uint16_t* T,uint16_t* H )
{
	uint8_t buffer[6] = {0};
	float SHT40_DATA[2]={0};
	
	//开启温度转换
	IIC_Start(CHANNEL_SHT40);														
	IIC_Send_Byte(0x88,CHANNEL_SHT40);	//0x44 <<1 +0
	delay_us(2);
	while(IIC_Wait_Ack (CHANNEL_SHT40)!=0){}; 
	IIC_Send_Byte(0xFD,CHANNEL_SHT40);
	while(IIC_Wait_Ack (CHANNEL_SHT40)!=0){}; 
	IIC_Stop(CHANNEL_SHT40);
		
	delay_ms(10);
	//读取温度
	IIC_Start(CHANNEL_SHT40);
	IIC_Send_Byte (0x89,CHANNEL_SHT40);	//0x44 <<1 +1
	delay_us(2);
	while(IIC_Wait_Ack(CHANNEL_SHT40)!=0){}; 
	buffer[0] = IIC_Read_Byte(1,CHANNEL_SHT40); //read data and ack
	buffer[1] = IIC_Read_Byte(1,CHANNEL_SHT40);
	buffer[2] = IIC_Read_Byte(1,CHANNEL_SHT40);
	buffer[3] = IIC_Read_Byte(1,CHANNEL_SHT40);
	buffer[4] = IIC_Read_Byte(1,CHANNEL_SHT40);
	buffer[5] = IIC_Read_Byte(0,CHANNEL_SHT40);				

//	if(PRINTTEST2) printf_U3("%x %x %x %x %x %x \r\n",buffer[0],buffer[1],buffer[02],buffer[3],buffer[4],buffer[5]);	
		if(PRINTTEST2) printf_U3("SHT40:%x %x\r\n",buffer[0],buffer[1]);			
	IIC_Stop(CHANNEL_SHT40);
						
	if(sht40_crc8(buffer))
	{
		SHT40_DATA[0] =buffer[0]<<8|buffer[1];			
		*T = (-45 + 175*(SHT40_DATA[0])/65535.0);		
		
		SHT40_DATA[1] =buffer[3]<<8|buffer[4];
		*H = (-6 + 125 *(SHT40_DATA[1])/65535.0);
	
//		sensor_data.sensor_16bit[TEMP]=*T;
//		sensor_data.sensor_16bit[HUMI]=*H;
//		
//		hmi_sethmivalue[TEMP_H+POSADD] = sensor_data.sensor_8bit[TEMP_L];
//		hmi_sethmivalue[TEMP_L+POSADD] = sensor_data.sensor_8bit[TEMP_H];
//		hmi_sethmivalue[HUMI_H+POSADD] = sensor_data.sensor_8bit[HUMI_L];
//		hmi_sethmivalue[HUMI_L+POSADD] = sensor_data.sensor_8bit[HUMI_H];
	}
}
bool sht40_crc8(const uint8_t *data)
{
	const uint8_t POLYNOMIAL = 0x31;
	uint8_t crc = 0xFF;
	int i, j;

	for (i=0; i<2; i++) 
	{
		crc ^= data[i];
		for (j=0; j<8; j++) 
		{
			crc = ( crc & 0x80 )? (crc << 1) ^ POLYNOMIAL: (crc <<= 1);
		}
	}
	
	if(crc!=data[2]) 
	{
		if(PRINTTEST3) printf_U3("SHT40-CRC校验失败\r\n");
		return false;
	}
	else 
	{
		if(PRINTTEST3) printf_U3("SHT40-CRC校验成功\r\n");
		return true;
	}
}
