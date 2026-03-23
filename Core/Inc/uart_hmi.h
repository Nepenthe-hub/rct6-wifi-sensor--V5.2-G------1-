#ifndef __UART_HMI_H__
#define __UART_HMI_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "usart.h"
#include "i2c.h"
#include "mqtt.h"
#include "i2c_eeprom.h"
#include "wifi.h"	

#define LED_SLEEP 0x05
#define LED_WORK  0x60

#define ADDR_AIRLEVEL 	0x1080
#define ADDR_ROOMNUM 		0x1210
#define ADDR_WIFIICON 	0x1200
#define ADDR_CONNECTION 0x1350
#define ADDR_HMIRESET 	0x0004
#define ADDR_SNESORDATA 0x1150

enum
{
	HMI_SENSOR_DATA,
	HMI_INTERFACE,
//	HMI_WIFI_ICON,
//	HMI_ROOM,
//	HMI_AIR_LEVEL,
//	HMI_CONNECT
};	
extern uint16_t hmi_value_data1;
extern uint16_t hmi_value_data2;

typedef enum
{
	HMI_IDLE,
	NOTE_WIFI_SSID,
	NOTE_WIFI_PASSWORD,
	CONNECT_WIFI,
	CANCEL_CONNECT,
	NOTE_ROOM_NUMBER,
	SET_ROOM_NUMBERR,
	HMI_SYSTEM_RESET
}hmi_order_t;	

typedef struct
{
	char ssid[18];
	char password[18];
	uint8_t length_ssid;
	uint8_t length_password;
}hmi_wifi_t;
extern hmi_wifi_t hmi_wifi[2];

extern hmi_order_t hmi_order;
extern uint8_t flag_cancel_connect;
extern uint8_t flag_hmi_connect_wifi;
extern uint8_t flag_hmi_save_room_num;
extern uint8_t flag_hmi_rec;

extern uint8_t hmi_setvalue[20];
extern uint8_t room_number[4];

void update_hmi_interface(uint16_t addr);
void update_hmi_sensordata(void);
void hmi_task(uint8_t item,uint16_t addr);
void get_hmi_value(void);
void air_quality_level(uint16_t pm);
void ini_icon(void);
void system_reset(void);
void hmi_data_order(void);
void hmi_connect_wifi(void);
void save_room_number(void);
void set_brightness(uint8_t brightness);
void wifi_icon(bool status);





#ifdef __cplusplus
}
#endif

#endif /* __I2C_H__ */


