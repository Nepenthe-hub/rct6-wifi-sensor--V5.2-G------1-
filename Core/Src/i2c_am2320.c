#include "i2c_am2320.h"

/*唤醒传感器*/
void AM2320_Wake(void)
{
	IIC_Start(CHANNEL_AM2320);
	IIC_Send_Byte(AM2320_ADDRESS,CHANNEL_AM2320);
	IIC_Wait_Ack(CHANNEL_AM2320);
	delay_us(1000); //延时1000微秒
	IIC_Stop(CHANNEL_AM2320);
}

/**
 * @brief  读取AM2320数据
 * @param  *Hum 湿度
 * @param  *Temp 温度
 * @retval 1 - 读取成功；0 - 读取失败
 */
void am2320_readdata(uint16_t *T, uint16_t *H)
{
	uint8_t Data[8];

	IIC_Start(CHANNEL_AM2320); //发送起始信号
	IIC_Send_Byte(AM2320_ADDRESS,CHANNEL_AM2320);
	if (IIC_Wait_Ack(CHANNEL_AM2320)) //判断应答信号
	{
		IIC_Stop(CHANNEL_AM2320); //发送停止信号
		delay_us(50);
		//再尝试读取一次
		IIC_Start(CHANNEL_AM2320); //发送起始信号
		IIC_Send_Byte(AM2320_ADDRESS,CHANNEL_AM2320);
		if (IIC_Wait_Ack(CHANNEL_AM2320)) //判断应答信号
		{
			delay_us(20);
			IIC_Stop(CHANNEL_AM2320); //发送停止信号
		}
		else
		{
			delay_us(20);//这里AM2320会莫名其妙地拉低SCL一段时间导致发送的数据出错，所以要延时20微秒等这段时间过去了AM2320释放SCL再继续
			IIC_Send_Byte(0x03,CHANNEL_AM2320); //发送功能码
			IIC_Wait_Ack(CHANNEL_AM2320);				   //等待应答信号
			IIC_Send_Byte(0x00,CHANNEL_AM2320); //发送要读取的寄存器起始地址
			IIC_Wait_Ack(CHANNEL_AM2320);				   //等待应答信号
			IIC_Send_Byte(0x04,CHANNEL_AM2320); //发送要读取的寄存器长度
			IIC_Wait_Ack(CHANNEL_AM2320);				   //等待应答信号
			delay_us(20);//这里AM2320会莫名其妙地拉低SCL一段时间导致停止信号发送失败，所以延时20微秒等这段时间过去了AM2320释放SCL再继续
			IIC_Stop(CHANNEL_AM2320);		   //发送停止信号
		}
	}
	else
	{
		delay_us(20);//这里AM2320会莫名其妙地拉低SCL一段时间导致发送的数据出错，所以要延时20微秒等这段时间过去了AM2320释放SCL再继续
		IIC_Send_Byte(0x03,CHANNEL_AM2320); //发送功能码
		IIC_Wait_Ack(CHANNEL_AM2320);				   //等待应答信号
		IIC_Send_Byte(0x00,CHANNEL_AM2320); //发送要读取的寄存器起始地址
		IIC_Wait_Ack(CHANNEL_AM2320);				   //等待应答信号
		IIC_Send_Byte(0x04,CHANNEL_AM2320); //发送要读取的寄存器长度
		IIC_Wait_Ack(CHANNEL_AM2320);				   //等待应答信号
		delay_us(20);//这里AM2320会莫名其妙地拉低SCL一段时间导致停止信号发送失败，所以延时20微秒等这段时间过去了AM2320释放SCL再继续
		IIC_Stop(CHANNEL_AM2320);		   //发送停止信号
	}

	delay_ms(2); //延时2毫秒

	IIC_Start(CHANNEL_AM2320);
	IIC_Send_Byte(AM2320_ADDRESS | 0x01,CHANNEL_AM2320); //发送读取指令
	IIC_Wait_Ack(CHANNEL_AM2320);
	delay_us(35);
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		if (i != 7)
		{
			Data[i] = IIC_Read_Byte(1,CHANNEL_AM2320);
		}
		else
		{
			Data[i] = IIC_Read_Byte(0,CHANNEL_AM2320); //读取最后一个字节时发送非应答信号
		}
	}
	IIC_Stop(CHANNEL_AM2320);

	if (CRC16(Data, 6) == (Data[6] | (Data[7] << 8))) //校验数据
	{
		*H = ((((uint16_t)Data[2]) << 8) | Data[3]) / 10; //计算湿度数据
		
		if (Data[4] >> 7)									  //判断温度数值是否为负
		{
			*T = ((((uint16_t)(Data[4] & 0x7F) << 8)) | Data[5]) / -10; //计算负温度
		}
		else
		{
			*T = ((((uint16_t)Data[4]) << 8) | Data[5]) / 10; //计算正温度
		}

//		*T = (uint16_t)(Temp*data_X[0]);//修正
//		*H = (uint16_t)(Hum*data_X[1]);
//		*T = (uint16_t)(Temp);
//		*H = (uint16_t)(Hum);		
		
//		sensor_data.sensor_16bit[TEMP]=*T;
//		sensor_data.sensor_16bit[HUMI]=*H;
		
//		hmi_sethmivalue[TEMP_H+POSADD] = sensor_data.sensor_8bit[TEMP_L];
//		hmi_sethmivalue[TEMP_L+POSADD] = sensor_data.sensor_8bit[TEMP_H];
//		hmi_sethmivalue[HUMI_H+POSADD] = sensor_data.sensor_8bit[HUMI_L];
//		hmi_sethmivalue[HUMI_L+POSADD] = sensor_data.sensor_8bit[HUMI_H];	
		
		if(PRINTTEST1)	printf_U3("温度=%d ℃, 湿度=%d %%\r\n",*T,*H);		
	}
}

unsigned short CRC16(unsigned char *ptr, unsigned char len)
{
	unsigned short crc = 0xFFFF;
	unsigned char i;
	while (len--)
	{
		crc ^= *ptr++;
		for (i = 0; i < 8; i++)
		{
			if (crc & 0x01)
			{
				crc >>= 1;
				crc ^= 0xA001;
			}
			else
			{
				crc >>= 1;
			}
		}
	}
	return crc;
}



























