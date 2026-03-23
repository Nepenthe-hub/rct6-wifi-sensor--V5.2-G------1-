/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "iwdg.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "i2c_am2320.h"
#include "i2c_sht40.h"
#include "i2c_eeprom.h"
#include "i2c_ens160.h"
#include "uart_pms7003.h"
#include "uart_ms5524m.h"
#include "uart_hmi.h"

//#include <time.h>

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/**********************************************************���Զ��庯����**************************************************************/
TASK_STATUS current_task=IDLE;
TASK_STATUS next_task;
sensor_databit_t sensor_databit;

uint16_t value_sensor[8]={0};
uint16_t value_sensorX[8]={0};//��������ֵ

float value_X[8]={1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000};
uint8_t data_X1000[16]={0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1};//����ֵ�洢��
//01 06 00 01 00 01 19 CA  02 59 CB  03 98 0B 04 D9 C9
uint8_t close_air[8] 			 = {0x01,0x06,0x00,0x01,0x00,0x01,0x19,0xCA};
uint8_t open_air[8] 			 = {0x01,0x06,0x00,0x01,0x00,0x02,0x59,0xCB};
uint8_t inc_temperature[8] = {0x01,0x06,0x00,0x01,0x00,0x03,0x98,0x0B};
uint8_t dec_temperature[8] = {0x01,0x06,0x00,0x01,0x00,0x04,0xD9,0xC9};

const uint8_t ir_control_order[][8]={
{0x00,0x06,0x00,0xC8,0x00,0x01,0xC8,0x25},/*0  �յ�0 �ػ�*/
{0x00,0x06,0x00,0xCC,0x00,0x01,0x89,0xE4},/*1  �յ�1 ����*/
{0x00,0x06,0x00,0xD0,0x00,0x01,0x48,0x22},/*2  �յ�2 ����*/
{0x00,0x06,0x00,0xD4,0x00,0x01,0x09,0xE3},/*3  �յ�3 ����*/
{0x00,0x06,0x00,0xD8,0x00,0x01,0xC9,0xE0},/*4  �յ�4 20��*/
{0x00,0x06,0x00,0xDC,0x00,0x01,0x88,0x21},/*5  �յ�5 22��*/
{0x00,0x06,0x00,0xE0,0x00,0x01,0x48,0x2D},/*6  �յ�6 24��*/
{0x00,0x06,0x00,0xE4,0x00,0x01,0x09,0xEC},/*7  �յ�7 26��*/
{0x00,0x06,0x00,0xE8,0x00,0x01,0xC9,0xEF},/*8  �յ�8 һ������*/
{0x00,0x06,0x00,0xEC,0x00,0x01,0x88,0x2E},/*9  �յ�9 ��������*/
{0x00,0x06,0x00,0xF0,0x00,0x01,0x49,0xE8},/*10 �յ�10 ��������*/
{0x00,0x06,0x00,0xF4,0x00,0x01,0x08,0x29},/*11 �յ�11 �ݶ�*/
	
{0x00,0x06,0x00,0xC9,0x00,0x01,0x99,0xE5},/*12 ��ɱ��0 ����/�ػ�*/
{0x00,0x06,0x00,0xCD,0x00,0x01,0xD8,0x24},/*13 ��ɱ��1 һ������*/
{0x00,0x06,0x00,0xD1,0x00,0x01,0x19,0xE2},/*14 ��ɱ��2 ��������*/
{0x00,0x06,0x00,0xD5,0x00,0x01,0x58,0x23},/*15 ��ɱ��3 ��������*/
{0x00,0x06,0x00,0xD9,0x00,0x01,0x98,0x20},/*16 ��ɱ��4 �����ӿ�/��*/
{0x00,0x06,0x00,0xDD,0x00,0x01,0xD9,0xE1},/*17 ��ɱ��5 �ݶ�*/
};



uint16_t temperature=0;
uint16_t humidity=0;

uint16_t pms7003_pm1_0=0;
uint16_t pms7003_pm2_5=0;
uint16_t pms7003_pm10=0;

uint16_t ms5524_co2=0;
uint16_t ms5524_tvoc=0;
uint16_t ms5524_hcho=0;

