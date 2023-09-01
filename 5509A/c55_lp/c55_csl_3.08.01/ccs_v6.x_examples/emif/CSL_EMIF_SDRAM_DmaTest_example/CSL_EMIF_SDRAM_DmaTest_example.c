/*  ============================================================================
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


/** @file CSL_EMIF_SDRAM_DmaTest_example.c
 *
 *  @brief EMIF functional layer SDRAM example source file
 *
 *
 * \page    page13  CSL EMIF EXAMPLE DOCUMENTATION
 *
 * \section EMIF8 EMIF EXAMPLE8 - SDRAM DMA MODE TEST
 *
 * \subsection EMIF8x TEST DESCRIPTION:
 * CSL_EMIF_SDRAM_DmaTest_example
 *
 *
 * Set the system clock frequency to 100 MHz
 * DMA and EMIF peripheral interfaces clocked & brought out of reset.
 * HPI_ON signal should be LOW. For this "HPI_ON in SW4"should towards ON, away from DOT.
 * DMA to be configured to transfer data from one SDRAM location to another.
 * DMA burst-size to be configured to 1-WORD first(32-bit transfer).
 * DMA tx and rx channels started.
 * Transmitted and received data compared to ensure there's no mismatch.
 * Test repeated with 2/4/8/16-WORD burst-sizes.
 *
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515 AND
 * C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection EMIF8y TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5515/C5517 EVM)
 *  @li Open the project "CSL project file" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the test result
 *
 * \subsection EMIF8z TEST RESULT:
 *  @li All the CSL APIs should return success
 *  @li Read and write data buffer comparison should be successful.
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
#include <string.h>
#include <csl_types.h>
#include <csl_dma.h>
#include "CSL_SDRAM_Test.h"

#ifdef CSL_SDRAM_USE_ALIGNED_BUFFER
#pragma DATA_ALIGN(gReadBuffer,32); /*32-byte boundary and it works*/
#endif
Uint16	gReadBuffer[CSL_SDRAM_TEST_BUFFER_WORD_COUNT];
#ifdef CSL_SDRAM_USE_ALIGNED_BUFFER
#pragma DATA_ALIGN(gWriteBuffer,32); /*32-byte boundary and it works*/
#endif
Uint16	gWriteBuffer[CSL_SDRAM_TEST_BUFFER_WORD_COUNT];

volatile Uint32 	gSdramCSnBASE;
Uint32				gSdramDMABYTEADDR;
CSL_DMATxBurstLen	burstLen;

CSL_DMA_Handle        dmaWrHandle;
CSL_DMA_Handle        dmaRdHandle;
CSL_DMA_Config        dmaConfig;
CSL_DMA_ChannelObj    dmaWrChanObj;
CSL_DMA_ChannelObj    dmaRdChanObj;

CSL_Status CSL_sdramDmaTransferTest(void);
CSL_Status CSL_emifResetAndClock(void);
CSL_Status CSL_setupSDRAM(void);
CSL_Status CSL_sdramConfDmaWriteChan(Uint32 dDestWordOffset, Uint16 numOfWords);
CSL_Status CSL_sdramConfDmaReadChan(Uint32 dSrcWordOffset, Uint16 numOfWords);

