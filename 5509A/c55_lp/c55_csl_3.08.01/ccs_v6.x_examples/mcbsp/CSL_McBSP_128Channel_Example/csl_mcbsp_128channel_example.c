/* ============================================================================
 * Copyright (c) 2008-2016 Texas Instruments Incorporated.
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/


/** @file McBSP_Master_AIC3204_48kbps_DMA.c
 *
 *  @brief Test code to verifes McBSP  in 128 channel mode
 *
 * \page    page9  McBSP EXAMPLE DOCUMENTATION
 *
 * \section McBSP5   McBSP EXAMPLE5 - 128 channel mode
 *
 * \subsection McBSP5x    TEST DESCRIPTION:
 * This test is to verify the McBSP TX-RX external loopback in 128 channel mode.
 *
 * \subsection McBSP5y    TEST PROCEDURE AND EXPECTED RESULT:
 * This test configures PLL to be 100MHz
 *
 * This is an External Loop-back Test
 * The following connections are required on J14 of EVM5517
 *
 * 1. McBSP_CLKX(pin#12) to  pin 20 on J15 (or) McBSP_CLKR(Test point -TP#15)
 * 2. McBSP_FSX(pin#8) to McBSP_FSR(pin#1)
 * 3. McBSP_DX(pin#22) to McBSP_DR(pin#21)
 *
 * USE_DMA macro:
 * Uncomment this to run in DMA mode.
 * Commenting this makes the test in POLL mode.
 *
 *
 * Brief description of the procedure and expected result to be checked:
 * 1. Configure DMA3 Channel0: 1Word Burst, McBSP Tx Dma event, write mode,
 *    global tx array addr as src and McBSP TXRL as dst, IO-mem transfer and suitable
 *    datalength, dma interrupt enabled.
 * 2. Configure DMA3 Channel1: 1Word Burst, McBSP Rx Dma event, read mode,
 *    McBSP RXRL as src and global rx array addr as dst, IO-mem transfer and suitable
 *    datalength, dma interrupt enabled.
 * 3. Configure McBSP: 128 channel mode (using R/XMCME, R/XMCM, R/XEMODE,
 *    R/XCERA/B/C/D/E/F/G/H), 32 Word wordlenth, framelenth 128, single phase, 0-bit data delay,
 *    r-justified msb first, clksrc sysclk, frame sync selection, sample rate
 * 4. Start DMA3 channels 0&1
 * 5. Enable and start McBSP Tx, Rx and Frame Sync Generator
 * 6. Wait for Rx and Tx DMA event completion in the DMA interrupt flag register
 * 7. Check Data in rx array for integrity vis-a-vis the tx array
*/
/* ============================================================================
 * Revision History
 * ================
 * 15-Apr-2015 Created
 * 10-Mar-2016 Updates to header
 * ============================================================================
 */


/*
 *---------main_mcbsp1.c---------
 * In this example, the MCBSP is configured in digital loopback
 * mode, with 32 bit data transfer, in multi-frame mode, using                                                   
 * sample rate generator to sync frames                        
 */
 
#include <stdio.h>
#include <csl_mcbsp.h>
#include <csl_dma.h>
#include "csl_mcbsp_tgt.h"

#define USE_DMA 1

#define CSL_MCBSP_TEST_PASSED      (0)
#define CSL_MCBSP_TEST_FAILED      (1)

//---------Global constants---------
#define N 1024    

int j=0;
CSL_McbspHandle  hMcbsp;
CSL_McbspObj 	 McbspObj;              

/* Create a MCBSP configuration structure */
CSL_McbspSetup McBSPconfig;   

/* CSL DMA data structures used for McBSP data transfers */
CSL_DMA_Handle dmaTxHandle;
CSL_DMA_Handle dmaRxHandle;
CSL_DMA_Config dmaConfig;
CSL_DMA_ChannelObj    dmaTxChanObj;
CSL_DMA_ChannelObj    dmaRxChanObj;  
  
/* Create data buffers for transfer */
Uint32 xmt[N], rcv[N]; 
Uint16	mcbsp_rx_count = 0;
Uint16	mcbsp_tx_count = 0;

