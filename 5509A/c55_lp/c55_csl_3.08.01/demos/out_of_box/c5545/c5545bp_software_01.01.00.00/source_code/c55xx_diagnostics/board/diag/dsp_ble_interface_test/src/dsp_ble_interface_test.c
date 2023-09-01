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
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNEROR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEG LIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*! \file dsp_ble_interface_test.c
*
*   \brief Functions that verifies the SPI and GPIO intrefaces between DSP 
*   and BLE modules.
*
*/

#include "dsp_ble_interface_test.h"

/* Global Structure Declaration */
CSL_GpioObj     gpioObj;
CSL_GpioObj     *handleGpio;

/* Reference the start of the interrupt vector table */
/* This symbol is defined in file vectors.asm       */
extern void VECSTART(void);

/**
 * \brief This function performs data write and read from the DSP side
 *
 * \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
static TEST_STATUS dsp_data_write_and_read_test(void *testArgs)
{

	Int16 			retVal;
	CSL_SpiHandle	hSpi;
	SPI_Config		hwConfig;
	volatile Uint32	looper;
	volatile Uint16 value = 0;
	volatile Uint16	pollStatus;
	volatile Uint16	fnCnt;
	Uint16	writeBuffer[4] = {0x0001, 0x0002, 0x0003, 0x0004};
	Uint16  readBuffer[4] = {0x0000,0x0000,0x0000,0x0000};
	Uint16 bufLen = 4;

	hSpi = NULL;
#if 1
	//Int16 retVal;
		Uint16 gpioReadBuffer;
		CSL_GpioPinConfig    config;
		volatile Uint32 	 loop;

	    /* Pin muxing for GPIO 10 and 11 Pin */
	    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_PPMODE, MODE2);
	    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP1MODE, MODE2);

	    retVal =  SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
	                          CSL_EBSR_PPMODE_2);
	    retVal |= SYS_setEBSR(CSL_EBSR_FIELD_SP1MODE,
	                          CSL_EBSR_SP1MODE_2);
	    SYS_setEBSR(CSL_EBSR_FIELD_PPMODE, 1);

	    if(CSL_SOK != retVal)
	    {
	        C55x_msgWrite("SYS_setEBSR failed\n");
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
	    handleGpio = GPIO_open(&gpioObj,&retVal);
	    if((NULL == handleGpio) || (CSL_SOK != retVal))
	    {
	        C55x_msgWrite("GPIO_open failed\n\r");
		    return (TEST_FAIL);
	    }

		/* Reset the GPIO module */
	    GPIO_reset(handleGpio);

	    for (loop=0; loop<0x5F5E10; loop++){}

		/* Configure GPIO pin 10(PPWRMGT1) as output pin on the DSP side*/
		config.pinNum    = CSL_GPIO_PIN10;
		config.direction = CSL_GPIO_DIR_OUTPUT;
		config.trigger   = CSL_GPIO_TRIG_CLEAR_EDGE;

	    retVal = GPIO_configBit(handleGpio,&config);
	   	if(CSL_SOK != retVal)
	   	{
	   		C55x_msgWrite("test failed - GPIO_configBit\n\r");
	   	    return (TEST_FAIL);
	   	}
#endif

	retVal = SPI_init();
	if(retVal != 0)
	{
		return (TEST_FAIL);
	}

	hSpi = SPI_open(SPI_CS_NUM_2, SPI_POLLING_MODE);
	if(NULL == hSpi)
	{
		return (TEST_FAIL);
	}

	retVal = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
                         CSL_EBSR_PPMODE_1);
	if (retVal != 0)
    {
        C55x_msgWrite("SYS_setEBSR failed\n\r");
        return (TEST_FAIL);
    }

	/** Set the hardware configuration 							*/
	hwConfig.spiClkDiv	= SPI_CLK_DIV;
	hwConfig.wLen		= SPI_WORD_LENGTH_8;
	hwConfig.frLen		= SPI_FRAME_LENGTH;
	hwConfig.wcEnable	= SPI_WORD_IRQ_DISABLE;
	hwConfig.fcEnable	= SPI_FRAME_IRQ_DISABLE;
	hwConfig.csNum		= SPI_CS_NUM_2;
	hwConfig.dataDelay	= SPI_DATA_DLY_0;
	hwConfig.csPol		= SPI_CSP_ACTIVE_LOW;
	hwConfig.clkPol		= SPI_CLKP_LOW_AT_IDLE;
	hwConfig.clkPh		= SPI_CLK_PH_FALL_EDGE;

	retVal = SPI_config(hSpi, &hwConfig);
	if(retVal != 0)
	{
		return (TEST_FAIL);
	}

	C55x_msgWrite("\n\rWriting Four Bytes of Data to BLE\n\r");
	C55x_msgWrite("0x%x\t"  , writeBuffer[0]);
	C55x_msgWrite("0x%x\t"  , writeBuffer[1]);
	C55x_msgWrite("0x%x\t"  , writeBuffer[2]);
	C55x_msgWrite("0x%x\n\n\r", writeBuffer[3]);
	retVal = SPI_write(hSpi, writeBuffer, bufLen);
	if(retVal != 0)
	{
		C55x_msgWrite("SPI write failed \r\n");
		return (TEST_FAIL);
	}
	else
	{
		C55x_msgWrite("Write Successful\n\n\r");
		//C55x_delay_msec(15000);
		//C55x_delay_msec(5000);
		/*Making the gpio 10{Power Management1) as low from the DSP side*/
			retVal = GPIO_write(handleGpio, CSL_GPIO_PIN10, 1);
			 if(CSL_SOK != retVal)
			 {
				 C55x_msgWrite("GPIO_write Failed\n\r");
				 return (TEST_FAIL);
			 }
	}
	//config.direction = CSL_GPIO_DIR_INPUT;
		//       config.trigger   = CSL_GPIO_TRIG_CLEAR_EDGE;
		  //     retVal = GPIO_configBit(handleGpio,&config);
	//retVal = GPIO_read ( handleGpio, CSL_GPIO_PIN10, &gpioReadBuffer);
	//C55x_msgWrite("gpio_pin 10 read val:%d\n", gpioReadBuffer);
	//C55x_delay_msec(15000);
	retVal = SPI_deInit();
	if(retVal != 0)
	{
		return (TEST_FAIL);
	}


	retVal = SPI_close(hSpi);
	if(retVal != 0)
	{
		return (TEST_FAIL);
	}

	//C55x_msgWrite("DSP is Waiting for 15 sec's for BLE to read the data\n\r"
		//	      "written by DSP and write a new data to the DSP\n\n\r");

	//C55x_delay_msec(15000);
	C55x_delay_msec(1000);

