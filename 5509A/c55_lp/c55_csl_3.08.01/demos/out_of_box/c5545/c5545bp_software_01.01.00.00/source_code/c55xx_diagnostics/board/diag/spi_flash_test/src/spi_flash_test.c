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

/*! \file spi_flash_test.c
*
*   \brief SPI flash test file
*
*/

#include "spi_flash_test.h"

/** Buffer to hold contents used for writing by SPI_Write() */
Uint16 spiWriteBuff[CSL_SPI_BUF_LEN];
/** Buffer to hold contents Read using SPI_Read() */
Uint16 spiReadBuff[CSL_SPI_BUF_LEN];

/** Command Buffer */
Uint16 cmdBuffer[4] = {0, 0, 0, 0};

/**
 *  \brief    This function is used to perform spi flash erase test
 *
 *  \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n         TEST_PASS  - Test Passed
 * \n         TEST_FAIL  - Test Failed
 */
static TEST_STATUS spi_flash_erase_test(void *testArgs)
{
	Int16 			status = CSL_TEST_FAILED;
	Int16 			result;
	CSL_SpiHandle	hSpi;
	SPI_Config		hwConfig;
	volatile Uint32	looper;
	volatile Uint16 value = 0;
	Uint32 			pageNo;
	Uint16          commandLength;
	volatile Uint16	pollStatus;
	volatile Uint16	fnCnt;

	hSpi = NULL;

	pageNo = 0x0000;

	result = SPI_init();
	if(CSL_SOK != result)
	{
		status = CSL_TEST_FAILED;
		return (status);
	}

	hSpi = SPI_open(SPI_CS_NUM_0, SPI_POLLING_MODE);
	if(NULL == hSpi)
	{
		return (CSL_TEST_FAILED);
	}

    status = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
                         CSL_EBSR_PPMODE_1);
	if (CSL_SOK != status)
    {
        C55x_msgWrite("SYS_setEBSR failed\n\r");
        return(status);
    }

	/** Set the hardware configuration 							*/
	hwConfig.spiClkDiv	= SPI_CLK_DIV;
	hwConfig.wLen		= SPI_WORD_LENGTH_8;
	hwConfig.frLen		= SPI_FRAME_LENGTH;
	hwConfig.wcEnable	= SPI_WORD_IRQ_ENABLE;
	hwConfig.fcEnable	= SPI_FRAME_IRQ_DISABLE;
	hwConfig.csNum		= SPI_CS_NUM_0;
	hwConfig.dataDelay	= SPI_DATA_DLY_0;
	hwConfig.csPol		= SPI_CSP_ACTIVE_LOW;
	hwConfig.clkPol		= SPI_CLKP_LOW_AT_IDLE;
	hwConfig.clkPh		= SPI_CLK_PH_FALL_EDGE;

	result = SPI_config(hSpi, &hwConfig);
	if(CSL_SOK != result)
	{
		return (CSL_TEST_FAILED);
	}

	/* Erase the spiflash before writing */

	/* Issue WPEN */
	CSL_SPI_REGS->SPICMD1 = 0x0000 | 0;

	cmdBuffer[0] = SPI_CMD_WREN;

	result = SPI_dataTransaction(hSpi ,cmdBuffer, 1, SPI_WRITE);
	if(CSL_SOK != result)
	{
		return (CSL_TEST_FAILED);
	}

	CSL_SPI_REGS->SPICMD1 = 0x0000 | 4 - 1;

	/* Issue erase */
	cmdBuffer[0] = SPI_CMD_ERASE;
	cmdBuffer[1] = ( Uint8 )( pageNo >> 16 );
	cmdBuffer[2] = ( Uint8 )( pageNo >> 8 );
	cmdBuffer[3] = ( Uint8 )( pageNo & 0xFF );

	commandLength = 4;

	result = SPI_dataTransaction(hSpi ,cmdBuffer, commandLength, SPI_WRITE);
	if(CSL_SOK != result)
	{
		return (CSL_TEST_FAILED);
	}

	/* C55x_delay_msec while busy */
	do
	{
		cmdBuffer[0] = SPI_CMD_RDSR;

		/* Send read status command */
		CSL_SPI_REGS->SPICMD1 = 0x0000 | 1;

		result = SPI_dataTransaction(hSpi ,cmdBuffer, 1, SPI_WRITE );
		result = SPI_dataTransaction(hSpi ,cmdBuffer, 1, SPI_READ);

	} while(cmdBuffer[0] & 0x01);


	CSL_SPI_REGS->SPICMD1 = 0x0000 | 0;

	result = SPI_deInit();

	if(CSL_SOK != result)
	{
		return (CSL_TEST_FAILED);
	}

	result = SPI_close(hSpi);

	if(CSL_SOK != result)
	{
		return (CSL_TEST_FAILED);
	}

	return result;

}

