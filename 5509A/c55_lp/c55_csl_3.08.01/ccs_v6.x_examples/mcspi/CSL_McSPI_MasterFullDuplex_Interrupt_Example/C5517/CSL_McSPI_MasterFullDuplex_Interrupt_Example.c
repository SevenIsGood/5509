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


/** @file CSL_McSPI_MasterFullDuplex_Interrupt.c
 *
 *  @brief McSPI functional layer sample source file
 *
 *
 * \page    page10   McSPI EXAMPLE DOCUMENTATION
 *
 * \section McSPI8   McSPI EXAMPLE8 - MASTER FULL DUPLEX TEST
 *
 * \subsection McSPI8x    TEST DESCRIPTION:
 *		This test code verifies the full duplex functionality of CSL McSPI module. McSPI module
 * on the C5517 DSP is used to communicate with SPI MsP430.
 * This test code verifies the McSPI functionality with MsP430 SPI .
 *
 * CSL McSPI module will be configured using McSPI_config() API.Write,Read function used for data transfer operation.
 * After successful completion of the write operation read  
 * sent to the SPI.Write and read buffers are compared for the data
 * verification. A small amount of delay is required after each data operation
 * to allow the device get ready for next data operation.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5517.
 * MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h. "#define CHIP_C5517"
 *
 * \subsection McSPI8y    TEST PROCEDURE:
 *  @li Open the CCS and connect the target (C5517 EVM-Master board)
 *  @li Open the project "CSL_McSPI_MasterFullDulpex_Example.pjt" and build it
 *  @li Load the program on to the target
 *  @li Set the PLL frequency to 12.288MHz
 *  @li Run the program and observe the test result
 *  @li Repeat the test at PLL frequencies 40, 60, 75 and 100MHz
 *  @li Repeat the test in Release mode
 *
 *  Manual test, load/run MSP430_SPI_SlaveFullDuplex_Interrupt_Example on MSP430 and 
 *  CSL_McSPI_MasterFullDuplex_Interrupt_Example on C5517
 *  Load the slave program in msp430
 *
 *  Load the program in c5517
 *
 *  Run the slave program first & c5525 master program.
 *
 *  Master program will print the Tx,Rx data .
 *
 *  Make sure the resisters are in position in master mode
 *
 *  R277 ,r275,r265 populate
 *
 *  R269 , r266 ,r274 removed
 *
 *  Observation:
 *
 *  Data should loopback & printed as below
 *
 *
 *
 *  McSPI RxL data : 122
 *
 *  McSPI RxL data : 123
 *
 *  McSPI RxL data : 124
 *
 *  McSPI RxL data : 125
 *
 *  McSPI RxL data : 126
 *
 *  McSPI RxL data : 127
 *
 *  McSPI RxL data : 128
 *
 *  McSPI RxL data : 129
 *
 *  McSPI RxL data : 130
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

#include <csl_mcspi.h>
#include <stdio.h>
#include <csl_general.h>
#include <csl_intc.h>

#include <csl_i2c.h>
#include "csl_gpio.h"


extern void VECSTART(void);
interrupt void McSpiIsr(void);
#define CSL_MCSPI_BUF_LEN 64
Uint8 readComplete=0, writeComplete=0;

CSL_McSpiHandle  hMcspi;
CSL_McSpiObj 	 McSpiObj; 



CSL_I2cSetup     i2cSetup;
CSL_I2cConfig    i2cConfig;


CSL_GpioObj    gpioObj;
CSL_GpioObj    *hGpio;


#define CSL_TEST_PASSED      (0)
#define CSL_TEST_FAILED      (1)

Uint16 mcspiWriteBuff[CSL_MCSPI_BUF_LEN]={0x1111, 0x2222, 0x3333, 0x4444,
										0x5555, 0x6666, 0x7777, 0x8888,
										0x9999, 0xAAAA, 0xBBBB, 0xCCCC,
										0xDDDD, 0xEEEE, 0xFFFF, 0x0000,
										0x1111, 0x2222, 0x3333, 0x4444,
										0x5555, 0x6666, 0x7777, 0x8888,
										0x9999, 0xAAAA, 0xBBBB, 0xCCCC,
										0xDDDD, 0xEEEE, 0xFFFF, 0x0000,
						0x1111, 0x2222, 0x3333, 0x4444,
						0x5555, 0x6666, 0x7777, 0x8888,
						0x9999, 0xAAAA, 0xBBBB, 0xCCCC,
						0xDDDD, 0xEEEE, 0xFFFF, 0x0000,
						0x1111, 0x2222, 0x3333, 0x4444,
						0x5555, 0x6666, 0x7777, 0x8888,
						0x9999, 0xAAAA, 0xBBBB, 0xCCCC,
						0xDDDD, 0xEEEE, 0xFFFF, 0x0000};

Uint16 mcspiReadBuff[CSL_MCSPI_BUF_LEN];
Uint16 dummy=0;
Uint32 tx_value,rd_value,rd_value1,rd_value2;

CSL_Status ioExpander_Setup(void);
CSL_Status ioExpander_Read(Uint16 port, Uint16 pin, Uint16 *rValue);
CSL_Status ioExpander_Write(Uint16 port, Uint16 pin, Uint16 value);

CSL_Status CSL_McspiTest(void);

volatile Int16 PaSs_StAtE = 0x0001; 
volatile Int16 PaSs       = 0x0000; 

McSPI_Config     McSPIHwConfig;


CSL_GpioObj    gpioObj;
CSL_GpioObj    *hGpio;




// Holds the IRQ event ids for Rx and Tx interrupts
Uint16 rxEventId = 0xFFFF, txEventId = 0xFFFF;
 

CSL_McSpiHandle  hMcspi;
CSL_McSpiObj 	 McSpiObj;

volatile Uint16   bufIndex=0;
volatile Uint8 xfr_complete_flag=0x0;

void main(void)
{
#if (defined(CHIP_C5517))
	CSL_Status status; //This is for the CSL status return
	
    printf("\r\nMcSPI Mode test case \r\n");

	status = CSL_McspiTest();

	if( CSL_TEST_PASSED == status )
	{
	 printf("\r\nMcSPI Mode test case passed\r\n");
	}
	else
	{
	 printf("\r\nMcSPI Mode test case failed\r\n");
	 PaSs_StAtE=0;

	}
   
	
    
    PaSs = PaSs_StAtE; 
#else
    printf("\nEnsure CHIP_C5517 is #defined. C5504/05/14/15 don't have McSPI\n");
#endif
    
}



CSL_Status CSL_McspiTest(void)
{

#if (defined(CHIP_C5517))
  Uint16 mode = 0  ;
  CSL_Status Status;
  Uint16 loop_count = 0 ;

/****    SEL_MMC1 High    ***/