#if 1
	retVal = SPI_init();
	if(retVal != 0)
	{
		return (TEST_FAIL);
	}

	hSpi = SPI_open(SPI_CS_NUM_2, SPI_POLLING_MODE);
	if(NULL == hSpi)
	{
		return (TEST_FAIL);
	}

	retVal = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
                         CSL_EBSR_PPMODE_1);
	if (retVal != 0)
    {
        C55x_msgWrite("SYS_setEBSR failed\n");
		return (TEST_FAIL);
    }

	/*Set the hardware configuration*/
	hwConfig.spiClkDiv	= SPI_CLK_DIV;
	hwConfig.wLen		= SPI_WORD_LENGTH_8;
	hwConfig.frLen		= SPI_FRAME_LENGTH;
	hwConfig.wcEnable	= SPI_WORD_IRQ_DISABLE;
	hwConfig.fcEnable	= SPI_FRAME_IRQ_DISABLE;
	hwConfig.csNum		= SPI_CS_NUM_2;
	hwConfig.dataDelay	= SPI_DATA_DLY_0;
	hwConfig.csPol		= SPI_CSP_ACTIVE_LOW;
	hwConfig.clkPol		= SPI_CLKP_LOW_AT_IDLE;
	hwConfig.clkPh		= SPI_CLK_PH_FALL_EDGE;

	retVal = SPI_config(hSpi, &hwConfig);
	if(retVal != 0)
	{
		return (TEST_FAIL);
	}
#if 1
	/*Making the gpio 11(Power management2) as an input port*/
		 config.pinNum    = CSL_GPIO_PIN11;
		 config.direction = CSL_GPIO_DIR_INPUT;
		 config.trigger   = CSL_GPIO_TRIG_RISING_EDGE;
		 retVal = GPIO_configBit(handleGpio,&config);
		 if(CSL_SOK != retVal)
		 {
			 C55x_msgWrite("test failed - GPIO_configBit\n\r");
			 return (TEST_FAIL);
		 }
		 else
		 {
			 //C55x_msgWrite("GPIO PIN11 is configured as Input Pin\n\r");
		 }

		 C55x_msgWrite("Waiting for Data from BLE\n");
		 while(1)
		 {
			 retVal = GPIO_read ( handleGpio, CSL_GPIO_PIN11, &gpioReadBuffer);
			 if(gpioReadBuffer)
				 break;
		 }

