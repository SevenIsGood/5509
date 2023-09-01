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

 /** @file csl_I2S0_I2S2_Mono_example.c
 *
 *  @brief Test code to verify the CSL I2S functionality in mono mode
 *
 *
 * \page    page6  I2S EXAMPLE DOCUMENTATION
 *
 * \section I2S7   I2S EXAMPLE7 - MONO MODE TEST
 *
 * \subsection I2S7x    TEST DESCRIPTION:
 * Tx: I2S0                Rx: I2S2
 * STEP1: Enable clock, get reg space pointer handlers, and set EBSR for the corresp. I2S channels
 * STEP2: Setup channel configuration: mono, fs and clk pol, datadelay: 1bit, packing,
 *        sign extension, fs/clk div: 16/32, dsp data format, master for Tx/slave for Rx
 * STEP3: Write into the Tx channel I2S0
 * STEP4: Read from the Rx channel I2S2
 * STEP5: Verify the Buffer data matching to ensure data integrity
 * STEP6: Close the channel handlers
 *
 * HPI_ON in switch DIP Switch bank SW4 is shifted right, away from the DOT, towards ON.
 * Connect pins on J14
 * 1.       I2S0_CLK(pin#12) to I2S2_CLK(pin#5)
 * 2.       I2S0_FS(pin#8)   to I2S2_FS(pin#9)
 * 3.       I2S0_RX(pin#21)  to I2S2_DX(pin#19)
 * 4.       I2S0_DX(pin#22)  to I2S2_RX(pin#14)
 *
 * On C5515,
 * Connect pins on J13
 * 1.       I2S0_CLK(pin#12) to I2S2_CLK(pin#3)
 * 2.       I2S0_FS(pin#8)   to I2S2_FS(pin#7)
 * 3.       I2S0_RX(pin#21)  to I2S2_DX(pin#11)
 * 4.       I2S0_DX(pin#22)  to I2S2_RX(pin#13)
 *
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515
 * AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection I2S7y    TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5515 EVM or C5517 EVM)
 *  @li Open the project "CSL_I2S_INTCExample_Out.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the test result
 *
 * \subsection I2S7z    TEST RESULT:
 *  @li All the CSL APIs should return success
 *  @li Read and write data buffer comparison should be successful.
 *
 * ============================================================================
 */

#include "csl_i2s.h"
#include "csl_intc.h"
#include <csl_general.h>
#include <stdio.h>

#define CSL_TEST_FAILED         (1)
#define CSL_TEST_PASSED         (0)
#define	CSL_I2S_BUF_LEN			(2)//STEREO:(4);MONO:(2)
#define	CSL_I2S_DATA_BUF_LEN	(64)

//#define I2S_DEBUG 0

CSL_IrqDataObj  CSL_IrqData;
Uint16 i2sWriteBuff[CSL_I2S_DATA_BUF_LEN];
Uint16 i2sReadBuff[CSL_I2S_DATA_BUF_LEN];
Uint16 *i2sWritePtr = NULL , *i2sReadPtr = NULL;
CSL_I2sHandle i2sRxHandle , i2sTxHandle;
CSL_I2sHandle i2sHandle0, i2sHandle2, i2sHandle3 ;
volatile Uint16 writeCompete = 0;
volatile Uint16 readComplete = 0;
// Holds the IRQ event ids for Rx and Tx interrupts
Uint16 rxEventId = 0xFFFF, txEventId = 0xFFFF;

CSL_Status ioExpander_Setup(void);
CSL_Status ioExpander_Read(Uint16 port, Uint16 pin, Uint16 *rValue);
CSL_Status ioExpander_Write(Uint16 port, Uint16 pin, Uint16 value);

extern void VECSTART(void);

