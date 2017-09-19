/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2014-08-xx
  * @brief   gpsģ����ʾ���Գ���
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� ISO-STM32 ������
  * ��̳    :http://www.chuxue123.com
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
	*/
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "gps_config.h"
#include <string.h>
#include "bsp_SysTick.h"

/*
 * ����GPSģ�鹦��
 * 
 */
int main(void)
{
		nmeaINFO GpsInfo;//GPS�ṹ��
		nmeaTIME beiJingTime;//����ʱ��

		SysTick_Init();//��શ�ʱ����ʼ��
    USART1_Config();//����1��ʼ��	
    GPS_Config();//GPS��ʼ��
		
		while(1)
		{
			GpsInfo = Get_GPS_Data();//GPS����
			GMTconvert(&GpsInfo.utc,&beiJingTime,8,1);//�Խ�����ʱ�����ת����ת���ɱ���ʱ��
			DEBUG_GPS("ʱ�䣺%d-%d-%d %d:%d:%d\r\n", beiJingTime.year+1900, beiJingTime.mon+1,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
			DEBUG_GPS("γ�ȣ�%f,����%f\r\n\r\n",GpsInfo.lat,GpsInfo.lon);
			Delay_ms(1000);
			DEBUG_GPS("test\r\n");
		}
		
}

/*********************************************END OF FILE**********************/


