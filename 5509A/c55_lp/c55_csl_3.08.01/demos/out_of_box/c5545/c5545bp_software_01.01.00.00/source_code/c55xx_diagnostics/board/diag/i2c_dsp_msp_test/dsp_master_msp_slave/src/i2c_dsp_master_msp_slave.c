/*
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*! \file i2c_dsp_master_msp_slave.c
*
*   \brief Implementation of I2C interface test between BoosterPack and
*          MSP432 launchpad with MSP configured as slave and DSP configured
*          as master
*
*/

#include "i2c_dsp_master_msp_slave.h"

Uint16 i2cMstDataBuf[I2C_MST_DATA_SIZE];

/**
 *  \brief    Initializes DSP I2C module and sends the data to slave device
 *            at address I2C_MST_SLAVE_ADDRESS
 *
 *  \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n         TEST_PASS  - Test Passed
 * \n         TEST_FAIL  - Test Failed
 */
static TEST_STATUS run_i2c_dspmst_mspslv_test(void *testArgs)
{
	Uint16 index, count;
	CSL_Status   retVal;
	Uint16 startStopFlag = ((CSL_I2C_START) | (CSL_I2C_STOP));
    CSL_I2cSetup     i2cSetup;

    retVal = I2C_init(CSL_I2C0);
	if (retVal != CSL_SOK)
	{
		C55x_msgWrite("I2C Initialization Failed\n\r");
		return (TEST_FAIL);
	}

	i2cSetup.addrMode    = CSL_I2C_ADDR_7BIT;
	i2cSetup.bitCount    = CSL_I2C_BC_8BITS;
	i2cSetup.loopBack    = CSL_I2C_LOOPBACK_DISABLE;
	i2cSetup.freeMode    = CSL_I2C_FREEMODE_DISABLE;
	i2cSetup.repeatMode  = CSL_I2C_REPEATMODE_DISABLE;
	i2cSetup.ownAddr     = CSL_I2C_ICOAR_DEFVAL;
	i2cSetup.sysInputClk = I2C_MST_SYS_CLK;
	i2cSetup.i2cBusFreq  = I2C_MST_BUS_FREQ;

	retVal = I2C_setup(&i2cSetup);
	if (retVal != CSL_SOK)
	{
		C55x_msgWrite("I2C Configuration Failed\n\r");
		return (TEST_FAIL);
	}

	for (index = 0; index < I2C_MST_DATA_SIZE; index++)
	{
		i2cMstDataBuf[index] = index;
	}

	C55x_msgWrite("\n\rSending %d Bytes to Slave in blocks of %d\n\r",
	              I2C_MST_DATA_SIZE, I2C_MST_DATA_BLOCK_SIZE);

	C55x_msgWrite("Data Sent to Slave:\n\r");

	for (index = 0; index < I2C_MST_DATA_SIZE; )
	{
		retVal = I2C_write(&i2cMstDataBuf[index], I2C_MST_DATA_BLOCK_SIZE,
						   I2C_MST_SLAVE_ADDRESS, TRUE, startStopFlag,
						   CSL_I2C_MAX_TIMEOUT);
		if (retVal != CSL_SOK)
		{
			C55x_msgWrite("I2C Write Failed\n\r");
			return (TEST_FAIL);
		}

		for (count = 0; count < I2C_MST_DATA_BLOCK_SIZE; count++)
		{
			C55x_msgWrite("0x%x\t", i2cMstDataBuf[index+count]);
		}
		C55x_msgWrite("\n\r");

		index += I2C_MST_DATA_BLOCK_SIZE;
		C55x_delay_msec(2000);
	}

    return (TEST_PASS);
}

/**
 * \brief This function performs i2c communication from dsp to msp test
 *
 * \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
TEST_STATUS i2cDspMastermspSlaveTest(void *testArgs)
{
    Int16 retVal;

	C55x_msgWrite("\n*************************************************\n\r");
	C55x_msgWrite(  "        'DSP Master - MSP Slave' I2C Test        \n\r");
	C55x_msgWrite(  "*************************************************\n\r");

	retVal = run_i2c_dspmst_mspslv_test(testArgs);
	if(retVal != TEST_PASS)
    {
	    C55x_msgWrite("\n'MSP Slave - DSP Master' I2C Test Failed!\n\r");
	    return (TEST_FAIL);
	}
	else
	{
		//C55x_msgWrite("\n'MSP Slave - DSP Master' I2C Test Passed!\n\r");
	}

	C55x_msgWrite("\n'MSP Slave - DSP Master' I2C Test Completed!!\n\r");

    return (TEST_PASS);

}
