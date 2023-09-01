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


/** @file CSL_NOR_DmaTransfer.c
 *
 *  @brief EMIF functional layer polled mode example source file
 *
 *
 * \page    page13  CSL EMIF EXAMPLE DOCUMENTATION
 *
 * \section EMIF6 EMIF EXAMPLE6 - NOR DMA MODE TEST
 *
 * \subsection EMIF6x TEST DESCRIPTION:
 * CSL_EMIF_NOR_DmaTransfer
 *
 * NOTE:   Do power-cycle the board each time you run the test
 *         NOR Flash on-board C5517 EVM is a 128Mbit Spansion flash S29Gl128S11DHIV20.
 *
 * In CSL_NOR_Test.h, comment #define EVM5515 1 and Uncomment #define EVM5517 1 if running on C5517 EVM.
 * In CSL_NOR_Test.h, comment #define EVM5517 1 and Uncomment #define EVM5515 1 if running on C5515 EVM.
 *
 * 1. Configure EMIF, start the clock and bring it out of reset.
 * 2. Configure the NOR flash parameters in EMIF registers such as AWCCR1, AWCCR2, ACS2CR1, ACS2CR2, etc.
 * 3. Read the manufacturer and device IDs.
 * 4. Check if the test-block is blocked for r/w or not.
 * 5. Erase the block and prepare it for r/w.
 * 6. Configure the DMA channel 13 to write a "Write-Buffer" size number of words to the NOR flash in the test-block.
 * 7. Send Write-Buffer command followed by number of words to the test-block address in the NOR Flash.
 * 8. Start the DMA so that it transfers the Write-Buffer size number of words to the flash.
 * 9. Issue a reset to change to read array mode.
 * 10. Configure the DMA channel 13 again for a reading the Write-Buffer size number of words.
 * 11. Start the DMA so that it reads Write-Buffer size of data into a local buffer.
 * 12. Perform a comparison of read and written data. There should be no mismatch.
 *
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515 AND
 * C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection EMIF6y TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5515/C5517 EVM)
 *  @li Open the project "CSL_NOR_PollExample_Out.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the test result
 *  @li Repeat the test at the following PLL frequencies
 *      C5515: 60MHz and 100MHz
 *      C5517: 60MHz, 100MHz, 150MHz and 200MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection EMIF6z TEST RESULT:
 *  @li All the CSL APIs should return success
 *  @li Read and write data buffer comparison should be successful.
 *
 */
/* ============================================================================
 * Revision History
 * ================
 * 05-Sep-2008 Created
 * 10-Mar-2016 Updates to header
 * ============================================================================
 */


#include <stdio.h>
#include <string.h>
#include <csl_types.h>
#include "inc\csl_nor.h"
#include <csl_dma.h>
#include "CSL_NOR_Test.h"
#include <csl_intc.h>

/* Save the lock status of 130 blocks */
Uint16	gBlockLockStatus[CSL_NOR_BLOCKLOCK_STATUS_ARRAY];

/* Align is for optimum performance while using Buffered and BEFP Programming */
#ifdef CSL_NOR_USE_ALIGNED_BUFFER
#pragma DATA_ALIGN(gNorReadBuffer,CSL_NOR_BUFFER_ALIGN_BOUNDARY); /*512-byte boundary and it works*/
#endif
Uint16	gNorReadBuffer[CSL_NOR_TEST_BUFFER_WORD_COUNT];

#ifdef CSL_NOR_USE_ALIGNED_BUFFER
#pragma DATA_ALIGN(gNorWriteBuffer,CSL_NOR_BUFFER_ALIGN_BOUNDARY); /*512-byte boundary and it works*/
#endif
Uint16	gNorWriteBuffer[CSL_NOR_TEST_BUFFER_WORD_COUNT];

Uint16	gIntStatus[50];
/*Reference the start of the interrupt vector table*/
extern void VECSTART(void);
CSL_DMA_Handle        dmaWrHandle;
CSL_DMA_Handle        dmaRdHandle;
CSL_DMA_Config        dmaConfig;
CSL_DMA_ChannelObj    dmaWrChanObj;
CSL_DMA_ChannelObj    dmaRdChanObj;

CSL_Status CSL_norConfDmaWriteChan(CSL_NorHandle hNor, Uint32 srcWordOffset,
									Uint32 dDestWordAddr, Uint16 numOfWords);
CSL_Status CSL_norConfDmaReadChan(CSL_NorHandle hNor, Uint32 dSrcWordAddr, Uint16 numOfWords);
interrupt void emif_errorint_isr(void);
CSL_Status CSL_norDmaTransferTest(void);

