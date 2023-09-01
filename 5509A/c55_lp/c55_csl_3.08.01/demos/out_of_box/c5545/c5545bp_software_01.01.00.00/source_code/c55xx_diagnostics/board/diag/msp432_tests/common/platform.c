/*
 * -------------------------------------------
 *    MSP432 DriverLib - v3_10_00_09
 * -------------------------------------------
 *
 * --COPYRIGHT--,BSD,BSD
 * Copyright (c) 2014, Texas Instruments Incorporated
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
 * --/COPYRIGHT--*/
/******************************************************************************
 * MSP432 UART - PC Echo with 12MHz BRCLK
 *
 * Description: This demo echoes back characters received via a PC serial port.
 * SMCLK/DCO is used as a clock source and the device is put in LPM0
 * The auto-clock enable feature is used by the eUSCI and SMCLK is turned off
 * when the UART is idle and turned on when a receive edge is detected.
 * Note that level shifter hardware is needed to shift between RS232 and MSP
 * voltage levels.
 *
 *               MSP432P401
 *             -----------------
 *            |                 |
 *            |                 |
 *            |                 |
 *       RST -|     P1.3/UCA0TXD|----> PC (echo)
 *            |                 |
 *            |                 |
 *            |     P1.2/UCA0RXD|<---- PC
 *            |                 |
 *
 * Author: Timothy Logan
*******************************************************************************/
/* DriverLib Includes */
#include "driverlib.h"
/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include "string.h"
#include "stdio.h"
#include "platform.h"

static WRITE_info	write_type;
static READ_info	read_type = PLATFORM_READ_SCANF;

/**
 * \brief    This function used to generate time delay in milliseconds
 *
 * \param    numOfmsec - number of milliseconds
 *
 * \return   void
 */
void delay_msec(int numOfmsec)
{
	volatile uint16_t delay;

	while(numOfmsec)
	{
		for(delay = 0; delay < 6250; delay++ );
		numOfmsec--;
	}
}

/**
 *
 * \brief This function Connfigures the input source for C55x_msgWrite
 *
 * \param    write_type - writes to uart console or ccs console
 *
 * \return
 * \n      0 - echo to UART
 * \n      1 - printf
 */
WRITE_info platformWriteConfigure (WRITE_info	wtype)
{
	WRITE_info original;
	original = write_type;
	write_type = wtype;

	return original;

}

/**
 *
 * \brief This function Connfigures the input source for C55x_msgRead
 *
 * \param    rdype - read from uart console or ccs console
 *
 * \return
 * \n      0 - echo to UART
 * \n      1 - printf
 */
READ_info platformReadConfigure (READ_info	rdype)
{
	READ_info original;
	original  = read_type;
	read_type = rdype;

	return original;
}

/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 9600 baud rate. These
 * values were calculated using the online calculator that TI provides
 * at:
 *http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
const eUSCI_UART_Config uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        78,                                     // BRDIV = 78
        2,                                       // UCxBRF = 2
        0,                                       // UCxBRS = 0
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_LSB_FIRST,                  // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling
};

#define MAX_WRITE_LEN (200)

static char	write_buffer[MAX_WRITE_LEN];

/**
 *
 * \brief This function perform's platform_write or echo to UART
 *
 * \param    fmt - character pointer
 *
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 */
int32_t platform_write(const char *fmt, ...)
{

	va_list   arg_ptr;
	uint32_t  length;
    char ch;
    uint8_t index = 0;

    /* Initial platform_write to temporary buffer.. at least try some sort of sanity check so we don't write all over
	 * memory if the print is too large.
	 */
	if (strlen(fmt) > MAX_WRITE_LEN)
	{
		fflush(stdout);
		platform_write("Exceed's the max write length\n");
		return (-1);
	}

	va_start( arg_ptr, fmt );
	length = vsprintf( (char *)write_buffer, fmt, arg_ptr );
	va_end( arg_ptr );
	length = length;

	delay_msec(1);

	if ((write_type == PLATFORM_WRITE_UART) || (write_type == PLATFORM_WRITE_ALL))
	{
		/* Halting WDT  */
		MAP_WDT_A_holdTimer();

		/* Selecting P1.2 and P1.3 in UART mode */
		MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
				GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

		/* Setting DCO to 12MHz */
		CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_12);

		/* Configuring UART Module */
		MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);

		/* Enable UART module */
		MAP_UART_enableModule(EUSCI_A0_BASE);

		while(length)
		{
			ch = write_buffer[index];
			MAP_UART_transmitData(EUSCI_A0_BASE, ch);
			length--;
			index++;
		}

	}

	if ((write_type == PLATFORM_WRITE_PRINTF) || (write_type == PLATFORM_WRITE_ALL))
	{
		printf("%s", write_buffer);
		fflush(stdout);
	}

	return 0;

}

/**
 *
 * \brief This function perform's scanf or read from UART
 *
 * \param    data     - character pointer
 * \param	 length	  - charcter length
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
uint8_t platform_read(uint8_t *ch)
{

	if (read_type == PLATFORM_READ_UART)
	{
		*ch = MAP_UART_receiveData(EUSCI_A0_BASE);
	}

	if (read_type == PLATFORM_READ_SCANF)
	{
		scanf("%c", ch);
	}

	return 0;

}
