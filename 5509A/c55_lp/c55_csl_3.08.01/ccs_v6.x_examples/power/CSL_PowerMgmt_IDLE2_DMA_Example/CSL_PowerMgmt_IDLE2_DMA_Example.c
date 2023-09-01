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


/** @file CSL_PowerMgmt_IDLE2_DMA_Example.c
 *
 *  @brief Example to exhibit I2S DMA TXN IN IDLE 2 state and recovery using RTC alarm interrupt
 *
 * \page    page23 POWER EXAMPLE DOCUMENTATION
 *
 * \section POWER6   POWER EXAMPLE6 -  I2S DMA TXN IN IDLE2
 *
 * \subsection POWER6x      TEST DESCRIPTION:
 *         This test configures RTC for generating an alarm 8s after it's
 * started, during which period the system is put into IDLE2 state.
 * During the same time, an I2S0(Tx)-I2S2(Rx) DMA transaction is also
 * initiated. The expected RTC alarm interrupt should bring the CPU out
 * of the IDLE2 state and resume execution from where it was before it
 * idled out. Also, the I2S DMA transaction should have gone through
 * and the rx'ed and tx'ed data shouldn't mismatch.
 *
 * \subsection POWER6y    TEST PROCEDURE AND EXPECTED RESULT:
 *  Manual test with I2S0 connect to I2S2, measuring power on JP1
 *  1. Ensure JP10 CLKSEL is shorted so that usb osc is not used.
 *  2. Run the test. The RTC alarm (expected after approx. 30s) would be set.
 *  3. Test will stop once usb clk is shut off.
 *  4. Pause and Resume the test.
 *  5. The test will also configure I2S0 as Tx and I2S2 as Rx and initiate a txfer using DMA.
 *  6. While CPU waits for DMA done, the test will execute the idle instsrn that will idle
 *     the cpu and wait for the alarm.
 *  7. Observe if the alarm interrupt got the CPU out of idle state and the I2S transaction goes through.
 *  8. A comparison of the I2S Read and Write buffers should not throw a mismatch.
 *
*/
/* ============================================================================
 * Revision History
 * ================
 * 15-Apr-2015 Created
 * 10-Mar-2016 Updates to header
 * ============================================================================
 */



#include "csl_dma.h"
#include "csl_i2s.h"
#include <stdio.h>
#include <csl_rtc.h>
#include <csl_intc.h>
#include <csl_general.h>

#define EVM5515 0x1

#define RTC_TIME_PRINT_CYCLE    (0xFFu)
#define RTC_CALL_BACK           (1u)

CSL_RtcTime	     InitTime;
CSL_RtcDate 	 InitDate;
CSL_RtcTime 	 GetTime;
CSL_RtcDate 	 GetDate;
CSL_RtcConfig    rtcConfig;
CSL_RtcConfig    rtcGetConfig;
CSL_RtcAlarm     AlarmTime;
CSL_RtcIsrAddr   isrAddr;
CSL_RtcIsrDispatchTable      rtcDispatchTable;
volatile Uint32 rtcTimeCount = RTC_TIME_PRINT_CYCLE;
volatile Uint16    alarm_event_intr = 0;

/* Reference the start of the interrupt vector table */
extern void VECSTART(void);
/* Prototype declaration for ISR function */
interrupt void rtc_isr(void);

void rtc_msIntc(void);
void rtc_secIntc(void);
void rtc_minIntc(void);
void rtc_hourIntc(void);
void rtc_dayIntc(void);
void rtc_extEvt(void);
void rtc_alarmEvt(void);


CSL_Status rtc_test(void);

CSL_Status ioExpander_Setup(void);
CSL_Status ioExpander_Read(Uint16 port, Uint16 pin, Uint16 *rValue);
CSL_Status ioExpander_Write(Uint16 port, Uint16 pin, Uint16 value);

#define I2S_DMA_PORTION
#ifdef I2S_DMA_PORTION

#define CSL_TEST_FAILED         (1)
#define CSL_TEST_PASSED         (0)
#define CSL_DMA0_CH0            (0)

#define CSL_I2S_DMA_BUF_LEN		(16)
//Valid Values for this:1,2,4,8,16
//And when changing this, change burstLen and
//datalen fields as well. Note: datalen =
//4*(2^burstlen field), i.e. 4,8,16,32,64 resp.

CSL_DMA_ChannelObj dmaObj;

