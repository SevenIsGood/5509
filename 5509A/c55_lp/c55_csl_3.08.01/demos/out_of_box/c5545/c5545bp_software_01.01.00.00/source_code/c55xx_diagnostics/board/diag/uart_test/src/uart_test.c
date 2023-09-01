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

/*! \file uart_test.c
*
*   \brief Functions that initializes the UART module for writing the data
*          to the serial console and reading it back to the CCS console.
*
*/

#include "uart_test.h"

CSL_UartObj uartObject;

/* UART data buffers */
char rdbuffer[100];
char buffer1[50] = "\r\n\nEnter the string of 10 characters:\r\n >> ";
char buffer2[60] = "\r\n\nPlease stop typing, reading already stopped...!!!\r\n";

/**
 *  \brief    This function is used to perform uart test by reading a
 *            string of 10 characters in serial console and display it
 *            on the CCS console
 *
 *  \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n         TEST_PASS  - Test Passed
 * \n         TEST_FAIL  - Test Failed
 */
static TEST_STATUS uart_serial_test(void *testArgs)
{
	CSL_UartHandle    hUart;
	Int16             retVal;
	Uint16            stringSize = 10;
	Uint32            sysClk;

	CSL_UartSetup *uartSetupArgs = (CSL_UartSetup *)testArgs;

	sysClk = C55x_getSysClk();

	uartSetupArgs->clkInput = sysClk * 1000;

	retVal = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
	                         CSL_EBSR_PPMODE_1);
	if(retVal != 0)
	{
		C55x_msgWrite("SYS_setEBSR failed\n");
		return (TEST_FAIL);
	}

	/* Loop counter and error flag */
	retVal = UART_init(&uartObject,CSL_UART_INST_0,UART_POLLED);
	if(retVal != 0)
	{
		C55x_msgWrite("UART_init failed error code %d\n", retVal);
		return (TEST_FAIL);
	}

	retVal = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
			CSL_EBSR_PPMODE_1);
	if(retVal != 0)
	{
		C55x_msgWrite("SYS_setEBSR failed\n");
		return (TEST_FAIL);
	}

	/* Handle created */
	hUart = (CSL_UartHandle)(&uartObject);

	/* Configure UART registers using setup structure */
	retVal = UART_setup(hUart, uartSetupArgs);
	if(retVal != 0)
	{
		C55x_msgWrite("UART_setup failed error code %d\n", retVal);
		return (TEST_FAIL);
	}

	rdbuffer[0] = '\0';
	rdbuffer[1] = '\0';

	retVal = UART_fputs(hUart, buffer1,0);
	if(retVal != 0)
	{
		C55x_msgWrite("UART_fputs failed error code %d\n", retVal);
		return (TEST_FAIL);
	}

	retVal = UART_read(hUart,rdbuffer,stringSize,0);
	if(retVal != 0)
	{
		C55x_msgWrite("UART_read failed error code %d\n", retVal);
		return (TEST_FAIL);
	}
	else
	{
		C55x_msgWrite("\n\nMessage Sent to HyperTerminal : %s\n",rdbuffer);
	}

	retVal = UART_fputs(hUart,buffer2,0);
	if(retVal != 0)
	{
		C55x_msgWrite("UART_fputs failed error code %d\n", retVal);
		return (TEST_FAIL);
	}

	retVal = UART_fputs(hUart,"\r\nThe characters entered by you are: ",0);
	if(retVal != 0)
	{
		C55x_msgWrite("UART_fputs failed error code %d\n", retVal);
		return (TEST_FAIL);
	}

	retVal = UART_fputs(hUart,rdbuffer,0);
	if(retVal != 0)
	{
		C55x_msgWrite("UART_fputs failed error code %d\n", retVal);
		return (TEST_FAIL);
	}

	retVal = UART_fputs(hUart,"\r\n",0);
	if(retVal != 0)
	{
		C55x_msgWrite("UART_fputs failed error code %d\n", retVal);
		return (TEST_FAIL);
	}

	rdbuffer[stringSize] = '\0';

	return (TEST_PASS);

}

/**
 *  \brief    This function is used to run uart serial test
 *
 *  \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n         TEST_PASS  - Test Passed
 * \n         TEST_FAIL  - Test Failed
 */
static TEST_STATUS run_uart_test(void *testArgs)
{
	Int16 retVal;

	retVal = uart_serial_test(testArgs);
	if(retVal != 0)
	{
		return -1;
	}

	return (retVal);

}

/**
 * \brief This function performs uart test
 *
 * \param testArgs  - Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
TEST_STATUS uartTest(void *testArgs)
{
	Int16 retVal;

    C55x_msgWrite("\n***************************\n\r");
    C55x_msgWrite(  "         UART Test       \n\r");
    C55x_msgWrite(  "***************************\n\r");

    if(testArgs == NULL)
    {
    	C55x_msgWrite("Invalid Test Arguments!\n\r");
    	C55x_msgWrite("Aborting the Test!!\n\r");
        return (TEST_FAIL);
    }


    retVal = run_uart_test(testArgs);
    if(retVal != 0)
    {
    	C55x_msgWrite("\nUART Test Failed!\n\r");
    }
    else
    {
    	C55x_msgWrite("\nUART Test Passed!\n\r");
    }

    return retVal;
}