ioExpander_Setup();

//Call this fn for enabling

// port,pin, value
 ioExpander_Write(1, 4, 0); // Select 
// port,pin, value
 ioExpander_Write(1, 5, 0); // Select



 CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP1MODE, MODE1);


  /* Open mcspi Port 0, this will return a mcspi handle that will */
  /* be used in calls to other CSl functions.                     */ 
  
  hMcspi =  McSPI_open(McSPI_POLLING_MODE,&McSpiObj,&Status);
  McSPI_close(hMcspi);  // close if clock already started
  hMcspi =  McSPI_open(McSPI_POLLING_MODE,&McSpiObj,&Status);

  if ( (hMcspi == NULL) || (Status != CSL_SOK) )
  {
   printf("mcspi_open() Failed \n");
  }
  else
  {
	printf("mcspi_open() Success \n");
  }


#if 1
  /* Disable interrupt */
	IRQ_globalDisable();

//	 Clear any pending interrupts
	IRQ_clearAll();

//	 Disable all the interrupts
	IRQ_disableAll();

	IRQ_setVecs((Uint32)(&VECSTART));

//	 Configuring Interrupt

	IRQ_plug (PROG2_EVENT, &McSpiIsr);

	IRQ_enable(PROG2_EVENT);

//	 Enabling Interrupt
	IRQ_globalEnable();
