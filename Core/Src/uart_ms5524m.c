/*
	MS5524M: eCO2(±100ppm) TVOC(±75ppb)  HCHO(±30ppb)
	9600bps 返回数据长度32字节 
	指令
	0x42 0x78 0x03 0x00 0x00 0x00 0x00 0x00 0xFF	//主动发送模式
	0x42 0x78 0x04 0x00 0x00 0x00 0x00 0x00 0xFF	//被动发送模式
	指令返回值
	0x42 0x78 0x01 0x00 0x00 0x00 0x00 0x00 0xFF	//发送成功
	0x42 0x78 0x00 0x00 0x00 0x00 0x00 0x00 0xFF	//发送失败
	被动模式返回值(一问一答) 
	发送指令
	0x42 0x86 0x04 0x00 0x00 0x00 0x00 0x00 0xFF	//被动发送模式
	返回值
	data[17]: 42 4D 00 03 50 04 D3 04 F2 04 27 00 8C 00 31 29 8E
	eCO2(ppm):  data[9]*256  + data[10]
	TVOC(ppb):  data[11]*256 + data[12]
	HCHO(ppb):  data[13]*256 + data[14]
*/

#include "uart_ms5524m.h"
//PMS7003串口指令
const uint8_t ms5524_order_getdata[] 			 = {0x42, 0x86, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};//读取数据
const uint8_t ms5524_order_state_active[]  = {0x42, 0x78, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};//间隔发送数据
const uint8_t ms5524_order_state_passive[] = {0x42 ,0x78, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};//一问一答模式

ms5524_SensorData_t ms5524_SensorData;

uint8_t crc_data[15]={0};
void Init_MS5524(void)
{
	HAL_UART_Transmit(&huart4, ms5524_order_state_passive, sizeof(ms5524_order_state_passive), 0xffff);	
	delay_ms(200);
}

void ms5524_readdata(uint16_t *data_eco2,uint16_t *data_tvoc,uint16_t *data_hcho)
{	
	uint8_t crc_data[13]={0};
	
	HAL_UART_Transmit(&huart4, ms5524_order_getdata, sizeof(ms5524_order_getdata), 0xffff);	
	delay_ms(200);
	
	ms5524_SensorData.eco2.byte[1]=rx4.data[9];
	ms5524_SensorData.eco2.byte[0]=rx4.data[10];
	
	ms5524_SensorData.tvoc.byte[1]=rx4.data[11];
	ms5524_SensorData.tvoc.byte[0]=rx4.data[12];
	
	ms5524_SensorData.hcho.byte[1]=rx4.data[13];
	ms5524_SensorData.hcho.byte[0]=rx4.data[14];	

	if(CRC_MS5524(rx4.data,13))
	{
		*data_hcho = (uint16_t)(ms5524_SensorData.hcho.data16*1.23);	
		if(USE_MS5524)
		{		
			*data_eco2 = ms5524_SensorData.eco2.data16;
			*data_tvoc = (uint16_t)(ms5524_SensorData.tvoc.data16*4.1);		
		}
	}
	if(PRINTTEST2)
	{
		printf_U3("ms5524:tvoc=%d, co2=%d, hcho=%d\r\n",ms5524_SensorData.tvoc.data16,ms5524_SensorData.eco2.data16,ms5524_SensorData.hcho.data16);
	}
}

//校验函数(42 4d不参与校验)
bool CRC_MS5524(uint8_t *arr_buff,uint8_t len)
{
	uint16_t crc=0xFFFF;
	uint8_t i,j;
	uint16_t crc_recv=0;
	for(j=0;j<len;j++)
	{
		crc ^= arr_buff[j+2]; 
		for(i=0;i<8;i++)
		{
			if(crc&0x0001)
			{
				crc = (crc >> 1) ^ 0xA001;
			}
			else
				crc >>= 1;
		}
	}
	crc_recv = arr_buff[15] | (arr_buff[16]<<8);
	
	if(crc!=crc_recv)
	{
		if(PRINTTEST3) printf_U3("MS5524M-CRC校验失败\r\n");
		return false;
	}
	else 
	{
		if(PRINTTEST3) printf_U3("MS5524M-CRC校验成功\r\n");
		return true;
	}
}












