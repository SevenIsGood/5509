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

/** @file CSL_PowerMgmt_IDLE2_Uart_Example.c
 *
 *  @brief Example to exhibit UART TXN IN IDLE 2 state and recovery using RTC alarm interrupt
 *
 * \page    page23 POWER EXAMPLE DOCUMENTATION
 *
 * \section POWER7   POWER EXAMPLE7 - UART IDLE2/RECOVERY TEST
 *
 * \subsection POWER7x    TEST DESCRIPTION:
 *         This test configures RTC for generating an alarm 8s after it's
 * started, during which period the system is put into IDLE2 state.
 * During the same time, an UART transaction is also
 * initiated. The expected RTC alarm interrupt should bring the CPU out
 * of the IDLE2 state and resume execution from where it was before it
 * idled out. Also, the UART transaction should have gone through.
 * 
 * \subsection POWER7y    TEST PROCEDURE AND EXPECTED RESULT:
 *  1. Manual test with Pin2/3, Pin7/8 on DB-9 connected, power measured on JP1
 *  2. Run the test. The RTC alarm (expected after approx. 30s) would be set.
 *  3. Test will stop once usb clk is shut off.
 *  4. Pause and Resume the test.
 *  5. The test will also configure UART and initiate a txfer .
 *  6. While CPU waits UART transfers, the test will execute the idle instsrn that will idle the cpu and wait for the alarm.
 *  7. Observe if the alarm interrupt got the CPU out of idle state and the UART transaction goes through.
 *
*/
/* ============================================================================
 * Revision History
 * ================
 * 15-Apr-2015 Created
 * 10-Mar-2016 Updates to header
 * ============================================================================
 */


#include <stdio.h>
#include <csl_rtc.h>
#include <csl_intc.h>
#include <csl_general.h>

#define EVM5515 0x1

#define CSL_TEST_FAILED         (1U)
#define CSL_TEST_PASSED         (0)

#define RTC_TIME_PRINT_CYCLE    (0xFFu)
#define RTC_CALL_BACK           (1u)

CSL_RtcTime	     InitTime;
CSL_RtcDate 	 InitDate;
CSL_RtcTime 	 GetTime;
CSL_RtcDate 	 GetDate;
CSL_RtcConfig    rtcConfig;
CSL_RtcConfig    rtcGetConfig;
CSL_RtcAlarm     AlarmTime;
CSL_RtcIsrAddr   isrAddr;
CSL_RtcIsrDispatchTable      rtcDispatchTable;
volatile Uint32 rtcTimeCount = RTC_TIME_PRINT_CYCLE;
volatile Uint16    alarm_event_intr = 0;

/* Reference the start of the interrupt vector table */
extern void VECSTART(void);
/* Prototype declaration for ISR function */
interrupt void rtc_isr(void);

void rtc_msIntc(void);
void rtc_secIntc(void);
void rtc_minIntc(void);
void rtc_hourIntc(void);
void rtc_dayIntc(void);
void rtc_extEvt(void);
void rtc_alarmEvt(void);

//extern CSL_Status uart_PollSample(void);
CSL_Status rtc_test(void);

CSL_Status uart_PollSample(void);

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

	printf("CSL RTC TESTS\n\n");
	printf("This test demonstrates RTC ALARM interrupt generation capability\r\n");
	
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
	#elif EVM5515
#if (defined(CHIP_C5517))
		 CSL_SYSCTRL_REGS->RSCR = 0x0001;
		 CSL_RTC_REGS->RGKR_LSW = 0xF1E0;
		 CSL_RTC_REGS->RGKR_MSW = 0x95A4;
#endif
	#else
		#warn "Define either EVM5515 or PHOENIX_QT"
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

	//rtcConfig.rtcintcr  = 0x803F; //Enabled all the isr for the RTC
	rtcConfig.rtcintcr  = 0x8000; //Enabled only RTC alarm interrupt

	/* Set the RTC init structure */
    InitDate.year  = 2013;
    InitDate.month = 12;
    InitDate.day   = 3;

    InitTime.hours = 12;
    InitTime.mins  = 12;
    InitTime.secs  = 12;
    InitTime.mSecs = 12;

	/* Set the RTC alarm time */
    AlarmTime.year  = 2013;
    AlarmTime.month = 12;
    AlarmTime.day   = 3;
    AlarmTime.hours = 12;
    AlarmTime.mins  = 12;
    AlarmTime.secs  = 30;
    AlarmTime.mSecs = 512;


    /* Register the ISR function */
    isrAddr.AlarmEvtAddr  = rtc_alarmEvt;

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

	/* Set the RTC Alarm time */
	status = RTC_setAlarm(&AlarmTime);
	if(status != CSL_SOK)
	{
		printf("RTC_setAlarm Failed\n");
		return(result);
	}
	else
	{
		printf("RTC_setAlarm Successful\n");
	}
	
	/* Enable the RTC alarm interrupts */
	status = RTC_eventEnable(CSL_RTC_ALARM_INTERRUPT);
	if(status != CSL_SOK)
	{
		printf("RTC_eventEnable for ALARM EVENT Failed\n");
		return(result);
	}
	else
	{
		printf("RTC_eventEnable for ALARM EVENT Successful\n");
	}

	printf("\nStarting the RTC..alarm will be produced on 12:12:40:0512, 3-12-13 ....Have a sleep \n\n");
	/* Start the RTC */
	RTC_start();

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

