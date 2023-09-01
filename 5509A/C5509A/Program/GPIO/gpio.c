/******************************************************************************/
/*  Copyright 2004 by SEED Electronic Technology LTD.                         */
/*  All rights reserved. SEED Electronic Technology LTD.                      */
/*  Restricted rights to use, duplicate or disclose this code are             */
/*  granted through contract.                                                 */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
/*----------------------------------------------------------------------------*/
/* MODULE NAME... GPIO														  */
/* FILENAME...... gpio.c													  */
/* DATE CREATED.. Wed 2/4/2004 												  */
/* PROJECT....... write and read data between the CPU and SDRAM				  */
/* COMPONENT..... 															  */
/* PREREQUISITS.. 															  */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/* DESCRIPTION:  															  */
/*   																		  */
/* This is an example for gpio of C5509										  */
/*----------------------------------------------------------------------------*/
#include <csl.h>
#include <csl_pll.h>
#include <csl_chip.h>
#include <csl_gpio.h>

void delay();
/*锁相环的设置*/
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

main()
{
	int i = 0;
	/*初始化CSL库*/	
    CSL_init();
    
    /*设置系统的运行速度为144MHz*/
    PLL_config(&myConfig);
    
    /*确定方向为输出*/
    GPIO_RSET(IODIR,0xFF);
    for(i = 0;i<0x0a;)
    {
    	 /*全亮*/
    	GPIO_RSET(IODATA,0xff);
    	delay();
   		/*全灭*/
   		GPIO_RSET(IODATA,0x0);
   		delay();
    }
	/*循环闪烁*/   
   	for(;;)
   	{
   		GPIO_RSET(IODATA,0x1);
   		delay();
		delay();
   		GPIO_RSET(IODATA,0x2);
   		delay();
		delay();
   		GPIO_RSET(IODATA,0x4);
   		delay();
		delay();
   		GPIO_RSET(IODATA,0x8);
   		delay();
		delay();
   		GPIO_RSET(IODATA,0x10);
   		delay();
		delay();
   		GPIO_RSET(IODATA,0x40);
   		delay();
		delay();
   		GPIO_RSET(IODATA,0x80);
   		delay();
   		delay();	
   	}			  
}
void delay()
{
	Uint32 j = 0,k = 0;
	for(j = 0;j<0x0c;j++)
	{
		for(k= 0;k<0xffff;k++)
		{}
	}
}
/*****************************************************************************
            End of gpio.c
******************************************************************************/