//---------Function prototypes---------
CSL_Status CSL_McBSPTest(void);
CSL_DMA_Handle CSL_configDmaForMcbsp(CSL_DMA_ChannelObj    *dmaChanObj,
                                    CSL_DMAChanNum        chanNum);
CSL_Status ioExpander_Setup(void);
CSL_Status ioExpander_Read(Uint16 port, Uint16 pin, Uint16 *rValue);
CSL_Status ioExpander_Write(Uint16 port, Uint16 pin, Uint16 value);

/*Reference the start of the interrupt vector table*/
extern void VECSTART(void);

/////INSTRUMENTATION FOR BATCH TESTING -- Part 1 --
/////  Define PaSs_StAtE variable for catching errors as program executes.
/////  Define PaSs flag for holding final pass/fail result at program completion.
volatile Int16 PaSs_StAtE = 0x0001; // Init to 1. Reset to 0 at any monitored execution error.
volatile Int16 PaSs = 0x0000; // Init to 0.  Updated later with PaSs_StAtE when and if
/////                                  program flow reaches expected exit point(s).
/////


void main() {
  CSL_Status result; 
  
  printf("\r\nMCBSP 128 channel mode test case!!\n");

  result = CSL_McBSPTest();
  if(result == CSL_MCBSP_TEST_PASSED)
  {
	printf("\nMcBSP 128 channel mode passed!!\n");
  }
  else
  {
	printf("\nMcBSP 128 channel mode failed!!\n");
	PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
  }

  /////INSTRUMENTATION FOR BATCH TESTING -- Part 3 --
  /////  At program exit, copy "PaSs_StAtE" into "PaSs".
  PaSs = PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with
  /////                   // pass/fail value determined during program execution.
  /////  Note:  Program should next exit to C$$EXIT and halt, where DSS, under
  /////   control of a host PC script, will read and record the PaSs' value.
  /////

  
}

