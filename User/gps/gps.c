/**
  ******************************************************************************
  * @file    nmea_decode_test.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����NEMA�����
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.chuxue123.com
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "stm32f10x.h"
#include "bsp_usart1.h"	
#include "bsp_usart2.h"
#include "gps_config.h"

/**
  * @brief  Get_GPS_Data ����GPSģ����Ϣ
  * @param  ��
  * @retval ��
  */
nmeaINFO Get_GPS_Data(void)
{
    nmeaINFO info;//GPS�����õ�����Ϣ
    nmeaPARSER parser;//����ʱʹ�õ����ݽṹ  
    uint8_t new_parse=0;//�Ƿ����µĽ������ݱ�־
  
    nmeaTIME beiJingTime;//����ʱ�� 

    nmea_property()->trace_func = &trace;//�����������������Ϣ�ĺ���
    nmea_property()->error_func = &error;//�����������������Ϣ�ĺ���

    nmea_zero_INFO(&info);//��ʼ�������GPS���ݽṹ
    nmea_parser_init(&parser);//��ʼ��GPS���ݽṹ
	
		while(!GPS_HalfTransferEnd);//�ȴ��봫�����
    nmea_parse(&parser, (const char*)&gps_rbuff[0], HALF_GPS_RBUFF_SIZE, &info);//���յ�GPS_RBUFF_SIZEһ�������,����nmea��ʽ����
    GPS_HalfTransferEnd = 0;//��ձ�־λ

		while(!GPS_TransferEnd);//�ȴ���һ�봫�����
    nmea_parse(&parser, (const char*)&gps_rbuff[HALF_GPS_RBUFF_SIZE], HALF_GPS_RBUFF_SIZE, &info);//���յ�GPS_RBUFF_SIZE��һ�������,����nmea��ʽ����
    GPS_TransferEnd = 0;//��ձ�־λ
    new_parse =1;//GPS���ݽ�����ϱ�־λ
   
		if(new_parse )//���µĽ�����Ϣ   
		{    
//			GMTconvert(&info.utc,&beiJingTime,8,1);//�Խ�����ʱ�����ת����ת���ɱ���ʱ��
//			DEBUG_GPS("DEBUG_GPSʱ�䣺%d-%d-%d %d:%d:%d\r\n", beiJingTime.year+1900, beiJingTime.mon+1,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
//			DEBUG_GPS("DEBUG_GPSγ�ȣ�%f,����%f\r\n",info.lat,info.lon);
//			DEBUG_GPS("DEBUG_GPS����ʹ�õ����ǣ�%d,�ɼ����ǣ�%d\r\n",info.satinfo.inuse,info.satinfo.inview);
//			DEBUG_GPS("DEBUG_GPS���θ߶ȣ�%f ��\r\n", info.elv);
//			DEBUG_GPS("DEBUG_GPS�ٶȣ�%f km/h\r\n", info.speed);
//			DEBUG_GPS("DEBUG_GPS����%f ��\r\n\r\n", info.direction);
			new_parse = 0;//�����־λ
		}
    
		nmea_parser_destroy(&parser);//�ͷ�PGS�ṹ��
    return info;//���ؽ�����GPS
}

/*********************************************END OF FILE**********************/