/*  ============================================================================
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


/** @file CSL_McSPI_MasterDma_DmaTxCh1_RxCh2.c
 *
 *  @brief Example code to verify McSPI-host data transactions using DMA Channels 1 and 2
 *
 *
 * \page    page10   McSPI EXAMPLE DOCUMENTATION
 *
 * \section McSPI1   McSPI EXAMPLE1 - McSPI DMA Chx 1 AND 2
 *
 * \subsection McSPI1x    TEST DESCRIPTION:
 *         This test uses DMA for facilitating data movement from CPU write buffer to CH0TX
 *   and CH0RX to CPU read buffer. The C5517 McSPI is configured as master and MSP430 host
 *   as the SPI slave. Data integrity is ensured by comparing the data being transmitted and
 *   data received back, albeit with some delay.
 *   DMA is configured for 1 32-bit word burst transaction. DMA0 Channel1 is used for Tx and
 *   DMA0 Channel2 is used for Rx.
 *
 *
 *		This test code verifies the dma functionality of McSPI module. McSPI module
 * on the C5517 DSP is used to communicate with SPI MsP430 through DMA.
 * This test code verifies the McSPI functionality using MsP430 SPI as slave module.
 *
 *   CSL McSPI module will be configured using McSPI_config() API.Write,Read function
 *   used for data transfer operation. After successful completion of the write
 *   operation read sent to the SPI.Write and read buffers are compared for the data
 * verification. A small amount of delay is required after each data operation
 * to allow the device get ready for next data operation.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5517.
 * MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h. "#define CHIP_C5517"
 *
 * \subsection McSPI1y    TEST PROCEDURE:
 *
 * Load the C5517_SPI_MASTER code on C5517.
 * Load the MSP430_SPI_SLAVE code in MSP430 .
 *
 * Run the slave code & then start the master code.
 * Verify the data transaction by putting the break points in MSP430_SPI_SLAVE ISR
 *
 * Master sent data's are available in MSP430 USCI_BO_SPI_MODE TX,RX buffer
 * Put break points in C5517_SPI_MASTER end of the code gmcspiDmaReadBuf,gmcspiDmaWriteBuf data's are availble
 *
 * SW4 Push button HPI_ON,HPI_SEL_IF is off.
 *
 * Run the DMA test @ 100 Mhz PLL .
 *
 * Other freq will read improper data
 *
 * \subsection McSPI1z    TEST RESULT:
 *   All the CSL APIs should return success
 * Run the slave program first & then the master program 
 *   Data in the read and write buffers should match.(or)Data needs to be verified in Slave program buffer
 */
 /*
 * Revision History
 * ================
 * 21-Nov-2011 Created
 * 09-Mar-2016 Update the header
 * ============================================================================
 */

#include <stdio.h>
#include "csl_mcspi.h"
#include "cslr_mcspi.h"
#include "soc.h"
#include <csl_dma.h>

#include "csl_gpio.h"
#include <csl_i2c.h>


#define CSL_TEST_PASSED      (0)
#define CSL_TEST_FAILED      (1)


#define CSL_I2C_DATA_SIZE        (64)
#define CSL_EEPROM_ADDR_SIZE     (2)
#define CSL_I2C_OWN_ADDR         (0x2F)
#define CSL_I2C_SYS_CLK          (100)
#define CSL_I2C_BUS_FREQ         (10)
#define CSL_I2C_EEPROM_ADDR                            (0x50)
#define CSL_I2C_CODEC_ADDR                                (0x18)
#define CSL_I2C_TCA6416_ADDR             (0x21)


Uint16  gI2cWrBuf[CSL_I2C_DATA_SIZE + CSL_EEPROM_ADDR_SIZE];
Uint16  gI2cRdBuf[CSL_I2C_DATA_SIZE];


CSL_I2cSetup     i2cSetup;
CSL_I2cConfig    i2cConfig;
CSL_GpioObj    gpioObj;
CSL_GpioObj    *hGpio;



#define CSL_MCSPI_TEST_PASSED      (0)
#define CSL_MCSPI_TEST_FAILED      (1)