CSL_Status CSL_sdramDmaTransferTest(void)
{
	CSL_Status 			status;
	Uint32				i, j;
	Uint32				startWordOffset;
	Uint16				numOfWords;

	status 				= CSL_SOK;
	startWordOffset		= CSL_SDRAM_TEST_START_OFFSET;
	numOfWords			= CSL_SDRAM_TEST_BUFFER_WORD_COUNT;
	gSdramCSnBASE		= CSL_SDRAM_DATA_WORD_ADDR;
	gSdramDMABYTEADDR	= CSL_SDRAM_DMA3_BYTE_ADDR;
	burstLen			= CSL_DMA_TXBURST_1WORD;

	  /* Initialize Dma */
	  status = DMA_init();
	  if (status != CSL_SOK)
	  {
	    printf("DMA_init Failed!\n");
	  }

	memset(&gWriteBuffer, 0x0, sizeof(gWriteBuffer));
	for(i=0; i<CSL_SDRAM_TEST_BUFFER_WORD_COUNT; i++)
	{
		gWriteBuffer[i] = i;
	}

	status = CSL_emifResetAndClock();
	if(CSL_SOK != status)
	{
		printf("SDRAM EMIF SETUP: Bad Status\n");
	}

	status = CSL_setupSDRAM();
	if(CSL_SOK != status)
	{
		printf("SDRAM SETUP: Bad Status\n");
	}

	for(j=0; j<5; j++)
	{
		memset(&gReadBuffer, 0x0, sizeof(gReadBuffer));

		switch(j)
		{
			case 0: burstLen = CSL_DMA_TXBURST_1WORD;
			printf("DMA burst length is 1-double word(s)\n");
			break;
			case 1: burstLen = CSL_DMA_TXBURST_2WORD;
			printf("DMA burst length is 2-double word(s)\n");
			break;
			case 2: burstLen = CSL_DMA_TXBURST_4WORD;
			printf("DMA burst length is 4-double word(s)\n");
			break;
			case 3: burstLen = CSL_DMA_TXBURST_8WORD;
			printf("DMA burst length is 8-double word(s)\n");
			break;
			case 4: burstLen = CSL_DMA_TXBURST_16WORD;
			printf("DMA burst length is 16-double word(s)\n");
			break;
			default: burstLen = CSL_DMA_TXBURST_1WORD;
			printf("DMA burst length is 1-double word(s)\n");
			break;
		}

		/*Dma write*/
		status = CSL_sdramConfDmaWriteChan((startWordOffset+(j*numOfWords)), numOfWords);
		if(CSL_SOK != status)
		{
			printf("SDRAM DMA WRITE CHAN CONF: Bad Status\n");
		}

		/* Start Dma write */
		status = DMA_start(dmaWrHandle);
		if(status != CSL_SOK)
		{
			printf("DMA WRITE START Failed!!\n");
			return status;
		}

		/* Check transfer complete status */
		while(DMA_getStatus(dmaWrHandle));

		status = DMA_close(dmaWrHandle);
		if(status != CSL_SOK)
		{
			printf("SRAM DMA close Failed!!\n");
			return status;
		}

		printf("DMA Data Write of %d-words to SDRAM @%lx Complete\n", numOfWords,
				(gSdramCSnBASE+startWordOffset+(j*numOfWords)));

		/*Dma read*/
		status = CSL_sdramConfDmaReadChan((startWordOffset+(j*numOfWords)), numOfWords);
		if(CSL_SOK != status)
		{
			printf("SDRAM DMA READ CHAN CONF: Bad Status\n");
		}

		/* Start Dma read */
		status = DMA_start(dmaRdHandle);
		if(status != CSL_SOK)
		{
			printf("DMA READ START Failed!!\n");
			return(status);
		}

		/* Check transfer complete status */
		while(DMA_getStatus(dmaRdHandle));

		status = DMA_close(dmaRdHandle);
		if(status != CSL_SOK)
		{
			printf("SRAM DMA close Failed!!\n");
			return status;
		}

		printf("DMA Data Read of %d-words from SDRAM @%lx Complete\n", numOfWords,
				(gSdramCSnBASE+startWordOffset+(j*numOfWords)));

		/*Compare and print result*/
		for(i=0;i<numOfWords;i++) 
		{
			if(gReadBuffer[i] != gWriteBuffer[i])
			{
				printf("COMPARE: FAILED Data doesn't match at %d\n", i);
				status = CSL_ESYS_FAIL;
				break;
			}
		}
		if(numOfWords == i)
		{
			printf("COMPARE: Success\n");
			status = CSL_SOK;
		}
	}

	return status;
}

