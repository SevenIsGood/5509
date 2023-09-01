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


/*! \file csl_usb_hid_gpio.c
*
*   \brief Functions that setup and controls gpio pins of the push buttons for
*          generating an interrupt while a button is pressed which display's
*          the name of the button pressed.
*
*/

#include "stdio.h"
#include "csl_general.h"
#include "csl_gpio.h"
#include "csl_intc.h"
#include "csl_sysctrl.h"
#include "csl_usb_hid_gpio.h"

/* Global Structure Declaration */
CSL_GpioObj     GpioObj;
CSL_GpioObj     *gpioHandle;
Uint16          readVal = 0;
Uint16          GPIO_Test = 0;
CSL_Status 		retVal;

/* Reference the start of the interrupt vector table */
/* This symbol is defined in file vectors.asm       */
extern void VECSTART(void);

/**
 *  \brief    This function is used to configures the GPIO pins of the
 *            switches as input and generata ans interrupt while the
 *            switch is pressed
 */
CSL_Status GPIO_initInstance(void)
{
	Int16 retVal;

	CSL_GpioPinConfig    config;
	volatile Uint32 	 loop;

	/* Set Bus for GPIOs */
	CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_PPMODE, MODE6);

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
    	printf("GPIO_open failed\n");
        return(retVal);
    }

	/* Reset the GPIO module */
    GPIO_reset(gpioHandle);

    for (loop=0; loop < 0x5F5E10; loop++){}

	/* GPIO_config API to make PIN12 as input pin */
	config.pinNum    = CSL_GPIO_PIN12;
    config.direction = CSL_GPIO_DIR_INPUT;
    config.trigger   = CSL_GPIO_TRIG_RISING_EDGE;
    retVal = GPIO_configBit(gpioHandle,&config);
	if(CSL_SOK != retVal)
	{
		printf("test failed - GPIO_configBit\n");
		return(retVal);
	}

    /** test GPIO_config API to make PIN13 as i/p */
	config.pinNum    = CSL_GPIO_PIN13;
    config.direction = CSL_GPIO_DIR_INPUT;
    config.trigger   = CSL_GPIO_TRIG_RISING_EDGE;
    retVal = GPIO_configBit(gpioHandle,&config);
	if(CSL_SOK != retVal)
	{
		printf("test failed - GPIO_configBit\n");
		return(retVal);
	}

    /** test GPIO_config API to make PIN14 as i/p */
	config.pinNum    = CSL_GPIO_PIN14;
    config.direction = CSL_GPIO_DIR_INPUT;
    config.trigger   = CSL_GPIO_TRIG_RISING_EDGE;
    retVal = GPIO_configBit(gpioHandle,&config);
	if(CSL_SOK != retVal)
	{
		printf("test failed - GPIO_configBit\n");
		return(retVal);
	}

	/* Enable GPIO interrupts */
    retVal = GPIO_enableInt(gpioHandle,CSL_GPIO_PIN12);
	if(CSL_SOK != retVal)
	{
		printf("test failed- GPIO_enableInt\n");
		return(retVal);
	}

	/* Enable GPIO interrupts */
    retVal = GPIO_enableInt(gpioHandle,CSL_GPIO_PIN13);
	if(CSL_SOK != retVal)
	{
		printf("test failed- GPIO_enableInt\n");
		return(retVal);
	}

	/* Enable GPIO interrupts */
    retVal = GPIO_enableInt(gpioHandle,CSL_GPIO_PIN14);
	if(CSL_SOK != retVal)
	{
		printf("test failed- GPIO_enableInt\n");
		return(retVal);
	}

	/* Clear any pending Interrupt */
    IRQ_clear(GPIO_EVENT);

    IRQ_plug(GPIO_EVENT,&gpioISR);

     /* Enabling Interrupt */
    IRQ_enable(GPIO_EVENT);
    ///IRQ_globalEnable();

    preKey = NO_KEY;

    return 0;
}

CSL_Status GPIO_closeInstance(void)
{
	return 0;
}

/**
 *  \brief  GPIO Interrupt Service Routine
 *
 *  \param testArgs   [IN]   Test arguments
 *
 *  \return none
 */
Uint32 gpioIntCount = 0;
interrupt void gpioISR(void)
{

   	Int16 retVal;

   	gpioIntCount++;

    /* Check for GPIO Interrupt Flag Register */
	if((1 == GPIO_statusBit(gpioHandle,CSL_GPIO_PIN12,&retVal)))
    {
		preKey = LEFT_KEY;
        /* Clear GPIO Interrupt Flag Register */
        GPIO_clearInt(gpioHandle,CSL_GPIO_PIN12);
    }

    /* Check for GPIO Interrupt Flag Register */
	if((1 == GPIO_statusBit(gpioHandle,CSL_GPIO_PIN13,&retVal)))
    {
		preKey = DOWN_KEY;
        /* Clear GPIO Interrupt Flag Register */
        GPIO_clearInt(gpioHandle,CSL_GPIO_PIN13);
    }

    /* Check for GPIO Interrupt Flag Register */
	if((1 == GPIO_statusBit(gpioHandle,CSL_GPIO_PIN14,&retVal)))
    {
		preKey = RIGHT_KEY;
        /* Clear GPIO Interrupt Flag Register */
        GPIO_clearInt(gpioHandle,CSL_GPIO_PIN14);
    }

}

Uint16 GPIO_GetKey(void)
{
	Uint16 curKey;

	curKey = preKey;
	preKey = NO_KEY;
	return (curKey);
}
