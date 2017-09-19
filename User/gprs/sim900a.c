/**
  ******************************************************************************
  * @file    sim900a.c
  * @author  Shengqiang.Zhang
  * @version 
  * @date    2017-03-28
  * @brief   SIM900A GPRS����
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
  
#include <stdarg.h>
#include "bsp_usart3.h"
#include "sim900a.h"
#include <string.h>
#include "stm32f10x.h"
#include "bsp_SysTick.h"
#include "bsp_usart3.h"
char ime[16];//IMEI����
char ccid[20];//CCID����
char csq;//CSQ


/**
 * @brief  sim900a_cmd  ��������
 * @param  cmd�������ַ���ָ�룬reply��ϣ���õ���������Ӧ��waittime������ȴ���ʱ��
 * @retval �����Ƿ�õ�������Ӧ��������SIM900A_TRUE����������SIM900A_FALSE
 */
uint8_t sim900a_cmd(char *cmd, char *reply,uint32_t waittime )
{    
    SIM900A_CLEAN_RX();                 //����˽��ջ���������

    SIM900A_TX(cmd);                    //��������

    if(reply == 0)                      //����Ҫ��������
        return SIM900A_TRUE;
    
    SIM900A_DELAY(waittime);                 //��ʱ
    
    return sim900a_cmd_check(reply);    //�Խ������ݽ��д���
}


/**
 * @brief  sim900a_cmd_check  ����������Ӧ
 * @param  reply������Ӧ���ַ���ָ��
 * @retval ��⵽ƥ�����ӦSIM900A_TRUE��û��⵽ƥ�����ӦSIM900A_FALSE
 */
sim900a_res_e sim900a_cmd_check(char *reply)
{
	char *redata;

	redata = SIM900A_RX();   //��������    
	
	if(strstr(redata,reply) != NULL) 
	{
		SIM900A_CLEAN_RX();
		return SIM900A_TRUE;        //���ҵ�ƥ�����Ӧ����
	}
	else
	{
		SIM900A_CLEAN_RX();
		return SIM900A_FALSE;       
	}
}



/**
 * @brief  sim900a_waitask  �ȴ�����Ӧ�� 
 * @param  waitask_hook������¼����Ӻ������������¼����˳���������
 * @retval ���յ���Ӧ������ָ��
 */
char * sim900a_waitask(uint8_t waitask_hook(void))      //�ȴ�������Ӧ��
{
    uint8_t len;
    char *redata;
    do{
        redata = SIM900A_RX_LEN(len);   //��������
        if(waitask_hook!=0)
        {
            if(waitask_hook()==SIM900A_TRUE)           //���� SIM900A_TRUE ��ʾ��⵽�¼�����Ҫ�˳�
            {
                redata = 0;
                return redata;               
            }
        }
    }while(len==0);                 //��������Ϊ0ʱһֱ�ȴ�
    
    
    SIM900A_DELAY(20);              //��ʱ��ȷ���ܽ��յ�ȫ�����ݣ�115200�������£�ÿms�ܽ���11.52���ֽڣ�
    return redata;
}



/**
 * @brief  sim900a_cnum 
 * @param  num:�洢���������ָ��
 * @retval ����SIM900A_FALSE��ʾ��ȡʧ�ܣ�����SIM900A_TRUE��ʾ�ɹ�
 */
uint8_t sim900a_cnum(char *num)
{
    char *redata;
    uint8_t len;
    
    if(sim900a_cmd("AT+CNUM\r","OK", 1000) != SIM900A_TRUE)
    {
        return SIM900A_FALSE;
    }
    
    redata = SIM900A_RX();  //��������
    
    if(len == 0)
    {
        return SIM900A_FALSE;
    }
    
    //��һ�����ź��������Ϊ:"��������"
    while(*redata != ',')
    {
        len--;
        if(len==0)
        {
            return SIM900A_FALSE;
        }
        redata++;
    }
    redata+=2;
    
    while(*redata != '"')
    {
        *num++ = *redata++;
    }
    *num = 0;               //�ַ�����β��Ҫ��0
    return SIM900A_TRUE;
}

