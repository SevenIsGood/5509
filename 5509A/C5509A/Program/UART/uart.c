/*----------------------------------------------------------------------------*/
/* DESCRIPTION:  															  */
/*   																		  */
/* This is an example for EMIF of C5509A					  				  */
/*----------------------------------------------------------------------------*/
/* MODULE NAME... Mcbsp												  */
/* FILENAME...... uart.c   												  */
/* DATE CREATED.. Mon 02/4/2004	    										  */
/* COMPONENT..... 															  */
/* PREREQUISITS.. 															  */
/*----------------------------------------------------------------------------*/
/* DESCRIPTION:																  */  
/*   																		  */
/* this example is that the software uart is done by Mcbsp */
/*----------------------------------------------------------------------------*/

#include <csl.h>
#include <csl_pll.h>
#include <csl_mcbsp.h>
#include <csl_irq.h>
#include <csl_timer.h>
#include <stdio.h>

//设置McBSP的PCR寄存器指针，用于读写其I/O端口
ioport unsigned int *PCR_1=(unsigned int *)0x2c12;
#define REG_PCR1 (*PCR_1)

/*锁相环的设置*/
PLL_Config  myConfig      = {
  0,    //IAI: the PLL locks using the same process that was underway 
                //before the idle mode was entered
  1,    //IOB: If the PLL indicates a break in the phase lock, 
                //it switches to its bypass mode and restarts the PLL phase-locking 
                //sequence
  12,    //PLL multiply value; multiply 12 times
  0             //Divide by 1 PLL divide value; it can be either PLL divide value 
                //(when PLL is enabled), or Bypass-mode divide value
                //(PLL in bypass mode, if PLL multiply value is set to 1)
};


/****************************设置Timer********************************/
/* 参考: TMS320VC5509 DSP Timers Reference Guide
         TMS320C55x Chip Support Library API Reference Guide (Rev. J)
		 HX-5509A 开发板使用手册                                     */
//---------Global data definition---------//
#define TIMER_CTRL    TIMER_TCR_RMK(\
                      TIMER_TCR_IDLEEN_DEFAULT,    /* IDLEEN == 0 */ \
                      TIMER_TCR_FUNC_OF(0),        /* FUNC   == 0 */ \
                      TIMER_TCR_TLB_RESET,         /* TLB    == 1 */ \
                      TIMER_TCR_SOFT_BRKPTNOW,     /* SOFT   == 0 */ \
                      TIMER_TCR_FREE_WITHSOFT,     /* FREE   == 0 */ \
                      TIMER_TCR_PWID_OF(0),        /* PWID   == 0 */ \
                      TIMER_TCR_ARB_RESET,         /* ARB    == 1 */ \
                      TIMER_TCR_TSS_START,         /* TSS    == 0 */ \
                      TIMER_TCR_CP_PULSE,          /* CP     == 0 */ \
                      TIMER_TCR_POLAR_LOW,         /* POLAR  == 0 */ \
                      TIMER_TCR_DATOUT_0           /* DATOUT == 0 */ \
)  

/* Create a TIMER configuration structure that can be passed */
/* to TIMER_config CSL function for initialization of Timer  */
/* control registers.                                        */
//产生1ms中断
TIMER_Config timCfg0 = {
   TIMER_CTRL,               /* TCR0 */
   749,                     /* PRD0 */
   0x0000                   /* PRSC */
};

/* Create a TIMER_Handle object for use with TIMER_open */
TIMER_Handle mhTimer0;

Uint16 eventId0;
Uint16 timer0_cnt = 0;
int old_intm;
Uint16 tim_val;

//---------Function prototypes---------
/* Reference start of interrupt vector table   */
/* This symbol is defined in file, vectors_timer1.s55 */
extern void VECSTART(void);

/* Function/ISR prototypes */
interrupt void timer0Isr(void);
/***********************************************************************/