CSL_DMA_Handle dmaLeftTxHandle;
CSL_DMA_Handle dmaRightTxHandle;
CSL_DMA_Handle dmaLeftRxHandle;
CSL_DMA_Handle dmaRightRxHandle;
CSL_DMA_Config dmaConfig;
//#pragma DATA_ALIGN(i2sDmaWriteLeftBuff, 32);
#pragma DATA_ALIGN(i2sDmaWriteLeftBuff, 8);
//Uint32 i2sDmaWriteLeftBuff[CSL_I2S_DMA_BUF_LEN] = {0xDEADBEEF};
Uint32 i2sDmaWriteLeftBuff[CSL_I2S_DMA_BUF_LEN];
//Uint32 i2sDmaWriteLeftBuff;
//#pragma DATA_ALIGN(i2sDmaWriteRightBuff, 32);
#pragma DATA_ALIGN(i2sDmaWriteRightBuff, 8);
//Uint32 i2sDmaWriteRightBuff[CSL_I2S_DMA_BUF_LEN] = {0x12345678};
Uint32 i2sDmaWriteRightBuff[CSL_I2S_DMA_BUF_LEN];
//Uint32 i2sDmaWriteRightBuff;
//#pragma DATA_ALIGN(i2sDmaReadLeftBuff, 32);
#pragma DATA_ALIGN(i2sDmaReadLeftBuff, 8);
//Uint32 i2sDmaReadLeftBuff[CSL_I2S_DMA_BUF_LEN] = {0x00000000};
Uint32 i2sDmaReadLeftBuff[CSL_I2S_DMA_BUF_LEN];
//Uint32 i2sDmaReadLeftBuff;
//#pragma DATA_ALIGN(i2sDmaReadRightBuff, 32);
#pragma DATA_ALIGN(i2sDmaReadRightBuff, 8);
//Uint32 i2sDmaReadRightBuff[CSL_I2S_DMA_BUF_LEN] = {0x00000000};
Uint32 i2sDmaReadRightBuff[CSL_I2S_DMA_BUF_LEN];
//Uint32 i2sDmaReadRightBuff;

/*
	This is functionality is to configure DMA
	for the source and destination address.

	Function returns:
	CSL_DMA_Handle              - Valid handler
	NULL                        - Invalid handler
*/

CSL_DMA_Handle CSL_configDmaForI2s(CSL_DMAChanNum    chanNum)
{
	CSL_DMA_Handle    dmaHandle;
	CSL_Status        status;
	ioport volatile CSL_SysRegs		  *sysRegs;

	sysRegs = (CSL_SysRegs *)CSL_SYSCTRL_REGS;

	/*enable the corresponding DMA clock from PCGCR Registers*/
    CSL_FINS(sysRegs->PCGCR1, SYS_PCGCR1_DMA0CG, CSL_SYS_PCGCR1_DMA0CG_ACTIVE);
	CSL_FINS(sysRegs->PCGCR2, SYS_PCGCR2_DMA1CG, CSL_SYS_PCGCR2_DMA1CG_ACTIVE);

    status = DMA_init();
    if (status != CSL_SOK)
    {
        printf("DMA_init() Failed \n");
        dmaHandle = NULL;
    }
    CSL_SYSCTRL_REGS->PCGCR2 |= 0x30;
	dmaHandle = DMA_open(chanNum, &dmaObj,&status);
    if (dmaHandle == NULL)
    {
        printf("DMA_open() Failed \n");
        dmaHandle = NULL;
    }

	status = DMA_config(dmaHandle, &dmaConfig);
    if (status != CSL_SOK)
    {
        printf("DMA_config() Failed \n");
        dmaHandle = NULL;
    }

    return(dmaHandle);
}

