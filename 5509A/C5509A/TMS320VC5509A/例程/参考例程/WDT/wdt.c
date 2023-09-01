/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DSP/BIOS 4.90.150 04-08-03 (barracuda-m02)" */
/******************************************************************************\
*           Copyright (C) 2000 Texas Instruments Incorporated.
*                           All Rights Reserved
*------------------------------------------------------------------------------
* MODULE NAME... WDTIM
* FILENAME...... wdt.c
* DATE CREATED.. Tu 11/03/2000 
* PROJECT....... Chip Support Library
* COMPONENT..... 
* PREREQUISITS.. 
*------------------------------------------------------------------------------
* HISTORY:
*   11/03/2000  (IM) Created
*  
*------------------------------------------------------------------------------
* DESCRIPTION:  
*   
* This is an example of the usage of WDTIM functions for 5509.
\*----------------------------------------------------------------------*/
#include <csl.h>
#include <csl_wdtim.h> 
#include <stdio.h>

int i, pscVal;
WDTIM_Config getConfig;
WDTIM_Config myConfig = {
     0x1000,          /* WDPRD */
     0x0000,          /* WDTCR */
     0x1000           /* WDTCR2 */
};


main()
{

    CSL_init();
  
#if (_WDTIM_SUPPORT)


    WDTIM_config(&myConfig);
    
    WDTIM_FSET(WDTCR, WDOUT, 1);           /* Connect to NMI */
    WDTIM_FSET(WDTCR, TDDR, 0xF);          /* Value to load PSC field */    
    WDTIM_FSET(WDTCR2, PREMD, 0);          /* Set direct mode */


        WDTIM_service();      /* enable watchdog */ 
          
    //for (i = 0; i < 100; i++)
        for (;;)
    {                     
        
        WDTIM_getConfig(&getConfig);  
        pscVal = WDTIM_FGET(WDTCR,PSC);

         printf("pscVal: %x, wdtcr: %x\n", 
                pscVal, getConfig.wdtcr);    
  
        
        /* write periodically to WDTIMER  - when this line is commented out,
           the watchdog times out, WDFLAG set to 1, indicating 
           that a Watchdog time-out occurred.*/ 
        WDTIM_service();         
    
    }           //end for loop
#endif
}



/******************************************************************************\
* End of wdt.c
\******************************************************************************/
