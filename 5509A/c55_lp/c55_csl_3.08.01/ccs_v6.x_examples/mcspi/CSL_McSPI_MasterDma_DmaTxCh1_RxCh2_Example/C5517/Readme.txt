Example code to verify McSPI-host data transactions using DMA Channels 1 and 2

McSPI DMA Chx 1 AND 2

TEST DESCRIPTION:
	This test uses DMA for facilitating data movement from CPU write buffer to CH0TX
  and CH0RX to CPU read buffer. The C5517 McSPI is configured as master and MSP430 host 
  as the SPI slave. Data integrity is ensured by comparing the data being transmitted and 
  data received back, albeit with some delay.
  DMA is configured for 1 32-bit word burst transaction. DMA0 Channel1 is used for Tx and 
  DMA0 Channel2 is used for Rx.
 

  This test code verifies the dma functionality of McSPI module. McSPI module
  on the C5517 DSP is used to communicate with SPI MsP430 through DMA.
  This test code verifies the McSPI functionality using MsP430 SPI as slave module.
   
  CSL McSPI module will be configured using McSPI_config() API.Write,Read function
  used for data transfer operation. After successful completion of the write 
  operation read sent to the SPI.Write and read buffers are compared for the data
  verification. A small amount of delay is required after each data operation
  to allow the device get ready for next data operation.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5517.
  MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
  CHIP_C5517

TEST PROCEDURE:

Load the C5517_SPI_MASTER code on C5517.
Load the MSP430_SPI_SLAVE code in MSP430 .

Run the slave code & then start the master code.
Verify the data transaction by putting the break points in MSP430_SPI_SLAVE ISR

Master sent data's are available in MSP430 USCI_BO_SPI_MODE TX,RX buffer 
Put break points in C5517_SPI_MASTER end of the code gmcspiDmaReadBuf,gmcspiDmaWriteBuf data's are availble

--------
SW4 Push button HPI_ON,HPI_SEL_IF is off.

Run the DMA test @ 100 Mhz PLL .

Other freq will read improper data

TEST RESULT:
  All the CSL APIs should return success
  Run the slave program first & then the master program 
  Data in the read and write buffers should match.(or)Data needs to be verified in Slave program buffer
