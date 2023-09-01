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

/** @file csl_gpt_dma_example.c
 *
 *  @brief DMA functional layer API defenition file
 *
 * \page    page4  GPT EXAMPLE DOCUMENTATION
 *
 * \section GPT4   GPT EXAMPLE4 - GPT DMA Test
 *
 * \subsection GPT4x    TEST DESCRIPTION:
 *
 *         This test configures all 3 GPT's (0,1, and 2) and synchronizes a DMA
 * transfer of 8 contiguous 16-bit data words from DARAM location 0x400 to SARAM
 * location 0x20000, with the three GPT events.
 *
 * \subsection GPT4y    TEST PROCEDURE and EXPECTED RESULT:
 * (Put breakpoints at all for() loop locations in the code. There are only 3
 *  such locations.)
 *
 * 1. Configure DMA0 channels 0,1,2 for DMA events w.r.t TIMER0,1,2: 4Word Burst,
 *    Write mode, 16byte datalength, Mem-mem transfer, SRC addr in DARAM and Dest
 *    Addr in SARAM.
 * 2. Configure TIMER0,1,2: Auto mode disabled, prescale i/p clk div-by-4.
 * 3. Write 8 contiguous 16-byte locations @ SRC addr in DARAM to be 0xFFFF.
 * 4. Start DMA0 channel0.
 * 5. Start TIMER0.
 * 6. Wait for DMA event w.r.t Timer0 count expiry.
 * 7. Stop DMA0 Channel0.
 * 8. Check if 8 contiguous 16-bit locations @ DST addr in SARAM have been
 *    written with 0xFFFF. (In the CCS Memory Browser Window)
 * 9. Write 8 contiguous 16-byte locations @ SRC addr in DARAM to be 0xAAAA.
 * 10. Start DMA0 channel1.
 * 11. Start TIMER1.
 * 12. Wait for DMA event w.r.t Timer1 count expiry.
 * 13. Stop DMA0 Channel1.
 * 14. Check if 8 contiguous 16-bit locations @ DST addr in SARAM have been
 *     written with 0xFFFF. (In the CCS Memory Browser Window)
 * 15. Write 8 contiguous 16-byte locations @ SRC addr in DARAM to be 0xBBBB.
 * 16. Start DMA0 channel2.
 * 17. Start TIMER2.
 * 18. Wait for DMA event w.r.t Timer2 count expiry.
 * 19. Stop DMA0 Channel2.
 * 20. Check if 8 contiguous 16-bit locations @ DST addr in SARAM have been
 *     written with 0xFFFF. (In the CCS Memory Browser Window)
 */
/*
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515/C5535/C5545 AND
 * C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * ============================================================================
*/
/* ============================================================================
 * Revision History
 * ================
 * 15-Apr-2015 Created
 * 10-Mar-2016 Updates to header
 * ============================================================================
 */


#include <stdio.h>
#include "csl_gpt.h"
#include "csl_intc.h"
#include <csl_general.h>
#include <csl_dma.h>

#if ((defined(CHIP_C5505_C5515)) || (defined(CHIP_C5504_C5514)) || (defined(CHIP_C5535) || defined(CHIP_C5545)))
#include <csl_sysctrl.h>
#endif

#define CSL_TEST_FAILED    (1u)
#define CSL_TEST_PASSED    (0)

#define CSL_PLL_DIV_000    (0)
#define CSL_PLL_DIV_001    (1u)
#define CSL_PLL_DIV_002    (2u)
#define CSL_PLL_DIV_003    (3u)
#define CSL_PLL_DIV_004    (4u)
#define CSL_PLL_DIV_005    (5u)
#define CSL_PLL_DIV_006    (6u)
#define CSL_PLL_DIV_007    (7u)


#define CSL_MCSPI_TEST_PASSED      (0)
#define CSL_MCSPI_TEST_FAILED      (1)
#define CSL_DMA_BUF_SIZE	(4u)
#define SRC_ADDR (0x400)
#define DST_ADDR (0x20000)

