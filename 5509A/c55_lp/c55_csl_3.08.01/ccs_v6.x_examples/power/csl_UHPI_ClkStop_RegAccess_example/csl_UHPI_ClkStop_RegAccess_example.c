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

/** @file csl_UHPI_ClkStop_RegAccess_example.c
 *
 *  @brief Example to verify UHPI r/w recovery after clock-gating
 *
 *
 * \page    page23 POWER EXAMPLE DOCUMENTATION
 *
 * \section POWER13  POWER EXAMPLE13 -  UHPI REG R/W RECOVERY AFTER CLOCK GATING
 *
 * \subsection POWER13x      TEST DESCRIPTION:
 *         This test starts reading default after-reset values in the
 * UHPI register space and validates them to be as expected by the
 * datasheet. In between the clock to the UHPI peripheral is shut off
 * and restarted. The UHPI register space r/w check is resumed. The
 * r/w shouldn't be affected by the shutdown-restart activity on the
 * clock to the UHPI module.
 *
 * \subsection POWER13y      TEST PROCEDURE:
 *
 * 1. Configure the clock to the UHPI module.
 * 2. Start reading the default after-reset values of the UHPI registers.
 * 3. Compare them with the expected values as mentioned in the datasheet.
 * 4. Request for gating UHPI clock by setting the corresponding bit in
 *    CLKSTOP1 and wait till ack is received in corresponding bit, again
 *    in CLKSTOP1.
 * 5. Shut down the clock to UHPI peripheral by setting the corresponding
 *    bit in PCGCR1.
 * 6. After a small delay, restart the clock by clearing the same bit.
 * 7. Resume reg r/w and ensure there are no mismatches.
 *
 * \subsection POWER13z      TEST RESULT:
 *   All the CSL APIs should return success.
 *   Reg r/w shouldn't throw any mismatch.
*/

#include <stdio.h>
#include "csl_uhpi.h"
#include "csl_intc.h"
#include "csl_general.h"
#include "cslr_sysctrl.h"

#define CSL_TEST_FAILED  (-1)
#define CSL_TEST_PASSED  (0)

#define CSL_PWREMU_MGMT_WRITEVAL 	0xFFFF
#define CSL_GPINT_CTRLL_WRITEVAL 	0xFFFF
#define CSL_GPINT_CTRLU_WRITEVAL 	0xFFFF
#define CSL_GPIO_ENL_WRITEVAL 		0xFFFF
#define CSL_GPIO_ENU_WRITEVAL 		0xFFFF
#define CSL_GPIO_DIR1L_WRITEVAL 	0xFFFF
#define CSL_GPIO_DIR1U_WRITEVAL 	0xFFFF
#define CSL_GPIO_DAT1L_WRITEVAL 	0xFFFF
#define CSL_GPIO_DAT1U_WRITEVAL 	0xFFFF
#define CSL_GPIO_DIR2L_WRITEVAL 	0xFFFF
#define CSL_GPIO_DIR2U_WRITEVAL		0xFFFF
#define CSL_GPIO_DAT2L_WRITEVAL 	0xFFFF
#define CSL_GPIO_DAT2U_WRITEVAL 	0xFFFF
#define CSL_GPIO_DIR3L_WRITEVAL 	0xFFFF
#define CSL_GPIO_DIR3U_WRITEVAL 	0xFFFF
#define CSL_GPIO_DAT3L_WRITEVAL 	0xFFFF
#define CSL_GPIO_DAT3U_WRITEVAL 	0xFFFF
#define CSL_HPICL_WRITEVAL 			0xFFFF
#define CSL_HPIAWL_WRITEVAL 		0xFFFF
#define CSL_HPIAWU_WRITEVAL 		0xFFFF
#define CSL_HPIARL_WRITEVAL 		0xFFFF
#define CSL_HPIARU_WRITEVAL			0xFFFF
#define CSL_XHPIAWL_WRITEVAL 		0xFFFF
#define CSL_XHPIAWU_WRITEVAL		0xFFFF
#define CSL_XHPIARL_WRITEVAL 		0xFFFF
#define CSL_XHPIARU_WRITEVAL		0xFFFF


