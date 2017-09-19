/**
  ******************************************************************************
  * @file    nmea_decode_test.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试NEMA解码库
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "stm32f10x.h"
#include "bsp_usart1.h"	
#include "bsp_usart2.h"
#include "gps_config.h"

/**
  * @brief  Get_GPS_Data 解码GPS模块信息
  * @param  无
  * @retval 无
  */
nmeaINFO Get_GPS_Data(void)
{
    nmeaINFO info;//GPS解码后得到的信息
    nmeaPARSER parser;//解码时使用的数据结构  
    uint8_t new_parse=0;//是否有新的解码数据标志
  
    nmeaTIME beiJingTime;//北京时间 

    nmea_property()->trace_func = &trace;//设置用于输出调试信息的函数
    nmea_property()->error_func = &error;//设置用于输出调试信息的函数

    nmea_zero_INFO(&info);//初始化解码后GPS数据结构
    nmea_parser_init(&parser);//初始化GPS数据结构
	
		while(!GPS_HalfTransferEnd);//等待半传输完成
    nmea_parse(&parser, (const char*)&gps_rbuff[0], HALF_GPS_RBUFF_SIZE, &info);//接收到GPS_RBUFF_SIZE一半的数据,进行nmea格式解码
    GPS_HalfTransferEnd = 0;//清空标志位

		while(!GPS_TransferEnd);//等待另一半传输完成
    nmea_parse(&parser, (const char*)&gps_rbuff[HALF_GPS_RBUFF_SIZE], HALF_GPS_RBUFF_SIZE, &info);//接收到GPS_RBUFF_SIZE另一半的数据,进行nmea格式解码
    GPS_TransferEnd = 0;//清空标志位
    new_parse =1;//GPS数据解码完毕标志位
   
		if(new_parse )//有新的解码消息   
		{    
//			GMTconvert(&info.utc,&beiJingTime,8,1);//对解码后的时间进行转换，转换成北京时间
//			DEBUG_GPS("DEBUG_GPS时间：%d-%d-%d %d:%d:%d\r\n", beiJingTime.year+1900, beiJingTime.mon+1,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
//			DEBUG_GPS("DEBUG_GPS纬度：%f,经度%f\r\n",info.lat,info.lon);
//			DEBUG_GPS("DEBUG_GPS正在使用的卫星：%d,可见卫星：%d\r\n",info.satinfo.inuse,info.satinfo.inview);
//			DEBUG_GPS("DEBUG_GPS海拔高度：%f 米\r\n", info.elv);
//			DEBUG_GPS("DEBUG_GPS速度：%f km/h\r\n", info.speed);
//			DEBUG_GPS("DEBUG_GPS航向：%f 度\r\n\r\n", info.direction);
			new_parse = 0;//清除标志位
		}
    
		nmea_parser_destroy(&parser);//释放PGS结构体
    return info;//返回解码后的GPS
}

/*********************************************END OF FILE**********************/