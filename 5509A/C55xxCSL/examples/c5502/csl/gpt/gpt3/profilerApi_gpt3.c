/*
 * Copyright (C) 2003 Texas Instruments Incorporated
 * All Rights Reserved
 */
/*
 *---------profilerApi_gpt3.c---------
 * This file defines all the API's of the profiler.
 */
#include "profiler_gpt3.h"
#include <stdio.h>

GPT_Handle hGpt;
Uint16 cpuByGptClk;
Uint16 timIdx;

//Config structure setup for GPTx in DUAL-32 bit un-chained mode
GPT_Config myGptCfg = 
{
	0,
	GPT_GPTGPINT_RMK(
		GPT_GPTGPINT_TIN1INV_DEFAULT,
		GPT_GPTGPINT_TIN1INT_DEFAULT
	),
	GPT_GPTGPEN_RMK(
		GPT_GPTGPEN_TOUT1EN_DEFAULT,
		GPT_GPTGPEN_TIN1EN_DEFAULT	
	),
	GPT_GPTGPDIR_RMK(
		GPT_GPTGPDIR_TOUT1DIR_DEFAULT,
		GPT_GPTGPDIR_TIN1DIR_DEFAULT
	),
	GPT_GPTGPDAT_RMK(
		GPT_GPTGPDAT_TOUT1DAT_DEFAULT,
		GPT_GPTGPDAT_TIN1DAT_DEFAULT
	),
	0xFFFF,	//PRD1
	0xFFFF,	//PRD2
	0xFFFF,	//PRD3
	0xFFFF,	//PRD4
	GPT_GPTCTL1_RMK(
		GPT_GPTCTL1_TIEN_DEFAULT,
		GPT_GPTCTL1_CLKSRC_DEFAULT,
		GPT_GPTCTL1_ENAMODE_ONCE,
		GPT_GPTCTL1_PWID_DEFAULT,
		GPT_GPTCTL1_CP_DEFAULT,
		GPT_GPTCTL1_INVIN_DEFAULT,
		GPT_GPTCTL1_INVOUT_DEFAULT			
	),
	GPT_GPTCTL2_RMK(
		GPT_GPTCTL2_TIEN_DEFAULT,
		GPT_GPTCTL2_CLKSRC_DEFAULT,
		GPT_GPTCTL2_ENAMODE_ONCE,
		GPT_GPTCTL2_PWID_DEFAULT,
		GPT_GPTCTL2_CP_DEFAULT,
		GPT_GPTCTL2_INVIN_DEFAULT,
		GPT_GPTCTL2_INVOUT_DEFAULT			
	),
	GPT_GPTGCTL1_RMK(
		GPT_GPTGCTL1_TDDR34_DEFAULT,
		GPT_GPTGCTL1_PSC34_DEFAULT,
		GPT_GPTGCTL1_TIMMODE_32BIT_DUAL,
		GPT_GPTGCTL1_TIM34RS_IN_RESET,
		GPT_GPTGCTL1_TIM12RS_IN_RESET
	)		
};

//Function to configure timer for profiler usage
Uint16 profile_timConfig(Int16 timDeviceNum, Uint16 clkRatio)
{	
	Uint16 gptClkRatio;
	
	CSL_init();
	
	//Return with invalid device number error if it is not
	//  in -1 to 1 range
	if (timDeviceNum < GPT_DEVANY || timDeviceNum > GPT_DEV1)
		return ERR_INVDEVNUM;
	
	if (timDeviceNum == GPT_DEV0)	timIdx = 0;
	else if (timDeviceNum == GPT_DEV1)	timIdx = 1;
	
	//
	gptClkRatio = GPT_RGETH(hGpt, GPTCLK);
	if (gptClkRatio == 0) gptClkRatio = 1;
	if (clkRatio != gptClkRatio)
		return ERR_INVCLKRATIO;
	
	//Store the ratio to use later
	cpuByGptClk = clkRatio;
	
	//Open the appropriate timer device
	hGpt = GPT_open(timDeviceNum, GPT_OPEN_RESET);
	
	//Return bad handle error if open fails
	if (hGpt == INV)
		return ERR_BADHANDLE;
	
	return 0;	
}

//Function to be used just before the code section to be profiled.
//It starts the profiler operation.
void profile_begin(void)
{
	//If open failed earlier, return without doing anything
	if (hGpt == INV)
		return;
	
	//Reset GPTx counters
	GPT_RSETH(hGpt,GPTCNT1, 0x0000);
	GPT_RSETH(hGpt,GPTCNT2, 0x0000);
	GPT_RSETH(hGpt,GPTCNT3, 0x0000);
	GPT_RSETH(hGpt,GPTCNT4, 0x0000);

	GPT_config(hGpt, &myGptCfg);
	
	//Start the timer to begin profiling
	if (timIdx == 0)	GPT_start12(hGpt);
	else if (timIdx == 1) GPT_start34(hGpt);
}

//Function to be used immediately after the code section to be profiled.
//It stops the profiler operation.
Uint32 profile_end(void)
{
	Uint32 tim12, tim34, retVal;
	Uint32 *tim12a = (Uint32 *)&tim12;
	Uint32 *tim34a = (Uint32 *)&tim34;
	
	if (hGpt != INV)
	{
		//Pause the timer to get the accurate count now
		if (timIdx == 0)	GPT_stop12(hGpt);
		else if (timIdx == 1) GPT_stop34(hGpt);
				
		//Get this count and multiply by the ratio of CPU to GPT CLK
		//  to return actual CPU cycles taken
		GPT_getCnt(hGpt, tim34a, tim12a);
		
		
		if (timIdx == 0)	retVal = ((Uint32)((*tim12a) * cpuByGptClk));
		else if (timIdx == 1)	retVal = ((Uint32)((*tim34a) * cpuByGptClk));
		
		return retVal;
	}
	return 0;	
}

//Function to close the profiler after its usage. To be used before exiting
//  the user program.
void profile_timClose(void)
{
	//If the handle is valid, close the timer device
	if (hGpt != INV)
		GPT_close(hGpt);
}
