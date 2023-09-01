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

/*! \file i2c_dsp_slave_msp_master.c
*
*   \brief Implementation of I2C interface test between BoosterPack and
*          MSP432 launchpad with MSP configured as master and DSP configured
*          as slave
*
*/

#include "i2c_dsp_slave_msp_master.h"

Uint16 	response = 0, i;
Uint16 i2cSlaveDataBuf[I2C_SLV_DATA_SIZE];

/**
 *  \brief    Initializes DSP I2C module and reads the data from the  slave device
 *            at address I2C_SLAVE_ADDRESS
 *
 *  \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n         TEST_PASS  - Test Passed
 * \n         TEST_FAIL  - Test Failed
 */
static TEST_STATUS run_i2c_mspmst_dspslv_test(void *testArgs)
{
	Int16  retVal;

	Uint16  index, count;

	/* Initialize I2C module */
	retVal = I2C_init(CSL_I2C0);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("I2C Init Failed!!\n\r");
		return (TEST_FAIL);
	}

	/* I2C in reset state */
	CSL_FINS(CSL_I2C_0_REGS->ICMDR,I2C_ICMDR_IRS,0);

	/* I2C in Out of reset state */
	CSL_FINS(CSL_I2C_0_REGS->ICMDR,I2C_ICMDR_IRS,1);

	/* I2C Set Mode as Slave */
	CSL_FINS(CSL_I2C_0_REGS->ICMDR,I2C_ICMDR_MST,0);

	/* I2C Set Address mode as 7-bit */
	CSL_FINS(CSL_I2C_0_REGS->ICMDR,I2C_ICMDR_XA,0);

	/* I2C Set own Address */
	CSL_FINS(CSL_I2C_0_REGS->ICOAR,I2C_ICOAR_OADDR,I2C_SLAVE_ADDRESS);

	/* Enable Receive Interrupt */
	CSL_FINS(CSL_I2C_0_REGS->ICIMR,I2C_ICIMR_ICRRDY,1);

	/*Setting the Bus to Busy state*/
	CSL_FINS(CSL_I2C_0_REGS->ICSTR,I2C_ICSTR_BB,1);

	/*Setting the Data Ready bit to not Ready*/
	CSL_FINS(CSL_I2C_0_REGS->ICSTR,I2C_ICSTR_ICRRDY,0);

	C55x_msgWrite("Waiting for the MSP to transfer data...\n\n\r");

    /*Wait till the Bus is free for transfer */
    do {
      response=CSL_FEXT (CSL_I2C_0_REGS->ICSTR,I2C_ICSTR_BB);
    }while((response & 0x1)!=0x0);

    /*Receive 64 bytes/Page Data from the Master*/
    for(i=0;i<I2C_SLV_DATA_SIZE;i++) {
		do {/*Check if Receive Data Register is Ready */
		response=CSL_FEXT(CSL_I2C_0_REGS->ICSTR,I2C_ICSTR_ICRRDY);
		}while((response & 0x01)==0);
	    /*Read data from Receive Data Register */
		i2cSlaveDataBuf[i] = CSL_FEXT(CSL_I2C_0_REGS->ICDRR,I2C_ICDRR_D);
	}

    C55x_msgWrite("Data Received from Master:\n\r");
	for (index = 0; index < I2C_SLV_DATA_SIZE; )
	{
		for (count = 0; count < I2C_SLV_DATA_BLOCK_SIZE; count++)
		{
			C55x_msgWrite("0x%x\t", i2cSlaveDataBuf[index+count]);
		}
		C55x_msgWrite("\n\r");

		index += I2C_SLV_DATA_BLOCK_SIZE;

		C55x_delay_msec(800);
	}

	/* Compare the data received */
	/* For ease of verification it is assumed that master sends 16 bytes of data
	 * from 0 to 15. Same data should be sent by master test implementation for
	 * successful execution of the test */
	for (index = 0; index < I2C_SLV_DATA_SIZE; index++)
	{
		if(i2cSlaveDataBuf[index] != index)
		{
			C55x_msgWrite("\n\rData Received from Master Mismatched with Expected Data at Offset %d\n\r", index);
			return (TEST_FAIL);
		}
	}

	C55x_msgWrite("\n\rData Received from Master Matched with Expected Data!\n\r");

	/* Reset the I2C Module */
	CSL_FINS(CSL_I2C_0_REGS->ICMDR,I2C_ICMDR_IRS,0);

	return (TEST_PASS);

}

/**
 * \brief This function performs i2c communication from msp to dsp test
 *
 * \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
 TEST_STATUS dspSlaveMspMasterTest(void *testArgs)
 {
     Int16 testStatus;

     C55x_msgWrite("\n*******************************************************\n\r");
     C55x_msgWrite(  "         'DSP Slave - MSP Master' I2C Test       \n\r");
     C55x_msgWrite(  "*******************************************************\n\r");

     testStatus = run_i2c_mspmst_dspslv_test(testArgs);
     if(testStatus != TEST_PASS)
     {
     	C55x_msgWrite("\n'MSP Master - DSP Slave' I2C Test Failed!\n\r");
     }
     else
     {
     	C55x_msgWrite("\n'MSP Master- DSP Slave ' I2C Test Passed!!\n\r");
     }

     C55x_msgWrite("\n'MSP Master - DSP Slave ' I2C Test Completed!!\n\r");

     return testStatus;

 }
