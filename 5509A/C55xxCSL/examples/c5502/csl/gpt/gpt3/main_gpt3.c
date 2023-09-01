/*
 * Copyright (C) 2003 Texas Instruments Incorporated
 * All Rights Reserved
 */
/*
 *---------main_gpt3.c---------
 * This example demonstrates use of GP Timer to profile (benchmark) the code.
 * Several API's are provided to work with the profiler.
 * It outputs the number of cpu cycles taken by 'add vectors' function
 * with optimization and without optimization.
 */

#include <stdio.h>
#include "profiler_gpt3.h"

//---------Global constants---------

//Array size
#define BUFF_SIZE			1000

//Timer device number to be used for profiling (Use -1 for any timer device)
#define TIMER_NUM			0

//Ratio of (CPU CLK/TIMER CLK)
#define CPUCLK_BY_TIMCLK	1

//---------Global data definition---------

// The buffers passed to 'Add vectors' routine
Int16 in1[BUFF_SIZE];
Int16 in2[BUFF_SIZE];
Int16 out1[BUFF_SIZE];
Int16 out2[BUFF_SIZE];

//---------Function prototypes---------
extern void addVecs_ver1(Int16 *, Int16 *, Int16 *, Int16);
extern void addVecs_ver2(Int16 *, Int16 *, Int16 *, Int16);

void init_arrays(Int16 *, Int16 *);

//---------main routine---------
void main()
{
	Int32 ret_status;
	Uint32 cpu_cycles1 = 0, cpu_cycles2 = 0;
	Int16 i, errAddVecs = 0;
	
	//Initialise the arrays
	init_arrays(in1, in2);
	
	//Configure timer to be used for profiling
	if ((ret_status = profile_timConfig(TIMER_NUM, CPUCLK_BY_TIMCLK)) != 0)
	{
		switch(ret_status)
		{
			case ERR_INVDEVNUM :
				printf("\nERROR:Invalide timer device number input");
				break;
			case ERR_INVCLKRATIO :
				printf("\nERROR:Invalide CLK ratio input");
				break;
			case ERR_BADHANDLE :
				printf("\nERROR:Timer open failed with invalid handle");
				break;
			default:
				printf("\nERROR:Profiler configure failed");			
		}
		printf("\nTEST FAILED\n");
		exit(1);
	}
	
	//Start profiler
	profile_begin();
	
	//Call the function to be profiled
	addVecs_ver1(in1, in2, out1, BUFF_SIZE);
	
	//Stop the profiler
	cpu_cycles1 = profile_end();
	
	//Start profiler
	profile_begin();
	
	//Call the function to be profiled
	addVecs_ver2(in1, in2, out2, BUFF_SIZE);
	
	//Stop the profiler
	cpu_cycles2 = profile_end();
	
	if (cpu_cycles1 == 0  ||  cpu_cycles2 == 0)
	{
		printf("\nERROR:Timer handle is invalid, no profiling is done\n");
		printf("\nTEST FAILED\n");
		exit(1);
	}
	
	for (i = 0; i < BUFF_SIZE; i++)
	{
		if (out1[i] != out2[i])
		{
			errAddVecs = 1;
			break;
		}
	}
	//Print appropriate output
	printf("\n\t\taddVecs_ver1\taddVecs_ver2");
	printf("\n-------------------------------------------------");
	printf("\nCPU cycles :\t%ld\t\t%ld\n",cpu_cycles1, cpu_cycles2);
	
	if (errAddVecs || (cpu_cycles1 < cpu_cycles2))
		printf("\nTEST FAILED\n");
	else printf("\nTEST PASSED\n");
	
	//Close the profiler just before returning from main()
	profile_timClose();
}

//Function to initialise two arrays to be used in main processing routine
void init_arrays(Int16 *arr1, Int16 *arr2)
{
	int i;
	
	for (i = 0; i < BUFF_SIZE; i++)
	{
		arr1[i] = i + 0x1111;
		arr2[i] = 0x2222 - i;
	}
}