CSL_DMA_Handle dmaT0Handle;
CSL_DMA_Handle dmaT1Handle;
CSL_DMA_Handle dmaT2Handle;
CSL_DMA_ChannelObj    dmaT0ChanObj;
CSL_DMA_ChannelObj    dmaT1ChanObj;
CSL_DMA_ChannelObj    dmaT2ChanObj;
CSL_DMA_Config dmaConfig;

CSL_DMA_Handle CSL_configDmaForGPT(CSL_DMA_ChannelObj    *dmaChanObj,
                                    CSL_DMAChanNum        chanNum);
#ifdef INTERRUPT_REQUIRED
extern void VECSTART(void);
#endif

Uint32    cpuCycleCount = 0;
Uint32    sysClk;

#ifdef INTERRUPT_REQUIRED
volatile Uint16    hitIsr = 0,hit_gpt0isr=0,hit_gpt1isr=0,hit_gpt2isr=0;
#endif



/**
 *  \brief  GPT Count Rate Verification test function
 *
 * This function verifies whether timer is running at the configured
 * rate or not. This function configures and enables the GPT interrupts
 * to indicate the expiry of the timer count. Test counts number of
 * cycles executed by the CPU after starting the timer till the expiry
 * of the timer. These cycles are used to verify the WDT count rate.
 * It is assumed that calculated CPU cycles will be with in the range
 * ±1% actual CPU clock cycles. Test will be successful if the calculated
 * CPU cycles fall with in this range.
 *
 * NOTE: Changing the PLL setting in the middle (After getSysClk() call)
 *       of the test will result in test failure.
 *
 *  \param  none
 *
 *  \return CSL_TEST_PASSED  - Success
 *          CSL_TEST_FAILED  - Failure
 */
Int16 CSL_gptDmaTest(void);

/**
 *  \brief  Function to calculate the system clock
 *
 *  \param    none
 *
 *  \return   System clock value in KHz
 */


Uint32 getSysClk(void);

#ifdef INTERRUPT_REQUIRED
/*GPT Interrupt Service Routine*/
interrupt void gptIsr(void);

/*GPT0 Interrupt Service code*/
void gpt0Isr(void);

/*GPT1 Interrupt Service code*/
void gpt1Isr(void);

/*GPT2 Interrupt Service code*/
void gpt2Isr(void);
#endif
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 1 --
   /////  Define PaSs_StAtE variable for catching errors as program executes.
   /////  Define PaSs flag for holding final pass/fail result at program completion.
        volatile Int16 PaSs_StAtE = 0x0001; // Init to 1. Reset to 0 at any monitored execution error.
        volatile Int16 PaSs = 0x0000; // Init to 0.  Updated later with PaSs_StAtE when and if
   /////                                  program flow reaches expected exit point(s).
   ////

void main(void)
{
	Int16	result;

	printf("General Purpose Timers Interrupt test!\n\n");

	//route all timer ISRs from NMI to TINT
	*((volatile ioport unsigned short *)0x1C3E) = 0x0000;

	/* Test to verify the Timer Interrupts */
	result = CSL_gptDmaTest();
	if(CSL_TEST_FAILED == result)
	{
		printf("TIMER INTERRUPT TEST FAILED!!\n");
 	    PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
	}
	else
	{
		printf("TIMER INTERRUPT TEST PASSED!!\n");
	}

   /////INSTRUMENTATION FOR BATCH TESTING -- Part 3 --
   /////  At program exit, copy "PaSs_StAtE" into "PaSs".
   PaSs = PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with
   /////                   // pass/fail value determined during program execution.
   /////  Note:  Program should next exit to C$$EXIT and halt, where DSS, under
  /////   control of a host PC script, will read and record the PaSs' value.
  /////

}