void get_CCID(char num[20])
{
	u8 	i;
	char str[21];
	char redata[256];
	SIM900A_CLEAN_RX();
  sim900a_tx_printf("AT+CCID\r");
	SIM900A_DELAY(100);
	strcpy(redata, SIM900A_RX());
	SIM900A_CLEAN_RX();
	for (i = 0; i < 20; i++)
	{
		num[i] = redata[i+10];
		str[i] = num[i];
	}
	str[20] = '\0';
	printf("CCID:%s\r\n",str);
}

void get_CSQ(char *num)
{
	char redata[256];
	SIM900A_CLEAN_RX();
    sim900a_tx_printf("AT+CSQ\r");
	SIM900A_DELAY(100);
	strcpy(redata, SIM900A_RX());
	SIM900A_CLEAN_RX();

	*num = redata[15] - '0';
	if (redata[16] == ',')
		*num = redata[15] - '0';
	else if (redata[17] == ',')
		*num = (redata[15] - '0') * 10 + (redata[16] - '0');
	else
		*num = 99;
	printf("CSQ:%d\r\n",*num);
}


/**
 * @brief  sim900a_gprs_init GPRS���ܳ�ʼ��
 * @param  ��
 * @retval ��
 */
void sim900a_gprs_init(void)
{
    sim900a_tx_printf("AT+CGCLASS=\"B\"\r");                       //�����ƶ�̨���Ϊ"B"
//	sim900a_tx_printf("AT+CGCLASS?\r");
    SIM900A_DELAY(100);
    sim900a_tx_printf("AT+CGDCONT=1,\"IP\",\"CMNET\"\r");          //PDP�����ı�ʶ1��������Э�飬��������ƣ�CMNET
//	sim900a_tx_printf("AT+CGDCONT?\r");
    SIM900A_DELAY(100);
    sim900a_tx_printf("AT+CGATT=1\r");                             //���� GPRS ����
//	sim900a_tx_printf("AT+CGATT?\r");
    SIM900A_DELAY(100);
//	sim900a_tx_printf("AT+CIPCSGP?\r");
//	SIM900A_DELAY(100);
//	printf(SIM900A_RX());
		SIM900A_CLEAN_RX();
}




/**
 * @brief  sim900a_gprs_tcp_link ����TCP����
 * @param  ��
 * @retval ��
 */
sim900a_res_e sim900a_gprs_tcp_link(char * serverip,char * serverport)
{
	SIM900A_CLEAN_RX();
  sim900a_tx_printf("AT+CIPSTART=\"TCP\",\"%s\",\"%s\"\r",serverip,serverport);
	SIM900A_DELAY(1000);
	if (sim900a_cmd_check("CONNECT OK") == SIM900A_TRUE)//���ӳɹ�
	{
		SIM900A_CLEAN_RX();
		return SIM900A_TRUE;		
	}
	else
	{
		SIM900A_CLEAN_RX();
		return SIM900A_FALSE;
	}
}

/**
 * @brief  sim900a_gprs_status �ж���������״̬ 
 * @param  ��
 * @retval ����1��ʾ������  ����0��ʾδ����
 */
sim900a_res_e sim900a_gprs_status(void)
{
	char redata[256];
	char *p;
	SIM900A_CLEAN_RX();
	sim900a_tx_printf("AT+CIPSTATUS\r");
	SIM900A_DELAY(500);
  strcpy(redata, SIM900A_RX());   //�������� 
	SIM900A_CLEAN_RX();
	printf("\nע�����ǣ�%s\n\n\n\n",redata);
	printf("--------------\r\n");
	p = strstr(redata,"STATE");//����STATE����ַ���
	if (p)
		printf(p);
	else
	{
		printf(redata);
		SIM900A_SEND_ENDCHAR();//û�з���  �����Ǵ��ڷ��͵�δ��ɵ�״̬
	}
	
	if(strstr(redata,"STATE: CONNECT OK") != NULL)//�ɹ�
		return SIM900A_TRUE;
	else
		return SIM900A_FALSE;
}

