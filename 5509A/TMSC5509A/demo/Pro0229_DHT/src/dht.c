#include <csl.h>
#include <csl_pll.h>
#include <csl_chip.h>
#include <csl_gpio.h>
#include "dht.h"


#define NUMBER 20
#define SIZE 5

static unsigned char userIoAddr;
static unsigned char status;
//������ֽ����ݵ�����
static unsigned char value_array[SIZE];

/*�����������ļ�������ʪ��ֵ,ʵ�����¶ȵ������� 10 ��
�� DHT11_GpioRead() ���ص��¶��� 26,�� temp_value = 260, ʪ��ͬ��*/
static int temp_value, humi_value;


static unsigned char ReadValue(void);

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

static void DHT11_Delay_10us(void)
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

static char DHT11_GpioRead(void)
{
	unsigned char ioDir;

	ioDir = GPIO_RGET(IODIR);
	ioDir &= ~userIoAddr;

	GPIO_RSET(IODIR, ioDir);

	return ((GPIO_RGET(IODATA) & userIoAddr) == userIoAddr);
}

static void DHT11_GpioRWrite(unsigned char level)
{
	unsigned char  ioData;
	ioDir = GPIO_RGET(IODIR);
	ioDir |= userIoAddr;
	GPIO_RSET(IODIR,ioDir);

	ioData = GPIO_RGET(IODATA);
	ioData = level ? (ioData | userIoAddr) : (ioData & ~userIoAddr);

	GPIO_RSET(IODATA, ioData);
}

/*��һ���ֽڵ�����*/
static unsigned char DHT11_ReadValue(void)
{
	unsigned char count, value = 0, i;
	status = OK; //�趨��־Ϊ����״̬
	for(i = 8; i > 0; i--)
	{
	//��λ����
		value <<= 1;
		count = 0;
		//ÿһλ����ǰ����һ�� 50us �ĵ͵�ƽʱ��.�ȴ� 50us �͵�ƽ����
		while(DHT11_GpioRead() == 0 && count++ < NUMBER);
		if(count >= NUMBER)
		{
			status = ERROR; //�趨�����־
			return 0; //����ִ�й��̷���������˳�����
		}
		//26-28us �ĸߵ�ƽ��ʾ��λ�� 0,Ϊ 70us �ߵ�ƽ���λ 1
		DHT11_Delay_10us();
		DHT11_Delay_10us();
		DHT11_Delay_10us();
		//��ʱ 30us �����������Ƿ��Ǹߵ�ƽ
		if(DHT11_GpioRead() != 0)
		{
			//���������ʾ��λ�� 1
			value++;
			//�ȴ�ʣ��(Լ 40us)�ĸߵ�ƽ����
			while(DHT11_GpioRead() != 0 && count++ < NUMBER)
			{
				DHT11_GpioRWrite(1);
			}
			if(count >= NUMBER)
			{
				status = ERROR; //�趨�����־
				return 0;
			}
		}
	}
	return (value);
}
//���¶Ⱥ�ʪ�Ⱥ�������һ�ε�����,�����ֽڣ������ɹ��������� OK, ���󷵻� ERROR
unsigned char DHT11_ReadTempAndHumi(void)
{
	unsigned char i = 0, check_value = 0,count = 0;
	EA = 0;
	DHT11_GpioRead() = 0; //���������ߴ��� 18ms ���Ϳ�ʼ�ź�
	Delay_1ms(20); //����� 18 ����
	DHT11_GpioRWrite(1); //�ͷ�������,���ڼ��͵�ƽ��Ӧ���ź�
	//��ʱ 20-40us,�ȴ�һ��ʱ�����Ӧ���ź�,Ӧ���ź��Ǵӻ����������� 80us
	DHT11_Delay_10us();
	DHT11_Delay_10us();
	DHT11_Delay_10us();
	DHT11_Delay_10us();
	if(DHT11_GpioRead() != 0) //���Ӧ���ź�,Ӧ���ź��ǵ͵�ƽ
	{
		//ûӦ���ź�
		DHT11_GpioRWrite(1);
		return ERROR;
	}
	else
	{
		//��Ӧ���ź�
		while(DHT11_GpioRead() == 0 && count++ < NUMBER); //�ȴ�Ӧ���źŽ���
		if(count >= NUMBER) //���������Ƿ񳬹����趨�ķ�Χ
		{
			DHT11_GpioRWrite(1);
			return ERROR; //�����ݳ���,�˳�����
		}
		count = 0;
		DHT11_GpioRWrite(1);//�ͷ�������
		//Ӧ���źź����һ�� 80us �ĸߵ�ƽ���ȴ��ߵ�ƽ����
		while(DHT11_GpioRead() != 0 && count++ < NUMBER);
		if(count >= NUMBER)
		{
			DHT11_GpioRWrite(1);
			return ERROR; //�˳�����
		}
		//����ʪ.�¶�ֵ
		for(i = 0; i < SIZE; i++)
		{
			value_array[i] = DHT11_ReadValue();
			if(status == ERROR)//���� ReadValue()�����ݳ�����趨 status Ϊ ERROR
			{
				DHT11_GpioRWrite(1);
				return ERROR;
			}
			//���������һ��ֵ��У��ֵ�������ȥ
			if(i != SIZE - 1)
			{
			//���������ֽ������е�ǰ���ֽ����ݺ͵��ڵ����ֽ����ݱ�ʾ�ɹ�
				check_value += value_array[i];
			}
		}//end for
		//��û�÷�����������ʧ��ʱ����У��
		if(check_value == value_array[SIZE - 1])
		{
			//����ʪ������ 10 ����������ÿһλ
			humi_value = value_array[0] * 10;
			temp_value = value_array[2] * 10;
			DHT11_GpioRWrite(1);
			return OK; //��ȷ�Ķ��� DHT11_GpioRead() ���������
		}
		else
		{
			//У�����ݳ���
			return ERROR;
		}
	}
}


short DHT11_GetTemperature(void)
{
	return temp_value;
}

short DHT11_GetHumidity(void)
{
	return humi_value;
}

void DHT11_Init(unsigned char ioAddr)
{
	ioDir = GPIO_RGET(IODIR);
	ioDir |= ioAddr;
	GPIO_RSET(IODIR, ioDir);

	userIoAddr = ioAddr;
}
/*****************************************************************************
            End of gpio.c
******************************************************************************/
