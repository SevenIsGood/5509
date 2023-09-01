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


/** @file csl_SAR_GPO_example.c
 *
 *  @brief Test code to verify the SAR GOP mode
 *
 *
 * \page    page16  CSL SAR EXAMPLE DOCUMENTATION
 *
 * \section SAR4   SAR EXAMPLE4 - SAR GPO MODE TEST
 *
 * \subsection SAR4x    TEST DESCRIPTION:
 * 1. Enable clocks to Analog Domain registers and SAR ADC.
 * 2. Enable GPO functionality in SARGPOCTRL register.
 * 3. Set breakpoints at lines 66, 71, 76, 80.
 * (i.e. at points where you drive the GPO pins low and high)
 * NOTE: Don't drive GPO_0 high.
 * It cant can't be driven high by design. Can be driven only low.
 * 4. Measure with a multimeter, voltages at pins 2,4,6,8 on JP15 exposed on-board.
 * Voltage must be equal to VDD_ANA, i.e. 1.3V when driven high and 0V when driven low.
 *
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

/* To test Keypad voltage Measurement */
int  sar_gpo_test();



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

	printf("CSL SAR  GPO test\n\n");

	status = sar_gpo_test();
	if(status != CSL_TEST_PASSED)
	{
		printf("\nCSL SAR  GPO test failed!!\n");
		/////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
		/////  Reseting PaSs_StAtE to 0 if error detected here.
		PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
		/////
	}
	else
	{
		printf("\nCSL SAR  GPO test passed!!\n");
	}
	/////INSTRUMENTATION FOR BATCH TESTING -- Part 3 --
	/////  At program exit, copy "PaSs_StAtE" into "PaSs".
	PaSs = PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with
	/////                   // pass/fail value determined during program execution.
	/////  Note:  Program should next exit to C$$EXIT and halt, where DSS, under
	/////   control of a host PC script, will read and record the PaSs' value.
	/////
}

Int sar_gpo_test(void)
{

    CSL_Status    status;
    int result;
    result = CSL_TEST_FAILED;

    /* Initialize the SAR module */
    status = SAR_init();
    if(status != CSL_SOK)
    {
        printf("SAR Init Failed!!\n");
        return(result);
    }

#if (defined(CHIP_C5517))
    CSL_SAR_REGS->SARGPOCTRL |= (CSL_SAR_SARGPOCTRL_GPO0EN_MASK |
    		CSL_SAR_SARGPOCTRL_GPO1EN_MASK |
    		CSL_SAR_SARGPOCTRL_GPO2EN_MASK |
    		CSL_SAR_SARGPOCTRL_GPO3EN_MASK);

    CSL_SAR_REGS->SARGPOCTRL &= ~(CSL_SAR_SARGPOCTRL_GPO3_MASK |
    		CSL_SAR_SARGPOCTRL_GPO2_MASK |
    		CSL_SAR_SARGPOCTRL_GPO1_MASK |
    		CSL_SAR_SARGPOCTRL_GPO0_MASK);

    CSL_SAR_REGS->SARGPOCTRL |= (CSL_SAR_SARGPOCTRL_GPO3_MASK |
    		CSL_SAR_SARGPOCTRL_GPO2_MASK |
    		CSL_SAR_SARGPOCTRL_GPO1_MASK);

    CSL_SAR_REGS->SARGPOCTRL &= ~(CSL_SAR_SARGPOCTRL_GPO3_MASK |
    		CSL_SAR_SARGPOCTRL_GPO2_MASK |
    		CSL_SAR_SARGPOCTRL_GPO1_MASK |
    		CSL_SAR_SARGPOCTRL_GPO0_MASK);
#else
#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined(CHIP_C5535) || defined(CHIP_C5545))
    CSL_SAR_REGS->SARGPOCTRL |= (CSL_ANACTRL_SARGPOCTRL_GPO0EN_MASK |
    		CSL_ANACTRL_SARGPOCTRL_GPO1EN_MASK |
    		CSL_ANACTRL_SARGPOCTRL_GPO2EN_MASK |
    		CSL_ANACTRL_SARGPOCTRL_GPO3EN_MASK);

    CSL_SAR_REGS->SARGPOCTRL &= ~(CSL_ANACTRL_SARGPOCTRL_GPO3_MASK |
    		CSL_ANACTRL_SARGPOCTRL_GPO2_MASK |
    		CSL_ANACTRL_SARGPOCTRL_GPO1_MASK |
    		CSL_ANACTRL_SARGPOCTRL_GPO0_MASK);

    CSL_SAR_REGS->SARGPOCTRL |= (CSL_ANACTRL_SARGPOCTRL_GPO3_MASK |
    		CSL_ANACTRL_SARGPOCTRL_GPO2_MASK |
    		CSL_ANACTRL_SARGPOCTRL_GPO1_MASK);

    CSL_SAR_REGS->SARGPOCTRL &= ~(CSL_ANACTRL_SARGPOCTRL_GPO3_MASK |
    		CSL_ANACTRL_SARGPOCTRL_GPO2_MASK |
    		CSL_ANACTRL_SARGPOCTRL_GPO1_MASK |
    		CSL_ANACTRL_SARGPOCTRL_GPO0_MASK);
#endif
#endif
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
