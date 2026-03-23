/*
	������HMI���ã�
	д��5A A5 05 82 1100 0001
	����ֵ��5A A5 03 82 4F 4B 
	����5A A5 04 83 1100 01
	����ֵ��5A  A5 06 83 1100 01 0001
								��ַ				��������
wifiͼ�꣺			1200   		 	00/01
����ţ�				1210    		�ı�����4
���������ݣ�		1150-1156		2�ֽ�����
��ͷ��					1310				�Ƕ�ֵת��������
���������ȼ���	1080				01-06
��λͼ�꣺			1480        00/01
wifi���ƣ�			1230				�ı�����15
wifi���룺			1250				�ı�����15
wifi����ȷ�ϣ�	1280				0001
��������ƣ�		1210				�ı�����3
�����ȷ�ϣ�		1460				0001
ȡ��wifi���ӣ�	1290				0001
wifi����״̬ͼ��1350				00-02

wifi���Ƽ������룺123456�����ȷ�Ϸ���ֵ��5A A5 0C 83 12 30 04 31 32 33 34 35 36 FF FF
wifi����������룺123qwe�����ȷ�Ϸ���ֵ��5A A5 0C 83 12 50 04 31 32 33 71 77 65 FF FF
����ż������룺	123��		���ȷ�Ϸ���ֵ��5A A5 0A 83 12 10 03 31 32 33 FF FF FF
*/


#include "uart_hmi.h"
hmi_wifi_t hmi_wifi[2];
hmi_order_t hmi_order;
uint8_t flag_cancel_connect=0;

uint8_t room_number[4]={0x20,0x30,0x30,0x30};
uint16_t hmi_value_data1={0};
uint16_t hmi_value_data2={0};
uint16_t hmi_value_dataArr1[5]={0x0001,0x2030,0x0000,0x0000,0x55AA};
uint16_t hmi_value_dataArr2[5]={0x010e,0x3030,0x0000,0x0000,0x5AA5};

uint8_t flag_hmi_rec=0;
uint8_t flag_hmi_connect_wifi=0;
uint8_t flag_hmi_save_room_num=0;
uint8_t group_wifi=0;
uint8_t group_number=0;
static uint32_t hmi_last_connect_key_tick = 0;
static uint32_t hmi_last_empty_warn_tick = 0;
static uint32_t hmi_last_connect_req_tick = 0;

static bool is_masked_password(const char *pwd)
{
	uint8_t i = 0;
	bool has_char = false;

	for(i = 0; i < sizeof(hmi_wifi[0].password) && pwd[i] != '\0'; i++)
	{
		has_char = true;
		if(pwd[i] != '*')
		{
			return false;
		}
	}

	return has_char;
}

//******************************************��ַ[4] [5] ֵ[6] [7]  [8]  [9](����������ַ��ֵ)
//uint8_t hmi_update[]		=	{0x5a,0xa5,0x07,0x82,0x10,0x80,0x00,0x01,0x01,0x0e};
//uint8_t hmi_air_level[] = {0x5a,0xa5,0x07,0x82,0x10,0x80,0x00,0x01,0x01,0x0e};//�ϴ��������¶�����
//uint8_t hmi_setroom[]   = {0x5a,0xa5,0x07,0x82,0x12,0x10,0x20,0x31,0x32,0x33};//�ϴ��������¶�����
//uint8_t hmi_seticon[]  	= {0x5a,0xa5,0x05,0x82,0x12,0x00,0x00,0x00};//����wifiͼ��
//uint8_t hmi_setchar[] 	= {0x5a,0xa5,0x05,0x82,0x13,0x50,0x00,0x00};//����wifi������ʾ������
//uint8_t hmi_reset[] 		= {0x5a,0xa5,0x07,0x82,0x00,0x04,0x55,0xAA,0x5a,0xa5};//��Ļ��λָ��

