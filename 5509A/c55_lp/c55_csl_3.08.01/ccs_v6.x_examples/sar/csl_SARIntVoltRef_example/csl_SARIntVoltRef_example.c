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


/** @file csl_SARIntVoltRef_example.c
 *
 *  @brief Test code to verify the SAR Internal Reference Voltage
 *
 *
 * \page    page16  CSL SAR EXAMPLE DOCUMENTATION
 *
 * \section SAR5   SAR EXAMPLE5 - SAR Internal Reference Voltage example
 *
 * \subsection SAR5x    TEST DESCRIPTION:
 * This test verifies the functionality of CSL SAR(Successive Approximation Register)
 * in interrupt mode. SAR module on C5515/C5517 DSP is used to convert the voltage
 * generated by the battery available on the C5515/C5517 EVM to digital values.
 * Please do place a 1.5V in J22, battery-holder.
 *
 * During the test battery voltage is read and the converted digital value is displayed
 * in the CCS "stdout" window. This digital value corresponds to the voltage of the
 * battery while running the test.
 *
 * SAR module is having six channels to read the data. This test configures channel zero
 * for reading the battery voltage in interrupt mode. CSL INTC module should be configured
 * and SAR ISR should registered before starting the SAR data conversion. After successful
 * configuration of the SAR, voltage conversion is started using the CSL API
 * SAR_startConversion(). Digital value of the battery voltage is read in the ISR twice
 * and is displayed in the CCS "stdout" window. After doing so SAR data conversion
 * is stopped and CSL SAR module will be closed.
 *
 * For a 1.5V battery, the digital value displayed out varies from 0xa-0xb.
 *
 * NOTE: TEST SHOULD NOT BE RUN BY SINGLE STEPPING AFTER ENABLING THE SAR INTERRUPTS.
 * DOING SO WILL LEAD TO IMPROPER HANDLING OF THE INTERRUPTS AND TEST FAILURE.
 *
 * \subsection SAR5y    TEST PROCEDURE:
 *  1. Place a 1.5V battery in J22 and Open CCS, connect target (C5515/C5517 EVM)
 *  2. Open the project "CSL_SAR_IntcExample.pjt" and build it
 *  3. Load the program on to the target
 *  4. Set the PLL frequency to 12.288MHz
 *  5. Run the program and observe the test result
*/
/* ============================================================================
 * Revision History
 * ================
 * 15-Apr-2015 Created
 * 10-Mar-2016 Updates to header
 * ============================================================================
 */


#include "csl_sar.h"
#include "csl_general.h"
#include <csl_intc.h>
#include <stdio.h>

#define CSL_TEST_FAILED         (1)
#define CSL_TEST_PASSED         (0)

/* SAR object Structure    */
CSL_SarHandleObj SarObj;
CSL_SarHandleObj *SarHandle;        /* SAR handle           */
Uint16           readBuffer;        /* SAR Read Buffer      */
int i = 0;

//---------Function prototypes---------//

/** Interrupt Service Routine */
interrupt void sarISR(void);

/* Reference the start of the interrupt vector table */
/* This symbol is defined in file vectors.asm       */
extern void VECSTART(void);

/* To test Keypad voltage Measurement */
int  sar_test_keypad_voltage();



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

	printf("CSL SAR  CH0 voltage calibration test\n\n");
	printf("CSL SAR Interrupt Mode Test\n\n");
	
	status = sar_test_keypad_voltage();
	if(status != CSL_TEST_PASSED)
	{
		printf("\nCSL SAR  CH0 voltage calibration test failed!!\n");
		/////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
		/////  Reseting PaSs_StAtE to 0 if error detected here.
		PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
		/////
	}
	else
	{
		printf("\nCSL SAR  CH0 voltage calibration test passed!!\n");
	}
	/////INSTRUMENTATION FOR BATCH TESTING -- Part 3 --
	/////  At program exit, copy "PaSs_StAtE" into "PaSs".
	PaSs = PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with
	/////                   // pass/fail value determined during program execution.
	/////  Note:  Program should next exit to C$$EXIT and halt, where DSS, under
	/////   control of a host PC script, will read and record the PaSs' value.
	/////
}

