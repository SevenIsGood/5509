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
/*******************************************************************************
 * MSP432 GPIO - Toggle Output High/Low
 *
 * Description: In this very simple example, the LED on P1.0 is configured as
 * an output using DriverLib's GPIO APIs. An infinite loop is then started
 * which will continuously toggle the GPIO and effectively blink the LED.
 *
 *                MSP432P401
 *             ------------------
 *         /|\|                  |
 *          | |                  |
 *          --|RST         P1.0  |---> P1.0 LED
 *            |                  |
 *            |                  |
 *            |                  |
 *            |                  |
 *
 * Author: Timothy Logan
 ******************************************************************************/
#include "gpio_launchpad_msp432_to_ble_test.h"

static TEST_STATUS run_launchpad_msp432_to_ble_test(void *testArgs)
{
	uint16_t retVal;
	uint8_t ch;

    /*Configuring the following pins as Input pins*/
    MAP_GPIO_setAsInputPin(GPIO_PORT_P3, GPIO_PIN2);

    platform_write("Configured Uart_rx as input pins\n\r");

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN3);

    platform_write("Configured Uart_tx as output pins\n\r");

	platform_write("Clearing the Uart_tx pin\n\r");

    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN3);

    platform_write("\nEnter any character once BLE side code starting running\n\r");
    platform_read(&ch);

    platform_write("\n\r");

    platform_write("Waiting for BLE to write HIGH for Uart_tx pins...\n\r");
    while(1)
    {
    	/*Reading Uart Rx pin*/
    	retVal = MAP_GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN2);
    	if(retVal == GPIO_INPUT_PIN_HIGH)
    	{
    		platform_write("Yes, Uart_tx pin is read as HIGH\n\r");
    		break;
    	}
    }

	platform_write("Writting HIGH to the Uart_tx\n\r");

    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN3);

	platform_write("Waiting for BLE to write LOW to Uart_tx pin...\n\r");
    while(1)
    {
    	/*Reading Uart Rx pin*/
    	retVal = MAP_GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN2);
    	if(retVal == GPIO_INPUT_PIN_LOW)
    	{
    		platform_write("Yes, Uart_rx pin read as LOW\n\r");
    		break;
    	}
    }

	platform_write("Writting LOW to the Uart_tx\n\r");

    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN3);

    return TEST_PASS;
}

/**
 * \brief This function performs led test
 *
 * \param testArgs  - Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
TEST_STATUS gpioLaunchpadMsp432ToBleTest(void *testArgs)
{
    TEST_STATUS testStatus;

    platform_write("\n******************************************\n\r");
    platform_write(  "        Gpio Lp Msp432 To Ble Test       \n\r");
    platform_write(  "******************************************\n\r");

    testStatus = run_launchpad_msp432_to_ble_test(testArgs);
    if(testStatus != TEST_PASS)
    {
    	platform_write("\n\rGpio Lp Msp432 To Ble Test Failed!\n\r");
    }
    else
    {
    	//platform_write("\n\n\rGpio Lp Msp432 To Ble Test Passed!\n\r");
    }

    platform_write("\n\n\rGpio Lp Msp432 To Ble Completed!!\n\r");

    return testStatus;
}
