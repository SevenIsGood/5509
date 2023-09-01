/******************************************************************************/
/*  Copyright 2004 by SEED Electronic Technology LTD.                         */
/*  All rights reserved. SEED Electronic Technology LTD.                      */
/*  Restricted rights to use, duplicate or disclose this code are             */
/*  granted through contract.                                                 */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
/*----------------------------------------------------------------------------*/
/* MODULE NAME... EMIF														  */
/* FILENAME...... emif.c													  */
/* DATE CREATED.. Wed 2/4/2004 												  */
/* PROJECT....... write and read data between the CPU and SDRAM				  */
/* COMPONENT..... 															  */
/* PREREQUISITS.. 															  */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/* DESCRIPTION:  															  */
/*   																		  */
/* This is an example for EMIF of C5509										  */
/*----------------------------------------------------------------------------*/
#include <csl.h>
#include <csl_pll.h>
#include <csl_emif.h>
#include <csl_chip.h>
#include <stdio.h>

Uint16 x;
Uint32 y;
CSLBool b;
unsigned int datacount = 0;
int databuffer[1000] ={0};
int *souraddr,*deminaddr;
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
/*SDRAM的EMIF设置*/
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
  0x7FFF,	//CE1_1:  CE0 space control register 1
  0xFFFF,	//CE1_2:  CE0 space control register 2
  0x00FF,	//CE1_3:  CE0 space control register 3
  
  0x7FFF,	//CE2_1:  CE0 space control register 1
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


main()
{
	unsigned int error=0;
	/*初始化CSL库*/	
    CSL_init();
    
    /*EMIF为全EMIF接口*/
    CHIP_RSET(XBSR,0x0a01);
    
    /*设置系统的运行速度为144MHz*/
    PLL_config(&myConfig);
    
    /*初始化DSP的外部SDRAM*/
    EMIF_config(&emiffig);
    /*向SDRAM中写入数据*/
    souraddr =  (int *)0x40000;
    deminaddr = (int *)0x41000;
    while(souraddr<deminaddr)
    {
    	*souraddr++ = datacount;	
    	datacount++	;
    }
    /*读出SRAM中的数据*/
    souraddr =  (int *)0x40000;
    datacount = 0;
    while(souraddr<deminaddr)
    {
    	databuffer[datacount++] = *souraddr++;
		if(databuffer[datacount-1]!=(datacount-1))
			error++;
    }
	if(error==0)
		printf("SDRAM test completed! No Error!");
    while(1);
}
/******************************************************************************\
* End of pll2.c
\******************************************************************************/
