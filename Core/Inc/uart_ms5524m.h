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

#ifndef __UART_MS5524M_H__
#define __UART_MS5524M_H__

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
} ms5524_uint16_Bytes_t; // Make it easy to convert 2 x uint8_t to uint16_t

typedef struct
{
	ms5524_uint16_Bytes_t eco2;
	ms5524_uint16_Bytes_t tvoc;
	ms5524_uint16_Bytes_t hcho;
}ms5524_SensorData_t;
extern ms5524_SensorData_t ms5524_SensorData;

	
void Init_MS5524(void);	
void ms5524_readdata(uint16_t *data_eco2,uint16_t *data_tvoc,uint16_t *data_hcho);
bool CRC_MS5524(uint8_t *arr_buff,uint8_t len);

	
	
	
	
	
	
	
	

#ifdef __cplusplus
}
#endif

#endif /* __I2C_H__ */


