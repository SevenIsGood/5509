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

/** @file csl_rtc_interrupt_xevt_example.c
 *
 *  @brief Power sample source file
 *
 *
 * \page    page23  POWER EXAMPLE DOCUMENTATION
 *
 * \section POWER11   POWER EXAMPLE11 - RTC Idle2 mode external evt wakeup example
 *
 * \subsection POWER11x    TEST DESCRIPTION:
 *         This test configures RTC for an external event after it's started.
 *
 * \subsection POWER11y    TEST PROCEDURE AND EXPECTED RESULT:
 *  1. Run the test. RTC would be configured for Ext event and RTC started.
 *  2. The test will wait for the external event interrupt.
 *  3. Move the jumper JP6 (on C5517 EVM)/ JP5 (on C5515 evm) from 1-2 position to 2-3 position. WAKEUP toggles HIGH->LOW.
 *  4. Observe if the external event interrupt got generated.
*/

#include <stdio.h>
#include <csl_rtc.h>
#include <csl_intc.h>
#include <csl_general.h>

#define  EVM5515	0x1

//#define PHOENIX_QT

#define RTC_TIME_PRINT_CYCLE    (0xFFu)
#define RTC_CALL_BACK           (1u)

CSL_RtcTime	     InitTime;
CSL_RtcDate 	 InitDate;
CSL_RtcTime 	 GetTime;
CSL_RtcDate 	 GetDate;
CSL_RtcConfig    rtcConfig;
CSL_RtcConfig    rtcGetConfig;
CSL_RtcIsrAddr   isrAddr;
CSL_RtcIsrDispatchTable      rtcDispatchTable;

volatile Uint16    ext_evt_intr = 0;

/* Reference the start of the interrupt vector table */
extern void VECSTART(void);
/* Prototype declaration for ISR function */
interrupt void rtc_isr(void);

void rtc_extEvt(void);
CSL_Status rtc_test(void);

   /////INSTRUMENTATION FOR BATCH TESTING -- Part 1 --
   /////  Define PaSs_StAtE variable for catching errors as program executes.
   /////  Define PaSs flag for holding final pass/fail result at program completion.
        volatile Int16 PaSs_StAtE = 0x0001; // Init to 1. Reset to 0 at any monitored execution error.
        volatile Int16 PaSs = 0x0000; // Init to 0.  Updated later with PaSs_StAtE when and if
   /////                                  program flow reaches expected exit point(s).
   /////
void main()
{
	CSL_Status    status;


	printf("CSL RTC EXTERNAL EVENT TEST\n\n");
	printf("This test demonstrates RTC Externel event interrupt capability\n");

	status =  rtc_test();

	if(status == CSL_SOK){

	}else{
		PaSs_StAtE=0x0000;
	}

    printf("Current Time and Date is : %02d:%02d:%02d:%04d, %02d-%02d-%02d\n",
	GetTime.hours,GetTime.mins,GetTime.secs,GetTime.mSecs,GetDate.day,GetDate.month,GetDate.year);

    IRQ_globalDisable();

	/* Clear any pending interrupts */
	IRQ_clearAll();

	/* Disable all the interrupts */
	IRQ_disableAll();

	/* Stop the RTC */
	RTC_stop();

	printf("\nCSL RTC TESTS COMPLETED\n");

   /////INSTRUMENTATION FOR BATCH TESTING -- Part 3 --
   /////  At program exit, copy "PaSs_StAtE" into "PaSs".
        PaSs = PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with
   /////                   // pass/fail value determined during program execution.
   /////  Note:  Program should next exit to C$$EXIT and halt, where DSS, under
   /////   control of a host PC script, will read and record the PaSs' value.
   /////

}

