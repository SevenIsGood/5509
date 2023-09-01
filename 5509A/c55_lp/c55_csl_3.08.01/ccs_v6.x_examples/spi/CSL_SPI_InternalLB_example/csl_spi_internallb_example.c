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


/** @file csl_spi_internallb_example.c
 *
 *  @brief SPI functional layer sample source file
 *
 *
 * \page    page17  CSL SPI EXAMPLE DOCUMENTATION
 *
 * \section SPI2   SPI EXAMPLE2 - SPI INTERNAL LOOPBACK MODE TEST
 *
 * \subsection SPI2x    TEST DESCRIPTION:
 * 1. Enable clocks to SPI and configure PPMODE in EBSR for SPI.
 * 2. Configure the SPI interface: clk, wordlength, framelength, chip sel,
 *    data delay, clk and chip sel polarities.
 * 3. Enable Loopback in SPIDCRU.
 * 4. Fill in CPU Write data buffer with alternate 0x11's and 0xAB's and
 *    Read Data buffer with alternate 0x00's and 0xCD's.
 * 4. Write Data into SPIDRU and collect it from SPIDRL.
 * 5. Compare the write and read data. There shouldn't be any data mismatch.
 *    Read Data Buffer should now have alternate 0x11's and 0xAB's.
 *
*/
/* ============================================================================
 * Revision History
 * ================
 * 15-Apr-2015 Created
 * 10-Mar-2016 Updates to header
 * ============================================================================
 */

#include "csl_spi.h"
#include "csl_general.h"
#include <stdio.h>

#define CSL_TEST_FAILED         (1)
#define CSL_TEST_PASSED         (0)

#define	CSL_SPI_BUF_LEN			(64)
#define	SPI_CLK_DIV				(25)
#define	SPI_FRAME_LENGTH		(1)

Uint16 spiWriteBuff[CSL_SPI_BUF_LEN];
Uint16 spiReadBuff[CSL_SPI_BUF_LEN];


Int16 spi_sample(void);

/////INSTRUMENTATION FOR BATCH TESTING -- Part 1 --
/////  Define PaSs_StAtE variable for catching errors as program executes.
/////  Define PaSs flag for holding final pass/fail result at program completion.
volatile Int16 PaSs_StAtE = 0x0001; // Init to 1. Reset to 0 at any monitored execution error.
volatile Int16 PaSs = 0x0000; // Init to 0.  Updated later with PaSs_StAtE when and if
/////                                  program flow reaches expected exit point(s).
/////

void main(void)
{
	Int16    status;

	printf("CSL SPI Internal Loopback Test\n\n");
	status = spi_sample();
	if(status != CSL_TEST_PASSED)
	{
		printf("\nCSL SPI Internal Loop back Test Failed!!\n");
	   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
   	   /////  Reseting PaSs_StAtE to 0 if error detected here.
       PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
       /////
	}
	else
	{
		printf("\nCSL SPI Internal Loop back Test Passed!!\n");
	}
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 3 --
   /////  At program exit, copy "PaSs_StAtE" into "PaSs".
   PaSs = PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with
   /////                   // pass/fail value determined during program execution.
   /////  Note:  Program should next exit to C$$EXIT and halt, where DSS, under
   /////   control of a host PC script, will read and record the PaSs' value.
   /////

}

Int16 spi_sample(void)
{
	Int16 			status = CSL_TEST_PASSED;
	Int16 			result;
	CSL_SpiHandle	hSpi;
	SPI_Config		hwConfig;
	volatile Uint32	looper;

	result = SPI_init();

	if(CSL_SOK != result)
	{
		status = CSL_TEST_FAILED;
		return (status);
	}
	else
	{
		printf ("SPI Instance Initialize successfully\n");
	}

	hSpi = SPI_open(SPI_CS_NUM_0, SPI_POLLING_MODE);

	if(NULL == hSpi)
	{
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf ("SPI Instance Opened successfully\n");
	}

	/** Set the hardware configuration 							*/
	hwConfig.spiClkDiv	= SPI_CLK_DIV;
	hwConfig.wLen		= SPI_WORD_LENGTH_8;
	hwConfig.frLen		= SPI_FRAME_LENGTH;
	hwConfig.wcEnable	= SPI_WORD_IRQ_DISABLE;
	hwConfig.fcEnable	= SPI_FRAME_IRQ_DISABLE;
	hwConfig.csNum		= SPI_CS_NUM_0;
	hwConfig.dataDelay	= SPI_DATA_DLY_0;
	hwConfig.csPol		= SPI_CSP_ACTIVE_LOW;
	hwConfig.clkPol		= SPI_CLKP_LOW_AT_IDLE;
	hwConfig.clkPh		= SPI_CLK_PH_FALL_EDGE;

	result = SPI_config(hSpi, &hwConfig);

	if(CSL_SOK != result)
	{
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf ("SPI Instance Configured successfully\n");
	}

	/*Enable the Internal Loopback mode*/
#if (defined(CHIP_C5517))
    CSL_FINST(CSL_SPI_REGS->SPIDCRU,SPI_SPIDCRU_LPBK,ENABLE);
#else
#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined(CHIP_C5535) || defined(CHIP_C5545))
    CSL_FINST(CSL_SPI_REGS->SPIDCR2,SPI_SPIDCR2_LPBK,ENABLE);
#endif
#endif

	for(looper = 0; looper < 64; )
	{
		spiWriteBuff[looper] = 0;
		spiWriteBuff[(looper + 1)] = 0;
		spiReadBuff[looper] = 0;
		spiReadBuff[(looper + 1)] = 0;
		looper += 2;
	}
	for(looper = 0; looper < 64; )
	{
		spiWriteBuff[looper] = 0x0011;
		spiWriteBuff[(looper + 1)] = 0x00AB;
		spiReadBuff[looper] = 0x0000;
		spiReadBuff[(looper + 1)] = 0x00CD;
		looper += 2;
	}

	/* Write the Data to the SPI*/
	result = SPI_dataTransaction(hSpi ,spiWriteBuff, CSL_SPI_BUF_LEN, SPI_WRITE);
	if(CSL_SOK != result)
	{
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf ("SPI Instance Write successfully\n");
	}
	/* Read the Data to the SPI*/
	result = SPI_dataTransaction(hSpi ,spiReadBuff, CSL_SPI_BUF_LEN, SPI_READ);
	if(CSL_SOK != result)
	{
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf ("SPI Instance Read successfully\n");
	}

	result = SPI_close(hSpi);

	if(CSL_SOK != result)
	{
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf ("SPI Instance Close successfully\n");
	}

	/* Compare the Data */
	for( looper = 0 ; looper  < CSL_SPI_BUF_LEN ; looper++ )
	{
		if(spiWriteBuff[looper] != spiReadBuff[looper] )
		{
			return (CSL_TEST_FAILED);
		}
    }
	return (status);

}

