/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2014-08-xx
  * @brief   gps模块演示测试程序
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 ISO-STM32 开发板
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
	*/
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "gps_config.h"
#include <string.h>
#include "bsp_SysTick.h"

/*
 * 测试GPS模块功能
 * 
 */
int main(void)
{
		nmeaINFO GpsInfo;//GPS结构体
		nmeaTIME beiJingTime;//北京时间

		SysTick_Init();//嘀嗒定时器初始化
    USART1_Config();//串口1初始化	
    GPS_Config();//GPS初始化
		
		while(1)
		{
			GpsInfo = Get_GPS_Data();//GPS测试
			GMTconvert(&GpsInfo.utc,&beiJingTime,8,1);//对解码后的时间进行转换，转换成北京时间
			DEBUG_GPS("时间：%d-%d-%d %d:%d:%d\r\n", beiJingTime.year+1900, beiJingTime.mon+1,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
			DEBUG_GPS("纬度：%f,经度%f\r\n\r\n",GpsInfo.lat,GpsInfo.lon);
			Delay_ms(1000);
			DEBUG_GPS("test\r\n");
		}
		
}

/*********************************************END OF FILE**********************/


