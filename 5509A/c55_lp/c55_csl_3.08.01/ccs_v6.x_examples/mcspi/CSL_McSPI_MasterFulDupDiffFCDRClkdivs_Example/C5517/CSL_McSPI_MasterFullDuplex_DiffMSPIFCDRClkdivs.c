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


/** @file CSL_McSPI_MasterFullDuplex_DiffMSPIFCDRClkdivs.c
 *
 *  @brief McSPI Example to verify the different McSPI clk configurations.
 *
 * \page    page10   McSPI EXAMPLE DOCUMENTATION
 *
 * \section McSPI5   McSPI EXAMPLE5 - MASTER different McSPI clk configurations
 *
 * \subsection McSPI5x    TEST DESCRIPTION:
 *                This test code verifies the full duplex functionality
 *  of CSL McSPI module. McSPI module on the C5517 DSP is used to
 *  communicate with SPI MsP430. This test code verifies the McSPI
 *  functionality with MsP430 SPI.
 *
 *  CSL McSPI module will be configured using McSPI_config() API.
 *  Write,Read function used for data transfer operation. After
 *  successful completion of the write operation read sent to the
 *  SPI.Write and read buffers are compared for the data
 *  verification. A small amount of delay is required after each
 *  data operation to allow the device get ready for next data
 *  operation.
 *
 *  Every 16 transactions, gate the McSPI clk and change FCDIV
 *  and OCDIV fields in MSPIFCDR register and resume the McSPI
 *  transfer.
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5517.
 * MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h. "#define CHIP_C5517"
 *
 * \subsection McSPI5y    TEST PROCEDURE:
 *
 * Load the slave program in msp430
 * Load the program in C5517
 *
 * Run the slave program first & C5517 master program.
 * Master program will print the Tx,Rx data .
 *
 *
 * observation:
 *
 *
 * Data should loopback & printed as below
 *
 *
 *
 * McSPI Tx data : 0
 * McSPI Rx data : ff
 *
 * McSPI Tx data : 1
 * McSPI Rx data : fe
 *
 * McSPI Tx data : 2
 * McSPI Rx data : 0
 *
 * McSPI Tx data : 3
 * McSPI Rx data : 1
 * McSPI Tx data : 4
 * McSPI Rx data : 2
 *
 * McSPI Tx data : 5
 * McSPI Rx data : 3
 *
 * McSPI Tx data : 6
 * McSPI Rx data : 4
 *
 * ...
 * ...
 * ...
 *
 *
 * McSPI Tx data : d
 * McSPI Rx data : b
 *
 * McSPI Tx data : e
 * McSPI Rx data : c
 *
 * McSPI Tx data : f
 * McSPI Rx data : d
 * count = 16
 * MSPIFCDR_OCDIV = 0x1
 * MSPIFCDR_FCDIV = 0x1
 * count = 32
 *
 * McSPI Tx data : 10
 * McSPI Rx data : e
 *
 * McSPI Tx data : 11
 * McSPI Rx data : f
 *
 * McSPI Tx data : 12
 * McSPI Rx data : 10
 *
 * McSPI Tx data : 13
 * McSPI Rx data : 11
 *
 * McSPI Tx data : 14
 * McSPI Rx data : 12
 *
 * McSPI Tx data : 15
 * McSPI Rx data : 13
 *
 *
 * ...
 * ...
 * ...
 *
 * McSPI Tx data : 1e
 * McSPI Rx data : 1c
 *
 * McSPI Tx data : 1f
 * McSPI Rx data : 1d
 * count = 48
 * MSPIFCDR_OCDIV = 0x1
 * MSPIFCDR_FCDIV = 0x2
 * count = 64
 *
 * McSPI Tx data : 20
 * McSPI Rx data : 1e
 *
 * McSPI Tx data : 21
 * McSPI Rx data : 1f
 *
 * McSPI Tx data : 22
 * McSPI Rx data : 20
 *
 * McSPI Tx data : 23
 * McSPI Rx data : 21
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



CSL_McSpiHandle  hMcspi;
CSL_McSpiObj 	 McSpiObj; 



CSL_I2cSetup     i2cSetup;
CSL_I2cConfig    i2cConfig;


CSL_GpioObj    gpioObj;
CSL_GpioObj    *hGpio;


#define CSL_TEST_PASSED      (0)
#define CSL_TEST_FAILED      (1)
#define CSL_MIBSPI_BUF_LEN   (5)

Uint16 mibspiWriteBuff[CSL_MIBSPI_BUF_LEN];
Uint16 mcspiReadBuff[CSL_MIBSPI_BUF_LEN];
Uint32 tx_value,rd_value,rd_value1,rd_value2;

CSL_Status CSL_McspiTest(void);

volatile Int16 PaSs_StAtE = 0x0001; 
volatile Int16 PaSs       = 0x0000; 

McSPI_Config     McSPIHwConfig;


CSL_GpioObj    gpioObj;
CSL_GpioObj    *hGpio;



extern void VECSTART(void);
// Holds the IRQ event ids for Rx and Tx interrupts
Uint16 rxEventId = 0xFFFF, txEventId = 0xFFFF;
 

CSL_McSpiHandle  hMcspi;
CSL_McSpiObj 	 McSpiObj;

CSL_Status ioExpander_Setup(void);
CSL_Status ioExpander_Read(Uint16 port, Uint16 pin, Uint16 *rValue);
CSL_Status ioExpander_Write(Uint16 port, Uint16 pin, Uint16 value);

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



CSL_GpioConfig    config;

unsigned short 	  ioindata1_rdval=0;


CSL_Status CSL_McspiTest(void) 
{

#if (defined(CHIP_C5517))
  Uint16 mode = 0  ;
  CSL_Status Status=0;

  Uint32 i=0,count=0x0;
  Uint16 mspifcdr_fcdiv=0x0, mspifcdr_ocdiv=0x1, flag=0x0;

/****    SEL_MMC1 High    ***/
  ioExpander_Setup();

  //Call this fn for enabling

  // port,pin, value

  // port,pin, value
  ioExpander_Write(1, 4, 0); // Select
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


	hMcspi->McSPIHwConfig.mode       = McSPI_MASTER;
    hMcspi->McSPIHwConfig.duplex     = McSPI_FULL_DUPLEX;
    hMcspi->McSPIHwConfig.wLen       = McSPI_WORD_LENGTH_8;
	hMcspi->McSPIHwConfig.dataDelay  = McSPI_DATA_DLY4;
    hMcspi->McSPIHwConfig.ClkDiv     = CSL_MCSPI_CH0CONFL_CLKD_DIV4;
    hMcspi->McSPIHwConfig.clkPh      = McSPI_CLK_PH_EVEN_EDGE;   //even edge ,polarity low important
    hMcspi->McSPIHwConfig.clkPol     = McSPI_CLKP_LOW;
    hMcspi->ch					     = McSPI_CS_NUM_0;
    hMcspi->McSPIHwConfig.csPol      = McSPI_CSP_ACTIVE_LOW;
    hMcspi->McSPIHwConfig.inSel      = McSPI_RX_SPIDAT0;
	hMcspi->McSPIHwConfig.enDat0_1   = (McSPI_TxEn_Dat0_1)McSPI_TX_ENABLE_SPIDAT0;


	Status = McSPI_config(hMcspi);
	if( CSL_SOK != Status )
	{
	 printf("\r\n McSPI_Config() is failed\r\n");
	}


	printf("McSPI Mode %d Test start \r\n",mode);
	
	tx_value = 0; // u45 pin 9

	rd_value = CSL_MCSPI_REGS->CH0STATL;
    CSL_MCSPI_REGS->CH0STATL = rd_value & CSL_MCSPI_CH0STATL_RXS_MASK;
    
	while(1)
	{
		if(!flag && (CSL_MCSPI_REGS->CH0STATL & CSL_MCSPI_CH0STATL_TXS_MASK)) // Check if TXBUFF is EMPTY
		{
			 
			 CSL_MCSPI_REGS->CH0TXL = (Uint16)tx_value;
			 CSL_MCSPI_REGS->CH0TXU = (Uint16)(tx_value>>16) ;
			 printf("\r\nMcSPI Tx data : %lx \r\n",tx_value);//need this line for delay
			 while(!(CSL_MCSPI_REGS->CH0STATL & CSL_MCSPI_CH0STATL_EOT_MASK)); // WAIT till END of Txn
			 tx_value ++;			
			

			if(CSL_MCSPI_REGS->CH0STATL & CSL_MCSPI_CH0STATL_RXS_MASK)			// Check if Data is Received
			{
		      rd_value = hMcspi->Regs->CH0RXL;
 			  printf("McSPI Rx data : %lx \r\n",rd_value&0xFF);
			  
			  rd_value =CSL_MCSPI_REGS->CH0STATL;
			  CSL_MCSPI_REGS->CH0STATL = rd_value & CSL_MCSPI_CH0STATL_RXS_MASK; // Clear RXFULL Mask
             
            }
			for(i = 0 ; i < 0X03FFFFF ; i++);  // Delay
        }
		count++;//printf("count = %ld\n",count);
				if(count%0x10==0)
				{
					if(!flag)
					{
						//assert clk stop request to mcspi
						CSL_SYSCTRL_REGS->CLKSTOP2 |= 0x9;
						// Wait until the clk stop ack is asserted
						while ((CSL_SYSCTRL_REGS->CLKSTOP2 & 0x0012) == (Uint16) 0x0000 )  {
							__asm("    NOP");
							}
						CSL_SYSCTRL_REGS->PCGCR2 |= 0x0200;//just mcspi
						for(i = 0 ; i < 0X03FFFFF ; i++);  // Delay
						flag=0x1;

						if(mspifcdr_fcdiv==0xF)
						{
							mspifcdr_fcdiv=0x0;
							CSL_SYSCTRL_REGS->MSPIFCDR &= ~CSL_SYS_MSPIFCDR_OCDIV_MASK;
							mspifcdr_ocdiv++;
							if(mspifcdr_ocdiv==0x3)
								mspifcdr_ocdiv=0x0;
							CSL_SYSCTRL_REGS->MSPIFCDR |= (mspifcdr_ocdiv<<14);

						}
						CSL_SYSCTRL_REGS->MSPIFCDR &= ~CSL_SYS_MSPIFCDR_FCDIV_MASK;
						mspifcdr_fcdiv++;
						CSL_SYSCTRL_REGS->MSPIFCDR |= mspifcdr_fcdiv;
						printf("count = %ld\n",count);
						printf("MSPIFCDR_OCDIV = 0x%x\n",mspifcdr_ocdiv);
						printf("MSPIFCDR_FCDIV = 0x%x\n",mspifcdr_fcdiv);

					}
					else
					{printf("count = %ld\n",count);

					CSL_SYSCTRL_REGS->PCGCR2 &= 0xFDFF;//just mcspi
					//Desassert clk stop request to mcspi
					CSL_SYSCTRL_REGS->CLKSTOP2 &= 0xFFF6;
					// Wait until the clk stop ack is deasserted
					while ((CSL_SYSCTRL_REGS->CLKSTOP2 & 0x0012) != (Uint16) 0x0000 )  {
					     __asm("    NOP");
					     }
					flag=0x0;
					}
				}
		     
	}//while loop end   
#endif//#if (defined(CHIP_C5517))
 }

