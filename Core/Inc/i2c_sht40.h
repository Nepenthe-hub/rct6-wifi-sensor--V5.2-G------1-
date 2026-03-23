#ifndef __I2C_SHT40_H__
#define __I2C_SHT40_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdio.h>
#include "i2c.h"
#include "usart.h"

void sht40_readdata(uint16_t* T,uint16_t* H );	
bool sht40_crc8(const uint8_t *data);
	
	
	
	
	
	
	
	
	
	
	
	
	
#ifdef __cplusplus
}
#endif

#endif /* __I2C_H__ */

