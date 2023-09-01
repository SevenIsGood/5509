/*
 * Copyright (C) 2003 Texas Instruments Incorporated
 * All Rights Reserved
 */
/*
 *---------main_uart1.c---------
 * This example demostrates simple data transfer between 5502 UART and
 * host machine (assumed to be PC) serial port. Follow the steps below to run
 * the example.
 * 1. Connect the PC COM port and UART port of 5502EVM using serial 
 *    communication cable.
 * 2. Open a terminal emulator program that support serial port connections.
 * 3. Setup this program for the baud rate given in the 'mysetup' structure
 *    below.
 * 4. Build and run the example. It will wait for input characters from COM
 *    port.
 * 5. Copy the complete string 'clipboardBuf' (having STR_LEN number of 
 *    characters) and send it through the terminal emulator program.
 * 6. After UART reads STR_LEN number of characters, it will transmit them,
 *    which will then be seen in the terminal emulator window.
 * Thus the string sent from PC COM port is transmitted back by UART.
 *
 * NOTE: 1. Copy all the characters (e.g STR_LEN = 83) here in 'clipboardBuf' 
 *       and send it through the terminal emulator program. If number of 
 *       characters sent are less than STR_LEN, UART receiver will continue
 *       to wait for more characters and no output will be seen on terminal
 *       emulator window.
 *
 *       2. If the baud rate in 'mySetup' structure is changed, the terminal
 *       emulator program must be setup for that baud rate i.e. 
 *       steps 3 to 6 must be followed again.
 *
 */
#include <stdio.h>
#include <csl.h>
#include <csl_uart.h>

//---------Global constants---------
/* String length to be received and transmitted */
#define STR_LEN        80

//---------Global data definition---------
/* Buffer to be copied to clip board and sent to UART 
   using terminal emulator program 
*/
char clipboardBuf[] = {
" <BEGIN>The quick brown fox jumped over lazy dog. This is simple UART test<END>"
};

/* User buffer that receives data */
char myBuf[STR_LEN];

/* UART setup structure */
UART_Setup mySetup = {
   75,  /* input clock freq */
   UART_BAUD_4800, /* baud rate */
   UART_WORD8, /* word length */
   UART_STOP1, /* stop bits */
   UART_DISABLE_PARITY, /* parity */
   UART_FIFO_DISABLE, /*DISABLE */
   UART_NO_LOOPBACK,   /* Loop Back enable */
};

//---------main routine---------
void main()
{
    /* Loop counter and error flag */
    Int16 i, error = 0;
    
    /* Initialize CSL library */
    CSL_init();
    
    /* Configure UART registers using setup structure */
    UART_setup(&mySetup);

    /* UART receiver reads data from PC COM port */
   	if (UART_read(myBuf, STR_LEN, 0) == FALSE) {
   	    error = 1;
   	}
   	else {
   	    
   	    /* UART transmitter write data to PC COM port */
   	    if ((UART_write(myBuf, STR_LEN, 0)) == FALSE)
   	        error = 1;
   	}
   	
   	/* Verify the recevied data for correctness */
   	for (i = 0; i < STR_LEN-1; i++) {
   	    if (myBuf[i] != clipboardBuf[i]) {
   	        error = 1;
   	        break;
   	    }
   	}
   	
   	/* Display PASS/FAIL status based on error flag */
   	if (error)
   	    printf("\nTEST FAILED\n");
   	else
   	    printf("\nTEST PASSED\n");
}
