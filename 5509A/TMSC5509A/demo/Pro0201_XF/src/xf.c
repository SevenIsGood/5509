#include <stdio.h>

#include <csl.h>
#include <csl_pll.h>
#include <csl_chiphal.h>


void Delay(Uint32 ms);

PLL_Config  myConfig      = {
	0,    //IAI: the PLL locks using the same process that was underway
				//before the idle mode was entered
	1,    //IOB: If the PLL indicates a break in the phase lock,
				//it switches to its bypass mode and restarts the PLL phase-locking
				//sequence
	24,    //PLL multiply value; multiply 24 times
	1             //Divide by 2 PLL divide value; it can be either PLL divide value
				//(when PLL is enabled), or Bypass-mode divide value
				//(PLL in bypass mode, if PLL multiply value is set to 1)
};

void main(void)
{
	Uint32 xfchange;
	 /* Initialize CSL library - This is REQUIRED !!! */
	 /*CLS��ĳ�ʼ�������Ǳ����*/
	 CSL_init();

    /*����ϵͳ�������ٶ�Ϊ144MHz*/
    PLL_config(&myConfig);

	for(;;)
	{
	    //��ʱ1ms
		if(xfchange == 0)
		{
		   //����XF��LED
		   CHIP_FSET(ST1_55,XF,1);
		   xfchange = 1;
		   Delay(1);
		}
		else
		{
			//�ص�XF��LED
		  	CHIP_FSET(ST1_55,XF,0);
		  	xfchange = 0;
			Delay(1);
		}
	}

}


void Delay(Uint32 ms)
{
	Uint32 j = 0, k = 0;
	Uint32 msCnt = 10000/144;
	for(j = 0; j < ms; j++)
	{
		for(k = 0;k < msCnt; k++)
		{
			asm("_nop");
		}
	}
}
