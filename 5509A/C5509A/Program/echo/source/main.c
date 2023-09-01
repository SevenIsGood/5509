#include "5509.h"
#include "util.h"
#include <stdio.h>
PC55XX_MCSP mcbspHandle;
void main()
{
	unsigned long aic23data;
	pllInit(200);
	emifInit();
	intEnableGlobal(); 
	mcbspHandle=(PC55XX_MCSP)openMcbsp(0);
	AIC23_Init();
	while(1)
	{
	}	
}
void mcbspRcvHook()
{
	static unsigned short data;
	data=readMcbsp16(mcbspHandle);
    writeMcbsp16(data,mcbspHandle);
}
void mcbspXmitHook()
{
}