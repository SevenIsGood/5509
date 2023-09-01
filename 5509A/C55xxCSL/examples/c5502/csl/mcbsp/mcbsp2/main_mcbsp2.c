/*
 * Copyright (C) 2003 Texas Instruments Incorporated
 * All Rights Reserved
 */
/*
 *---------main_mcbsp2.c---------
 *
 * This Example configures the serial port for digital loopback mode
 * to transfer 32-bit of data.
 * We then use the ISR's to write/read from the port. In loopback mode, we
 * should read back the same value as written, which is checked for in a loop
 */
#include <stdio.h>
#include <csl.h>
#include <csl_mcbsp.h>
#include <csl_irq.h>

//---------Global constants---------
#define SIZE			1024

//---------Global data definition---------

//Handle for McBSP
static volatile MCBSP_Handle hMcbsp;

//Buffers to store data
static volatile Uint32 writeBuff[SIZE], readBuff[SIZE];

//Event id's
Uint16 rcvEventID, xmtEventID;

Uint16 cnt=0;

//Transfer Complete code for McBSP
static volatile int transferComplete=0;

/* create a MCBSP config structure for digital loopback mode */
static MCBSP_Config ConfigLoopBack32= {
		  MCBSP_SPCR1_RMK(
		    MCBSP_SPCR1_DLB_ON,                    /* DLB    = 1 */
		    MCBSP_SPCR1_RJUST_RZF,                 /* RJUST  = 0 */
		    MCBSP_SPCR1_CLKSTP_DISABLE,            /* CLKSTP = 0 */
		    MCBSP_SPCR1_DXENA_NA,                  /* DXENA  = 0 */
		    MCBSP_SPCR1_ABIS_DISABLE,              /* ABIS   = 0 */
		    MCBSP_SPCR1_RINTM_RRDY,                /* RINTM  = 0 */
		    0,                                     /* RSYNCER = 0 */
		    MCBSP_SPCR1_RRST_DISABLE               /* RRST   = 0 */
		  ),
		  MCBSP_SPCR2_RMK(
		    MCBSP_SPCR2_FREE_NO,                   /* FREE   = 0 */
		    MCBSP_SPCR2_SOFT_NO,                   /* SOFT   = 0 */
		    MCBSP_SPCR2_FRST_RESET,                /* FRST   = 0 */
		    MCBSP_SPCR2_GRST_RESET,                /* GRST   = 0 */
		    MCBSP_SPCR2_XINTM_XRDY,                /* XINTM  = 0 */
		    0,                                     /* XSYNCER = N/A */
		    MCBSP_SPCR2_XRST_DISABLE               /* XRST   = 0 */
		  ),
		  MCBSP_RCR1_RMK(
		    MCBSP_RCR1_RFRLEN1_OF(0),                /* RFRLEN1 = 0 */
		    MCBSP_RCR1_RWDLEN1_32BIT                 /* RWDLEN1 = 5 */
		  ),
		  MCBSP_RCR2_RMK(
		    MCBSP_RCR2_RPHASE_SINGLE,              /* RPHASE  = 0 */
		    MCBSP_RCR2_RFRLEN2_OF(0),              /* RFRLEN2 = 0 */
		    MCBSP_RCR2_RWDLEN2_8BIT,               /* RWDLEN2 = 0 */
		    MCBSP_RCR2_RCOMPAND_MSB,               /* RCOMPAND = 0 */
		    MCBSP_RCR2_RFIG_YES,                   /* RFIG    = 0 */
		    MCBSP_RCR2_RDATDLY_2BIT                /* RDATDLY = 0 */
		  ),
		  MCBSP_XCR1_RMK(
		    MCBSP_XCR1_XFRLEN1_OF(0),              /* XFRLEN1 = 0 */
		    MCBSP_XCR1_XWDLEN1_32BIT               /* XWDLEN1 = 5 */

		  ),
		  MCBSP_XCR2_RMK(
		    MCBSP_XCR2_XPHASE_SINGLE,              /* XPHASE  = 0 */
		    MCBSP_XCR2_XFRLEN2_OF(0),              /* XFRLEN2 = 0 */
		    MCBSP_XCR2_XWDLEN2_8BIT,               /* XWDLEN2 = 0 */
		    MCBSP_XCR2_XCOMPAND_MSB,               /* XCOMPAND = 0 */
		    MCBSP_XCR2_XFIG_YES,                   /* XFIG    = 0 */
		    MCBSP_XCR2_XDATDLY_2BIT                /* XDATDLY = 0 */
		  ),
		  MCBSP_SRGR1_RMK(
		    MCBSP_SRGR1_FWID_OF(1),                /* FWID    = 1 */
		    MCBSP_SRGR1_CLKGDV_OF(1)               /* CLKGDV  = 1 */
		  ),
		  MCBSP_SRGR2_RMK(
		    MCBSP_SRGR2_GSYNC_FREE,                /* FREE    = 0 */
		    MCBSP_SRGR2_CLKSP_RISING,              /* CLKSP   = 0 */
		    MCBSP_SRGR2_CLKSM_INTERNAL,            /* CLKSM   = 1 */
		    MCBSP_SRGR2_FSGM_DXR2XSR,              /* FSGM    = 0 */
		    MCBSP_SRGR2_FPER_OF(15)                /* FPER    = 0 */
		  ),
		  MCBSP_MCR1_DEFAULT,
		  MCBSP_MCR2_DEFAULT,
		  MCBSP_PCR_RMK(
		  	MCBSP_PCR_XIOEN_SP,                     /* XIOEN    = 0   */
		    MCBSP_PCR_RIOEN_SP,                     /* RIOEN    = 0   */
		    MCBSP_PCR_FSXM_INTERNAL,                /* FSXM     = 1   */
		    MCBSP_PCR_FSRM_EXTERNAL,                /* FSRM     = 0   */
		    MCBSP_PCR_CLKXM_OUTPUT,                 /* CLKXM    = 1   */
		    MCBSP_PCR_CLKRM_INPUT,                  /* CLKRM    = 0   */
		    MCBSP_PCR_SCLKME_NO,                    /* SCLKME   = 0   */
		    0,                                      /* DXSTAT = N/A   */
		    MCBSP_PCR_FSXP_ACTIVEHIGH,              /* FSXP     = 0   */
		    MCBSP_PCR_FSRP_ACTIVEHIGH,              /* FSRP     = 0   */
		    MCBSP_PCR_CLKXP_RISING,                 /* CLKXP    = 0   */
		    MCBSP_PCR_CLKRP_FALLING                 /* CLKRP    = 0   */
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

//---------Function prototypes---------

//ISR's to send and receive data through McBSP
interrupt void writeISR(void);
interrupt void readISR(void);

/* Reference the start of the interrupt vector table */
/* This symbol is defined in file vectors.s55        */
extern void VECSTART(void);

//---------main routine---------
void main()
{

	volatile Uint32 x,y;
	int success = 1,i,gie;

	// Fill the buffers with some patterns
	for (i = 0; i <= SIZE - 1; i++) {
		//Set up src buffer
	    writeBuff[i] = ((Uint32)i << 16) + i;
	    //Corrupt dst buffer
	    readBuff[i] = 0xdeaddead;
    }

    //Disable interrupt
    gie=IRQ_globalDisable();
    //Open McBSP channel
    hMcbsp = MCBSP_open(MCBSP_PORT0, MCBSP_OPEN_RESET);

    /* Get EventId's associated with MCBSP Port 0 receive and transmit */
    rcvEventID = MCBSP_getRcvEventId(hMcbsp);
    xmtEventID = MCBSP_getXmtEventId(hMcbsp);

	IRQ_setVecs((Uint32)(&VECSTART));
    /* Clear any pending receive or transmit interrupts */
    IRQ_clear(rcvEventID);
    IRQ_plug(rcvEventID, &readISR);

    IRQ_clear(xmtEventID);
    IRQ_plug(xmtEventID, &writeISR);

    /* Write values from configuration structure to MCBSP control regs */
    MCBSP_config(hMcbsp, &ConfigLoopBack32);

    /* Enable the MCBSP receive and transmit interrupts */
    IRQ_enable(rcvEventID);
    IRQ_enable(xmtEventID);

    /* Start Sample Rate Generator and Frame Sync */
	MCBSP_start(hMcbsp,
	  MCBSP_SRGR_START | MCBSP_SRGR_FRAMESYNC,
	  0x300
	);

	/* Enable MCBSP transmit and receive */
	MCBSP_start(hMcbsp,
	  MCBSP_RCV_START | MCBSP_XMIT_START,
	  0
	);

	IRQ_globalEnable();

	while(!transferComplete);

	for(i=0;i<SIZE;i++)
	{
		if(readBuff[i] != writeBuff[i])
		{
			success=0;
			break;
		}
	}

	IRQ_globalRestore(gie);
	MCBSP_close(hMcbsp);

	if(success)
		printf("TEST PASSED\n");
	else
		printf("TEST FAILED\n");
}

// ISR to transmit data thro McBSP
interrupt void writeISR(void)
{
	IRQ_disable(xmtEventID);
	MCBSP_write32(hMcbsp,writeBuff[cnt]);
}

// ISR to receive data
interrupt void readISR(void)
{
    readBuff[cnt] = MCBSP_read32(hMcbsp);
    cnt++;

    if (cnt == SIZE) {
        IRQ_disable(rcvEventID);
        IRQ_disable(xmtEventID);
        MCBSP_reset(hMcbsp);
        transferComplete=1;
    }
    else {
        IRQ_enable(xmtEventID);
    }
}
