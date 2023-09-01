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

/** @file csl_I2C_PollMode_DiffI2cClks_example.c
 *
 *  @brief I2C functional layer interrupt mode example source file
 *
 *
 * \page    page5  I2C EXAMPLE DOCUMENTATION
 *
 * \section I2C7   I2C EXAMPLE7 - POLL MODE Different i2c clock TEST
 * \subsection I2C    TEST DESCRIPTION:
 *  This test verifies the operation of CSL I2C module in Poll mode.i
 *  for different i2c clock frequency test.
 * STEP 1: Configure I2C in poll mode
 * STEP 2: Write 64 bytes of data through I2C
 * STEP 3: Read 64 bytes of data through I2C
 * STEP 4: Verify the buffer match
 * Repeat the above steps for different I2C clock frequencies
 *
 * ============================================================================
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515 AND
 * C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection I2C7y   TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5515 EVM or C5517 EVM)
 *  @li Open the project "CSL_I2C_IntcExample_Out.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the test result
 *
 * \subsection I2C7z   TEST RESULT:
 *  @li All the CSL APIs should return success
 *  @li Read and write buffer comparison should be successful.
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


#include <csl_i2c.h>
#include <csl_pll.h>
#include <stdio.h>
#include <string.h>
#include <csl_general.h>

#define CSL_I2C_TEST_PASSED      (0)
#define CSL_I2C_TEST_FAILED      (1)

#define CSL_I2C_DATA_SIZE        (64)
#define CSL_EEPROM_ADDR_SIZE     (2)
#define CSL_I2C_OWN_ADDR         (0x2F)
#define CSL_I2C_SYS_CLK          (100)
#define CSL_I2C_BUS_FREQ         (100)
#define CSL_I2C_EEPROM_ADDR		 (0x50)
#define CSL_I2C_CODEC_ADDR		 (0x18)

Uint16  gI2cWrBuf[CSL_I2C_DATA_SIZE + CSL_EEPROM_ADDR_SIZE];
Uint16  gI2cRdBuf[CSL_I2C_DATA_SIZE];

CSL_I2cSetup     i2cSetup;
CSL_I2cConfig    i2cConfig;

Uint32 GetSysClk(void);

/**
 *  \brief  Tests I2C polled mode operation
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status CSL_i2cPolledTest(Uint16 csl_i2c_bus_freq);

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
	Uint16 csl_i2c_bus_freq;

	printf("I2C POLLED MODE TEST!\n\n");

	for(csl_i2c_bus_freq = CSL_I2C_BUS_FREQ;
			csl_i2c_bus_freq < CSL_I2C_BUS_FREQ*5;
			csl_i2c_bus_freq += CSL_I2C_BUS_FREQ)
	{
		result =  CSL_i2cPolledTest(csl_i2c_bus_freq);
		memset(&gI2cWrBuf, 0x0, sizeof(gI2cWrBuf));
		memset(&gI2cRdBuf, 0x0, sizeof(gI2cRdBuf));


		if(result == CSL_I2C_TEST_PASSED)
		{
			printf("I2C POLLED MODE TEST PASSED for %dKbps Data txfer rate!!\n\n", csl_i2c_bus_freq);
		}
		else
		{
			printf("I2C POLLED MODE TEST FAILED for %dKbps Data txfer rate!!\n\n", csl_i2c_bus_freq);
			PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
		}



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
 *  \brief  Tests I2C polled mode operation
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status    CSL_i2cPolledTest(Uint16 csl_i2c_bus_freq)
{
	CSL_Status         status;
	CSL_Status         result;
	Uint16             startStop;
	volatile Uint16    looper;
	volatile Uint32    csl_i2c_sys_clk;

	result = CSL_I2C_TEST_FAILED;

	/* Assign the EEPROM page address */
	gI2cWrBuf[0] = 0x0;
	gI2cWrBuf[1] = 0x0;

	for(looper = 0; looper < CSL_I2C_DATA_SIZE; looper++)
	{
		gI2cWrBuf[looper + CSL_EEPROM_ADDR_SIZE] = looper;
		gI2cRdBuf[looper] = 0x0000;
	}

	/* Initialize I2C module */
	status = I2C_init(CSL_I2C0);
	if(status != CSL_SOK)
	{
		printf("I2C Init Failed!!\n");
		return(result);
	}

	csl_i2c_sys_clk = GetSysClk();
	/* Setup I2C module */
	i2cSetup.addrMode    = CSL_I2C_ADDR_7BIT;
	i2cSetup.bitCount    = CSL_I2C_BC_8BITS;
	i2cSetup.loopBack    = CSL_I2C_LOOPBACK_DISABLE;
	i2cSetup.freeMode    = CSL_I2C_FREEMODE_DISABLE;
	i2cSetup.repeatMode  = CSL_I2C_REPEATMODE_DISABLE;
	i2cSetup.ownAddr     = CSL_I2C_OWN_ADDR;
	//sysclk to i2c, before pre-scaling
	i2cSetup.sysInputClk = csl_i2c_sys_clk/1000u;
	//actual i2c serial clk
	i2cSetup.i2cBusFreq = csl_i2c_bus_freq;
	startStop            = ((CSL_I2C_START) | (CSL_I2C_STOP));

	status = I2C_setup(&i2cSetup);
	if(status != CSL_SOK)
	{
		printf("I2C Setup Failed!!\n");
		return(result);
	}

	/* Write data */
	status = I2C_write(gI2cWrBuf, (CSL_I2C_DATA_SIZE + CSL_EEPROM_ADDR_SIZE),
                       CSL_I2C_EEPROM_ADDR, TRUE, startStop,
                       CSL_I2C_MAX_TIMEOUT);
	if(status != CSL_SOK)
	{
		printf("I2C Write Failed!!\n");
		return(result);
	}

	printf("I2C Write Complete\n");

	
	/* Write data EEPROM page address for read operation */
	status = I2C_write(gI2cWrBuf, CSL_EEPROM_ADDR_SIZE, CSL_I2C_EEPROM_ADDR,
	                   TRUE, startStop, CSL_I2C_MAX_TIMEOUT);
	if(status != CSL_SOK)
	{
		printf("I2C Write Failed!!\n");
		return(result);
	}


	/* Read data */
	status = I2C_read(gI2cRdBuf, CSL_I2C_DATA_SIZE, CSL_I2C_EEPROM_ADDR, NULL, 0,
	                   TRUE, startStop, CSL_I2C_MAX_TIMEOUT, FALSE);
	if(status != CSL_SOK)
	{
		printf("I2C Read Failed!!\n");
		return(result);
	}

	printf("I2C Read Complete\n");

	/* Compare the buffers */
	for(looper = 0; looper < CSL_I2C_DATA_SIZE; looper++)
	{
		if(gI2cWrBuf[looper + CSL_EEPROM_ADDR_SIZE] != gI2cRdBuf[looper])
		{
			printf("Read Write Buffers Does not Match!!\n");
			return(result);
		}
	}

	printf("Read and Write Buffers Match!!\n");

	result = CSL_I2C_TEST_PASSED;
	return(result);
}


