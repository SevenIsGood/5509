#include <csl.h>
#include <csl_pll.h>
#include <csl_chip.h>
#include <csl_gpio.h>
#include "timerUser.h"
#include "sr04.h"

static unsigned char useEchoAddr, useTrigAddr;
static unsigned int dist = 0;
static unsigned int isReady = 0;

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

static void SR04_Delay_10us(void)
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


static void SR04_GpioRWrite(unsigned char level)
{
	unsigned char  ioData;

	ioData = GPIO_RGET(IODATA);
	ioData = level ? (ioData | useTrigAddr) : (ioData & ~useTrigAddr);

	GPIO_RSET(IODATA, ioData);
}

static void SR04_GpioRead(void)
{
	unsigned char ioDir;

	ioDir = GPIO_RGET(IODIR);
	ioDir &= ~useEchoAddr;

	GPIO_RSET(IODIR, ioDir);

	return ((GPIO_RGET(IODATA) & useEchoAddr) == useEchoAddr);
}

void SR04_Trig(void)
{
	dist = 0;
	isReady = 1;
	SR04_GpioRWrite(0);
	SR04_Delay_10us();
	SR04_GpioRWrite(1);
	SR04_Delay_10us();
	SR04_Delay_10us();
	SR04_Delay_10us();
	SR04_GpioRWrite(0);
}

void SR04_Isr(void)
{
	if(SR04_GpioRead())
	{
		dist++;
	}
	else if(!dist && isReady)
	{
		isReady = 0;
	}

}

unsigned char SR04_IsReady(void)
{
	return !isReady;
}

/*输出结果为厘米*/
unsigned int SR04_GetDistance(void)
{
	float Distance;

	if(SR04_GpioRead())
	{
		return -1;
	}
	/*超声波传播速度：340m/s=34000cm/s=0.034cm/us, Distance=(Ultrasonic_Count*0.034*20/2)*1.03,
	  其中乘1.03是为了修正误差加进去的
	  采集时间为5ms一个dist，总时间为 dist * 5
	*/
	Distance = dist * 5 * 0.34*1.03 * 100;
	return Distance;
}


void SR04_Init(unsigned char echoAddr, unsigned char trigAddr)
{
	unsigned char ioDir;

	useIoAddr = echoAddr;
	ioDir = GPIO_RGET(IODIR);
	ioDir |= echoAddr;

	GPIO_RSET(IODIR, ioDir);

	useTrigAddr = trigAddr;
	ioDir = GPIO_RGET(IODIR);
	ioDir &= ~useTrigAddr;

	GPIO_RSET(IODIR, ioDir);

	timerCreated(5);
	timerSetCallbcak(SR04_Isr);

}
/*****************************************************************************
            End of gpio.c
******************************************************************************/