CSL_Status CSL_sdramConfDmaWriteChan(Uint32 dDestWordOffset, Uint16 numOfWords)
{
	CSL_Status status;

	status = CSL_SOK;

	/*Configure DMA channel for nor write*/
#if ((defined(CHIP_C5505_C5515)) || (defined(CHIP_5504_C5514)))
	dmaConfig.pingPongMode = CSL_DMA_PING_PONG_DISABLE;
#endif
	dmaConfig.autoMode     = CSL_DMA_AUTORELOAD_DISABLE;
	dmaConfig.burstLen     = burstLen;
	dmaConfig.trigger      = CSL_DMA_SOFTWARE_TRIGGER;
	dmaConfig.dmaEvt       = CSL_DMA_EVT_NONE;
	dmaConfig.dmaInt       = CSL_DMA_INTERRUPT_DISABLE;
	dmaConfig.chanDir      = CSL_DMA_WRITE;
	dmaConfig.trfType      = CSL_DMA_TRANSFER_MEMORY;
	/*Data length in bytes*/
	dmaConfig.dataLen      = numOfWords*2;
	dmaConfig.srcAddr      = (Uint32)&gWriteBuffer[0];
	/*DMA address of CS2 + offset byte address*/
	dmaConfig.destAddr     = gSdramDMABYTEADDR+(dDestWordOffset*2);

	dmaWrHandle = DMA_open(CSL_SDRAM_DMA_CHAN, &dmaWrChanObj, &status);
	if(dmaWrHandle == NULL)
    {
        printf("DMA_open of Chan%d-Write Failed!\n", CSL_SDRAM_DMA_CHAN);
		return -1;
    }

	/* Configure a Dma channel */
	status = DMA_config(dmaWrHandle, &dmaConfig);
    if(status != CSL_SOK)
    {
        printf("DMA_config of Chan%d-Write Failed!\n", CSL_SDRAM_DMA_CHAN);
        return -1;
    }

	return status;
}

CSL_Status CSL_sdramConfDmaReadChan(Uint32 dSrcWordOffset, Uint16 numOfWords)
{
	CSL_Status status;

	status = CSL_SOK;

	/* Configure DMA channel  for nor read */
#if ((defined(CHIP_C5505_C5515)) || (defined(CHIP_5504_C5514)))
	dmaConfig.pingPongMode = CSL_DMA_PING_PONG_DISABLE;
#endif
	dmaConfig.autoMode     = CSL_DMA_AUTORELOAD_DISABLE;
	dmaConfig.burstLen     = burstLen;
	dmaConfig.trigger      = CSL_DMA_SOFTWARE_TRIGGER;
	dmaConfig.dmaEvt       = CSL_DMA_EVT_NONE;
	dmaConfig.dmaInt       = CSL_DMA_INTERRUPT_DISABLE;
	dmaConfig.chanDir      = CSL_DMA_READ;
	dmaConfig.trfType      = CSL_DMA_TRANSFER_MEMORY;
	/*Data length in bytes*/
	dmaConfig.dataLen      = numOfWords*2;
	/*DMA address of CS2 + offset byte address*/
	dmaConfig.srcAddr      = gSdramDMABYTEADDR+(dSrcWordOffset*2);
	dmaConfig.destAddr     = (Uint32)&gReadBuffer[0];

	dmaRdHandle = DMA_open(CSL_SDRAM_DMA_CHAN, &dmaRdChanObj, &status);
    if(dmaRdHandle == NULL)
    {
        printf("DMA_open of Chan%d-Read Failed!\n", CSL_SDRAM_DMA_CHAN);
		return -1;
    }

	/* Configure a Dma channel */
	status = DMA_config(dmaRdHandle, &dmaConfig);
    if(status != CSL_SOK)
    {
        printf("DMA_config of Chan%d-Read Failed!\n", CSL_SDRAM_DMA_CHAN);
        return -1;
    }

	return status;
}

