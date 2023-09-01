Change the MSP430_SPI_DIR switch in SW11 to OFF position.

Load the C5517_SPI_SLAVE code on C5517.

Load the MSP430_SPI_MASTER code in MsP430 

Verify the data transaction by putting the break points in MSP430 program.

the data's are available in MSP430 USCI_BO_SPI_MODE TX,RX buffer 

C5517_SPI_SLAVE  program will print the full duplex data 

-------------------------------------------
NOTE:
1. Start the C5517 slave mcspi side first, then MSP430 master side.
2. SW11 switch 6 away from ON towards dot, which you have confirmed to have already checked.
3. Wait for 30s or so, there will be a continuous print of "The Halfway mark reached" debug printf's. 
4. Then again wait for 30s, after which the C5517 end program will print out the data received.
Will update the same on the Readme.
 
Output:
McSPI Mode test case 
mcspi_open() Success 
McSPI Mode 0 Test start 
Halfway mark reached
Halfway mark reached
Halfway mark reached
Halfway mark reached
Halfway mark reached
...
...
..
.
Halfway mark reached
Halfway mark reached
! xfr complete !
Rcvd Data[0]:0x0001
Rcvd Data[1]:0x0002
Rcvd Data[2]:0x0003
Rcvd Data[3]:0x0004
Rcvd Data[4]:0x0005
Rcvd Data[5]:0x0006
Rcvd Data[6]:0x0007
Rcvd Data[7]:0x0008
...
...
..
.
Rcvd Data[43]:0x002c
Rcvd Data[44]:0x002d
Rcvd Data[45]:0x002e
Rcvd Data[46]:0x002f
Rcvd Data[47]:0x0030
Rcvd Data[48]:0x0031
Rcvd Data[49]:0x0032
Rcvd Data[50]:0x0033
Rcvd Data[51]:0x0034
Rcvd Data[52]:0x0035
Rcvd Data[53]:0x0036
Rcvd Data[54]:0x0037
Rcvd Data[55]:0x0038
Rcvd Data[56]:0x0039
Rcvd Data[57]:0x003a
Rcvd Data[58]:0x003b
Rcvd Data[59]:0x003c
Rcvd Data[60]:0x003d
Rcvd Data[61]:0x003e
Rcvd Data[62]:0x003f
Rcvd Data[63]:0x0040