#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined(CHIP_C5535) || defined(CHIP_C5545))

Uint32 GetSysClk(void)
{
	Bool      pllRDBypass;
	Bool      pllOutDiv;
	Uint32    sysClk;
	Uint16    pllM;
	Uint16    pllRD;
	Uint16    pllOD;

	pllM = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR1, SYS_CGCR1_M);

	pllRD = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR2, SYS_CGCR2_RDRATIO);
	pllOD = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR4, SYS_CGCR4_ODRATIO);

	pllRDBypass = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR2, SYS_CGCR2_RDBYPASS);
	pllOutDiv   = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR4, SYS_CGCR4_OUTDIVEN);

	sysClk = CSL_PLL_CLOCKIN;

	if (0 == pllRDBypass)
	{
		sysClk = sysClk/(pllRD + 4);
	}

	sysClk = (sysClk * (pllM + 4));

	if (1 == pllOutDiv)
	{
		sysClk = sysClk/(pllOD + 1);
	}

	/* Return the value of system clock in KHz */
	return(sysClk/1000);
}

#elif (defined(CHIP_C5517))

Uint32 GetSysClk(void)
{
	Uint32    sysClk;
	float    Multiplier;
	Uint16    OD;
	Uint16    OD2;
	Uint16    RD, RefClk;
	Uint32	  temp1, temp2, temp3, vco;

	temp2 =  PLL_CNTL2;
	temp3 =  (temp2 & 0x8000) <<1 ;
	temp1 = temp3 + PLL_CNTL1;
	Multiplier = temp1/256.0 +1;
	RD = (PLL_CNTL2 & 0x003F) ;

	RefClk = 12000/(RD+1);

	vco = Multiplier * (Uint32)RefClk;

	OD = (PLL_CNTL4 & 0x7);

	sysClk = vco/(OD+1);

	OD2 = ((PLL_CNTL4 >> 10) & 0x1F) ;

	if (PLL_CNTL3 & 0x8000)	// PLL Bypass
		sysClk = RefClk;
	else
		sysClk = vco/(OD+1);

	if ((PLL_CNTL4 & 0x0020) == 0)	/* OutDiv2 */
		sysClk = sysClk / ( 2*(OD2+1));

	/* Return the value of system clock in Hz */
	return(sysClk);
}

#else

Uint32 GetSysClk(void)
{
	Bool      pllRDBypass;
	Bool      pllOutDiv;
	Bool      pllOutDiv2;
	Uint32    sysClk;
	Uint16    pllVP;
	Uint16    pllVS;
	Uint16    pllRD;
	Uint16    pllVO;
	Uint16    pllDivider;
	Uint32    pllMultiplier;

	pllVP = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR1, SYS_CGCR1_MH);
	pllVS = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR2, SYS_CGCR2_ML);

	pllRD = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR2, SYS_CGCR2_RDRATIO);
	pllVO = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR4, SYS_CGCR4_ODRATIO);

	pllRDBypass = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR2, SYS_CGCR2_RDBYPASS);
	pllOutDiv   = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR4, SYS_CGCR4_OUTDIVEN);
	pllOutDiv2  = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR4, SYS_CGCR4_OUTDIV2BYPASS);

	pllDivider = ((pllOutDiv2) | (pllOutDiv << 1) | (pllRDBypass << 2));

	pllMultiplier = ((Uint32)CSL_PLL_CLOCKIN * ((pllVP << 2) + pllVS + 4));

	switch(pllDivider)
	{
		case CSL_PLL_DIV_000:
		case CSL_PLL_DIV_001:
			sysClk = pllMultiplier / (pllRD + 4);
		break;

		case CSL_PLL_DIV_002:
			sysClk = pllMultiplier / ((pllRD + 4) * (pllVO + 4) * 2);
		break;

		case CSL_PLL_DIV_003:
			sysClk = pllMultiplier / ((pllRD + 4) * 2);
		break;

		case CSL_PLL_DIV_004:
		case CSL_PLL_DIV_005:
			sysClk = pllMultiplier;
		break;

		case CSL_PLL_DIV_006:
			sysClk = pllMultiplier / ((pllVO + 4) * 2);
		break;

		case CSL_PLL_DIV_007:
			sysClk = pllMultiplier / 2;
		break;
	}

	/* Return the value of system clock in KHz */
	return(sysClk/1000);
}
#endif