#if 0
		 /*Reading the gpio 11(Power Management2) pin from the DSP side*/
		 retVal = GPIO_read ( handleGpio, CSL_GPIO_PIN11, &gpioReadBuffer);
		 if(CSL_SOK != retVal)
		 {
			 C55x_msgWrite("Reading the gpio pin failed\n\r");
			 return (TEST_FAIL);
		 }

		 C55x_msgWrite("gpioReadBuffer:%d\n",gpioReadBuffer);
		 /*Cheacking for the gpio11(Power Management2) pin for high*/
		// if(!gpioReadBuffer)
		 //{
			 C55x_msgWrite("\nWaiting for DSP...\n\r");
			 /*Continues Reading of the Power Management pin until it is high*/
			 while (!gpioReadBuffer)
			 {
				 retVal = GPIO_read ( handleGpio, CSL_GPIO_PIN11, &gpioReadBuffer);
				 if(CSL_SOK != retVal)
				 {
					 C55x_msgWrite("Reading the gpio pin failed\n\r");
					 return (TEST_FAIL);
				 }

			 }

			 C55x_msgWrite("DSP is Ready\n\r");

		 }
#endif
#endif
	/*Reading 4 bytes of data from BLE*/
    //C55x_delay_msec(15000);
    //C55x_delay_msec(5000);
	retVal = SPI_read(hSpi, readBuffer, bufLen);
	if(retVal != 0)
	{
		C55x_msgWrite("SPI read failed \r\n");
		return (TEST_FAIL);
	}

	C55x_msgWrite("\n\rData Read from BLE\n\r");

	C55x_msgWrite("0x%x\t"  , readBuffer[0]);
	C55x_msgWrite("0x%x\t"  , readBuffer[1]);
	C55x_msgWrite("0x%x\t"  , readBuffer[2]);
	C55x_msgWrite("0x%x\n\n\r", readBuffer[3]);

	if((readBuffer[0]==0x5) && (readBuffer[1]==0x6) && (readBuffer[2]==0x7)
	    && (readBuffer[3]==0x8))
	{
		C55x_msgWrite("Read data match with expected data\n\n\r");
	}
	else
	{
		C55x_msgWrite("The data read doesn't match with expected data\n\n\r");
		return (TEST_FAIL);
	}


	retVal = SPI_deInit();
	if(retVal != 0)
	{
		return (TEST_FAIL);
	}

	retVal = SPI_close(hSpi);
	if(retVal != 0)
	{
		return (TEST_FAIL);
	}
#if 0
	/*Making the gpio 10{Power Management1) as low from the DSP side*/
				retVal = GPIO_write(handleGpio, CSL_GPIO_PIN10, 0);
				 if(CSL_SOK != retVal)
				 {
					 C55x_msgWrite("GPIO_write Failed\n\r");
					 return (TEST_FAIL);
				 }
				 else
				 {
					 C55x_msgWrite("GPIO_write Successful\n\r");
				 }
#endif
#endif
	return (TEST_PASS);

}

/**
 * \brief This function used to initiate the data write and read from the DSP side
 *
 * \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
static TEST_STATUS run_dsp_spi_test(void *testArgs)
{
	Int16 retVal;

	retVal = dsp_data_write_and_read_test(testArgs);
	if(retVal != 0)
	{
		return (TEST_FAIL);
	}

	return (TEST_PASS);

}

/**
 * \brief This function performs dsp-BLE interface test
 *
 * \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
TEST_STATUS dspBLETest(void *testArgs)
{
    Int16 retVal;

    C55x_msgWrite("\n************************************\n\r");
    C55x_msgWrite(  "       DSP-BLE Interface Test       \n\r");
    C55x_msgWrite(  "************************************\n\r");

    C55x_msgWrite("\n\rTest verifies SPI & GPIO interfaces between BLE and DSP\n\n\r");

    retVal = run_dsp_spi_test(testArgs);
    if(retVal != 0)
    {
    	C55x_msgWrite("DSP-BLE Interface Test Failed!\n\r");
    	return (TEST_FAIL);
    }
    else
    {
    	C55x_msgWrite("DSP-BLE Interface Test Passed!\n\r");
    }

    return (TEST_PASS);

}