#define CSL_PWREMU_MGMT_READVAL 	0x0003
#define CSL_GPINT_CTRLL_READVAL 	0x0007
#define CSL_GPINT_CTRLU_READVAL 	0x0007
#define CSL_GPIO_ENL_READVAL 		0xFFFF
#define CSL_GPIO_ENU_READVAL 		0x0001
#define CSL_GPIO_DIR1L_READVAL 		0xFFFF
#define CSL_GPIO_DIR1U_READVAL 		0xFFFF
#define CSL_GPIO_DAT1L_READVAL 		0xFFFF
#define CSL_GPIO_DAT1U_READVAL 		0xFFFF
#define CSL_GPIO_DIR2L_READVAL 		0x7FFF
#define CSL_GPIO_DIR2U_READVAL		0x0000
#define CSL_GPIO_DAT2L_READVAL 		0x7FFF
#define CSL_GPIO_DAT2U_READVAL 		0x0000
#define CSL_GPIO_DIR3L_READVAL 		0xFFFF
#define CSL_GPIO_DIR3U_READVAL 		0xFFFF
#define CSL_GPIO_DAT3L_READVAL 		0xFFCF
#define CSL_GPIO_DAT3U_READVAL 		0xFFFF
#define CSL_HPICL_READVAL 			0x00C4
#define CSL_HPIAWL_READVAL 			0xFFFF
#define CSL_HPIAWU_READVAL 			0xFFFF
#define CSL_HPIARL_READVAL 			0xFFFF
#define CSL_HPIARU_READVAL			0xFFFF
#define CSL_XHPIAWL_READVAL 		0xFFFF
#define CSL_XHPIAWU_READVAL			0xFFFF
#define CSL_XHPIARL_READVAL 		0xFFFF
#define CSL_XHPIARU_READVAL			0xFFFF


CSL_Status CSL_uhpiRegAccessTest(void);

/////INSTRUMENTATION FOR BATCH TESTING -- Part 1 --
/////  Define PaSs_StAtE variable for catching errors as program executes.
/////  Define PaSs flag for holding final pass/fail result at program completion.
   volatile Int16 PaSs_StAtE = 0x0001; // Init to 1. Reset to 0 at any monitored execution error.
   volatile Int16 PaSs = 0x0000; // Init to 0.  Updated later with PaSs_StAtE when and if
/////                                  program flow reaches expected exit point(s).
/////

 void main()
 {

#if (defined(CHIP_C5517))
    CSL_Status status;

	printf("\r\nUHPI REGISTER ACCESS TEST\r\n");

	status = CSL_uhpiRegAccessTest();
	if(status != CSL_SOK)
	{
		printf("\n\nCSL UHPI REGISTER ACCESS TEST FAILED!!\n");
		PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
	}
	else
	{
		printf("\n\nCSL UHPI REGISTER ACCESS TEST PASSED!!\n");
	}
		/////INSTRUMENTATION FOR BATCH TESTING -- Part 3 --
	/////  At program exit, copy "PaSs_StAtE" into "PaSs".
	PaSs = PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with
	/////                   // pass/fail value determined during program execution.
	/////  Note:  Program should next exit to C$$EXIT and halt, where DSS, under
	/////   control of a host PC script, will read and record the PaSs' value.
	/////
#else
#warn "Ensure CHIP_C5517 is defined. C5504/05/14/15 don't have UHPI"
	printf("\n\nEnsure CHIP_C5517 is #defined. C5504/05/14/15 don't have UHPI\n");
#endif
 }