/*
	This is ISR for the data transmission.
	This function returns:
	Void               -void
*/
interrupt void i2s_rxIsr()
{
	Int16 			result;

	IRQ_disable(rxEventId);

	for(result=0;result<200;result++);
	/** Read data from register and buffered					*/
	/* CSL_I2S_BUF_LEN - STEREO:(4);MONO:(2) */
	result = I2S_read(i2sRxHandle, i2sReadPtr, CSL_I2S_BUF_LEN);
	if(result == CSL_SOK)
	{
		//printf ("I2S Read Successful\n");
		readComplete = 1;
	}
	//rxisr_count++;
}
/*
	This is ISR for the data Receive.
	This function returns:
	Void               -void
*/
interrupt void i2s_txIsr()
{
	Int16 			result;

	IRQ_disable(txEventId);

	/** Write the buffer data to the registers					*/
	/* CSL_I2S_BUF_LEN - STEREO:(4);MONO:(2) */
	result = I2S_write(i2sTxHandle, i2sWritePtr, CSL_I2S_BUF_LEN);
	if(result == CSL_SOK)
	{
		writeCompete = 1;
	}
}


/*
  This function is will write and read the data to/from I2S modules and
  compare the data

	Function returns:
	CSL_TEST_FAILED                -Failure
	CSL_TEST_PASSED                -Success

 */
CSL_Status i2s_write_read_buf (CSL_I2sHandle hI2sWrite,
                    CSL_I2sHandle hI2sRead,
					Uint16 *writeBuff, Uint16 *readBuff, Uint16 buffLen)
{
    Uint16 result,bufLen = buffLen;

    if (readBuff == NULL || writeBuff == NULL || buffLen < 4 )
	{
			return (CSL_TEST_FAILED);
	}
    /* Assign write/read buffer to global write/read buffer */
    i2sWritePtr = writeBuff;
	i2sReadPtr = readBuff;
	// Assign Tx, Rx handles to globals
    i2sTxHandle = hI2sWrite;
	i2sRxHandle = hI2sRead;
    /* Enable the I2S Master */
   	result = I2S_transEnable(hI2sWrite, TRUE);
	if(result != CSL_SOK)
    {
	    return (CSL_TEST_FAILED);
    }
    /* Enable the I2S Slave */
   	result = I2S_transEnable(hI2sRead, TRUE);
	if(result != CSL_SOK)
    {
	    return (CSL_TEST_FAILED);
    }

	while(bufLen > 0)
	{
    	/* Enabling Tx Interrupt */
	    IRQ_enable(txEventId);
	    //IRQ_globalEnable();
	    // write the data in isr
		while(writeCompete != 1);
		// Ready for next write
        writeCompete = 0;
		// Adjust the write buffer accordingly
		i2sWritePtr += CSL_I2S_BUF_LEN;
	    /* Enabling Rx Interrupt */
		IRQ_enable(rxEventId);
        // read the data in isr
        while(readComplete != 1);
		readComplete = 0;
		// Adjust the write buffer accordingly
		i2sReadPtr += CSL_I2S_BUF_LEN;
		// Decrement the buffer len by 4
        bufLen -= CSL_I2S_BUF_LEN ;
	}
	IRQ_globalDisable();
   	result = I2S_transEnable(hI2sWrite, FALSE);
	if(result != CSL_SOK)
    {
	    return (CSL_TEST_FAILED);
    }
   	result = I2S_transEnable(hI2sRead, FALSE);
	if(result != CSL_SOK)
    {
	    return (CSL_TEST_FAILED);
    }
    return (CSL_TEST_PASSED);
}



