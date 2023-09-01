/*----------------------------------------------------------------------------*/
/* DESCRIPTION:  															  */
/*   																		  */
/* This is an example for gpio of C5509A								      */
/*----------------------------------------------------------------------------*/
#include <csl.h>
#include <csl_pll.h>
#include <csl_chip.h>
#include "spiflash.h"

/****************************************************************************/
/*                                                                          */
/*              全局变量                                                    */
/*                                                                          */
/****************************************************************************/
EMIF_Config emiffig = {
  0x221, 	//EGCR  : the MEMFREQ = 00,the clock for the memory is equal to cpu frequence
  			//		  the WPE = 0 ,forbiden the writing posting when we debug the EMIF
  			//        the MEMCEN = 1,the memory clock is reflected on the CLKMEM pin
  			//        the NOHOLD = 1,HOLD requests are not recognized by the EMIF
  0xFFFF,	//EMI_RST: any write to this register resets the EMIF state machine
  0x3FFF,	//CE0_1:  CE0 space control register 1
  			//        MTYPE = 011,Synchronous DRAM(SDRAM),16-bit data bus width
  0xFFFF,   //CE0_2:  CE0 space control register 2
  0x00FF,   //CE0_3:  CE0 space control register 3
  			//        TIMEOUT = 0xFF;
  0x1FFF,	//CE1_1:  CE0 space control register 1
            //        Asynchronous, 16Bit
  0xFFFF,	//CE1_2:  CE0 space control register 2
  0x00FF,	//CE1_3:  CE0 space control register 3
  0x1FFF,	//CE2_1:  CE0 space control register 1
            //        Asynchronous, 16Bit
  0xFFFF,	//CE2_2:  CE0 space control register 2
  0x00FF,	//CE2_3:  CE0 space control register 3

  0x7FFF,	//CE3_1:  CE0 space control register 1
  0xFFFF,	//CE3_2:  CE0 space control register 2
  0x00FF,	//CE3_3:  CE0 space control register 3

  0x2911,   //SDC1:   SDRAM control register 1
  			//		  TRC = 8
  			//        SDSIZE = 0;SDWID = 0
  			//        RFEN = 1
  			//        TRCD = 2
  			//        TRP  = 2
  0x0410,	//SDPER : SDRAM period register
  			//		  7ns *4096
  0x07FF,    //SDINIT: SDRAM initialization register
  			//        any write to this register to init the all CE spaces,
  			//        do it after hardware reset or power up the C55x device
  0x0131	//SDC2:	  SDRAM control register 2
  			//        SDACC = 0;
  			//        TMRD = 01;
  			//        TRAS = 0101;
  			//        TACTV2ACTV = 0001;
  };

/*锁相环的设置*/
PLL_Config  myConfig	  = {
	0, 	//IAI: the PLL locks using the same process that was underway
		//before the idle mode was entered
	1, 	//IOB: If the PLL indicates a break in the phase lock,
		//it switches to its bypass mode and restarts the PLL phase-locking
		//sequence
	24, //PLL multiply value; multiply 24 times
	1 	//Divide by 2 PLL divide value; it can be either PLL divide value
		//(when PLL is enabled), or Bypass-mode divide value
		//(PLL in bypass mode, if PLL multiply value is set to 1)
};

main()
{
	int i = 0, weight;
	/*初始化CSL库*/
    CSL_init();

   /*EMIF为全EMIF接口*/
	CHIP_RSET(XBSR,0x0a01);

	/*设置系统的运行速度为144MHz*/
	PLL_config(&myConfig);

	EMIF_config(&emiffig);

    /*循环闪烁*/
    for(;;)
    {
		HX711_Init();

		weight = HX711_GetWeight();

		Delay_1ms(1000);
    }

}

/*****************************************************************************
            End of gpio.c
******************************************************************************/
