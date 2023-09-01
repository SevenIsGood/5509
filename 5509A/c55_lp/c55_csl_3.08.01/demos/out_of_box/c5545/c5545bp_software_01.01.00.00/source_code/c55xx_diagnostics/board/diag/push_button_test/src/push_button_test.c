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

/*! \file push_button_test.c
*
*   \brief Functions that setup and controls gpio pins of the push buttons for
*          generating an interrupt while a button is pressed which display's
*          the name of the button pressed.
*
*/

#include "push_button_test.h"

/* Global Structure Declaration */
CSL_GpioObj     GpioObj;
CSL_GpioObj     *gpioHandle;
Uint16          readVal = 0;
Uint16          GPIO_Test = 0;
CSL_Status 		retVal;
volatile Uint16  gpioInterrupt;

/* Reference the start of the interrupt vector table */
/* This symbol is defined in file vectors.asm       */
extern void VECSTART(void);

/**
 *  \brief    This function is used to configures the GPIO pins of the
 *            switches as input and generata ans interrupt while the
 *            switch is pressed
 *
 *  \param testArgs   [IN]   Test arguments
 *
 * \return
 * \n         TEST_PASS  - Test Passed
 * \n         TEST_FAIL  - Test Failed
 */
static TEST_STATUS run_push_button_test(void *testArgs)
{
	Int16 retVal;

	CSL_GpioPinConfig    config;
	volatile Uint32 	 loop;
	Uint16 			     repeat = 1;
	Uint8 c = 0;
	gpioInterrupt = 0;

	/* Set Bus for GPIOs */
	CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_PPMODE, MODE1);

	//CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_PPMODE, MODE6);

    /* Disable CPU interrupt */
    IRQ_globalDisable();

	/* Clear any pending interrupts */
	IRQ_clearAll();

	/* Disable all the interrupts */
	IRQ_disableAll();

    /* Initialize Interrupt Vector table */
    IRQ_setVecs((Uint32)(&VECSTART));

	/* Open GPIO module */
    gpioHandle = GPIO_open(&GpioObj,&retVal);
    if((NULL == gpioHandle) || (CSL_SOK != retVal))
    {
        C55x_msgWrite("GPIO_open failed\n");
        return (TEST_FAIL);
    }

	/* Reset the GPIO module */
    GPIO_reset(gpioHandle);

    for (loop=0; loop < 0x5F5E10; loop++){}

    C55x_msgWrite("Press any push button switch on the BoosterPack and check\n\r"
    		      "if the corresponding switch name is displayed on the console\n\n\r");

	/* GPIO_config API to make PIN12 as input pin */
	config.pinNum    = CSL_GPIO_PIN12;
    config.direction = CSL_GPIO_DIR_INPUT;
    config.trigger   = CSL_GPIO_TRIG_RISING_EDGE;
    retVal = GPIO_configBit(gpioHandle,&config);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("test failed - GPIO_configBit\n");
	     return (TEST_FAIL);
	}

    /** test GPIO_config API to make PIN13 as i/p */
	config.pinNum    = CSL_GPIO_PIN13;
    config.direction = CSL_GPIO_DIR_INPUT;
    config.trigger   = CSL_GPIO_TRIG_RISING_EDGE;
    retVal = GPIO_configBit(gpioHandle,&config);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("test failed - GPIO_configBit\n");
	     return (TEST_FAIL);
	}

    /** test GPIO_config API to make PIN14 as i/p */
	config.pinNum    = CSL_GPIO_PIN14;
    config.direction = CSL_GPIO_DIR_INPUT;
    config.trigger   = CSL_GPIO_TRIG_RISING_EDGE;
    retVal = GPIO_configBit(gpioHandle,&config);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("test failed - GPIO_configBit\n");
	     return (TEST_FAIL);
	}

	/* Enable GPIO interrupts */
    retVal = GPIO_enableInt(gpioHandle,CSL_GPIO_PIN12);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("test failed- GPIO_enableInt\n");
	     return (TEST_FAIL);
	}

	/* Enable GPIO interrupts */
    retVal = GPIO_enableInt(gpioHandle,CSL_GPIO_PIN13);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("test failed- GPIO_enableInt\n");
		return(retVal);
	}

	/* Enable GPIO interrupts */
    retVal = GPIO_enableInt(gpioHandle,CSL_GPIO_PIN14);
	if(CSL_SOK != retVal)
	{
		C55x_msgWrite("test failed- GPIO_enableInt\n");
		return(retVal);
	}

	/* Clear any pending Interrupt */
    IRQ_clear(GPIO_EVENT);

    IRQ_plug(GPIO_EVENT,&gpioISR);

     /* Enabling Interrupt */
    IRQ_enable(GPIO_EVENT);
    IRQ_globalEnable();

