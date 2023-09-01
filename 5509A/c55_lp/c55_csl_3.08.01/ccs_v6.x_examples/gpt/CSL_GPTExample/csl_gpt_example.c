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


/** @file csl_gpt_example.c
 *
 *  @brief Example test code to verify the CSL GPT module functionality
 *
 * \page    page4  GPT EXAMPLE DOCUMENTATION
 *
 * \section GPT1   GPT EXAMPLE1 - GPT INTERRUPT TEST
 *
 * \subsection GPT1x    TEST DESCRIPTION:
 *		This test code verifies the functionality of the CSL GPT (General
 * Purpose Timer) module. GPT module of C5535/C5515/C5517 DSP is used to provide
 * interrupts to the CPU to schedule periodic tasks or a delayed task.
 * GPT can be configured with a counter and pre-scaler divider value. When the
 * GPT is started counter will be decremented to 0. The rate at which this
 * count happens can be controlled by the prescaler divider value. GPT clock
 * is derived by dividing the CPU clock with the pre-scaler divider value.
 *
 * This test is having two parts. First part of the test verifies whether the
 * GTP is decrementing the counter or not. GPT is configured with a count value
 * 0xFFFF and pre-scaler divider value 256. GPT is started and counter value is
 * read. After few cycles of delay GPT counter value is read again. Both the
 * counter values are compared to verify whether the second count value is less
 * than the first counter value or not.
 *
 * During second part of the test timer count rate of the GPT is verified.
 * As the GPT clock is derived from the CPU clock based on the pre-scaler
 * value it possible to verify the GPT rate with respect to CPU clock rate.
 * GPT pre-scaler value is set to divide by 4. So the counter runs at a speed
 * that is 1/4th of the CPU clock. Clock at which the CPU is running during the
 * test is calculated using the function getSysClk(). This function returns
 * the value of system clock in KHz. Value of the system clock in KHz
 * represents the CPU clock cycles for one millisecond. GPT count value is set
 * to 1/4th of the CPU clock cycles for one millisecond. With this setup GPT
 * should take one millisecond to count down the counter value to 0. In other
 * way clock cycles executed by the CPU from the time GPT is started to the
 * time counter value reaches 0 should be equal to the number of cycles that
 * are executed by the CPU in one millisecond.GPT is configured to generate
 * interrupt when the count value reaches to 0. GPT is started and the CPU
 * clock cycles are counted in while loop. This loop increments a global
 * variable 'cpuCycleCount' until the GPT generate time-out interrupt.
 * This while loop execution is taking 12 CPU cycles. So the total CPU cycles
 * executed will be 12*cpuCycleCount. At the end of the test this CPU cycles
 * calculated are compared with the CPU clock cycles that will be executed in
 * one millisecond. The calculated CPU cycles can vary �1% from the actual
 * CPU cycles. If this condition is satisfied GPT is running at the configured
 * rate.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
 * AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection GPT1y    TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5535/C5515 EVM or C5517 EVM))
 *  @li Open the project "CSL_GPTExampale_Out.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the test result
 *  @li Repeat the test at the following PLL frequencies
 *      C5515: 60MHz and 100MHz
 *      C5517: 60MHz, 100MHz, 150MHz and 200MHz
 *      C5535 eZdsp: 60MHz and 100MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection GPT1z    TEST RESULT:
 *  @li All the CSL APIs should return success
 *  @li Timer count value comparison should be successful in the first part of
 *      the test
 *  @li Timer should run at the configured rate in the second part of the test
 *
 *
 */

/* ============================================================================
 * Revision History
 * ================
 * 05-Oct-2008 Created
 * 10-Jul-2012 Added C5517 Compatibility
 * 09-Mar-2016 Update the header
 * ============================================================================
 */

#include <stdio.h>
#include "csl_gpt.h"
#include "csl_intc.h"
#include <csl_general.h>

#define CSL_TEST_FAILED    (1u)
#define CSL_TEST_PASSED    (0)

#define CSL_PLL_DIV_000    (0)
#define CSL_PLL_DIV_001    (1u)
#define CSL_PLL_DIV_002    (2u)
#define CSL_PLL_DIV_003    (3u)
#define CSL_PLL_DIV_004    (4u)
#define CSL_PLL_DIV_005    (5u)
#define CSL_PLL_DIV_006    (6u)
#define CSL_PLL_DIV_007    (7u)