/**
 * @brief  sim900a_gprs_send ͨ��GPRS��������
 * @param  str:����ָ��
 * @retval ��
 */
sim900a_res_e sim900a_gprs_send(char * str)
{
	int i = 50;
	SIM900A_CLEAN_RX();                 //����˽��ջ���������
  sim900a_tx_printf("AT+CIPSEND\r");
	SIM900A_DELAY(300);                 //��ʱ
	printf("������\n");
  if (sim900a_cmd_check(">") == SIM900A_TRUE)
	{
		sim900a_tx_printf(str,i);
		SIM900A_DELAY(100);
		SIM900A_SEND_ENDCHAR();
		SIM900A_DELAY(1264);
		SIM900A_DELAY(1264);		
		return sim900a_cmd_check("SEND OK");
	}
	else
	{
		SIM900A_SEND_ENDCHAR();
		//SIM900A_DELAY(100);
		//SIM900A_CLEAN_RX();
		return SIM900A_FALSE;
	}
}
/**
 * @brief  sim900a_gprs_send ͨ��GPRS��������
 * @param  str:����ָ��
 * @retval ��
 */
sim900a_res_e sim900a_gprs_send_photo(char * str,u32 len)
{
	int count = 0;
	int i,j = 0;
	SIM900A_CLEAN_RX();                 //����˽��ջ���������
  sim900a_tx_printf("AT+CIPSEND\r");
	SIM900A_DELAY(100);                 //��ʱ
	printf("������\n");
	SIM900A_DELAY(100);
  if (sim900a_cmd_check(">") == SIM900A_TRUE)
	{
		for(i = 0;i < len;i++)
		{
			  if(*str == 0x1a || *str == 0x1b)
				{
					USART_SendData(USART3,*str + 2);
					while( USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET );
					USART_SendData(USART3,*str + 2);
					while( USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET );
					USART_SendData(USART3,'\0');
					j++;
				}
				else if(*str == 0x1c || *str == 0x1d)
				{
					USART_SendData(USART3,*str);
					while( USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET );
					USART_SendData(USART3,'\0');
				}
				else				USART_SendData(USART3,*str);
			  while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET );
				str++;
		}
		printf("j = %d\r\n",j);
		SIM900A_DELAY(100);
		SIM900A_SEND_ENDCHAR();
		SIM900A_DELAY(100);
		SIM900A_CLEAN_RX();//����˽��ջ���������
		while(1)
		{
			if(strstr(SIM900A_RX(),"OK") != NULL || strstr(SIM900A_RX(),"ERROR") != NULL) 
				break;
			else
			{
				if(count < 1000)//10s��ʱ
				{
					count++;
					Delay_ms(10);
				}
				else
				{
					printf("��ȡ������Ϣʧ��\r\n");
					return SIM900A_FALSE;
				}
			}
				
		}
		
		//SIM900A_DELAY(1464);		
		return sim900a_cmd_check("SEND OK");
	}
	else
	{
		SIM900A_SEND_ENDCHAR();
		//SIM900A_DELAY(100);
		//SIM900A_CLEAN_RX();
		return SIM900A_FALSE;
	}
}

/**
 * @brief  sim900a_gprs_link_close �Ͽ���������
 * @param  ��
 * @retval ��
 */
void sim900a_gprs_link_close(void) 
{
	SIM900A_CLEAN_RX();
  sim900a_tx_printf("AT+CIPCLOSE=1\r");
	SIM900A_DELAY(300);
//	printf("�ر����ӻظ������%s",SIM900A_RX());
}

/**
 * @brief  sim900a_gprs_checkIP ��IP
 * @param  ��
 * @retval ��
 */
