/*
 * Copyright (C) 2003 Texas Instruments Incorporated
 * All Rights Reserved
 */
/*
 *---------main_pll1.c---------
 * This example demostrates the usage of PLL_config and PLL_setFreq functions
 */

#include <stdio.h>

#include <csl.h>
#include <csl_pll.h>

//---------Global data definition---------

//Pre-initialized configuration structure for PLL
PLL_Config myCfg =
{
	PLL_PLLCSR_RMK(
		PLL_PLLCSR_PLLRST_RESET_RELEASED,
		PLL_PLLCSR_OSCPWRDN_OSC_ON,
		PLL_PLLCSR_PLLPWRDN_PLL_ON,
		PLL_PLLCSR_PLLEN_DEFAULT
	),
	PLL_PLLM_PLLM_OF(10),
	PLL_PLLDIV0_RMK(
		PLL_PLLDIV0_D0EN_ENABLED,
		PLL_PLLDIV0_PLLDIV0_OF(0)
	),
	PLL_PLLDIV1_RMK(
		PLL_PLLDIV1_D1EN_ENABLED,
		PLL_PLLDIV1_PLLDIV1_OF(0)
	),
	PLL_PLLDIV2_RMK(
		PLL_PLLDIV2_D2EN_ENABLED,
		PLL_PLLDIV2_PLLDIV2_OF(0)
	),
	PLL_PLLDIV3_RMK(
		PLL_PLLDIV3_D3EN_ENABLED,
		PLL_PLLDIV3_PLLDIV3_OF(0)
	),
	PLL_OSCDIV1_RMK(
		PLL_OSCDIV1_OD1EN_DISABLED,
		PLL_OSCDIV1_OSCDIV1_OF(0)
	),
	PLL_WKEN_RMK(
		PLL_WKEN_WKEN4_ENABLED,
		PLL_WKEN_WKEN3_ENABLED,
		PLL_WKEN_WKEN2_ENABLED,
		PLL_WKEN_WKEN1_ENABLED,
		PLL_WKEN_WKEN0_ENABLED
	),
	PLL_CLKMD_RMK(
		PLL_CLKMD_CLKMD0_OSCOUT
	),
	PLL_CLKOUTSR_RMK(
		PLL_CLKOUTSR_CLKOSEL_SYSCLK1,
		PLL_CLKOUTSR_CLKOUTDIS_ENABLED
	)
};

//---------main routine---------
void main(void)
{
	//Initialize CSL library
	CSL_init();
	
	//Use pre-initialized ocnfiguration structure to set-up PLL registers
  	PLL_config(&myCfg);
  	
  	/*Parameters for PLL_setFreq are:  	
  	enable/mode, multiply, divider0, divider1, divider2, divider3, osc divider
  	
  	    mode  = 1 means PLL enabled (non-bypass mode)
		mul   = 5 means multiply by 5
  		div0  = 0 means divide by 1 using PLLDIV0
  		div1  = 3 means divide by 4 using PLLDIV1
  		div2  = 3 means divide by 4 using PLLDIV2
  		div3  = 3 means divide by 4 using PLLDIV3
  		oscdiv= 1 means divide by 2 using OSCDIV1
  	*/  	
  	//Set the required frequncy for CPU, Fast and Slow peripherals and EMIF
  	PLL_setFreq(1,5,0,3,3,3,1);

	//If PLL_config and PLL_setFreq run successfully, code will reach here.
	//  Otherwise program hangs before this statement.
	printf("\nTEST PASSED\n");
	  	
	//Infinite loop that outputs a periodic clock at XF pin
	//Use oscilloscope to see the waveform at XF pin
	//Changing the parameters of PLL_setFreq changes the
	//output at XF pin
  	asmtest();
}
