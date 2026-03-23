/* Includes ------------------------------------------------------------------*/
#include "mqtt.h"

wifi_data_t wifi_data1;
wifi_data_t wifi_data2;
net_t net;
uint8_t wifi_num=0;
uint8_t mqtt_test_cnt=0;
uint8_t current_uploade_task=0;
uint16_t time_between_task=0; //间隔时间

uint8_t flag_timeout=0;
uint8_t flag_recvmqtt=0;
uint8_t flag_mqttpub=0;
uint8_t delete_type=0;

uint8_t keyword_mqtt=0;

uint8_t data_zero[255]={0};
uint8_t data_default_x[16]={0x03,0xE8,0x03,0xE8,0x03,0xE8,0x03,0xE8,0x03,0xE8,0x03,0xE8,0x03,0xE8,0x03,0xE8};

uint16_t time_updatedata=90;	//传感器数据上传周期
uint8_t  time_updatedataA[2]={0};	//传感器数据保存数组
uint16_t total_time_detection=0;	//累计消杀时间分钟，65535最多存储45天分钟数，
uint32_t total_mins_detection=0;//累计总分钟数
uint8_t  total_time_detectionA[3]={0};// time_disinfection[2]每十天加一，十天对应累计分钟数10*24*60=14400分；t+n*14400;



/***************************************************【rtc变量】****************************************************************************/
uint32_t hours=0;
uint32_t mins=0;
uint32_t sec=0;
uint8_t flag_updatetime=0;
RTC_DateTypeDef GetDate;  //获取日期结构体
RTC_TimeTypeDef GetTime;   //获取时间结构体
RTC_DateTypeDef SetDate;
RTC_TimeTypeDef SetTime;
uint32_t time_cnt=0;
uint8_t time_now[3]={0};
uint8_t rtc_time[3]={0};

