/*
 * Copyright (C) 2003 Texas Instruments Incorporated
 * All Rights Reserved
 */
/*
 *---------addV2_gpt3.c---------
 * This is a simple routine that performs addition of two 16 bit vectors
 */

#include <csl.h>

void addVecs_ver2(Int16 *in1, Int16 *in2, Int16 *out, Int16 len)
{
	Int16 i;
	
	for (i = 0; i < len; i++)
		out[i] = in1[i] + in2[i];
		
	return;
}
