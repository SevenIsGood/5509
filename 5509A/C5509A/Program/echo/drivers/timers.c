#include "5509.h"
#include "timer.h"
#include "main.h"
void timerInit(void)
{
	PC55XX_TIMER timer0=(PC55XX_TIMER)C55XX_TIM0_ADDR;
	timer0->tcr=0x0530;
	//timer0->tcr==0x00e0;
	timer0->tim=0;
	timer0->prd=0x0186a;
	timer0->prsc=1;
}
void timerStart(void)
{
	PC55XX_TIMER timer0=(PC55XX_TIMER)C55XX_TIM0_ADDR;
	timer0->tcr=0x0120;
}
void interrupt isrTimer0()
{
/*     ms++;
     request_time++;    
     link_time++;*/
	 isrTimer0Hook();
}
