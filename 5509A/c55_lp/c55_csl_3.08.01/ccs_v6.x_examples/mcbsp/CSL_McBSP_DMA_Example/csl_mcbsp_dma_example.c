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

 /** @file csl_mcbsp_dma_example.c
 *
 *  @brief Example code to verify McBSP transfers using DMA
 *
 * \page    page9  McBSP EXAMPLE DOCUMENTATION
 *
 * \section McBSP7   McBSP EXAMPLE7 - McBSP DMA mode
 *
 * \subsection McBSP7x    TEST DESCRIPTION:
 * This test is to verify the McBSP external loopback TX-RX transfers using DMA.
 *
 * \subsection McBSP7y    TEST PROCEDURE AND EXPECTED RESULT:
 * Uncomment "#define EVM5525 1" & Comment "#define USE_MCBSP_TARGET 1"
 * This test configures PLL to be 100MHz
 *
 * This is an External Loop-back Test
 * The following connections are required on J14 of EVM5517 revD
 *
 * 1. McBSP_CLKX(pin#12) to  pin 20 on J15 (or) McBSP_CLKR(Test point -TP#15)
 * 2. McBSP_FSX(pin#8) to McBSP_FSR(pin#1)
 * 3. McBSP_DX(pin#22) to McBSP_DR(pin#21)
 *
 *
 * Brief description of the procedure and the expected result to be checked:
 * 1. Configure DMA3 Channel0: 1Word Burst, McBSP Tx Dma event, write mode,
 *    global tx array addr as src and McBSP TXRL as dst, IO-mem transfer i
 *    and suitable datalength, dma interrupt enabled.
 * 2. Configure DMA3 Channel1: 1Word Burst, McBSP Rx Dma event, read mode, i
 *    McBSP RXRL as src and global rx array addr as dst, IO-mem transfer and i
 *    suitable datalength, dma interrupt enabled.
 * 3. Configure McBSP: 32 Word wordlenth, framelenth 1, single phase, 0-bit data delay, i
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

#define CSL_MCBSP_TEST_PASSED      (0)
#define CSL_MCBSP_TEST_FAILED      (1)

//---------Global constants---------
#define CSL_MCBSP_BUF_SIZE (4u)

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

/**Dma write is happening in 32 bits,if use character array*/
Uint32 gmcbspDmaWriteBuf[CSL_MCBSP_BUF_SIZE];
Uint32 gmcbspDmaReadBuf[CSL_MCBSP_BUF_SIZE];


//---------Function prototypes---------
CSL_Status CSL_McBSPTest(void);
/*Reference the start of the interrupt vector table*/
extern void VECSTART(void);
/* Protype declaration for ISR function */

CSL_DMA_Handle CSL_configDmaForMcbsp(CSL_DMA_ChannelObj    *dmaChanObj,
                                    CSL_DMAChanNum        chanNum);
CSL_Status ioExpander_Setup(void);
CSL_Status ioExpander_Read(Uint16 port, Uint16 pin, Uint16 *rValue);
CSL_Status ioExpander_Write(Uint16 port, Uint16 pin, Uint16 value);

