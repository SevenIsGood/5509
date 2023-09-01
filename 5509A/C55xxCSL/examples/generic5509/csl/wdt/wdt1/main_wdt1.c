/*
 * Copyright (C) 2003 Texas Instruments Incorporated
 * All Rights Reserved
 */
/*
 *---------main_wdt1.c---------
 * This is an example of the usage of WDTIM functions for 5509.
 */
#include <csl.h>
#include <csl_wdtim.h> 
#include <stdio.h>

//---------Global data definition---------
int i, pscVal;
WDTIM_Config getConfig;
WDTIM_Config myConfig = {
     0x1000,          /* WDPRD */
     0x0000,          /* WDTCR */
     0x1000           /* WDTCR2 */
};

//---------main routine---------
main()
{
    CSL_init();
    printf ("\nTESTING...\n");
  
    WDTIM_config(&myConfig);
    WDTIM_FSET(WDTCR, WDOUT, 1);	/* Connect to NMI */
    WDTIM_FSET(WDTCR, TDDR, 0xF);	/* Value to load PSC field */    
    WDTIM_FSET(WDTCR2, PREMD, 0);	/* Set direct mode */
    WDTIM_service();				/* enable watchdog */ 
          
    for (;;)
    {                     
        WDTIM_getConfig(&getConfig);  
        pscVal = WDTIM_FGET(WDTCR,PSC);

        printf("pscVal: %x, wdtcr: %x\n", pscVal, getConfig.wdtcr);    
        
        /* write periodically to WDTIMER  - when this line is commented out,
           the watchdog times out, WDFLAG set to 1, indicating 
           that a Watchdog time-out occurred.*/ 
        WDTIM_service();         
    }           //end for loop
}
