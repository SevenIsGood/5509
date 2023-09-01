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
 
/*! \file system_init.c
*
*   \brief This file contains the implemention of system level initialization
*    functions
*
*/

#include "platform_test.h"
#include "platform.h"
#include "stdio.h"

/**
 *  \brief    Initializes user interface
 *
 *  User input for the diagnostic tests can be taken from
 *  serial console or CCS console. This function configures
 *  the user interface based on the selected input.
 *
 *  Below are settings needs to be used on host machine in case
 *  of serial console user interface
 *   Baud Rate    - 115200
 *   Data width   - 8 bit
 *   Parity       - None
 *   Stop Bits    - 1
 *   Flow Control - None
 *
 *  \return    - Platform_EOK on Success or error code
 */
static Platform_STATUS initUserIf(void)
{
	Platform_STATUS status = Platform_EOK;

#ifdef USE_SERIAL_CONSOLE
	/* Set output console to serial port */
	platformWriteConfigure(PLATFORM_WRITE_UART);
	platformReadConfigure(PLATFORM_READ_UART);
#else
	/* Set output console to CCS */
	platformWriteConfigure(PLATFORM_WRITE_PRINTF);
	platformReadConfigure(PLATFORM_READ_SCANF);
#endif

	return (status);
}

/**
 *  \brief    Initializes platform test modules
 *
 *  \return   Platform_EOK on Success or error code
 */
Platform_STATUS initPlatform(void)
{
	Platform_STATUS status = 0;

	/* Initialize user interface module */
    status = initUserIf();
    if(status != Platform_EOK)
    {
    	printf("User Interface Init Failed\n");
    	return (status);
    }

    return (status);
}