#define CSL_PLL_CLOCKIN    (32768u)

#define PLL_CNTL1        *(ioport volatile unsigned *)0x1C20
#define PLL_CNTL2        *(ioport volatile unsigned *)0x1C21
#define PLL_CNTL3        *(ioport volatile unsigned *)0x1C22
#define PLL_CNTL4        *(ioport volatile unsigned *)0x1C23

extern void VECSTART(void);
Uint32    cpuCycleCount = 0;
Uint32    sysClk;
volatile Uint16    hitIsr = 0;

/**
 *  \brief  GPT Count Read Test function
 *
 * This is function verifies reading the counter values from the GPT
 *
 *  \param  none
 *
 *  \return CSL_TEST_PASSED  - Success
 *          CSL_TEST_FAILED  - Failure
 */
Int16 CSL_gptCountReadTest(void);

/**
 *  \brief  GPT Count Rate Verification test function
 *
 * This function verifies whether timer is running at the configured
 * rate or not. This function configures and enables the GPT interrupts
 * to indicate the expiry of the timer count. Test counts number of
 * cycles executed by the CPU after starting the timer till the expiry
 * of the timer. These cycles are used to verify the WDT count rate.
 * It is assumed that calculated CPU cycles will be with in the range
 * �1% actual CPU clock cycles. Test will be successful if the calculated
 * CPU cycles fall with in this range.
 *
 * NOTE: Changing the PLL setting in the middle (After getSysClk() call)
 *       of the test will result in test failure.
 *
 *  \param  none
 *
 *  \return CSL_TEST_PASSED  - Success
 *          CSL_TEST_FAILED  - Failure
 */
Int16 CSL_gptIntrTest(void);

/**
 *  \brief  Function to calculate the system clock
 *
 *  \param    none
 *
 *  \return   System clock value in KHz
 */
Uint32 getSysClk(void);

/**
 *  \brief  GPT Interrupt Service Routine
 *
 *  \param  none
 *
 *  \return none
 */
interrupt void gpt0Isr(void);

