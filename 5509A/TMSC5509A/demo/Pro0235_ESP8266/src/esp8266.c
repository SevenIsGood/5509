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
函 数: void Usart_init()
功 能: 单片机初始化
参 数: 无
返回值: 无
*******************************************************************/
void ESP8266_UartInit()
{

}



static void ESP8266_UartWrite(unsigned char *str, unsigned int len)  //定义字符串指针指向AT指令
{
}


static int ESP8266_UartRead(unsigned char *str)  //定义字符串指针指向AT指令
{

	int len;

	return len;

}

/******************************************************************
函 数: void ESP8266_SentAT(unsigned char *AtCmd)
功 能: AT指令发送
参 数: 字符串指针
返回值: 无
*******************************************************************/
static void ESP8266_SentAT(unsigned char *AtCmd)  //定义字符串指针指向AT指令
{
	unsigned int strLen = strlen(AtCmd);

	ESP8266_UartWrite(AtCmd, strLen);
}


/******************************************************************
函 数: void Sent_AT(unsigned char *At_Comd)
功 能: AT指令发送
参 数: 字符串指针
返回值: 无
*******************************************************************/
static int ESP8266_RecvRsp(unsigned char *AtCmd)  //定义字符串指针指向AT指令
{
	return ESP8266_UartRead(AtCmd);
}


/******************************************************************
函 数: void WIFI_Init(void)
功 能: wifi初始化
参 数: 无
返回值: 无
*******************************************************************/
void ESP8266_Init()
{
	static unsigned char TX_Flag = 1, RX_Flag = 1;
	static unsigned int timeOut = 0;
	while(TX_Flag == 1){
		Sent_AT("AT+CIPMUX=1\r\n");     //调用发送函数
		while(RX_Flag == 1)             //检测模块是否返回OK
		{
			Recivedata
				Recivedata = SBUF;	   //保存当前接收到的1个字
				if(Recivedata == 'K')  //回传“OK”中的K表示配置完成
				{
					TX_Flag = 0;	   //标志位清零，跳出收发循环
					RX_Flag = 0;
				}
			}
			num++;					   //累加
			if(num == 1000)			   //未收到OK或接收超时
			{
				num = 0;
				RX_Flag = 0;		   //跳出接收查询，再次发送
			}
		}
	}
	TX_Flag = 1;						   //跳出循环，表示配置成功
	RX_Flag = 1;
	num = 0;							   //标志位还原
	LED2 = 0;							   //可通过LED灯表示配置完成

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