/**
 *  \brief    This function is used to perform spi read write comparission
 *  		  test on spi flash
 *
 *  \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n         TEST_PASS  - Test Passed
 * \n         TEST_FAIL  - Test Failed
 */
static TEST_STATUS spi_flash_read_write_comparission_test(void *testArgs)
{
	Int16 			status = CSL_TEST_FAILED;
	Int16 			result;
	CSL_SpiHandle	hSpi;
	SPI_Config		hwConfig;
	volatile Uint32	looper;
	volatile Uint16 value = 0;
	Uint32 			pageNo;
	Uint16          commandLength;
	volatile Uint16	pollStatus;
	volatile Uint16	fnCnt;

	hSpi = NULL;

	pageNo = 0x0000;

	result = SPI_init();
	if(CSL_SOK != result)
	{
		status = CSL_TEST_FAILED;
		return (status);
	}

	hSpi = SPI_open(SPI_CS_NUM_0, SPI_POLLING_MODE);
	if(NULL == hSpi)
	{
		return (CSL_TEST_FAILED);
	}

    status = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
                         CSL_EBSR_PPMODE_1);
	if (CSL_SOK != status)
    {
        C55x_msgWrite("SYS_setEBSR failed\n\r");
        return(status);
    }

	/** Set the hardware configuration 							*/
	hwConfig.spiClkDiv	= SPI_CLK_DIV;
	hwConfig.wLen		= SPI_WORD_LENGTH_8;
	hwConfig.frLen		= SPI_FRAME_LENGTH;
	hwConfig.wcEnable	= SPI_WORD_IRQ_ENABLE;
	hwConfig.fcEnable	= SPI_FRAME_IRQ_DISABLE;
	hwConfig.csNum		= SPI_CS_NUM_0;
	hwConfig.dataDelay	= SPI_DATA_DLY_0;
	hwConfig.csPol		= SPI_CSP_ACTIVE_LOW;
	hwConfig.clkPol		= SPI_CLKP_LOW_AT_IDLE;
	hwConfig.clkPh		= SPI_CLK_PH_FALL_EDGE;

	result = SPI_config(hSpi, &hwConfig);
	if(CSL_SOK != result)
	{
		return (CSL_TEST_FAILED);
	}
	/* WREN Command */
	cmdBuffer[0] = SPI_CMD_WREN;

	result = SPI_dataTransaction(hSpi ,cmdBuffer, 1, SPI_WRITE);
	if(CSL_SOK != result)
	{
		return (CSL_TEST_FAILED);
	}

    /* C55x_delay_msec while busy */
    do
    {
		cmdBuffer[0] = SPI_CMD_RDSR;

		/* Send read status command */
		CSL_SPI_REGS->SPICMD1 = 0x0000 | 1;

		result = SPI_dataTransaction(hSpi ,cmdBuffer, 1, SPI_WRITE );
		result = SPI_dataTransaction(hSpi ,cmdBuffer, 1, SPI_READ);

	} while(cmdBuffer[0] & 0x01);

	/* Write Command */
	cmdBuffer[0] = SPI_CMD_WRITE;
    /* Flash address - MSB */
    cmdBuffer[1] = (pageNo >> 0x10 & 0xFF);
    /* Flash address */
    cmdBuffer[2] = ((pageNo >> 0x08) & 0xFF);
    /* Flash address - LSB */
    cmdBuffer[3] = (pageNo & 0xFF);

    commandLength = 4;

	CSL_SPI_REGS->SPICMD1 = 0x0000 | CSL_SPI_BUF_LEN + 4 - 1;

	result = SPI_dataTransaction(hSpi ,cmdBuffer, commandLength, SPI_WRITE);
	if(CSL_SOK != result)
	{
		return (CSL_TEST_FAILED);
	}

	for(looper = 0; looper < 64; )
	{
		spiWriteBuff[looper] = looper;
		spiReadBuff[looper] = 0x0000;
		looper ++;
	}
	result = SPI_dataTransaction(hSpi ,spiWriteBuff, CSL_SPI_BUF_LEN, SPI_WRITE);
	if(CSL_SOK != result)
	{
		return (CSL_TEST_FAILED);
	}

    /* C55x_delay_msec while busy */
    do
    {
		cmdBuffer[0] = SPI_CMD_RDSR;

		/* Send read status command */
		CSL_SPI_REGS->SPICMD1 = 0x0000 | 1;

		result = SPI_dataTransaction(hSpi ,cmdBuffer, 1, SPI_WRITE );
		result = SPI_dataTransaction(hSpi ,cmdBuffer, 1, SPI_READ);

	} while(cmdBuffer[0] & 0x01);

    /* Configure the SPI for read from SPI Flash */
    /* read command */
    cmdBuffer[0] = SPI_CMD_READ;
    /* Flash address - MSB */
    cmdBuffer[1] = ((pageNo >> 0x10) & 0xFF);
    /* Flash address */
    cmdBuffer[2] = ((pageNo >> 0x08) & 0xFF);
	/*Flash address - LSB */
	cmdBuffer[3] = (pageNo & 0xFF);

	commandLength = 4;

	CSL_SPI_REGS->SPICMD1 = 0x0000 | CSL_SPI_BUF_LEN + 4 - 1;

	result = SPI_dataTransaction(hSpi ,cmdBuffer, commandLength, SPI_WRITE);
	if(CSL_SOK != result)
	{
		return (CSL_TEST_FAILED);
	}

	result = SPI_dataTransaction(hSpi ,spiReadBuff, CSL_SPI_BUF_LEN, SPI_READ);
	if(CSL_SOK != result)
	{
		return (CSL_TEST_FAILED);
	}

	result = SPI_deInit();

	if(CSL_SOK != result)
	{
		return (CSL_TEST_FAILED);
	}

	result = SPI_close(hSpi);

	if(CSL_SOK != result)
	{
		return (CSL_TEST_FAILED);
	}

	for(looper=0; looper < CSL_SPI_BUF_LEN; looper++)
    {
        if(spiReadBuff[looper] != spiWriteBuff[looper])
		{
			status = CSL_TEST_FAILED;
			break;
		}
		else
		{
			status = CSL_TEST_PASSED;
		}

    }

	/* Check result */
	if(status == CSL_TEST_PASSED)
	{
		C55x_msgWrite ("SPI Write & Read buffer matching\n\r");
	}
	else
	{
		C55x_msgWrite ("\nSPI Write & Read buffers are not matching\n\r");
		return (TEST_FAIL);
	}

	return (status);

}