/**
 *  \brief  GPT0 Count Rate Verification test function
 *
 * This function verifies whether timer is running at the configured
 * rate or not. This function configures and enables the GPT interrupts
 * to indicate the expiry of the timer count. Test counts number of
 * cycles executed by the CPU after starting the timer till the expiry
 * of the timer. These cycles are used to verify the WDT count rate.
 * It is assumed that calculated CPU cycles will be with in the range
 * ±1% actual CPU clock cycles. Test will be successful if the calculated
 * CPU cycles fall with in this range.
 *
 * NOTE: Changing the PLL setting in the middle (After getSysClk() call)
 *       of the test will result in test failure.
 *
 *  \param  none
 *
 *  \return CSL_TEST_PASSED  - Success
 *          CSL_TEST_FAILED  - Failure
 */
Int16 CSL_gptDmaTest(void)
{
	CSL_Handle    hGpt0,hGpt1,hGpt2;
	CSL_Status 	  status;
	CSL_Config 	  hwConfig;
	CSL_GptObj	  gptObj0,gptObj1,gptObj2;

	Uint8	 			loop_count=0x0;
	volatile Uint16*	daram_space=0x0;

	daram_space = (Uint16 *) SRC_ADDR;

#ifdef INTERRUPT_REQUIRED
	hitIsr = FALSE;
#endif
	status   = 0;

	/* Get the System clock value at which CPU is currently running */
#ifdef CHIP_C5517
    sysClk = getSysClk();
#else
	#ifdef PHOENIX_QT
    sysClk = PHOENIX_QTCLK;
    #elif (defined(CHIP_5515))
    sysClk = getSysClk();
    #else
     #warn "Define either EVM5515 or PHOENIX_QT"
	#endif
#endif

	printf("\n\nCPU clock is running at %ldKHz\n", sysClk);

	  /* Initialize Dma */
	  status = DMA_init();
	  if (status != CSL_SOK)
	  {
	    printf("DMA_init Failed!\n");
	  }

	/* Open the CSL GPT modules */
	hGpt0 = GPT_open (GPT_0, &gptObj0, &status);
	if((NULL == hGpt0) || (CSL_SOK != status))
	{
		printf("GPT0 Open Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT0 Open Successful\n");
	}

	hGpt1 = GPT_open (GPT_1, &gptObj1, &status);
	if((NULL == hGpt1) || (CSL_SOK != status))
	{
		printf("GPT1 Open Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT1 Open Successful\n");
	}

	hGpt2 = GPT_open (GPT_2, &gptObj2, &status);
	if((NULL == hGpt2) || (CSL_SOK != status))
	{
		printf("GPT2 Open Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT2 Open Successful\n");
	}

	/* Reset the GPT modules */
	status = GPT_reset(hGpt0);
	if(CSL_SOK != status)
	{
		printf("GPT0 Reset Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT0 Reset Successful\n");
	}

	status = GPT_reset(hGpt1);
	if(CSL_SOK != status)
	{
		printf("GPT1 Reset Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT1 Reset Successful\n");
	}

	status = GPT_reset(hGpt2);
	if(CSL_SOK != status)
	{
		printf("GPT2 Reset Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT2 Reset Successful\n");
	}

	for(loop_count=0;loop_count<(CSL_DMA_BUF_SIZE*4)/2;loop_count++)
		daram_space[loop_count]=0xFFFF;


    /* Configure the DMA channel for mcspi transmit */
#if ((defined(CHIP_C5505_C5515)) || (defined(CHIP_C5504_C5514)) || defined(CHIP_C5517) || (defined(CHIP_C5535) || defined(CHIP_C5545)))
	dmaConfig.pingPongMode = CSL_DMA_PING_PONG_DISABLE;
#endif
	dmaConfig.autoMode = CSL_DMA_AUTORELOAD_DISABLE;
	dmaConfig.burstLen = CSL_DMA_TXBURST_4WORD;
	dmaConfig.trigger  = CSL_DMA_EVENT_TRIGGER;
	dmaConfig.dmaEvt   = CSL_DMA_EVT_TIMER0;
	dmaConfig.dmaInt   = CSL_DMA_INTERRUPT_DISABLE;
	dmaConfig.chanDir  = CSL_DMA_WRITE;
	dmaConfig.trfType  = CSL_DMA_TRANSFER_MEMORY;
	dmaConfig.dataLen  = (CSL_DMA_BUF_SIZE*4);
	dmaConfig.srcAddr  = (Uint32)(SRC_ADDR);
	dmaConfig.destAddr = (Uint32)(DST_ADDR);

    dmaT0Handle = CSL_configDmaForGPT(&dmaT0ChanObj, CSL_DMA_CHAN0);
	if(dmaT0Handle == NULL)
	{
		printf("DMA Config for Timer0 DMA Write Failed!\n!");
		return(CSL_MCSPI_TEST_FAILED);
	}

	dmaConfig.dmaEvt   = CSL_DMA_EVT_TIMER1;
	dmaT1Handle = CSL_configDmaForGPT(&dmaT1ChanObj, CSL_DMA_CHAN1);
	if(dmaT1Handle == NULL)
	{
		printf("DMA Config for Timer1 DMA Write Failed!\n!");
		return(CSL_MCSPI_TEST_FAILED);
	}

	dmaConfig.dmaEvt   = CSL_DMA_EVT_TIMER2;
	dmaT2Handle = CSL_configDmaForGPT(&dmaT2ChanObj, CSL_DMA_CHAN2);
	if(dmaT2Handle == NULL)
	{
		printf("DMA Config for Timer2 DMA Write Failed!\n!");
		return(CSL_MCSPI_TEST_FAILED);
	}

#ifdef INTERRUPT_REQUIRED
	/* Clear any pending interrupts */
	IRQ_clearAll();

	/* Disable all the interrupts */
	IRQ_disableAll();

	IRQ_setVecs((Uint32)(&VECSTART));

	IRQ_plug(TINT_EVENT, &gptIsr);
	IRQ_enable(TINT_EVENT);
#endif

	/* Configuring the GPT0 */
	hwConfig.autoLoad 	 = GPT_AUTO_DISABLE;
	hwConfig.ctrlTim 	 = GPT_TIMER_ENABLE;
	hwConfig.preScaleDiv = GPT_PRE_SC_DIV_1;
	hwConfig.prdLow 	 = (sysClk)/4;
	hwConfig.prdHigh 	 = 0x0000;

	/* Configure the GPT0 module */
	status =  GPT_config(hGpt0, &hwConfig);
	if(CSL_SOK != status)
	{
		printf("GPT0 Config Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT0 Config Successful\n");
	}

	/* Configuring the GPT1 */
	hwConfig.autoLoad 	 = GPT_AUTO_DISABLE;
	hwConfig.ctrlTim 	 = GPT_TIMER_ENABLE;
	hwConfig.preScaleDiv = GPT_PRE_SC_DIV_1;
	hwConfig.prdLow 	 = (sysClk)/4;
	hwConfig.prdHigh 	 = 0x0000;

	/* Configure the GPT1 module */
	status =  GPT_config(hGpt1, &hwConfig);
	if(CSL_SOK != status)
	{
		printf("GPT1 Config Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT1 Config Successful\n");
	}

	/* Configuring the GPT2 */
	hwConfig.autoLoad 	 = GPT_AUTO_DISABLE;
	hwConfig.ctrlTim 	 = GPT_TIMER_ENABLE;
	hwConfig.preScaleDiv = GPT_PRE_SC_DIV_1;
	hwConfig.prdLow 	 = (sysClk)/4;
	hwConfig.prdHigh 	 = 0x0000;

	/* Configure the GPT2 module */
	status =  GPT_config(hGpt2, &hwConfig);
	if(CSL_SOK != status)
	{
		printf("GPT2 Config Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT2 Config Successful\n");
	}

#ifdef INTERRUPT_REQUIRED
	/* Enable CPU Interrupts */
	IRQ_globalEnable();
#endif

	status = DMA_start(dmaT0Handle);
	if(status != CSL_SOK)
	{
	  printf("Timer0 Dma Write start Failed!!\n");
	  return(status);
	}
	/* Start the Timer0 */
	GPT_start(hGpt0);
	// Check transfer complete status
	while(DMA_getStatus(dmaT0Handle));
	printf("DMA Data Write from DARAM to SARAM"
			" using Timer0 event Complete\n");
	DMA_stop(dmaT0Handle);

	for(loop_count=0;loop_count<(CSL_DMA_BUF_SIZE*4)/2;loop_count++)
			daram_space[loop_count]=0xAAAA;

	status = DMA_start(dmaT1Handle);
	if(status != CSL_SOK)
	{
	  printf("Timer1 Dma Write start Failed!!\n");
	  return(status);
	}
	/* Start the Timer1 */
	GPT_start(hGpt1);

	while(DMA_getStatus(dmaT1Handle));
	printf("DMA Data Write from DARAM to SARAM"
			" using Timer1 event Complete\n");
	DMA_stop(dmaT1Handle);

	for(loop_count=0;loop_count<(CSL_DMA_BUF_SIZE*4)/2;loop_count++)
			daram_space[loop_count]=0xBBBB;

	status = DMA_start(dmaT2Handle);
	if(status != CSL_SOK)
	{
	  printf("Timer2 Dma Write start Failed!!\n");
	  return(status);
	}

	/* Start the Timer2 */
	GPT_start(hGpt2);

	while(DMA_getStatus(dmaT2Handle));
	printf("DMA Data Write from DARAM to SARAM"
			" using Timer2 event Complete\n");
	DMA_stop(dmaT2Handle);

	printf("DMA COMPLETED\r\n");

#ifdef INTERRUPT_REQUIRED
    while((hit_gpt0isr==0)||(hit_gpt1isr==0)||(hit_gpt2isr==0) );

	printf("\r\nTimer0,1,2 generated the interrupts!\r\n");

	//Execute the below code if you want to stop timers after some time.
	/* Disable the CPU interrupts */
	IRQ_globalDisable();

	/* Clear any pending interrupts */
	IRQ_clearAll();

	/* Disable all the interrupts */
	IRQ_disableAll();
#endif

	/* Stop the Timer0 */
	status = GPT_stop(hGpt0);
	if(CSL_SOK != status)
	{
		printf("GPT0 Stop Failed \n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT0 Stop Successful\n");
	}

	/* Stop the Timer1 */
	status = GPT_stop(hGpt1);
	if(CSL_SOK != status)
	{
		printf("GPT1 Stop Failed \n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT1 Stop Successful\n");
	}

	/* Stop the Timer2 */
	status = GPT_stop(hGpt2);
	if(CSL_SOK != status)
	{
		printf("GPT2 Stop Failed \n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT2 Stop Successful\n");
	}



	status = GPT_reset(hGpt0);
	status = GPT_reset(hGpt1);
	status = GPT_reset(hGpt2);

	/* Close The GPT0 Module */
	status = GPT_close(hGpt0);
	if(CSL_SOK != status)
	{
		printf("GPT0 Close Failed\n");
		return (CSL_TEST_FAILED);
	}

	/* Close The GPT1 Module */
	status = GPT_close(hGpt1);
	if(CSL_SOK != status)
	{
		printf("GPT1 Close Failed\n");
		return (CSL_TEST_FAILED);
	}

	/* Close The GPT2 Module */
	status = GPT_close(hGpt2);
	if(CSL_SOK != status)
	{
		printf("GPT2 Close Failed\n");
		return (CSL_TEST_FAILED);
	}


	return (CSL_TEST_PASSED);
}

