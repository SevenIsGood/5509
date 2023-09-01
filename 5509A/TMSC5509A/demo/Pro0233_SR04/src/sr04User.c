/*----------------------------------------------------------------------------*/
/* DESCRIPTION:  															  */
/*   																		  */
/* This is an example for gpio of C5509A								      */
/*----------------------------------------------------------------------------*/
#include <csl.h>
#include <csl_pll.h>
#include <csl_chip.h>
#include "sr04.h"

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
	int i = 0, dist;
	/*初始化CSL库*/
    CSL_init();

    /*设置系统的运行速度为144MHz*/
    PLL_config(&myConfig);

	SR04_Init(0x40, 0x20);
    /*循环闪烁*/
    for(;;)
    {
		SR04_Trig();
		while(SR04_IsReady());//等待测量完成

		dist = SR04_GetDistance();

		Delay_1ms(1000);
    }

}

/*****************************************************************************
            End of gpio.c
******************************************************************************/