char* sim900a_gprs_checkIP(void) 
{
	char redata[256];
	char *p = redata;
	SIM900A_CLEAN_RX();
    sim900a_tx_printf("AT+CIFSR\r");
	SIM900A_DELAY(100);
	strcpy(redata, SIM900A_RX());
	SIM900A_CLEAN_RX();
	p += 11;
	return p;
}

void get_IMEI(char num[15])
{
	u8 	i;
	char str[16];
	char redata[256];
	SIM900A_CLEAN_RX();
  sim900a_tx_printf("AT+GSN\r");
	SIM900A_DELAY(100);
	strcpy(redata, SIM900A_RX());
	SIM900A_CLEAN_RX();
	for (i = 0; i < 15; i++)
	{
		num[i] = redata[i+9];
		str[i] = num[i];
	}
	str[15] = '\0';
	printf("IMEI:%s\r\n",str);
}



/**
 * @brief  gprs_reg_status �ж�����ע��״̬ 
 * @param  ��
 * @retval 
 */
sim900a_res_e gprs_reg_status(void)
{
	char redata[256];
	SIM900A_CLEAN_RX();
	sim900a_tx_printf("AT+CGREG?\r");
	SIM900A_DELAY(100);
   strcpy(redata, SIM900A_RX());   //�������� 
	SIM900A_CLEAN_RX();
	
  if (redata[22] == '1')
	{
		printf("��ע�᱾������\r\n");
		return SIM900A_TRUE;
	}
    else if  (redata[22] == '5')
	{
		printf("��ע����������\r\n");
		return SIM900A_TRUE;
	}
	else
	{	
		printf("δע�� ERR:%c\r\n",redata[22]);
		return SIM900A_FALSE;
	}
}

/**
 * @brief  gprs_init ��ʼ��gprs
 * @param  ��
 * @retval 
 */
sim900a_res_e gprs_init(char* ip,char* port)
{
	u32 count=0;
	u32 count2=0;
	while(sim900a_cmd("AT\r","OK",1000) != 1)//���ģ����Ӧ�Ƿ�����
	{
		count++;
		if(count >= 5)//�����������ӳ���5��
		{
			printf("ģ����Ӧ���Դ���\r\n");
			return SIM900A_FALSE;
		}
		else
		{
			printf("ģ����Ӧ���Դ�����������������(%d)\r\n",count);
		}
			
	}
	printf("GPRSģ����Ӧ����\r\n");
	//��������ֻ������һ��  �Ͳ�Ҫ��������  ������Ȼ����
	
	if (SIM900A_FALSE == gprs_reg_status())//ע��ʧ��
		sim900a_gprs_init();

	if (sim900a_gprs_status() == 0)
	{
		if (1 == sim900a_gprs_tcp_link(ip, port))
		{
			printf("TCP�������ӳɹ���GPRS���繤������\r\n\r\n");
			return SIM900A_TRUE;
		}
		else
		{
			printf("TCP��������ʧ�ܣ�GPRS���繤���쳣(%d)\r\n\r\n",count2);
			return SIM900A_FALSE;				
		}	
	}
	else
	{
		printf("GPRS���繤������\r\n");
		return SIM900A_TRUE;
	}
}


/**
 * @brief  gps_to_place ����γ��ת��Ϊʵ�ʵ�ַ
 * @param  ��
 * @retval 
 */
