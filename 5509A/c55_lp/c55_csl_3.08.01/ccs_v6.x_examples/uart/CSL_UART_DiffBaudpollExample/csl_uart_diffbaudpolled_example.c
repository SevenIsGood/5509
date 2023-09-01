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


/** @file csl_uart_diffbaudpolled_example.c
 *
 *  @brief UART example code to test the different baud rates in poll mode
 *
 *
 * \page    page18  CSL UART EXAMPLE DOCUMENTATION
 *
 * \section UART5   UART EXAMPLE5 - CSL_UART_DiffBaudpollExample
 *
 * \subsection UART5x    TEST DESCRIPTION:
 *
 * 1. Connect the RS232 cable; One end to the UART port on the C5535/C5515/C5517 EVM(J13/J1/J2)
 *      and other  to the COM port of the Windows Xp PC.
 * 2. Open the HyperTerminal on the Host PC. To open the HyperTerminal click
 *    on 'Start->Programs->Accessories->Communications->HyperTerminal<92>
 * 3. Disconnect the HyperTerminal by selecting menu Call->Disconnect
 * 4. Select Menu 'File->Properties' and click on the Button 'Configure'
 *      Set 'Bits Per Second(Baud Rate)' to 2400.
 *      Set 'Data bits' to 8.
 *     Set 'Parity' to None.
 *      Set 'Stop bits' to 1.
 *      Set 'Flow control' to None.
 *      Click on 'OK' button.
 *      HyperTerminal is configured and ready for communication
 * 5. Connect the HyperTerminal by selecting menu Call->Call.
 * 6. Put a breakpoint on line 239 and run the CSL_UART_DiffBaudpollExample.
 * 7. Upon running the test, a message prompting to enter the string size will be
 *        displayed on HyperTerminal.
 * 8.     Enter the string size which is greater than 1 and less than 99.
 * 9.     String length should be two characters and the numbers below 10 should be preceded
 *        by 0(01,02,03...).
 * 10.After reading the string size a message prompting to enter the string will be
 *        displayed on HyperTerminal.
 * 11.Enter the string which is exactly of the length specified in the previous query.
 * 12.Once the string of the specified size is entered, test code will stop reading from the
 *        HyperTerminal and send message to inform that the reading from HyperTerminal has been stopped.
 * 13.After reading the string test code will send the same string back to the HyperTerminal.
 * 14.Repeat the test for baudrates 4800,9600,19200,38400.
 *
 * Jumpers on-board :
 *  A jumper on JP4 on C5517 EVM, and JP3 in the case of C5515 EVM. (UART_EN)
 *
 */

/* ============================================================================
 * Revision History
 * ================
 * 16-Sep-2008 Created
 * 23-Jul-2012 Added C5517 Compatibility
 * 09-Mar-2016 Update header
 * ============================================================================
 */

#include <stdio.h>
#include "csl_uart.h"
#include "csl_uartAux.h"
#include "csl_general.h"
#include "csl_sysctrl.h"

/* Global constants */
/* String length to be received and transmitted */
#define WR_STR_LEN    (80u)
#define RD_STR_LEN    (10u)

#define CSL_PLL_DIV_000    (0)
#define CSL_PLL_DIV_001    (1u)
#define CSL_PLL_DIV_002    (2u)
#define CSL_PLL_DIV_003    (3u)
#define CSL_PLL_DIV_004    (4u)
#define CSL_PLL_DIV_005    (5u)
#define CSL_PLL_DIV_006    (6u)
#define CSL_PLL_DIV_007    (7u)

#define CSL_PLL_CLOCKIN    (32768u)

#define PLL_CNTL1        *(ioport volatile unsigned *)0x1C20
#define PLL_CNTL2        *(ioport volatile unsigned *)0x1C21
#define PLL_CNTL3        *(ioport volatile unsigned *)0x1C22
#define PLL_CNTL4        *(ioport volatile unsigned *)0x1C23

/* Global data definition */
/* UART setup structure */
CSL_UartSetup uartSetup =
{
	/* Input clock freq in MHz */
    60000000,
	/* Baud rate */
    2400,
	/* Word length of 8 */
    CSL_UART_WORD8,
	/* To generate 1 stop bit */
    0,
	/* Disable the parity */
    CSL_UART_DISABLE_PARITY,
	/* Disable fifo */
	/* Enable trigger 14 fifo */
	CSL_UART_FIFO_DMA1_DISABLE_TRIG14,
	/* Loop Back enable */
    CSL_UART_NO_LOOPBACK,
	/* No auto flow control*/
	CSL_UART_NO_AFE ,
	/* No RTS */
	CSL_UART_NO_RTS ,
};