/*
	This is functionality Transmit  and
	receive data with DMA mode.
	The data transmission and receive happen in stereo mode.

	Function returns:
	CSL_TEST_FAILED                -Failure
	CSL_TEST_PASSED                -Success
*/
Int16 i2s_DMA_sample(void)
{
	Int16 			status = CSL_TEST_FAILED;
	Int16 			result;
	CSL_I2sHandle	hI2s;
	I2S_Config		hwConfig;

	Uint8 			looper;

#if (defined(CHIP_C5517))
	ioExpander_Setup();
    /* Test Requires SW4 HPI_ON to be OFF */
    /* J14 pins: I2S0@ 12,8,22,21; I2S2@ 5,9,19,14 (Clk,FS,DX,RX)*/

    /* Set SEL_MMC0_I2S high for routing I2S0 to J14*/
    ioExpander_Write(1, 0 , 1);// SEL_MMC0_I2S = 1

    /* SPI_I2S2_S1=0, SPI_I2S2_S0=1 for routing I2S2 to J14 */
    ioExpander_Write(0, 5 , 1);
    ioExpander_Write(0, 6 , 0);

#endif
	/* Initialize data buffers */
	for(looper=0; looper < CSL_I2S_DMA_BUF_LEN; looper++)
	{
		i2sDmaWriteLeftBuff[looper] = 0x12345678;
		i2sDmaWriteRightBuff[looper] = 0x56781234;
		i2sDmaReadRightBuff[looper] = 0x0;
		i2sDmaReadLeftBuff[looper]	= 0;
	}

	/* On C5505/C5515 DSP DMA swaps the words in the source buffer
	   before transferring it to the I2S registers. No data mismatch is
	   observed When the  write and read operations are done in DMA mode
	   as the word swap occurs in both the operations.
	   There will be data mismatch if data write is in DMA mode
	   and read is in polling mode or vice versa.
	   To ensure that the data will be written to memory properly in DMA mode
	   words in the write buffer are swapped by software. During DMA transfer
	   DMA hardware again will do a word swap which will bring the data buffer
	   back to original values. Word swap is not required for read
	   buffer after read operation in DMA mode as I2S hardware will do
	   a word swap on the data before looping it back to receive registers.
	   This is peculiar behavior of the I2S HW in loopback mode
	 */
	/* Swap words in I2S write buffer */

	result = DMA_swapWords((Uint16*)i2sDmaWriteLeftBuff, 2*CSL_I2S_DMA_BUF_LEN);

	if(result != CSL_SOK)
	{
		printf ("DMA word swap API failed\n");
		status = CSL_TEST_FAILED;
		return (status);
	}

	result = DMA_swapWords((Uint16*)i2sDmaWriteRightBuff, 2*CSL_I2S_DMA_BUF_LEN);

	if(result != CSL_SOK)
	{
		printf ("DMA word swap API failed\n");
		status = CSL_TEST_FAILED;
		return (status);
	}

	/** Open the device with instance 0							*/
	hI2s = I2S_open(I2S_INSTANCE0, DMA_POLLED, I2S_CHAN_STEREO);
	if(NULL == hI2s)
	{
		status = CSL_TEST_FAILED;
		return (status);
	}
	else
	{
		printf ("I2S Module 0 Instance opened successfully\n");
	}


	/* Pin Muxing for Serial Port0 Pins--Mode1--I2S0 and GPIO[5:4]*/
		CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP0MODE, MODE1);


	/** Set the value for the configure structure				*/
	hwConfig.dataType 			= I2S_STEREO_ENABLE;
	hwConfig.loopBackMode 		= I2S_LOOPBACK_DISABLE;
	hwConfig.fsPol 				= I2S_FSPOL_HIGH;
	hwConfig.clkPol				= I2S_RISING_EDGE;
	hwConfig.datadelay			= I2S_DATADELAY_ONEBIT;
    hwConfig.dataFormat         = I2S_DATAFORMAT_LJUST;
	hwConfig.datapack			= I2S_DATAPACK_DISABLE;
	hwConfig.signext			= I2S_SIGNEXT_DISABLE;
	hwConfig.wordLen			= I2S_WORDLEN_32;
	hwConfig.i2sMode			= I2S_MASTER;
	hwConfig.clkDiv				= I2S_CLKDIV16;
	hwConfig.fsDiv				= I2S_FSDIV32;
	hwConfig.FError				= I2S_FSERROR_DISABLE;
	hwConfig.OuError			= I2S_OUERROR_DISABLE;

	/** Configure hardware registers							*/
	result = I2S_setup(hI2s, &hwConfig);
	if(result != CSL_SOK)
	{
		status = CSL_TEST_FAILED;
		return (status);
	}
	else
	{
		printf ("I2S Module 0 Configured successfully\n");
	}



	/* Configure DMA channel  for I2S write */
#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined(CHIP_C5517) || defined(CHIP_C5535) || defined(CHIP_C5545))
	dmaConfig.pingPongMode = CSL_DMA_PING_PONG_DISABLE;
#endif

	dmaConfig.autoMode     = CSL_DMA_AUTORELOAD_DISABLE;
	dmaConfig.burstLen     = CSL_DMA_TXBURST_16WORD;
	dmaConfig.trigger      = CSL_DMA_EVENT_TRIGGER;
	dmaConfig.dmaEvt       = CSL_DMA_EVT_I2S0_TX;
	dmaConfig.dmaInt       = CSL_DMA_INTERRUPT_DISABLE;
	dmaConfig.chanDir      = CSL_DMA_WRITE;
	dmaConfig.trfType      = CSL_DMA_TRANSFER_IO_MEMORY;
	dmaConfig.dataLen      = 64;
	dmaConfig.srcAddr      = (Uint32)i2sDmaWriteLeftBuff;
	dmaConfig.destAddr     = (Uint32)(0x2808);
	dmaLeftTxHandle = CSL_configDmaForI2s(CSL_DMA_CHAN1);
	if(dmaLeftTxHandle == NULL)
	{
		printf("DMA Config for I2S Write Failed!\n!");
		return(CSL_TEST_FAILED);
	}

	I2S_transEnable(hI2s, TRUE);

	status = DMA_start(dmaLeftTxHandle);
	if(status != CSL_SOK)
	{
		printf("I2S Dma Write Failed!!\n");
		return(result);
	}

	while(DMA_getStatus(dmaLeftTxHandle));

	/* Configure DMA channel  for I2S write */
#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined(CHIP_C5517) || defined(CHIP_C5535) || defined(CHIP_C5545))
	dmaConfig.pingPongMode = CSL_DMA_PING_PONG_DISABLE;
