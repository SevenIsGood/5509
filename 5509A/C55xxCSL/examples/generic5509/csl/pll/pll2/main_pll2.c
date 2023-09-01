/*
 * Copyright (C) 2003 Texas Instruments Incorporated
 * All Rights Reserved
 */
/*
 *---------main_pll2.c---------
 * This is an example for PLL_config() function for C5509 (Digital PLL)
 */
#include <csl.h>
#include <csl_pll.h>
#include <stdio.h>

//---------Global data definition---------
Uint16 x;
Uint32 y;
CSLBool b;

PLL_Config  myConfig      = {
  0,    //IAI: the PLL locks using the same process that was underway 
                //before the idle mode was entered
  1,    //IOB: If the PLL indicates a break in the phase lock, 
                //it switches to its bypass mode and restarts the PLL phase-locking 
                //sequence
  7,    //PLL multiply value; multiply 7 times
  1             //Divide by 2 PLL divide value; it can be either PLL divide value 
                //(when PLL is enabled), or Bypass-mode divide value
                //(PLL in bypass mode, if PLL multiply value is set to 1)
};

main()
{
        //Initialize CSL library
        CSL_init();
        
        //Configure PLL to output desired frequency clock
        PLL_config(&myConfig);
        
		//If the program hangs in PLL_config call, the TEST FAILS
		printf("\nTEST PASSED\n");
        
        //Infinite loop that outputs a periodic clock at XF pin
		//Use oscilloscope to see the waveform at XF pin
		//Changing the PLL multiply and divide values in myConfig
		//structure changes the output at XF pin
        asmtest();
}
