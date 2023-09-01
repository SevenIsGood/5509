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
 
/*! \file platform_test.h
*
*   \brief Platform test header file to define the common data structures
*   used by the test functions.
*
*/ 

#ifndef _PLATFORM_TEST_H_
#define _PLATFORM_TEST_H_

#include "stdint.h"
#include "stdlib.h"

#define PLATFORM_TEST_AUTO_ALL     (0)
#define PLATFORM_TEST_START        (1)
#define PLATFORM_AUTO_TEST_COUNT   (3)
#define PLATFORM_TEST_COUNT        (4)

/* Platform test return codes */
#define TEST_PASS     (0)
#define TEST_FAIL     (-1)

typedef int32_t Platform_STATUS;
int32_t diag_main (void);

/**
 *  \brief    Initializes platform test modules
 *
 *  \return    - Platform_EOK on Success or error code
 */
Platform_STATUS initPlatform(void);

typedef struct diagTestTable_s
{
	const char *testName;
	int16_t (*testFunction)(void *);
	void  *args;
	uint8_t  pass;
	uint8_t fail;
}diagTestTable_t;
#endif
