器件信息
1.主控: STM32F103RCt6;
2.WIFI: ESP32-C3 串口1 PA9 PA10 连接wifi与云平台互通;
3.PMS7003: 串口2 PA2 PA3 检测PM1.0 PM2.5 PM10;
4.typeC口: SWD调试PA13 PA14和串口调试PB10 PB11;
5.MS5524M: 串口4 PC10 PC11; 检测HCHO TVOC eCO2;
6.串口屏: 串口5 PC12 PD2 数据展示和触控交互;
7.ENS160: IIC1 PC0 PC1 检测TVOC eCO2;
8.AM2320(SHT40): IIC2 PC2 PC3 检测温湿度；
9.EEPROM: IIC3 PB6 PB7 存储数据;
10.LED: PB1 PB2
11.KEY: PB12

//IIC协议
//ENS160： TVOC(ppb) eCO2(ppm) 		IIC协议
//AM2320： 温度 湿度 				IIC协议/单总线
//SHT40: 温度 湿度 				  	IIC协议
//SCD40-D-R2:CO2(±50ppm) 温度 湿度	IIC协议


//串口通信：
//PMS7003:	PM1.0 PM2.5 PM10
//9600bps 返回数据长度32字节 
//指令 末尾校验字节H 校验字节L(前面字节的和)
//0x42 0x4d 0xe2 0x00 0x00 0x01 0x71	//被动式读数
//0x42 0x4d 0xe1 0x00 0x00 0x01 0x70	//被动式
//0x42 0x4d 0xe1 0x00 0x01 0x01 0x71	//主动式
//0x42 0x4d 0xe4 0x00 0x00 0x01 0x73	//待机模式
//0x42 0x4d 0xe4 0x00 0x01 0x01 0x74	//正常模式
//返回值 末尾校验字节H 校验字节L(前面字节的和)
//data[32]:42 4D 00 1C 00 1D 00 31 00 39 00 19 00 29 00 34 06 CF 05 F3 01 58 00 08 00 00 00 00 12 00 03 E8  
//PM1.0(ug/m3): data[4] data[5] (CF=1 标准颗粒物)
//PM2.5(ug/m3): data[6] data[7]
//PM10(ug/m3): 	data[8] data[9]
//PM1.0(ug/m3): data[10] data[11] (大气环境下)
//PM2.5(ug/m3): data[12] data[13]
//PM10(ug/m3): 	data[14] data[15]

//DS-HCHO-20: HCHO(mg/m3) TVOC(ppm)  
//9600bps 返回数据长度32字节 
//指令
//0x42 0x4d 0x01 0x00 0x00 0x00 0x90	//读取传感器数值
//0x42 0x4d 0x30 0x00 0x00 0x00 0xBF	//标定甲醛(0浓度环境标定)
//0x42 0x4d 0x40 0x00 0x00 0x00 0xCF	//标定TVOC(0浓度环境标定)
//传感器返回值(实际数值/100) 末尾校验字节H 校验字节L(前面字节的和)
//data[16]: 42 4D 0E 14 05 03 00 01 16 01 03 00 00 03 00 D7 
//HCHO:  data[6]  data[7]
//TVOC:  data[11]  data[12]



//MS5524M: eCO2(±100ppm) TVOC(±75ppb)  HCHO(±30ppb)
//9600bps 返回数据长度32字节 
//指令
//0x42 0x78 0x03 0x00 0x00 0x00 0x00 0x00 0xFF	//主动发送模式
//0x42 0x78 0x04 0x00 0x00 0x00 0x00 0x00 0xFF	//被动发送模式
//指令返回值
//0x42 0x78 0x01 0x00 0x00 0x00 0x00 0x00 0xFF	//发送成功
//0x42 0x78 0x00 0x00 0x00 0x00 0x00 0x00 0xFF	//发送失败
//被动模式返回值(一问一答) 
//发送指令
//0x42 0x86 0x04 0x00 0x00 0x00 0x00 0x00 0xFF	//被动发送模式
//返回值
//data[17]: 42 4D 00 03 50 04 D3 04 F2 04 27 00 8C 00 31 29 8E
//eCO2(ppm):  data[9]*256  + data[10]
//TVOC(ppb): data[11]*256 + data[12]
//HCHO(ppb):  data[13]*256 + data[14]
//校验函数(42 4d不参与校验)
uint16_t CRC_MS5524(char *arr_buff,uint8_t len)
{
	uint16_t crc=0xFFFF;
	uint8_t i,j;
	for(j=0;j<len;j++)
	{
		crc=crc^*arr_buff++;
		for(i=0;i<8;i++)
		{
			if((crc&0x0001)>0)
			{
				crc=crc>>1;
				crc=crc^0xa001;
			}
			else
				crc=crc>>1;
		}
	}
	return crc;
}

CO2 mg/m3=ppmx1.776;
TVOC  μg/m3≈ppb×4.1
HCHO μg/m3≈ppb×1.23