/////INSTRUMENTATION FOR BATCH TESTING -- Part 1 --
/////  Define PaSs_StAtE variable for catching errors as program executes.
/////  Define PaSs flag for holding final pass/fail result at program completion.
volatile Int16 PaSs_StAtE = 0x0001; // Init to 1. Reset to 0 at any monitored execution error.
volatile Int16 PaSs = 0x0000; // Init to 0.  Updated later with PaSs_StAtE when and if
/////                                  program flow reaches expected exit point(s).
/////
void main() {
  CSL_Status result; 
  
  printf("\r\nMCBSP DMA Mode test!!\n");

  result = CSL_McBSPTest();
  if(result == CSL_MCBSP_TEST_PASSED)
  {
 	printf("\r\nMCBSP DMA Mode test paased!!\n");
  }
  else
  {
	printf("\r\nMCBSP DMA Mode test failed!!\n");
	PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
  }

  /////INSTRUMENTATION FOR BATCH TESTING -- Part 3 --
  /////  At program exit, copy "PaSs_StAtE" into "PaSs".
  PaSs =PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with
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
  for( i = 0; i < CSL_MCBSP_BUF_SIZE ; i++ ) 
  {
    gmcbspDmaWriteBuf[i] = 0x12345678;
    gmcbspDmaReadBuf[i] = 0;
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

#ifdef USE_MCBSP_TARGET
 // To configure the McBSP target through GPIO on S1
 //   ====================================================================================================================
 //   MCBSP Target Mode     | GPIO Data | Data          |  Frame Sync |   Clock                 | External Clock         |
 //                         | [9:6]     | Loop back     | Loop back   |  Loop back              | Connection             |
 //   ====================================================================================================================
 //   MCBSP_TGT_NO_LB       |  0000     |  No loop back |  --         |  --                     |  --                    |
 //   MCBSP_TGT_LB_MODE0    |  0001     |  dx -> dr     |  fsr -> fsx |  clkr -> clkx           |  --                    |
 //   MCBSP_TGT_LB_MODE1    |  0011     |  dx -> dr     |  fsx -> fsr |  clkr -> clkx           |  --                    |
 //   MCBSP_TGT_LB_MODE2    |  0101     |  dx -> dr     |  fsr -> fsx |  clkx -> clkr           |  clkx -> clks          |
 //   MCBSP_TGT_LB_MODE3    |  0111     |  dx -> dr     |  fsx -> fsr |  clkx -> clkr           |  clkx -> clks          |
 //   MCBSP_TGT_LB_MODE4    |  1001     |  dx -> dr     |  fsr -> fsx |  External Clock -> clkx |  --                    |
 //   MCBSP_TGT_LB_MODE5    |  1011     |  dx -> dr     |  fsx -> fsr |  External Clock -> clkx |  --                    |
 //   MCBSP_TGT_LB_MODE6    |  1101     |  dx -> dr     |  fsr -> fsx |  External Clock -> clkr |  External Clock -> clks|
 //   MCBSP_TGT_LB_MODE7    |  1111     |  dx -> dr     |  fsx -> fsr |  External Clock -> clkr |  External Clock -> clks|
 //   ====================================================================================================================
MCBSP_target_config(MCBSP_TGT_LB_MODE3);
#endif

  /* Initialize Dma */
  status = DMA_init();
  if (status != CSL_SOK)
  {
    printf("DMA_init Failed!\n");
  }


    /* Configure the DMA channel for mcbsp transmit */
#if ((defined(CHIP_C5505_C5515)) || (defined(CHIP_C5504_C5514)) || defined(CHIP_C5517) || (defined(CHIP_C5535) || defined(CHIP_C5545)))
	dmaConfig.pingPongMode = CSL_DMA_PING_PONG_DISABLE;
#endif
	dmaConfig.autoMode = CSL_DMA_AUTORELOAD_DISABLE;
	dmaConfig.burstLen = CSL_DMA_TXBURST_1WORD;
	dmaConfig.trigger  = CSL_DMA_EVENT_TRIGGER;
	dmaConfig.dmaEvt   = CSL_DMA_EVT_MCBSP_TX;
	dmaConfig.dmaInt   = CSL_DMA_INTERRUPT_ENABLE;
	dmaConfig.chanDir  = CSL_DMA_WRITE;
	dmaConfig.trfType  = CSL_DMA_TRANSFER_IO_MEMORY;
	dmaConfig.dataLen  = (CSL_MCBSP_BUF_SIZE*4);
	dmaConfig.srcAddr  = (Uint32)gmcbspDmaWriteBuf;
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
	dmaConfig.autoMode = CSL_DMA_AUTORELOAD_DISABLE;
	dmaConfig.burstLen = CSL_DMA_TXBURST_1WORD;
    dmaConfig.trigger  = CSL_DMA_EVENT_TRIGGER;
    dmaConfig.dmaEvt   = CSL_DMA_EVT_MCBSP_RX;
	dmaConfig.dmaInt   = CSL_DMA_INTERRUPT_ENABLE;
	dmaConfig.chanDir  = CSL_DMA_READ;
	dmaConfig.trfType  = CSL_DMA_TRANSFER_IO_MEMORY;
	dmaConfig.dataLen  = (CSL_MCBSP_BUF_SIZE*4);
	dmaConfig.srcAddr  = (Uint32)&(hMcbsp->Regs->DRRL);
	dmaConfig.destAddr = (Uint32)gmcbspDmaReadBuf;

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

  /* Write configuration structure values to MCBSP control register*/
  
#ifdef USE_MCBSP_TARGET
 /** Input clock in Hz -- As our test cases will always run on QT we are always assigning the PHOENIX_QTCLK to this structure member*/
 McBSPconfig.clkInput = PHOENIX_QTCLK;
#else
 McBSPconfig.clkInput = 100000000;
#endif
   /* Operating mode */
   McBSPconfig.mcbsp_opmode = CSL_MCBSP_OPMODE_NORMAL;
   //McBSPconfig.mcbsp_opmode = CSL_MCBSP_OPMODE_128CHANNEL;  
   /**Word length selection
   Valid values - 8,12,16,20,24,32bits*/
   McBSPconfig.wordLength = CSL_MCBSP_WORD32;
   /**Frame length selection
   Valid values - (1-128)*/
   McBSPconfig.frameLength = CSL_MCBSP_FRMAELENGHT(1);
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
   McBSPconfig.sample_polarity = CSL_MCBSP_DATASAMPLE_RISING;
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

    status = DMA_start(dmaRxHandle);
	if(status != CSL_SOK)
	{
	  printf("McBSP Dma Read start Failed!!\n");
	  return(status);
	}

 	/* Start UART tx Dma transfer */
	status = DMA_start(dmaTxHandle);
	if(status != CSL_SOK)
	{
		printf("McBSP Dma write start Failed!!\n");
		return(status);
	}

   /* In the test case, the TX and RX should start before the srg starts. This is to avoid missing the first channel on mcbsp */
   /* Enable MCBSP transmit and receive */
   MCBSP_start(hMcbsp, MCBSP_RCV_START | MCBSP_XMIT_START, 0 );
 
   /* Start Sample Rate Generator and Frame Sync */
  MCBSP_start(hMcbsp,MCBSP_SRGR_START | MCBSP_SRGR_FRAMESYNC, 0x300 );
  
  	/*Poll for the interrupt of tx_dma and rx_dma completions*/
	while( 0x3000 != CSL_SYSCTRL_REGS->DMAIFR );
   
	CSL_SYSCTRL_REGS->DMAIFR = 0x3000;

//Compare the data here                     
    printf("\r\n Checking the data integrity\r\n");
  /* Check data to make sure transfer was successful */
  for(i=0; i<CSL_MCBSP_BUF_SIZE;i++) {
    if (gmcbspDmaWriteBuf[i] != gmcbspDmaReadBuf[i]) {
       ++error;
       break;
    }
  }

  /* We are done with MCBSP, so close it */ 
  MCBSP_close(hMcbsp);

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


