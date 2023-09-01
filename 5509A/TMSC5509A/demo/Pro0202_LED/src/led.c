/*----------------------------------------------------------------------------*/
/* DESCRIPTION:  															  */
/*   																		  */
/* This is an example for gpio of C5509A								      */
/*----------------------------------------------------------------------------*/
#include <csl.h>
#include <csl_pll.h>
#include <csl_chip.h>
#include <csl_gpio.h>

void Delay(Uint32 ms);
/*锁相环的设置*/
PLL_Config  myConfig      = {
	0,    		//IAI: the PLL locks using the same process that was underway
				//before the idle mode was entered
	1,    		//IOB: If the PLL indicates a break in the phase lock,
				//it switches to its bypass mode and restarts the PLL phase-locking
				//sequence
	24,    		//PLL multiply value; multiply 24 times
	1           //Divide by 2 PLL divide value; it can be either PLL divide value
				//(when PLL is enabled), or Bypass-mode divide value
				//(PLL in bypass mode, if PLL multiply value is set to 1)
};

main()
{
	int i = 0;
	/*初始化CSL库*/
    CSL_init();

    /*设置系统的运行速度为144MHz*/
    PLL_config(&myConfig);


    /*确定方向为输出*/
    GPIO_RSET(IODIR,0xff);
    /*循环闪烁*/
    for(;;)
    {
    	 /*全亮*/
    	GPIO_RSET(IODATA,0xfe);
    	Delay(100);
   		/*全灭*/
   		GPIO_RSET(IODATA,0x0);
   		Delay(100);

    	GPIO_RSET(IODATA,0xff);
    	Delay(100);
   		/*全灭*/
   		GPIO_RSET(IODATA,0x01);
   		Delay(100);
    }

}

void Delay(Uint32 ms)
{
	Uint32 j = 0, k = 0;
	Uint32 msCnt = 144000;
	for(j = 0; j < ms; j++)
	{
		for(k = 0;k < msCnt; k++)
		{
			asm("_nop");
		}
	}
}
/*****************************************************************************
            End of gpio.c
******************************************************************************/
