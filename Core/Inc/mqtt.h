/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MQTT_H__
#define __MQTT_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"
#include <stdbool.h>
#include "wifi.h"
#include "usart.h"
#include "i2c_eeprom.h"
#include "rtc.h"
#include "uart_hmi.h"

#define DATA_NULL 			0
#define DATA_CORRECTING 1
#define DATA_UPLOADTIME 2
#define DATA_CLEAR 			3
#define DATA_SYSTEMRST 	4

#define DELETE_ALL 	1
#define DELETE_X 		2
#define DELETE_TIME 3

#define UPLOADTASK_MQTTTEST 	1
#define UPLOADTASK_ALLTIME 		2
#define UPLOADTASK_SENSORDATA	4
#define UPLOADTASK_ROOMNUM		8

typedef struct{
	uint8_t ssid[20];			//存储WIFI名称
	uint8_t password[20]; //存储WIFI密码
	uint8_t slength;			//存储WIFI名称长度
	uint8_t plength;			//存储WIFI密码长度
	uint8_t used;					//使用中
}wifi_data_t;


extern wifi_data_t wifi_data1;
extern wifi_data_t wifi_data2;
extern uint8_t wifi_num;
extern uint8_t mqtt_test_cnt;
extern uint8_t current_uploade_task;
extern uint16_t time_between_task;
extern uint8_t flag_mqttpub;
extern uint8_t flag_recvmqtt;
extern uint8_t flag_timeout;

extern uint16_t time_updatedata;	
extern uint8_t  time_updatedataA[2];	
extern uint16_t total_time_detection;
extern uint32_t total_mins_detection;
extern uint8_t  total_time_detectionA[3];

extern uint8_t data_zero[255];

typedef struct{
	bool wifi_status;	
	bool mqtt_status; 
}net_t;
extern net_t net;

#ifdef __cplusplus
}
#endif

#endif /* __DMA_H__ */
