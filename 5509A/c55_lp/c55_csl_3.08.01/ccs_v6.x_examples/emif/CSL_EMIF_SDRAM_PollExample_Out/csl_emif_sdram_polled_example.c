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


/** @file csl_emif_sdram_polled_example.c
 *
 *  @brief EMIF functional layer polled mode example source file
 *
 *
 * \page    page13  CSL EMIF EXAMPLE DOCUMENTATION
 *
 * \section EMIF9 EMIF EXAMPLE9 - SDRAM POLL MODE TEST
 *
 * \subsection EMIF9x    TEST DESCRIPTION:
 *      Following example code tests the read and write operations of CSL
 * EMIF SDRAM module in polled mode. A 1KB data is written and read from
 * SDRAM in this test.Data buffer "gsdramWriteBuf" will hold the data to
 * be written to SDRAM.Data buffer "gsdramReadBuf" is used to store the
 * data read from the SDRAM device. SDRAM address of 0x28000 is written/read
 * in the current code. Address can be changed in the write and read operation APIs
 *
 * During the test CSL EMIF module will be initialized and configured write
 * operation. SDRAM configurations are set. Data is written to the SDRAM device
 * using CSL API SDRAM_write.
 *
 * The Powerdown mode and Self refresh modes are enabled and disabled using the
 * respective APIs. Then data is read from SDRAM to a buffer and the write buffer
 * and read buffer contents are compared. Test is passed if the contents match.
 *
 * NOTE: This test assumes that
 *  a. SDRAM is connected to emif on C5515 EVM and on C5517 EVM
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515 AND
 * C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection EMIF9y    TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5515/C5517 EVM)
 *  @li Open the project "CSL_EMIF_SDRAM_PollExample_Out.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the test result
 *  @li Repeat the test at the following PLL frequencies
 *      C5515: 60MHz and 100MHz
 *      C5517: 60MHz, 100MHz, 150MHz and 200MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection EMIF9z    TEST RESULT:
 *  @li All the CSL APIs should return success
 *  @li Read and write data buffer comparison should be successful.
 *
 * ============================================================================
 */

/* ============================================================================
 * Revision History
 * ================
 * 19-Feb-2013 Created
 * 09-Mar-2016 Update the header
 * ============================================================================
 */

#include <csl_emif.h>
#include <csl_general.h>
#include <stdio.h>

#define CSL_TEST_FAILED         (1)
#define CSL_TEST_PASSED         (0)

#define CSL_SDRAM_DELAY_CYCLES  (4096)
#define CSL_SDRAM_BUF_SIZE      (1024)

Uint16  gsdramReadBuf[CSL_SDRAM_BUF_SIZE];
Uint16  gsdramWriteBuf[CSL_SDRAM_BUF_SIZE];


/**
 *  \brief  Tests SDRAM polled mode operation
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status CSL_sdramPolledTest(void);

/**
 *  \brief  Tests SDRAM EMIF CSL module
 *
 *  \param  None
 *
 *  \return None
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
	CSL_Status    result;

	/* Tests the sdram operation in Polled mode */
	result = CSL_sdramPolledTest();

	if(result != 0)
	{
		printf("\nSDRAM Polled Mode Test Failed!!\n");
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
   /////  Reseting PaSs_StAtE to 0 if error detected here.
        PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
   /////
	}
	else
	{
		printf("\nSDRAM Polled Mode Test Passed!!\n");
	}
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 3 --
   /////  At program exit, copy "PaSs_StAtE" into "PaSs".
        PaSs = PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with
   /////                   // pass/fail value determined during program execution.
   /////  Note:  Program should next exit to C$$EXIT and halt, where DSS, under
   /////   control of a host PC script, will read and record the PaSs' value.
   /////
#endif
}