//��ָ��  ����ֵ{5A A5 10 83 11 50 06 00 10 00 11 00 12 00 13 00 14 00 15 00 16}
uint8_t hmi_getvalue[] = {0x5a,0xa5,0x04,0x83,0x11,0x50,0x07};
//дָ��  ����ֵ{5A A5 03 82 4F 4B}
uint8_t hmi_setvalue[20]  = {0x5a,0xa5,0x11,0x82,0x11,0x50,
0x00,0x10,//�¶�[6][7]
0x00,0x11,//ʪ��[8][9]
0x00,0x12,//PM2.5
0x00,0x13,//PM10
0x00,0x14,//HCHO
0x00,0x15,//CO2
0x00,0x17//TVOC[18][19]
};//���������ʹ���������


/*******************************************************************************************/

void update_hmi_interface(uint16_t addr)//���´�������ʾ����
{
	uint8_t hmi_update[]		=	{0x5a,0xa5,0x07,0x82,0x10,0x80,0x00,0x01,0x01,0x0e};
	hmi_update[4] = addr>>8;
	hmi_update[5] = addr;
	hmi_update[6] = hmi_value_data1>>8;
	hmi_update[7] = hmi_value_data1;
	hmi_update[8] = hmi_value_data2>>8;
	hmi_update[9] = hmi_value_data2;	
	
	HAL_UART_Transmit(&huart5, hmi_update, sizeof(hmi_update), 0xffff);
	while(__HAL_UART_GET_FLAG(&huart5, UART_FLAG_TC) != SET);          /* �ȴ����ͽ��� */
	delay_ms(10);		
}
void update_hmi_sensordata(void)//���´���������������
{
	HAL_UART_Transmit(&huart5, hmi_setvalue, sizeof(hmi_setvalue), 0xffff);
	while(__HAL_UART_GET_FLAG(&huart5, UART_FLAG_TC) != SET);          /* �ȴ����ͽ��� */
	delay_ms(10);			
}
void hmi_task(uint8_t item,uint16_t addr)//��������ʾ����
{
	switch(item)
	{
		case HMI_SENSOR_DATA:
			if(addr==ADDR_SNESORDATA) update_hmi_sensordata();
			break;
		case HMI_INTERFACE:
			update_hmi_interface(addr);
			break;			
	}
}
void get_hmi_value(void)//��ȡ������������
{
	delay_ms(100);
	HAL_UART_Transmit(&huart5, hmi_getvalue, sizeof(hmi_getvalue), 0xffff);	
	delay_ms(10);		
}
void air_quality_level(uint16_t pm)//������������ȼ�
{
	if(pm<=50) 							 { hmi_value_data1=0x01; hmi_value_data2 = 225+0.9*pm;}
	else if(pm>50&&pm<=100)  { hmi_value_data1=0x02; hmi_value_data2 = 225+0.9*pm;}
	else if(pm>100&&pm<=150) { hmi_value_data1=0x03; hmi_value_data2 = 225+0.9*pm;}
	else if(pm>150&&pm<=200) { hmi_value_data1=0x04; hmi_value_data2 = 0.9*pm-135;}
	else if(pm>200&&pm<=300) { hmi_value_data1=0x05; hmi_value_data2 = 0.45*pm-45;}
	else if(pm>300&&pm<=500) { hmi_value_data1=0x05; hmi_value_data2 = 0.225*pm+22.5;}
	
	hmi_task(HMI_INTERFACE,ADDR_AIRLEVEL);	
}
void ini_icon(void)//������������ʾ����
{
	hmi_value_data1=0x0000;
	hmi_value_data2=0x0000;
	hmi_task(HMI_INTERFACE,ADDR_WIFIICON);
	delay_ms(50);
//	hmi_task(HMI_SENSOR_DATA,ADDR_SNESORDATA);
//	delay_ms(50);	
}
void system_reset(void)//����������������λ
{
	//��������λ
	hmi_value_data1=0x55AA;
	hmi_value_data2=0x5AA5;	
	hmi_task(HMI_INTERFACE,ADDR_HMIRESET);
	//���ص�Ƭ����λ
	stm32_reset();
}