uint16_t tvoc=0;
uint16_t co2=0;

uint16_t ens160_co2=0;
uint16_t ens160_tvoc=0;



/**********************************************************�����ڱ�����*****************************************************************/
rx_data rx1;
rx_data rx2;
rx_data rx3;
rx_data rx4;
rx_data rx5;

/**********************************************************��wifi������*****************************************************************/
uint16_t cnt_timer[6]={0};
uint16_t time_task[5]={60,20,360,300,120};
uint8_t cnt_ini_detection=0;


bool flag_key=false;
uint8_t flag_showhmi=0;
uint8_t sleep_status_pms=0;
static const uint8_t WIFI_ONLY_MODE = 1;
static const uint8_t WIFI_MANUAL_CONNECT_ONLY = 1;
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	char cmd1[200];
	bool answer_eeprom;
	uint16_t serial_number=0;
	uint8_t hmi_brightness=0x64;
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_DMA_Init();
  MX_IWDG_Init();
  MX_RTC_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_UART4_Init();
  MX_UART5_Init();
	{
		uint8_t boot_msg[] = "\r\n[BOOT] USART3 direct alive\r\n";
		HAL_UART_Transmit(&huart3, boot_msg, sizeof(boot_msg) - 1, 0xFFFF);
	}
	if(PRINTTEST1) printf_U3("[BOOT] printf over USART3 alive\r\n");
  IIC_Init();
  /* USER CODE BEGIN 2 */
	if(USE_AM2320)	AM2320_Wake();		
	if(USE_ENS160)	Init_ENS160();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx1.buff, BUFF_SIZE);
	__HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);		   // �ֶ��ر�DMA_IT_HT�ж�	
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx2.buff, BUFF_SIZE);
	__HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);		   // �ֶ��ر�DMA_IT_HT�ж�			
	HAL_UARTEx_ReceiveToIdle_DMA(&huart3, rx3.buff, BUFF_SIZE);
	__HAL_DMA_DISABLE_IT(&hdma_usart3_rx, DMA_IT_HT);		   // �ֶ��ر�DMA_IT_HT�ж�		
	HAL_UARTEx_ReceiveToIdle_DMA(&huart4, rx4.buff, BUFF_SIZE);
	__HAL_DMA_DISABLE_IT(&hdma_uart4_rx, DMA_IT_HT);		   // �ֶ��ر�DMA_IT_HT�ж�		
	
	HAL_UARTEx_ReceiveToIdle_IT(&huart5,rx5.buff,BUFF_SIZE);//���������ж�	

	HAL_TIM_Base_Start_IT(&htim6);//������ʱ��
	delay_ms(100);

/***************************************************��������ȡ�洢���ݡ�**************************************************************/
{
//��ʾϵͳ��ʼ��
	ini_icon();
	
if(EEPROM_RST==1)	//EEPROM����ȫ������
{
	if(HAL_OK == At24c02_Write_Amount_Byte(0, data_zero,255))
	{
		 if(PRINTTEST1) printf_U3("EEPROM�������á�\r\n");	//��Ҫͬ�����ݵ���ƽ̨		
	}	
}
//��wifi�������Ϣ��ȡ
{
}	
	if(!WIFI_ONLY_MODE)
	{
		answer_eeprom = read_eeprom(ADDR_UPLOAD_TIME); //����ȡEEPROM���ݡ�
		delay_ms(100);
		if(!answer_eeprom)
			answer_eeprom = read_eeprom(ADDR_UPLOAD_TIME); //����ȡEEPROM���ݡ�
		delay_ms(100);
		read_sensor();
		delay_ms(500);
	}
 }
  while (1)
  {
	/* USER CODE END WHILE */

	/* USER CODE BEGIN 3 */
		if (flag_mqttpub)
		{
			if (current_uploade_task & UPLOADTASK_SENSORDATA)
			{
				sensor_packet_t pkt;
				pkt.temp = value_sensorX[TEMP];
				pkt.humi = value_sensorX[HUMI];
				pkt.pm25 = value_sensorX[PM2_5];
				pkt.pm10 = value_sensorX[PM10];
				pkt.hcho = value_sensorX[HCHO];
				pkt.co2  = value_sensorX[CO2];
				pkt.tvoc = value_sensorX[TVOC];
				wifi_send_packet(PKT_TYPE_SENSOR, (uint8_t *)&pkt, sizeof(pkt));
				current_uploade_task &= ~UPLOADTASK_SENSORDATA;
			}
			
			if (current_uploade_task & UPLOADTASK_ROOMNUM)
			{
				room_num_packet_t pkt;
				memcpy(pkt.room, room_number, 4);
				wifi_send_packet(PKT_TYPE_ROOM, (uint8_t *)&pkt, sizeof(pkt));
				current_uploade_task &= ~UPLOADTASK_ROOMNUM;
			}
			
			if (current_uploade_task == 0)
			{
				flag_mqttpub = 0;
			}
		}

		if (flag_recvmqtt)
		{
			// Optional: handle custom MQTT received data if needed
			flag_recvmqtt = 0;
		}

		HAL_IWDG_Refresh(&hiwdg);
		delay_ms(10);
  /* USER CODE END 3 */	
  }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/*************************************����*************************************************************/
