#ifndef __I2C_ENS160_H__
#define __I2C_ENS160_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "i2c.h"
#include <string.h>
#include <stdio.h>
#include "usart.h"
#include <stdbool.h>

	
	
#define   ENS160_ADDR_W  0xA4
#define   ENS160_ADDR_R  0xA5

void Init_ENS160(void);
uint16_t Get_TVOC(void);
uint16_t Get_ECO2(void);	
void ens160_readdata(uint16_t *tvoc,uint16_t *co2);
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	

#ifdef __cplusplus
}
#endif

#endif /* __I2C_H__ */