CSL_UartObj uartObj;

/* UART data buffers */
char rdbuffer[100];
char wrbuffer[55] = "\r\nEnter the size of the string(min 01 to max 99)\r\n >> ";
char buffer1[30] = "\r\n\nEnter the string:\r\n >> ";
char buffer2[60] = "\r\n\nPlease stop typing, reading already stopped...!!!\r\n";

/**
 *  \brief  Tests CSL UART module in polled mode.
 *
 *  This functions sends and receives data from the HyperTerminal
 *  on the host PC.
 *
 *  \param  Uint16 baudrate
 *
 *  \return Test result
 */
CSL_Status CSL_uartPolledTest(Uint16 baudrate);

/**
 *  \brief  Function to calculate the system clock
 *
 *  \param    none
 *
 *  \return   System clock value in Hz
 */
Uint32 getSysClk(void);

/**
 *  \brief  main function
 *
 *  This function calls UART test function and displays the test result
 *
 *  \param  none
 *
 *  \return none
 */
   /////INSTRUMENTATION FOR BATCH TESTING -- Part 1 --
   /////  Define PaSs_StAtE variable for catching errors as program executes.
   /////  Define PaSs flag for holding final pass/fail result at program completion.
        volatile Int16 PaSs_StAtE = 0x0001; // Init to 1. Reset to 0 at any monitored execution error.
        volatile Int16 PaSs = 0x0000; // Init to 0.  Updated later with PaSs_StAtE when and if
   /////                                  program flow reaches expected exit point(s).
   /////
int main(void)
{
	CSL_Status    status;
	Uint8 looper=0;
	Uint16 baudrate[5]={2400u,4800u,9600u,19200u,38400u};

	printf("CSL UART POLLED MODE TEST!\n\n");
	printf("Please Make Sure That HyperTerminal on the Host PC is connected\n\n");

	for(looper=0;looper<5;looper++)
	{
		printf("\n\nBaudrate: 0x%x\n\n",baudrate[looper]);
		printf("Please disconnect the Hyperterminal, ensure it's also "
				"configured for the same baudrate, and connect again\n\n");
		status = CSL_uartPolledTest(baudrate[looper]);
		if(status != CSL_SOK)
		{
			printf("\n\nCSL UART POLLED MODE TEST FAILED!!\n");
			/////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
			/////  Reseting PaSs_StAtE to 0 if error detected here.
			PaSs_StAtE = 0x0000; // Was intialized to 1 at declaration.
			/////
		}
		else
		{
			printf("\n\nCSL UART POLLED MODE TEST COMPLETED!!\n");
		}
		/////INSTRUMENTATION FOR BATCH TESTING -- Part 3 --
		/////  At program exit, copy "PaSs_StAtE" into "PaSs".
		PaSs = PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with
		/////                   // pass/fail value determined during program execution.
		/////  Note:  Program should next exit to C$$EXIT and halt, where DSS, under
		/////   control of a host PC script, will read and record the PaSs' value.
		/////
	}
}

/**
 *  \brief  Tests CSL UART module in polled mode.
 *
 *  This functions sends and receives data from the HyperTerminal
 *  on the host PC.
 *
 *  \param  none
 *
 *  \return Test result
 */
