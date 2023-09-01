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

/*! \file gpio_dsp_to_lp_cc3200_test.h
*
*   \brief Test to verify interface lines between CC3200 LP and DSP as GPIOs
*
*/

#include "gpio_dsp_to_lp_cc3200_test.h"

/* Global Structure Declaration */
CSL_GpioObj     object;
CSL_GpioObj     *handle;

/* Reference the start of the interrupt vector table */
/* This symbol is defined in file vectors.asm       */
extern void VECSTART(void);

/**
 * \brief This function is used to read and write the gpio pins at dsp side
 *
 * \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
static TEST_STATUS run_gpio_dsp_test(void *testArgs)
{
	Int16                retVal;
	CSL_GpioPinConfig    config;
	volatile Uint32 	 loop;
	Uint16 				 readVal=0;

#ifndef SD_BOOT
	retVal = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
			             CSL_EBSR_PPMODE_2);
#else
	/*MODE for UART prints*/
	retVal = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
			             CSL_EBSR_PPMODE_1);
#endif

	/*MODE for configuring GPIO pins 6,7,8,9*/
    retVal |= SYS_setEBSR(CSL_EBSR_FIELD_SP1MODE,
                          CSL_EBSR_SP1MODE_2);
    if(CSL_SOK != retVal)
    {
        C55x_msgWrite("SYS_setEBSR failed\n\r");
        return(retVal);
    }

    /* Disable CPU interrupt */
    IRQ_globalDisable();

	/* Clear any pending interrupts */
	IRQ_clearAll();

	/* Disable all the interrupts */
	IRQ_disableAll();

    /* Initialize Interrupt Vector table */
    IRQ_setVecs((Uint32)(&VECSTART));

	/* Open GPIO module */
    handle = GPIO_open(&object,&retVal);
    if((NULL == handle) || (CSL_SOK != retVal))
    {
        C55x_msgWrite("\n\rGPIO_open failed\n\r");
        return(retVal);
    }

	/* Reset the GPIO module */
    GPIO_reset(handle);

	for (loop=0; loop<0x5F5E10; loop++){}

	/***************Configuring INPUT & OUTPUT as GPIO PINS************************/

	 /* Configure GPIO pin 6(DSP_I2S1_CLK) as input pin on the DSP side*/
	config.pinNum    = CSL_GPIO_PIN6;
	config.direction = CSL_GPIO_DIR_INPUT;
	config.trigger   = CSL_GPIO_TRIG_CLEAR_EDGE;
	retVal = GPIO_configBit(handle,&config);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("test failed - GPIO_configBit\n\r");
		return(retVal);
	}

	/* Configure GPIO pin 7(DSP_I2S1_FS) as input pin on the DSP side*/
	config.pinNum    = CSL_GPIO_PIN7;
	config.direction = CSL_GPIO_DIR_INPUT;
	config.trigger   = CSL_GPIO_TRIG_CLEAR_EDGE;
	retVal = GPIO_configBit(handle,&config);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("test failed - GPIO_configBit\n\r");
		return(retVal);
	}

	config.pinNum    = CSL_GPIO_PIN8;
	config.direction = CSL_GPIO_DIR_OUTPUT;
	config.trigger   = CSL_GPIO_TRIG_CLEAR_EDGE;
	retVal = GPIO_configBit(handle,&config);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("test failed - GPIO_configBit\n\r");
		return(retVal);
	}

	/* Configure GPIO pin 9(DSP_I2S1_RX) as input pin on the DSP side*/
	config.pinNum    = CSL_GPIO_PIN9;
	config.direction = CSL_GPIO_DIR_INPUT;
	config.trigger   = CSL_GPIO_TRIG_CLEAR_EDGE;
	retVal = GPIO_configBit(handle,&config);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("test failed - GPIO_configBit\n\r");
		return(retVal);
	}

#ifndef SD_BOOT
	/* Configure GPIO pin 30(DSP_UART_RX) as input pin on the DSP side*/
	config.pinNum    = CSL_GPIO_PIN30;
	config.direction = CSL_GPIO_DIR_INPUT;
	config.trigger   = CSL_GPIO_TRIG_CLEAR_EDGE;
	retVal = GPIO_configBit(handle,&config);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("test failed - GPIO_configBit\n\r");
		return(retVal);
	}

    /* Configure GPIO pin 31(DSP_UART_TX) as output pin on the DSP side */
    config.pinNum    = CSL_GPIO_PIN31;
    config.direction = CSL_GPIO_DIR_OUTPUT;
    config.trigger   = CSL_GPIO_TRIG_CLEAR_EDGE;
    retVal = GPIO_configBit(handle,&config);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("test failed - GPIO_configBit\n\r");
		return(retVal);
	}