#define CSL_MCSPI_BUF_SIZE (1024u)

int j=0;
CSL_McSpiHandle  hMcspi;
CSL_McSpiObj 	 McSpiObj; 

extern CSL_Status ioExpander_Setup(void);
extern CSL_Status ioExpander_Write(Uint16 port, Uint16 pin, Uint16 value);  
  
/* CSL DMA data structures used for mcspi data transfers */
CSL_DMA_Handle dmaTxHandle;
CSL_DMA_Handle dmaRxHandle;
CSL_DMA_Config dmaConfig;
CSL_DMA_ChannelObj    dmaTxChanObj;
CSL_DMA_ChannelObj    dmaRxChanObj;

/**Dma write is happening in 32 bits,if use character array*/
Uint32 gmcspiDmaWriteBuf[CSL_MCSPI_BUF_SIZE]={0xAA};
Uint32 gmcspiDmaReadBuf[CSL_MCSPI_BUF_SIZE];
Uint16  gmcspiDmaCompBuf[4*CSL_MCSPI_BUF_SIZE];

/**
 * \brief Comparison of received buffer.
 *
 * This structure is used for converting 32-bit word to two 16-bit values.
 */
typedef union 
{
	unsigned int dat[2];	/**< Accessing two 16-bit values*/
	Uint32 value;		/**< Accessing 32-bit value*/
}word;

word buffer;

//---------Function prototypes---------
CSL_Status CSL_McspiTest(void);
/*Reference the start of the interrupt vector table*/
extern void VECSTART(void);

CSL_DMA_Handle CSL_configDmaFormcspi(CSL_DMA_ChannelObj    *dmaChanObj,
                                    CSL_DMAChanNum        chanNum);

/////INSTRUMENTATION FOR BATCH TESTING -- Part 1 --
/////  Define PaSs_StAtE variable for catching errors as program executes.
/////  Define PaSs flag for holding final pass/fail result at program completion.
volatile Int16 PaSs_StAtE = 0x0001; // Init to 1. Reset to 0 at any monitored execution error.
volatile Int16 PaSs = 0x0000; // Init to 0.  Updated later with PaSs_StAtE when and if
/////                                  program flow reaches expected exit point(s).
/////
void main() {

  CSL_Status result; 
  
  printf("\r\nMCSPI DMA Mode test!!\n");

  result = CSL_McspiTest();
  if(result == CSL_MCSPI_TEST_PASSED)
  {
 	printf("\r\nmcspi DMA Mode test passed!!\n");
  }
  else
  {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
	printf("\r\nMcspi DMA Mode test failed!!\n");
	PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
  }

  /////INSTRUMENTATION FOR BATCH TESTING -- Part 3 --
  /////  At program exit, copy "PaSs_StAtE" into "PaSs".
  PaSs =PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with
  /////                   // pass/fail value determined during program execution.
  /////  Note:  Program should next exit to C$$EXIT and halt, where DSS, under
  /////   control of a host PC script, will read and record the PaSs' value.
  /////

  
}


  CSL_Status        status;
  CSL_GpioConfig    config;


