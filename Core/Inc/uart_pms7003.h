/*
	串口通信：
	PMS7003:	PM1.0 PM2.5 PM10
	9600bps 返回数据长度32字节 
	指令 末尾校验字节H 校验字节L(前面字节的和)
	0x42 0x4d 0xe2 0x00 0x00 0x01 0x71	//被动式读数
	0x42 0x4d 0xe1 0x00 0x00 0x01 0x70	//被动式状态
	0x42 0x4d 0xe1 0x00 0x01 0x01 0x71	//主动式状态
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
*/

#ifndef __UART_PMS7003_H__
#define __UART_PMS7003_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdio.h>
#include "usart.h"
#include "i2c.h"
	
typedef union
{
  uint16_t data16;
  uint8_t byte[2];
} pms7003_uint16_Bytes_t; // Make it easy to convert 2 x uint8_t to uint16_t

typedef struct
{
	pms7003_uint16_Bytes_t pm1_0;
	pms7003_uint16_Bytes_t pm2_5;
	pms7003_uint16_Bytes_t pm10;
}pms7003_SensorData_t;
extern pms7003_SensorData_t pms7003_SensorData;
	
typedef enum
{
	PMS_GETDATA,
	PMS_STATE_ACTIVE,
	PSM_STATE_PASSIVE,
	PMS_MODE_NORMAL,
	PMS_MODE_SLEEP
}pms_mode_t;
extern pms_mode_t pms_mode;
	
	
void Init_PMS7003(void);	
void set_mode_pms7003(uint8_t mode);
void pms7003_readdata(uint16_t *data_pm1_0,uint16_t *data_pm2_5,uint16_t *data_pm10);
bool pms7003_crc(uint8_t *data);

#ifdef __cplusplus
}
#endif

#endif /* __I2C_H__ */




