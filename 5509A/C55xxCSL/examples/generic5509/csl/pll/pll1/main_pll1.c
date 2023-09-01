/*
 * Copyright (C) 2003 Texas Instruments Incorporated
 * All Rights Reserved
 */
/*
 *---------main_pll1.c---------
 * This is an example of the usage of PLL_setFreq function on C5509.           
 *                                                                             
 * The following assumptions for the PLL CLKMD fields were implemented         
 * in the PLL_setFreq() function:                                              
 *       - IAI = 0; the PLL locks using the same process that was underway     
 *                  before the idle mode was entered.                          
 *       - IOB = 1; If the PLL indicates a break in the phase lock, it         
 *                  switches to its bypass mode and restarts the PLL           
 *                  phase-locking sequence.                                    
 *       - PLLDIV = BYPASSDIV = div -1;                                        
 *       - PLLENAB = 0 (bypass mode) if mul = 1, and 1 (PLL enabled) otherwise.
 *                                                                             
 *                                                                             
 */
#include <csl.h>
#include <csl_pll.h>
#include <stdio.h>

//---------main routine---------
main()
{ 
    //Initialize CSL library
    CSL_init();
 
    //multiplier = 7; Divisor = 2; PLL - enabled
    PLL_setFreq(7,2);       
	
	//If the program hangs in PLL_setFreq call, the TEST FAILS
	printf("\nTEST PASSED\n");
	
	//Infinite loop that outputs a periodic clock at XF pin
	//Use oscilloscope to see the waveform at XF pin
	//Changing the parameters of PLL_setFreq changes the
	//output at XF pin
    asmtest();
}