Int sar_test_keypad_voltage(void)
{

    CSL_Status    status;
    Bool flag = 1;
    CSL_SarChSetup param;
    int result;

    result = CSL_TEST_FAILED;


    //Disable interrupt
    IRQ_globalDisable();

	/* Clear any pending interrupts */
	IRQ_clearAll();

	/* Disable all the interrupts */
	IRQ_disableAll();

    /* Initialize Interrupt Vector table */
    IRQ_setVecs((Uint32)(&VECSTART));


    /* Initialize the SAR module */
    status = SAR_init();
    if(status != CSL_SOK)
    {
        printf("SAR Init Failed!!\n");
        return(result);
    }
    /* Open SAR channel */
    status = SAR_chanOpen(&SarObj,CSL_SAR_CHAN_0);
    SarHandle = &SarObj;
    if(status != CSL_SOK)
    {
        printf("SAR_chanOpen Failed!!\n");
        return result;
    }
    /* Initialize channel */
    status = SAR_chanInit(SarHandle);
    if(status != CSL_SOK)
    {
        printf("SAR_chanInit Failed!!\n");
        return(result);
    }
	
	    /* Clear any pending Interrupt */
    IRQ_clear(SAR_EVENT);
    IRQ_test(SAR_EVENT,&flag);
    /* Register the ISR */
    IRQ_plug(SAR_EVENT,&sarISR);

    param.OpMode = CSL_SAR_INTERRUPT;
    param.MultiCh = CSL_SAR_NO_DISCHARGE;
    param.RefVoltage = CSL_SAR_REF_0_8v;
    param.SysClkDiv = 0x63 ;
    /* Configuration for SAR module */
    status = SAR_chanSetup(SarHandle,&param);
    if(status != CSL_SOK)
    {
        printf("SAR_chanConfig Failed!!\n");
        return(result);
    }
    /* Set channel cycle set */
    status = SAR_chanCycSet(SarHandle,CSL_SAR_CONTINUOUS_CONVERSION);
    if(status != CSL_SOK)
    {
        printf("SAR_chanCycSet Failed!!\n");
        return(result);
    }
#if (defined(CHIP_C5517))
	/* Disable GPO0En pin */
    CSL_FINST (SarHandle->baseAddr->SARGPOCTRL,SAR_SARGPOCTRL_GPO0EN,
                CLEAR);

     /* Channel 0 is grounded */
     CSL_FINST (SarHandle->baseAddr->SARPINCTRL,SAR_SARPINCTRL_GNDON,SET);

     /* Input the VIn voltage without dividing by 2 */
	CSL_FINST (SarHandle->baseAddr->SARPINCTRL,SAR_SARPINCTRL_HALF,CLEAR);

      /* Reference voltage is based on bandgap voltage  */
    CSL_FINST (SarHandle->baseAddr->SARPINCTRL,SAR_SARPINCTRL_REFAVDDSEL,
	 		CLEAR);
#else
#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined(CHIP_C5535) || defined(CHIP_C5545))
	/* Disable GPO0En pin */
    CSL_FINST (SarHandle->baseAddr->SARGPOCTRL,ANACTRL_SARGPOCTRL_GPO0EN,
                CLEAR);

     /* Channel 0 is grounded */
     CSL_FINST (SarHandle->baseAddr->SARPINCTRL,ANACTRL_SARPINCTRL_GNDON,SET);

     /* Input the VIn voltage without dividing by 2 */
	CSL_FINST (SarHandle->baseAddr->SARPINCTRL,ANACTRL_SARPINCTRL_HALF,CLEAR);

      /* Reference voltage is based on bandgap voltage  */
    CSL_FINST (SarHandle->baseAddr->SARPINCTRL,ANACTRL_SARPINCTRL_REFAVDDSEL,
	 		CLEAR);
#endif
#endif
	
    
	    /* Enabling Interrupt */
    IRQ_enable(SAR_EVENT);
    IRQ_globalEnable();

	/* start the conversion */	
    status = SAR_startConversion(SarHandle);
    if(status != CSL_SOK)
    {
        printf("SAR_startConversion Failed!!\n");
        return(result);
    }


    /* ISR runs for 2 times */
    while(TRUE)
    {
        if(i == 2)
        break;
    }
	
     /* Channel 0 is NO longer grounded */
#if (defined(CHIP_C5517))
     CSL_FINST (SarHandle->baseAddr->SARPINCTRL,SAR_SARPINCTRL_GNDON,CLEAR);
#else
#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined(CHIP_C5535) || defined(CHIP_C5545))
     CSL_FINST (SarHandle->baseAddr->SARPINCTRL,ANACTRL_SARPINCTRL_GNDON,CLEAR);
#endif
#endif

	    /* Stop the conversion */
    status = SAR_stopConversion(SarHandle);
    if(status != CSL_SOK)
    {
        printf("SAR_stopConversion Failed!!\n");
        return(result);
    }
    
	/* Clear any pending interrupts */
	IRQ_clearAll();

	/* Disable all the interrupts */
	IRQ_disableAll();

    //Disable interrupt
    IRQ_globalDisable();

	/* Close the channel */
    status = SAR_chanClose(SarHandle);
    if(status != CSL_SOK)
    {
        printf("SAR_chanClose Failed!!\n");
        return(result);
    }
    /* Deinit */
    status = SAR_deInit();
    if(status != CSL_SOK)
    {
        printf("SAR_deInit Failed!!\n");
        return(result);
    }
    result = CSL_TEST_PASSED;
    return(result);


}

// ISR to read ADC data
interrupt void sarISR(void)
{

    SAR_readData(SarHandle, &readBuffer);
    printf("SAR ADC read data 0x%x\n",readBuffer);
    i++;
    /* For 2 times  */
    if(i == 2)
    IRQ_disable(SAR_EVENT);

}