interrupt void emif_errorint_isr(void)
{
	static Uint16 		i = 0;
	CSL_EmifRegsOvly 	emifRegs;

	/*For instance 0-hard coded*/
	emifRegs = CSL_EMIF_REGS;

	/*Log status*/
	gIntStatus[i] = CSL_FEXTR(emifRegs->EIRR, 5, 0);

	/*Clear status-Dont bother all-bit-write*/
	CSL_FINSR(emifRegs->EIRR, 5, 0, 0x003F);

	/*Increment logger pointer*/
	i++;
}

CSL_Status CSL_norDmaTransferTest(void)
{
	CSL_Status 			status;
	CSL_NorObj			norObj;
	CSL_NorHandle		hNor;
	Uint16				norStatus;
	Uint16				blockNum;
	Uint32				i, j;
	Uint32				dWordAddress;
	Uint16				wordOffset;
#ifdef EVM5515
	CSL_NorAsyncCfg		norAsyncCfg		= PC28F128P30T85_ASYNCCFG_DEFAULT;
	CSL_NorAsyncWaitCfg	norAsyncWaitCfg = PC28F128P30T85_ASYNCWAITCFG_DEFAULT;
	CSL_NorConfig		norCfg			= PC28F128P30T85_NORCFG_DMA;
#elif EVM5517
	CSL_NorAsyncCfg		norAsyncCfg		= S29GL128S11DHIV20_ASYNCCFG_DEFAULT;
	CSL_NorAsyncWaitCfg	norAsyncWaitCfg = S29GL128S11DHIV20_ASYNCWAITCFG_DEFAULT;
	CSL_NorConfig		norCfg			= S29GL128S11DHIV20_NORCFG_DMA;
#endif

	status 				= CSL_SOK;
	hNor 				= &norObj;
	norCfg.asyncWaitCfg	= &norAsyncWaitCfg;
	norCfg.asyncCfg		= &norAsyncCfg;
	j					= j;/*Only used for debug*/

	memset(&gNorReadBuffer, 0x0, sizeof(gNorReadBuffer));
	memset(&gNorWriteBuffer, 0x0, sizeof(gNorWriteBuffer));
	for(i=0; i<CSL_NOR_TEST_BUFFER_WORD_COUNT; i++)
	{
		gNorWriteBuffer[i] = i;
	}

	status = NOR_setup(&norObj, 0);
	if(CSL_SOK != status)
	{
		printf("SETUP: Bad Status\n");
	}

	status = NOR_emifResetAndClock(hNor);
	if(CSL_SOK != status)
	{
		printf("NOR EMIF RST & CLK: Bad Status\n");
	}

	/*norCfg was filled when it was defined*/
	status = NOR_setupEmifChipSelect(hNor, &norCfg);
	if(CSL_SOK != status)
	{
		printf("SETUP EMIF CS: Bad Status\n");
	}

	memset(hNor->deviceID, 0x0, sizeof(hNor->deviceID));
	status = NOR_readId(hNor);
	if(CSL_SOK != status)
	{
		printf("READ ID: Bad Status\n");
	}
	
	if((0xFF == hNor->deviceID[1]) || (0xFFFF == hNor->deviceID[1]))
	{
		printf("No Device Found\n");
	}
	else
	{
		printf("Manufacturer ID %x\n", hNor->deviceID[0]);
		printf("Device ID %x\n", hNor->deviceID[1]);
	}

	memset(hNor->cfiData, 0x0, sizeof(hNor->cfiData));
	status = NOR_readCFI(hNor);
	if(CSL_SOK != status)
	{
		printf("READ CFI: Bad Status\n");
	}

	/*Read Block Lock Status */
	memset(&gBlockLockStatus, 0x0, sizeof(gBlockLockStatus));
	status = NOR_readBlockLockStatus(hNor, &gBlockLockStatus[0]);
	if(CSL_SOK != status)
	{
		printf("READ BLOCK LOCK STATUS: Bad Status\n");
	}

	/*Unlock the Block*/
	blockNum = CSL_NOR_TEST_BLOCK_NUM;
	if(gBlockLockStatus[CSL_NOR_TEST_BLOCK_NUM/16] & (0x1 << (CSL_NOR_TEST_BLOCK_NUM%16)))
	{
		status = NOR_unlockBlock(hNor, blockNum, &norStatus);
		if(CSL_SOK != status)
		{
			printf("UNLOCK BLOCK: Bad Status\n");
		}
		if(norStatus & 0x1)
		{
			printf("Block-%d is LOCKED\n", blockNum);
			if(norStatus & 0x2)
				printf("Block-%d is LOCKED-DOWN\n", blockNum);
			else
				printf("Block-%d is not Locked-Down\n", blockNum);
		}
		else
		{
			printf("Block-%d is Unlocked\n", blockNum);
		}
	}
	else
	{
		printf("Block-%d was Unlocked\n", blockNum);
	}

	/* Erase the Block */
	blockNum = CSL_NOR_TEST_BLOCK_NUM;
	norStatus = 0;
	//status = NOR_eraseBlock(hNor, blockNum, CSL_NOR_DQ7_STATUS, &norStatus);
	status = NOR_eraseBlock(hNor, blockNum, CSL_NOR_DQ6_STATUS, &norStatus);
	if(CSL_SOK == status)
	{
		if(CSL_NOR_INTEL_STD_CMDSET & hNor->commandSet)
		{
			if(norStatus & 0x20)
			{
				printf("ERASE: Failed\n");

				if(norStatus & 0x8)
					printf("ERASE: Vpp range Error\n");
				if((norStatus & 0x30) == 0x30)
					printf("ERASE: Command sequence Error\n");
				if(norStatus & 0x2)
					printf("ERASE: Block Locked Error\n");
			}
		}
		else
		{
			printf("ERASE: Success\n");
		}
	}
	else
	{
		printf("NOR_eraseBlock: Failed\n");
	}

#if 1
    status = DMA_init();
    if (status != CSL_SOK)
    {
        printf("DMA_init() Failed \n");
    }
#endif

	/*Write the Block Using DMA3*/
	blockNum = CSL_NOR_TEST_BLOCK_NUM;
	wordOffset = CSL_NOR_TEST_WORD_OFFSET;
	for(i=0; i<(CSL_NOR_TEST_BUFFER_WORD_COUNT/norCfg.writeBufferSize); i++)
	{
		dWordAddress = ((Uint32)blockNum * hNor->blockLength) + (Uint32)(wordOffset+(i*norCfg.writeBufferSize));
		status = CSL_norConfDmaWriteChan(hNor, (i*norCfg.writeBufferSize), dWordAddress, norCfg.writeBufferSize);
		if(CSL_SOK != status)
		{
			printf("DMA WRITE CHAN CONF: Bad Status\n");
		}

		/* NOR Write Buffer setup*/
		status = NOR_writeBufferSetup(hNor, blockNum, (wordOffset+(i*norCfg.writeBufferSize)),
									norCfg.writeBufferSize);
		if(CSL_SOK != status)
		{
			printf("1. WRITE BUFFER SETUP: Bad Status\n");
			break;
		}

#if 0/*Cpu write for debug purposes*/
		for(j=0; j<norCfg.writeBufferSize; j++)
		{//CSL_VUint16* dummy=0x0;
		//dummy=(CSL_VUint16*)(hNor->CSnBASE+dWordAddress+j);
			*(CSL_VUint16*)(hNor->CSnBASE+dWordAddress+j) =
											gNorWriteBuffer[(i*norCfg.writeBufferSize)+j];
		}
#endif
#if 1
		/* Start Dma write */
		status = DMA_start(dmaWrHandle);
		if(status != CSL_SOK)
		{
			printf("NOR Dma start Failed!!\n");
			return status;
		}

		/* Check Dma transfer status */
		while(DMA_getStatus(dmaWrHandle));
#endif
		status = DMA_close(dmaWrHandle);
		if(status != CSL_SOK)
		{
			printf("NOR DMA close Failed!!\n");
			return status;
		}

		/*NOR Write Buffer complete*/
		status = NOR_writeBufferComplete(hNor, blockNum, (wordOffset+(i*norCfg.writeBufferSize)), 
								norCfg.writeBufferSize, &gNorWriteBuffer[i*norCfg.writeBufferSize],
								//CSL_NOR_DQ7_STATUS, &norStatus);
								CSL_NOR_DQ6_STATUS, &norStatus);
		if(CSL_SOK == status)
		{
			if(CSL_NOR_INTEL_STD_CMDSET & hNor->commandSet)
			{
				if(norStatus & 0x10)
				{
					printf("PROGRAM: Failed\n");

					if(norStatus & 0x8)
						printf("PROGRAM: Vpp range Error\n");
					if((norStatus & 0x30) == 0x30)
						printf("PROGRAM: Command sequence Error\n");
					if(norStatus & 0x2)
						printf("PROGRAM: Block Locked Error\n");
					break;
				}
			}
		}
		else
		{
			printf("2. WRITE BUFFER COMPLETE: Bad Status\n");
			break;
		}

	}

	printf("DMA Data Write to NOR Complete\n");

	/*Read the Block Using DMA3*/
	blockNum = CSL_NOR_TEST_BLOCK_NUM;
	wordOffset = CSL_NOR_TEST_WORD_OFFSET;
	status = NOR_readArray(hNor, blockNum, wordOffset);
	if(CSL_SOK != status)
	{
		printf("READ ARRAY: Bad Status\n");
	}
#if 1
	blockNum = CSL_NOR_TEST_BLOCK_NUM;
	wordOffset = CSL_NOR_TEST_WORD_OFFSET;
	dWordAddress = ((Uint32)blockNum * hNor->blockLength) + (Uint32)wordOffset;
	status = CSL_norConfDmaReadChan(hNor, dWordAddress, CSL_NOR_TEST_BUFFER_WORD_COUNT);
	if(CSL_SOK != status)
	{
		printf("DMA READ CHAN CONF: Bad Status\n");
	}

	/* Start Dma transfer */
	status = DMA_start(dmaRdHandle);
	if(status != CSL_SOK)
	{
		printf("DMA START Failed!!\n");
		return(status);
	}

	/* Check transfer complete status */
	while(DMA_getStatus(dmaRdHandle));

	status = DMA_close(dmaRdHandle);
	if(status != CSL_SOK)
	{
		printf("NOR DMA close Failed!!\n");
		return status;
	}

	printf("DMA Data Read from NOR Complete\n");
#endif
#if 0/*Read cpu for debug purpose*/
	blockNum = CSL_NOR_TEST_BLOCK_NUM;
	dWordAddress = ((Uint32)blockNum * hNor->blockLength)+ (Uint32)wordOffset;
	for(i=0;i<CSL_NOR_TEST_BUFFER_WORD_COUNT;i++)
	{
		gNorReadBuffer[i] = (*(CSL_VUint16*)(hNor->CSnBASE+dWordAddress+i));
	}
#endif

	/*Compare and print result*/
	for(i=0;i<CSL_NOR_TEST_BUFFER_WORD_COUNT;i++) 
	{
		if(gNorReadBuffer[i] != gNorWriteBuffer[i])
		{
			status=CSL_ESYS_FAIL;
			printf("COMPARE: FAILED Data doesn't match at %d\n", i);
			break;
		}
	}
	if(CSL_NOR_TEST_BUFFER_WORD_COUNT == i)
	{
		printf("COMPARE: Success\n");
	}

	return status;
}