#endif

	dmaConfig.autoMode     = CSL_DMA_AUTORELOAD_DISABLE;
	dmaConfig.burstLen     = CSL_DMA_TXBURST_16WORD;
	dmaConfig.trigger      = CSL_DMA_EVENT_TRIGGER;
	dmaConfig.dmaEvt       = CSL_DMA_EVT_I2S0_TX;
	dmaConfig.dmaInt       = CSL_DMA_INTERRUPT_DISABLE;
	dmaConfig.chanDir      = CSL_DMA_WRITE;
	dmaConfig.trfType      = CSL_DMA_TRANSFER_IO_MEMORY;
	dmaConfig.dataLen      = 64;

	dmaConfig.srcAddr      = (Uint32)i2sDmaWriteRightBuff;
	dmaConfig.destAddr     = (Uint32)(0x280C);
	dmaRightTxHandle = CSL_configDmaForI2s(CSL_DMA_CHAN2);
	if(dmaRightTxHandle == NULL)
	{
		printf("DMA Config for I2S Write Failed!\n!");
		return(CSL_TEST_FAILED);
	}

	I2S_transEnable(hI2s, TRUE);

	status = DMA_start(dmaRightTxHandle);
	if(status != CSL_SOK)
	{
		printf("I2S Dma Write Failed!!\n");
		return(result);
	}
	while(DMA_getStatus(dmaRightTxHandle));

	#if 1
	/**********/

	/** Open the device with instance 2							*/

	hI2s = I2S_open(I2S_INSTANCE2, DMA_POLLED, I2S_CHAN_STEREO);
	if(NULL == hI2s)
	{
		status = CSL_TEST_FAILED;
		return (status);
	}
	else
	{
		printf ("I2S Module 2 Instance opened successfully\n");
	}

	/** Set the value for the configure structure				*/
	hwConfig.dataType 			= I2S_STEREO_ENABLE;
	hwConfig.loopBackMode 		= I2S_LOOPBACK_DISABLE;
	hwConfig.fsPol 				= I2S_FSPOL_HIGH;
	hwConfig.clkPol				= I2S_RISING_EDGE;
	hwConfig.datadelay			= I2S_DATADELAY_ONEBIT;
	hwConfig.datapack			= I2S_DATAPACK_DISABLE;
	hwConfig.signext			= I2S_SIGNEXT_DISABLE;
	hwConfig.wordLen			= I2S_WORDLEN_32;
	hwConfig.i2sMode			= I2S_SLAVE;
	hwConfig.clkDiv				= I2S_CLKDIV16;
	hwConfig.fsDiv				= I2S_FSDIV32;
   hwConfig.dataFormat         = I2S_DATAFORMAT_LJUST;
	hwConfig.FError				= I2S_FSERROR_DISABLE;
	hwConfig.OuError			= I2S_OUERROR_DISABLE;

	/** Configure hardware registers							*/
	result = I2S_setup(hI2s, &hwConfig);
	if(result != CSL_SOK)
	{
		status = CSL_TEST_FAILED;
		return (status);
	}
	else
	{
		printf ("I2S Module 2 Configured successfully\n");
	}


	/**********/

#endif

	/* Configure DMA channel  for I2S Read */
#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined(CHIP_C5517) || defined(CHIP_C5535) || defined(CHIP_C5545))
	dmaConfig.pingPongMode = CSL_DMA_PING_PONG_DISABLE;
#endif

	dmaConfig.autoMode     = CSL_DMA_AUTORELOAD_DISABLE;
	dmaConfig.burstLen     = CSL_DMA_TXBURST_16WORD;
	dmaConfig.trigger      = CSL_DMA_EVENT_TRIGGER;
	dmaConfig.dmaEvt       = CSL_DMA_EVT_I2S2_RX;
	dmaConfig.dmaInt       = CSL_DMA_INTERRUPT_DISABLE;
	dmaConfig.chanDir      = CSL_DMA_READ;
	dmaConfig.trfType      = CSL_DMA_TRANSFER_IO_MEMORY;
	dmaConfig.dataLen      = 64;

	dmaConfig.srcAddr      = (Uint32)(0x2A28);
	dmaConfig.destAddr     = (Uint32)i2sDmaReadLeftBuff;
	dmaLeftRxHandle = CSL_configDmaForI2s(CSL_DMA_CHAN4);
	if(dmaLeftRxHandle == NULL)
	{
		printf("DMA Config for I2S Read Failed!\n!");
		return(CSL_TEST_FAILED);
	}



	status = DMA_start(dmaLeftRxHandle);
	if(status != CSL_SOK)
	{
		printf("I2S Dma Read Failed!!\n");
		return(result);
	}

	I2S_transEnable(hI2s, TRUE);
	while(DMA_getStatus(dmaLeftRxHandle));



	/* Configure DMA channel  for I2S Read */
#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined(CHIP_C5517) || defined(CHIP_C5535) || defined(CHIP_C5545))
	dmaConfig.pingPongMode = CSL_DMA_PING_PONG_DISABLE;
#endif

	dmaConfig.autoMode     = CSL_DMA_AUTORELOAD_DISABLE;
	dmaConfig.burstLen     = CSL_DMA_TXBURST_16WORD;
	dmaConfig.trigger      = CSL_DMA_EVENT_TRIGGER;
	dmaConfig.dmaEvt       = CSL_DMA_EVT_I2S2_RX;
	dmaConfig.dmaInt       = CSL_DMA_INTERRUPT_DISABLE;
	dmaConfig.chanDir      = CSL_DMA_READ;
	dmaConfig.trfType      = CSL_DMA_TRANSFER_IO_MEMORY;
	dmaConfig.dataLen      = 64;

	dmaConfig.srcAddr      = (Uint32)(0x2A2C);
	dmaConfig.destAddr     = (Uint32)i2sDmaReadRightBuff;
	dmaRightRxHandle = CSL_configDmaForI2s(CSL_DMA_CHAN6);
	if(dmaRightRxHandle == NULL)
	{
		printf("DMA Config for I2S Read Failed!\n!");
		return(CSL_TEST_FAILED);
	}

	status = DMA_start(dmaRightRxHandle);
	if(status != CSL_SOK)
	{
		printf("I2S Dma Read Failed!!\n");
		return(result);
	}

	I2S_transEnable(hI2s, TRUE);
