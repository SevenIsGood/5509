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

#include "gpio_launchpad_msp432_to_dsp_test.h"

/**
 * \brief This function reads and writes the gpio pins on the msp432 side
 *
 * \param testArgs  - Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
static TEST_STATUS run_launchpad_msp432_to_dsp_test(void *testArgs)
{
	uint16_t retVal;
	uint8_t ch;

#ifndef SD_BOOT
    /*Configuring the following pins as Input pins*/
    MAP_GPIO_setAsInputPin(GPIO_PORT_P3, GPIO_PIN2);
#endif

    MAP_GPIO_setAsInputPin(GPIO_PORT_P5, GPIO_PIN5);

#ifndef SD_BOOT
    platform_write("Configured Uart_rx and I2s_data as input pins\n\r");
#else
    platform_write("Configured I2s_data as input pin\n\r");
#endif

#ifndef SD_BOOT
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN3);
#endif

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN5);

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN7);

    MAP_GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN4);

#ifndef SD_BOOT
    platform_write("Configured I2s_clk, Uart_tx, i2s_rx, i2s_fs data as output pins\n\r");
#else
    platform_write("Configured I2s_clk, i2s_rx, i2s_fs data as output pins\n\r");
#endif

#ifndef SD_BOOT
	platform_write("Clearing Uart_tx, I2s_fx, I2s_clk and I2s_rx pins\n\r");
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN3);
#else
	platform_write("Clearing I2s_fx, I2s_clk and I2s_rx pins\n\r");
#endif

    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN5);

    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN7);

    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN4);

    platform_write("\nEnter any character once DSP side code started running\n\r");
    platform_read(&ch);

#ifndef SD_BOOT
    platform_write("Waiting for DSP to write HIGH for Uart_tx and I2s_data pins...\n\r");
#else
    platform_write("Waiting for DSP to write HIGH to I2s_data pin...\n\r");
#endif

    while(1)
    {
    	/*Reading Uart Rx pin*/
    	retVal = MAP_GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN5);
    	if(retVal == GPIO_INPUT_PIN_HIGH)
    	{
    		platform_write("Yes, I2s_data is read HIGH\n\r");
    		break;
    	}
    }

#ifndef SD_BOOT
	/*Reading Uart Rx pin*/
	retVal = MAP_GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN2);
	if(retVal == GPIO_INPUT_PIN_HIGH)
	{
		platform_write("Yes, Uart_tx pin also read HIGH\n\r");
	}
#endif

#ifndef SD_BOOT
	platform_write("Writting HIGH to the Uart_tx, I2s_fx, I2s_clk and I2s_rx\n\r");
    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN3);
#else
	platform_write("Writting HIGH to the I2s_fx, I2s_clk and I2s_rx\n\r");
#endif

    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN5);

    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN7);

    MAP_GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN4);

#ifndef SD_BOOT
    platform_write("Waiting for DSP to write LOW for Uart_tx and I2s_data pins...\n\r");
#else
    platform_write("Waiting for DSP to write LOW to I2s_data pin...\n\r");
#endif
    while(1)
    {
    	/*Reading Uart Rx pin*/
    	retVal = MAP_GPIO_getInputPinValue(GPIO_PORT_P5, GPIO_PIN5);
    	if(retVal == GPIO_INPUT_PIN_LOW)
    	{
    		platform_write("Yes, I2s_data pin is read LOW\n\r");
    		break;
    	}
    }

#ifndef SD_BOOT
	/*Reading Uart Rx pin*/
	retVal = MAP_GPIO_getInputPinValue(GPIO_PORT_P3, GPIO_PIN2);
	if(retVal == GPIO_INPUT_PIN_LOW)
	{
		platform_write("Yes, Uart_rx pin also read LOW\n\r");
	}
#endif

#ifndef SD_BOOT
	platform_write("Writting LOW to the Uart_tx[30], I2s_fx[7], I2s_clk[6] and I2s_rx[9]\n\r");
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN3);
#else
	platform_write("Writting LOW to the I2s_fx, I2s_clk and I2s_rx\n\r");
#endif

    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN5);

    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN7);

    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN4);

    return 0;
}

/**
 * \brief This function performs gpio Msp432 to dsp test
 *
 * \param testArgs  - Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
TEST_STATUS gpioLaunchpadMsp432ToDspTest(void *testArgs)
{
    TEST_STATUS testStatus;

    platform_write("\n******************************************\n\r");
    platform_write(  "        Gpio Lp Msp432 To Dsp Test       \n\r");
    platform_write(  "******************************************\n\r");

    testStatus = run_launchpad_msp432_to_dsp_test(testArgs);
    if(testStatus != TEST_PASS)
    {
    	platform_write("\n\rGpio Lp to Msp432 To Dsp Test Failed!\n\r");
    }
    else
    {
    	//platform_write("\n\n\rGpio Lp to Msp432 To Dsp Test Passed!\n\r");
    }

    platform_write("\n\n\rGpio Lp to Msp432 To Dsp Test Completed!!\n\r");

    return testStatus;
}