Int16 i2s_tc_22(void)
{
	Uint16 			looper,status = CSL_TEST_FAILED;
	Int16 			result;
	I2S_Config		hwConfig;
	CSL_IRQ_Config 	config;

#if (defined(CHIP_C5517))
	ioExpander_Setup();
    /* Test Requires SW4 HPI_ON to be OFF */
    /* J14 pins: I2S0@ 12,8,22,21; I2S2@ 5,9,19,14 (Clk,FS,DX,RX)*/

    /* Set SEL_MMC0_I2S high for routing I2S0 to J14*/
    ioExpander_Write(1, 0 , 1);// SEL_MMC0_I2S = 1

    /* SPI_I2S2_S1=0, SPI_I2S2_S0=1 for routing I2S2 to J14 */
    ioExpander_Write(0, 5 , 1);
    ioExpander_Write(0, 6 , 0);
#endif
    IRQ_globalDisable();
	/* Open the device with instance 0 	*/
	i2sHandle0 = I2S_open(I2S_INSTANCE0, I2S_INTERRUPT, I2S_CHAN_MONO);
	if(NULL == i2sHandle0)
	{
		status = CSL_TEST_FAILED;
		return (status);
	}
	else
	{
		printf ("I2S Module0 Instance Open Successful\n");
	}

	/* Pin Muxing for Serial Port0 Pins--Mode1--I2S0 and GPIO[5:4]*/
		CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP0MODE, MODE1);


	hwConfig.dataType 			= I2S_MONO_ENABLE;
	hwConfig.loopBackMode 		= I2S_LOOPBACK_DISABLE;
	hwConfig.fsPol 				= I2S_FSPOL_HIGH;
	hwConfig.clkPol				= I2S_RISING_EDGE;
	hwConfig.datadelay			= I2S_DATADELAY_ONEBIT;
	hwConfig.datapack			= I2S_DATAPACK_DISABLE;
	hwConfig.signext			= I2S_SIGNEXT_DISABLE;
	hwConfig.fsDiv				= I2S_FSDIV16;
	hwConfig.wordLen			= I2S_WORDLEN_10;
	hwConfig.i2sMode			= I2S_MASTER;
    hwConfig.dataFormat         = I2S_DATAFORMAT_DSP;
	hwConfig.clkDiv				= I2S_CLKDIV32;
	hwConfig.FError				= I2S_FSERROR_ENABLE;
	hwConfig.OuError			= I2S_OUERROR_ENABLE;


	/* Configure hardware registers	 	*/
	result = I2S_setup(i2sHandle0, &hwConfig);

	if(result != CSL_SOK)
	{
		status = CSL_TEST_FAILED;
		return (status);
	}
	else
	{
		printf ("I2S Module0 Configuration Successful\n");
	}

	/* Open the device with instance 2	*/
	i2sHandle2 = I2S_open(I2S_INSTANCE2, I2S_INTERRUPT, I2S_CHAN_MONO);
	if(NULL == i2sHandle2)
	{
		status = CSL_TEST_FAILED;
		return (status);
	}
	else
	{
		printf ("I2S Module2 Instance Open Successful\n");
	}

	/* I2S2 should be available with default PPMODE 001 */
		CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_PPMODE, MODE1);

	/* Set the value for the configure structure 	*/
	hwConfig.i2sMode			= I2S_SLAVE;

#if I2S_DEBUG
	hwConfig.i2sMode			= I2S_MASTER;
#endif
	/* Configure hardware registers	 	*/
	result = I2S_setup(i2sHandle2, &hwConfig);
	if(result != CSL_SOK)
	{
		status = CSL_TEST_FAILED;
		return (status);
	}
	else
	{
		printf ("I2S Module2 Configuration Successful\n");
	}

	/* Clear any pending interrupts */
	IRQ_clearAll();

	/* Disable all the peripheral interrupts */
	IRQ_disableAll();

	IRQ_setVecs((Uint32)(&VECSTART));
	/* Configuring I2S0 Tx Interrupt */
	txEventId = PROG0_EVENT ;

#if I2S_DEBUG
	txEventId = XMT3_EVENT ;
#endif

    config.funcAddr = &i2s_txIsr;
	IRQ_plug (txEventId, config.funcAddr);
	/* Configuring I2S2 Rx Interrupt */
	rxEventId = RCV2_EVENT ;
	config.funcAddr = &i2s_rxIsr;
	IRQ_plug (rxEventId, config.funcAddr);

	/* Initialize the read and write buffers for 10 bit word length	*/
	for(looper=0; looper < CSL_I2S_DATA_BUF_LEN; looper++)
	{
		if(looper%2)
		i2sWriteBuff[looper] = (looper<<6);
		i2sReadBuff[looper]  = 0x0;
	}

	IRQ_globalEnable();