#if 1
	    printf("\nSystem is in IDLE2 mode. "
	        		"You may measure the power now..."
	    		"\n Expect The Alarm event and subsequent exit from "
	    		  " the IDLE state at the time mentioned above"

	    		);
    CSL_IDLECTRL_REGS->ICR |= (CSL_IDLE_ICR_HWAI_MASK | CSL_IDLE_ICR_CPUI_MASK);


    asm("\tIDLE                      ;====> CODE AUTO-GENERATED by CSL");

    asm("\tNOP");
    asm("\tNOP");
    asm("\tNOP");
    asm("\tNOP");
    asm("\tNOP");
    asm("\tNOP");
#endif
	while(DMA_getStatus(dmaRightRxHandle));

	I2S_transEnable(hI2s, FALSE);
	/** Reset the registers										*/
	result = I2S_reset(hI2s);

	if(result != CSL_SOK)
	{
		status = CSL_TEST_FAILED;
		return (status);
	}
	else
	{
		printf ("I2S Reset Successful\n");
	}

	/** Close the operation										*/
	result = I2S_close(hI2s);

	if(result != CSL_SOK)
	{
		status = CSL_TEST_FAILED;
		return (status);
	}
	else
	{
		printf ("I2S Close Successful\n");
	}

	/* Swap words in I2S write buffer
	   This will make the data in write buffer get back to original values
	   after that write buffer can be used for validating the read buffer
	   This sep is required only incase of comparing read and write buffers */
	/*for(looper=0; looper < CSL_I2S_DMA_BUF_LEN; looper++)
	{
		printf("I2S Readl[%d]: 0x%08x & Writel[%d]: 0x%08x\n"
				"I2S Readr[%d]: 0x%08x & Writer[%d]: 0x%08x\n\n",looper,
				i2sDmaReadLeftBuff[looper],looper,i2sDmaWriteLeftBuff[looper],looper,
				i2sDmaReadRightBuff[looper],looper,i2sDmaWriteRightBuff[looper]);
	}*/
	result = DMA_swapWords((Uint16*)i2sDmaWriteLeftBuff, 2*CSL_I2S_DMA_BUF_LEN);
	if(result != CSL_SOK)
	{
		printf ("DMA word swap API failed\n");
		status = CSL_TEST_FAILED;
		return (status);
	}
	result = DMA_swapWords((Uint16*)i2sDmaWriteRightBuff, 2*CSL_I2S_DMA_BUF_LEN);
	if(result != CSL_SOK)
	{
		printf ("DMA word swap API failed\n");
		status = CSL_TEST_FAILED;
		return (status);
	}


	/*for(looper=0; looper < CSL_I2S_DMA_BUF_LEN; looper++)
	{
		printf("I2S Readl[%d]: 0x%08x & Writel[%d]: 0x%08x\n"
				"I2S Readr[%d]: 0x%08x & Writer[%d]: 0x%08x\n\n",looper,
				i2sDmaReadLeftBuff[looper],looper,i2sDmaWriteLeftBuff[looper],looper,
				i2sDmaReadRightBuff[looper],looper,i2sDmaWriteRightBuff[looper]);
	}*/
	/** Compare the read and write buffer						*/
	for(looper=0; looper < CSL_I2S_DMA_BUF_LEN; looper++)
	{
		if( (i2sDmaWriteLeftBuff[looper] & 0xFFFFFFFF) != (i2sDmaReadLeftBuff[looper] & 0xFFFFFFFF))
		{
			printf("I2S Read & Write Buffers doesn't Match!!!\n");
			status = CSL_TEST_FAILED;
			return(status);
		}
	}

	if(looper == CSL_I2S_DMA_BUF_LEN)
	{
		printf("I2S Read & Write Buffers Match!!!\n");
	}

	status = CSL_TEST_PASSED;
	return (status);
}



#endif

   /////INSTRUMENTATION FOR BATCH TESTING -- Part 1 --
   /////  Define PaSs_StAtE variable for catching errors as program executes.
   /////  Define PaSs flag for holding final pass/fail result at program completion.
        volatile Int16 PaSs_StAtE = 0x0001; // Init to 1. Reset to 0 at any monitored execution error.
        volatile Int16 PaSs = 0x0000; // Init to 0.  Updated later with PaSs_StAtE when and if
   /////                                  program flow reaches expected exit point(s).
   /////
void main()
{
	CSL_Status    status;

	printf("CSL RTC TESTS\n\n");
	printf("This test demonstrates RTC ALARM interrupt generation capability\r\n");
	
	status =  rtc_test();

	if(status == CSL_SOK){

	}else{
		PaSs_StAtE=0x0000;
	}

    printf("Current Time and Date is : %02d:%02d:%02d:%04d, %02d-%02d-%02d\n",
	GetTime.hours,GetTime.mins,GetTime.secs,GetTime.mSecs,GetDate.day,GetDate.month,GetDate.year);

    IRQ_globalDisable();

	/* Clear any pending interrupts */
	IRQ_clearAll();

	/* Disable all the interrupts */
	IRQ_disableAll();

	/* Stop the RTC */
	RTC_stop();

	printf("\nCSL RTC TESTS COMPLETED\n");

	 /////INSTRUMENTATION FOR BATCH TESTING -- Part 3 --
   /////  At program exit, copy "PaSs_StAtE" into "PaSs".
        PaSs = PaSs_StAtE; //If flow gets here, override PaSs' initial 0 with
   /////                   // pass/fail value determined during program execution.
   /////  Note:  Program should next exit to C$$EXIT and halt, where DSS, under
   /////   control of a host PC script, will read and record the PaSs' value.
   /////
}


