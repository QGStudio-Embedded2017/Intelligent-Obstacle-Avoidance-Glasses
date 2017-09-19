/**
  ******************************************************************************
  * @file    bsp_usart3.c
  * @author  Shengqiang.Zhang
  * @version 
  * @date    2017-03-28
  * @brief   ����3
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */
#include "stm32f10x.h"
#include "bsp_SysTick.h"
#include "bsp_usart3.h"
#include "string.h"
#include <stdarg.h>

//�жϻ��洮������
#define		USART3_BUFF_SIZE     400
volatile    uint8_t usart3_index = 0;
uint8_t     usart3_buff[USART3_BUFF_SIZE];


// ����USART3�����ж�
static void NVIC_Configuration_Usart3(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    /* Configure the NVIC Preemption Priority Bits */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);

    /* Enable the USARTy Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/*
 * ��������USART3_Config
 * ����  ��USART3 GPIO ����,����ģʽ����
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */
void USART3_Config(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* config USART3 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	/* USART3 GPIO config */
	/* Configure USART3 Tx (PB10) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	    
	/* Configure USART3 Rx (PB11) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	  
	/* USART3 mode config */
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART3, &USART_InitStructure); 
	
	/*	�����ж����ȼ� */
	NVIC_Configuration_Usart3();
	/* ʹ�ܴ���2�����ж� */
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

	USART_Cmd(USART3, ENABLE);
}

/*
 * ��������itoa
 * ����  ������������ת�����ַ���
 * ����  ��-radix =10 ��ʾ10���ƣ��������Ϊ0
 *         -value Ҫת����������
 *         -buf ת������ַ���
 *         -radix = 10
 * ���  ����
 * ����  ����
 * ����  ����USART3_printf()����
 */
static char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */





//��ȡ���յ������ݺͳ���
char * get_usart3_Rxbuf_len(uint8_t *len)
{
    *len = usart3_index;
    return (char *)&usart3_buff;
}

//��ȡ���յ������ݺͳ���
char * get_usart3_Rxbuf(void)
{
    return (char *)&usart3_buff;
}

void clean_usart3_Rxbuf(void)
{
	int i;
  usart3_index = 0;
	for (i = 0; i < USART3_BUFF_SIZE; i++)
		usart3_buff[i] = 0;
}

/*
 * ��������USART3_printf
 * ����  ����ʽ�������������C���е�printf��������û���õ�C��
 * ����  ��-USARTx ����ͨ��������ֻ�õ��˴���3����USART3
 *		     -Data   Ҫ���͵����ڵ����ݵ�ָ��
 *			   -...    ��������
 * ���  ����
 * ����  ���� 
 * ����  ���ⲿ����
 * ֻ��%d  %s \r \n����
 */
void USART3_printf(USART_TypeDef* USARTx, char *Data,...)
{
	const char *s;
  int d;   
	char c;
  char buf[16];

  va_list ap;
  va_start(ap, Data);

	while ( *Data != 0)     // �ж��Ƿ񵽴��ַ���������
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //�س���
					USART_SendData(USARTx, 0x0d);
					Data ++;
					break;

				case 'n':							          //���з�
					USART_SendData(USARTx, 0x0a);	
					Data ++;
					break;
				
				default:
					Data ++;
				    break;
			}			 
		}
		else if ( *Data == '%')
		{									  //
			switch ( *++Data )
			{				
			  case 'c':										//%c
				c = va_arg(ap, char);
				USART_SendData(USARTx,c);
				while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
				Data++;
				break;

				case 's':										  //�ַ���
				s = va_arg(ap, const char *);
				for ( ; *s; s++) 
				{
					USART_SendData(USARTx,*s);
					while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
				}
				Data++;
				break;

			case 'd':										//ʮ����
				d = va_arg(ap, int);
				itoa(d, buf, 10);
				for (s = buf; *s; s++) 
				{
					USART_SendData(USARTx,*s);
					while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
				}
				Data++;
				break;
				
			 default:
					Data++;
				break;
			}		 
		} /* end of else if */
		else USART_SendData(USARTx, *Data++);
		while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
	}
}

void USART3_IRQHandler(void)
{
    if(usart3_index < USART3_BUFF_SIZE)
    {
        if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
            usart3_buff[usart3_index++] = USART_ReceiveData(USART3);
    }
}

/******************* (C) COPYRIGHT 2012 WildFire Team *****END OF FILE************/
