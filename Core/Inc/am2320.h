/*
 * AM2320.h
 *
 *  Created on: Jul 28, 2021
 *      Author: 39569
 */

#ifndef __AM2320_H
#define __AM2320_H

#include "stm32f1xx_hal.h"
// pin
#define DHT_PORT GPIOC
#define DHT_PIN GPIO_PIN_2
// error codes
#define DHT_ERR_RESPONSE_FAILED 1
#define DHT_ERR_READ_BIT_FAILED 2
#define DHT_ERR_DATA_CHKSUM_FAILED 3
// function prototypes
void delay_us(uint32_t nus);
void delay_ms(uint32_t nms);
void AM2320_Init(); //initialization function
void ONE_WIRE_Start(); //Start communication
uint8_t ONE_WIRE_Response(); //Sensor response for start command
void AM2320_THMsg(uint16_t Temp, uint16_t Humi, char THMsg[30]); //generate message string
uint8_t AM2320_Get_Data(uint16_t *temp,uint16_t *humi); //function available for user to use
uint64_t AM2320_ReadData(void); // read original data
//void delay_us(uint32_t us);
#endif /* INC_AM2320_H_ */