CSL_Status CSL_uartPolledTest(Uint16 baudrate)
{
    CSL_UartHandle    hUart;
    CSL_Status        status;
	Uint16            stringSize;
	Uint32            sysClk;

	sysClk = getSysClk();

	uartSetup.clkInput = sysClk;
	uartSetup.baud = baudrate;

    status = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
                         CSL_EBSR_PPMODE_1);
    if(CSL_SOK != status)
    {
        printf("SYS_setEBSR failed\n");
        return(status);
    }

    /* Loop counter and error flag */
    status = UART_init(&uartObj,CSL_UART_INST_0,UART_POLLED);
    if(CSL_SOK != status)
    {
        printf("UART_init failed error code %d\n",status);
        return(status);
    }
    else
    {
		printf("UART_init Successful\n");
	}

    status = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
                         CSL_EBSR_PPMODE_1);
    if(CSL_SOK != status)
    {
        printf("SYS_setEBSR failed\n");
        return (status);
    }

    /* Handle created */
    hUart = (CSL_UartHandle)(&uartObj);

    //UART_reset(hUart);
    /* Configure UART registers using setup structure */
    status = UART_setup(hUart,&uartSetup);
    if(CSL_SOK != status)
    {
        printf("UART_setup failed error code %d\n",status);
        return(status);
    }
    else
    {
		printf("UART_setup Successful\n");
	}
    //UART_resetOff(hUart);

	/* Send the message to HyperTerminal to Query the string size */
   	status = UART_fputs(hUart,wrbuffer,0);
    if(CSL_SOK != status)
    {
        printf("UART_fputs failed error code %d\n",status);
        return(status);
    }
    else
    {
		printf("\n\nMessage Sent to HyperTerminal : %s\n",wrbuffer);
	}

	/* Read the string size entered on HyperTerminal */
   	status = UART_read(hUart,rdbuffer,2,0);
    if(CSL_SOK != status)
    {
        printf("UART_read failed error code %d\n",status);
        return(status);
    }
    else
    {
		printf("\nString Size Read from HyperTerminal \n");
	}

	/* Convert the string size read from HyperTerminal to integer value */
	stringSize = ((rdbuffer[0]- 0x30)*10) + (rdbuffer[1]-0x30);

	/* Verify the string size. This test code support string size 1 to 99 */
	if((stringSize < 1) || (stringSize > 99))
	{
		printf("Wrong String size - %d!\n",stringSize);
		return(CSL_ESYS_FAIL);
	}

	rdbuffer[0] = '\0';
	rdbuffer[1] = '\0';

	printf("\nSize of the  string entered: %d\n",stringSize);

	status = UART_fputs(hUart, buffer1,0);
    if(CSL_SOK != status)
    {
        printf("UART_fputs failed error code %d\n",status);
        return(status);
    }
    else
    {
		printf("\n\nMessage Sent to HyperTerminal : %s\n",buffer1);
	}

	status = UART_read(hUart,rdbuffer,stringSize,0);
    if(CSL_SOK != status)
    {
        printf("UART_read failed error code %d\n",status);
        return(status);
    }
    else
    {
		printf("\nString Read from HyperTerminal \n");
	}

	status = UART_fputs(hUart,buffer2,0);
    if(CSL_SOK != status)
    {
        printf("UART_fputs failed error code %d\n",status);
        return(status);
    }
    else
    {
		printf("\n\nMessage Sent to HyperTerminal : %s\n",buffer2);
	}

	status = UART_fputs(hUart,"\r\nYOU HAVE ENTERED: ",0);
    if(CSL_SOK != status)
    {
        printf("UART_fputs failed error code %d\n",status);
        return(status);
    }

	status = UART_fputs(hUart,rdbuffer,0);
    if(CSL_SOK != status)
    {
        printf("UART_fputs failed error code %d\n",status);
        return(status);
    }
    else
    {
		printf("\nMessage Sent to HyperTerminal : \n%s%s\n","\r\nYOU HAVE ENTERED: ",rdbuffer);
	}

	status = UART_fputs(hUart,"\r\n",0);
    if(CSL_SOK != status)
    {
        printf("UART_fputs failed error code %d\n",status);
        return(status);
    }

	rdbuffer[stringSize] = '\0';

	return(CSL_SOK);
}

/**
 *  \brief  Function to calculate the clock at which system is running
 *
 *  \param    none
 *
 *  \return   System clock value in Hz
 */
#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined(CHIP_C5535) || defined(CHIP_C5545))

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

	/* Return the value of system clock in Hz */
	return(sysClk);
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
	pllVS = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR2, SYS_CGCR2_ML);

	pllRD = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR2, SYS_CGCR2_RDRATIO);
	pllVO = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR4, SYS_CGCR4_ODRATIO);

	pllRDBypass = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR2, SYS_CGCR2_RDBYPASS);
	pllOutDiv   = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR4, SYS_CGCR4_OUTDIVEN);
	pllOutDiv2  = CSL_FEXT(CSL_SYSCTRL_REGS->CGCR4, SYS_CGCR4_OUTDIV2BYPASS);

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

