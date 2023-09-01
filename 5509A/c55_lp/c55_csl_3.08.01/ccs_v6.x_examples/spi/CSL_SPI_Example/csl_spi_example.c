/* ============================================================================
 * Copyright (c) 2008-2016 Texas Instruments Incorporated.
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/


/** @file csl_spi_example.c
 *
 *  @brief SPI functional layer sample source file
 *
 *
 * \page    page17  CSL SPI EXAMPLE DOCUMENTATION
 *
 * \section SPI1   SPI EXAMPLE1 - POLL MODE TEST
 *
 * \subsection SPI1x    TEST DESCRIPTION:
 *		This test code verifies the functionality of CSL SPI module. SPI module
 * on the C5535/C5515/C5517 DSP is used to communicate with SPI EEPROM and audio code.
 * This test code verifies the SPI functionality using SPI EEPROM.
 *
 * During the test an EEPROM page will be written and read back using CSL SPI
 * module. CSL SPI module will be configured using SPI_config() API. EEPROM
 * status is checked to confirm that the device is not busy. Write pin is
 * enabled on the EEPROM for SPI data write operation. Write command is sent
 * to the SPI. Data is written to the  EEPROM page using SPI_dataTransaction()
 * API. After successful completion of the write operation read command is
 * sent to the SPI. Data from the EEPROM page is read using
 * SPI_dataTransaction() API. Write and read buffers are compared for the data
 * verification. A small amount of delay is required after each data operation
 * to allow the device get ready for next data operation.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5535, C5515
 * AND C5517. MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection SPI1y    TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5535/C5515/C5517 EVM)
 *  @li Open the project "CSL_SPI_Example_Out.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the test result
 *  @li Repeat the test at the following PLL frequencies
 *      C5515: 60MHz and 100MHz
 *      C5517: 60MHz, 100MHz, 150MHz and 200MHz
 *      C5535 eZdsp: 60MHz and 100MHz
 *  @li Repeat the test in Release mode
 *
 * \subsection SPI1z    TEST RESULT:
 *  @li All the CSL APIs should return success
 *  @li Data in the read and write buffers should match.
 *
 *
 */

/* ============================================================================
 * Revision History
 * ================
 * 09-Sep-2008 Created
 * 20-Jul-2012 Added C5517 Compatibility
 * 09-Mar-2016 Update header
 * 04-Apr-2016 Added C5545_BSTPCK support
 * ============================================================================
 */

#include "csl_spi.h"
#include "csl_general.h"
#include "spi_eepromApi.h"
#include "csl_sysctrl.h"
#include <stdio.h>

#define CSL_TEST_FAILED         (1)
/**< Error Status to indicate Test has Failed */
#define CSL_TEST_PASSED         (0)
/**< Error Status to indicate Test has Passed */

#define	CSL_SPI_BUF_LEN			(64)
/**< SPI Buffer length used for reading and writing */

#define	SPI_CLK_DIV				(25)
/**< SPI Clock Divisor */

#define	SPI_FRAME_LENGTH		(1)
/**< SPI Frame length */

#if (defined (C5535_EZDSP) || defined (C5545_BSTPCK) || defined (CHIP_C5517))
/** Buffer to hold contents used for writing by SPI_Write() */
Uint16 spiWriteBuff[CSL_SPI_BUF_LEN];
/** Buffer to hold contents Read using SPI_Read() */
Uint16 spiReadBuff[CSL_SPI_BUF_LEN];
#endif

CSL_Status ioExpander_Setup(void);
CSL_Status ioExpander_Read(Uint16 port, Uint16 pin, Uint16 *rValue);
CSL_Status ioExpander_Write(Uint16 port, Uint16 pin, Uint16 value);

/** Command Buffer */
Uint16 cmdBuffer[4] = {0, 0, 0, 0};

/**
 * \brief API to test SPI read\write API by writing to SPI Flash/EEPROM
 */
Int16 spi_sample(void);

#if (defined (C5535_EZDSP) || defined (C5545_BSTPCK) || defined (CHIP_C5517))
/**
 * \brief API to test SPI read\write API by writing to SPI Flash
 */
Int16 spi_flash_sample(void);
#else
/**
 * \brief API to test SPI read\write API by writing to EEPROM
 */
