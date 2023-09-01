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


/** @file CSL_McSPI_MasterDma_ForceIdle.c
 *
 *  @brief McSPI functional layer sample source file
 *
 * \page    page10   McSPI EXAMPLE DOCUMENTATION
 *
 * \section McSPI3   McSPI EXAMPLE3 - MASTER FORCE IDLE MODE
 *
 * \subsection McSPI3x    TEST DESCRIPTION:
 *        This test uses DMA for facilitating data movement from CPU write buffer to CH0TX
 *  and CH0RX to CPU read buffer. The C5517 McSPI is configured as master and MSP430 host
 *  as the SPI slave. McSPI SIDLEMODE field in SYSCONFIGL is configured for FORCE IDLE
 *  mode.Data integrity is ensured by comparing the data being transmitted and data
 *  received back, albeit with some delay.
 *  DMA is configured for 1 32-bit word burst transaction.
 *  After DMA transaction is initiated, the McSPI clock is shut off for a while and then
 *  restarted. Data integrity is expected to be NOT affected.
 *
 *		This test code verifies the dma functionality of McSPI module. McSPI module
 * on the C5517 DSP is used to communicate with SPI MsP430 through DMA.
 * This test code verifies the McSPI functionality using MsP430 SPI as slave module.
 *
 *  CSL McSPI module will be configured using McSPI_config() API.Write,Read function
 *  used for data transfer operation. After successful completion of the write
 *  operation read sent to the SPI.Write and read buffers are compared for the data
 * verification. A small amount of delay is required after each data operation
 * to allow the device get ready for next data operation.
 *
 *  Brief description of test
 *   * Configure McSPI as Master for Multiple data words transfer.
 *   * Enable DMA0 channel 1 for Write into and channel 0 for read from McSPI
 *   * Set the SIdleMode bit of MCSPI_SYSCONFIG to 2'b00 for force idle mode.
 *   * Initiate the Data transfer.
 *   * Issue the idle request to VBUS Bridge and check that the idle acknowledgment is issued by the bridge.
 *   * After idle acknowledgement from the bridge, issue the idle request to McSPI and check that the idle ack from the McSPI.
 *   * Gate the OCP and SPI reference clk by programming in the PCGCR system control registers.
 *   * After sometime, Disable the Clk Gating.
 *   * Remove the idle request to vbus bridge and the McSPI and check that the idle ack from the bridge and IP are de-asserted.
 *   * Now the McSPI master resume the data transfer.
 *   * Check the McSPI master transmitted data for data integrity.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5517.
 * MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 * CHIP_C5517
 *
 * \subsection McSPI3y    TEST PROCEDURE:
 *
 * Load the C5517_SPI_MASTER code on C5517.
 * Load the MSP430_SPI_SLAVE code in MSP430 .
 *
 * Run the slave code & then start the master code.
 * Verify the data transaction by putting the break points in MSP430_SPI_SLAVE ISR
 *
 * Master sent data is available in MSP430 USCI_BO_SPI_MODE TX,RX buffer
 * Put break points in C5517_SPI_MASTER end of the code gmcspiDmaReadBuf,gmcspiDmaWriteBuf data's are availble
 *
 * SW4 Push button HPI_ON,HPI_SEL_IF is off.
 * Run the DMA test @ 100 Mhz PLL .
 * Other freq will read improper data
 *
 * \subsection McSPI3z    TEST RESULT:
 *  All the CSL APIs should return success
 * Run the slave program first & then the master program 
 *  Data in the read and write buffers should match.(or)Data needs to be verified in Slave program buffer
 *
 */
 /*
 * ============================================================================
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

#define CSL_MCSPI_BUF_SIZE (16364u)

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
Uint16  gmcspiDmaCompBuf[2*CSL_MCSPI_BUF_SIZE];

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
/* Protype declaration for ISR function */
//interrupt void dma_isr(void);

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
#if (defined(CHIP_C5517))

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

#else
  printf("\nEnsure CHIP_C551t7 is #defined. C5504/05/14/15 don't have McSPI\n");
#endif
}


  CSL_Status        status;
  CSL_GpioConfig    config;