/**************************McBSP1设置成GPIO*****************************/
/* 参考: TMS320C55x DSP Multichannel Buffered Serial Port (McBSP) Reference Guide
         HX-5509A 开发板使用手册                                       
		 TMS320C55x Chip Support Library API Reference Guide (Rev. J)  */
MCBSP_Config Mcbsptest;

MCBSP_Config Mcbsp1Config = {
  MCBSP_SPCR1_RMK(
    MCBSP_SPCR1_DLB_OFF,                   /* DLB    = 0,禁止自闭环方式 */
    MCBSP_SPCR1_RJUST_LZF,                 /* RJUST  = 2 */
    MCBSP_SPCR1_CLKSTP_DISABLE,            /* CLKSTP = 0 */
    MCBSP_SPCR1_DXENA_ON,                  /* DXENA  = 1 */
    0,                                     /* ABIS   = 0 */
    MCBSP_SPCR1_RINTM_RRDY,                /* RINTM  = 0 */
    0,                                     /* RSYNCER = 0 */
    MCBSP_SPCR1_RRST_DISABLE               /* RRST   = 0 */
   ),
    MCBSP_SPCR2_RMK(
    MCBSP_SPCR2_FREE_NO,                   /* FREE   = 0 */
    MCBSP_SPCR2_SOFT_NO,                   /* SOFT   = 0 */
    MCBSP_SPCR2_FRST_FSG,                  /* FRST   = 0 */
    MCBSP_SPCR2_GRST_CLKG,                 /* GRST   = 0 */
    MCBSP_SPCR2_XINTM_XRDY,                /* XINTM  = 0 */
    0,                                     /* XSYNCER = N/A */           
    MCBSP_SPCR2_XRST_DISABLE               /* XRST   = 0 */
   ),
   /*单数据相，接受数据长度为16位,每相2个数据*/
  MCBSP_RCR1_RMK( 
  	MCBSP_RCR1_RFRLEN1_OF(1),              /* RFRLEN1 = 1 */
  	MCBSP_RCR1_RWDLEN1_16BIT               /* RWDLEN1 = 2 */
  ),
  MCBSP_RCR2_RMK(    
    MCBSP_RCR2_RPHASE_SINGLE,              /* RPHASE  = 0 */
    MCBSP_RCR2_RFRLEN2_OF(0),              /* RFRLEN2 = 0 */
    MCBSP_RCR2_RWDLEN2_8BIT,               /* RWDLEN2 = 0 */
    MCBSP_RCR2_RCOMPAND_MSB,               /* RCOMPAND = 0 */
    MCBSP_RCR2_RFIG_YES,                   /* RFIG    = 0 */
    MCBSP_RCR2_RDATDLY_1BIT                /* RDATDLY = 1 */
    ),  
   MCBSP_XCR1_RMK(    
    MCBSP_XCR1_XFRLEN1_OF(1),              /* XFRLEN1 = 1 */ 
    MCBSP_XCR1_XWDLEN1_16BIT               /* XWDLEN1 = 2 */
    
 ),   
 MCBSP_XCR2_RMK(   
    MCBSP_XCR2_XPHASE_SINGLE,              /* XPHASE  = 0 */
    MCBSP_XCR2_XFRLEN2_OF(0),              /* XFRLEN2 = 0 */
    MCBSP_XCR2_XWDLEN2_8BIT,               /* XWDLEN2 = 0 */
    MCBSP_XCR2_XCOMPAND_MSB,               /* XCOMPAND = 0 */
    MCBSP_XCR2_XFIG_YES,                   /* XFIG    = 0 */
    MCBSP_XCR2_XDATDLY_1BIT                /* XDATDLY = 1 */
  ),            
 MCBSP_SRGR1_DEFAULT,
 MCBSP_SRGR2_DEFAULT,  
 MCBSP_MCR1_DEFAULT,
 MCBSP_MCR2_DEFAULT, 
 MCBSP_PCR_RMK(
   MCBSP_PCR_IDLEEN_RESET,                 /* IDLEEN   = 0   */
   MCBSP_PCR_XIOEN_GPIO,                   /* XIOEN    = 1   */
   MCBSP_PCR_RIOEN_GPIO,                   /* RIOEN    = 1   */
   MCBSP_PCR_FSXM_EXTERNAL,                /* FSXM     = 0   */
   MCBSP_PCR_FSRM_EXTERNAL,                /* FSRM     = 0   */
   MCBSP_PCR_CLKXM_INPUT,                  /* CLKXM    = 0   */
   MCBSP_PCR_CLKRM_INPUT,                  /* CLKRM    = 0   */
   MCBSP_PCR_SCLKME_NO,                    /* SCLKME   = 0   */
   MCBSP_PCR_DXSTAT_1,                     /* DXSTAT   = 1   */
   MCBSP_PCR_FSXP_ACTIVEHIGH,              /* FSXP     = 0   */
   MCBSP_PCR_FSRP_ACTIVEHIGH,              /* FSRP     = 1   */
   MCBSP_PCR_CLKXP_FALLING,                /* CLKXP    = 1   */
   MCBSP_PCR_CLKRP_RISING                  /* CLKRP    = 1   */
 ),
 MCBSP_RCERA_DEFAULT, 
 MCBSP_RCERB_DEFAULT, 
 MCBSP_RCERC_DEFAULT, 
 MCBSP_RCERD_DEFAULT, 
 MCBSP_RCERE_DEFAULT, 
 MCBSP_RCERF_DEFAULT, 
 MCBSP_RCERG_DEFAULT, 
 MCBSP_RCERH_DEFAULT, 
 MCBSP_XCERA_DEFAULT,
 MCBSP_XCERB_DEFAULT,
 MCBSP_XCERC_DEFAULT,
 MCBSP_XCERD_DEFAULT,  
 MCBSP_XCERE_DEFAULT,
 MCBSP_XCERF_DEFAULT,  
 MCBSP_XCERG_DEFAULT,
 MCBSP_XCERH_DEFAULT
};     

