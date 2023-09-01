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

/*! \file sd_test.c
*
*   \brief Functions that initializes and setup MMCSD module which will read
*          the SD details and perform the write and read operations in both
*          1-bit and 4-bit modes
*
*/

#include "sd_test.h"

/* SD card Buffer size in Bytes                                              */
#define BUFFER_MAX_SIZE    (512u)
/* SD card physical address with respect to sector number                    */
#define CARD_START_ADDR    (0x0)

#define CSL_SD_CLOCK_MAX_KHZ      (20000u)

/* SD card Data buffers */
Uint16 pReadBuff[BUFFER_MAX_SIZE/2];
Uint16 pWritedBuff[BUFFER_MAX_SIZE/2];

CSL_MMCControllerObj 	pMmcsdContObj;
CSL_MmcsdHandle 		mmcsdHandle;
CSL_MMCCardIdObj 		sdCardIdObj;
CSL_MMCCardCsdObj 		sdCardCsdObj;
CSL_MMCCardExtCsdObj    sdCardExtCsObj;

CSL_GpioObj    *hGpio;

/**
 *  \brief    This function is used to perform sd card detection
 *
 *  \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n         TEST_PASS  - Test Passed
 * \n         TEST_FAIL  - Test Failed
 */
static TEST_STATUS card_identification_test(void *testArgs)
{
	CSL_Status           status;
	CSL_GpioPinConfig    config;
	CSL_GpioObj          gpioObj;
	Uint16               gpioReadBuffer;


    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_PPMODE, MODE2);
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP1MODE, MODE2);

    status =  SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
                          CSL_EBSR_PPMODE_2);
    if(CSL_SOK != status)
    {
        C55x_msgWrite("SYS_setEBSR failed\n\r");
        return(status);
    }

	/* Enable clocks to all peripherals */
	CSL_SYSCTRL_REGS->PCGCR1 = 0x0000;
	CSL_SYSCTRL_REGS->PCGCR2 = 0x0000;

	/* Open GPIO module */
	hGpio = GPIO_open(&gpioObj, &status);
	if((NULL == hGpio) || (CSL_SOK != status))
	{
		C55x_msgWrite("GPIO_open failed\n\r");
		return(CSL_SOK);
	}

	 /*Making the gpio 30(UART RX) as an input port*/
	 config.pinNum    = CSL_GPIO_PIN29;
	 config.direction = CSL_GPIO_DIR_INPUT;
	 config.trigger   = CSL_GPIO_TRIG_RISING_EDGE;
	 status = GPIO_configBit(hGpio,&config);
	 if(CSL_SOK != status)
	 {
		 C55x_msgWrite("Configuring GPIO Bit failed\n\r");
		 return(status);
	 }

	 /*Cheacking for the gpio30(UART RX) pin for high*/
	 status = GPIO_read ( hGpio, CSL_GPIO_PIN29, &gpioReadBuffer);
	 if(CSL_SOK != status)
	 {
		 C55x_msgWrite("Reading the gpio pin failed\n\r");
		 SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
				     CSL_EBSR_PPMODE_1);
		 return(status);
	 }
	 else if(gpioReadBuffer == 1)
	 {
		 SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
				     CSL_EBSR_PPMODE_1);
		 return (TEST_FAIL);
	 }
	 else
	 {
		 SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
				     CSL_EBSR_PPMODE_1);
			return (TEST_PASS);
	 }

}

/**
 *  \brief    Function to calculate the memory clock rate

 *  \param    void
 *
 *  \return   MMC clock rate value
 */