#ifdef INTERRUPT_REQUIRED
/**
 *  \brief  GPT Interrupt Service Routine
 *
 *  \param  none
 *
 *  \return none
 */
interrupt void gptIsr(void)
{
	unsigned char TIAFR_VAL=0;
	hitIsr++;

	TIAFR_VAL = CSL_SYSCTRL_REGS->TIAFR;
    IRQ_clear(TINT_EVENT);
	if((TIAFR_VAL&0x01) == 0x01)
	{
	 gpt0Isr();
	}
	if((TIAFR_VAL&0x02) == 0x02)
	{
	 gpt1Isr();
	}
	if((TIAFR_VAL&0x04) == 0x04)
	{
	 gpt2Isr();
	}

}


/**
 *  \brief  GPT0 Interrupt Service code
 *
 *  \param  none
 *
 *  \return none
 */
void gpt0Isr(void)
{
    /* Clear Timer Interrupt Aggregation Flag Register (TIAFR) */
    CSL_SYSCTRL_REGS->TIAFR = 0x01;
	hit_gpt0isr=1;
	printf("\r\nTimer0 ISR\r\n");
}

/**
 *  \brief  GPT1 Interrupt Service code
 *
 *  \param  none
 *
 *  \return none
 */
void gpt1Isr(void)
{
    /* Clear Timer Interrupt Aggregation Flag Register (TIAFR) */
    CSL_SYSCTRL_REGS->TIAFR = 0x02;
	hit_gpt1isr=1;
	printf("\r\nTimer1 ISR\r\n");
}