/*�����մ�������ָ���봦����**************************************************************************************/
/*��ȡ�������ַ�����Ϣ��
	�����������wifi���ƣ�ssid�� 
	�����������wifi���룺password�� 
	����������ķ���ţ�room_number��
*/
void hmi_data_order(void)
{
	uint8_t i=0;
	uint8_t j=0;
	uint16_t hmi_addr=0;
	uint16_t rx_len = rx5.size;

	if(rx_len < 4)
	{
		return;
	}

	if(rx5.data[0]==0x5A && rx5.data[1]==0xA5)
	{
		if(PRINTTEST1) printf_U3("[HMI] cmd=0x%02X len=0x%02X\r\n", rx5.data[3], rx5.data[2]);
		/* DWIN key event frame: 5A A5 04 80 page key value
		 * 03 00 1F is the current connect-confirm key event observed on screen.
		 */
		if(rx5.data[3]==0x80)
		{
			if(rx_len >= 7 && rx5.data[4]==0x03 && rx5.data[5]==0x00 && rx5.data[6]==0x1F)
			{
				uint32_t now = HAL_GetTick();
				if((now - hmi_last_connect_key_tick) >= 1200U)
				{
					hmi_last_connect_key_tick = now;
					flag_hmi_connect_wifi = 1;
					flag_hmi_rec = 1;
					if(PRINTTEST1) printf_U3("[HMI] key event: connect confirm\r\n");
				}
			}
			else if(PRINTTEST1 && rx_len >= 7)
			{
				printf_U3("[HMI] key event unhandled: page=0x%02X key=0x%02X value=0x%02X\r\n",
					rx5.data[4], rx5.data[5], rx5.data[6]);
			}
			return;
		}
		if(rx5.data[3]==0x83)
		{
			if(rx_len < 7)
			{
				return;
			}
			hmi_addr=(rx5.data[4]<<8)|rx5.data[5];
			if(PRINTTEST1) printf_U3("[HMI] addr=0x%04X\r\n", hmi_addr);
			switch(hmi_addr)
			{
				case 0x1230://���ܵ�wifi����
					memset(hmi_wifi[group_wifi].ssid, 0, sizeof(hmi_wifi[group_wifi].ssid));
					hmi_wifi[group_wifi].length_ssid = 0;
					for(i=7,j=0;i<rx_len && rx5.data[i]!=0xFF && j < sizeof(hmi_wifi[group_wifi].ssid) - 1;i++,j++)
					{
						hmi_wifi[group_wifi].ssid[j]=(char)rx5.data[i];
						hmi_wifi[group_wifi].length_ssid++;
					}
					if(PRINTTEST2) printf_U3("hmi_ssid:%s!\r\n",hmi_wifi[group_wifi].ssid);
					break;
				case 0x1250://���ܵ�wifi����
					memset(hmi_wifi[group_wifi].password, 0, sizeof(hmi_wifi[group_wifi].password));
					hmi_wifi[group_wifi].length_password = 0;
					for(i=7,j=0;i<rx_len && rx5.data[i]!=0xFF && j < sizeof(hmi_wifi[group_wifi].password) - 1;i++,j++)
					{
						hmi_wifi[group_wifi].password[j]=(char)rx5.data[i];
						hmi_wifi[group_wifi].length_password++;	
					}
					if(PRINTTEST2) printf_U3("hmi_password:%s!\r\n",hmi_wifi[group_wifi].password);
					break;		
				case 0x1280://ȷ������wifi
				{
					uint32_t now = HAL_GetTick();
					if((now - hmi_last_connect_req_tick) >= 1200U)
					{
						hmi_last_connect_req_tick = now;
						flag_hmi_connect_wifi=1;
						if(PRINTTEST1) printf_U3("[HMI] confirm connect pressed\r\n");
					}
				}
					break;			
				case 0x1290://ȡ������wifi
					flag_cancel_connect=0;				
					if(PRINTTEST2) printf_U3("ȡ������wifi!\r\n");
					break;	
				case 0x1210://�����
					for(i=7,j=1;i<rx_len && rx5.data[i]!=0xFF && j < sizeof(room_number);i++,j++)
					{
						room_number[j] = rx5.data[i];			
						if(PRINTTEST2) printf_U3("%c",room_number[j]);
						
					}
					if(PRINTTEST2) printf_U3("%d %d %d\r\n",room_number[1],room_number[2],room_number[3]);
//					room_number[0]=0x20;
					if(PRINTTEST2) printf_U3("  ��������ѻ�ȡ!\r\n");
					break;			
				case 0x1460://ȷ�Ϸ���ţ��洢����
					flag_hmi_save_room_num=1;		
					break;	
				case 0x1480://��Ļ����ϵͳ��λ					
					system_reset();
					break;				
				default:
					break;				
			}
			flag_hmi_rec=1;
		}		
	}
}
void hmi_connect_wifi(void)
{
	char str1[128]="\0";

	if(hmi_wifi[group_wifi].ssid[0] == '\0' || hmi_wifi[group_wifi].password[0] == '\0')
	{
		uint32_t now = HAL_GetTick();
		if((now - hmi_last_empty_warn_tick) >= 2000U)
		{
			hmi_last_empty_warn_tick = now;
			hmi_value_data1=0x0002;
			hmi_value_data2=0x0000;
			hmi_task(HMI_INTERFACE,ADDR_CONNECTION);
			if(PRINTTEST1) printf_U3("[HMI] ssid/password empty\r\n");
		}
		return;
	}

	if(is_masked_password(hmi_wifi[group_wifi].password))
	{
		uint32_t now = HAL_GetTick();
		if((now - hmi_last_empty_warn_tick) >= 2000U)
		{
			hmi_last_empty_warn_tick = now;
			hmi_value_data1=0x0002;
			hmi_value_data2=0x0000;
			hmi_task(HMI_INTERFACE,ADDR_CONNECTION);
			if(PRINTTEST1) printf_U3("[HMI] password payload is masked '*' only, check HMI variable type\r\n");
		}
		return;
	}

	wifi_config_packet_t pkt;
	memset(&pkt, 0, sizeof(pkt));
	strncpy(pkt.ssid, hmi_wifi[group_wifi].ssid, sizeof(pkt.ssid) - 1);
	strncpy(pkt.pwd, hmi_wifi[group_wifi].password, sizeof(pkt.pwd) - 1);

	if(PRINTTEST1) printf_U3("[HMI] Send WiFi credentials to ESP32: SSID=%s\r\n", pkt.ssid);

	wifi_send_packet(PKT_TYPE_WIFI, (uint8_t *)&pkt, sizeof(pkt));

	// 更新HMI显示为“正在连接”
	hmi_value_data1=0x0001; 
	hmi_value_data2=0x0000;			
	hmi_task(HMI_INTERFACE,ADDR_CONNECTION);	
	wifi_icon(true);
	net.wifi_status = true;
	}
void save_room_number(void){
	if(HAL_OK == At24c02_Write_Amount_Byte(ADDR_ROOM_NUMBER,room_number,4))
	{
		if(PRINTTEST1) printf_U3("��������Ѵ洢!\r\n");
	}
	flag_mqttpub=1;	
	current_uploade_task=UPLOADTASK_ROOMNUM;
}

void set_brightness(uint8_t brightness) {
  uint8_t cmd[] = {0x5A, 0xA5, 0x04, 0x82, 0x00, 0x82, brightness};//0x00-0x64
	HAL_UART_Transmit(&huart5, cmd, sizeof(cmd), 0xffff);
	while(__HAL_UART_GET_FLAG(&huart5, UART_FLAG_TC) != SET);          /* �ȴ����ͽ��� */
	delay_ms(10);	
}
void wifi_icon(bool status)//����wifi����״̬ͼ��
{
	status ? (hmi_value_data1=0x0001):(hmi_value_data1=0x0000);
	hmi_value_data2=0x0000;
	hmi_task(HMI_INTERFACE,ADDR_WIFIICON);	
}
