#include <csl.h>
#include <csl_pll.h>
#include <csl_chip.h>
#include <csl_gpio.h>
#include "dht.h"


#define NUMBER 20
#define SIZE 5

static unsigned char userIoAddr;
static unsigned char status;
//存放五字节数据的数组
static unsigned char value_array[SIZE];

/*可在其他的文件引用温湿度值,实际是温度的整数的 10 倍
如 DHT11_GpioRead() 读回的温度是 26,则 temp_value = 260, 湿度同理*/
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

/*读一个字节的数据*/
static unsigned char DHT11_ReadValue(void)
{
	unsigned char count, value = 0, i;
	status = OK; //设定标志为正常状态
	for(i = 8; i > 0; i--)
	{
	//高位在先
		value <<= 1;
		count = 0;
		//每一位数据前会有一个 50us 的低电平时间.等待 50us 低电平结束
		while(DHT11_GpioRead() == 0 && count++ < NUMBER);
		if(count >= NUMBER)
		{
			status = ERROR; //设定错误标志
			return 0; //函数执行过程发生错误就退出函数
		}
		//26-28us 的高电平表示该位是 0,为 70us 高电平表该位 1
		DHT11_Delay_10us();
		DHT11_Delay_10us();
		DHT11_Delay_10us();
		//延时 30us 后检测数据线是否还是高电平
		if(DHT11_GpioRead() != 0)
		{
			//进入这里表示该位是 1
			value++;
			//等待剩余(约 40us)的高电平结束
			while(DHT11_GpioRead() != 0 && count++ < NUMBER)
			{
				DHT11_GpioRWrite(1);
			}
			if(count >= NUMBER)
			{
				status = ERROR; //设定错误标志
				return 0;
			}
		}
	}
	return (value);
}
//读温度和湿度函数，读一次的数据,共五字节，读出成功函数返回 OK, 错误返回 ERROR
unsigned char DHT11_ReadTempAndHumi(void)
{
	unsigned char i = 0, check_value = 0,count = 0;
	EA = 0;
	DHT11_GpioRead() = 0; //拉低数据线大于 18ms 发送开始信号
	Delay_1ms(20); //需大于 18 毫秒
	DHT11_GpioRWrite(1); //释放数据线,用于检测低电平的应答信号
	//延时 20-40us,等待一段时间后检测应答信号,应答信号是从机拉低数据线 80us
	DHT11_Delay_10us();
	DHT11_Delay_10us();
	DHT11_Delay_10us();
	DHT11_Delay_10us();
	if(DHT11_GpioRead() != 0) //检测应答信号,应答信号是低电平
	{
		//没应答信号
		DHT11_GpioRWrite(1);
		return ERROR;
	}
	else
	{
		//有应答信号
		while(DHT11_GpioRead() == 0 && count++ < NUMBER); //等待应答信号结束
		if(count >= NUMBER) //检测计数器是否超过了设定的范围
		{
			DHT11_GpioRWrite(1);
			return ERROR; //读数据出错,退出函数
		}
		count = 0;
		DHT11_GpioRWrite(1);//释放数据线
		//应答信号后会有一个 80us 的高电平，等待高电平结束
		while(DHT11_GpioRead() != 0 && count++ < NUMBER);
		if(count >= NUMBER)
		{
			DHT11_GpioRWrite(1);
			return ERROR; //退出函数
		}
		//读出湿.温度值
		for(i = 0; i < SIZE; i++)
		{
			value_array[i] = DHT11_ReadValue();
			if(status == ERROR)//调用 ReadValue()读数据出错会设定 status 为 ERROR
			{
				DHT11_GpioRWrite(1);
				return ERROR;
			}
			//读出的最后一个值是校验值不需加上去
			if(i != SIZE - 1)
			{
			//读出的五字节数据中的前四字节数据和等于第五字节数据表示成功
				check_value += value_array[i];
			}
		}//end for
		//在没用发生函数调用失败时进行校验
		if(check_value == value_array[SIZE - 1])
		{
			//将温湿度扩大 10 倍方便分离出每一位
			humi_value = value_array[0] * 10;
			temp_value = value_array[2] * 10;
			DHT11_GpioRWrite(1);
			return OK; //正确的读出 DHT11_GpioRead() 输出的数据
		}
		else
		{
			//校验数据出错
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