/**
 *  \brief  GPT2 Interrupt Service code
 *
 *  \param  none
 *
 *  \return none
 */
void gpt2Isr(void)
{
    /* Clear Timer Interrupt Aggregation Flag Register (TIAFR) */
    CSL_SYSCTRL_REGS->TIAFR = 0x04;
	hit_gpt2isr=1;
	printf("\r\nTimer2 ISR\r\n");
}
#endif

/**
 *  \brief  Function to calculate the clock at which system is running
 *
 *  \param    none
 *
 *  \return   System clock value in KHz
 */
#if (defined(CHIP_C5517))

#define CSL_PLL_CLOCKIN (12000000u)//(32768u)


Uint32 getSysClk (void)

{

Bool pllOD2Bypass;

Uint32 sysClk;

Uint16 pllRD;

Uint16 pllOD;

Uint16 pllOD2;

Uint16 pllM_15_0;

Uint16 pllM_16;

Uint32 pllM;


pllM_15_0 = CSL_FEXT(CSL_SYSCTRL_REGS->PMR, SYS_PMR_PLLM15_0);

pllM_16 = CSL_FEXT(CSL_SYSCTRL_REGS->PICR, SYS_PICR_PLLM16);

pllM = pllM_15_0;

if(1u == pllM_16)

pllM += (65536u);

pllRD = CSL_FEXT(CSL_SYSCTRL_REGS->PICR, SYS_PICR_RD);

pllOD = CSL_FEXT(CSL_SYSCTRL_REGS->PODCR, SYS_PODCR_OD);


pllOD2Bypass = CSL_FEXT(CSL_SYSCTRL_REGS->PODCR, SYS_PODCR_OUTDIV2BY);

pllOD2 = CSL_FEXT(CSL_SYSCTRL_REGS->PODCR, SYS_PODCR_OD2);


sysClk = CSL_PLL_CLOCKIN;


sysClk = sysClk * ((pllM/256)+1);


sysClk = sysClk / ((pllRD+1) * (pllOD+1));


if(0 == pllOD2Bypass)

sysClk = sysClk / (2 * (pllOD2+1));


/* Return the value of system clock in KHz */

return (((Uint32)sysClk)/1000);

}

