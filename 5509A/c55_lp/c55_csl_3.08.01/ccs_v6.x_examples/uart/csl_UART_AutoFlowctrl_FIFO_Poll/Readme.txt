This test is to verify the CSL UART module operation in Poll mode with FIFOs and Auto Flow Control enabled.

UART peripheral is configured with the following parameters
Baud Rate - 2400.
Data bits - 8.
FIFO Mode - Trigger level 04 and DMA disabled
Parity - No parity.
Stop bits - 1.
Internal Loopback - disable
Flow control - Enable.

Will Send the FIFO trigger level no of bytes using the CPU poll mode and receive using 
CPU poll mode and compares the data. In this test case we have configured the FIFO and 
set the trigger to one byte.


Jumpers on-board : 
 A jumper on UART connector from TX(pin 3) to RX(pin 2).
 A jumper on JP4 on C5517 EVM, and JP3 in the case of C5515 EVM. (UART_EN)


Ensure the bytes transmitted are received and there is no mismatch. 