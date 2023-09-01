/*
 * Copyright (c) 2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*! \file rtc_test.c
*
*   \brief Functions that initializes the rtc module and read the set date and time
*          and check the generation of alarm interrupt for the set time.
*/

#include "rtc_test.h"

CSL_RtcTime	     InitTime;
CSL_RtcDate 	 InitDate;
CSL_RtcTime 	 GetTime;
CSL_RtcDate 	 GetDate;
CSL_RtcConfig    rtcConfig;
CSL_RtcConfig    rtcGetConfig;
CSL_RtcAlarm     AlarmTime;
CSL_RtcIsrAddr   isrAddr;
CSL_RtcIsrDispatchTable      rtcDispatchTable;
Uint16           secIntrCnt = 0;

/* Reference the start of the interrupt vector table */
extern void VECSTART(void);

/* Prototype declaration for ISR function */
interrupt void rtc_isr(void);

/**
 * \brief    This function used to set ,read and displays the time
 *
 * \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n         TEST_PASS  - Test Passed
 * \n         TEST_FAIL  - Test Failed
 */
static TEST_STATUS rtc_set_time_read_test(void *testArgs)
{

	TEST_STATUS   retVal;
	Uint16        iteration;
	Uint32 		  rtcTimeCount = RTC_TIME_PRINT_CYCLE;

	iteration = 1;

	C55x_msgWrite("\n\nThis test demonstrates RTC Time functionality\n\r");
	C55x_msgWrite("RTC Time will be set, read and displayed %ld times.\n\r", rtcTimeCount);
	C55x_msgWrite("RTC interrupt will be generated for each Second\n\n\r");

	/* Set the RTC config structure */
	rtcConfig.rtcyear  = 8;
	rtcConfig.rtcmonth = 8;
	rtcConfig.rtcday   = 8;
	rtcConfig.rtchour  = 8;
	rtcConfig.rtcmin   = 8;
	rtcConfig.rtcsec   = 8;
	rtcConfig.rtcmSec  = 8;

	rtcConfig.rtcintcr  = 0x803F;

	/* Set the RTC init structure */
	InitDate.year  = 8;
	InitDate.month = 10;
	InitDate.day   = 16;

	InitTime.hours = 12;
	InitTime.mins  = 12;
	InitTime.secs  = 12;
	InitTime.mSecs = 12;

	/* Register the ISR function */
	isrAddr.MilEvtAddr    = rtc_msIntc;
	isrAddr.SecEvtAddr    = rtc_secIntc;
	isrAddr.MinEvtAddr    = rtc_minIntc;
	isrAddr.HourEvtAddr   = rtc_hourIntc;
	isrAddr.DayEvtAddr    = rtc_dayIntc;
	isrAddr.ExtEvtAddr    = rtc_extEvt;
	isrAddr.AlarmEvtAddr  = rtc_alarmEvt;

	retVal = RTC_setCallback(&rtcDispatchTable, &isrAddr);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("RTC_setCallback Failed\n\r");
		return (retVal);
	}
	else
	{
		C55x_msgWrite("RTC_setCallback Successful\n\r");
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
	retVal = RTC_config(&rtcConfig);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("RTC_config Failed\n\r");
		return (retVal);
	}

	/* Read the configuration values from the RTC module */
	retVal = RTC_getConfig(&rtcGetConfig);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("RTC_getConfig Failed\n\r");
		return (retVal);
	}

	/* Set the RTC time */
	retVal = RTC_setTime(&InitTime);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("RTC_setTime Failed\n\r");
		return (retVal);
	}

	C55x_msgWrite("Setting RTC Time Successful\n\r");

	/* Set the RTC date */
	retVal = RTC_setDate(&InitDate);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("RTC_setDate Failed\n\r");
		return (retVal);
	}

	C55x_msgWrite("Setting RTC Date Successful\n\r");

	/* Set the RTC interrupts */
	retVal = RTC_setPeriodicInterval(CSL_RTC_MINS_PERIODIC_INTERRUPT);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("RTC_setPeriodicInterval Failed\n\r");
		return (retVal);
	}

	/* Enable the RTC SEC interrupts */
	retVal = RTC_eventEnable(CSL_RTC_SECEVENT_INTERRUPT);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("RTC_eventEnable for SEC EVENT Failed\n\r");
		return (retVal);
	}

	C55x_msgWrite("Setting RTC Events Successful\n\r");

	C55x_msgWrite("\nStarting the RTC\n\n\r");
	/* Start the RTC */
	RTC_start();

	/* This loop will display the RTC time for 255 times */
	while(rtcTimeCount--)
	{
		retVal = RTC_getTime(&GetTime);
		if(retVal != CSL_SOK)
		{
			C55x_msgWrite("RTC_getTime Failed\n\r");
			return (retVal);
		}

		retVal = RTC_getDate(&GetDate);
		if(retVal != CSL_SOK)
		{
			C55x_msgWrite("RTC_getDate Failed\n\r");
			return (retVal);
		}

		C55x_msgWrite("Iteration %d: ",iteration++);

		C55x_msgWrite("Time and Date is : %02d:%02d:%02d:%04d, "
				"%02d-%02d-%02d\n\r", GetTime.hours,GetTime.mins,GetTime.secs,
				GetTime.mSecs,GetDate.day,GetDate.month,GetDate.year);
	}

	IRQ_globalDisable();

	/* Clear any pending interrupts */
	IRQ_clearAll();

	/* Disable all the interrupts */
	IRQ_disableAll();

	/* Stop the RTC */
	RTC_stop();

    return (TEST_PASS);

}