#if 1

    printf("\nDisabling USB Clk domain\n");
    CSL_USB_REGS->FADDR_POWER |= CSL_USB_FADDR_POWER_SUSPENDM_MASK;
#if (defined(CHIP_C5517))
    CSL_SYSCTRL_REGS->CLKSTOP1 |= CSL_SYS_CLKSTOP1_USBCLKSTPREQ_MASK;
    while(!(CSL_SYSCTRL_REGS->CLKSTOP1 &
    		CSL_SYS_CLKSTOP1_USBCLKSTPACK_MASK));
    CSL_SYSCTRL_REGS->PCGCR2 |= CSL_SYS_PCGCR2_USBCG_MASK;
    CSL_SYSCTRL_REGS->USBSCR |= CSL_SYS_USBSCR_USBOSCDIS_MASK;

    CSL_SYSCTRL_REGS->CLKSTOP1 |=
    		(CSL_SYS_CLKSTOP1_UHPICLKSTPREQ_MASK |
    		CSL_SYS_CLKSTOP1_MBPCLKSTPREQ_MASK |
    		//CSL_SYS_CLKSTOP1_URTCLKSTPREQ_MASK |
    		CSL_SYS_CLKSTOP1_EMIFCLKSTPREQ_MASK);
    while(!(CSL_SYSCTRL_REGS->CLKSTOP1 &
    			  (CSL_SYS_CLKSTOP1_UHPICLKSTPACK_MASK |
    				CSL_SYS_CLKSTOP1_MBPCLKSTPACK_MASK |
    				//CSL_SYS_CLKSTOP1_URTCLKSTPACK_MASK |
    				CSL_SYS_CLKSTOP1_USBCLKSTPACK_MASK |
    				CSL_SYS_CLKSTOP1_EMIFCLKSTPACK_MASK)));


    CSL_SYSCTRL_REGS->CLKSTOP2 |=
    		(CSL_SYS_CLKSTOP2_MSPBRIDGECLKSTPREQ_MASK |
    				CSL_SYS_CLKSTOP2_MSPCLKSTPREQ_MASK);
    while(!(CSL_SYSCTRL_REGS->CLKSTOP2 &
    		(CSL_SYS_CLKSTOP2_MSPBRIDGECLKSTPACK_MASK |
    				CSL_SYS_CLKSTOP2_MSPCLKSTPACK_MASK)));
#else
#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined(CHIP_C5535) || defined(CHIP_C5545))

    CSL_SYSCTRL_REGS->CLKSTOP |= CSL_SYS_CLKSTOP_USBCLKSTPREQ_MASK;
    while(!(CSL_SYSCTRL_REGS->CLKSTOP &
    		CSL_SYS_CLKSTOP_USBCLKSTPACK_MASK));
    CSL_SYSCTRL_REGS->PCGCR2 |= CSL_SYS_PCGCR2_USBCG_MASK;
    CSL_SYSCTRL_REGS->USBSCR |= CSL_SYS_USBSCR_USBOSCDIS_MASK;

    CSL_SYSCTRL_REGS->CLKSTOP |=
    		(//CSL_SYS_CLKSTOP_URTCLKSTPREQ_MASK |
    		CSL_SYS_CLKSTOP_EMFCLKSTPREQ_MASK);
    while(!(CSL_SYSCTRL_REGS->CLKSTOP &
    			  (//CSL_SYS_CLKSTOP_URTCLKSTPACK_MASK |
    				//CSL_SYS_CLKSTOP_USBCLKSTPACK_MASK |
    				CSL_SYS_CLKSTOP_EMFCLKSTPACK_MASK)));

#endif
#endif
    printf("\nDisabling Clk to all preipherals except"
    		" Sysclk to clk generator\n");
    CSL_SYSCTRL_REGS->PCGCR1 = 0x0;
    CSL_SYSCTRL_REGS->PCGCR2 = 0x0;
    CSL_SYSCTRL_REGS->PCGCR1 |= 0x7FFB;//0x3EF7;//0x7FFF;
    CSL_SYSCTRL_REGS->PCGCR2 |= 0xFF;

    CSL_CPU_REGS->IFR0 |= 0xFBFC;
    CSL_CPU_REGS->IFR1 |= 0x07FF;

    CSL_RTC_REGS->RTCINTREG |= CSL_RTC_RTCINTREG_EXTINTEN_MASK;

#if (defined(CHIP_C5517))
	 CSL_SYSCTRL_REGS->RSCR = 0x0001;
	 CSL_RTC_REGS->RGKR_LSW = 0xF1E0;
	 CSL_RTC_REGS->RGKR_MSW = 0x95A4;
#endif

		printf("CSL UART POLL TEST!\n\n");

		status = uart_PollSample();

		if(status != CSL_TEST_PASSED)
		{
			printf("\nCSL UART POLL TXN FAILED!!\n\n");
	   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
	   /////  Reseting PaSs_StAtE to 0 if error detected here.
	       // PaSs_StAtE = 0x0000; // Was initialized to 1 at declaration.
	   /////
		}
		else
		{
			printf("\nCSL UART POLL PASSED!\n\n");
		}



#endif

	while(!alarm_event_intr); //wait for the alarm interrupt

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

void rtc_alarmEvt(void)
{
    CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_ALARMFL, SET);
	alarm_event_intr=1;
	printf("\nRTC Alarm Interrupt has been generated\n\n");
}