Uint16 computeClkRate(void)
{
	Uint32    sysClock;
	Uint32    remainder;
	Uint32    memMaxClk;
	Uint16    clkRate;

	sysClock  = 0;
	remainder = 0;
	memMaxClk = CSL_SD_CLOCK_MAX_KHZ;
	clkRate   = 0;

	/* Get the clock value at which CPU is running */
	sysClock = C55x_getSysClk();

	if (sysClock > memMaxClk)
	{
		if (memMaxClk != 0)
		{
			clkRate   = sysClock / memMaxClk;
			remainder = sysClock % memMaxClk;

            /*
             * If the remainder is not equal to 0, increment clock rate to make
             * sure that memory clock value is less than the value of
             * 'CSL_SD_CLOCK_MAX_KHZ'.
             */
			if (remainder != 0)
			{
				clkRate++;
			}

            /*
             * memory clock divider '(2 * (CLKRT + 1)' will always
             * be an even number. Increment the clock rate in case of
             * clock rate is not an even number
             */
			if (clkRate%2 != 0)
			{
				clkRate++;
			}

			/*
			 * AT this point 'clkRate' holds the value of (2 * (CLKRT + 1).
			 * Get the value of CLKRT.
			 */
			clkRate = clkRate/2;
			clkRate = clkRate - 1;

			/*
			 * If the clock rate is more than the maximum allowed clock rate
			 * set the value of clock rate to maximum value.
			 * This case will become true only when the value of
			 * 'CSL_SD_CLOCK_MAX_KHZ' is less than the minimum possible
			 * memory clock that can be generated at a particular CPU clock.
			 *
			 */
			if (clkRate > CSL_MMC_MAX_CLOCK_RATE)
			{
				clkRate = CSL_MMC_MAX_CLOCK_RATE;
			}
		}
		else
		{
			clkRate = CSL_MMC_MAX_CLOCK_RATE;
		}
	}

	return (clkRate);
}

