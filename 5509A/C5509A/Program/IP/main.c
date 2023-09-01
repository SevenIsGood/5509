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
#include <csl_irq.h>
#include <csl_pll.h>
#include <csl_emif.h>
#include <csl_chip.h>

#ifndef _CESECT_
#define _CESECT_
#define CESECT1 0x200000  //CE1空间基地址(16位)
#define CESECT2 0x400000  //CE2空间基地址(16位)
#define CESECT3 0x600000  //CE3空间基地址(16位)
#endif

Uint16 x;
Uint32 y;
CSLBool b;

Uint16  FrameLenth;
Uint16  RxSuccessFlag;
int		ms;
//-------------------
Uint16	TxEthnetFrameBuffer[1518/2];
Uint16	RxEthnetFrameBuffer[1518/2];

extern	struct	ipaddr	server_ipaddr;
Uint16	temp;

int *souraddr,*deminaddr;

Uint16 eventId0;
int old_intm;
interrupt void int1(void);

//---------Function prototypes---------
/* Reference start of interrupt vector table   */
/* This symbol is defined in file, vectors_IP.s55 */
extern void VECSTART(void);

/*锁相环的设置*/
PLL_Config  myConfig      = {
  0,    //IAI: the PLL locks using the same process that was underway 
                //before the idle mode was entered
  1,    //IOB: If the PLL indicates a break in the phase lock, 
                //it switches to its bypass mode and restarts the PLL phase-locking 
                //sequence
  12,    //PLL multiply value; multiply 12 times
  1             //Divide by 2 PLL divide value; it can be either PLL divide value 
                //(when PLL is enabled), or Bypass-mode divide value
                //(PLL in bypass mode, if PLL multiply value is set to 1)
};
/*EMIF设置*/
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
            //Aynchronous, 16Bit
  0xFFFF,	//CE1_2:  CE0 space control register 2
  0x00FF,	//CE1_3:  CE0 space control register 3
  
  0x1FFF,	//CE2_1:  CE0 space control register 1
            //Aynchronous, 16Bit
  0xFFFF,	//CE2_2:  CE0 space control register 2
  0x00FF,	//CE2_3:  CE0 space control register 3
  
  0x1FFF,	//CE3_1:  CE0 space control register 1
            //Aynchronous, 16Bit
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

//5509A中断设置，使能INT1中断
void INTconfig()
{
    /* Temporarily disable all maskable interrupts */
    IRQ_setVecs((Uint32)(&VECSTART));

	/* Temporarily disable all maskable interrupts */
    old_intm = IRQ_globalDisable();

	/* Get Event Id associated with External INT1(8019), for use with */
	eventId0 = IRQ_EVT_INT1;

	/* Clear any pending INT1 interrupts */
	IRQ_clear(eventId0);

	/* Place interrupt service routine address at */
    /* associated vector location */
    IRQ_plug(eventId0,&int1);

	/* Enable INT1(8019) interrupt */
    IRQ_enable(eventId0); 

	/* Enable all maskable interrupts */
    IRQ_globalEnable();    
}

main()
{
	/*初始化CSL库*/	
    CSL_init();
    
    /*EMIF为全EMIF接口*/
    CHIP_RSET(XBSR,0x0a01);
    
    /*设置系统的运行速度为144MHz*/
    PLL_config(&myConfig);
    
    /*初始化5509A的EMIF*/
    EMIF_config(&emiffig);

	//设置8019寄存器指针
	REGconfig();

    //复位8019
	RST8019();

	//片选(使能)8019
	CS8019();

	//初始化8019
	Init8019();

	//设置并使能5509A芯片的INT1中断(8019中断)
	INTconfig();

	ddelay(100);
	ArpRequest();

	while(1);
}

//External INT1(8019)中断，响应ICMP数据包
interrupt void int1()
{
   	   temp = RecFrame();
	   if(temp) 	DoNetworkStuff();
	   page(0);
	   delay(10);
	   deminaddr = (int *)0x200007;
	   *deminaddr = 0x00FF;
}

/******************************************************************************\
* End of pll2.c
\******************************************************************************/
