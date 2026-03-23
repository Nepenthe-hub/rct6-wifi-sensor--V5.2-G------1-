/*
	串口通信：
	PMS7003:	PM1.0 PM2.5 PM10
	9600bps 返回数据长度32字节 
	指令 末尾校验字节H 校验字节L(前面字节的和)
	0x42 0x4d 0xe2 0x00 0x00 0x01 0x71	//被动式读数
	0x42 0x4d 0xe1 0x00 0x00 0x01 0x70	//被动式
	0x42 0x4d 0xe1 0x00 0x01 0x01 0x71	//主动式
	0x42 0x4d 0xe4 0x00 0x00 0x01 0x73	//待机模式
	0x42 0x4d 0xe4 0x00 0x01 0x01 0x74	//正常模式
	返回值 末尾校验字节H 校验字节L(前面字节的和)
	data[32]:42 4D 00 1C 00 1D 00 31 00 39 00 19 00 29 00 34 06 CF 05 F3 01 58 00 08 00 00 00 00 12 00 03 E8  
	PM1.0(ug/m3): data[4] data[5] (CF=1 标准颗粒物)
	PM2.5(ug/m3): data[6] data[7]
	PM10(ug/m3): 	data[8] data[9]
	PM1.0(ug/m3): data[10] data[11] (大气环境下)
	PM2.5(ug/m3): data[12] data[13]
	PM10(ug/m3): 	data[14] data[15]
	
	//PMS7003串口指令
	const uint8_t order_getdata[] 			= {0x42, 0x4d, 0xe2, 0x00, 0x00, 0x01, 0x71};//读取数据
	const uint8_t order_state_active[] 	= {0x42, 0x4d, 0xe1, 0x00, 0x01, 0x01, 0x71};//间隔发送数据
	const uint8_t order_state_passive[] = {0x42, 0x4d, 0xe1, 0x00, 0x00, 0x01, 0x70};//一问一答模式
	const uint8_t order_mode_normal[]		= {0x42, 0x4d, 0xe4, 0x00, 0x01, 0x01, 0x74};//正常状态	
	const uint8_t order_mode_sleep[]		= {0x42, 0x4d, 0xe4, 0x00, 0x00, 0x01, 0x73};//睡眠状态	
*/
#include "uart_pms7003.h"

//PMS7003串口指令
const uint8_t pms7003_order_getdata[] 			= {0x42, 0x4d, 0xe2, 0x00, 0x00, 0x01, 0x71, 0xFF};//读取数据
const uint8_t pms7003_order_state_active[] 	= {0x42, 0x4d, 0xe1, 0x00, 0x01, 0x01, 0x71, 0xFF};//间隔发送数据
const uint8_t pms7003_order_state_passive[] = {0x42, 0x4d, 0xe1, 0x00, 0x00, 0x01, 0x70, 0xFF};//一问一答模式
const uint8_t pms7003_order_mode_normal[]		= {0x42, 0x4d, 0xe4, 0x00, 0x01, 0x01, 0x74, 0xFF};//正常状态
const uint8_t pms7003_order_mode_sleep[]		= {0x42, 0x4d, 0xe4, 0x00, 0x00, 0x01, 0x73, 0xFF};//睡眠状态

pms7003_SensorData_t pms7003_SensorData;
pms_mode_t pms_mode;

//设置传感器模式

void Init_PMS7003(void)
{
	set_mode_pms7003(PMS_MODE_NORMAL);
	delay_ms(100);
	set_mode_pms7003(PSM_STATE_PASSIVE);
	delay_ms(100);
}
	
void set_mode_pms7003(uint8_t mode)
{
//	pms_mode=mode;
	switch(mode)
	{
		case PMS_GETDATA:
			HAL_UART_Transmit(&huart2, pms7003_order_getdata, sizeof(pms7003_order_getdata), 0xffff);	
			delay_ms(100);
			break;
		case PMS_STATE_ACTIVE:
			HAL_UART_Transmit(&huart2, pms7003_order_state_active, sizeof(pms7003_order_state_active), 0xffff);
			delay_ms(100);		
			break;
		case PSM_STATE_PASSIVE:		
			HAL_UART_Transmit(&huart2, pms7003_order_state_passive, sizeof(pms7003_order_state_passive), 0xffff);	
			delay_ms(100);
			break;			
		case PMS_MODE_NORMAL:	
			HAL_GPIO_WritePin(GPIOB, S_STATE_Pin, GPIO_PIN_SET);			
			HAL_UART_Transmit(&huart2, pms7003_order_mode_normal, sizeof(pms7003_order_mode_normal), 0xffff);	
			delay_ms(100);
			break;	
		case PMS_MODE_SLEEP:
//			HAL_GPIO_WritePin(GPIOB, S_STATE_Pin, GPIO_PIN_RESET);	
			HAL_UART_Transmit(&huart2, pms7003_order_mode_sleep, sizeof(pms7003_order_mode_sleep), 0xffff);		
			delay_ms(100);
		default:
			break;			
	}
	delay_ms(10);
}
void pms7003_readdata(uint16_t *data_pm1_0,uint16_t *data_pm2_5,uint16_t *data_pm10)
{
	set_mode_pms7003(PMS_GETDATA);
	
	pms7003_SensorData.pm1_0.byte[1]=rx2.data[4];
	pms7003_SensorData.pm1_0.byte[0]=rx2.data[5];
	
	pms7003_SensorData.pm2_5.byte[1]=rx2.data[6];
	pms7003_SensorData.pm2_5.byte[0]=rx2.data[7];
	
	pms7003_SensorData.pm10.byte[1]=rx2.data[8];
	pms7003_SensorData.pm10.byte[0]=rx2.data[9];

	if(pms7003_crc(rx2.data))
	{
		*data_pm1_0 = pms7003_SensorData.pm1_0.data16;
		*data_pm2_5 = pms7003_SensorData.pm2_5.data16;
		*data_pm10  = pms7003_SensorData.pm10.data16;	

//	sensor_data.sensor_16bit[PM1_0] = *data_pm1_0;
//	sensor_data.sensor_16bit[PM2_5] =	*data_pm2_5;
//	sensor_data.sensor_16bit[PM10]	=	*data_pm10;
//	
//	hmi_sethmivalue[PM2_5_H+POSADD] = sensor_data.sensor_8bit[PM2_5_L];
//	hmi_sethmivalue[PM2_5_L+POSADD] = sensor_data.sensor_8bit[PM2_5_H];
//	hmi_sethmivalue[PM10_H+POSADD]	= sensor_data.sensor_8bit[PM10_L];
//	hmi_sethmivalue[PM10_L+POSADD]  = sensor_data.sensor_8bit[PM10_H];
	}
}

bool pms7003_crc(uint8_t *data)
{
	uint8_t i=0;
	uint16_t crc_recv=0;
	uint16_t crc=0;
	for(i=0;i<30;i++)
	{
		crc+=data[i];
	}
	
	crc_recv = (data[30]<<8) | data[31];
	if(crc!=crc_recv)
	{
		if(PRINTTEST3) printf_U3("PMS7003-CRC校验失败\r\n");
		return false;
	}
	else 
	{
		if(PRINTTEST3) printf_U3("PMS7003-CRC校验成功\r\n");
		return true;
	}
}













