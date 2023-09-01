Example to verify the different McSPI clk configurations.

MCSPI DIFERENT CLKS

TEST DESCRIPTION:
 		This test code verifies the full duplex functionality 
  of CSL McSPI module. McSPI module on the C5517 DSP is used to
  communicate with SPI MsP430. This test code verifies the McSPI
  functionality with MsP430 SPI.
 
  CSL McSPI module will be configured using McSPI_config() API.
  Write,Read function used for data transfer operation. After
  successful completion of the write operation read sent to the
  SPI.Write and read buffers are compared for the data 
  verification. A small amount of delay is required after each 
  data operation to allow the device get ready for next data 
  operation.

  Every 16 transactions, gate the McSPI clk and change FCDIV 
  and OCDIV fields in MSPIFCDR register and resume the McSPI 
  transfer.
 
  NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5517.
  MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
  # define CHIP_C5517 

TEST PROCEDURE:

Load the slave program in msp430
Load the program in C5517

Run the slave program first & C5517 master program.
Master program will print the Tx,Rx data .


observation:
--------------


Data should loopback & printed as below



McSPI Tx data : 0 
McSPI Rx data : ff 

McSPI Tx data : 1 
McSPI Rx data : fe 

McSPI Tx data : 2 
McSPI Rx data : 0 

McSPI Tx data : 3 
McSPI Rx data : 1 

McSPI Tx data : 4 
McSPI Rx data : 2 

McSPI Tx data : 5 
McSPI Rx data : 3 

McSPI Tx data : 6 
McSPI Rx data : 4 

...
...
...


McSPI Tx data : d 
McSPI Rx data : b 

McSPI Tx data : e 
McSPI Rx data : c 

McSPI Tx data : f 
McSPI Rx data : d 
count = 16
MSPIFCDR_OCDIV = 0x1
MSPIFCDR_FCDIV = 0x1
count = 32

McSPI Tx data : 10 
McSPI Rx data : e 

McSPI Tx data : 11 
McSPI Rx data : f 

McSPI Tx data : 12 
McSPI Rx data : 10 

McSPI Tx data : 13 
McSPI Rx data : 11 

McSPI Tx data : 14 
McSPI Rx data : 12 

McSPI Tx data : 15 
McSPI Rx data : 13 


...
...
...

McSPI Tx data : 1e 
McSPI Rx data : 1c 

McSPI Tx data : 1f 
McSPI Rx data : 1d 
count = 48
MSPIFCDR_OCDIV = 0x1
MSPIFCDR_FCDIV = 0x2
count = 64

McSPI Tx data : 20 
McSPI Rx data : 1e 

McSPI Tx data : 21 
McSPI Rx data : 1f 

McSPI Tx data : 22 
McSPI Rx data : 20 

McSPI Tx data : 23 
McSPI Rx data : 21 