#else// CHIP_C5517

#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined(CHIP_C5535) || defined(CHIP_C5545))

#define CSL_PLL_CLOCKIN    (32768u)

Uint32 getSysClk(void)
{
	Bool      pllRDBypass;
	Bool      pllOutDiv;
	Uint32    sysClk;
	Uint16    pllM;
	Uint16    pllRD;
	Uint16    pllOD;

	pllM = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR1, SYS_CGCR1_M);

	pllRD = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR2, SYS_CGCR2_RDRATIO);
	pllOD = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR4, SYS_CGCR4_ODRATIO);

	pllRDBypass = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR2, SYS_CGCR2_RDBYPASS);
	pllOutDiv   = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR4, SYS_CGCR4_OUTDIVEN);

	sysClk = CSL_PLL_CLOCKIN;

	if (0 == pllRDBypass)
	{
		sysClk = sysClk/(pllRD + 4);
	}

	sysClk = (sysClk * (pllM + 4));

	if (1 == pllOutDiv)
	{
		sysClk = sysClk/(pllOD + 1);
	}

	/* Return the value of system clock in KHz */
	return(sysClk/1000);
}
#else

#define CSL_PLL_CLOCKIN    (32768u)

Uint32 getSysClk(void)
{
	Bool      pllRDBypass;
	Bool      pllOutDiv;
	Bool      pllOutDiv2;
	Uint32    sysClk;
	Uint16    pllVP;
	Uint16    pllVS;
	Uint16    pllRD;
	Uint16    pllVO;
	Uint16    pllDivider;
	Uint32    pllMultiplier;

	pllVP = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR1, SYS_CGCR1_MH);
	pllVS = CSL_FEXT(CSL_SYSCTRL_REGS->CGICR, SYS_CGICR_ML);

	pllRD = CSL_FEXT(CSL_SYSCTRL_REGS->CGICR, SYS_CGICR_RDRATIO);
	pllVO = CSL_FEXT(CSL_SYSCTRL_REGS->CGOCR, SYS_CGOCR_ODRATIO);

	pllRDBypass = CSL_FEXT(CSL_SYSCTRL_REGS->CGICR, SYS_CGICR_RDBYPASS);
	pllOutDiv   = CSL_FEXT(CSL_SYSCTRL_REGS->CGOCR, SYS_CGOCR_OUTDIVEN);
	pllOutDiv2  = CSL_FEXT(CSL_SYSCTRL_REGS->CGOCR, SYS_CGOCR_OUTDIV2BYPASS);

	pllDivider = ((pllOutDiv2) | (pllOutDiv << 1) | (pllRDBypass << 2));

	pllMultiplier = ((Uint32)CSL_PLL_CLOCKIN * ((pllVP << 2) + pllVS + 4));

	switch(pllDivider)
	{
		case CSL_PLL_DIV_000:
		case CSL_PLL_DIV_001:
			sysClk = pllMultiplier / (pllRD + 4);
		break;

		case CSL_PLL_DIV_002:
			sysClk = pllMultiplier / ((pllRD + 4) * (pllVO + 4) * 2);
		break;

		case CSL_PLL_DIV_003:
			sysClk = pllMultiplier / ((pllRD + 4) * 2);
		break;

		case CSL_PLL_DIV_004:
		case CSL_PLL_DIV_005:
			sysClk = pllMultiplier;
		break;

		case CSL_PLL_DIV_006:
			sysClk = pllMultiplier / ((pllVO + 4) * 2);
		break;

		case CSL_PLL_DIV_007:
			sysClk = pllMultiplier / 2;
		break;
	}

	/* Return the value of system clock in KHz */
	return(sysClk/1000);
}

#endif

#endif// CHIP_C5517

CSL_DMA_Handle CSL_configDmaForGPT(CSL_DMA_ChannelObj    *dmaChanObj,
                                    CSL_DMAChanNum        chanNum)
{
	CSL_DMA_Handle    dmaHandle;
	CSL_Status        status;

	dmaHandle = NULL;

	/* Initialize Dma */
    /*status = DMA_init();
    if (status != CSL_SOK)
    {
        printf("DMA_init Failed!\n");
    }*/

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
