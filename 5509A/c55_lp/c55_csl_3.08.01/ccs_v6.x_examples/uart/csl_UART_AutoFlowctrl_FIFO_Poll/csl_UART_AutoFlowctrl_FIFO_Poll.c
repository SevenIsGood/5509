/* ============================================================================
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


/** @file csl_UART_AutoFlowctrl_FIFO_Poll.c
 *
 *  @brief This test is to verify the CSL UART module operation in Poll mode
 *         with FIFOs and Auto Flow Control enabled.
 *
 *
 * \page    page18  CSL UART EXAMPLE DOCUMENTATION
 *
 * \section UART4   UART EXAMPLE4 - Auto flow control mode test
 *
 * \subsection UART4x    TEST DESCRIPTION:
 * UART peripheral is configured with the following parameters
 * Baud Rate - 2400.
 * Data bits - 8.
 * FIFO Mode - Trigger level 04 and DMA disabled
 * Parity - No parity.
 * Stop bits - 1.
 * Internal Loopback - disable
 * Flow control - Enable.
 *
 * Will Send the FIFO trigger level no of bytes using the CPU poll mode and receive using
 * CPU poll mode and compares the data. In this test case we have configured the FIFO and
 * set the trigger to one byte.
 *
 *
 * Jumpers on-board :
 * Manual test with Pin2/3, Pin7/8 on DB-9 connected
 *  A jumper on UART connector from TX(pin 3) to RX(pin 2).
 *  A jumper on JP4 on C5517 EVM, and JP3 in the case of C5515 EVM. (UART_EN)
 *
 * Ensure the bytes transmitted are received and there is no mismatch.
 *
*/
/* ============================================================================
 * Revision History
 * ================
 * 15-Apr-2015 Created
 * 10-Mar-2016 Updates to header
 * ============================================================================
 */


#include <stdio.h>
#include "csl_uart.h"
#include "csl_uartAux.h"
#include "csl_intc.h"
#include "csl_general.h"

#define CSL_TEST_FAILED         (1U)
#define CSL_TEST_PASSED         (0)
#define	CSL_UART_BUF_LEN    	(4U)

#define CSL_PLL_DIV_000    (0)
#define CSL_PLL_DIV_001    (1u)
#define CSL_PLL_DIV_002    (2u)
#define CSL_PLL_DIV_003    (3u)
#define CSL_PLL_DIV_004    (4u)
#define CSL_PLL_DIV_005    (5u)
#define CSL_PLL_DIV_006    (6u)
#define CSL_PLL_DIV_007    (7u)

#define CSL_PLL_CLOCKIN    (32768u)

#define EVM5515  1
#define PLL_CNTL1        *(ioport volatile unsigned *)0x1C20
#define PLL_CNTL2        *(ioport volatile unsigned *)0x1C21
#define PLL_CNTL3        *(ioport volatile unsigned *)0x1C22
#define PLL_CNTL4        *(ioport volatile unsigned *)0x1C23

/* Global data definition */
/* UART setup structure */
CSL_UartSetup mySetup =
{
	/* Input clock freq in MHz */
    60000000,
	/* Baud rate */
    2400,
	/* Word length of 8 */
    CSL_UART_WORD8,
	/* To generate 1 stop bit */
    0,
	/* No parity */
    CSL_UART_DISABLE_PARITY,
	/* Enable trigger 01 fifo */
	CSL_UART_FIFO_DMA1_DISABLE_TRIG01,
	/* internal Loop Back disable */
	CSL_UART_NO_LOOPBACK,
	/* Auto flow control*/
	CSL_UART_AFE ,
	/* RTS */
	CSL_UART_RTS
};

/* Global constants */
#define NO_OF_CHAR_TO_READ     (01u)
#define NO_OF_CHAR_TO_WRITE    (01u)

/* CSL UART data structures */
CSL_UartObj uartObj;
CSL_UartHandle hUart;

/* CSL UART data buffers */
char guartWriteBuf[NO_OF_CHAR_TO_WRITE];
char guartReadBuf[NO_OF_CHAR_TO_READ];
volatile unsigned char rx_fifo_full=0,tx_fifo_empty=0;


/**
 *  \brief  Function to calculate the system clock
 *
 *  \param    none
 *
 *  \return   System clock value in Hz
 */
Uint32 getSysClk(void);


/**
 *  \brief  UART Poll Test function
 *
 *  \param  none
 *
 *  \return Test result(Only Failure Case)
 */