/**
 *  \brief    This function is used to perform spi detect and
 *  		  read write comparission tests on spi flash
 *
 *  \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n         TEST_PASS  - Test Passed
 * \n         TEST_FAIL  - Test Failed
 */
static TEST_STATUS run_spi_flash_test(void *testArgs)
{
	Int16    status;

	C55x_msgWrite("\nRunning SPI flash erase test...\n\r");
	status = spi_flash_erase_test(testArgs);
	if(status != CSL_TEST_PASSED)
	{
		C55x_msgWrite("\nSPI flash erase test failed!!\n\n\r");
		return (TEST_FAIL);
	}
	else
	{
		C55x_msgWrite("\nSPI flash erase test passed!!\n\n\r");
	}

	C55x_msgWrite("\nRunning SPI flash data read/write test...\n\n\r");
	status = spi_flash_read_write_comparission_test(testArgs);
	if(status != CSL_TEST_PASSED)
	{
		C55x_msgWrite("\nSPI flash read write test failed!!\n\r");
		return (TEST_FAIL);
	}
	else
	{
		C55x_msgWrite("\nSPI flash read write test Passed!!\n\r");
	}

	return 0;

}

/**
 * \brief This function performs spi flash test
 *
 * \param testArgs  - Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
TEST_STATUS spiFlashTest(void *testArgs)
{
    TEST_STATUS testStatus;

    C55x_msgWrite("\n********************************\n\r");
    C55x_msgWrite(  "         SPI FLASH Test       \n\r");
    C55x_msgWrite(  "********************************\n\r");

    testStatus = run_spi_flash_test(testArgs);
    if(testStatus != TEST_PASS)
    {
    	C55x_msgWrite("\nSPI FLASH Test Failed!\n\r");
    	return (TEST_FAIL);
    }
    else
    {
    	//C55x_msgWrite("\nSPI FLASH Test Passed!\n\r");
    }

    C55x_msgWrite("\nSPI FLASH Test Completed!!\n\r");

    return testStatus;

}