/**
 * \brief    This function is used to generating an alarm interrupt
 *   		  at a particular time
 *
 * \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n         TEST_PASS  - Test Passed
 * \n         TEST_FAIL  - Test Failed
 */
static TEST_STATUS rtc_alarm_interrupt_test(void *testArgs)
{
	TEST_STATUS  retVal;

	C55x_msgWrite("\n\nRTC Alarm Interrupt Test\n\r");
	C55x_msgWrite("This test demonstrates RTC Alarm functionality\n\r");
	C55x_msgWrite("RTC interrupt will be generated for each Second\n\r");
	C55x_msgWrite("RTC ALARM interrupt will be generated at "
			      "Time 12:12:14:512\n\n\r");

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

	rtcConfig.rtcintcr  = 0x803F;

	/* Set the RTC init structure */
	InitDate.year  = 8;
	InitDate.month = 10;
	InitDate.day   = 16;

	InitTime.hours = 12;
	InitTime.mins  = 12;
	InitTime.secs  = 12;
	InitTime.mSecs = 12;

	/* Set the RTC alarm time */
	AlarmTime.year  = 8;
	AlarmTime.month = 10;
	AlarmTime.day   = 16;
	AlarmTime.hours = 12;
	AlarmTime.mins  = 12;
	AlarmTime.secs  = 14;
	AlarmTime.mSecs = 512;

	/* Register the ISR function */
	isrAddr.MilEvtAddr    = rtc_msIntc;
	isrAddr.SecEvtAddr    = rtc_secIntc;
	isrAddr.MinEvtAddr    = rtc_minIntc;
	isrAddr.HourEvtAddr   = rtc_hourIntc;
	isrAddr.DayEvtAddr    = rtc_dayIntc;
	isrAddr.ExtEvtAddr    = rtc_extEvt;
	isrAddr.AlarmEvtAddr  = rtc_alarmEvt;

	retVal = RTC_setCallback(&rtcDispatchTable, &isrAddr);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("RTC_setCallback Failed\n\r");
		return retVal;
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
	retVal = RTC_config(&rtcConfig);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("RTC_config Failed\n\r");
		return (retVal);
	}

	/* Read the configuration values from the RTC module */
	retVal = RTC_getConfig(&rtcGetConfig);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("RTC_getConfig Failed\n\r");
		return (retVal);
	}

	/* Set the RTC time */
	retVal = RTC_setTime(&InitTime);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("RTC_setTime Failed\n\r");
		return (retVal);
	}

	/* Set the RTC date */
	retVal = RTC_setDate(&InitDate);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("RTC_setDate Failed\n\r");
		return (retVal);
	}

	/* Set the RTC Alarm time */
	retVal = RTC_setAlarm(&AlarmTime);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("RTC_setAlarm Failed\n\r");
		return (retVal);
	}

	C55x_msgWrite("RTC Time and Alarm Time Configured Successfully\n\r");

	/* Set the RTC interrupts */
	retVal = RTC_setPeriodicInterval(CSL_RTC_MINS_PERIODIC_INTERRUPT);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("RTC_setPeriodicInterval Failed\n\r");
		return (retVal);
	}

	/* Enable the RTC alarm interrupts */
	retVal = RTC_eventEnable(CSL_RTC_ALARM_INTERRUPT);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("RTC_eventEnable for ALARM EVENT Failed\n\r");
		return (retVal);
	}

	C55x_msgWrite("RTC Events Enabled Successfully\n\r");

	C55x_msgWrite("\nStarting the RTC\n\n\r");
	/* Start the RTC */
	RTC_start();

	Uint16 count = 10;
	while(count--)
	{
	    RTC_getTime(&GetTime);

	    RTC_getDate(&GetDate);

	    C55x_msgWrite("The current Time and Date is : %02d:%02d:%02d:%04d,"
	    		     " %02d-%02d-%02d\n\r", GetTime.hours,GetTime.mins,
					  GetTime.secs,GetTime.mSecs,GetDate.day,GetDate.month,
					  GetDate.year);

	    C55x_delay_msec(1000);
	}

	IRQ_globalDisable();

	/* Clear any pending interrupts */
	IRQ_clearAll();

	/* Disable all the interrupts */
	IRQ_disableAll();

	/* Stop the RTC */
	RTC_stop();

	return retVal;

}