CSL_Status uart_PollSample(void)
{
	CSL_Status         status;
	Uint32            sysClk;
	int i=0;
	unsigned char fail_flag=0;

	#ifdef PHOENIX_QT
	sysClk = PHOENIX_QTCLK;
	#elif EVM5515
	sysClk = getSysClk();
	#else
	 #warn "Define either EVM5515 or PHOENIX_QT"
	#endif

	mySetup.clkInput = sysClk;

    /* Loop counter and error flag */
    status = UART_init(&uartObj,CSL_UART_INST_0,UART_INTERRUPT);
    if(CSL_SOK != status)
    {
        printf("UART_init failed error code %d\n",status);
        return(status);
    }
	else
	{
		printf("UART_init Successful\n");
	}

    /* Handle created */
    hUart = (CSL_UartHandle)(&uartObj);

    /* Configure UART registers using setup structure */
    status = UART_setup(hUart,&mySetup);
    if(CSL_SOK != status)
    {
        printf("UART_setup failed error code %d\n",status);
        return(status);
    }
	else
	{
		printf("UART_setup Successful\n");
	}


	/* Fill the Transmit buffer */
	for( i = 0 ; i < NO_OF_CHAR_TO_WRITE ; i++ )
	{
 		//guartWriteBuf[i] = i+0x30;
 		guartWriteBuf[i] = i; //For data length 5 the max data is 0x1F. But adding 0x30. Reason for failure
	}
	/* Check for the TX FIFO empty status */

	for( i = 0 ; i < NO_OF_CHAR_TO_WRITE ; i++ )
	{
 		hUart->uartRegs->THR = guartWriteBuf[i];
	}
    while(!UART_getXmitHoldRegEmptyStatus(hUart));


	for( i = 0 ; i < NO_OF_CHAR_TO_READ ; i++ )
	{
		while(!UART_getDataReadyStatus(hUart));
 		guartReadBuf[i] = hUart->uartRegs->RBR;
   	    //for(delay=0;delay<600;delay++);  //Free run mode with out delay failing
	}
	/*Compare the tx and rx data*/
 	for(i=0 ; i < NO_OF_CHAR_TO_READ; i++ )
	{
		printf("\n%c",guartReadBuf[i]);
		printf("%c",guartWriteBuf[i]);
		if( guartReadBuf[i] !=  guartWriteBuf[i] )
		{
   		 printf("\r\nData Mismatch\r\n");
		 fail_flag+=1;
		}
    }



	if(!fail_flag)
	{
	 return(CSL_TEST_PASSED);
	}
	return(CSL_TEST_FAILED);
}

/**
 *  \brief  Configures Dma
 *
 *  \param  chanNum - Dma channel number
 *
 *  \return Dma handle
 */
/////INSTRUMENTATION FOR BATCH TESTING -- Part 1 --
/////  Define PaSs_StAtE variable for catching errors as program executes.
/////  Define PaSs flag for holding final pass/fail result at program completion.
    volatile Int16 PaSs_StAtE = 0x0001; // Init to 1. Reset to 0 at any monitored execution error.
    volatile Int16 PaSs = 0x0000; // Init to 0.  Updated later with PaSs_StAtE when and if
/////                                  program flow reaches expected exit point(s).
/////

void main()
{
	CSL_Status   status;

	printf("CSL UART AUTOFLOW EXTERNAL LOOPBACK TEST in FIFO mode!\n\n");

	status = uart_PollSample();
	if(status != CSL_TEST_PASSED)
	{
		printf("\r\nCSL UART AUTOFLOW EXTERNAL LOOPBACK TEST in FIFO mode failed!\n\n");
		PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
	}
	else
	{
		printf("\r\nCSL UART AUTOFLOW EXTERNAL LOOPBACK TEST in FIFO mode passed!\n\n");
	}

	/////INSTRUMENTATION FOR BATCH TESTING -- Part 3 --
	/////  At program exit, copy "PaSs_StAtE" into "PaSs".
	PaSs = PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with
	/////                   // pass/fail value determined during program execution.
	/////  Note:  Program should next exit to C$$EXIT and halt, where DSS, under
	/////   control of a host PC script, will read and record the PaSs' value.
	/////
}

/**
 *  \brief  Function to calculate the clock at which system is running
 *
 *  \param    none
 *
 *  \return   System clock value in Hz
 */
#if ((defined(CHIP_C5505_C5515)) || (defined(CHIP_C5504_C5514)) || (defined(CHIP_C5535) || defined(CHIP_C5545)))

