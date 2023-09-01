/*
 * Copyright (C) 2003 Texas Instruments Incorporated
 * All Rights Reserved
 */
/*
 *---------profiler_timer2.h---------
 * This file list all the symbols and API's provided by the profiler
 * to the users.
 */
#include <csl.h>
#include <csl_gpt.h>

//---------Error codes for the profiler functions---------

//Invalide timer device number
#define ERR_INVDEVNUM		0x00000001

//Invalid cpu by timer clock ratio
#define ERR_INVCLKRATIO		0x00000002

//Timer could not be opened because of invalid handle returned
#define ERR_BADHANDLE		0x00000003

//---------User API's to use 'Profiler' ---------

//Function to configure timer for profiler usage
//  Arg1  : Int16 timDeviceNum : Which timer device to use for profiling
//  Arg2  : Uint16 clkRatio     : CPU CLK by TIMER CLK ratio
//
//  Return: Uint16				 : 0   - Success
//                           : > 0 - Error in configuring timer 
extern Uint16 profile_timConfig(Int16 timDeviceNum, Uint16 clkRatio);

//Function to be used just before the code section to be profiled.
//It starts the profiler operation.
extern void profile_begin(void);

//Function to be used immediately after the code section to be profiled.
//It stops the profiler operation.
Uint32 profile_end(void);

//Function to close the profiler after its usage. To be used before exiting
//  the user program.
void profile_timClose(void);