CSL_Status rtc_test(void){

	CSL_Status         status;
	CSL_Status         result;

	result = !CSL_SOK;


#ifdef PHOENIX_QT
	 *((volatile ioport unsigned short *)0x1C27)=0x0001;
	 CSL_RTC_REGS->RGKR_LSW = 0xF1E0;
	 CSL_RTC_REGS->RGKR_MSW = 0x95A4;
#else
	#if (defined(C5515_EVM)||defined(C5517_EVM))
		#if (defined(CHIP_C5517))
				 CSL_SYSCTRL_REGS->RSCR = 0x0001;
				 CSL_RTC_REGS->RGKR_LSW = 0xF1E0;
				 CSL_RTC_REGS->RGKR_MSW = 0x95A4;
		#endif
	#else
		#warn "Define C5515_EVM, C5517_EVM or PHOENIX_QT"
	#endif
#endif


	/* Set the RTC config structure */
	rtcConfig.rtcyear  = 8;
	rtcConfig.rtcmonth = 8;
	rtcConfig.rtcday   = 8;
	rtcConfig.rtchour  = 8;
	rtcConfig.rtcmin   = 8;
	rtcConfig.rtcsec   = 8;
	rtcConfig.rtcmSec  = 8;

	rtcConfig.rtcyeara  = 8;
	rtcConfig.rtcmontha = 8;
	rtcConfig.rtcdaya   = 8;
	rtcConfig.rtchoura  = 8;
	rtcConfig.rtcmina   = 8;
	rtcConfig.rtcseca   = 8;
	rtcConfig.rtcmSeca  = 10;

	//rtcConfig.rtcintcr  = 0x803F;
	rtcConfig.rtcintcr  = 0x0020; //Enable the RTC externel event interrupt

	/* Set the RTC init structure */
    InitDate.year  = 2013;
    InitDate.month = 12;
    InitDate.day   = 3;

    InitTime.hours = 13;
    InitTime.mins  = 54;
    InitTime.secs  = 12;
    InitTime.mSecs = 12;

    /* Register the ISR function */
    isrAddr.ExtEvtAddr    = rtc_extEvt;

    status = RTC_setCallback(&rtcDispatchTable, &isrAddr);
	if(status != CSL_SOK)
	{
		printf("RTC_setCallback Failed\n");
		return(result);
	}
	else
	{
		printf("RTC_setCallback Successful\n");
	}

	/* Configure and enable the RTC interrupts using INTC module */
    IRQ_globalDisable();

	/* Clear any pending interrupts */
	IRQ_clearAll();

	/* Disable all the interrupts */
	IRQ_disableAll();

	IRQ_setVecs((Uint32)&VECSTART);
	IRQ_clear(RTC_EVENT);

	IRQ_plug (RTC_EVENT, &rtc_isr);

	IRQ_enable(RTC_EVENT);
	IRQ_globalEnable();

	/* Reset the RTC */
	RTC_reset();

	/* Configure the RTC module */
	status = RTC_config(&rtcConfig);
	if(status != CSL_SOK)
	{
		printf("RTC_config Failed\n");
		return(result);
	}
	else
	{
		printf("RTC_config Successful\n");
	}

	/* Read the configuration values from the RTC module */
	status = RTC_getConfig(&rtcGetConfig);
	if(status != CSL_SOK)
	{
		printf("RTC_getConfig Failed\n");
		return(result);
	}
	else
	{
		printf("RTC_getConfig Successful\n");
	}

	/* Set the RTC time */
	status = RTC_setTime(&InitTime);
	if(status != CSL_SOK)
	{
		printf("RTC_setTime Failed\n");
		return(result);
	}
	else
	{
		printf("RTC_setTime Successful\n");
	}

	/* Set the RTC date */
	status = RTC_setDate(&InitDate);
	if(status != CSL_SOK)
	{
		printf("RTC_setDate Failed\n");
		return(result);
	}
	else
	{
		printf("RTC_setDate Successful\n");
	}

 	/* Enable the RTC EXTERNEL EVENT interrupts */
	status = RTC_eventEnable(CSL_RTC_EXTEVENT_INTERRUPT);
	if(status != CSL_SOK)
	{
		printf("RTC_eventEnable for EXTERNEL EVENT Failed\n");
		return(result);
	}
	else
	{
		printf("RTC_eventEnable for EXTERNEL EVENT Successful\n");
	}



	printf("\nStarting the RTC\n\n");
	/* Start the RTC */
	RTC_start();
	printf("\nRTC started...waiting for the external event...\n\n");

 	status = RTC_getTime(&GetTime);
	if(status != CSL_SOK)
	{
		printf("RTC_getTime Failed\n");
		return(result);
	}

 	status = RTC_getDate(&GetDate);
	if(status != CSL_SOK)
	{
		printf("RTC_getDate Failed\n");
		return(result);
	}


    printf("Current Time and Date is : %02d:%02d:%02d:%04d, %02d-%02d-%02d\n",
	GetTime.hours,GetTime.mins,GetTime.secs,GetTime.mSecs,GetDate.day,GetDate.month,GetDate.year);

	while(!ext_evt_intr); //Wait for the externel events

 	status = RTC_getTime(&GetTime);
	if(status != CSL_SOK)
	{
		printf("RTC_getTime Failed\n");
		return(result);
	}

 	status = RTC_getDate(&GetDate);
	if(status != CSL_SOK)
	{
		printf("RTC_getDate Failed\n");
		return(result);
	}
	result = CSL_SOK;
	return(result);

}
interrupt void rtc_isr(void)
{

#ifdef RTC_CALL_BACK
    CSL_RTCEventType rtcEventType;

    rtcEventType = RTC_getEventId();

    if (((void (*)(void))(rtcDispatchTable.isr[rtcEventType])))
     {
         ((void (*)(void))(rtcDispatchTable.isr[rtcEventType]))();
     }
#else
    Uint16 statusRegVal;

    statusRegVal = CSL_RTC_REGS->RTCINTFL;

    /* check for alarm interrupt */
    if (CSL_RTC_RTCINTFL_ALARMFL_MASK ==
                (statusRegVal & (Uint16)CSL_RTC_RTCINTFL_ALARMFL_MASK ))
    {
		CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_ALARMFL, SET);
    }
    /* check for external event interrupt */
    else if (CSL_RTC_RTCINTFL_EXTFL_MASK ==
                (statusRegVal &(Uint16)CSL_RTC_RTCINTFL_EXTFL_MASK ))
    {
		CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_EXTFL, SET);
    }
    /* check for day interrupt */
    else if (CSL_RTC_RTCINTFL_DAYFL_MASK ==
                (statusRegVal & CSL_RTC_RTCINTFL_DAYFL_MASK))
    {
		CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_DAYFL, SET);
		CSL_FINST(CSL_RTC_REGS->R , RTC_RTCINTFL_DAYFL, SET);
    }
    /* check for hour interrupt */
    else if (CSL_RTC_RTCINTFL_HOURFL_MASK ==
                (statusRegVal & CSL_RTC_RTCINTFL_HOURFL_MASK))
    {
		CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_HOURFL, SET);
    }
    /* check for minute interrupt */
    else if (CSL_RTC_RTCINTFL_MINFL_MASK ==
                (statusRegVal & CSL_RTC_RTCINTFL_MINFL_MASK ))
    {
		CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_MINFL, SET);
    }
    /* check for seconds interrupt */
    else if (CSL_RTC_RTCINTFL_SECFL_MASK ==
                (statusRegVal & CSL_RTC_RTCINTFL_SECFL_MASK ))
    {
		CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_SECFL, SET);
    }
    /* check for milliseconds interrupt */
    else if (CSL_RTC_RTCINTFL_MSFL_MASK ==
                (statusRegVal & CSL_RTC_RTCINTFL_MSFL_MASK ))
    {
		CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_MSFL, SET);
    }
#endif
}

void rtc_extEvt(void)
{
    CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_EXTFL, SET);
	ext_evt_intr=1;
	printf("\r\nExternel event interrupt has been generated\r\n");
}