#ifdef USE_USER_INPUT
    /*Waits Continuously to read the input from the push button switches
      and display's the switch name on the CCS console*/
	while(repeat)
	{
		if(gpioInterrupt == 1)
		{

			C55x_msgWrite("Press X/x for exiting the test\n\r");
			C55x_msgWrite("Press Y/y to continue\n\r");

			C55x_msgRead(&c, 1);

			if((c == 'y') || (c == 'Y'))
			{
				C55x_msgWrite("\n\rcontinue\n\r");
				gpioInterrupt = 0;
				GPIO_clearInt(gpioHandle,CSL_GPIO_PIN12);
				GPIO_clearInt(gpioHandle,CSL_GPIO_PIN13);
				GPIO_clearInt(gpioHandle,CSL_GPIO_PIN14);
				/* Clear any pending Interrupt */
				IRQ_clear(GPIO_EVENT);

				IRQ_plug(GPIO_EVENT,&gpioISR);

				/* Enabling Interrupt */
				IRQ_enable(GPIO_EVENT);
				IRQ_globalEnable();
			}
			if((c == 'x') || (c == 'X'))
			{
				C55x_msgWrite("\n\rExiting from the push button test\n\r");
				repeat = 0;
			}

		}

	}
#else
while(iterations < 200)
{
	C55x_delay_msec(100);
	gpioInterrupt = 0;
	GPIO_clearInt(gpioHandle,CSL_GPIO_PIN12);
	GPIO_clearInt(gpioHandle,CSL_GPIO_PIN13);
	GPIO_clearInt(gpioHandle,CSL_GPIO_PIN14);
	/* Clear any pending Interrupt */
	IRQ_clear(GPIO_EVENT);

	IRQ_plug(GPIO_EVENT,&gpioISR);

	/* Enabling Interrupt */
	IRQ_enable(GPIO_EVENT);
	IRQ_globalEnable();

	iterations++;
}
#endif

	return (TEST_PASS);

}

/**
 *  \brief  GPIO Interrupt Service Routine
 *
 *  \param testArgs   [IN]   Test arguments
 *
 *  \return none
 */
static interrupt void gpioISR(void)
{
   	Int16 retVal;

	IRQ_globalDisable();
	/* Disable all the interrupts */
	IRQ_disableAll();

    /* Check for GPIO Interrupt Flag Register */
	if((1 == GPIO_statusBit(gpioHandle,CSL_GPIO_PIN12,&retVal)))
    {
		C55x_msgWrite("SW2 is pressed\n\r");
        /* Clear GPIO Interrupt Flag Register */
        GPIO_clearInt(gpioHandle,CSL_GPIO_PIN12);
        gpioInterrupt = 1;
    }

    /* Check for GPIO Interrupt Flag Register */
	if((1 == GPIO_statusBit(gpioHandle,CSL_GPIO_PIN13,&retVal)))
    {
		C55x_msgWrite("SW3 is pressed\n\r");
        /* Clear GPIO Interrupt Flag Register */
        GPIO_clearInt(gpioHandle,CSL_GPIO_PIN13);
        gpioInterrupt = 1;
    }

    /* Check for GPIO Interrupt Flag Register */
	if((1 == GPIO_statusBit(gpioHandle,CSL_GPIO_PIN14,&retVal)))
    {
		C55x_msgWrite("SW4 is pressed\n\r");
        /* Clear GPIO Interrupt Flag Register */
        GPIO_clearInt(gpioHandle,CSL_GPIO_PIN14);
        gpioInterrupt = 1;
    }

	IRQ_clear(GPIO_EVENT);
}

/**
 * \brief This function performs push button test
 *
 * \param testArgs   [IN]   Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
TEST_STATUS pushButtonTest(void *testArgs)
{
	Int16 retVal;

	C55x_msgWrite("\n********************************\n\r");
	C55x_msgWrite(  "        Push Button Test       \n\r");
	C55x_msgWrite(  "********************************\n\r");

	retVal = run_push_button_test(testArgs);
	if(retVal != 0)
	{
		C55x_msgWrite("\nPush Button Test Failed!\n\r");
		return (TEST_FAIL);
	}
	else
	{
		//C55x_msgWrite("\nPush Button Test Passed!\n\r");
	}

	C55x_msgWrite("\nPush Button Test Completed!\n\r");

    return (TEST_PASS);

}