CSL_Status CSL_McspiTest(void) 
{

#if (defined(CHIP_C5517))
  Uint16 i,k;

  unsigned char error=0;
  CSL_Status status; //This is for the CSL status return


 /****    SEL_MMC1 High    ***/


 
  ioExpander_Setup();

  //Call this fn for enabling

  // port,pin, value

  // port,pin, value
  ioExpander_Write(1, 4, 0); // Select
  ioExpander_Write(1, 5, 0); // Select


    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP1MODE, MODE1);

  /* Initilize data buffers. xmt will be 8 bit value      */
  /* with element number in 8 bits  */
  for( i = 0; i < CSL_MCSPI_BUF_SIZE ; i++ ) 
  {
    gmcspiDmaWriteBuf[i] = 0x12345600+i;
    gmcspiDmaReadBuf[i] = 0;
  }
  
  
  k=0;
  gmcspiDmaCompBuf[0] = 0xFFFF;
  for( i = 1; i < (2*CSL_MCSPI_BUF_SIZE) ; i++ ) 
  {
    gmcspiDmaCompBuf[i] = k; 
    gmcspiDmaCompBuf[i]<<= 8; 
	gmcspiDmaCompBuf[i] |= k+1; 
	k+=2;
	if(!(k&0xFF))
		k=0;
       
  }
  
  /* Initialize Dma */
  status = DMA_init();
  if (status != CSL_SOK)
  {
    printf("DMA_init Failed!\n");
  }


  /* Open mcspi Port 0, this will return a mcspi handle that will */
  /* be used in calls to other CSl functions.                     */ 

  hMcspi =  McSPI_open(McSPI_DMA_MODE,&McSpiObj,&status);
  McSPI_close(hMcspi);
  hMcspi =  McSPI_open(McSPI_DMA_MODE,&McSpiObj,&status);
 
  if ( (hMcspi == NULL) || (status != CSL_SOK) )
  {
   printf("mcspi_open() Failed \n");
  }
  else
  {
	printf("mcspi_open() Success \n");
  }

 	hMcspi->McSPIHwConfig.mode       = McSPI_MASTER;
    hMcspi->McSPIHwConfig.duplex     = McSPI_FULL_DUPLEX;
    hMcspi->McSPIHwConfig.wLen       = McSPI_WORD_LENGTH_32;
    hMcspi->McSPIHwConfig.dataDelay  = McSPI_DATA_NODLY;
    hMcspi->McSPIHwConfig.ClkDiv     = CSL_MCSPI_CH0CONFL_CLKD_DIV16;
#if 0
    hMcspi->McSPIHwConfig.clkPh      = McSPI_CLK_PH_ODD_EDGE;
    hMcspi->McSPIHwConfig.clkPol     = McSPI_CLKP_HIGH;
#else 
  
	hMcspi->McSPIHwConfig.clkPh      = McSPI_CLK_PH_EVEN_EDGE;
    hMcspi->McSPIHwConfig.clkPol     = McSPI_CLKP_LOW;

#endif

    hMcspi->ch					     = McSPI_CS_NUM_0;
    hMcspi->McSPIHwConfig.csPol      = McSPI_CSP_ACTIVE_LOW;
    hMcspi->McSPIHwConfig.inSel      = McSPI_RX_SPIDAT0;
	hMcspi->McSPIHwConfig.enDat0_1   = (McSPI_TxEn_Dat0_1)McSPI_TX_ENABLE_SPIDAT0;


    /* Configure the DMA channel for mcspi transmit */
#if ((defined(CHIP_C5505_C5515)) || (defined(CHIP_C5504_C5514)) || defined(CHIP_C5517) || (defined(CHIP_C5535) || defined(CHIP_C5545)))
	dmaConfig.pingPongMode = CSL_DMA_PING_PONG_DISABLE;
#endif
	dmaConfig.autoMode = CSL_DMA_AUTORELOAD_DISABLE;
	dmaConfig.burstLen = CSL_DMA_TXBURST_1WORD;
	dmaConfig.trigger  = CSL_DMA_EVENT_TRIGGER;
	dmaConfig.dmaEvt   = CSL_DMA_EVT_MCSPI_CH0_TX;
	dmaConfig.dmaInt   = CSL_DMA_INTERRUPT_DISABLE;
	dmaConfig.chanDir  = CSL_DMA_WRITE;
	dmaConfig.trfType  = CSL_DMA_TRANSFER_IO_MEMORY;
	dmaConfig.dataLen  = (CSL_MCSPI_BUF_SIZE*4);
	dmaConfig.srcAddr  = (Uint32)gmcspiDmaWriteBuf;
	dmaConfig.destAddr = (Uint32)&(hMcspi->Regs->CH0TXL);

    dmaTxHandle = CSL_configDmaFormcspi(&dmaTxChanObj, CSL_DMA_CHAN1);
	if(dmaTxHandle == NULL)
	{
		printf("DMA Config for mcspi DMA Write Failed!\n!");
		return(CSL_MCSPI_TEST_FAILED);
	}

    


    // Configure the DMA channel for mcspi receive 