Int16 spi_eeprom_sample(void);
#endif

   /////INSTRUMENTATION FOR BATCH TESTING -- Part 1 --
   /////  Define PaSs_StAtE variable for catching errors as program executes.
   /////  Define PaSs flag for holding final pass/fail result at program completion.
        volatile Int16 PaSs_StAtE = 0x0001; // Init to 1. Reset to 0 at any monitored execution error.
        volatile Int16 PaSs = 0x0000; // Init to 0.  Updated later with PaSs_StAtE when and if
   /////                                  program flow reaches expected exit point(s).
   /////
/**
 * Main Program
 */
void main(void)
{
	Int16    status;

	printf("CSL SPI Test\n\n");

	status = spi_sample();
	if(status != CSL_TEST_PASSED)
	{
		printf("\nCSL SPI Test Failed!!\n");
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
   /////  Reseting PaSs_StAtE to 0 if error detected here.
        PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
   /////
	}
	else
	{
		printf("\nCSL SPI Test Passed!!\n");
	}
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 3 --
   /////  At program exit, copy "PaSs_StAtE" into "PaSs".
        PaSs = PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with
   /////                   // pass/fail value determined during program execution.
   /////  Note:  Program should next exit to C$$EXIT and halt, where DSS, under
   /////   control of a host PC script, will read and record the PaSs' value.
   /////
}

/**
 * \brief API to test SPI read\write API by writing to SPI Flash/EEPROM
 */
Int16 spi_sample(void)
{
	Int16 status;

	status = CSL_TEST_FAILED;

#if (defined (C5535_EZDSP) || defined (C5545_BSTPCK) || defined (CHIP_C5517))
	status = spi_flash_sample();
	if(CSL_SOK == status)
	{
		printf("\nSPI example with Flash Passed\n");
	}
	else
	{
		printf("\nSPI example with Flash Failed\n");
	}
#else
	status = spi_eeprom_sample();
	if(CSL_SOK == status)
	{
		printf("\nSPI example with EEPROM Passed\n");
	}
	else
	{
		printf("\nSPI example with EEPROM Failed\n");
	}
#endif

	return (status);
}

#if (defined (C5535_EZDSP) || defined (C5545_BSTPCK) || defined (CHIP_C5517))
/**
 * \brief API to test SPI read\write API by writing to SPI Flash
 */
Int16 spi_flash_sample(void)
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
	else
	{
		printf ("SPI Instance Initialize successfully\n");
	}

	hSpi = SPI_open(SPI_CS_NUM_0, SPI_POLLING_MODE);
	if(NULL == hSpi)
	{
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf ("SPI Instance Opened successfully\n");
	}

#if (defined (C5535_EZDSP) || defined (C5545_BSTPCK))
    status = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
                         CSL_EBSR_PPMODE_1);
#else
    status = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
                         CSL_EBSR_PPMODE_3);
#endif
	if (CSL_SOK != status)
    {
        printf("SYS_setEBSR failed\n");
        return(status);
    }

#ifdef CHIP_C5517
    status = ioExpander_Setup();
	if (status != CSL_SOK)
    {
        printf("ERROR: ioExpander_Setup Failed\n");
        return (CSL_TEST_FAILED);
    }

    status = ioExpander_Write(0, 5, 0);  /* Enable SPI_I2S2_S0 Mode */
	if (status != CSL_SOK)
    {
        printf("ERROR: ioExpander_Write Failed\n");
        return (CSL_TEST_FAILED);
    }

    status = ioExpander_Write(0, 6, 0);  /* Disable SPI_I2S2_S1 Mode */
	if (status != CSL_SOK)
    {
        printf("ERROR: ioExpander_Write Failed\n");
        return (CSL_TEST_FAILED);
    }
#endif

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
	else
	{
		printf ("SPI Instance Configured successfully\n");
	}

/* Erase the spiflash before writing */

    /* Issue WPEN */
#if (defined(CHIP_C5517))
	CSL_SPI_REGS->SPICMDL = 0x0000 | 0;
#else
    CSL_SPI_REGS->SPICMD1 = 0x0000 | 0;