#endif

	/******************CLEARING THE DEFAULT VALUES*****************************/
	/*Making the gpio 9(DSP_I2S1_RX) as low from the DSP side*/
	//C55x_msgWrite("Clearing the Values at Launchpad side to default.\n");

#ifndef SD_BOOT
	C55x_msgWrite("Writing HIGH to GPIO PINS 8 & 31\n\r");
#else
	C55x_msgWrite("Writing HIGH to GPIO PIN 8\n\r");
#endif
	retVal = GPIO_write(handle, CSL_GPIO_PIN8, 1);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("GPIO_write Failed\n\r");
		return(TEST_FAIL);
	}
	else
	{
		//C55x_msgWrite("GPIO_write 0 for gpio 8(DSP_I2S1_DX) is Successful\n\r");
	}

#ifndef SD_BOOT
	/*Making the gpio 31(DSP_UART_TX) as low from the DSP side*/
	retVal = GPIO_write(handle, CSL_GPIO_PIN31, 1);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("GPIO_write Failed\n\r");
		return(TEST_FAIL);
	}
	else
	{
		//C55x_msgWrite("GPIO_write 0 gpio 31(DSP_UART_TX)is Successful\n\r");
	}
#endif

#ifndef SD_BOOT
	C55x_msgWrite("Waiting for LaunchPad to Write HIGH on GPIO PINS 6,7,9 & 30...\n\r");
#else
	C55x_msgWrite("Waiting for LaunchPad to Write HIGH on GPIO PINS 6,7 & 9...\n\r");
#endif
	while(1)
	{

		/*Making the gpio 6(DSP_I2S1_CLK) as low from the DSP side*/
		readVal =0;
		retVal = GPIO_read(handle, CSL_GPIO_PIN6, &readVal);
		if(CSL_SOK != retVal)
		{
			 C55x_msgWrite("GPIO_read Failed\n\r");
			 return(TEST_FAIL);
		}

		if(readVal)
		{
			C55x_msgWrite("LaunchPad write completed DSP started reading...\n\r");
			break;
		}

	}

	/*Reading the gpio 7(DSP_I2S1_FS) as low from the DSP side*/
	retVal = GPIO_read(handle, CSL_GPIO_PIN7, &readVal);
	if(CSL_SOK != retVal)
	{
		 C55x_msgWrite("GPIO_read Failed\n\r");
		 return(TEST_FAIL);
	 }

	if(!readVal)
	{
		C55x_msgWrite("NO, The PIN7 is read as LOW...TEST FAILED\n\r");
		return(TEST_FAIL);
	}

	/*Making the gpio 9(DSP_I2S1_RX) as low from the DSP side*/
	retVal = GPIO_read(handle, CSL_GPIO_PIN9, &readVal);
	if(CSL_SOK != retVal)
	{
		 C55x_msgWrite("GPIO_read Failed\n\r");
		 return(TEST_FAIL);
	}

	if(!readVal)
	{
		C55x_msgWrite("NO, The PIN9 is read as LOW...TEST FAILED\n\r");
		return(TEST_FAIL);
	}

	C55x_msgWrite("DSP is able to read the GPIO PINS 6,7 & 9 as HIGH\n\r");

#ifndef SD_BOOT
	/*Making the gpio 30(DSP_UART_RX) as low from the DSP side*/
	retVal = GPIO_read(handle, CSL_GPIO_PIN30, &readVal);
	if(CSL_SOK != retVal)
	{
		 C55x_msgWrite("GPIO_read Failed\n\r");
		 return(TEST_FAIL);
	 }

	if(!readVal)
	{
		C55x_msgWrite("NO, The PIN30 is read as LOW...TEST FAILED\n\r");
		return(TEST_FAIL);
	}

#endif

	/*Making the gpio 9(DSP_I2S1_RX) as low from the DSP side*/
#ifndef SD_BOOT
	C55x_msgWrite("Writing LOW to GPIO PINS 8 & 31\n\r");