/**
 *  \brief  This function is used to perform the SD Data Write and Read test
 *
 *  \param  testArgs   [IN]   Test arguments
 *			bitMode      -    0 for '1' bit Mode
 *			bitMode      -    1 for '4' bit Mode
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 */
CSL_Status data_write_and_read_test(void *testArgs, Uint16 bitMode)
{
	CSL_Status	 mmcStatus;
	Uint16		 count;
	Uint32       cardAddr = 0X0000;
	Uint16       rca;
	Uint32       cardStatus;

	CSL_MMCCardObj *mmcCardObj = (CSL_MMCCardObj *)testArgs;

	/* Initialize data buffers */
	for(count = 0; count < (BUFFER_MAX_SIZE/2); count++)
	{
	    pReadBuff[count]   = 0x0;
		pWritedBuff[count] = count;
	}

	/* Get the clock divider value for the current CPU frequency */
	computeClkRate();

	/* Initialize the CSL MMCSD module */
	mmcStatus = MMC_init();
	if(mmcStatus != CSL_SOK)
	{
		C55x_msgWrite("MMC_init Failed\n");
		return(mmcStatus);
	}

    mmcStatus = SYS_setEBSR(CSL_EBSR_FIELD_SP0MODE,
                            CSL_EBSR_SP0MODE_0);
    mmcStatus |= SYS_setEBSR(CSL_EBSR_FIELD_SP1MODE,
                             CSL_EBSR_SP1MODE_0);
    if(CSL_SOK != mmcStatus)
    {
    	C55x_msgWrite("SYS_setEBSR failed\n");
        return (mmcStatus);
    }

	/* Open the MMCSD module in POLLED mode */
	mmcsdHandle = MMC_open(&pMmcsdContObj, CSL_MMCSD0_INST, CSL_MMCSD_OPMODE_POLLED, &mmcStatus);
	if(mmcStatus != CSL_SOK)
	{
		C55x_msgWrite("MMC_open Failed\n\r");
		return(mmcStatus);
	}

	/* Check for the card */
    mmcStatus = MMC_selectCard(mmcsdHandle, mmcCardObj);
	if((mmcStatus == CSL_ESYS_BADHANDLE) ||
	   (mmcStatus == CSL_ESYS_INVPARAMS))
	{
		C55x_msgWrite("MMC_selectCard Failed\n\r");
		return(mmcStatus);
	}

	/* Set the init clock */
    mmcStatus = MMC_sendOpCond(mmcsdHandle, 70);
	if(mmcStatus != CSL_SOK)
	{
		C55x_msgWrite("MMC_sendOpCond Failed\n\r");
		return(mmcStatus);
	}

	/* Send the card identification Data */
	mmcStatus = SD_sendAllCID(mmcsdHandle, &sdCardIdObj);
	if(mmcStatus != CSL_SOK)
	{
		C55x_msgWrite("SD_sendAllCID Failed\n\r");
		return(mmcStatus);
	}
	else if(bitMode == 0)
	{
		C55x_msgWrite("Card Information\n\r");
		C55x_msgWrite("----------------\n\r");
		C55x_msgWrite("Manufacture Id            : %d\n\r", sdCardIdObj.mfgId);
		C55x_msgWrite("Serial No                 : %lu\n\r", sdCardIdObj.serialNumber);
		C55x_msgWrite("Manufacturing date        : %dth Month %d\n\r", sdCardIdObj.month, sdCardIdObj.year);
		C55x_msgWrite("product name              : %s\n\r", sdCardIdObj.productName);
	}

	/* Set the Relative Card Address */
	mmcStatus = SD_sendRca(mmcsdHandle, mmcCardObj, &rca);
	if(mmcStatus != CSL_SOK)
	{
		C55x_msgWrite("SD_sendRca Failed\n\r");
		return(mmcStatus);
	}

	/* Read the SD Card Specific Data */
	mmcStatus = SD_getCardCsd(mmcsdHandle, &sdCardCsdObj);
	if(mmcStatus != CSL_SOK)
	{
		C55x_msgWrite("SD_getCardCsd Failed\n\r");
		return(mmcStatus);
	}
	else if(bitMode == 0)
	{
		float size;
		if(mmcCardObj->cardType == 1)
		{
			C55x_msgWrite("Card Type                 : SD_CARD\n\r");
		}
		else
		{
			C55x_msgWrite("Reading the card type has failed\n\r");
			return (TEST_FAIL);
		}
		C55x_msgWrite("Card Capacity             : %lu\n\r",  mmcCardObj->cardCapacity);
		C55x_msgWrite("Total number of sectors   : %lu\n\r", mmcCardObj->totalSectors);
		size = mmcCardObj->totalSectors;
		size = ((size * 512)/(1024 * 1000000));
		C55x_msgWrite("Size in GB                : %f\n\r", size);
	}

	/* Set bus width - Optional */
	mmcStatus = SD_setBusWidth(mmcsdHandle, bitMode);
	if(mmcStatus != CSL_SOK)
	{
		C55x_msgWrite("SD_setBusWidth Failed\n\r");
		return(mmcStatus);
	}

	/* Write data to the SD card */
  	mmcStatus = MMC_write(mmcsdHandle, cardAddr, BUFFER_MAX_SIZE, pWritedBuff);
	if(mmcStatus != CSL_SOK)
	{
		C55x_msgWrite("MMC_write Failed\n\r");
		return(mmcStatus);
	}

	/* Read data from the SD card */
	mmcStatus = MMC_read(mmcsdHandle, cardAddr, BUFFER_MAX_SIZE, pReadBuff);
	if(mmcStatus != CSL_SOK)
	{
		C55x_msgWrite("MMC_read Failed\n\r");
		return(mmcStatus);
	}

	/* Get card stataus */
	mmcStatus = MMC_getCardStatus(mmcsdHandle, &cardStatus);
	if(mmcStatus != CSL_SOK)
	{
		C55x_msgWrite("MMC_getCardStatus Failed\n\r");
		return(mmcStatus);
	}

	/* Deselect the SD card */
	mmcStatus = MMC_deselectCard(mmcsdHandle, mmcCardObj);
	if(mmcStatus != CSL_SOK)
	{
		C55x_msgWrite("MMC_deselectCard Failed\n\r");
		return(mmcStatus);
	}

	/* Clear the MMCSD card response registers */
	mmcStatus = MMC_clearResponse(mmcsdHandle);
	if(mmcStatus != CSL_SOK)
	{
		C55x_msgWrite("MMC_clearResponse Failed\n\r");
		return(mmcStatus);
	}

	/* Send CMD0 to the SD card */
	mmcStatus = MMC_sendCmd(mmcsdHandle, 0x00, 0x00, CSL_MMCSD_EVENT_EOFCMD);
	if(mmcStatus != CSL_SOK)
	{
		C55x_msgWrite("MMC_sendCmd Failed\n\r");
		return(mmcStatus);
	}

	/* Close the MMCSD module */
	mmcStatus = MMC_close(mmcsdHandle);
	if(mmcStatus != CSL_SOK)
	{
		C55x_msgWrite("MMC_close Failed\n\r");
		return(mmcStatus);
	}

	/* Compare the MMC read and write buffers */
	for(count = 0; count < (BUFFER_MAX_SIZE / 2); count++)
	{
		if(pReadBuff[count] != pWritedBuff[count])
		{
			C55x_msgWrite("Buffer miss matched at position %d\n\r",count);
			return(CSL_ESYS_FAIL);
		}
	}

	C55x_msgWrite("SD Card Read & Write Buffer Matched\n\r");

	return(TEST_PASS);

}