#endif

    cmdBuffer[0] = SPI_CMD_WREN;

    result = SPI_dataTransaction(hSpi ,cmdBuffer, 1, SPI_WRITE);
	if(CSL_SOK != result)
	{
		return (CSL_TEST_FAILED);
	}

#if (defined(CHIP_C5517))
	CSL_SPI_REGS->SPICMDL = 0x0000 | 4 - 1;
#else
    CSL_SPI_REGS->SPICMD1 = 0x0000 | 4 - 1;
#endif

    /* Issue erase */
#if (defined (C5535_EZDSP) || defined (C5545_BSTPCK) || defined (CHIP_C5517))
    cmdBuffer[0] = SPI_CMD_ERASE;
    cmdBuffer[1] = ( Uint8 )( pageNo >> 16 );
    cmdBuffer[2] = ( Uint8 )( pageNo >> 8 );
    cmdBuffer[3] = ( Uint8 )( pageNo & 0xFF );

    commandLength = 4;
#else
    cmdBuffer[0] = SPI_CMD_ERASE;
    cmdBuffer[1] = ( Uint8 )( pageNo >> 8 );
    cmdBuffer[2] = ( Uint8 )( pageNo  & 0xFF );

    commandLength = 3;
#endif /* #if (defined (C5535_EZDSP) || defined (C5545_BSTPCK) || defined (CHIP_C5517)) */

    result = SPI_dataTransaction(hSpi ,cmdBuffer, commandLength, SPI_WRITE);
	if(CSL_SOK != result)
	{
		return (CSL_TEST_FAILED);
	}

    /* Wait while busy */
    do
    {
		cmdBuffer[0] = SPI_CMD_RDSR;

		/* Send read status command */
#if (defined(CHIP_C5517))
	CSL_SPI_REGS->SPICMDL = 0x0000 | 1;
#else
		CSL_SPI_REGS->SPICMD1 = 0x0000 | 1;
#endif

		result = SPI_dataTransaction(hSpi ,cmdBuffer, 1, SPI_WRITE );
		result = SPI_dataTransaction(hSpi ,cmdBuffer, 1, SPI_READ);

	} while(cmdBuffer[0] & 0x01);

#if (defined(CHIP_C5517))
	CSL_SPI_REGS->SPICMDL = 0x0000 | 0;
#else
	CSL_SPI_REGS->SPICMD1 = 0x0000 | 0;
#endif

	/* WREN Command */
	cmdBuffer[0] = SPI_CMD_WREN;

	result = SPI_dataTransaction(hSpi ,cmdBuffer, 1, SPI_WRITE);
	if(CSL_SOK != result)
	{
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf ("SPI Instance Write successfully\n");
	}

    /* Wait while busy */
    do
    {
		cmdBuffer[0] = SPI_CMD_RDSR;

		/* Send read status command */
#if (defined(CHIP_C5517))
		CSL_SPI_REGS->SPICMDL = 0x0000 | 1;
#else
		CSL_SPI_REGS->SPICMD1 = 0x0000 | 1;
#endif

		result = SPI_dataTransaction(hSpi ,cmdBuffer, 1, SPI_WRITE );
		result = SPI_dataTransaction(hSpi ,cmdBuffer, 1, SPI_READ);

	} while(cmdBuffer[0] & 0x01);

#if (defined (C5535_EZDSP) || defined (C5545_BSTPCK) || defined (CHIP_C5517))
	/* Write Command */
	cmdBuffer[0] = SPI_CMD_WRITE;
    /* Flash address - MSB */
    cmdBuffer[1] = (pageNo >> 0x10 & 0xFF);
    /* Flash address */
    cmdBuffer[2] = ((pageNo >> 0x08) & 0xFF);
    /* Flash address - LSB */
    cmdBuffer[3] = (pageNo & 0xFF);

    commandLength = 4;
#else
	/* Write Command */
	cmdBuffer[0] = SPI_CMD_WRITE;
    /* Flash address */
    cmdBuffer[1] = ((pageNo >> 0x08) & 0xFF);
    /* Flash address - LSB */
    cmdBuffer[2] = (pageNo & 0xFF);

    commandLength = 3;