#else
	C55x_msgWrite("Writing LOW to GPIO PIN 8\n\r");
#endif
	retVal = GPIO_write(handle, CSL_GPIO_PIN8, 0);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("GPIO_write Failed\n\r");
		return(TEST_FAIL);
	}

#ifndef SD_BOOT
	/*Making the gpio 31(DSP_UART_TX) as low from the DSP side*/
	retVal = GPIO_write(handle, CSL_GPIO_PIN31, 0);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("GPIO_write Failed\n\r");
		return(TEST_FAIL);
	}
#endif

#ifndef SD_BOOT
	C55x_msgWrite("Waiting for LaunchPad to Write LOW on GPIO PIN 6,7,9 & 30...\n\r");
#else
	C55x_msgWrite("Waiting for LaunchPad to Write LOW on GPIO PIN 6,7 & 9...\n\r");
#endif

	/*Continuous reading of the gpio pin16 untill it becomes low*/
	while(1)
	{

		/*Making the gpio 6(DSP_I2S1_CLK) as low from the DSP side*/
		readVal =0;
		retVal = GPIO_read(handle, CSL_GPIO_PIN6, &readVal);
		if(CSL_SOK != retVal)
		{
			C55x_msgWrite("GPIO_read Failed\n\r");
			return(TEST_FAIL);
		}

		if(!readVal)
		{
			C55x_msgWrite("LaunchPad write completed DSP started reading...\n\r");
			break;
		}

	}

	/*Making the gpio 7(DSP_I2S1_FS) as low from the DSP side*/
	retVal = GPIO_read(handle, CSL_GPIO_PIN7, &readVal);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("GPIO_read Failed\n\r");
		return(TEST_FAIL);
	}
	if(readVal)
	{
		C55x_msgWrite("NO, The PIN7 is read as HIGH...TEST FAILED\n\r");
		return(TEST_FAIL);
	}

	 /*Making the gpio 9(DSP_I2S1_RX) as low from the DSP side*/
	retVal = GPIO_read(handle, CSL_GPIO_PIN9, &readVal);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("GPIO_read Failed\n\r");
		return(TEST_FAIL);
	}
	if(readVal)
	{
		C55x_msgWrite("NO, The PIN9 is read as HIGH...TEST FAILED\n\r");
		return(TEST_FAIL);
	}


	C55x_msgWrite("DSP is able to read the GPIO PINS 6,7 & 9 as LOW\n\r");

#ifndef SD_BOOT
	/*Making the gpio 30(DSP_UART_RX) as low from the DSP side*/
	readVal =0;
	retVal = GPIO_read(handle, CSL_GPIO_PIN30, &readVal);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("GPIO_read Failed\n\r");
		return(TEST_FAIL);
	 }
#endif

	/******************CLEARING THE DEFAULT VALUES*****************************/
	/*Making the gpio 9(DSP_I2S1_RX) as low from the DSP side*/

	retVal = GPIO_write(handle, CSL_GPIO_PIN8, 0);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("GPIO_write Failed\n\r");
		return(TEST_FAIL);
	}

#ifndef SD_BOOT
	/*Making the gpio 31(DSP_UART_TX) as low from the DSP side*/
	retVal = GPIO_write(handle, CSL_GPIO_PIN31, 0);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("GPIO_write Failed\n\r");
		return(TEST_FAIL);
	}
#endif

	return (retVal);

}

/**
 * \brief This function writes and reads the gpio lines connected between DSP
 *        and launchpad CC3200 from DSP side
 *
 * \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
TEST_STATUS gpioDspToLpCC3200Test(void *testArgs)
{
    Int16 retVal;

    C55x_msgWrite("\n***********************************************\n\r");
    C55x_msgWrite(  "       C5545 BP TO CC3200 LAUNCHPAD TEST       \n\r");
    C55x_msgWrite(  "***********************************************\n\r");

    retVal = run_gpio_dsp_test(testArgs);
    if(retVal != TEST_PASS)
    {
    	C55x_msgWrite("\nGPIO C5545 BP TO LAUNCHPAD TEST Failed!\n\r");
    	return (-1);
    }
    else
    {
    	C55x_msgWrite("\nGPIO C5545 BP TO LAUNCHPAD TEST Passed!\n\r");
    }

    return retVal;

}