/**
 *  \brief  main function
 *
 *  This function calls the GPT test function and displays the
 *  test result.
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
	Int16	result;

	printf("CSL GPT TESTS!\n\n");

	printf("TIMER COUNT READ TEST!\n");
	result = CSL_gptCountReadTest();
	if(CSL_TEST_FAILED == result)
	{
		printf("TIMER COUNT READ TEST FAILED!!\n");
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
   /////  Reseting PaSs_StAtE to 0 if error detected here.
        PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
   /////
	}
	else
	{
		printf("TIMER COUNT READ TEST PASSED!!\n");
	}

	/* Test to verify the Timer count Rate */
	printf("\n\nTIMER COUNT RATE VERIFICATION TEST!\n");
	printf("THIS TEST VERIFIES WHETHER THE TIMER IS RUNNING AT CONFIGURED RATE OR NOT");
	result = CSL_gptIntrTest();
	if(CSL_TEST_FAILED == result)
	{
		printf("TIMER COUNT RATE VERIFICATION TEST FAILED!!\n");
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
   /////  Reseting PaSs_StAtE to 0 if error detected here.
        PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
   /////
	}
	else
	{
		printf("TIMER COUNT RATE VERIFICATION TEST PASSED!!\n");
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
 *  \brief  GPT Count Read Test function
 *
 * This is function verifies reading the counter values from the GPT
 *
 *  \param  none
 *
 *  \return CSL_TEST_PASSED  - Success
 *          CSL_TEST_FAILED  - Failure
 */
Int16 CSL_gptCountReadTest(void)
{
	CSL_GptObj		   gptObj;
	CSL_Handle	       hGpt;
	CSL_Status 	       status;
	CSL_Config 	       hwConfig;
	Uint32 		       timeCnt1;
	Uint32 		       timeCnt2;
	volatile Uint16	   delay;

	hitIsr = FALSE;
	status = 0;

	/* Open the CSL GPT module */
	hGpt = GPT_open (GPT_0, &gptObj, &status);
	if((NULL == hGpt) || (CSL_SOK != status))
	{
		printf("GPT Open Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT Open Successful\n");
	}

	/* Reset the GPT module */
	status = GPT_reset(hGpt);
	if(CSL_SOK != status)
	{
		printf("GPT Reset Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT Reset Successful\n");
	}

	/* Configure GPT module */
	hwConfig.autoLoad 	 = GPT_AUTO_ENABLE;
	hwConfig.ctrlTim 	 = GPT_TIMER_ENABLE;
	hwConfig.preScaleDiv = GPT_PRE_SC_DIV_7;
	hwConfig.prdLow 	 = 0xFFFF;
	hwConfig.prdHigh 	 = 0x0000;

	status =  GPT_config(hGpt, &hwConfig);
	if(CSL_SOK != status)
	{
		printf("GPT Config Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT Config Successful\n");
	}

	/* Start the Timer */
	status = GPT_start(hGpt);
	if(CSL_SOK != status)
	{
		printf("GPT Start Failed\n");
		return (CSL_TEST_FAILED);
	}

	/* Read the Timer Count */
	status = GPT_getCnt(hGpt, &timeCnt1);
	if(CSL_SOK != status)
	{
		printf("GPT Count-1 Failed \n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT Count-1 Successful\n");
	}

	/* Give Some Delay */
	for(delay = 0; delay < 100; delay++);

	/* Read the Timer Count Again */
	status = GPT_getCnt(hGpt, &timeCnt2);
	if(CSL_SOK != status)
	{
		printf("GPT Count-2 Failed \n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT Count-2 Success \n");
	}

	/* Compare the timer count to verify whether the timer is counting or not */
	if(timeCnt2 >= timeCnt1)
	{
		printf("GTP Time Count Compare Failed\n");
		return(CSL_TEST_FAILED);
	}
	else
	{
		printf("GTP Time Count Compare Successful\n");
	}

	/* Stop The Timer */
	status = GPT_stop(hGpt);
	if(CSL_SOK != status)
	{
		printf("GPT Stop Failed \n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT Stop Successful\n");
	}

	/* Reset the timer count to zero */
	status = GPT_reset(hGpt);

	/* Close the GPT module */
	status = GPT_close(hGpt);
	if(CSL_SOK != status)
	{
		printf("GPT Close Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT Close Successful\n");
	}

	return (CSL_TEST_PASSED);
}

/**
 *  \brief  GPT Count Rate Verification test function
 *
 * This function verifies whether timer is running at the configured
 * rate or not. This function configures and enables the GPT interrupts
 * to indicate the expiry of the timer count. Test counts number of
 * cycles executed by the CPU after starting the timer till the expiry
 * of the timer. These cycles are used to verify the WDT count rate.
 * It is assumed that calculated CPU cycles will be with in the range
 * �1% actual CPU clock cycles. Test will be successful if the calculated
 * CPU cycles fall with in this range.
 *
 * NOTE: Changing the PLL setting in the middle (After getSysClk() call)
 *       of the test will result in test failure.
 *
 *  \param  none
 *
 *  \return CSL_TEST_PASSED  - Success
 *          CSL_TEST_FAILED  - Failure
 */
Int16 CSL_gptIntrTest(void)
{
	CSL_Handle    hGpt;
	CSL_Status 	  status;
	CSL_Config 	  hwConfig;
	CSL_GptObj	  gptObj;
	Uint32        cpuCycleDelta;

	hitIsr   = FALSE;
	status   = 0;

	/* Get the System clock value at which CPU is currently running */
	sysClk = getSysClk();

	/* CPU clock cycles that  are 1% of the sysClk" value */
	cpuCycleDelta = (sysClk/100);

	printf("\n\nCPU clock is running at %ldKHz\n", sysClk);
	printf("Timer Prescaler Divide Value is Set to Divide by 4\n");
	printf("GPT Runs at Rate 1/4 of the CPU System Clock\n");
	printf("GPT Count is Initialized to 1/4 of CPU Clock Cycles per Millisecond\n");
	printf("With Reference to CPU Clock GPT Will Take 1 Millisecond to Count Down the Timer to 0\n");
	printf("So The CPU Should Execute Approximately %ld(�1%%) Clock Cycles ",sysClk);
	printf("From the Starting of the Timer Till the Expiry of the Timer\n\n");

	/* Open the CSL GPT module */
	hGpt = GPT_open (GPT_0, &gptObj, &status);
	if((NULL == hGpt) || (CSL_SOK != status))
	{
		printf("GPT Open Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT Open Successful\n");
	}

	/* Reset the GPT module */
	status = GPT_reset(hGpt);
	if(CSL_SOK != status)
	{
		printf("GPT Reset Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT Reset Successful\n");
	}

	/* Clear any pending interrupts */
	IRQ_clearAll();

	/* Disable all the interrupts */
	IRQ_disableAll();

	IRQ_setVecs((Uint32)(&VECSTART));
	IRQ_plug(TINT_EVENT, &gpt0Isr);
	IRQ_enable(TINT_EVENT);

	hwConfig.autoLoad 	 = GPT_AUTO_ENABLE;
	hwConfig.ctrlTim 	 = GPT_TIMER_ENABLE;
	hwConfig.preScaleDiv = GPT_PRE_SC_DIV_1;
	hwConfig.prdLow 	 = (sysClk)/4;
	hwConfig.prdHigh 	 = 0x0000;

	/* Configure the GPT module */
	status =  GPT_config(hGpt, &hwConfig);
	if(CSL_SOK != status)
	{
		printf("GPT Config Failed\n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT Config Successful\n");
	}

	/* Enable CPU Interrupts */
	IRQ_globalEnable();

	/* Start the Timer */
	GPT_start(hGpt);

	/* Wait for the timer interrupt */
	/* This loop takes 12 CPU cycles to execute on iteration.
	   So the CPU cycles executed after starting the timer till the
	   expiry of the timer will be 12*cpuCycleCount. */
	while(hitIsr != TRUE)
	{
		cpuCycleCount++;
	}

	/* Disable the CPU interrupts */
	IRQ_globalDisable();

	/* Clear any pending interrupts */
	IRQ_clearAll();

	/* Disable all the interrupts */
	IRQ_disableAll();

	/* Stop the Timer */
	status = GPT_stop(hGpt);
	if(CSL_SOK != status)
	{
		printf("GPT Stop Failed \n");
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf("GPT Stop Successful\n");
	}

	status = GPT_reset(hGpt);

	/* Display the number of CPU cycles executed */
	printf("\nTimer Count Reached Zero\n");
	cpuCycleCount *= 12;
	printf("\n\nNUMBER OF CPU CYCLES EXECUTED AFTER STARTING THE TIMER TILL THE TIMER EXPIRY: ");
	printf("%ld\n", cpuCycleCount);

	/* Test passes if the "cpuCycles" value is in the range
	   ((sysClk) � (1% of sysClk)) */
	if(((cpuCycleCount - cpuCycleDelta) < sysClk) &&
	    ((cpuCycleCount + cpuCycleDelta) > sysClk))
	{
		printf("GPT IS RUNNING AT THE CONFIGURED RATE!\n\n");
	}
	else
	{
		printf("GPT IS NOT RUNNING AT THE CONFIGURED RATE!\n\n");
		return (CSL_TEST_FAILED);
	}

	/* Close The GPT Module */
	status = GPT_close(hGpt);
	if(CSL_SOK != status)
	{
		printf("GPT Close Failed\n");
		return (CSL_TEST_FAILED);
	}

	return (CSL_TEST_PASSED);
}

/**
 *  \brief  GPT Interrupt Service Routine
 *
 *  \param  none
 *
 *  \return none
 */
interrupt void gpt0Isr(void)
{
	hitIsr = TRUE;
    IRQ_clear(TINT_EVENT);
    /* Clear Timer Interrupt Aggregation Flag Register (TIAFR) */
    CSL_SYSCTRL_REGS->TIAFR = 0x01;
}

/**
 *  \brief  Function to calculate the clock at which system is running
 *
 *  \param    none
 *
 *  \return   System clock value in KHz
 */
#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined(CHIP_C5535) || defined(CHIP_C5545))

Uint32 getSysClk(void)
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
Uint32 getSysClk(void)
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
	Multiplier = temp1/256 +1;
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

	/* Return the value of system clock in KHz */
	return(sysClk);
}
#else

Uint32 getSysClk(void)
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

