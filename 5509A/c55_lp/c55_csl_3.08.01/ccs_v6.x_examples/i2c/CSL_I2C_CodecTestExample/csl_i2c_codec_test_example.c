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


/** @file csl_i2c_codec_test_example.c
 *
 *  @brief Example to configure the Audio Codec using I2C in polled mode
 *
 *
 * \page    page5  I2C EXAMPLE DOCUMENTATION
 *
 * \section I2C6   I2C EXAMPLE6 - Codec Configure TEST
 *
 * \subsection I2C6x    TEST DESCRIPTION:
 *		This test verifies the operation of configuring AUdio codec using CSL
 * I2C module in polled mode.
 *
 * During the test the Audio Codec registers are configured, by writing some
 * value to them using the I2C_write() API provided by the CSL I2C module.
 * After the I2C_write() API is successful, the configured codec register
 * values are read back using I2C_read() API. Finally the written and the read
 * values are compared and validated whether they are same or not. Depending on
 * the comparison results, the example code returns success or failure.
 *
 * I2C operates at all the system clock frequencies. Value of the system clock
 * at which CPU is running during the test should be passed to the I2C_setup()
 * function. This will be used to calculate I2C clock and prescaler values.
 * 'CSL_I2C_SYS_CLK' macro is defined with different system clock values.
 * Depending on the system clock value corresponding value definition of the
 * macro should be uncommented and re-build the project for proper operation
 * of the test.
 *
 * Both read and write APIs are synchronous and returns to the application only
 * after completing the data transfer. A small delay is required after each data
 * operation to allow the device to get ready for the next operation.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
 * AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection I2C6y    TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5535/C5515 EVM or C5517 EVM)
 *  @li Open the project "CSL_I2C_CodecTestExample.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the test result
 *  @li Repeat the test at the following PLL frequencies
 *      C5515: 60MHz and 100MHz
 *      C5517: 60MHz, 100MHz, 150MHz and 200MHz
 *      C5535 eZdsp: 60MHz and 100MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection I2C6z    TEST RESULT:
 *  @li All the CSL APIs should return success
 *  @li Read and write value comparison should be successful.
 *
 * ============================================================================
 */

/* ============================================================================
 * Revision History
 * ================
 * 16-Oct-2008 Created
 * 10-Jul-2012 Added C5517 Compatibility
 * 09-Mar-2016 Update the header
 * ============================================================================
 */

#include <csl_i2c.h>
#include <stdio.h>
#include <csl_general.h>

#define CSL_I2C_TEST_PASSED      (0)
#define CSL_I2C_TEST_FAILED      (1)

#define CSL_I2C_DATA_SIZE        (64)
#define CSL_I2C_OWN_ADDR         (0x2F)
//#define CSL_I2C_SYS_CLK          (12.228)
//#define CSL_I2C_SYS_CLK          (40)
//#define CSL_I2C_SYS_CLK          (60)
//#define CSL_I2C_SYS_CLK          (75)
#define CSL_I2C_SYS_CLK          (100)
#define CSL_I2C_BUS_FREQ         (10)
#define CSL_I2C_EEPROM_ADDR		 (0x50)
#define CSL_I2C_CODEC_ADDR		 (0x18)


CSL_I2cSetup     i2cSetup;
CSL_I2cConfig    i2cConfig;

