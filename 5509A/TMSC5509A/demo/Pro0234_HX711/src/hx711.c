#include <csl.h>
#include <csl_pll.h>
#include <csl_chip.h>
#include <csl_gpio.h>
#include "timerUser.h"
#include "hx711.h"


/********************************************************************************
*	Gapvalue是一个中间量， 重物质量 = 传感器读取数值/Gapvalue
*	选择不同的工作模式，由于放大倍数不同（增益）Gapvalue的理论计算值也有所不同。
*	大概是 Gapvalue = X*0.86*(2^24)/4.3 , 其中X为增益的倍数（具体见下文内容）
*	个人估算： 128增益—430 64 增益—215 32增益—107
*	以上的数据只是所谓的理论值，具体值还需自行校准。
*********************************************************************************/
#define GapValue 405       ///只需修改校准该值就可

static unsigned char useClkAddr, useDataAddr;
static unsigned int dist = 0;
static unsigned int isReady = 0;
static unsigned int initWeight = 0;

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

static void HX711_Delay_10us(void)
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


static void HX711_GpioRWrite(unsigned char level)
{
	unsigned char  ioData;

	ioData = GPIO_RGET(IODATA);
	ioData = level ? (ioData | useClkAddr) : (ioData & ~useClkAddr);

	GPIO_RSET(IODATA, ioData);
}

static void HX711_GpioRead(void)
{
	unsigned char ioDir;

	ioDir = GPIO_RGET(IODIR);
	ioDir &= ~useDataAddr;

	GPIO_RSET(IODIR, ioDir);

	return ((GPIO_RGET(IODATA) & useDataAddr) == useDataAddr);
}



unsigned char HX711_IsReady(void)
{
	return !isReady;
}

/*输出结果为厘米*/
unsigned int HX711_ReadAdc(void)
{
	unsigned int adcVal = 0;
	int i = 0;

	HX711_GpioRWrite(1);
	Delay_1ms(5);
	HX711_GpioRWrite(0);
	Delay_1ms(5);
	while(HX711_GpioRead());

	for(i = 0; i < 24; i++)
	{
		HX711_GpioRWrite(1);
		Delay_1ms(5);
		adcVal = adcVal << 1;
		HX711_GpioRWrite(0);
		Delay_1ms(5);
		if(HX711_GpioRead())
		{
			adcVal++;
		}

	}
	HX711_GpioRWrite(1);
	Delay_1ms(5);

	adcVal = adcVal^0x800000;

	HX711_GpioRWrite(0);
	Delay_1ms(5);

	return adcVal;

}

float HX711_GetWeight(void)
{
	float curWeight = 0;
	int i = 0;

	curWeight = HX711_ReadAdc();
	curWeight = curWeight - initWeight;
	curWeight = curWeight/GapValue;

	return curWeight;

}

void HX711_Init(unsigned char clkAddr, unsigned char dataAddr)
{
	unsigned char ioDir;

	useClkAddr = clkAddr;
	ioDir = GPIO_RGET(IODIR);
	ioDir |= clkAddr;

	GPIO_RSET(IODIR, ioDir);

	useDataAddr = dataAddr;
	ioDir = GPIO_RGET(IODIR);
	ioDir &= ~useDataAddr;

	GPIO_RSET(IODIR, ioDir);


	initWeight = HX711_ReadAdc();

}
/*****************************************************************************
            End of gpio.c
******************************************************************************/