CSL_Status CSL_uhpiRegAccessTest(void)
{

	unsigned char fail_flag=0, loop_cnt=0;
#if (defined(CHIP_C5517))
	/* Enable clocks to all peripherals */

	CSL_SYSCTRL_REGS->PCGCR2 &= 0xFFFE;//just uhpi


	//Desassert clk stop request to uhpi
	CSL_SYSCTRL_REGS->CLKSTOP1 &= 0xF3FF;
	  // Wait until the clk stop ack is deasserted
	while ((CSL_SYSCTRL_REGS->CLKSTOP1 & 0x0800) != (Uint16) 0x0000 )  {
	     __asm("    NOP");
	     }
	CSL_SYSCTRL_REGS->EBSR &= 0x8FFF;


    
	if(CSL_UHPI_PWREMU_MGMT_RESETVAL 			!= CSL_UHPI_REGS->PWREMU_MGMT)
	{
	 printf("\n\n Reset Value is deviating for Register:PWREMU_MGMT!!\n");
	 fail_flag+=1;
	}

 	if(CSL_UHPI_GPINT_CTRLL_RESETVAL			!= CSL_UHPI_REGS->GPINT_CTRLL)   		
	{
	 printf("\n\n Reset Value is deviating for Register:GPINT_CTRLL!!\n");
	 fail_flag +=1;
	} 
 	if(CSL_UHPI_GPINT_CTRLU_RESETVAL			!= CSL_UHPI_REGS->GPINT_CTRLU)   		
	{
	 printf("\n\n Reset Value is deviating for Register:GPINT_CTRLU!!\n");
	 fail_flag +=1;
	}
	if(CSL_UHPI_GPIO_ENL_RESETVAL 			!= CSL_UHPI_REGS->GPIO_ENL)         
	{
	 printf("\n\n Reset Value is deviating for Register:GPIO_ENL!!\n");
	 fail_flag +=1;
	}
 	if(CSL_UHPI_GPIO_ENU_RESETVAL			!= CSL_UHPI_REGS->GPIO_ENU)   		
	{
	 printf("\n\n Reset Value is deviating for Register:GPIO_ENU!!\n");
	 fail_flag +=1;
	}
 	if(CSL_UHPI_GPIO_DIR1L_RESETVAL			!= CSL_UHPI_REGS->GPIO_DIR1L)   	
	{
	 printf("\n\n Reset Value is deviating for Register:GPIO_DIR1L!!\n");
	 fail_flag +=1;
	}
	if(CSL_UHPI_GPIO_DIR1U_RESETVAL			!= CSL_UHPI_REGS->GPIO_DIR1U)   	
	{
	 printf("\n\n Reset Value is deviating for Register:GPIO_DIR1U!!\n");
	 fail_flag +=1;
	}
	if(CSL_UHPI_GPIO_DAT1U_RESETVAL			!= CSL_UHPI_REGS->GPIO_DAT1U)		
	{
	 printf("\n\n Reset Value is deviating for Register:GPIO_DAT1U!!\n");
	 fail_flag +=1;
	}
	if(CSL_UHPI_GPIO_DIR2L_RESETVAL 	!= CSL_UHPI_REGS->GPIO_DIR2L)
	{
	 printf("\n\n Reset Value is deviating for Register:GPIO_DIR2L!!\n");
	 fail_flag +=1;
	}
	if(CSL_UHPI_GPIO_DIR2U_RESETVAL 	!= CSL_UHPI_REGS->GPIO_DIR2U)
	{
	 printf("\n\n Reset Value is deviating for Register:GPIO_DIR2U!!\n");
	 fail_flag +=1;
	}
	if(CSL_UHPI_GPIO_DAT2U_RESETVAL 	!= CSL_UHPI_REGS->GPIO_DAT2U)
	{
	 printf("\n\n Reset Value is deviating for Register:GPIO_DAT2U!!\n");
	 fail_flag +=1;
	}
	if(CSL_UHPI_GPIO_DIR3L_RESETVAL 	!= CSL_UHPI_REGS->GPIO_DIR3L)
	{
	 printf("\n\n Reset Value is deviating for Register:GPIO_DIR3L!!\n");
	 fail_flag +=1;
	}
	if(CSL_UHPI_GPIO_DIR3U_RESETVAL 	!= CSL_UHPI_REGS->GPIO_DIR3U)
	{
	 printf("\n\n Reset Value is deviating for Register:GPIO_DIR2U!!\n");
	 fail_flag +=1;
	}
	if(CSL_UHPI_GPIO_DAT3L_RESETVAL 	!= CSL_UHPI_REGS->GPIO_DAT3L)
	{
	 printf("\n\n Reset Value is deviating for Register:GPIO_DAT3L!!\n");
	 fail_flag +=1;
	}
	if(CSL_UHPI_GPIO_DAT3U_RESETVAL 	!= CSL_UHPI_REGS->GPIO_DAT3U)
	{
	 printf("\n\n Reset Value is deviating for Register:GPIO_DAT3U!!\n");
	 fail_flag +=1;
	}
	if(CSL_UHPI_HPICL_RESETVAL 	!= CSL_UHPI_REGS->UHPICL)
	{
	 printf("\n\n Reset Value is deviating for Register:UHPICL!!\n");
	 fail_flag +=1;
	}
	if(CSL_UHPI_UHPIAWL_RESETVAL 	!= CSL_UHPI_REGS->UHPIAWL)
	{
	 printf("\n\n Reset Value is deviating for Register:UHPIAWL!!\n");
	 fail_flag +=1;
	}
	if(CSL_UHPI_UHPIAWU_RESETVAL 	!= CSL_UHPI_REGS->UHPIAWU)
	{
	 printf("\n\n Reset Value is deviating for Register:HPIAWU!!\n");
	 fail_flag +=1;
	}
	if(CSL_UHPI_UHPIARL_RESETVAL 	!= CSL_UHPI_REGS->UHPIARL)
	{
	 printf("\n\n Reset Value is deviating for Register:HPIARL!!\n");
	 fail_flag +=1;
	}
	if(CSL_UHPI_UHPIARU_RESETVAL 	!= CSL_UHPI_REGS->UHPIARU)
	{
	 printf("\n\n Reset Value is deviating for Register:HPIARU!!\n");
	 fail_flag +=1;
	}
#ifndef CHIP_C5517
	if(CSL_UHPI_XHPIAWL_RESETVAL 	!= CSL_UHPI_REGS->XHPIAWL)
	{
	 printf("\n\n Reset Value is deviating for Register:XHPIAWL!!\n");
	 fail_flag +=1;
	}
	if(CSL_UHPI_XHPIAWU_RESETVAL 	!= CSL_UHPI_REGS->XHPIAWU)
	{
	 printf("\n\n Reset Value is deviating for Register:XHPIAWU!!\n");
	 fail_flag +=1;
	}
	if(CSL_UHPI_XHPIARL_RESETVAL 	!= CSL_UHPI_REGS->XHPIARL)
	{
	 printf("\n\n Reset Value is deviating for Register:XHPIARL!!\n");
	 fail_flag +=1;
	}
	if(CSL_UHPI_XHPIARU_RESETVAL 	!= CSL_UHPI_REGS->XHPIARU)
	{
	 printf("\n\n Reset Value is deviating for Register:XHPIARU!!\n");
	 fail_flag +=1;
	}
#endif


	CSL_SYSCTRL_REGS->PCGCR2 &= 0xFFFE;//just uhpi

	//Desassert clk stop request to uhpi
	CSL_SYSCTRL_REGS->CLKSTOP1 |= 0x0400;
	  // Wait until the clk stop ack is deasserted
	while ((CSL_SYSCTRL_REGS->CLKSTOP1 & 0x0800) == (Uint16) 0x0000 )  {
	     __asm("    NOP");
	     }

	CSL_SYSCTRL_REGS->PCGCR2 |= 0x0001;//just gate uhpi clk

	for(loop_cnt=0;loop_cnt<1000;loop_cnt++);

	CSL_SYSCTRL_REGS->PCGCR2 &= 0xFFFE;//restart uhpi clk
		//Desassert clk stop request to uhpi
		CSL_SYSCTRL_REGS->CLKSTOP1 &= 0xF3FF;
		  // Wait until the clk stop ack is deasserted
		while ((CSL_SYSCTRL_REGS->CLKSTOP1 & 0x0800) != (Uint16) 0x0000 )  {
		     __asm("    NOP");
		     }

	/* Register access test */
	    CSL_UHPI_REGS->PWREMU_MGMT = CSL_PWREMU_MGMT_WRITEVAL;
		if(CSL_PWREMU_MGMT_READVAL 			!= CSL_UHPI_REGS->PWREMU_MGMT)
		{
		 printf("\n\n Register access failed for Register:PWREMU_MGMT!!\n");
		 fail_flag+=1;
		}
		CSL_UHPI_REGS->PWREMU_MGMT = CSL_UHPI_PWREMU_MGMT_RESETVAL;
	
	 	CSL_UHPI_REGS->GPINT_CTRLL = CSL_GPINT_CTRLL_WRITEVAL;
	 	if(CSL_GPINT_CTRLL_READVAL			!= CSL_UHPI_REGS->GPINT_CTRLL)   		
		{
		 printf("\n\n Register access failed for Register:GPINT_CTRLL!!\n");
		 fail_flag +=1;
		} 
		CSL_UHPI_REGS->GPINT_CTRLL = CSL_UHPI_GPINT_CTRLL_RESETVAL;

		CSL_UHPI_REGS->GPINT_CTRLU = CSL_GPINT_CTRLU_WRITEVAL;
	 	if(CSL_GPINT_CTRLU_READVAL			!= CSL_UHPI_REGS->GPINT_CTRLU)   		
		{
		 printf("\n\n Register access failed for Register:GPINT_CTRLU!!\n");
		 fail_flag +=1;
		}
		CSL_UHPI_REGS->GPINT_CTRLU = CSL_UHPI_GPINT_CTRLU_RESETVAL;


		CSL_UHPI_REGS->UHPICL = CSL_HPICL_WRITEVAL;
		if(CSL_HPICL_READVAL 	!= CSL_UHPI_REGS->UHPICL)
		{
		 printf("\n\n Register access failed for Register:HPICL!!\n");
		 fail_flag +=1;
		}
		CSL_UHPI_REGS->UHPICL = CSL_UHPI_HPICL_RESETVAL;


#endif
	if( !fail_flag )
	{
		return (CSL_TEST_PASSED);
	}
	else
	{
	    return (CSL_TEST_FAILED);
	}
	
 
}
