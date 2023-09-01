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


/** @file csl_SDRAM_AddressTest_example.c
 *
 * \page    page13  CSL EMIF EXAMPLE DOCUMENTATION
 *
 * \section EMIF7 EMIF EXAMPLE7 SDRAM ADDRESS TEST
 *
 * \subsection EMIF7x TEST DESCRIPTION:
 * csl_SDRAM_AddressTest_example
 *
 * This  will test address/data test, with SDRAM configured for optimum refresh
 * frequency and CAS latency for SDRAM
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515 AND
 * C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection EMIF7y TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5515/C5517 EVM)
 *  @li Open the project "CSL project file" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the test result
 *  @li Repeat the test at the following PLL frequencies
 *      C5515: 60MHz and 100MHz
 *      C5517: 60MHz, 100MHz, 150MHz and 200MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection EMIF7z TEST RESULT:
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
#include <csl_types.h>
#include <csl_dma.h>
#include "CSL_SDRAM_Test.h"

volatile Uint32	gSdramCSnBASE;
volatile Uint32	*gSdramCSnBASEp;

CSL_Status CSL_sdramAddressTest(void);
CSL_Status CSL_emifResetAndClock(void);
CSL_Status CSL_setupSDRAM(void);

CSL_Status CSL_sdramAddressTest(void)
{
	CSL_Status 	status;
	Uint32		i;
	Uint16		j;
	Uint32		testData;

	status 			= CSL_SOK;
	gSdramCSnBASEp 	= (Uint32*)CSL_SDRAM_DATA_WORD_ADDR;

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

	printf("The test has started. It may take a few minutes to complete\n");
	printf("Testing %lx word locations\n", (CSL_SDRAM_TOTAL_LEN_IN_DWORD*2));

	for(j=0; j<4; j++)
	{
		switch(j)
		{
			case 0: testData = CSL_SDRAM_DATA_TEST_VALUE5;
			break;
			case 1: testData = CSL_SDRAM_DATA_TEST_VALUE6;
			break;
			case 2: testData = CSL_SDRAM_DATA_TEST_VALUE7;
			break;
			case 3: testData = CSL_SDRAM_DATA_TEST_VALUE8;
			break;
			default: testData = CSL_SDRAM_DATA_TEST_VALUE7;
			break;
		}

		/*Cpu write*/
		for(i=0; i<CSL_SDRAM_TOTAL_LEN_IN_DWORD; i++)
		{
			*(gSdramCSnBASEp+(Uint32)i) = (Uint32)testData;
		}

		/*Compare and print result*/
		for(i=0;i<CSL_SDRAM_TOTAL_LEN_IN_DWORD;i++) 
		{
			if(testData != *(gSdramCSnBASEp+(Uint32)i))
			{
				printf("COMPARE: FAILED Data doesn't match at %lx. Test data used: %lx\n", i, testData);
				status = CSL_ESYS_FAIL;
				break;
			}
		}
		if(CSL_SDRAM_TOTAL_LEN_IN_DWORD == i)
		{
			printf("COMPARE: Success. Test data used: %lx\n", testData);
			status = CSL_SOK;
		}
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
	//CSL_FINS(sysCtrlRegs->ECDR, SYS_ECDR_EDIV, 0x1);
	CSL_FINS(sysCtrlRegs->ECDR, SYS_ECDR_EDIV, 0x0);

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

	status = CSL_sdramAddressTest();

	if(0 != status)
	{
		printf("SDRAM_AddressTest - Test FAILED!!\n\n");
	   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
	   /////  Reseting PaSs_StAtE to 0 if error detected here.
			PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
	   /////
	}
	else
	{
		printf("SDRAM_AddressTest - Test Passed\n\n");
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

