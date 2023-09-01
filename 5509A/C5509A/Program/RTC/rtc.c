/*
 *  Copyright 2003 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 *  
 */
/* "@(#) DSP/BIOS 4.90.170 04-22-03 (barracuda-m03)" */
/******************************************************************************\
*           Copyright (C) 1999 Texas Instruments Incorporated.
*                           All Rights Reserved
*------------------------------------------------------------------------------
* PERIPHERAL NAME... RTC
* FILENAME...... rtc3.c
* DATE CREATED.. Thu 03/07/02
* PROJECT....... RTC testing
* COMPONENT..... 5509 (tested on Amadeus skywalker board)
* IMPORTS....... 
*------------------------------------------------------------------------------
* HISTORY:
*   03/07/02 (IM) Created
*
*------------------------------------------------------------------------------
* DESCRIPTION:  
*   
* This is an example of using the RTC periodic and update interrupts for 5509
\*-----------------------------------------------------------------------------*/

#include <csl.h>
#include <csl_rtc.h>
#include <csl_irq.h>
#include <stdio.h>

void myPeriodicIsr(void); 
void myUpdateIsr(void); 
extern Uint32 VECSTART();              //the content found at the label myVec is of type Uint32

volatile Uint16 rtc_cnt = 0;
volatile Uint16 counterPer = 0, counterUp = 0, sec;
int min0, min1 = 0;
int stop = 0;
int old_intm;
int eventId;

RTC_Date myDate = {
     0x03,     /* Year */
     0x02,     /* Month */
     0x28,     /* Daym */
     0x05     /* Dayw */
};

RTC_Time myTime = {
     0x1,     /* Hour */      
     0x4,     /* Minutes */
     0x59,     /* Seconds */
};

RTC_IsrAddr addr = {
        myPeriodicIsr,
//        NULL,
        NULL,
        myUpdateIsr
};
        
main() 
{
    CSL_init();
    
    //Configure RTC time and date
    RTC_setTime(&myTime);       
    RTC_setDate(&myDate);

    old_intm = IRQ_globalDisable();   
    IRQ_setVecs((Uint32)(&VECSTART));    //set IVPD

    /* Clear any pending RTC interrupts */
    eventId = RTC_getEventId();
    IRQ_clear(eventId);
    
    // Place interrupt service routine address at 
    // associated vector location
    RTC_setCallback(&addr);         

    /* Set the RTC periodic interval */
    //RTC_setPeriodicInterval(RTC_RATE_250ms);
    RTC_setPeriodicInterval(RTC_RATE_500ms);
        
    /* Enable all maskable interrupts */
    IRQ_globalEnable();      
    
    /* Start RTC */
    RTC_start();  
        
   sec = RTC_RGET(RTCSEC);
   
   while (sec != 0) 
   {
		sec = RTC_RGET(RTCSEC);
   }
 
 
    /* Enable alarm interrupt to start at 1::5::0 
    - one second after clock is started */
        RTC_eventEnable(RTC_EVT_PERIODIC);      
        RTC_eventEnable(RTC_EVT_UPDATE);        
 
        min0 = RTC_RGET(RTCMIN);

    /* Wait for interrupt to happen */
        while (!stop)   //let interrupts occur for a minute
        {
           while (RTC_FGET(RTCPINTR,UIP) != 0);
           min1 = RTC_RGET(RTCMIN);
        
                if ((min1 - min0) >= 1)
                {
                        RTC_eventDisable(RTC_EVT_PERIODIC);     // disable periodic interrupt after a minute
                        RTC_eventDisable(RTC_EVT_UPDATE); // disable update interrupt after a minute
                        stop = 1;
                }

        }       
        
        printf("\nRTC - testing a combination of update and periodic interrupts - successful\n");
        
        RTC_stop();
        for(;;)
        {}
}

void myPeriodicIsr()
{

   ++counterPer; 
   //sec = RTC_RGET(RTCSEC);
      
   printf("\tPeriodic interrupt at: %x::%x::%x\n",
                RTC_FGET(RTCHOUR, HR), min1, RTC_RGET(RTCSEC));

   //asm("\tNOP                       ;====> RTC periodic interrupt routine");  
}

void myUpdateIsr()
{

   ++counterUp; 
   sec = RTC_RGET(RTCSEC);
   
   printf("Update interrupt at: %x::%x::%x\n",
                RTC_FGET(RTCHOUR, HR), min1, sec);
      
   //asm("\tNOP                       ;====> RTC update interrupt routine");  
}

/******************************************************************************\
* End of rtc3.c
\******************************************************************************/