CSL_Status rtc_test(void){

	CSL_Status         status;
	CSL_Status         result;

	result = !CSL_SOK;


	#ifdef PHOENIX_QT

		 *((volatile ioport unsigned short *)0x1C27)=0x0001;
		 CSL_RTC_REGS->RGKR_LSW = 0xF1E0;
		 CSL_RTC_REGS->RGKR_MSW = 0x95A4;
	#elif EVM5515
#if (defined(CHIP_C5517))
		 CSL_SYSCTRL_REGS->RSCR = 0x0001;
		 CSL_RTC_REGS->RGKR_LSW = 0xF1E0;
		 CSL_RTC_REGS->RGKR_MSW = 0x95A4;
#endif
	#else
		#warn "Define either EVM5515 or PHOENIX_QT"
	#endif

	/* Set the RTC config structure */
	rtcConfig.rtcyear  = 8;
	rtcConfig.rtcmonth = 8;
	rtcConfig.rtcday   = 8;
	rtcConfig.rtchour  = 8;
	rtcConfig.rtcmin   = 8;
	rtcConfig.rtcsec   = 8;
	rtcConfig.rtcmSec  = 8;

	rtcConfig.rtcyeara  = 8;
	rtcConfig.rtcmontha = 8;
	rtcConfig.rtcdaya   = 8;
	rtcConfig.rtchoura  = 8;
	rtcConfig.rtcmina   = 8;
	rtcConfig.rtcseca   = 8;
	rtcConfig.rtcmSeca  = 10;

	//rtcConfig.rtcintcr  = 0x803F; //Enabled all the isr for the RTC
	rtcConfig.rtcintcr  = 0x8000; //Enabled only RTC alarm interrupt

	/* Set the RTC init structure */
    InitDate.year  = 2013;
    InitDate.month = 12;
    InitDate.day   = 3;

    InitTime.hours = 12;
    InitTime.mins  = 12;
    InitTime.secs  = 12;
    InitTime.mSecs = 12;

	/* Set the RTC alarm time */
    AlarmTime.year  = 2013;
    AlarmTime.month = 12;
    AlarmTime.day   = 3;
    AlarmTime.hours = 12;
    AlarmTime.mins  = 12;
    AlarmTime.secs  = 30;
    AlarmTime.mSecs = 512;


    /* Register the ISR function */
    isrAddr.AlarmEvtAddr  = rtc_alarmEvt;

    status = RTC_setCallback(&rtcDispatchTable, &isrAddr);
	if(status != CSL_SOK)
	{
		printf("RTC_setCallback Failed\n");
		return(result);
	}
	else
	{
		printf("RTC_setCallback Successful\n");
	}

	/* Configure and enable the RTC interrupts using INTC module */
    IRQ_globalDisable();

	/* Clear any pending interrupts */
	IRQ_clearAll();

	/* Disable all the interrupts */
	IRQ_disableAll();

	IRQ_setVecs((Uint32)&VECSTART);
	IRQ_clear(RTC_EVENT);

	IRQ_plug (RTC_EVENT, &rtc_isr);

	IRQ_enable(RTC_EVENT);
	IRQ_globalEnable();

	/* Reset the RTC */
	RTC_reset();

	/* Configure the RTC module */
	status = RTC_config(&rtcConfig);
	if(status != CSL_SOK)
	{
		printf("RTC_config Failed\n");
		return(result);
	}
	else
	{
		printf("RTC_config Successful\n");
	}

	/* Read the configuration values from the RTC module */
	status = RTC_getConfig(&rtcGetConfig);
	if(status != CSL_SOK)
	{
		printf("RTC_getConfig Failed\n");
		return(result);
	}
	else
	{
		printf("RTC_getConfig Successful\n");
	}

	/* Set the RTC time */
	status = RTC_setTime(&InitTime);
	if(status != CSL_SOK)
	{
		printf("RTC_setTime Failed\n");
		return(result);
	}
	else
	{
		printf("RTC_setTime Successful\n");
	}

	/* Set the RTC date */
	status = RTC_setDate(&InitDate);
	if(status != CSL_SOK)
	{
		printf("RTC_setDate Failed\n");
		return(result);
	}
	else
	{
		printf("RTC_setDate Successful\n");
	}

	/* Set the RTC Alarm time */
	status = RTC_setAlarm(&AlarmTime);
	if(status != CSL_SOK)
	{
		printf("RTC_setAlarm Failed\n");
		return(result);
	}
	else
	{
		printf("RTC_setAlarm Successful\n");
	}
	
	/* Enable the RTC alarm interrupts */
	status = RTC_eventEnable(CSL_RTC_ALARM_INTERRUPT);
	if(status != CSL_SOK)
	{
		printf("RTC_eventEnable for ALARM EVENT Failed\n");
		return(result);
	}
	else
	{
		printf("RTC_eventEnable for ALARM EVENT Successful\n");
	}

	printf("\nStarting the RTC..alarm will be produced on 12:12:40:0512, 3-12-13 ....Have a sleep \n\n");
	/* Start the RTC */
	RTC_start();

 	status = RTC_getTime(&GetTime);
	if(status != CSL_SOK)
	{
		printf("RTC_getTime Failed\n");
		return(result);
	}

 	status = RTC_getDate(&GetDate);
	if(status != CSL_SOK)
	{
		printf("RTC_getDate Failed\n");
		return(result);
	}


    printf("Current Time and Date is : %02d:%02d:%02d:%04d, %02d-%02d-%02d\n",
	GetTime.hours,GetTime.mins,GetTime.secs,GetTime.mSecs,GetDate.day,GetDate.month,GetDate.year);

#if 1

    printf("\nDisabling USB Clk domain\n");
    CSL_USB_REGS->FADDR_POWER |= CSL_USB_FADDR_POWER_SUSPENDM_MASK;
#if (defined(CHIP_C5517))
    CSL_SYSCTRL_REGS->CLKSTOP1 |= CSL_SYS_CLKSTOP1_USBCLKSTPREQ_MASK;
    while(!(CSL_SYSCTRL_REGS->CLKSTOP1 &
    		CSL_SYS_CLKSTOP1_USBCLKSTPACK_MASK));
    CSL_SYSCTRL_REGS->PCGCR2 |= CSL_SYS_PCGCR2_USBCG_MASK;
    CSL_SYSCTRL_REGS->USBSCR |= CSL_SYS_USBSCR_USBOSCDIS_MASK;

    CSL_SYSCTRL_REGS->CLKSTOP1 |=
    		(CSL_SYS_CLKSTOP1_UHPICLKSTPREQ_MASK |
    		CSL_SYS_CLKSTOP1_MBPCLKSTPREQ_MASK |
    		CSL_SYS_CLKSTOP1_URTCLKSTPREQ_MASK |
    		CSL_SYS_CLKSTOP1_EMIFCLKSTPREQ_MASK);
    while(!(CSL_SYSCTRL_REGS->CLKSTOP1 &
    			  (CSL_SYS_CLKSTOP1_UHPICLKSTPACK_MASK |
    				CSL_SYS_CLKSTOP1_MBPCLKSTPACK_MASK |
    				CSL_SYS_CLKSTOP1_URTCLKSTPACK_MASK |
    				CSL_SYS_CLKSTOP1_USBCLKSTPACK_MASK |
    				CSL_SYS_CLKSTOP1_EMIFCLKSTPACK_MASK)));


    CSL_SYSCTRL_REGS->CLKSTOP2 |=
    		(CSL_SYS_CLKSTOP2_MSPBRIDGECLKSTPREQ_MASK |
    				CSL_SYS_CLKSTOP2_MSPCLKSTPREQ_MASK);
    while(!(CSL_SYSCTRL_REGS->CLKSTOP2 &
    		(CSL_SYS_CLKSTOP2_MSPBRIDGECLKSTPACK_MASK |
    				CSL_SYS_CLKSTOP2_MSPCLKSTPACK_MASK)));
#else
#if (defined(CHIP_C5505_C5515) || defined(CHIP_C5504_C5514) || defined(CHIP_C5535) || defined(CHIP_C5545))

    CSL_SYSCTRL_REGS->CLKSTOP |= CSL_SYS_CLKSTOP_USBCLKSTPREQ_MASK;
    while(!(CSL_SYSCTRL_REGS->CLKSTOP &
    		CSL_SYS_CLKSTOP_USBCLKSTPACK_MASK));
    CSL_SYSCTRL_REGS->PCGCR2 |= CSL_SYS_PCGCR2_USBCG_MASK;
    CSL_SYSCTRL_REGS->USBSCR |= CSL_SYS_USBSCR_USBOSCDIS_MASK;

    CSL_SYSCTRL_REGS->CLKSTOP |=
    		(CSL_SYS_CLKSTOP_URTCLKSTPREQ_MASK |
    		CSL_SYS_CLKSTOP_EMFCLKSTPREQ_MASK);
    while(!(CSL_SYSCTRL_REGS->CLKSTOP &
    			  (CSL_SYS_CLKSTOP_URTCLKSTPACK_MASK |
    				CSL_SYS_CLKSTOP_USBCLKSTPACK_MASK |
    				CSL_SYS_CLKSTOP_EMFCLKSTPACK_MASK)));

#endif
#endif
    printf("\nDisabling Clk to all preipherals except"
    		" Sysclk to clk generator\n");
    CSL_SYSCTRL_REGS->PCGCR1 = 0x0;
    CSL_SYSCTRL_REGS->PCGCR2 = 0x0;
    CSL_SYSCTRL_REGS->PCGCR1 |= 0x3EF7;//0x7FFF;
    CSL_SYSCTRL_REGS->PCGCR2 |= 0xF7;//0xFF;

    CSL_CPU_REGS->IFR0 |= 0xFBFC;
    CSL_CPU_REGS->IFR1 |= 0x07FF;

    CSL_RTC_REGS->RTCINTREG |= CSL_RTC_RTCINTREG_EXTINTEN_MASK;

#if (defined(CHIP_C5517))
	 CSL_SYSCTRL_REGS->RSCR = 0x0001;
	 CSL_RTC_REGS->RGKR_LSW = 0xF1E0;
	 CSL_RTC_REGS->RGKR_MSW = 0x95A4;
#endif

#ifdef I2S_DMA_PORTION
		printf("CSL I2S DMA MODE(I2S0_I2S2_16Clk_1FS_1Dly) TEST!\n\n");

		status = i2s_DMA_sample();

		if(status != CSL_TEST_PASSED)
		{
			printf("\nCSL I2S DMA MODE(I2S0_I2S2_16Clk_1FS_1Dly) TXN FAILED!!\n\n");
	   /////INSTRUMENTATION FOR BATCH TESTING -- Part 2 --
	   /////  Reseting PaSs_StAtE to 0 if error detected here.
	       // PaSs_StAtE = 0x0000; // Was initialized to 1 at declaration.
	   /////
		}
		else
		{
			printf("\nCSL I2S DMA MODE(I2S0_I2S2_16Clk_1FS_1Dly) TXN PASSED!\n\n");
		}
#endif

#if 0
	    printf("\nSystem is in IDLE2 mode. "
	        		"You may measure the power now..."
	    		"\n Expect The Alarm event and subsequent exit from "
	    		  " the IDLE state at the time mentioned above"

	    		);
    CSL_IDLECTRL_REGS->ICR |= (CSL_IDLE_ICR_HWAI_MASK | CSL_IDLE_ICR_CPUI_MASK);

    //for(delay_count=0;delay_count<0xFFFF;delay_count++);
    asm("\tIDLE                      ;====> CODE AUTO-GENERATED by CSL");

    asm("\tNOP");
    asm("\tNOP");
    asm("\tNOP");
    asm("\tNOP");
    asm("\tNOP");
    asm("\tNOP");
#endif

#endif

	while(!alarm_event_intr); //wait for the alarm interrupt

 	status = RTC_getTime(&GetTime);
	if(status != CSL_SOK)
	{
		printf("RTC_getTime Failed\n");
		return(result);
	}

 	status = RTC_getDate(&GetDate);
	if(status != CSL_SOK)
	{
		printf("RTC_getDate Failed\n");
		return(result);
	}
	result = CSL_SOK;
	return(result);
}
interrupt void rtc_isr(void)
{

#ifdef RTC_CALL_BACK
    CSL_RTCEventType rtcEventType;

    rtcEventType = RTC_getEventId();

    if (((void (*)(void))(rtcDispatchTable.isr[rtcEventType])))
     {
         ((void (*)(void))(rtcDispatchTable.isr[rtcEventType]))();
     }
#else
    Uint16 statusRegVal;

    statusRegVal = CSL_RTC_REGS->RTCINTFL;

    /* check for alarm interrupt */
    if (CSL_RTC_RTCINTFL_ALARMFL_MASK ==
                (statusRegVal & (Uint16)CSL_RTC_RTCINTFL_ALARMFL_MASK ))
    {
		CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_ALARMFL, SET);
    }
    /* check for external event interrupt */
    else if (CSL_RTC_RTCINTFL_EXTFL_MASK ==
                (statusRegVal &(Uint16)CSL_RTC_RTCINTFL_EXTFL_MASK ))
    {
		CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_EXTFL, SET);
    }
    /* check for day interrupt */
    else if (CSL_RTC_RTCINTFL_DAYFL_MASK ==
                (statusRegVal & CSL_RTC_RTCINTFL_DAYFL_MASK))
    {
		CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_DAYFL, SET);
		CSL_FINST(CSL_RTC_REGS->R , RTC_RTCINTFL_DAYFL, SET);
    }
    /* check for hour interrupt */
    else if (CSL_RTC_RTCINTFL_HOURFL_MASK ==
                (statusRegVal & CSL_RTC_RTCINTFL_HOURFL_MASK))
    {
		CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_HOURFL, SET);
    }
    /* check for minute interrupt */
    else if (CSL_RTC_RTCINTFL_MINFL_MASK ==
                (statusRegVal & CSL_RTC_RTCINTFL_MINFL_MASK ))
    {
		CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_MINFL, SET);
    }
    /* check for seconds interrupt */
    else if (CSL_RTC_RTCINTFL_SECFL_MASK ==
                (statusRegVal & CSL_RTC_RTCINTFL_SECFL_MASK ))
    {
		CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_SECFL, SET);
    }
    /* check for milliseconds interrupt */
    else if (CSL_RTC_RTCINTFL_MSFL_MASK ==
                (statusRegVal & CSL_RTC_RTCINTFL_MSFL_MASK ))
    {
		CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_MSFL, SET);
    }
#endif
}

void rtc_alarmEvt(void)
{
    CSL_FINST(CSL_RTC_REGS->RTCINTFL, RTC_RTCINTFL_ALARMFL, SET);
	alarm_event_intr=1;
	printf("\nRTC Alarm Interrupt has been generated\n\n");
}