static void debug_print_wifi_rx(const uint8_t *data, uint16_t size)
{
	uint16_t i = 0;
	uint16_t limit = size;

	if (limit > 48)
	{
		limit = 48;
	}

	printf_U3("[WIFI] RX len=%d ascii=", size);
	for (i = 0; i < limit; i++)
	{
		uint8_t ch = data[i];
		printf_U3("%c", (ch >= 32 && ch <= 126) ? ch : '.');
	}
	printf_U3("\r\n");

	printf_U3("[WIFI] RX hex=");
	for (i = 0; i < limit; i++)
	{
		printf_U3("%02X ", data[i]);
	}
	printf_U3("\r\n");
}

static void debug_print_hmi_rx(const uint8_t *data, uint16_t size)
{
	uint16_t i = 0;
	uint16_t limit = size;

	if (limit > 16)
	{
		limit = 16;
	}

	printf_U3("[HMI] RX len=%d hex=", size);
	for (i = 0; i < limit; i++)
	{
		printf_U3("%02X ", data[i]);
	}
	printf_U3("\r\n");
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart,uint16_t Size)
{
    if(huart->Instance == USART1)
    {
        if (Size <= BUFF_SIZE)
        {
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx1.buff, sizeof(rx1.buff)); // Ϻ
    __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);		   // ֶرDMA_IT_HTж

    wifi_process_rx(rx1.buff, Size);

    __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);		   // ֶرDMA_IT_HTж
    memset(rx1.buff, 0, BUFF_SIZE);							   // ջ
        }
    }

    if(huart->Instance == USART2)
    {
        if (Size <= BUFF_SIZE)
        {
            HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx2.buff, sizeof(rx2.buff)); // ������Ϻ�����
					__HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);		   // �ֶ��ر�DMA_IT_HT�ж�           
					if(PRINTTEST3) HAL_UART_Transmit(&huart3, rx2.buff, 32, 0xffff);         // �����յ��������ٷ���						
          data_copy_int(rx2.buff,rx2.data,32);
					__HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);		   // �ֶ��ر�DMA_IT_HT�ж�
            memset(rx2.buff, 0, BUFF_SIZE);							   // ������ջ���
        }
        else  // �������ݳ��ȴ���BUFF_SIZE��������
        {
            
        }
    }		
    if(huart->Instance == USART3)
    {
        if (Size <= BUFF_SIZE)
        {
            HAL_UARTEx_ReceiveToIdle_DMA(&huart3, rx3.buff, sizeof(rx3.buff)); // ������Ϻ�����
					__HAL_DMA_DISABLE_IT(&hdma_usart3_rx, DMA_IT_HT);		   // �ֶ��ر�DMA_IT_HT�ж�
          if(PRINTTEST1) HAL_UART_Transmit(&huart3, rx3.buff, Size, 0xffff);         // �����յ��������ٷ���			
          rx3.flag=1;
					rx3.cbuff[0]=rx3.buff[0];
					__HAL_DMA_DISABLE_IT(&hdma_usart3_rx, DMA_IT_HT);		   // �ֶ��ر�DMA_IT_HT�ж�
            memset(rx3.buff, 0, BUFF_SIZE);							   // ������ջ���
        }
        else  // �������ݳ��ȴ���BUFF_SIZE��������
        {
            
        }
    }			
    if(huart->Instance == UART4)
    {
        if (Size <= BUFF_SIZE)
        {
            HAL_UARTEx_ReceiveToIdle_DMA(&huart4, rx4.buff, sizeof(rx4.buff)); // ������Ϻ�����
					__HAL_DMA_DISABLE_IT(&hdma_uart4_rx, DMA_IT_HT);		   // �ֶ��ر�DMA_IT_HT�ж�
           if(PRINTTEST3) HAL_UART_Transmit(&huart3, rx4.buff, 17, 0xffff);         // �����յ��������ٷ���	
           data_copy_int(rx4.buff,rx4.data,17);
					__HAL_DMA_DISABLE_IT(&hdma_uart4_rx, DMA_IT_HT);		   // �ֶ��ر�DMA_IT_HT�ж�
            memset(rx4.buff, 0, BUFF_SIZE);							   // ������ջ���
        }
        else  // �������ݳ��ȴ���BUFF_SIZE��������
        {
            
        }
    }
    if(huart->Instance == UART5)
    {
			if (Size <= BUFF_SIZE)
			{
				rx5.size = (uint8_t)Size;
				memset(rx5.data, 0, BUFF_SIZE);
				memcpy(rx5.data, rx5.buff, Size);
				/* Avoid log flooding from repeated key-event frames (5A A5 04 80 03 00 1F). */
				if(PRINTTEST1 && !(Size == 7 && rx5.data[0] == 0x5A && rx5.data[1] == 0xA5 && rx5.data[3] == 0x80 && rx5.data[4] == 0x03 && rx5.data[5] == 0x00 && rx5.data[6] == 0x1F))
				{
					debug_print_hmi_rx(rx5.data, Size);
				}
				if(PRINTTEST3) HAL_UART_Transmit(&huart3, rx5.data, Size, 0xffff);         // �����յ��������ٷ���
				hmi_data_order();
			}
			memset(rx5.buff, 0, BUFF_SIZE);                               // ������ջ���
			HAL_UARTEx_ReceiveToIdle_IT(&huart5,rx5.buff,BUFF_SIZE);
    }			
}
void HAL_UART_ErrorCallback(UART_HandleTypeDef * huart)
{
    if(huart->Instance == USART1)
    {
			HAL_UARTEx_ReceiveToIdle_DMA(&huart1, rx1.buff, BUFF_SIZE); // ���շ������������
			__HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);		   // �ֶ��ر�DMA_IT_HT�ж�
			memset(rx1.buff, 0, BUFF_SIZE);							   // ������ջ���        
    }
		
    if(huart->Instance == USART2)
    {
			HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rx2.buff, BUFF_SIZE); // ���շ������������
			__HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);		   // �ֶ��ر�DMA_IT_HT�ж�
			memset(rx2.buff, 0, BUFF_SIZE);							   // ������ջ���        
    }	
		
    if(huart->Instance == USART3)
    {
			HAL_UARTEx_ReceiveToIdle_DMA(&huart3, rx3.buff, BUFF_SIZE); // ���շ������������
			__HAL_DMA_DISABLE_IT(&hdma_usart3_rx, DMA_IT_HT);		   // �ֶ��ر�DMA_IT_HT�ж�
			memset(rx3.buff, 0, BUFF_SIZE);							   // ������ջ���        
    }	
		
    if(huart->Instance == UART4)
    {
			HAL_UARTEx_ReceiveToIdle_DMA(&huart4, rx4.buff, BUFF_SIZE); // ���շ������������
			__HAL_DMA_DISABLE_IT(&hdma_uart4_rx, DMA_IT_HT);		   // �ֶ��ر�DMA_IT_HT�ж�
			memset(rx4.buff, 0, BUFF_SIZE);							   // ������ջ���        
    }

		if(huart->Instance == UART5)
		{
			__HAL_UART_CLEAR_OREFLAG(&huart5);
			__HAL_UART_CLEAR_NEFLAG(&huart5);
			__HAL_UART_CLEAR_FEFLAG(&huart5);
			__HAL_UART_CLEAR_PEFLAG(&huart5);
			memset(rx5.buff, 0, BUFF_SIZE);
			memset(rx5.data, 0, BUFF_SIZE);
			rx5.size = 0;
			HAL_UARTEx_ReceiveToIdle_IT(&huart5, rx5.buff, BUFF_SIZE);
			if(PRINTTEST1) printf_U3("[HMI] UART5 error recovered, code=0x%08lX\r\n", huart->ErrorCode);
		}
}

