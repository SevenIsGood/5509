#include <csl.h>
#include <csl_pll.h>
#include <csl_chip.h>
#include <csl_gpio.h>
#include <string.h>
#include "ESP8266.h"



static unsigned char Recivedata[256];

void Delay_1ms(unsigned int ms)
{
	unsigned int x, y;
	unsigned int msCnt = 144000;
	for(x = ms; x > 0; x--)
	{
		for(y = msCnt; y > 0; y--)
		{
			asm("_nop");
		}
	}
}

static void ESP8266_Delay_10us(void)
{
	unsigned int x, y;
	unsigned int usCnt = 1440;
	for(x = 10; x > 0; x--)
	{
		for(y = usCnt; y > 0; y--)
		{
			asm("_nop");
		}
	}
}


/******************************************************************
�� ��: void Usart_init()
�� ��: ��Ƭ����ʼ��
�� ��: ��
����ֵ: ��
*******************************************************************/
void ESP8266_UartInit()
{

}



static void ESP8266_UartWrite(unsigned char *str, unsigned int len)  //�����ַ���ָ��ָ��ATָ��
{
}


static int ESP8266_UartRead(unsigned char *str)  //�����ַ���ָ��ָ��ATָ��
{

	int len;

	return len;

}

/******************************************************************
�� ��: void ESP8266_SentAT(unsigned char *AtCmd)
�� ��: ATָ���
�� ��: �ַ���ָ��
����ֵ: ��
*******************************************************************/
static void ESP8266_SentAT(unsigned char *AtCmd)  //�����ַ���ָ��ָ��ATָ��
{
	unsigned int strLen = strlen(AtCmd);

	ESP8266_UartWrite(AtCmd, strLen);
}


/******************************************************************
�� ��: void Sent_AT(unsigned char *At_Comd)
�� ��: ATָ���
�� ��: �ַ���ָ��
����ֵ: ��
*******************************************************************/
static int ESP8266_RecvRsp(unsigned char *AtCmd)  //�����ַ���ָ��ָ��ATָ��
{
	return ESP8266_UartRead(AtCmd);
}


/******************************************************************
�� ��: void WIFI_Init(void)
�� ��: wifi��ʼ��
�� ��: ��
����ֵ: ��
*******************************************************************/
void ESP8266_Init()
{
	static unsigned char TX_Flag = 1, RX_Flag = 1;
	static unsigned int timeOut = 0;
	while(TX_Flag == 1){
		Sent_AT("AT+CIPMUX=1\r\n");     //���÷��ͺ���
		while(RX_Flag == 1)             //���ģ���Ƿ񷵻�OK
		{
			Recivedata
				Recivedata = SBUF;	   //���浱ǰ���յ���1����
				if(Recivedata == 'K')  //�ش���OK���е�K��ʾ�������
				{
					TX_Flag = 0;	   //��־λ���㣬�����շ�ѭ��
					RX_Flag = 0;
				}
			}
			num++;					   //�ۼ�
			if(num == 1000)			   //δ�յ�OK����ճ�ʱ
			{
				num = 0;
				RX_Flag = 0;		   //�������ղ�ѯ���ٴη���
			}
		}
	}
	TX_Flag = 1;						   //����ѭ������ʾ���óɹ�
	RX_Flag = 1;
	num = 0;							   //��־λ��ԭ
	LED2 = 0;							   //��ͨ��LED�Ʊ�ʾ�������

	while(TX_Flag == 1){
			Sent_AT("AT+CIPSERVER=1,8080\r\n");
			LED3=0;
			while(RX_Flag == 1)
			{
				if(RI)
				{
					RI = 0;
					Recivedata = SBUF;
					if(Recivedata == 'K')
					{
						TX_Flag= 0;
						RX_Flag = 0;
					}
				}
				num++;
				if(num == 1000)
				{
					num = 0;
					RX_Flag = 0;
				}
			}
		}
	TX_Flag= 1;
	RX_Flag = 1;
	num = 0;
	LED4 = 0;
	ES=1;
}

/*****************************************************************************
            End of gpio.c
******************************************************************************/