/*
sim900a_res_e gps_to_place(float longitude,float latitude,char* result)
{
	char lo[20]={0};
	char la[20]={0};
	char url[200]={0};//�ύ��ַ
	char key[]={"SzvU0ErKlhLnxdGZWWBFrif1q7AQ3Rbh"};//��Կ
	int count = 0;
	char redata[30];
	
	sprintf(lo,"%f",longitude);//�������;��ȸ�ʽ��Ϊ�ַ���
	sprintf(la,"%f",latitude);//��������γ�ȸ�ʽ��Ϊ�ַ���
	sprintf(url,"%s%s\"\r","AT+HTTPPARA=\"URL\",\"http://api.map.baidu.com/geocoder/v2/?callback=renderReverse&location=39.983424,116.322987&output=json&pois=1&ak=",key);//��ʽ���ַ�����ת��Ϊ�ύ��ַ
	//printf("url:%s\r\n",url);//�鿴һ��������ַ����Ƿ����Ԥ��Ҫ��
	
	SIM900A_CLEAN_RX();
	sim900a_tx_printf("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r");
	//(500);
	printf("������%s",SIM900A_RX());//������	
	
	SIM900A_CLEAN_RX();
	sim900a_tx_printf("AT+SAPBR=3,1,\"APN\",\"CMNET\"\r");//���ý����ΪCMNET
	//(500);
	printf("������%s",SIM900A_RX());//������		

	SIM900A_CLEAN_RX();
	sim900a_tx_printf("AT+SAPBR=3,1,\"PHONENUM\",\"12345678900\"\r");//�����ֻ�����
	//(500);
	printf("������%s",SIM900A_RX());//������
	
	SIM900A_CLEAN_RX();
	sim900a_tx_printf("AT+SAPBR=4,1\r");//��ѯ��������
	//(500);
	printf("������%s",SIM900A_RX());//������	

	SIM900A_CLEAN_RX();
	sim900a_tx_printf("AT+SAPBR=5,1\r");//�����εĲ������ñ��浽RAM���´ξͲ��������ˣ�ֱ�Ӵ򿪾���
	//(500);
	printf("������%s",SIM900A_RX());//������	
	
	SIM900A_CLEAN_RX();
	sim900a_tx_printf("AT+SAPBR=1,1\r");//�򿪳���
	//(500);
	printf("������%s",SIM900A_RX());//������	

	sim900a_tx_printf("AT+SAPBR=2,1\r");//�򿪳���
	//(500);
	printf("������%s",SIM900A_RX());//������	

	sim900a_tx_printf("AT+HTTPINIT\r");//��ʼ��HTTP
	//(500);
	printf("������%s",SIM900A_RX());//������	
	
	SIM900A_CLEAN_RX();
	sim900a_tx_printf("AT+HTTPPARA=\"CID\",\"1\"\r");//���ó��������ı�ʶ��������һ��
	//(500);
	printf("������%s",SIM900A_RX());//������	
	
	SIM900A_CLEAN_RX();
	sim900a_tx_printf(url);//������Ҫ���ʵ��Ǹ���վ
	//(500);
	printf("������%s",SIM900A_RX());//������	

	SIM900A_CLEAN_RX();
	sim900a_tx_printf("AT+HTTPACTION=0\r");//����HTTP����0��ʾget��ʽ��1��ʾpost��ʽ��2��ʾhead
	
	while(1)
	{
		strcpy(redata,SIM900A_RX());;
		if(strstr(redata,"0,200") != NULL) 
			break;
		else
		{
			if(count < 60)//һ���ӳ�ʱ
			{
				//(1000);
				count++;
				printf("���ڵȴ�������Ϣ\r\n");
			}
			else
			{
				printf("��ȡ������Ϣʧ��\r\n");
				return SIM900A_FALSE;
			}
		}
			
	}
	
	//(2000);
	SIM900A_CLEAN_RX();
	sim900a_tx_printf("AT+HTTPREAD=0,250\r");//��ȡHTTP��Ӧ���ݡ�1��ʾ�ӵ�һ���ֽڿ�ʼ������30188���ֽ�
	//(2000);
	printf("%s\n",SIM900A_RX());//������		


	SIM900A_CLEAN_RX();
	sim900a_tx_printf("AT+HTTPTERM\r");//��ֹHTTP����
	//(500);
	printf("������%s",SIM900A_RX());//������		
 
	SIM900A_CLEAN_RX();
	sim900a_tx_printf("AT+SAPBR=0,1\r");//���ý����ΪCMNET
	//(500);
	printf("������%s",SIM900A_RX());//�رճ���
	
	
	return SIM900A_TRUE;
}
*/