#endif

	hMcspi->McSPIHwConfig.mode       = McSPI_MASTER;
    hMcspi->McSPIHwConfig.duplex     = McSPI_FULL_DUPLEX;
    hMcspi->McSPIHwConfig.wLen       = McSPI_WORD_LENGTH_8;
	hMcspi->McSPIHwConfig.dataDelay  = McSPI_DATA_DLY4;
    hMcspi->McSPIHwConfig.ClkDiv     = CSL_MCSPI_CH0CONFL_CLKD_DIV16;//CSL_MCSPI_CH0CONFL_CLKD_DIV4;
    hMcspi->McSPIHwConfig.clkPh      = McSPI_CLK_PH_EVEN_EDGE;   //even edge ,polarity low important
    hMcspi->McSPIHwConfig.clkPol     = McSPI_CLKP_LOW;
    hMcspi->ch					     = McSPI_CS_NUM_0;
    hMcspi->McSPIHwConfig.csPol      = McSPI_CSP_ACTIVE_LOW;
    hMcspi->McSPIHwConfig.inSel      = McSPI_RX_SPIDAT0;
	hMcspi->McSPIHwConfig.enDat0_1   = (McSPI_TxEn_Dat0_1)McSPI_TX_ENABLE_SPIDAT0;
	hMcspi->McSPIHwConfig.fifo   	 = McSPI_FIFO_EN;

	Status = McSPI_config(hMcspi);
	if( CSL_SOK != Status )
	{
	 printf("\r\n McSPI_Config() is failed\r\n");
	}


	printf("McSPI Mode %d Test start \r\n",mode);
	
	for(loop_count=0; loop_count<CSL_MCSPI_BUF_LEN;loop_count++)
		mcspiReadBuff[loop_count++]=0x0;

	CSL_SYSCTRL_REGS->MSIAER |= 0x1;


	while(1)
		{
			if(xfr_complete_flag)
			{
				printf("! xfr complete !\n");
				xfr_complete_flag=0x0;
				for(loop_count=0; loop_count<CSL_MCSPI_BUF_LEN;loop_count++)
					printf("Rcvd Data[%d]:0x%04x\n",loop_count,mcspiReadBuff[loop_count]);
			}

		}
    
#endif//#if (defined(CHIP_C5517))

 }

/*
	This is ISR for the McSPI.
	This function returns:
	Void               -void
*/
interrupt void McSpiIsr()
{
#if (defined(CHIP_C5517))
	Uint16 	      getWLen;

	CSL_SYSCTRL_REGS->MSIAER &= 0xFFFE;//Clear McSPI Interrupt Aggregation Enable bit
	CSL_SYSCTRL_REGS->MSIAFR |= 0x0001;//Clear McSPI Interrupt Aggregation Enable flag


		      	getWLen = CSL_FEXT(hMcspi->Regs->CH0CONFL,MCSPI_CH0CONFL_WL);
		             if(getWLen >=  McSPI_MAX_WORD_LEN)
		             {
		            	 printf ("Invalid WordLength\n");
		             }

		         if(McSPI_TxEmptyInt_Chk(hMcspi)){
					 hMcspi->Regs->CH0TXL = (Uint16)(mcspiWriteBuff[bufIndex]);

			      	  	  if(McSPI_RxFullInt_Chk(hMcspi))
			      	  		  mcspiReadBuff[bufIndex] = hMcspi->Regs->CH0RXL;

					  	  bufIndex++;
					  if(getWLen >=  McSPI_WORD_LENGTH_16){
						  hMcspi->Regs->CH0TXU = (Uint16)(mcspiWriteBuff[bufIndex]);

			              if(McSPI_RxFullInt_Chk(hMcspi))
			            	  mcspiReadBuff[bufIndex] = hMcspi->Regs->CH0RXU;

						  bufIndex++;
					  }else{
						  hMcspi->Regs->CH0TXU = (Uint16)0x0000;
					  }
					  while(!(CSL_MCSPI_REGS->CH0STATL & CSL_MCSPI_CH0STATL_EOT_MASK ));
				  }



	    if(bufIndex < CSL_MCSPI_BUF_LEN)
	    {
	    	CSL_SYSCTRL_REGS->MSIAER |= 0x0001;//Set McSPI Interrupt Aggregation Enable bit
	    	xfr_complete_flag = 0x1;

	    }
#endif//#if (defined(CHIP_C5517))
}



