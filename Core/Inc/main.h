/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h> 
#include "string.h"
#include <stdlib.h>
#include "stm32f1xx_it.h"

#include "wifi.h"	
#include "mqtt.h"	
	
	
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SDA_Pin GPIO_PIN_0
#define SDA_GPIO_Port GPIOC
#define LED1_Pin GPIO_PIN_1
#define LED1_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_2
#define LED2_GPIO_Port GPIOB
#define KEY1_Pin GPIO_PIN_12
#define KEY1_GPIO_Port GPIOB
#define KEY1_EXTI_IRQn EXTI15_10_IRQn
#define S_STATE_Pin GPIO_PIN_13
#define S_STATE_GPIO_Port GPIOB
#define S_RST_Pin GPIO_PIN_14
#define S_RST_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

#define USE_SHT40		1
#define USE_AM2320	0
#define USE_ENS160	0
#define USE_MS5524	1


#define DAYS	14400

#define BUFF_SIZE	255
typedef struct
{
	uint8_t buff[BUFF_SIZE];	//串口接收用
	uint8_t data[BUFF_SIZE];	//复制接收的串口数据，后期处理用
	char		cbuff[BUFF_SIZE];	//字符格式
	char		recvbuff[BUFF_SIZE];	//字符格式
	uint8_t flag;						  //串口接收标志
	uint8_t size;							//串口接收数据长度
}rx_data;
extern rx_data rx1;
extern rx_data rx2;
extern rx_data rx3;
extern rx_data rx4;
extern rx_data rx5;


enum
{
	TIME_WAITSMART=0,	//手动配网等待时间
	TIME_IWDG,				//看门狗复位周期
	TIME_UPLOADDATA,	//传感器数据上传周期
	TIME_UPLOADTIME,	//累计监测时间上传周期
	TIME_MQTTTEST,		//网络测试周期
};
extern uint16_t time_task[5];


typedef enum
{
	IDLE,
	HMI_UPDATE,
	HMI_RECV,
	WIFI_UPDATE,
	WIFI_RECV,
	SENSOR_DECTECT,
	DATA_SAVE
}TASK_STATUS;
extern TASK_STATUS current_task;

#define POSADD 		6
enum
{
	TEMP_H=0,
	TEMP_L,
	HUMI_H,
	HUMI_L,
	PM1_0_H,
	PM1_0_L,	
	PM2_5_H,
	PM2_5_L,
	PM10_H,
	PM10_L,
	CO2_H,
	CO2_L,
	TVOC_H,
	TVOC_L,
	HCHO_H,
	HCHO_L
};
enum
{
	TEMP=0,
	HUMI,
	PM1_0,
	PM2_5,
	PM10,
	CO2,
	TVOC,
	HCHO	
};

typedef union
{
	uint8_t sensor_8bit[16];
	uint16_t sensor_16bit[8];
}sensor_databit_t;
extern sensor_databit_t sensor_databit;

extern uint16_t value_sensor[8];
extern uint16_t value_sensorX[8];
extern float value_X[8];
extern uint8_t data_X1000[16];
extern uint16_t cnt_timer[6];

extern uint8_t open_air[8];
extern uint8_t close_air[8];
extern uint8_t inc_temperature[8];
extern uint8_t dec_temperature[8];

extern const uint8_t ir_control_order[][8];

void read_sensor(void);
void stm32_reset(void) ;
void data_copy_int(uint8_t *data1,uint8_t *data2,uint16_t Size);
void data_copy_char(uint8_t *data1,char *data2,uint16_t Size);
void Write_Buff(char *aimbuff,char *data);

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
