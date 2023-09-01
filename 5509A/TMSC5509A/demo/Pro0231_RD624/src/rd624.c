#include <csl.h>
#include <csl_pll.h>
#include <csl_chip.h>
#include <csl_gpio.h>
#include "dht.h"

static unsigned char useIoAddr;

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

unsigned char RD624_StatusRead(void)
{
	return (GPIO_RGET(IODATA) & useIoAddr == useIoAddr);
}

void RD624_Init(unsigned char ioAddr)
{
	unsigned char ioDir;

	useIoAddr = ioAddr;
	ioDir = GPIO_RGET(IODIR);
	ioDir &= ~ioAddr;

	GPIO_RSET(IODIR, ioDir);


}
/*****************************************************************************
            End of gpio.c
******************************************************************************/
