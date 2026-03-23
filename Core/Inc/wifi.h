#ifndef __WIFI_H__
#define __WIFI_H__

#ifdef __cplusplus
extern "C" {
#endif


/***********************************************Ӧ**********************************************************************/
#define YES 1
#define NO  0 

#define OK 			1
#define ERROR 	2
#define SSID 		3
#define GOTIP 	4
#define RECV 		5

#define DATA_CORRECTING 1
#define DATA_UPLOAD 2
#define DATA_CLEAR 3

// Protocol Constants
#define PKT_HEADER1   0xAA
#define PKT_HEADER2   0xBB
#define PKT_MIN_LEN   5  // Header(2) + Type(1) + Len(1) + Checksum(1)

// Packet Types
#define PKT_TYPE_SENSOR 0x01
#define PKT_TYPE_ROOM   0x02
#define PKT_TYPE_WIFI   0x03
#define PKT_TYPE_STATUS 0x81
#define PKT_TYPE_TIME   0x82

#pragma pack(1)
typedef struct {
    uint16_t temp;
    uint16_t humi;
    uint16_t pm25;
    uint16_t pm10;
    uint16_t hcho;
    uint16_t co2;
    uint16_t tvoc;
} sensor_packet_t;

typedef struct {
    uint8_t room[4];
} room_num_packet_t;

typedef struct {
    char ssid[32];
    char pwd[64];
} wifi_config_packet_t;

typedef struct {
    uint8_t wifi_status;
    uint8_t mqtt_status;
} status_packet_t;

typedef struct {
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} time_sync_packet_t;
#pragma pack()

void wifi_send_packet(uint8_t type, const uint8_t *data, uint8_t len);
void wifi_process_rx(const uint8_t *data, uint16_t size);



//2025-03-28
#define PRINTTEST1 1
#define PRINTTEST2 1
#define PRINTTEST3 0

#define EEPROM_RST 0
#define MachineNumber	AIR08//ѡ���豸���
#define AIR	001
#define CWJAP					"AT+CWJAP=\"GZJG\",\"88888888\"\r\n"
//#define CWJAP					"AT+CWJAP=\"JSJCJG\",\"tc123456789\"\r\n"

#define FLAG_WIFICONN 		0	//WIFI���ӱ�־
#define FLAG_RST 					1	//wifi��λ��־
#define FLAG_CWMODE 			2	//esp32ģʽ���ñ�־
#define FLAG_CWJAP 				3	//WIFI�ɹ����������־

#define FLAG_STARTSMART 	4	//�ֶ�������־
#define FLAG_MQTTUSERCFG 	5	//MQTT�û����ñ�־
#define FLAG_MQTTCONN 		6	//MQTT���ӱ�־
#define FLAG_MQTTSUB 			7	//MQTT���ı�־
#define FLAG_MQTTPUB 			8	//MQTT�ϴ���־
#define FLAG_RESTORE 			9	//MQTT�ϴ���־

#define FLAG_MQTTANS 			10	//MQTT�ϴ���־
#define FLAG_MQTTSTATE 		11	//MQTT�ϴ���־

/***********************************************�豸���-��Ӧλ��*********************************************************************************/



#define DATA_TEMP		0
#define DATA_HUMI		1
#define DATA_PM1_0	2
#define DATA_PM2_5	3
#define DATA_PM10		4
#define DATA_CO2		5
#define DATA_TVOC		6
#define DATA_HCHO		7



//#define SERIAL_Machine(x) x

#define AIR01	201
#define AIR02	202
#define AIR03	203
#define AIR04	204
#define AIR05	205
#define AIR06	206
#define AIR07	207
#define AIR08	208
#define AIR09	209
#define AIR10	210

#define AIR11	211
#define AIR12	212
#define AIR13	213//��һ��
#define AIR14	214
#define AIR15	215
#define AIR16	216
#define AIR17	217
#define AIR18	218

#define AIR30	219
#define AIR31	220

/*********************************ÿ���������Ӧ�ķ�����������Կ*********************************/
#if   MachineNumber == AIR  
    #define MQTTUSERCFG		"AT+MQTTUSERCFG=0,1,\"wd63pxpq\",\"6nq0cinworrvbbef\",\"dQX0D9qvfg\",0,0,\"\"\r\n"		
#elif   MachineNumber == AIR01  
    #define MQTTUSERCFG		"AT+MQTTUSERCFG=0,1,\"ceii4bc8\",\"mch35ukvza8358v4\",\"dQX0D9qvfg\",0,0,\"\"\r\n"
#elif MachineNumber == AIR02  
    #define MQTTUSERCFG		"AT+MQTTUSERCFG=0,1,\"xkuf8qb0\",\"1phb795ej2v8cgfp\",\"dQX0D9qvfg\",0,0,\"\"\r\n"
#elif MachineNumber == AIR03  
    #define MQTTUSERCFG		"AT+MQTTUSERCFG=0,1,\"z33qs3zw\",\"y9r33d266jghjgi2\",\"dQX0D9qvfg\",0,0,\"\"\r\n"	
#elif MachineNumber == AIR04  
    #define MQTTUSERCFG		"AT+MQTTUSERCFG=0,1,\"tv58dbmi\",\"szzwecjk5w4akz10\",\"dQX0D9qvfg\",0,0,\"\"\r\n"	
#elif MachineNumber == AIR04  
    #define MQTTUSERCFG		"AT+MQTTUSERCFG=0,1,\"ieswrel9\",\"uf20ikr20wpt5p62\",\"dQX0D9qvfg\",0,0,\"\"\r\n"	
#elif MachineNumber == AIR05  
    #define MQTTUSERCFG		"AT+MQTTUSERCFG=0,1,\"mqf9bl8a\",\"xpwftzbfuip1a2qk\",\"dQX0D9qvfg\",0,0,\"\"\r\n"			
#elif MachineNumber == AIR06  
    #define MQTTUSERCFG		"AT+MQTTUSERCFG=0,1,\"5l5snl7s\",\"sg401my0cbm5d2qp\",\"dQX0D9qvfg\",0,0,\"\"\r\n"	
#elif MachineNumber == AIR07 
    #define MQTTUSERCFG		"AT+MQTTUSERCFG=0,1,\"6wv5n1ld\",\"91ppwlvk46gch233\",\"dQX0D9qvfg\",0,0,\"\"\r\n"	
#elif MachineNumber == AIR08  
    #define MQTTUSERCFG		"AT+MQTTUSERCFG=0,1,\"4ck8xb3z\",\"7efgrvax9rr5gs1t\",\"dQX0D9qvfg\",0,0,\"\"\r\n"	
#elif MachineNumber == AIR09  
    #define MQTTUSERCFG		"AT+MQTTUSERCFG=0,1,\"e30wkarw\",\"qzdzrstptvf8f2mp\",\"dQX0D9qvfg\",0,0,\"\"\r\n"	
#elif MachineNumber == AIR10  
    #define MQTTUSERCFG		"AT+MQTTUSERCFG=0,1,\"hphlkya0\",\"sqszp1xasbq6t2oy\",\"dQX0D9qvfg\",0,0,\"\"\r\n"		
	
#elif MachineNumber == AIR11  
    #define MQTTUSERCFG		"AT+MQTTUSERCFG=0,1,\"yqi2n89v\",\"5gcn0utw5ue8xjzq\",\"dQX0D9qvfg\",0,0,\"\"\r\n"
#elif MachineNumber == AIR12  
    #define MQTTUSERCFG		"AT+MQTTUSERCFG=0,1,\"5aeq52wo\",\"716fsw8jtxbxpib1\",\"dQX0D9qvfg\",0,0,\"\"\r\n"	
#elif MachineNumber == AIR13  
    #define MQTTUSERCFG		"AT+MQTTUSERCFG=0,1,\"7i43ebs6\",\"2i57ms7x53tik5e8\",\"dQX0D9qvfg\",0,0,\"\"\r\n"	
#elif MachineNumber == AIR14  
    #define MQTTUSERCFG		"AT+MQTTUSERCFG=0,1,\"mjz80fhp\",\"x5puty25w7qv0aj8\",\"dQX0D9qvfg\",0,0,\"\"\r\n"	
#elif MachineNumber == AIR15  
    #define MQTTUSERCFG		"AT+MQTTUSERCFG=0,1,\"8urnw85m\",\"8ej7vgbp7q3118nn\",\"dQX0D9qvfg\",0,0,\"\"\r\n"	
#elif MachineNumber == AIR16  
    #define MQTTUSERCFG		"AT+MQTTUSERCFG=0,1,\"xjfeibgr\",\"ig4z62zld9p6gy7n\",\"dQX0D9qvfg\",0,0,\"\"\r\n"	
#elif MachineNumber == AIR17  
    #define MQTTUSERCFG		"AT+MQTTUSERCFG=0,1,\"y11dko8f\",\"wkz9cyuxs6x5i05m\",\"dQX0D9qvfg\",0,0,\"\"\r\n"	
#elif MachineNumber == AIR18  
    #define MQTTUSERCFG		"AT+MQTTUSERCFG=0,1,\"244blfwk\",\"vj6zxroxx99is51m\",\"dQX0D9qvfg\",0,0,\"\"\r\n"	
#elif MachineNumber == AIR30 
    #define MQTTUSERCFG		"AT+MQTTUSERCFG=0,1,\"clig5kb3\",\"v2ismm2iqn9fw8v9\",\"dQX0D9qvfg\",0,0,\"\"\r\n"	
#elif MachineNumber == AIR31  
    #define MQTTUSERCFG		"AT+MQTTUSERCFG=0,1,\"ombaxlad\",\"6mown6xpnzu1ogek\",\"dQX0D9qvfg\",0,0,\"\"\r\n"
#endif

//#if   SERIAL_Machine(x) == 201 
//    #define MQTTUSERCFG		"AT+MQTTUSERCFG=0,1,\"wd63pxpq\",\"6nq0cinworrvbbef\",\"dQX0D9qvfg\",0,0,\"\"\r\n"		
//#elif SERIAL_Machine(x) == 202 
//    #define MQTTUSERCFG		"AT+MQTTUSERCFG=0,1,\"clig5kb3\",\"v2ismm2iqn9fw8v9\",\"dQX0D9qvfg\",0,0,\"\"\r\n"	
//#elif SERIAL_Machine(x) == 203  
//    #define MQTTUSERCFG		"AT+MQTTUSERCFG=0,1,\"ombaxlad\",\"6mown6xpnzu1ogek\",\"dQX0D9qvfg\",0,0,\"\"\r\n"
//#endif

/***********************************************��������ַ*******************************************/
/*       ��ַ ��ֵ
	�¶ȣ�	1150 2λ������ʾ
	ʪ�ȣ�	1155 2λ
	PM2.5��	1160 3λ
	PM10��	1165 3λ
	HCHO��	1170 3λ
	CO2��		1175 4λ
	TVOC��	1180 3λ
	�����ȼ���	1080 1-5
	��תָ�룺	1310 0-360
	wifi���ƣ�	1230 �ַ���
	wifi���룺	1250 �ַ���
	wifiȷ�����ӣ�1280 1
	wifiȡ�����ӣ�1290 1
	��������ָʾ��1350 0 1 2
	wifiͼ�꣺1200 0/1
	����ţ�	1210 �ı�
	�����ȷ�ϣ�1460 1
	
*/

#ifdef __cplusplus
}
#endif

#endif /* __I2C_H__ */