#if ((defined(CHIP_C5505_C5515)) || (defined(CHIP_C5504_C5514)) || defined(CHIP_C5517) || (defined(CHIP_C5535) || defined(CHIP_C5545)))
	dmaConfig.pingPongMode = CSL_DMA_PING_PONG_DISABLE;
#endif
	dmaConfig.autoMode = CSL_DMA_AUTORELOAD_DISABLE;
	dmaConfig.burstLen = CSL_DMA_TXBURST_1WORD;
    dmaConfig.trigger  = CSL_DMA_EVENT_TRIGGER;
    dmaConfig.dmaEvt   = CSL_DMA_EVT_MCSPI_CH0_RX;
	dmaConfig.dmaInt   = CSL_DMA_INTERRUPT_DISABLE;
	dmaConfig.chanDir  = CSL_DMA_READ;
	dmaConfig.trfType  = CSL_DMA_TRANSFER_IO_MEMORY;
	dmaConfig.dataLen  = (CSL_MCSPI_BUF_SIZE*4);
	dmaConfig.srcAddr  = (Uint32)&(hMcspi->Regs->CH0RXL);
	dmaConfig.destAddr = (Uint32)gmcspiDmaReadBuf;

	dmaRxHandle = CSL_configDmaFormcspi(&dmaRxChanObj,CSL_DMA_CHAN2);
	if(dmaRxHandle == NULL)
	{
		printf("DMA Config for mcspi DMA Read Failed!\n!");
		return(CSL_MCSPI_TEST_FAILED);
	}

    status = DMA_start(dmaRxHandle);
	if(status != CSL_SOK)
	{
	  printf("mcspi Dma Read start Failed!!\n");
	  return(status);
	}

	/* Start mcspi tx Dma transfer */
	status = DMA_start(dmaTxHandle);
	if(status != CSL_SOK)
	{
		printf("mcspi Dma write start Failed!!\n");
		return(status);
	}


	status = McSPI_config(hMcspi);
	if(CSL_SOK != status )
	{
	 printf("McSPI_config() failed\r\n");
	}
	else
	{
	 printf("McSPI_config() success\r\n");
	}


		// Check transfer complete status 
	while(DMA_getStatus(dmaTxHandle));
	printf("DMA Data Write to McSPI Complete\n");

	while(DMA_getStatus(dmaRxHandle));
	printf("DMA Data Read from McSPI Complete\n");
 
   for(i=1;i<CSL_MCSPI_BUF_SIZE;i++)
   {
	buffer.value=gmcspiDmaReadBuf[i];

     if(((buffer.dat[0])!=gmcspiDmaCompBuf[(i*2)]) ||
	 ((buffer.dat[1])!=gmcspiDmaCompBuf[(i*2)+1]))	 
     {
		error++;
		break;
	 }
	
   }


  printf("DMA COMPLETED\r\n");

  /* We are done with mcspi, so close it */ 
   McSPI_close(hMcspi);



  if(error)
  {
	return(CSL_MCSPI_TEST_FAILED);
  }
  else
  {
	return(CSL_MCSPI_TEST_PASSED);
  }
#endif
                     
}



CSL_DMA_Handle CSL_configDmaFormcspi(CSL_DMA_ChannelObj    *dmaChanObj,
                                    CSL_DMAChanNum        chanNum)
{
	CSL_DMA_Handle    dmaHandle;
	CSL_Status        status;

	dmaHandle = NULL;

	/* Open A Dma channel */
	dmaHandle = DMA_open(chanNum, dmaChanObj, &status);
    if(dmaHandle == NULL)
    {
        printf("DMA_open Failed!\n");
    }

	/* Configure a Dma channel */
	status = DMA_config(dmaHandle, &dmaConfig);
    if(status != CSL_SOK)
    {
        printf("DMA_config Failed!\n");
        dmaHandle = NULL;
    }

    return(dmaHandle);
}