#if I2S_DEBUG
    result = i2s_write_read_buf(i2sHandle3,i2sHandle3,i2sWriteBuff,
                                i2sReadBuff,64);
#else
    result = i2s_write_read_buf(i2sHandle0,i2sHandle2,i2sWriteBuff,
                                i2sReadBuff,CSL_I2S_DATA_BUF_LEN);
#endif
	if(result != CSL_SOK)
	{
		return (CSL_TEST_FAILED);
	}

	IRQ_globalDisable();
	/* Reset the registers */
	result = I2S_reset(i2sHandle0);
	if(result != CSL_SOK)
	{
		return (CSL_TEST_FAILED);
	}
	/* Reset the registers */
	result = I2S_reset(i2sHandle2);
	if(result != CSL_SOK)
	{
		return (CSL_TEST_FAILED);
	}
	/* Close the operation */
	result = I2S_close(i2sHandle0);
	if(result != CSL_SOK)
	{
		status = CSL_TEST_FAILED;
		return (status);
	}
	/* Close the operation */
	result = I2S_close(i2sHandle2);
	if(result != CSL_SOK)
	{
		status = CSL_TEST_FAILED;
		return (status);
	}

	/* Clear any pending interrupts */
	IRQ_clearAll();

	/* Disable all the peripheral interrupts */
	IRQ_disableAll();
	/* Compare the read and write buffer						*/
	for(looper=0; looper < CSL_I2S_DATA_BUF_LEN; looper++)
	{
		printf("I2S Read[%d]: 0x%04x & Write[%d]: 0x%04x\n",looper,
				i2sReadBuff[looper],looper,i2sWriteBuff[looper]);
	}

	for(looper=0; looper < CSL_I2S_DATA_BUF_LEN; looper+=2)
	{
		if((i2sWriteBuff[looper+1]&0xFFC0) != (i2sReadBuff[looper]&0xFFC0))
		{
			printf("I2S Read & Write Buffers doesn't Match!!! ");

			status = CSL_TEST_FAILED;
			return(status);
		}
	}
	if(looper == CSL_I2S_DATA_BUF_LEN)
	{
		printf("I2S Read & Write Buffers Match!!!\n");
	}
	return (CSL_TEST_PASSED);
}

/////INSTRUMENTATION FOR BATCH TESTING -- Part 1 --
/////  Define PaSs_StAtE variable for catching errors as program executes.
/////  Define PaSs flag for holding final pass/fail result at program completion.
volatile Int16 PaSs_StAtE = 0x0001; // Init to 1. Reset to 0 at any monitored execution error.
volatile Int16 PaSs = 0x0000; // Init to 0.  Updated later with PaSs_StAtE when and if
/////                                  program flow reaches expected exit point(s).
/////


/*
 This is the main function to call sample program
*/
void main(void)
{
	Int16    status;

	status = i2s_tc_22();
	if(status != CSL_TEST_PASSED)
	{
		printf("\nI2S0_I2S2_32Clk_16FS_1Dly_Mono FAILED!!\n\n");
		   	PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.assigned 0 when any error
	}
	else
	{
		printf("\nI2S0_I2S2_32Clk_16FS_1Dly_Mono PASSED!!\n\n");
	}


   /////INSTRUMENTATION FOR BATCH TESTING -- Part 3 --
   /////  At program exit, copy "PaSs_StAtE" into "PaSs".
   PaSs = PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with
   /////                   // pass/fail value determined during program execution.
   /////  Note:  Program should next exit to C$$EXIT and halt, where DSS, under
   /////   control of a host PC script, will read and record the PaSs' value.
   /////

}
