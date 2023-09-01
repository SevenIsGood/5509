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

/** @file csl_mcbsp_idle_example.c
 *
 *  @brief This test verifies McBSP TX-RX external loopback in McBSP-IDLE_EN bit set in PCRL
 *
 * \page    page9  McBSP EXAMPLE DOCUMENTATION
 *
 * \section McBSP8   McBSP EXAMPLE8 - McBSP IDLE function
 *
 * \subsection McBSP8x    TEST DESCRIPTION:
 * This test verifies McBSP TX-RX external loopback in McBSP-IDLE_EN bit set in PCRL
 *
 *
 * \subsection McBSP8y    TEST PROCEDURE AND EXPECTED RESULT:
 * This test configures PLL to be 100MHz
 *
 * This is an External Loop-back Test
 * The following connections are required on J14 of EVM5517
 *
 * 1. McBSP_CLKX(pin#12) to  pin 20 on J15 (or) McBSP_CLKR(Test point -TP#15)
 * 2. McBSP_FSX(pin#8) to McBSP_FSR(pin#1)
 * 3. McBSP_DX(pin#22) to McBSP_DR(pin#21)
 *
 * Brief description of the procedure and the expected result to be checked:
 * 1. Configure McBSP: 32 Word wordlenth, framelenth 1, single phase, 0-bit
 *    data delay, r-justified msb first, clksrc sysclk, frame sync selection,
 *    sample rate.
 * 2. Enable and start McBSP Tx, Rx and Frame Sync Generator.
 * 3. Set IDLE_EN in PCRL.
 * 4. Wait for XRDY bit in SPCRU to be set.
 * 5. Send data in xmt[] out of McBSP Tx via DXRU and DXRL.
 * 6. Wait for RRDY bit in SPCRL to be set.
 * 7. Collect the data rfom RXRU and RXRL into rcv[].
 * 8. Check Data in rcv array for integrity vis-a-vis the xmt array.
 *
*/

/*
 *---------main_mcbsp1.c---------
 * In this example, the MCBSP is configured in digital loopback
 * mode, with 32 bit data transfer, in multi-frame mode, using                                                   
 * sample rate generator to sync frames                        
 */
 
#include <stdio.h>
#include <csl_mcbsp.h>
#include "csl_mcbsp_tgt.h"

#define CSL_MCBSP_TEST_PASSED      (0)
#define CSL_MCBSP_TEST_FAILED      (1)

//---------Global constants---------
#define N 1024    

int j=0;
CSL_McbspHandle  hMcbsp;
CSL_McbspObj 	 McbspObj;              

/* Create a MCBSP configuration structure */

CSL_McbspSetup McBSPconfig;
  
  
/* Create data buffers for transfer */
Uint32 xmt[N], rcv[N]; 
Uint16	mcbsp_rx_count = 0;
Uint16	mcbsp_tx_count = 0;

//---------Function prototypes---------
CSL_Status CSL_McBSPTest(void);

/*Reference the start of the interrupt vector table*/
extern void VECSTART(void);
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
  
  printf("\r\nMCBSP Normal Mode test case!!\n");

  result = CSL_McBSPTest();
  if(result == CSL_MCBSP_TEST_PASSED)
  {
 	printf("\r\nMCBSP Normal Mode test passed!!\n");
  }
  else
  {
	 printf("\r\nMCBSP Normal Mode test failed!!\n");
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
  for(i=0;i<=N-1;i++) 
  {
    //xmt[i] = 0xFFFFFFFF;
	xmt[i] = 0x12345678;
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

#ifdef USE_MCBSP_TARGET

#endif

  /* Write configuration structure values to MCBSP control register*/
  

  /** Input clock in Hz -- As our test cases will always run on QT we are always assigning the PHOENIX_QTCLK to this structure member*/
	McBSPconfig.clkInput = 100000000;
   /* Operating mode */
   McBSPconfig.mcbsp_opmode = CSL_MCBSP_OPMODE_NORMAL;
   /**Word length selection
   Valid values - 8,12,16,20,24,32bits*/
   McBSPconfig.wordLength = CSL_MCBSP_WORD32;
   /**Frame length selection
   Valid values - (1-128)*/
   McBSPconfig.frameLength = CSL_MCBSP_FRMAELENGHT(1);
   /** single phase frame/double phase frame*/
   McBSPconfig.phase = CSL_MCBSP_DUALPHASE;
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
  

  Mcbsp_Configure(hMcbsp, &McBSPconfig); /* Currently hMcbsp is not used anywhere in the CSL, so please ignore the warn - Pramod*/

   /* In the test case, the TX and RX should start before the srg starts. This is to avoid missing the first channel on mcbsp */
   /* Enable MCBSP transmit and receive */
   MCBSP_start(hMcbsp, MCBSP_RCV_START | MCBSP_XMIT_START, 0 );
 
   /* Start Sample Rate Generator and Frame Sync */
  MCBSP_start(hMcbsp,MCBSP_SRGR_START | MCBSP_SRGR_FRAMESYNC, 0x300 );

  //IDLE the McBSP
  CSL_MCBSP_REGS->PCRL |= CSL_MCBSP_PCRL_IDLE_EN_MASK;

   
                                    
  i=0;
  j=0;
  for (i=0; i<=N-1;i++) 
  {
 
    /* Wait for XRDY signal before writing data to DXR */
   while (!MCBSP_xrdy(hMcbsp));       
                 
    /* Write 32 bit data value to DXR */    
    MCBSP_write32(hMcbsp,xmt[i]);  
    
    /* Wait for RRDY signal to read data from DRR */
    while (!MCBSP_rrdy(hMcbsp));

    /* Read 32 bit value from DRR */
    rcv[i] = MCBSP_read32(hMcbsp);               
  }        

 

  printf("\r\n Checking the data integrity\r\n");
  /* Check data to make sure transfer was successful */
  for(i=0; i<= N-1;i++) {
    if (xmt[i] != rcv[i]) {
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







