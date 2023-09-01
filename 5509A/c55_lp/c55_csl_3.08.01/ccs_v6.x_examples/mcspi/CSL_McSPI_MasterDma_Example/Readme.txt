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