/**
 *  \brief    This function is used to perform sd card detection and
 *            verifies the operation of sd card
 *
 *  \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n         TEST_PASS  - Test Passed
 * \n         TEST_FAIL  - Test Failed
 */
static TEST_STATUS run_sd_card_test(void *testArgs)
{
	Int16 retVal;
	Uint16 bitMode;

	retVal = card_identification_test(testArgs);
	if(retVal != 0)
	{
		C55x_msgWrite("\n\rCard was not detected\n\r");
		C55x_msgWrite("Please insert the SD card in the SD Card slot\n\r");
		return (TEST_FAIL);
	}
	else
	{
		C55x_msgWrite("\n\rSD Card Detected Successfully\n\r\n\r");
	}

	bitMode = 0; /*1-Bit MODE*/
	retVal = data_write_and_read_test(testArgs, bitMode);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("SD Data Write and Read test failed!\n\r");
		return (TEST_FAIL);
	}
	else
	{
		C55x_msgWrite("SD Data Write and Read test passed\n\r\n\r");
	}

	bitMode = 1; /*4-Bit MODE*/
	retVal = data_write_and_read_test(testArgs, bitMode);
	if(retVal != CSL_SOK)
	{
		C55x_msgWrite("SD Data Write and Read test(4-Bit mode) failed!\n\r");
		return (TEST_FAIL);
	}
	else
	{
		C55x_msgWrite("SD Data Write and Read test(4-Bit mode) passed\n\r\n\r");
	}

	return (TEST_PASS);
}

/**
 * \brief This function performs sd test
 *
 * \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
TEST_STATUS sdTest(void *testArgs)
{
    Int16 retVal;

    C55x_msgWrite("\n******************************\n\r");
    C55x_msgWrite(  "         SD Card Test       \n\r");
    C55x_msgWrite(  "******************************\n\r");

    if(testArgs == NULL)
    {
    	C55x_msgWrite("Invalid Test Arguments!\n\r");
    	C55x_msgWrite("Aborting the Test!!\n\r");
        return (TEST_FAIL);
    }

    retVal = run_sd_card_test(testArgs);
    SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
                          CSL_EBSR_PPMODE_1);

    if(retVal != TEST_PASS)
    {
    	C55x_msgWrite("\nSD Card Test Failed!\n\r");
    	return (TEST_FAIL);
    }
    else
    {
    	C55x_msgWrite("\nSD Card Test Passed!\n\r");
    }

    return (TEST_PASS);

}