CSL_Status CSL_McBSPTest(void) 
{

#if (defined(CHIP_C5517))
  Uint16 i;
  unsigned char error=0;
  CSL_Status status; //This is for the CSL status return

  /* Test Requires SW4 HPI_ON to be OFF */
    /* McBSP@ J14 pins:  12(CLKX),8(FSX),22(DX),2(DR),1(FSR); and J15:  20(CLKR)*/
  
  ioExpander_Setup();

  //Call this fn for enabling

  // port,pin, value

  // port,pin, value
  ioExpander_Write(1, 0, 1); // Select SEL_MMC0_I2S


  /* Initilize data buffers. xmt will be 8 bit value      */
  /* with element number in 8 bits  */
  for(i=0;i<=N-1;i++) 
  {
    xmt[i] = (((Uint32)0xFFFF << 16) | i);
    rcv[i] = 0;
  }

  /* Open MCBSP Port 0, this will return a MCBSP handle that will */
  /* be used in calls to other CSl functions.                     */ 
  hMcbsp = MCBSP_open(CSL_MCBSP_INST_0, &McbspObj,&status);
  if ( (hMcbsp == NULL) || (status != CSL_SOK) )
  {
   printf("MCBSP_open() Failed \n");
  }
  else
  {
	printf("MCBSP_open() Success \n");
  }

  CSL_FINS(hMcbsp->sysRegs->EBSR,SYS_EBSR_SP0MODE,CSL_SYS_EBSR_SP0MODE_MODE3);

#ifdef USE_DMA
  /* Initialize Dma */
  status = DMA_init();
  if (status != CSL_SOK)
  {
    printf("DMA_init Failed!\n");
  }


    /* Configure the DMA channel for mcbsp transmit */
#if ((defined(CHIP_C5505_C5515)) || (defined(CHIP_C5504_C5514)) || (defined(CHIP_C5517)) || (defined(CHIP_C5535) || defined(CHIP_C5545)))
	dmaConfig.pingPongMode = CSL_DMA_PING_PONG_DISABLE;
#endif
	dmaConfig.autoMode = CSL_DMA_AUTORELOAD_ENABLE;
	dmaConfig.burstLen = CSL_DMA_TXBURST_1WORD;
	dmaConfig.trigger  = CSL_DMA_EVENT_TRIGGER;
	dmaConfig.dmaEvt   = CSL_DMA_EVT_MCBSP_TX;
	dmaConfig.dmaInt   = CSL_DMA_INTERRUPT_ENABLE;
	dmaConfig.chanDir  = CSL_DMA_WRITE;
	dmaConfig.trfType  = CSL_DMA_TRANSFER_IO_MEMORY;
	dmaConfig.dataLen  = (N*4);
	dmaConfig.srcAddr  = (Uint32)xmt;
	dmaConfig.destAddr = (Uint32)&(hMcbsp->Regs->DXRL);

    dmaTxHandle = CSL_configDmaForMcbsp(&dmaTxChanObj, CSL_DMA_CHAN12);
	if(dmaTxHandle == NULL)
	{
		printf("DMA Config for mcbsp DMA Write Failed!\n!");
		return(CSL_MCBSP_TEST_FAILED);
	}

    

    // Configure the DMA channel for mcbsp receive 
#if ((defined(CHIP_C5505_C5515)) || (defined(CHIP_C5504_C5514)) || (defined(CHIP_C5517)) || (defined(CHIP_C5535) || defined(CHIP_C5545)))
	dmaConfig.pingPongMode = CSL_DMA_PING_PONG_DISABLE;
#endif
	dmaConfig.autoMode = CSL_DMA_AUTORELOAD_ENABLE;
	dmaConfig.burstLen = CSL_DMA_TXBURST_1WORD;
    dmaConfig.trigger  = CSL_DMA_EVENT_TRIGGER;
    dmaConfig.dmaEvt   = CSL_DMA_EVT_MCBSP_RX;
	dmaConfig.dmaInt   = CSL_DMA_INTERRUPT_ENABLE;
	dmaConfig.chanDir  = CSL_DMA_READ;
	dmaConfig.trfType  = CSL_DMA_TRANSFER_IO_MEMORY;
	dmaConfig.dataLen  = (N*4);
	dmaConfig.srcAddr  = (Uint32)&(hMcbsp->Regs->DRRL);
	dmaConfig.destAddr = (Uint32)rcv;

	dmaRxHandle = CSL_configDmaForMcbsp(&dmaRxChanObj,CSL_DMA_CHAN13);
	if(dmaRxHandle == NULL)
	{
		printf("DMA Config for mcbsp DMA Read Failed!\n!");
		return(CSL_MCBSP_TEST_FAILED);
	}

  CSL_DMA3_REGS->DMACH0DSAL = 0x6010;
  CSL_DMA3_REGS->DMACH0DSAU = 0x0000;

  CSL_DMA3_REGS->DMACH1SSAL = 0x6000;
  CSL_DMA3_REGS->DMACH1SSAU = 0x0000;
#endif

/* Write configuration structure values to MCBSP control register*/
 

   /** Input clock in Hz -- On QT we are assign PHOENIX_QTCLK to this*/
 	McBSPconfig.clkInput = 100000000;
   /* Operating mode */
   McBSPconfig.mcbsp_opmode = CSL_MCBSP_OPMODE_128CHANNEL;
   /**Word length selection
   Valid values - 8,12,16,20,24,32bits*/
   McBSPconfig.wordLength = CSL_MCBSP_WORD32;
   /**Frame length selection
   Valid values - (1-128)*/
   McBSPconfig.frameLength = CSL_MCBSP_FRMAELENGHT(128);
   /** single phase frame/double phase frame*/
   McBSPconfig.phase = CSL_MCBSP_SINGLEPHASE;
   /** data delay*/
   McBSPconfig.datadelay = CSL_MCBSP_DATADELAY0BIT;
   /** frame ignore flag */
   McBSPconfig.frameignore = CSL_MCBSP_FRMAEIGNOREFALSE;
   /**Loopback mode enable/disable*/
   McBSPconfig.loopBackmode = CSL_MCBSP_INTERNALLOOPBACK_DISABLE;
   /**Companding*/
   McBSPconfig.companding = CSL_MCBSP_COFF_MSBFIRST;
   /**Rjust mode*/
   McBSPconfig.rjust_mode = CSL_MCBSP_RJUST_RJZFMSB;
   /**Abis mode*/
   McBSPconfig.abismode = CSL_MCBSP_ABIS_DISABLE;
   /**DXENA enable/disable*/
   McBSPconfig.dxena = CSL_MCBSP_DXENA_DISABLE;
   /**CLKSTP */
   McBSPconfig.clkstp = CSL_MCBSP_SPCRL_CLKSTP_DISABLE;
   /**CLKXP,CLKRP */
   McBSPconfig.sample_polarity = CSL_MCBSP_DATASAMPLE_FALLING;
   /**FRAMESYNC */
   McBSPconfig.fs_polarity = CSL_MCBSP_FS_ACTIVEHIGH;
   /** Clock Source */
   McBSPconfig.clocksource = CSL_MCBSP_CLOCKSOURCE_SYSCLK;
   /** Interrupt Source */
   McBSPconfig.intr_event = CSL_MCBSP_INTR_RDY;
   /*sample rate(samples per sec) */
   McBSPconfig.sample_rate = 310;
   /*FSX pin source */
   McBSPconfig.fsxpin_status = CSL_MCBSP_FSX_DXRTOXSRCPYOP;

  Mcbsp_Configure(hMcbsp, &McBSPconfig); 

#ifdef USE_DMA
	CSL_FINS(hMcbsp->Regs->SRGRL,MCBSP_SRGRL_CLKGDV, 1);
#else
	CSL_FINS(hMcbsp->Regs->SRGRL,MCBSP_SRGRL_CLKGDV, 9);
#endif

#ifdef USE_DMA
    status = DMA_start(dmaRxHandle);
	if(status != CSL_SOK)
	{
	  printf("McBSP Dma Read start Failed!!\n");
	  return(status);
	}

	status = DMA_start(dmaTxHandle);
	if(status != CSL_SOK)
	{
		printf("McBSP Dma write start Failed!!\n");
		return(status);
	}
#endif
 /* In the test case, the TX and RX should start before the srg starts. This is to avoid missing the first channel on mcbsp */
  /* Enable MCBSP transmit and receive */
  MCBSP_start(hMcbsp, MCBSP_RCV_START | MCBSP_XMIT_START, 0 );

  /* Start Sample Rate Generator and Frame Sync */
  MCBSP_start(hMcbsp,MCBSP_SRGR_START | MCBSP_SRGR_FRAMESYNC, 0x300 );
   
                                    
  i=0;
  j=0;

#ifdef USE_DMA
//while(1)
{
  	/*Poll for the interrupt of tx_dma and rx_dma completions*/
	while( 0x3000 != CSL_SYSCTRL_REGS->DMAIFR );
   
	CSL_SYSCTRL_REGS->DMAIFR = 0x3000;//break;
}
#else /*POLL*/
  while (1) {
                
    /* Write 32 bit data value to DXR */    
    if(MCBSP_xrdy(hMcbsp) && i<=(N-1)) {
      MCBSP_write32(hMcbsp,xmt[i]);
      i++;
    }
 
    /* Read 32 bit value from DRR */
    if(MCBSP_rrdy(hMcbsp) && j<=(N-1)) {
      rcv[j] = MCBSP_read32(hMcbsp);               
      j++;
    }

    if(i==(N) && j==(N)) {
      break;
    }

 }       
#endif

  /* We are done with MCBSP, so close it */ 
  MCBSP_close(hMcbsp);
  printf("\r\n Checking the data integrity\r\n");

  /* Check data to make sure transfer was successful */
  for(i=0; i< N;i++) {
    if (xmt[i] != rcv[i]) {
       ++error;
       break;
    }
  }




  if(error)
  {
	return(CSL_MCBSP_TEST_FAILED);
  }
  else
  {
	return(CSL_MCBSP_TEST_PASSED);
  }
#endif
                     
}


CSL_DMA_Handle CSL_configDmaForMcbsp(CSL_DMA_ChannelObj    *dmaChanObj,
                                    CSL_DMAChanNum        chanNum)
{
	CSL_DMA_Handle    dmaHandle;
	CSL_Status        status;

	dmaHandle = NULL;


	/* Open A Dma channel */
	dmaHandle = DMA_open(chanNum, dmaChanObj, &status);
    if(dmaHandle == NULL)
    {
        printf("DMA_open Failed!\n");
    }

	/* Configure a Dma channel */
	status = DMA_config(dmaHandle, &dmaConfig);
    if(status != CSL_SOK)
    {
        printf("DMA_config Failed!\n");
        dmaHandle = NULL;
    }

    return(dmaHandle);
}





