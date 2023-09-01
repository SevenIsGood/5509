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


/** @file csl_daram_retention_example.c
 *
 *  @brief DARAM memory retention mode test example source file
 *
 *
 * \page    page11  MEMORY EXAMPLE DOCUMENTATION
 *
 * \section MEM2   MEMORY EXAMPLE2 - DARAM RETENTION MODE TEST
 *
 * \subsection MEM2x    TEST DESCRIPTION:
 *		This test verifies the memory retention mode feature of the C5535/C5515/C5517
 * DARAM. Internal memory of the C5535/C5515/C5517 DSP can be placed into a low power
 * memory retention while retaining the content stored in the memory. This low
 * power mode is activated through the Sleep Mode Memory Control. When the
 * memory is placed in this mode, no accesses can occur. When the memory
 * retention mode is enabled, inline diodes will be added to reduce the local
 * power supply to the on-chip RAM.Memory retention mode can be enabled/disabled
 * for the DARAM and SARAM independently.
 *
 * A data buffer is allocated in the DARAM is initialized with some
 * data. CSL memory module is initialized and memory retention mode for
 * DARAM is enabled using the CSL API MEM_enableRetentionMode(). Memory
 * retention mode will be disabled after few CPU cycles by using the CSL API
 * MEM_disableRetentionMode(). Data in the DARAM buffer is verified after
 * few CPU cycles of delay. Buffer should contain the data with which it was
 * initialized before enabling the memory retention mode. This proves that
 * the data stored in the DARAM is retained when it is placed into memory
 * retention mode and is accessible after disabling the memory retention mode.
 *
 * NOTE: DURING MEMORY RETENTION MODE TEST FOR DARAM ALL THE PROGRAM SECTIONS
 * SHOULD BE PLACED IN THE SARAM. ACCESSING THE DATA IN THE DARAM BY ANY WAY
 * (CCS WATCH WINDOW, MEMORY WINDOW OR PLACING THE MOUSE POINTER ON THE DARAM
 * DATA BUFFER TO VIEW THE VALUE) AFTER ENABLING THE MEMORY RETENTION MODE
 * WILL CORRUPT ALL THE DATA IN DARAM.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
 * AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection MEM2y    TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5535/C5515/C5517 EVM)
 *  @li Open the project "CSL_MEMORY_DARAM_RetentionExample_Out.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the test result
 *  @li Repeat the test at the following PLL frequencies
 *      C5515: 60MHz and 100MHz
 *      C5517: 60MHz, 100MHz, 150MHz and 200MHz
 *      C5535 eZdsp: 60MHz and 100MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection MEM2z    TEST RESULT:
 *  @li All the CSL APIs should return success
 *  @li DARAM buffer data verification after disabling the memory retention
 *      mode should be successful
 *
 * ============================================================================
 */

/* ============================================================================
 * Revision History
 * ================
 * 06-May-2008 Created for Testing DARAM memory retention mode on C5515 PG1.4
 * 10-Jul-2012 Added C5517 Compatibility
 * 09-Mar-2016 Update the header
 * ============================================================================
 */

#include <csl_mem.h>
#include <csl_general.h>
#include <stdio.h>

#define CSL_DARAM_BUF_SIZE    (100u)
#define CSL_DARAM_TEST_DATA   (0x1234u)
#define CSL_DARAM_DELAY       (100u)

/* Allocate .global section in DARAM for testing DARAM Retention Mode */
#pragma DATA_SECTION (testData,".global")
Uint16    testData[CSL_DARAM_BUF_SIZE];

#define CSL_DARAM_TEST_PASSED    (1u)
#define CSL_DARAM_TEST_FAILED    (0)

/**
 *  \brief  Tests DARAM retention mode
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status CSL_DARAM_RetentionTest(void);


/**
 *  \brief  main function
 *
 *  \param  none
 *
 *  \return none
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
	CSL_Status status;

	printf("DARAM Memory Retention Mode test!\n\n");

	status = CSL_DARAM_RetentionTest();
	if (CSL_DARAM_TEST_PASSED == status)
	{
		printf("\nDARAM Memory Retention Mode test Passed!!\n");
	}
	else
	{
		printf("\nDARAM Memory Retention Mode test Failed!!\n");
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
   /////  Reseting PaSs_StAtE to 0 if error detected here.
        PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
   /////
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
 *  \brief  Tests DARAM retention mode
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status CSL_DARAM_RetentionTest(void)
{
	CSL_Status    status;
	CSL_Status    result;
	Uint16 volatile looper;

	result = CSL_DARAM_TEST_FAILED;

	/* Initialize the memory module */
	status = MEM_init();
	if (status != CSL_SOK)
	{
		printf("MEM_init failed");
		return (result);
	}

	/* Initialize DARAM data buffer */
	for (looper = 0; looper < CSL_DARAM_BUF_SIZE; looper++)
	{
		testData[looper] = CSL_DARAM_TEST_DATA;
	}

	printf("Enabling the Memory Retention Mode\n");
	/* Enable memory retention mode for DARAM */
	status = MEM_enableRetentionMode (CSL_MEM_DARAM);
	if (status != CSL_SOK)
	{
		printf ("Enabling DARAM Memory Retention Mode Failed!\n");
		return (result);
	}

	printf("Wait for few CPU Cycles....\n");

	/* Give a small delay */
	for (looper = 0; looper < CSL_DARAM_DELAY; looper++)
	{
		__asm("\tNOP");
	}

	printf("Disabling the Memory Retention Mode\n");

	/* Disable memory retention mode for DARAM */
	status = MEM_disableRetentionMode (CSL_MEM_DARAM);
	if (status != CSL_SOK)
	{
		printf ("Disabling DARAM Memory Retention Mode Failed!\n");
		return (result);
	}

	/* Give a small delay */
	for (looper = 0; looper < CSL_DARAM_DELAY; looper++)
	{
		__asm("\tNOP");
	}

	/* Verify the data */
	for (looper = 0; looper < CSL_DARAM_BUF_SIZE; looper++)
	{
		if((testData[looper] != CSL_DARAM_TEST_DATA))
		{
			printf("DARAM data is not Retained!\n");
			return (result);
		}
	}

	printf("DARAM Data Buffer verification successful\n");
	printf("DARAM data is Retained!\n");

	result = CSL_DARAM_TEST_PASSED;

	return (result);
}