CSL_Status CSL_emifResetAndClock(void)
{
	CSL_Status 				status;
	CSL_SysRegsOvly 		sysCtrlRegs;
	Uint16					i;

	status 		= CSL_SOK;
	sysCtrlRegs = CSL_SYSCTRL_REGS;

	/* Configure EMIF */
	/* 1c26h ECDR */
	CSL_FINS(sysCtrlRegs->ECDR, SYS_ECDR_EDIV, 0x1);

	/* 1c04h PSRCR - Set Reset Count */
	CSL_FINS(sysCtrlRegs->PSRCR, SYS_PSRCR_COUNT, 0x20);

	/* 1c05h PRCR - enable emif self-clearing*/
	CSL_FINS(sysCtrlRegs->PRCR, SYS_PRCR_PG4_RST, 0x0);

	/* 1c05h PRCR - Reset EMIF */
	CSL_FINS(sysCtrlRegs->PRCR, SYS_PRCR_PG1_RST, 0x1);

	/* Give some delay for the device to reset */
	for(i = 0; i < 100; i++){;}

	/* 1C02h Enable EMIF module in Idle PCGCR */
	CSL_FINST(sysCtrlRegs->PCGCR1, SYS_PCGCR1_EMIFCG, ACTIVE);
	
	/* 1c33h ESCR - EMIF generates 16bit access to External Memory for every word access */
	CSL_FINS(sysCtrlRegs->ESCR, SYS_ESCR_BYTEMODE, 0x0);

	return status;
}

CSL_Status CSL_setupSDRAM(void)
{
	CSL_Status status;

	status = CSL_SOK;

	*(ioport volatile unsigned *)0x1C33 = 0x0000;	// EMIF_ACCESS

	*(ioport volatile unsigned *)0x1020 = 0x4710;	// SDTIMR1
	*(ioport volatile unsigned *)0x1021 = 0x3911;	// SDTIMR2
	*(ioport volatile unsigned *)0x100C = 0x061A;	// SDRCR SDRAM Refresh Control Register
	*(ioport volatile unsigned *)0x1008 = 0x4720;	// SDCR1
	*(ioport volatile unsigned *)0x1009 = 0x0001;	// SDCR2
	*(ioport volatile unsigned *)0x1C1E = 0x0001;	// CCR1 SD Clock ON/OFF

	return status;
}


/**
 * \brief Tests SDRAM
 * \param None
 * \param None
 */
/////INSTRUMENTATION FOR BATCH TESTING -- Part 1 --
/////  Define PaSs_StAtE variable for catching errors as program executes.
/////  Define PaSs flag for holding final pass/fail result at program completion.
     volatile Int16 PaSs_StAtE = 0x0001; // Init to 1. Reset to 0 at any monitored execution error.
     volatile Int16 PaSs = 0x0000; // Init to 0.  Updated later with PaSs_StAtE when and if
/////                                  program flow reaches expected exit point(s).
/////
void main(void)
{
#if (defined(CHIP_C5535) || defined(CHIP_C5545))
	  printf("\nEnsure CHIP_C5517, CHIP_C5505_C5515 or CHIP_C5504_C5514 is #defined. C5535/C5545 don't have EMIF\n");
#else
	CSL_Status	status;

	status = CSL_SOK;

	status = CSL_sdramDmaTransferTest();

	if(0 != status)
	{
		printf("SDRAM_DmaTest - Test FAILED!!\n\n");
	   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
	   /////  Reseting PaSs_StAtE to 0 if error detected here.
			PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
	   /////
	}
	else
	{
		printf("SDRAM_DmaTest - Test Passed\n\n");
	   /////INSTRUMENTATION FOR BATCH TESTING -- Part 3 --
	   /////  At program exit, copy "PaSs_StAtE" into "PaSs".
			PaSs = PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with
	   /////                   // pass/fail value determined during program execution.
	   /////  Note:  Program should next exit to C$$EXIT and halt, where DSS, under
	   /////   control of a host PC script, will read and record the PaSs' value.
	   /////
	}
#endif
}