CSL_Status CSL_norConfDmaWriteChan(CSL_NorHandle hNor, Uint32 srcWordOffset,
									Uint32 dDestWordAddr, Uint16 numOfWords)
{
	CSL_Status status;

	status = CSL_SOK;

	/*Configure DMA channel for nor write*/
#if ((defined(CHIP_C5505_C5515)) || (defined(CHIP_C5504_C5514)))
	dmaConfig.pingPongMode = CSL_DMA_PING_PONG_DISABLE;
#endif
	dmaConfig.autoMode     = CSL_DMA_AUTORELOAD_DISABLE;
	dmaConfig.burstLen     = CSL_DMA_TXBURST_1WORD;
	dmaConfig.trigger      = CSL_DMA_SOFTWARE_TRIGGER;
	dmaConfig.dmaEvt       = CSL_DMA_EVT_NONE;
	dmaConfig.dmaInt       = CSL_DMA_INTERRUPT_DISABLE;
	dmaConfig.chanDir      = CSL_DMA_WRITE;
	dmaConfig.trfType      = CSL_DMA_TRANSFER_MEMORY;
	//dmaConfig.trfType      = CSL_DMA_TRANSFER_IO_MEMORY;
	/*Data length in bytes*/
	dmaConfig.dataLen      = numOfWords*2;
	dmaConfig.srcAddr      = (Uint32)&gNorWriteBuffer[srcWordOffset];
	/*DMA address of CS2 + offset byte address*/
	//dmaConfig.destAddr     = hNor->DMABYTEADDR+(dDestWordAddr*2);
	dmaConfig.destAddr     = CSL_DMA_CS2_START_ADDR+(dDestWordAddr);


	dmaWrHandle = DMA_open(CSL_NOR_DMA_CHAN, &dmaWrChanObj, &status);
	if(dmaWrHandle == NULL)
    {
        printf("NOR DMA_open of %d-Write Failed!\n", CSL_NOR_DMA_CHAN);
		return -1;
    }

	/* Configure a Dma channel */
	status = DMA_config(dmaWrHandle, &dmaConfig);
    if(status != CSL_SOK)
    {
        printf("NOR DMA_config of %d-Write Failed!\n", CSL_NOR_DMA_CHAN);
        return -1;
    }

    //CSL_DMA3_REGS->DMACH1TCRU &= 0xFC3F;

	return status;
}