/**
 *  \brief  Tests I2C polled mode operation by configuring Codec register
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status CSL_i2cCodecConfigureTest(void);

/**
 *  \brief  Tests i2c_write and i2c_read APIs by configuring the Codec register
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status CSL_testRegister(int regAddr, int regValue);

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
	CSL_Status    result;

	printf("I2C CODEC CONFIGURING IN POLLED MODE TEST!\n\n");

	result =  CSL_i2cCodecConfigureTest();

	if(result == CSL_I2C_TEST_PASSED)
	{
		printf("\nI2C CODEC CONFIGURING IN POLLED MODE TEST PASSED!!\n");
	}
	else
	{
		printf("\nI2C CODEC CONFIGURING IN POLLED MODE TEST FAILED!!\n");
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
 *  \brief  Tests I2C Codec Configuring In Polled Mode Test
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status  CSL_i2cCodecConfigureTest(void)
{
	CSL_Status         status;
	CSL_Status         result;
	volatile Uint16    looper, i;
	int                regAddr;
	int                regValue;

	result = CSL_I2C_TEST_FAILED;

	*(ioport volatile unsigned *) 0x1c00 = 0x2 << 10;	// Select GPIO10
	*(ioport volatile unsigned *) 0x1c06 |= 0x400;		// Set GPIO-10 as output
	for(i=1;i<10;i++ )
		asm("	nop");
	*(ioport volatile unsigned *)0x1c0A |= 0x000;	// Set GPIO-10 = 1
	for(i=1;i<100;i++ )
		asm("	nop");
	*(ioport volatile unsigned *)0x1c0A |= 0x400;	// Set GPIO-10 = 1
	for(i=1;i<10;i++ )
		asm("	nop");

	/* Initialize I2C module */
	status = I2C_init(CSL_I2C0);
	if(status != CSL_SOK)
	{
		printf("I2C Init Failed!!\n");
		return(result);
	}

	/* Setup I2C module */
	i2cSetup.addrMode    = CSL_I2C_ADDR_7BIT;
	i2cSetup.bitCount    = CSL_I2C_BC_8BITS;
	i2cSetup.loopBack    = CSL_I2C_LOOPBACK_DISABLE;
	i2cSetup.freeMode    = CSL_I2C_FREEMODE_DISABLE;
	i2cSetup.repeatMode  = CSL_I2C_REPEATMODE_DISABLE;
	i2cSetup.ownAddr     = CSL_I2C_OWN_ADDR;
	i2cSetup.sysInputClk = CSL_I2C_SYS_CLK;
	i2cSetup.i2cBusFreq  = CSL_I2C_BUS_FREQ;

	status = I2C_setup(&i2cSetup);
	if(status != CSL_SOK)
	{
		printf("I2C Setup Failed!!\n");
		return(result);
	}

	regAddr  = 0;
	regValue = 0;
	status = CSL_testRegister (regAddr, regValue);
	if(status != CSL_SOK)
	{
		printf("Read Write Buffers Don't Match for Register:%d with Value:%d\n", regAddr, regValue);
		printf("CSL_testRegister Failed!!\n\n");
		return(result);
	}
	else
	{
		printf("Read Write Buffers Match for Register:%d with Value:%d\n\n", regAddr, regValue);
	}

	regAddr  = 65;
	regValue = 81;
	status = CSL_testRegister (regAddr, regValue);
	if(status != CSL_SOK)
	{
		printf("Read Write Buffers Don't Match for Register:%d with Value:%d\n", regAddr, regValue);
		printf("CSL_testRegister Failed\n\n");
		return(result);
	}
	else
	{
		printf("Read Write Buffers Match for Register:%d with Value:%d\n\n", regAddr, regValue);
	}

	regAddr  = 66;
	regValue = 99;
	status = CSL_testRegister (regAddr, regValue);
	if(status != CSL_SOK)
	{
		printf("Read Write Buffers Don't Match for Register:%d with Value:%d\n", regAddr, regValue);
		printf("CSL_testRegister Failed\n\n");
		return(result);
	}
	else
	{
		printf("Read Write Buffers Match for Register:%d with Value:%d\n\n", regAddr, regValue);
	}

	regAddr  = 0;
	regValue = 1;
	status = CSL_testRegister (regAddr, regValue);
	if(status != CSL_SOK)
	{
		printf("Read Write Buffers Don't Match for Register:%d with Value:%d\n", regAddr, regValue);
		printf("CSL_testRegister Failed!!\n\n");
		return(result);
	}
	else
	{
		printf("Read Write Buffers Match for Register:%d with Value:%d\n\n", regAddr, regValue);
	}

	regAddr  = 51;
	regValue = 0x28;
	status = CSL_testRegister (regAddr, regValue);
	if(status != CSL_SOK)
	{
		printf("Read Write Buffers Don't Match for Register:%d with Value:%d\n", regAddr, regValue);
		printf("CSL_testRegister Failed!!\n\n");
		return(result);
	}
	else
	{
		printf("Read Write Buffers Match for Register:%d with Value:%d\n\n", regAddr, regValue);
	}

	regAddr  = 52;
	regValue = 0x3C;
	status = CSL_testRegister (regAddr, regValue);
	if(status != CSL_SOK)
	{
		printf("Read Write Buffers Don't Match for Register:%d with Value:%d\n", regAddr, regValue);
		printf("CSL_testRegister Failed!!\n\n");
		return(result);
	}
	else
	{
		printf("Read Write Buffers Match for Register:%d with Value:%d\n\n", regAddr, regValue);
	}

	result = CSL_I2C_TEST_PASSED;
	return(result);
}


/**
 *  \brief  Tests i2c_write and i2c_read APIs
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status CSL_testRegister(int regAddr, int regValue)
{
	CSL_Status  status;
	Uint16      startStop;
	CSL_Status  result;
	Uint16      testWrBuffer[2];
	Uint16      testRdBuffer[2];
	volatile Uint16    looper;

	result    = CSL_I2C_TEST_FAILED;
	startStop = ((CSL_I2C_START) | (CSL_I2C_STOP));

	testWrBuffer[0] = regAddr;
	testWrBuffer[1] = regValue;

	/* Write data */
	status = I2C_write(testWrBuffer, 2,
                       CSL_I2C_CODEC_ADDR, TRUE, startStop,
                       CSL_I2C_MAX_TIMEOUT);
	if(status != CSL_SOK)
	{
		printf("I2C Write Failed!!\n");
		return(result);
	}

	printf("I2C Write Complete\n");

	/* Give some delay */
	for(looper = 0; looper < CSL_I2C_MAX_TIMEOUT; looper++){;}

	testRdBuffer[0] = 0xFF;
	/* Read data */
	status = I2C_read(testRdBuffer, 1, CSL_I2C_CODEC_ADDR,
	                  (Uint16 *)&regAddr, 1, TRUE,
	                  startStop, CSL_I2C_MAX_TIMEOUT, FALSE);
	if(status != CSL_SOK)
	{
		printf("I2C Read Failed!!\n");
		return(result);
	}
	printf("I2C Read Complete\n");

	if (testRdBuffer[0] == regValue)
	{
		result = CSL_I2C_TEST_PASSED;
	}

	return (result);
}
