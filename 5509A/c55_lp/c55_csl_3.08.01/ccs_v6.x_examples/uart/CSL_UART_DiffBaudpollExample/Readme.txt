 CSL_UART_DiffBaudpollExample

 1. Connect the RS232 cable; One end to the UART port on the C5535/C5515/C5517 EVM(J13/J1/J2)
   and other  to the COM port of the Windows Xp PC.
 2. Open the HyperTerminal on the Host PC. To open the HyperTerminal click
    on 'Start->Programs->Accessories->Communications->HyperTerminal’
 3. Disconnect the HyperTerminal by selecting menu Call->Disconnect
 4. Select Menu 'File->Properties' and click on the Button 'Configure'
      Set 'Bits Per Second(Baud Rate)' to 2400.
      Set 'Data bits' to 8.
      Set 'Parity' to None.
      Set 'Stop bits' to 1.
      Set 'Flow control' to None.
      Click on 'OK' button.
      HyperTerminal is configured and ready for communication
 5. Connect the HyperTerminal by selecting menu Call->Call.
 6. Put a breakpoint on line 239 and run the CSL_UART_DiffBaudpollExample.
 7. Upon running the test, a message prompting to enter the string size will be 
 	displayed on HyperTerminal.
 8.	Enter the string size which is greater than 1 and less than 99. 
 9.	String length should be two characters and the numbers below 10 should be preceded 
 	by 0(01,02,03...). 
 10.After reading the string size a message prompting to enter the string will be
 	displayed on HyperTerminal.
 11.Enter the string which is exactly of the length specified in the previous query. 
 12.Once the string of the specified size is entered, test code will stop reading from the
 	HyperTerminal and send message to inform that the reading from HyperTerminal has been stopped.
 13.After reading the string test code will send the same string back to the HyperTerminal.
 14.Repeat the test for baudrates 4800,9600,19200,38400.
 
 Jumpers on-board : 
  A jumper on JP4 on C5517 EVM, and JP3 in the case of C5515 EVM. (UART_EN)
 