#endif  /* #if (defined (C5535_EZDSP) || defined (C5545_BSTPCK) || defined (CHIP_C5517)) */

#if (defined(CHIP_C5517))
	CSL_SPI_REGS->SPICMDL = 0x0000 | CSL_SPI_BUF_LEN + 4 - 1;
#else
	CSL_SPI_REGS->SPICMD1 = 0x0000 | CSL_SPI_BUF_LEN + 4 - 1;
#endif

	result = SPI_dataTransaction(hSpi ,cmdBuffer, commandLength, SPI_WRITE);
	if(CSL_SOK != result)
	{
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf ("SPI Instance Write successfully\n");
	}

	/****************************************************************************/
	for(looper = 0; looper < 64; )
	{
		spiWriteBuff[looper] = (Uint16)0x17;
		spiWriteBuff[(looper + 1)] = (Uint16)0x52;
		spiReadBuff[looper] = 0x0000;
		spiReadBuff[(looper + 1)] = 0x000D;
		looper += 2;
	}

	result = SPI_dataTransaction(hSpi ,spiWriteBuff, CSL_SPI_BUF_LEN, SPI_WRITE);
	if(CSL_SOK != result)
	{
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf ("SPI Instance Write successfully\n");
	}

    /* Wait while busy */
    do
    {
		cmdBuffer[0] = SPI_CMD_RDSR;

		/* Send read status command */
#if (defined(CHIP_C5517))
		CSL_SPI_REGS->SPICMDL = 0x0000 | 1;
#else
		CSL_SPI_REGS->SPICMD1 = 0x0000 | 1;
#endif

		result = SPI_dataTransaction(hSpi ,cmdBuffer, 1, SPI_WRITE );
		result = SPI_dataTransaction(hSpi ,cmdBuffer, 1, SPI_READ);

	} while(cmdBuffer[0] & 0x01);

	/******************************************************************************/
#if (defined (C5535_EZDSP) || defined (C5545_BSTPCK) || defined (CHIP_C5517))
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

#else
   /* Configure the SPI for read from SPI Flash */
    /* read command */
    cmdBuffer[0] = SPI_CMD_READ;
    /* Flash address */
    cmdBuffer[1] = ((pageNo >> 0x08) & 0xFF);
	/*Flash address - LSB */
	cmdBuffer[2] = (pageNo & 0xFF);

	commandLength = 3;

#endif  /* #if (defined (C5535_EZDSP) || defined (C5545_BSTPCK) || defined (CHIP_C5517)) */

#if (defined(CHIP_C5517))
		CSL_SPI_REGS->SPICMDL = 0x0000 | CSL_SPI_BUF_LEN + 4 - 1;
#else
	CSL_SPI_REGS->SPICMD1 = 0x0000 | CSL_SPI_BUF_LEN + 4 - 1;
#endif

	result = SPI_dataTransaction(hSpi ,cmdBuffer, commandLength, SPI_WRITE);
	if(CSL_SOK != result)
	{
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf ("SPI Instance Write successfully\n");
	}

	/******************************************************************************/
	result = SPI_dataTransaction(hSpi ,spiReadBuff, CSL_SPI_BUF_LEN, SPI_READ);
	if(CSL_SOK != result)
	{
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf ("SPI Instance Read successfully\n");
	}

	result = SPI_deInit();

	if(CSL_SOK != result)
	{
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf ("SPI Instance deIntialize successfully\n");
	}

	result = SPI_close(hSpi);

	if(CSL_SOK != result)
	{
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf ("SPI Instance Close successfully\n");
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
		printf ("SPI Write & Read buffer matching\n");
	}
	else
	{
		printf ("\nSPI Write & Read buffers are not matching\n");
	}

	return (status);
}

#else

/** Data to be written to EEPROM */
#define DATA_TO_WRITE (0x17)

/** Buffer length used for reading and writing to EEPROM */
#define CSL_SPI_BUF_LEN_EEPROM (32)

/** Buffer to hold contents used for writing by SPI_Write() */
static Uint8 spiWriteBuff[CSL_SPI_BUF_LEN_EEPROM];

/** Buffer to hold contents Read using SPI_Read() */
static Uint8 spiReadBuff[CSL_SPI_BUF_LEN_EEPROM];

