#include <csl.h>
#include <csl_pll.h>
#include <csl_chip.h>
#include <csl_gpio.h>
#include "dht.h"

static unsigned char useIoAddr;
static unsigned int nVolADC0[10];
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

unsigned char MQ_GpioRead(void)
{
	return (GPIO_RGET(IODATA) & useIoAddr == useIoAddr);
}


unsigned char MQ_StatusRead(void)
{

	if(MQ_GpioRead())
	{
		Delay_1ms(1000);

		if(MQ_GpioRead())
		{
			return MQ_GpioRead();
		}
	}

	return 0;
}

/*输出结果为毫安*/
unsigned char MQ_GetVaule(void)
{
	unsigned char i = 0;
	/*采集通道0数据*/
	ADC_read(0, nADC0, 10);

	for(i = 1; i < 10; i++)
	{
		nADC0[0] += nADC0[i]
	}

	nADC0[0] = nADC0[0] / 10;

	if(nADC0[0] > 250) {
		nADC0[0] = 250;
	}

	return nADC0[0] * 20;
}


void MQ_Init(unsigned char ioAddr)
{
	unsigned char ioDir;

	useIoAddr = ioAddr;
	ioDir = GPIO_RGET(IODIR);
	ioDir &= ~ioAddr;

	GPIO_RSET(IODIR, ioDir);

	ADC_setFreq(0x23, 0, 0x4f);
}
/*****************************************************************************
            End of gpio.c
******************************************************************************/