CSL_Status CSL_norConfDmaReadChan(CSL_NorHandle hNor, Uint32 dSrcWordAddr, Uint16 numOfWords)
{
	CSL_Status status;

	status = CSL_SOK;

	/* Configure DMA channel  for nor read */
#if ((defined(CHIP_C5505_C5515)) || (defined(CHIP_C5504_C5514)))
	dmaConfig.pingPongMode = CSL_DMA_PING_PONG_DISABLE;
#endif
	dmaConfig.autoMode     = CSL_DMA_AUTORELOAD_DISABLE;
	dmaConfig.burstLen     = CSL_DMA_TXBURST_1WORD;
	dmaConfig.trigger      = CSL_DMA_SOFTWARE_TRIGGER;
	dmaConfig.dmaEvt       = CSL_DMA_EVT_NONE;
	dmaConfig.dmaInt       = CSL_DMA_INTERRUPT_DISABLE;
	dmaConfig.chanDir      = CSL_DMA_READ;
	dmaConfig.trfType      = CSL_DMA_TRANSFER_MEMORY;
	//dmaConfig.trfType      = CSL_DMA_TRANSFER_IO_MEMORY;
	/*Data length in bytes*/
	dmaConfig.dataLen      = numOfWords*2;
	/*DMA address of CS2 + offset byte address*/
	//dmaConfig.srcAddr      = hNor->DMABYTEADDR+(dSrcWordAddr*2);

	dmaConfig.srcAddr      = CSL_DMA_CS2_START_ADDR+(dSrcWordAddr);
	dmaConfig.destAddr     = (Uint32)gNorReadBuffer;

	dmaRdHandle = DMA_open(CSL_NOR_DMA_CHAN, &dmaRdChanObj, &status);
    if(dmaRdHandle == NULL)
    {
        printf("NOR DMA_open of %d-Read Failed!\n", CSL_NOR_DMA_CHAN);
		return -1;
    }

	/* Configure a Dma channel */
	status = DMA_config(dmaRdHandle, &dmaConfig);
    if(status != CSL_SOK)
    {
        printf("NOR DMA_config of %d-Read Failed!\n", CSL_NOR_DMA_CHAN);
        return -1;
    }

    //CSL_DMA3_REGS->DMACH1TCRU &= 0xFC3F;
	return status;
}