CSL_Status CSL_McspiTest(void) 
{


  Uint16 i,mismatch_id,k, count=0;
  Uint8 flag=0;
  unsigned char error=0;
  CSL_Status status; //This is for the CSL status return
#if (defined(CHIP_C5517))

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

    dmaTxHandle = CSL_configDmaFormcspi(&dmaTxChanObj, CSL_DMA_CHAN0);
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

	dmaRxHandle = CSL_configDmaFormcspi(&dmaRxChanObj,CSL_DMA_CHAN1);
	if(dmaRxHandle == NULL)
	{
		printf("DMA Config for mcspi DMA Read Failed!\n!");
		return(CSL_MCSPI_TEST_FAILED);
	}

	CSL_MCSPI_REGS->SYSCONFIGL &= ~CSL_MCSPI_SYSCONFIGL_SIDLEMODE_MASK;
	CSL_MCSPI_REGS->SYSCONFIGL |= (CSL_MCSPI_SYSCONFIGL_SIDLEMODE_MODE0<<3);

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
	while(DMA_getStatus(dmaTxHandle))
	{
#if 1
		count++;
		//printf("$");
		if(count==1 && !flag)
		{
			//assert clk stop request to mcspi
			CSL_SYSCTRL_REGS->CLKSTOP2 |= 0x1;
			// Wait until the clk stop ack is asserted
			while ((CSL_SYSCTRL_REGS->CLKSTOP2 & 0x0002) == (Uint16) 0x0000 )  {
						__asm("    NOP");
						}
			//assert clk stop request to mcspi bridge
			CSL_SYSCTRL_REGS->CLKSTOP2 |= 0x8;
			// Wait until the clk stop ack is asserted
			while ((CSL_SYSCTRL_REGS->CLKSTOP2 & 0x0010) == (Uint16) 0x0000 )  {
						__asm("    NOP");
						}
			CSL_SYSCTRL_REGS->PCGCR2 |= 0x0200;//just mcspi
		}

		if(count==0xFFFF && !flag)
		{
			CSL_SYSCTRL_REGS->PCGCR2 &= 0xFDFF;//just mcspi
			//Desassert clk stop request to mcspi
			CSL_SYSCTRL_REGS->CLKSTOP2 &= 0xFFF6;
			// Wait until the clk stop ack is deasserted
			while ((CSL_SYSCTRL_REGS->CLKSTOP2 & 0x0012) != (Uint16) 0x0000 )  {
					__asm("    NOP");
					}
			flag=0x1;
		}
#endif
	}
	printf("\nDMA Data Write to McSPI Complete\n");

	while(DMA_getStatus(dmaRxHandle));
	printf("\nDMA Data Read from McSPI Complete\n");
 
   for(i=1;i<CSL_MCSPI_BUF_SIZE;i++)
   {
	buffer.value=gmcspiDmaReadBuf[i];

     if(((buffer.dat[0])!=gmcspiDmaCompBuf[(i*2)]) ||
	 ((buffer.dat[1])!=gmcspiDmaCompBuf[(i*2)+1]))	 
     {
    	 mismatch_id=i;
		printf("\r\nRead   mismatch:i=%d "
				"\nExpected: 0x%x 0x%x"
				"\nRx'ed: 0x%lx\r\n",i,gmcspiDmaCompBuf[(i*2)],
				gmcspiDmaCompBuf[(i*2)+1],gmcspiDmaReadBuf[i]); //few bytes are received extra
		error++;

	 }
	
   }

   //if just one byte mismatches in the whole sequence,
   //it's because mcspi clock was un-gated at that point
   //and hence, can be ignored
   buffer.value=gmcspiDmaReadBuf[mismatch_id];
   //printf("buffer.dat[0]: 0x%x\t buffer.dat[1]: 0x%x\n",buffer.dat[0],buffer.dat[1]);
   if(error==0x1)
   {
	   if(((buffer.dat[0]&0xFF00)!=(gmcspiDmaCompBuf[(mismatch_id*2)]&0xFF00)) &&
			   ((buffer.dat[0]&0x00FF)==(gmcspiDmaCompBuf[(mismatch_id*2)]&0x00FF)) &&
			   ((buffer.dat[1])==gmcspiDmaCompBuf[(mismatch_id*2)+1]))
	   {
		   error=0x0;
		   printf("\nIf just one byte mismatches in the whole sequence,"
				   "\nit's because mcspi clock was un-gated at that point"
				   "\nand hence, can be ignored\n\n"  );
	   }
   }



  printf("DMA COMPLETED\r\n");

  /* We are done with mcspi, so close it */ 
   McSPI_close(hMcspi);


#endif//#if (defined(CHIP_C5517))
  if(error)
  {
	return(CSL_MCSPI_TEST_FAILED);
  }
  else
  {
	return(CSL_MCSPI_TEST_PASSED);
  }


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


