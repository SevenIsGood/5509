
Load the slave program in msp430

Load the program in c5517



Run the slave program first & c5517 master program.

Master program will print the Tx,Rx data .


-----------------------------
Make sure the resisters are in position in master mode


R277 ,r275,r265 populate

R269 , r266 ,r274 removed 

-----------------------------


observation:
--------------


Data should loopback & printed as below



McSPI Mode test case 
mcspi_open() Success 
McSPI Mode 0 Test start 

McSPI Tx data : 0 
McSPI Rx data : ff 

McSPI Tx data : 1 
McSPI Rx data : 0 

McSPI Tx data : 2 
McSPI Rx data : 0 

McSPI Tx data : 3 
McSPI Rx data : 81 

McSPI Tx data : 4 
McSPI Rx data : 1 

McSPI Tx data : 5 
McSPI Rx data : 82 

McSPI Tx data : 6 
McSPI Rx data : 2 

McSPI Tx data : 7 
McSPI Rx data : 83 

McSPI Tx data : 8 
McSPI Rx data : 3 

McSPI Tx data : 9 
McSPI Rx data : 84 

McSPI Tx data : a 
McSPI Rx data : 4 

McSPI Tx data : b 
McSPI Rx data : 85 

McSPI Tx data : c 
McSPI Rx data : 5 

McSPI Tx data : d 
McSPI Rx data : 86 

McSPI Tx data : e 
McSPI Rx data : 6 

McSPI Tx data : f 
McSPI Rx data : 87 

McSPI Tx data : 10 
McSPI Rx data : 7 

McSPI Tx data : 11 
McSPI Rx data : 88 

McSPI Tx data : 12 
McSPI Rx data : 8 

McSPI Tx data : 13 
McSPI Rx data : 89 

McSPI Tx data : 14 
McSPI Rx data : 9 

McSPI Tx data : 15 
McSPI Rx data : 8a 

McSPI Tx data : 16 
McSPI Rx data : a 


This is not a device failure.
This is the observation on RevE C5517 EVM. On RevD, the Rx data is a sequential 1,2,3,4...
RevE has the SIMO and SOMI lines additionally shared with LCD via mux-chips U82 and U83.
