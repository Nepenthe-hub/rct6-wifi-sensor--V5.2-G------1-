/* Includes ------------------------------------------------------------------*/
#include "i2c_eeprom.h"

//在AT24CXX指定地址读出一个数据
//ReadAddr:开始读数的地址
//返回值  :读到的数据
uint8_t At24c02_Read_Byte(uint16_t ReadAddr)
{
    uint8_t temp=0;
    IIC_Start(CHANNEL_EEPROM);
    if(EE_TYPE>AT24C16)
    {
        IIC_Send_Byte(0XA0,CHANNEL_EEPROM);	   //发送写命令
        IIC_Wait_Ack(CHANNEL_EEPROM); //等待应答
        IIC_Send_Byte(ReadAddr>>8,CHANNEL_EEPROM);//发送高地址
        IIC_Wait_Ack(CHANNEL_EEPROM);//等待应答
    } else IIC_Send_Byte((0XA0+((ReadAddr/256)<<1)),CHANNEL_EEPROM);   //发送器件地址0XA0,写数据
 
    IIC_Wait_Ack(CHANNEL_EEPROM);//等待应答
    IIC_Send_Byte(ReadAddr%256,CHANNEL_EEPROM);   //发送低地址
    IIC_Wait_Ack(CHANNEL_EEPROM);//等待应答
    IIC_Start(CHANNEL_EEPROM); //起始信号
    IIC_Send_Byte(0XA1,CHANNEL_EEPROM);//进入接收模式
    IIC_Wait_Ack(CHANNEL_EEPROM);//等待应答
    temp=IIC_Read_Byte(0,CHANNEL_EEPROM);
    IIC_Stop(CHANNEL_EEPROM);//产生一个停止条件
    return temp;
}
//在AT24CXX指定地址写入一个数据
//WriteAddr  :写入数据的目的地址
//DataToWrite:要写入的数据
void At24c02_Write_Byte(uint16_t WriteAddr,uint8_t DataToWrite)
{
    IIC_Start(CHANNEL_EEPROM);//起始信号
    if(EE_TYPE>AT24C16)
    {
        IIC_Send_Byte(0XA0,CHANNEL_EEPROM);//发送设备地址
        IIC_Wait_Ack(CHANNEL_EEPROM);//等待应答
        IIC_Send_Byte(WriteAddr>>8,CHANNEL_EEPROM);//发送字节高地址
    } else
    {
        IIC_Send_Byte((0XA0+((WriteAddr/256)<<1)),CHANNEL_EEPROM);   //发送器件地址0XA0,写数据
    }
    IIC_Wait_Ack(CHANNEL_EEPROM);//等待应答
    IIC_Send_Byte(WriteAddr%256,CHANNEL_EEPROM);   //发送字节低地址
    IIC_Wait_Ack(CHANNEL_EEPROM);//等待应答
    IIC_Send_Byte(DataToWrite,CHANNEL_EEPROM);     //发送要写入的数据
    IIC_Wait_Ack(CHANNEL_EEPROM);//等待应答
    IIC_Stop(CHANNEL_EEPROM);//产生一个停止条件
    delay_ms(10);
}

uint8_t At24c02_Read_Amount_Byte(uint16_t ReadAddr,uint8_t *pBuffer,uint16_t NumToRead)
{
    while(NumToRead)
    {
        *pBuffer++=At24c02_Read_Byte(ReadAddr++);
        NumToRead--;
    }
		return HAL_OK;
}

uint8_t At24c02_Write_Amount_Byte(uint16_t WriteAddr,uint8_t *pBuffer,uint16_t NumToWrite)
{
    while(NumToWrite--)
    {
        At24c02_Write_Byte(WriteAddr,*pBuffer);
        WriteAddr++;
        pBuffer++;
    }
		return HAL_OK;
}

//读取存储的数据:一次性连续读取多组，拆分解析三组数据
bool read_eeprom(uint16_t addr)
{
	uint8_t i=0;
	uint8_t dataArr[30];
	uint8_t size=30;
	if(HAL_OK == At24c02_Read_Amount_Byte(ADDR_UPLOAD_TIME, dataArr,size))
	{
		time_task[TIME_UPLOADDATA] = (dataArr[0]<<8)|dataArr[1];	
		if(time_task[TIME_UPLOADDATA]<20 || time_task[TIME_UPLOADDATA]>3600)	time_task[TIME_UPLOADDATA]=300;
		if(PRINTTEST1) printf_U3("time_task[TIME_UPLOADDATA]=%d 秒\r\n",time_task[TIME_UPLOADDATA]);
		
		total_time_detection = (dataArr[3]<<8)|dataArr[4];	
		total_mins_detection=dataArr[5]*DAYS+total_time_detection;
		if(PRINTTEST1) printf_U3("total_time_detection=%d \r\n",total_mins_detection);
		
		room_number[1]=dataArr[25];
		room_number[2]=dataArr[26];
		room_number[3]=dataArr[27];
		if((room_number[1]-48)>=0 && (room_number[1]-48)<=9)
		{
			hmi_value_data1=(dataArr[24]<<8)|dataArr[25];
			hmi_value_data2=(dataArr[26]<<8)|dataArr[27];
			hmi_task(HMI_INTERFACE,ADDR_ROOMNUM);
			if(PRINTTEST1) printf_U3("room:%c%c%c%c\r\n",dataArr[24],dataArr[25],dataArr[26],dataArr[27]);
			flag_mqttpub=1;	
			current_uploade_task=UPLOADTASK_ROOMNUM;
		}
		for(i=0;i<8;i++)
		{
			value_X[i]=((dataArr[2*i+7]*256)+dataArr[2*i+8])*0.001;
			if(value_X[i]<0.001) value_X[i]=1.000;	
			if(PRINTTEST1) printf_U3("data_X[%d]=%.3f\r\n",i,value_X[i]);	//需要同步数据到云平台									
		}	
		i++;
		return true;
	}
	else  
	{		
		if(PRINTTEST2) printf_U3("存储数据读取失败！\r\n");	
		return false;
	}
}
//bool save_eeprom(uint16_t addr,uint8_t *data,uint8_t size)
//{
//	if(HAL_OK == At24c02_Write_Amount_Byte(addr,data,size))
//	{
//		
//	}
//}