MCBSP_Handle hMcbsp;
/***********************************************************************/


Uint16 i,temp;

unsigned char timer0_cnten;
unsigned char uart_rec, uart_send, uart_recflag, uart_sendflag;
unsigned char uart_bitscan, uart_bitcnt, uart_recstart, uart_sendbitcnt;
unsigned char uart_bitsend, uart_bitsendcnt, uart_sendcnten, uart_sendend;

void delay(Uint32 k)
{
   while(k--);
}

void settimer(void)
{
	/* Set IVPH/IVPD to start of interrupt vector table */
	/*修改寄存器IVPH，IVPD，重新定义中断向量表*/
	IRQ_setVecs((Uint32)(&VECSTART));
	 	
	/* Temporarily disable all maskable interrupts */
    old_intm = IRQ_globalDisable();   

    /* Open Timer 0, set registers to power on defaults */
    mhTimer0 = TIMER_open(TIMER_DEV0, TIMER_OPEN_RESET);

    /* Get Event Id associated with Timer 0, for use with */
    /* CSL interrupt enable functions.                    */         
    eventId0 = TIMER_getEventId(mhTimer0);

    /* Clear any pending Timer interrupts */
    IRQ_clear(eventId0);
    
    /* Place interrupt service routine address at */
    /* associated vector location */
    IRQ_plug(eventId0,&timer0Isr);              

    /* Write configuration structure values to Timer control regs */ 
    TIMER_config(mhTimer0, &timCfg0);  
 
    /* Enable Timer interrupt */
    IRQ_enable(eventId0);             
   
    /* Enable all maskable interrupts */
    IRQ_globalEnable();   
    
    timer0_cnten = 0; 
    uart_bitscan = 0;
    uart_bitcnt = 0;  
	uart_recstart = 0;
	uart_recflag = 0;
	uart_sendflag = 0;
 
    /* Start Timer */
    TIMER_start(mhTimer0);
}


