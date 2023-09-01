/*
 * Copyright (C) 2003 Texas Instruments Incorporated
 * All Rights Reserved
 */
/*
 *---------main_rtc1.c---------
 * This is an example of using the RTC configuration functions for 5509
 */

#include <csl_rtc.h>
#include <stdio.h>
 
//---------Global data definition---------
int i; 
RTC_Time getTime;
RTC_Date getDate;
RTC_Config Config; 
RTC_Alarm getAlarm;

RTC_Config myConfig = {
     0x40,    /* Seconds */
     0x00,    /* Seconds Alarm */
     0x58,    /* Minutes */
     0x00,    /* Minutes Alarm */
     0x11,    /* Hour */
     0x00,    /* Hours Alarm */
     0x05,    /* Day of the week and day alarm */
     0x10,    /* Day of the month */
     0x10,    /* Month */
     0x03,    /* Year */
     0x08,    /* Register A */
     0x82,    /* Register B - 24-hour mode */
};

RTC_Date myDate = {
     0x13,     /* Year */
     0x10,     /* Month */
     0x10,     /* Daym */
     0x05,     /* Dayw */
};

RTC_Time myTime = {
     0x11,     /* Hour */      
     0x58,     /* Minutes */
     0x40,     /* Seconds */
};

RTC_Alarm myAlarm = {
     0x01,     /* alHour */  
     0x59,     /* alMinutes - every minute */
     0x03,     /* alSeconds */
     0x05,     /* alDayw */
};

//---------main routine---------
main()
{
	CSL_init();
	printf ("\nTESTING...\n");
  
	RTC_reset();    // Reset the registers
	RTC_config(&myConfig);   // Initialization  
	RTC_getConfig(&Config);        
	printf ("RTCSEC value is: %x\n",Config.rtcsec);  
	printf ("RTCMIN value is: %x\n",Config.rtcmin); 
	printf ("RTCHOUR value is: %x\n",Config.rtchour); 
	printf ("RTCDAYW value is: %x\n",Config.rtcdayw); 
	printf ("RTCDAYM value is: %x\n",Config.rtcdaym);        
	printf ("RTCYEAR value is: %x\n",Config.rtcyear);        

	RTC_setTime(&myTime);       
	RTC_getTime(&getTime);
	printf("Current RTC time is: %x:%x:%x\n",getTime.hour,getTime.minute,getTime.second);
  
	RTC_getDate(&getDate);        
	printf("Current RTC date is: %x/%x/%x\tDay of week: %x\n",getDate.month,getDate.daym,getDate.year,getDate.dayw); 
      
	/* Set an alarm at the time specified by myAlarm */         
	/* Alarm needs to be enabled to occur */
	RTC_setAlarm(&myAlarm);           
	printf("Alarm time: %x : %x : %x\tDay of week: %x\n",
	RTC_FGET(RTCHOURA,HAR),RTC_FGET(RTCMINA,MAR),
	RTC_FGET(RTCSECA,SAR),RTC_FGET(RTCDAYW,DAR));
   
	RTC_start();    // start the RTC running
}
