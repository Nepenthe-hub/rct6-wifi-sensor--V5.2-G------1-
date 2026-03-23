#ifndef __I2C_EEPROM_H__
#define __I2C_EEPROM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* USER CODE BEGIN Includes */
#include "tim.h"
#include "i2c.h"
#include <string.h>
#include <stdio.h>
#include "usart.h"
#include "uart_hmi.h"
#include "wifi.h"	

#define AT24CXX_ADDR_WRITE  0xA0    // 写命令
#define AT24CXX_ADDR_READ   0xA1    // 读命令
 
#define AT24C1		127
#define AT24C2		255
#define AT24C4		511
#define AT24C8		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64		8189
#define AT24C128	16383
#define AT24C256	32767
#define EE_TYPE		AT24C2
 
#define BufferSize 	AT24C02//芯片型号
  
// 根据BufferSize宏的值来设置I2C_MEMADD_SIZE
#if BufferSize == AT24C64  
    #define I2C_MEMADD_SIZE I2C_MEMADD_SIZE_16BIT  
#elif BufferSize == AT24C02  
    #define I2C_MEMADD_SIZE I2C_MEMADD_SIZE_8BIT  
#endif

uint8_t At24c02_Read_Byte(uint16_t ReadAddr);
void At24c02_Write_Byte(uint16_t WriteAddr,uint8_t DataToWrite);
uint8_t At24c02_Write_Amount_Byte(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite);
uint8_t At24c02_Read_Amount_Byte(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead);

bool read_eeprom(uint16_t addr);

//EEPROM数据存储地址
#define ADDR_WIFI_FLAG		5
#define ADDR_UPLOAD_TIME	10  //[10][11]
#define ADDR_TOTAL_TIME		13	//[13][14]
#define ADDR_CORRECT_X		17	//[17][18][19][20][21][22][23][24]-[32]
#define ADDR_ROOM_NUMBER	34

#define ADDR_SSID1 40
#define ADDR_PASSWORD1 60
#define ADDR_SSID2 80
#define ADDR_PASSWORD2 100

#ifdef __cplusplus
}
#endif

#endif /* __I2C_H__ */