/////INSTRUMENTATION FOR BATCH TESTING -- Part 1 --
/////  Define PaSs_StAtE variable for catching errors as program executes.
/////  Define PaSs flag for holding final pass/fail result at program completion.
volatile Int16 PaSs_StAtE = 0x0001; // Init to 1. Reset to 0 at any monitored execution error.
volatile Int16 PaSs = 0x0000; // Init to 0.  Updated later with PaSs_StAtE when and if
/////                                  program flow reaches expected exit point(s).
/////

/**
 * \brief Tests NOR
 * \param None
 * \param None
 */
void main(void)
{
#if (defined(CHIP_C5535) || defined(CHIP_C5545))
	  printf("\nEnsure CHIP_C5517, CHIP_C5505_C5515 or CHIP_C5504_C5514 is #defined. C5535/C5545 don't have EMIF\n");
#else
	CSL_Status	status;

	status = CSL_SOK;

	status = CSL_norDmaTransferTest();

	if(0 != status){
		printf("NOR_DmaTransfer - Test FAILED!!\n\n");
	   	PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
	}else
		printf("NOR_DmaTransfer - Test Passed\n\n");

	   /////INSTRUMENTATION FOR BATCH TESTING -- Part 3 --
   /////  At program exit, copy "PaSs_StAtE" into "PaSs".
   PaSs = PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with
   /////                   // pass/fail value determined during program execution.
   /////  Note:  Program should next exit to C$$EXIT and halt, where DSS, under
   /////   control of a host PC script, will read and record the PaSs' value.
   /////
#endif
}