/**
 * \brief API to test SPI read\write API by writing to EEPROM
 */
Int16 spi_eeprom_sample(void)
{
	Int16 			status;
	Int16 			result;
	CSL_SpiHandle	hSpi;
	Int16 			pageNo;
	volatile Uint32	looper;

    Uint16* pdata;

	status = CSL_SOK;

    pageNo = 100;

	result = SPI_init();
	if(CSL_SOK != result)
	{
		status = CSL_TEST_FAILED;
		return (status);
	}
	else
	{
		printf ("SPI Instance Initialize successfully\n");
	}

#if (defined (C5535_EZDSP) || defined (C5545_BSTPCK))
    result = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
                         CSL_EBSR_PPMODE_1);
#else
    result = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
                         CSL_EBSR_PPMODE_3);
#endif
	if(CSL_SOK != result)
    {
        printf("SYS_setEBSR failed\n");
		status = CSL_TEST_FAILED;
		return (status);
	}

	hSpi = SPI_open(SPI_CS_NUM_0, SPI_POLLING_MODE);
	if(NULL == hSpi)
	{
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf ("SPI Instance Opened successfully\n");
	}

	/* Initialize the SPI EEPROM interface */
    result = spi_eeprom_init(hSpi);
	if(CSL_SOK != result)
	{
		printf("SPI EEPROM Interface Initialization Failed\n");
		status = CSL_TEST_FAILED;
		return (status);
	}
	else
	{
		printf ("SPI EEPROM Interface Initialization Successful\n");
	}

    /* Write to spi_eeprom */
    /* Initialize the Write Buffer */
    pdata = ( Uint16* )spiWriteBuff;
    for ( looper = 0 ; looper < CSL_SPI_BUF_LEN_EEPROM ; looper++ )
        *pdata++ = ((DATA_TO_WRITE + looper)& 0xFF);

    /* Write a page */
    result = SPI_EEPROM_write(hSpi,
                              (Uint32)spiWriteBuff,
                              (pageNo * CSL_SPI_BUF_LEN_EEPROM),
                              CSL_SPI_BUF_LEN_EEPROM );
	if(CSL_SOK != result)
	{
		printf ("SPI_EEPROM_write API Failed\n");
		return (CSL_TEST_FAILED);
	}

	while((CSL_SPI_REGS->SPISTAT1 & 0x0001) != 0) {};

	for (looper = 0; looper < 0xffff; looper++)
	;

    /* Read and verify SPI EEPROM */
	/* Clear the Receive buffer */
    for ( looper = 0 ; looper < CSL_SPI_BUF_LEN_EEPROM ; looper++ )
        spiReadBuff[looper] = 0;

    /* Read a page */
    result = SPI_EEPROM_read(hSpi,
                             (pageNo * CSL_SPI_BUF_LEN_EEPROM),
                             (Uint32)spiReadBuff,
                             CSL_SPI_BUF_LEN_EEPROM );
	if(CSL_SOK != result)
	{
		printf ("SPI_EEPROM_read API Failed\n");
		return (CSL_TEST_FAILED);
	}

    /* Check the pattern */
    pdata = ( Uint16* )spiReadBuff;
    for ( looper = 0 ; looper < CSL_SPI_BUF_LEN_EEPROM ; looper++ )
    {
		if (((DATA_TO_WRITE + looper) & 0xFF) != spiReadBuff[looper] )
		{
            status = CSL_TEST_FAILED;
            break;
		}
	}

	result = SPI_deInit();
	if(CSL_SOK != result)
	{
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf ("SPI Instance deIntialize successfully\n");
	}

	result = SPI_close(hSpi);
	if(CSL_SOK != result)
	{
		return (CSL_TEST_FAILED);
	}
	else
	{
		printf ("SPI Instance Close successfully\n");
	}

	if (status == CSL_TEST_FAILED)
	{
		printf ("spi_eeprom_test failed: Buffers not Matching\n");
	}
	else
	{
		printf ("spi_eeprom_test passed: Buffers are Matching\n");
	}

	return (status);
}

#endif /* #if (defined (C5535_EZDSP) || defined (C5545_BSTPCK) || defined (CHIP_C5517)) */