/**
 *  \brief  Tests SDRAM polled mode operation
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status  CSL_sdramPolledTest(void)
{
	CSL_SdramCfg            sdramCfg;
	CSL_SdramTimr           sdramTimr;
	CSL_EmifHandle          hEmif;
    CSL_SdramRowSize        rowSize;
	CSL_EmifObj             emifObj;
	CSL_Status              status;
	CSL_Status              result;
	Uint16                  i;
	Uint16                  looper;

	/*Uint16 rd, od, od2, pllm,RefClk;
	Uint32  sysclk, vco;
	float temp;
	for(pllm=0x3000;pllm<=0xFFFF;pllm++)
	{
		for(rd=0;rd<=0x3F;rd++)
		{
			for(od=0;od<=0x7;od++)
			{
				for(od2=0;od2<=0x1F;od2++)
				{

					//temp=pllm/256.0+1;
					temp=pllm/256+1;
					RefClk = 12000/(rd+1);
					vco = temp * (Uint32)RefClk;
					sysclk= vco/((od+1)*2*(od2+1));
					//if(pllm==0x41ab)
						//if(rd==0x3)
							//if(od==0)
								//if(od2==0)
									//break;
					if(sysclk==100000)
						break;
				}
				if(sysclk==100000)
										break;
			}
			if(sysclk==100000)
									break;
		}
		if(sysclk==100000)
								break;
	}
	*/
	result  = CSL_TEST_FAILED;
	hEmif   = &emifObj;
	rowSize = CSL_SDRAM_ROW_ADDR_BITS_9;

	for(looper=0; looper < CSL_SDRAM_BUF_SIZE; looper++)
	{
		gsdramWriteBuf[looper] = looper;
		gsdramReadBuf[looper]  = 0x0;
	}

	printf("SDRAM Polled mode test\n\n");

	/* Initialize Emif module */
	status = EMIF_init(&emifObj);
	if(status != CSL_SOK)
	{
		printf("EMIF SDRAM Init Failed!!\n");
		return(result);
	}

	/* Assign values to sdram config structure */
    sdramCfg.NM           = CSL_SDRAM_SDCR1_NM_DEFAULT;
	sdramCfg.CasLatency   = CSL_SDRAM_SDCR1_CL_DEFAULT;
	sdramCfg.bit11to9Lock = CSL_SDRAM_SDCR1_BIT_11_9_LOCK_DEFAULT;
	sdramCfg.ibankPos     = CSL_SDRAM_SDCR1_IBANK_DEFAULT;
	sdramCfg.ebank        = CSL_SDRAM_SDCR1_EBANK_DEFAULT;
	sdramCfg.pageSize     = CSL_SDRAM_SDCR1_PAGESIZE_DEFAULT;
	sdramCfg.selfRefresh  = CSL_SDRAM_SDCR2_SR_DEFAULT;
	sdramCfg.pdMode       = CSL_SDRAM_SDCR2_PD_DEFAULT;
	sdramCfg.pdwr         = CSL_SDRAM_SDCR2_PDWR_DEFAULT;
	sdramCfg.pasr         = CSL_SDRAM_SDCR2_PASR_DEFAULT;
	sdramCfg.rowSize      = rowSize;
	sdramCfg.ibankPos     = CSL_SDRAM_SDCR2_IBANK_POS_DEFAULT;
	sdramCfg.sdramDrive   = CSL_SDRAM_SDCR2_SDRAM_DRIVE_DEFAULT;
	sdramCfg.bit9to1Lock  = CSL_SDRAM_SDCR2_BIT_9_1_LOCK_DEFAULT;

    sdramTimr.tRAS        = CSL_SDRAM_TRAS_VAL;
    sdramTimr.tRC         = CSL_SDRAM_TRC_VAL;
	sdramTimr.tRRD        = CSL_SDRAM_TRRD_VAL;
	sdramTimr.tRFC        = CSL_SDRAM_TRFC_VAL;
	sdramTimr.tRP         = CSL_SDRAM_TRP_VAL;
	sdramTimr.tRCD        = CSL_SDRAM_TRCD_VAL;
	sdramTimr.tWR         = CSL_SDRAM_TWR_VAL;
	sdramTimr.tXS         = CSL_SDRAM_TXS_VAL;
    sdramTimr.refRate     = CSL_SDRAM_REFRATE_VAL;


	/* Configure SDRAM settings */
	status = SDRAM_config(hEmif, &sdramCfg, &sdramTimr);
	if(status != CSL_SOK)
	{
		printf("EMIF SDRAM Config Failed!!\n");
		return(result);
	}

    for(i=0;i<CSL_SDRAM_DELAY_CYCLES;i++){;}

    /* SDRAM power down mode is enabled */
    status = SDRAM_enablePowerDownMode(hEmif);
	printf("\nSDRAM is in power down mode!\n");

    for(i=0;i<CSL_SDRAM_DELAY_CYCLES;i++){;}

    /* SDRAM power down mode is disabled */
	status = SDRAM_disablePowerDownMode(hEmif);
	printf("\nSDRAM is out of power down mode!\n");

    /* SDRAM self refresh mode is enabled */
    status = SDRAM_enableSelfRefreshMode(hEmif);
	printf("\nSDRAM is in self refresh mode!\n");

    for(i=0;i<CSL_SDRAM_DELAY_CYCLES;i++){;}

    /* SDRAM self refresh mode is disabled */
	status = SDRAM_disableSelfRefreshMode(hEmif);
	printf("\nSDRAM is out of self refresh mode!\n");

	/* Perform write operation to SDRAM */
	status = SDRAM_write(hEmif, CSL_SDRAM_BUF_SIZE, (CSL_SDRAM_START_ADDR), &gsdramWriteBuf[0]);
	if(status != CSL_SOK)
	{
		printf("EMIF SDRAM Write operation Failed!!\n");
		return(result);
	}

	/* Perform read operation to SDRAM */
	status = SDRAM_read(hEmif, CSL_SDRAM_BUF_SIZE, (CSL_SDRAM_START_ADDR), &gsdramReadBuf[0]);
	if(status != CSL_SOK)
	{
		printf("EMIF SDRAM Read operation Failed!!\n");
		return(result);
	}

	/* Compare Read-Write Buffers */
	for(looper = 0; looper < CSL_SDRAM_BUF_SIZE; looper++)
	{
		if(gsdramWriteBuf[looper] != gsdramReadBuf[looper])
		{
			printf("\nSDRAM Read Write Buffers doesn't Match!\n");
			return(result);
		}
	}

	printf("\nSDRAM Read Write Buffers Match!\n");

	result = CSL_TEST_PASSED;
	return(result);
}




