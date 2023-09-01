/*
 * Copyright (C) 2003 Texas Instruments Incorporated
 * All Rights Reserved
 */
/*
 *---------main_rtc2.c---------
 * This is an example of using the RTC ANSI-style time functions for 5509
 */
#include <csl_rtc.h> 
#include <stdio.h>   
#include <csl_time.h>    
 
//---------main routine---------
main()
{
	time_t rtcTime;     
	time_t *timer = NULL;     
	//size_t sz;       
	size_t maxsz = 40;
	char *outTime = "";
	struct tm* realTime; 
	time_t* lcl;    
	
	CSL_init();
	printf ("\nTESTING...\n");
  
	/* Get the current calendar time and date from RTC */
	rtcTime = RTC_time(timer);  
	printf("RTC raw time: %lu\n", rtcTime);   
	 
	/* Convert calendar time to local time */
	lcl = &rtcTime;   
	realTime = RTC_localtime(lcl); 
	
	/* Format the time into a character string */
	RTC_strftime(outTime, maxsz, "%c", realTime);        
	printf("%s\n", outTime);  
}