Uint32 getSysClk(void)
{
	Bool      pllRDBypass;
	Bool      pllOutDiv;
	Uint32    sysClk;
	Uint16    pllM;
	Uint16    pllRD;
	Uint16    pllOD;

	pllM = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR1, SYS_CGCR1_M);

	pllRD = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR2, SYS_CGCR2_RDRATIO);
	pllOD = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR4, SYS_CGCR4_ODRATIO);

	pllRDBypass = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR2, SYS_CGCR2_RDBYPASS);
	pllOutDiv   = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR4, SYS_CGCR4_OUTDIVEN);

	sysClk = CSL_PLL_CLOCKIN;

	if (0 == pllRDBypass)
	{
		sysClk = sysClk/(pllRD + 4);
	}

	sysClk = (sysClk * (pllM + 4));

	if (1 == pllOutDiv)
	{
		sysClk = sysClk/(pllOD + 1);
	}

	/* Return the value of system clock in KHz */
	return(sysClk/1000);
}

#elif (defined(CHIP_C5517))
Uint32 getSysClk(void)
{
	Uint32    sysClk;
	float    Multiplier;
	Uint16    OD;
	Uint16    OD2;
	Uint16    RD, RefClk;
	Uint32	  temp1, temp2, temp3, vco;
	//Uint16 DIV;

	temp2 =  PLL_CNTL2;
	temp3 =  (temp2 & 0x8000) <<1 ;
	temp1 = temp3 + PLL_CNTL1;
	Multiplier = temp1/256 +1;
	RD = (PLL_CNTL2 & 0x003F) ;

	RefClk = 12000/(RD+1);

	vco = Multiplier * (Uint32)RefClk;

	OD = (PLL_CNTL4 & 0x7);

	sysClk = vco/(OD+1);

	OD2 = ((PLL_CNTL4 >> 10) & 0x1F) ;

	if (PLL_CNTL3 & 0x8000)	// PLL Bypass
		sysClk = RefClk;
	else
		sysClk = vco/(OD+1);

	if ((PLL_CNTL4 & 0x0020) == 0)	/* OutDiv2 */
		sysClk = sysClk / ( 2*(OD2+1));

	/* Return the value of system clock in KHz */
	return(sysClk*1000);
}

#else

Uint32 getSysClk(void)
{
	Bool      pllRDBypass;
	Bool      pllOutDiv;
	Bool      pllOutDiv2;
	Uint32    sysClk;
	Uint16    pllVP;
	Uint16    pllVS;
	Uint16    pllRD;
	Uint16    pllVO;
	Uint16    pllDivider;
	Uint32    pllMultiplier;

	pllVP = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR1, SYS_CGCR1_MH);
	pllVS = CSL_FEXT(CSL_SYSCTRL_REGS->CGICR, SYS_CGICR_ML);

	pllRD = CSL_FEXT(CSL_SYSCTRL_REGS->CGICR, SYS_CGICR_RDRATIO);
	pllVO = CSL_FEXT(CSL_SYSCTRL_REGS->CGOCR, SYS_CGOCR_ODRATIO);

	pllRDBypass = CSL_FEXT(CSL_SYSCTRL_REGS->CGICR, SYS_CGICR_RDBYPASS);
	pllOutDiv   = CSL_FEXT(CSL_SYSCTRL_REGS->CGOCR, SYS_CGOCR_OUTDIVEN);
	pllOutDiv2  = CSL_FEXT(CSL_SYSCTRL_REGS->CGOCR, SYS_CGOCR_OUTDIV2BYPASS);

	pllDivider = ((pllOutDiv2) | (pllOutDiv << 1) | (pllRDBypass << 2));

	pllMultiplier = ((Uint32)CSL_PLL_CLOCKIN * ((pllVP << 2) + pllVS + 4));

	switch(pllDivider)
	{
		case CSL_PLL_DIV_000:
		case CSL_PLL_DIV_001:
			sysClk = pllMultiplier / (pllRD + 4);
		break;

		case CSL_PLL_DIV_002:
			sysClk = pllMultiplier / ((pllRD + 4) * (pllVO + 4) * 2);
		break;

		case CSL_PLL_DIV_003:
			sysClk = pllMultiplier / ((pllRD + 4) * 2);
		break;

		case CSL_PLL_DIV_004:
		case CSL_PLL_DIV_005:
			sysClk = pllMultiplier;
		break;

		case CSL_PLL_DIV_006:
			sysClk = pllMultiplier / ((pllVO + 4) * 2);
		break;

		case CSL_PLL_DIV_007:
			sysClk = pllMultiplier / 2;
		break;
	}

	/* Return the value of system clock in KHz */
	return(sysClk);
}

#endif
