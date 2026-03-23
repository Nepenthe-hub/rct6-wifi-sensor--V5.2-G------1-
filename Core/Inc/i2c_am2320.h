#ifndef __I2C_AM2320_H__
#define __I2C_AM2320_H__

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

#define AM2320_ADDRESS 0xB8


void AM2320_Wake(void);
void am2320_readdata(uint16_t *T, uint16_t *H);
unsigned short CRC16(unsigned char *ptr, unsigned char len);	
	


#ifdef __cplusplus
}
#endif

#endif /* __I2C_H__ */