/**
 * \brief    This function is used to execute while the interrupt is triggered
 *
 * \param    void
 *
 * \return   void
 */
interrupt void rtc_isr(void)
{

    CSL_RTCEventType rtcEventType;

    rtcEventType = RTC_getEventId();

    if (((void (*)(void))(rtcDispatchTable.isr[rtcEventType])))
     {
         ((void (*)(void))(rtcDispatchTable.isr[rtcEventType]))();
     }
}

/**
 * \brief    This function acts as millisecond interrupt service routine
 *
 * \param    void
 *
 * \return   void
 */
void rtc_msIntc(void)
{
    CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_MSFL, SET);
}

/**
 * \brief    This function acts as second interrupt service routine
 *
 * \param    void
 *
 * \return   void
 */
void rtc_secIntc(void)
{
    CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_SECFL, SET);
	secIntrCnt++;
	C55x_msgWrite("\nRTC Sec Interrupt %d\n\n\r",secIntrCnt);
}

/**
 * \brief    This function acts as minnute interrupt service routine
 *
 * \param    void
 *
 * \return   void
 */
void rtc_minIntc(void)
{
    CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_MINFL, SET);
}

/**
 * \brief    This function acts as minnute interrupt service routine
 *
 * \param    void
 *
 * \return   void
 */
void rtc_hourIntc(void)
{
    CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_HOURFL, SET);
}

/**
 * \brief    This function acts as day interrupt service routine
 *
 * \param    void
 *
 * \return   void
 */
void rtc_dayIntc(void)
{
    CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_DAYFL, SET);
}

/**
 * \brief    This function acts as External interrupt service routine
 *
 * \param    void
 *
 * \return   void
 */
void rtc_extEvt(void)
{
    CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_EXTFL, SET);
}

/**
 * \brief    This function acts as Alarm interrupt service routine
 *
 * \param    void
 *
 * \return   void
 */
void rtc_alarmEvt(void)
{
    CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_ALARMFL, SET);

    RTC_getTime(&GetTime);

    RTC_getDate(&GetDate);

	C55x_msgWrite("The Alarm interrupt is triggered, Please check weather the\n\r"
			      "below displayed date and time matches with the the alarm\n\r"
			      "time set\n\r");

    C55x_msgWrite("Time and Date is : %02d:%02d:%02d:%04d, %02d-%02d-%02d\n\r",
    		GetTime.hours,GetTime.mins,GetTime.secs,GetTime.mSecs,GetDate.day,
			GetDate.month,GetDate.year);


}

/**
 * \brief    This function is used to perform rtc test by setting time
 *            and generating an alarm interrupt at a particular time
 *
 * \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n         TEST_PASS  - Test Passed
 * \n         TEST_FAIL  - Test Failed
 */
static TEST_STATUS run_rtc_test(void *testArgs)
{
	Int16 retVal;

	C55x_msgWrite("\nRTC Set Time Test...\n\r");
	retVal = rtc_set_time_read_test(testArgs);
	if(retVal != 0)
	{
		C55x_msgWrite("\nRTC Set Time Test Failed!!\n\r");
		return(retVal);
	}
	else
	{
		C55x_msgWrite("\nRTC Set Time Test Passed!!\n\r");
	}

#if 0
    retVal = rtc_alarm_interrupt_test(testArgs);
	if(retVal != 0)
	{
		C55x_msgWrite("\nRTC Alarm Interrupt Test Failed!!\n\r");
		return (retVal);
	}
	else
	{
		C55x_msgWrite("\nRTC Alarm Interrupt Test Passed!!\n\r");
	}
#endif

    return (TEST_PASS);

}

 /**
  * \brief	This function performs RTC test
  *
  * \param    testArgs   [IN]   Test arguments
  *
  * \return
  * \n      TEST_PASS  - Test Passed
  * \n      TEST_FAIL  - Test Failed
  *
  */
 TEST_STATUS rtcTest(void *testArgs)
 {
     Int16 retVal;

     C55x_msgWrite("\n**************************\n\r");
     C55x_msgWrite(  "         RTC Test       \n\r");
     C55x_msgWrite(  "**************************\n\r");

     retVal = run_rtc_test(testArgs);
     if(retVal != TEST_PASS)
     {
     	C55x_msgWrite("\nRTC Test Failed!\n\r");
     	return (TEST_FAIL);
     }
     else
     {
     	//C55x_msgWrite("\nRTC Test Passed!\n\r");
     }

     C55x_msgWrite("\nRTC Test Completed!\n\r");

     return (TEST_PASS);

}