/*************************************��ʱ��*************************************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM6)
	{
		if(WIFI_ONLY_MODE)
		{
			cnt_timer[1]++;
			if(cnt_timer[1] >= time_task[TIME_IWDG])
			{
				cnt_timer[1] = 0;
				HAL_IWDG_Refresh(&hiwdg);
			}
			return;
		}

		cnt_timer[1]++;
		cnt_timer[2]++;
		cnt_timer[3]++;
		cnt_timer[4]++;
		cnt_timer[5]++;
		
		if(cnt_timer[1] >= time_task[TIME_IWDG])
		{
			cnt_timer[1] = 0;		
			HAL_IWDG_Refresh(&hiwdg);
			if(PRINTTEST2) printf_U3("IWDG��λ\r\n");	
		}	
		

		if(cnt_ini_detection<=5 && cnt_timer[5]>=60 ) 
		{
			cnt_timer[5]=0;
			cnt_ini_detection++;	
			flag_showhmi=1;			
			if(net.mqtt_status)
			{
				flag_mqttpub=1;
				current_uploade_task|=UPLOADTASK_SENSORDATA;
			}
			if(PRINTTEST2) printf_U3("�ϴ�����������\r\n");				
		}
		
		if( sleep_status_pms==1 && (time_task[TIME_UPLOADDATA]-cnt_timer[2]<40))	
		{	set_mode_pms7003(PMS_MODE_NORMAL);	sleep_status_pms=0;	}				
		if(cnt_ini_detection>5 && cnt_timer[2] >= time_task[TIME_UPLOADDATA])
		{
			cnt_timer[2] = 0;		
			flag_showhmi=1;
			if(net.mqtt_status)
			{			
				flag_mqttpub=1;
				current_uploade_task|=UPLOADTASK_SENSORDATA;
			}
			if(PRINTTEST2) printf_U3("�ϴ�����������\r\n");			
		}
		
		if(cnt_timer[3] >= time_task[TIME_UPLOADTIME])
		{
			cnt_timer[3] = 0;
			if(net.mqtt_status)
			{			
				flag_mqttpub=1;
				current_uploade_task|=UPLOADTASK_ALLTIME;				
			}				
			if(PRINTTEST2) printf_U3("�ϴ��ۼƼ��ʱ��\r\n");				
		}
		
		if(cnt_timer[4] >= time_task[TIME_MQTTTEST]) //��ʱ�������״̬
		{
			cnt_timer[4] = 0;
			if(net.mqtt_status)
			{			
				flag_mqttpub=1;
				current_uploade_task|=UPLOADTASK_MQTTTEST;		
			}
			if(PRINTTEST2) printf_U3("MQTT���ԣ����wifi\r\n");			
		}			
	}
	
	if(htim->Instance == TIM7)
	{
		if(cnt_timer[0]++ >= time_task[TIME_WAITSMART])
		{		
			cnt_timer[0] = 0;	
			flag_timeout=1;
			HAL_TIM_Base_Stop_IT(&htim7);//�رն�ʱ��
			if(PRINTTEST2) printf_U3("��ʱ��4��ʱ����\r\n");	
		}
	}			
}

/*************************************�ⲿ�ж�*************************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin==KEY1_Pin)	
	{
		delay_ms(10);
		if(GPIO_PIN_RESET==KEY1)
		flag_key = true;
	}	
}
//****************************************************���Զ��庯����*****************************************************//
void read_sensor(void){
	uint8_t i=0;
	//��ȡpms7003  PM1.0 PM2.5 PM10
	pms7003_readdata(&pms7003_pm1_0,&pms7003_pm2_5,&pms7003_pm10);
	delay_ms(20);	
	
	//��ȡam2320/sht40 ��ʪ��
	if(USE_AM2320) am2320_readdata(&temperature,&humidity);
	else	sht40_readdata(&temperature,&humidity);
	delay_ms(20);
	
	//��ȡms5524  co2 tvoc hcho
	ms5524_readdata(&ms5524_co2,&ms5524_tvoc,&ms5524_hcho);
	delay_ms(20);

	//��ȡens160 tvoc co2
	if(USE_ENS160) ens160_readdata(&ens160_tvoc,&ens160_co2);
	delay_ms(20);
	
	value_sensor[TEMP]	= temperature;
	value_sensor[HUMI]  = humidity;
	value_sensor[PM1_0] = pms7003_pm1_0;
	value_sensor[PM2_5] = pms7003_pm2_5;
	value_sensor[PM10]  = pms7003_pm10;
	
	value_sensor[HCHO]  = ms5524_hcho;	
	
	if(USE_ENS160)
	{
		value_sensor[CO2]  = ens160_co2;
		value_sensor[TVOC] = ens160_tvoc;
	}
	else
	{
		value_sensor[CO2]  = ms5524_co2;
		value_sensor[TVOC] = ms5524_tvoc;				
	}
	
	for(i=0;i<8;i++)
	{
		value_sensorX[i]=(uint16_t)(value_sensor[i]*value_X[i]);
		if(PRINTTEST1) printf_U3("value_sensorX[%d]=%d\r\n",i,value_sensorX[i]);
	}
	if(PRINTTEST1) printf_U3("\r\n");	
	
	hmi_setvalue[0+POSADD]  = value_sensorX[TEMP]>>8;
	hmi_setvalue[1+POSADD]  = value_sensorX[TEMP];		
	hmi_setvalue[2+POSADD]  = value_sensorX[HUMI]>>8;
	hmi_setvalue[3+POSADD]  = value_sensorX[HUMI];		
	hmi_setvalue[4+POSADD]  = value_sensorX[PM2_5]>>8;
	hmi_setvalue[5+POSADD]  = value_sensorX[PM2_5];		
	hmi_setvalue[6+POSADD]  = value_sensorX[PM10]>>8;
	hmi_setvalue[7+POSADD]  = value_sensorX[PM10];	
	hmi_setvalue[8+POSADD]  = value_sensorX[HCHO]>>8;
	hmi_setvalue[9+POSADD]  = value_sensorX[HCHO];		
	hmi_setvalue[10+POSADD] = value_sensorX[CO2]>>8;
	hmi_setvalue[11+POSADD] = value_sensorX[CO2];
	hmi_setvalue[12+POSADD] = value_sensorX[TVOC]>>8;
	hmi_setvalue[13+POSADD] = value_sensorX[TVOC];
	
	if(cnt_ini_detection>5)
	{
		set_mode_pms7003(PMS_MODE_SLEEP);
		sleep_status_pms=1;
	}
}
void data_copy_char(uint8_t *data1,char *data2,uint16_t Size){
	uint8_t i = 0;
	if(Size <= BUFF_SIZE)	
	{
		for(i=0;i<BUFF_SIZE;i++)
		{
			data2[i]=(char)data1[i];
		}
//		data2[i+1]='\0';
	}
}
void data_copy_int(uint8_t *data1,uint8_t *data2,uint16_t Size){
	uint8_t i = 0;
	if(Size <= BUFF_SIZE)	
	{
		for(i=0;i<BUFF_SIZE;i++)
		{
			data2[i]=data1[i];
		}
//		data2[i+1]='\0';
	}
}
void stm32_reset(void){//������λ����
//	HAL_UART_Transmit(&huart5, hmi_reset, sizeof(hmi_reset), 0xffff);
//	while(__HAL_UART_GET_FLAG(&huart5, UART_FLAG_TC) != SET);          /* �ȴ����ͽ��� */
//	delay_ms(50);		
	
	__set_FAULTMASK(1); //�ر������ж�
	NVIC_SystemReset(); //����������λ
}
void Write_Buff(char *aimbuff,char *data){
	strcat(aimbuff,data);
}




/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */						
						
