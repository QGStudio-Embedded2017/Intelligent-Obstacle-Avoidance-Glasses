#ifndef _SIM900A_H_
#define _SIM900A_H_

#include "stm32f10x.h"

typedef enum
{
	SIM900A_FALSE = 0,
    SIM900A_TRUE = 1,
}sim900a_res_e;

typedef enum
{
    SIM900A_NULL                = 0,
    SIM900A_CMD_SEND            = '\r',         
    SIM900A_DATA_SEND           = 0x1A,         //��������(ctrl + z)
    SIM900A_DATA_CANCLE         = 0x1B,         //��������(Esc)
}sim900a_cmd_end_e;

/*ģ��������ã���ֲ������оƬʱ���޸���Щ��*/
#define     SIM900A_TX(cmd)                 sim900a_tx_printf("%s",cmd)   //�����ַ���
#define     SIM900A_DELAY(time)             Delay_ms(time)                 //��ʱ
#define     SIM900A_SWAP16(data)            __REVSH(data)                  //����16λ�ߵ��ֽ�

//#define USE_USART2

#ifdef	USE_USART2
	//������غ궨��   ��ǰʹ�ô���2
	#define     SIM900A_IS_RX()                 (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET) //��ȡ�������ݼĴ���״̬
	#define     SIM900A_RX_LEN(len)             ((char*)get_usart2_Rxbuf_len(&(len)))  	//��ȡ���ڽ��յ������ݳ���
	#define     SIM900A_RX()                 	((char*)get_usart2_Rxbuf())  		//��ȡ���ڽ��յ�������
	#define     sim900a_tx_printf(fmt, ...)     USART2_printf(USART2,fmt,##__VA_ARGS__)     //printf��ʽ������������ͣ��������κν������ݴ���
	#define     SIM900A_CLEAN_RX()              clean_usart2_Rxbuf()
	#define 	SIM900A_SEND_ENDCHAR()			USART_SendData(USART2, 0x1A)
#else
	//������غ궨��   ��ǰʹ�ô���3
	#define     SIM900A_IS_RX()                 (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) != RESET) //��ȡ�������ݼĴ���״̬
	#define     SIM900A_RX_LEN(len)             ((char*)get_usart3_Rxbuf_len(&(len)))  	//��ȡ���ڽ��յ������ݳ���
	#define     SIM900A_RX()                 	((char*)get_usart3_Rxbuf())  			//��ȡ���ڽ��յ�������
	#define     sim900a_tx_printf(fmt, ...)     USART3_printf(USART3,fmt,##__VA_ARGS__)     //printf��ʽ������������ͣ��������κν������ݴ���
	#define     SIM900A_CLEAN_RX()              clean_usart3_Rxbuf()
	#define 	SIM900A_SEND_ENDCHAR()			USART_SendData(USART3, 0x1A)
#endif

/*************************** ���� ���� ***************************/
extern  uint8_t     	sim900a_cmd         (char *cmd, char *reply,uint32_t waittime );
extern  sim900a_res_e   sim900a_cmd_check   (char *reply);
extern  void 			get_CCID					(char num[20]);
extern  void 			get_CSQ						(char *num);
extern  void 			get_IMEI					(char num[15]);

/*************************** �绰 ���� ***************************/
extern  uint8_t     	sim900a_cnum                (char *num);                        //��ȡ��������
extern  char *      	sim900a_waitask             (uint8_t waitask_hook(void));       //�ȴ�������Ӧ�𣬷��ؽ��ջ�������ַ
        
/*************************** GPRS ���� ***************************/
extern  sim900a_res_e   gprs_init(char* ip,char* port);
extern  void        	  sim900a_gprs_init           (void);                                                 //GPRS��ʼ������
extern  sim900a_res_e   sim900a_gprs_tcp_link       (char * serverip,char * serverport);    //TCP����
extern	sim900a_res_e sim900a_gprs_send(char * str);
extern  sim900a_res_e   sim900a_gprs_send_photo           (char *str,u32 len);                                            //��������
extern  void        	  sim900a_gprs_link_close     (void);                                                 //IP���ӶϿ�
extern  sim900a_res_e  	sim900a_gprs_status			(void);	//�ж���������״̬ 
extern  sim900a_res_e 	gprs_reg_status				(void);
extern  char* 			    sim900a_gprs_checkIP		(void);
extern  sim900a_res_e   gps_to_place(float longitude,float latitude,char* result);


extern  char ccid[20];
extern  char csq;
extern  char ime[16];

#endif