void main(void)
{
    /* Initialize CSL library - This is REQUIRED !!! */
    /*初始化CSL库*/
    CSL_init();
	
	/*设置系统的运行速度为144MHz*/
    PLL_config(&myConfig);
    
	/*初始化McBSP1*/
	hMcbsp = MCBSP_open(MCBSP_PORT1,MCBSP_OPEN_RESET);
	/*设置McBSP1*/
	MCBSP_config(hMcbsp,&Mcbsp1Config);

    MCBSP_getConfig(hMcbsp,&Mcbsptest);

	settimer();
	    	   
/*************************以下程序实现软件Uart************************/
/* 参考：HX-5509A 开发板使用手册                                     */
	while(TRUE)
	{
		if(uart_recflag==1)
		{
			uart_recflag = 0;
			if(uart_sendflag==0)
			{
				uart_bitsendcnt = 0;
				uart_sendend = 0;
				uart_sendcnten = 0;
				uart_bitsend = 0;
				uart_send = uart_rec;
				uart_sendflag = 1;
				uart_sendbitcnt = 0;
			}
		}
	};     	   	    	       	   	    	   
}


interrupt void timer0Isr(void)
{
	temp = REG_PCR1;

	//Send
	if(uart_sendflag==1)
	{
		if(uart_sendcnten==0)
		{
			REG_PCR1 &= 0xffdf;
			uart_bitsend = 1;
			uart_sendcnten = 1;
		}
		if(uart_sendcnten==1)
		{
			uart_bitsendcnt++;
			if(uart_bitsendcnt==10)
			{
				uart_sendbitcnt++;
				if(uart_sendend==1)
				{
					uart_sendflag = 0;
				}
				else
				{
					if(uart_sendbitcnt<9)
					{
						if((uart_send & uart_bitsend)==0)
						{
							REG_PCR1 &= 0xffdf;
						}
						else
						{
							REG_PCR1 |= 0x0020;
						}
						uart_bitsend <<= 1;
					}
					else
					{
						REG_PCR1 |= 0x0020;
                        uart_sendflag = 0; 
					}
				}
				uart_bitsendcnt = 0;
			}
		}
	}
	
	//Receive
	if((uart_bitcnt==0)&&(uart_recstart==0))
	{
		if((temp&0x0010)==0)
		{
			timer0_cnt = 0;
			timer0_cnten = 1;
			uart_bitscan = 0;
			uart_rec = 0;
			uart_recstart = 1;
		}
	}
	if(timer0_cnten==1)
	{
		timer0_cnt++;
		if((temp&0x0010)!=0)
		{
			uart_bitscan++;
		}
		if(timer0_cnt==10)
		{
			timer0_cnt = 0;
			if(uart_bitscan>5)
			{
				switch(uart_bitcnt)
				{
					case 0:  uart_rec = 0;
					         timer0_cnten = 0;
							 uart_recstart = 0;
							 break;
					case 1:  uart_bitcnt++;
							 uart_rec |= 0x01;
							 break;
					case 2:  uart_bitcnt++;
							 uart_rec |= 0x02;
							 break;
					case 3:  uart_bitcnt++;
							 uart_rec |= 0x04;
							 break;
					case 4:  uart_bitcnt++;
							 uart_rec |= 0x08;
							 break;
					case 5:  uart_bitcnt++;
							 uart_rec |= 0x10;
							 break;
					case 6:  uart_bitcnt++;
							 uart_rec |= 0x20;
							 break;
					case 7:  uart_bitcnt++;
							 uart_rec |= 0x40;
							 break;
					case 8:  uart_bitcnt++;
							 uart_rec |= 0x80;
							 break;
					case 9:  uart_bitcnt = 0;
							 timer0_cnten = 0;
							 uart_recflag = 1;
							 uart_recstart = 0;
							 break;
					default: break;
				}
			}
			else
			{
			    uart_bitcnt++;
				if(uart_bitcnt>9)
				{
					uart_bitcnt = 0;
					timer0_cnten = 0;
				}
			}
			uart_bitscan = 0;
		}		
	}

}

/******************************************************************************\
* End of uart.c
\******************************************************************************/
