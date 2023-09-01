 Example to verify McSPI full duplex transfer recovery after clock-gating

 POWER - MCSPI FULL DUPLEX RECOVERY FROM CLOCK GATING

 TEST DESCRIPTION:
 		This test code verifies the full duplex functionality of CSL McSPI module. McSPI module
  on the C5517 DSP is used to communicate with SPI MsP430.
  This test code verifies the McSPI functionality with MsP430 SPI .
 
  CSL McSPI module will be configured using McSPI_config() API.Write,Read function used for data transfer operation.
  After successful completion of the write operation read  
  sent to the SPI.Write and read buffers are compared for the data
  verification. A small amount of delay is required after each data operation
  to allow the device get ready for next data operation.
 
 NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSION C5517.
  MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
  c55xx_csl\inc\csl_general.h.
  # define CHIP_C5517 
 

TEST PROCEDURE:

	Load the slave program in msp430
	Load the program in C5517


	Run the slave program first & then the C5517 master program.
	Master program will print the Tx,Rx data .


TEST RESULT:
--------------
    All the CSL APIs should return success
    Run the slave program first & then the master program 
    Data in the read and write buffers should match.(or)Data needs to be verified in Slave program buffer

Data should loopback & printed as below


McSPI Tx Data : 0
McSPI Rx data : ff 
count=1

McSPI Tx Data : 1
McSPI Rx data : fe 
count=2

McSPI Tx Data : 2
McSPI Rx data : 0 
count=3

McSPI Tx Data : 3
McSPI Rx data : 1 
count=4

McSPI Tx Data : 4
McSPI Rx data : 2 
count=5

McSPI Tx Data : 5
McSPI Rx data : 3 
count=6

McSPI Tx Data : 6
McSPI Rx data : 4 
count=7

McSPI Tx Data : 7
McSPI Rx data : 5 
count=8
...
...
...

McSPI Tx data : 19 
McSPI Rx data : 17 
count = 26

McSPI Tx data : 1a 
McSPI Rx data : 18 
count = 27

McSPI Tx data : 1b 
McSPI Rx data : 19 
count = 28

McSPI Tx data : 1c 
McSPI Rx data : 1a 
count = 29

McSPI Tx data : 1d 
McSPI Rx data : 1b 
count = 30

McSPI Tx data : 1e 
McSPI Rx data : 1c 
count = 31

McSPI Tx data : 1f 
McSPI Rx data : 1d 
count = 32
count = 33
count = 34
count = 35
count = 36
count = 37
count = 38
count = 39
count = 40
count = 41
count = 42
count = 43
count = 44
count = 45
count = 46
count = 47
count = 48
count = 49
count = 50
count = 51
count = 52
count = 53
count = 54
count = 55
count = 56
count = 57
count = 58
count = 59
count = 60
count = 61
count = 62
count = 63
count = 64
count = 64

McSPI Tx data : 20 
McSPI Rx data : 1e 
count = 65

McSPI Tx data : 21 
McSPI Rx data : 1f 
count = 66

McSPI Tx data : 22 
McSPI Rx data : 20 
count = 67

McSPI Tx data : 23 
McSPI Rx data : 21 
count = 68

McSPI Tx data : 24 
McSPI Rx data : 22 
count = 69