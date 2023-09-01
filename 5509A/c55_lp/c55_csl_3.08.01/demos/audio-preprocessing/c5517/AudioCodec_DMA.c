/* ============================================================================
 * Copyright (c) 2017 Texas Instruments Incorporated.
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


/** @file AudioCodec_DMA.c
 *
 *  @brief Test code to verify the CSL I2S functionality
 *
 * \page    page6  I2S EXAMPLE DOCUMENTATION
 *
 *  \section I2S6    I2S EXAMPLE6 - AUDIO CODEC1 DMA IDLE LOOP
 *
 * \subsection I2S6x     TEST DESCRIPTION:
 *  1. Clear SEL_I2C_S0 and SEL_I2C_S1 (IO Expander lines P02,P03) to direct I2C data and scl lines to AIC0.
 *  2. Set AIC_MCBSP_MODE (IO Expander line P01) to choose AIC mode.
  * 3. Set SEL_MMC0_I2S to choose I2S mode. (IO Expander line P10).
  * 4. Send a reset to AIC0 by clearing AIC_RST (IO Expander line P00).
 *  5. Enable Clk to DMA and I2S0 and bring them out of reset.
 *  6. Configure I2S0 to be slave, l-justified stereo, loopback-datapack-sign extension disabled,
 *     wordlength 16-bit, Tx on rising edge, L-channel txn when FS low.
 *  7. Configure DMA channels 0,1,2,3 for L-R I2S audio tx and rx, 1 32-bit burst word, interrupts
 *     enabled, pingpong disabled, auto-reload enabled.
 *  8. Configure the Audio codec to collect line in over IN2_L and IN2_R, send it over to C5517 
 *     over I2S0 and on the way back, collect data from
 *     C5517 over I2S0, finally send it out of the HPL and HPR.
 *  9. Start the Rx-dma, and wait for two Rx-DMA transaction complete interrupts.
 *  10. The received audio data is stored in i2sDmaReadBufLeft and i2sDmaReadBufRight.
 *  11. Transfer this collected data into i2sDmaWriteBufLeft and i2sDmaWriteBufRight.
 *  12. Start the Tx-dma channels that will transfer this received data out of C5517 to the headphone.
 *  13. The process is repeated in a while(1).
 *
 *
 * NOTE: THIS TEST HAS BEEN DEVELOPED TO WORK WITH CHIP VERSIONS C5515/C5517.
 * MAKE SURE THAT PROPER CHIP VERSION MACRO IS DEFINED IN THE FILE
 * c55xx_csl\inc\csl_general.h.
 *
 * \subsection I2S6y      TEST PROCEDURE:
 *   Open the CCS and connect the target (C5515 EVM or C5517 EVM)
 *   Open the project "BF_rt_bios.pjt" and build it
 *   Load the program on to the target
 *   Set the PLL frequency to 100MHz
 *   Run the program and observe the test result
 *
 * \subsection I2S6z      TEST RESULT:
 *   All the CSL APIs should return success
 *   Audio lineout from PC should be heard in the Headphone
 *
 */

/* ============================================================================
 * Revision History
 * ================
 * 07th-Aug-2014 Created
 * 09th-Mar-2016 Header update
  * ============================================================================
 */

#include <log.h>
#include <std.h>
#include <mbx.h>
#include <sem.h>
#include <tsk.h>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

/* CSLR include files */
#include "soc.h"
#include "cslr.h"
#include "cslr_sysctrl.h"
/* CSL include files */
#include "csl_gpio.h"
#include "csl_i2s.h"
#include "csl_dma.h"
#include "csl_intc.h"
#include "csl_sysctrl.h"

#include "pll_control.h"
#include "codec_aic3254.h"
#include "codec_pcm186x.h"
#include "IdleLoop.h"
#include "BF_rt_bios_cfg.h"

#include "sfc31.h"
#ifdef PROFILE_CYCLES
#include "csl_gpt.h"
#endif

#define I2S_INSTANCE    ( I2S_INSTANCE2 )
#define DMA_CHAN_TX_L      ( CSL_DMA_CHAN4 )
#define DMA_CHAN_TX_R      ( CSL_DMA_CHAN5 )
#define DMA_CHAN_RX_L      ( CSL_DMA_CHAN6 )
#define DMA_CHAN_RX_R      ( CSL_DMA_CHAN7 )

#ifdef INSTANCE3_I2S
#define I2S_INSTANCE_2    ( I2S_INSTANCE3 )
#define DMA_CHAN_RX_L2      ( CSL_DMA_CHAN8 )
#define DMA_CHAN_RX_R2      ( CSL_DMA_CHAN9 )
#endif

#ifdef INSTANCE0_I2S
#define I2S_INSTANCE_3    ( I2S_INSTANCE0 )
#define DMA_CHAN_RX_L3      ( CSL_DMA_CHAN0 )
#define DMA_CHAN_RX_R3      ( CSL_DMA_CHAN1 )
#endif

#ifdef PROFILE_CYCLES
typedef struct cycles_s {
  long     max; /**< running maximum of cycles */
  long     avg; /**< running average of cycles (using exponential averaging) */
} cycles_t;

extern CSL_Handle  hGpt;
extern CSL_Config  hwConfig;
CSL_Status 	       statusInt = 0;
Uint32 		       timeCntInt1= 0;
Uint32 		       timeCntInt2 = 0;
cycles_t		   cyclesDmaInt = {0, 0};
cycles_t		   cyclesDownSamp = {0, 0};
cycles_t		   cyclesUpSamp = {0, 0};
static inline void process_cycles(long cycles, cycles_t * cyclesPtr)
{
  if (cycles > cyclesPtr->max) {
    cyclesPtr->max = cycles;
  }

  cyclesPtr->avg = ((cyclesPtr->avg) * 255 + cycles) >> 8;
}
#endif

extern void VECSTART(void); // defined in vector table
CSL_IRQ_Dispatch     dispatchTable;

CSL_GpioObj gGpioObj;
GPIO_Handle hGpio;

CSL_I2sHandle hI2s;

CSL_DMA_Handle dmaLeftTxHandle;
CSL_DMA_Handle dmaRightTxHandle;
CSL_DMA_ChannelObj dmaObj0, dmaObj1, dmaObj2, dmaObj3;
volatile Uint16 dmaTxFrameCount = 0, dmaRxFrameCount=0;

CSL_DMA_Handle dmaLeftRxHandle;
CSL_DMA_Handle dmaRightRxHandle;

/* Ping/pong buffers for serilizer 1 (I2S2) */
#ifdef ENABLE_AUDIO_CONVERSION
#pragma DATA_ALIGN(i2sDmaWriteBufLeftOut, 8);
Uint32 i2sDmaWriteBufLeftOut[I2S_DMA_BUF_LEN*SYS_FS_RATIO];  // left Tx write ping/pong buffer output
#pragma DATA_ALIGN(i2sDmaWriteBufRightOut, 8);
Uint32 i2sDmaWriteBufRightOut[I2S_DMA_BUF_LEN*SYS_FS_RATIO]; // right Tx write ping/pong buffe output
#pragma DATA_ALIGN(i2sDmaWriteBufLeftOutDelay, 8);
Uint16 i2sDmaWriteBufLeftOutDelay[I2S_DMA_BUF_LEN+(sfc_NUM_DEF_COEFS)/SYS_FS_RATIO];  // left Tx write delay buffer output
#pragma DATA_ALIGN(i2sDmaWriteBufRightOutDelay, 8);
Uint16 i2sDmaWriteBufRightOutDelay[I2S_DMA_BUF_LEN+(sfc_NUM_DEF_COEFS)/SYS_FS_RATIO]; // right Tx write delay buffe output
#endif
#pragma DATA_ALIGN(i2sDmaWriteBufLeft, 8);
Uint32 i2sDmaWriteBufLeft[I2S_DMA_BUF_LEN];  // left Tx write ping/pong buffer
#pragma DATA_ALIGN(i2sDmaWriteBufRight, 8);
Uint32 i2sDmaWriteBufRight[I2S_DMA_BUF_LEN]; // right Tx write ping/pong buffe
#ifdef ENABLE_AUDIO_CONVERSION
#pragma DATA_ALIGN(i2sDmaReadBufLeftIn, 8);
Uint32 i2sDmaReadBufLeftIn[I2S_DMA_BUF_LEN*SYS_FS_RATIO];  // left Rx read ping/pong buffer input
#pragma DATA_ALIGN(i2sDmaReadBufRightIn, 8);
Uint32 i2sDmaReadBufRightIn[I2S_DMA_BUF_LEN*SYS_FS_RATIO]; // right Rx read ping/pong buffer input
#pragma DATA_ALIGN(i2sDmaReadBufLeftInDelay, 8);
Uint16 i2sDmaReadBufLeftInDelay[I2S_DMA_BUF_LEN*SYS_FS_RATIO+sfc_NUM_DEF_COEFS];  // left Rx read delay buffer input
#pragma DATA_ALIGN(i2sDmaReadBufRightInDelay, 8);
Uint16 i2sDmaReadBufRightInDelay[I2S_DMA_BUF_LEN*SYS_FS_RATIO+sfc_NUM_DEF_COEFS]; // right Rx read delay buffer input
#endif
#pragma DATA_ALIGN(i2sDmaReadBufLeft, 8);
Uint32 i2sDmaReadBufLeft[I2S_DMA_BUF_LEN];  // left Rx read ping/pong buffer
#pragma DATA_ALIGN(i2sDmaReadBufRight, 8);
Uint32 i2sDmaReadBufRight[I2S_DMA_BUF_LEN]; // right Rx read ping/pong buffer

#ifdef INSTANCE3_I2S
CSL_I2sHandle hI2s2;

/* Ping/pong buffers for serilizer 2 (I2S3) */
#ifdef ENABLE_AUDIO_CONVERSION
#pragma DATA_ALIGN(i2sDmaReadBufLeft2In, 8);
Uint32 i2sDmaReadBufLeft2In[I2S_DMA_BUF_LEN*SYS_FS_RATIO];  // left Rx read ping/pong buffer 2 input
#pragma DATA_ALIGN(i2sDmaReadBufRight2In, 8);
Uint32 i2sDmaReadBufRight2In[I2S_DMA_BUF_LEN*SYS_FS_RATIO]; // right Rx read ping/pong buffer 2 input
#pragma DATA_ALIGN(i2sDmaReadBufLeft2InDelay, 8);
Uint16 i2sDmaReadBufLeft2InDelay[I2S_DMA_BUF_LEN*SYS_FS_RATIO+sfc_NUM_DEF_COEFS];  // left Rx read delay buffer 2 input
#pragma DATA_ALIGN(i2sDmaReadBufRight2InDelay, 8);
Uint16 i2sDmaReadBufRight2InDelay[I2S_DMA_BUF_LEN*SYS_FS_RATIO+sfc_NUM_DEF_COEFS]; // right Rx read delay buffer 2 input
#endif
#pragma DATA_ALIGN(i2sDmaReadBufLeft2, 8);
Uint32 i2sDmaReadBufLeft2[I2S_DMA_BUF_LEN];  // left Rx read ping/pong buffer 2
#pragma DATA_ALIGN(i2sDmaReadBufRight2, 8);
Uint32 i2sDmaReadBufRight2[I2S_DMA_BUF_LEN]; // right Rx read ping/pong buffer 2

CSL_DMA_Handle dmaLeftRxHandle2;
CSL_DMA_Handle dmaRightRxHandle2;
CSL_DMA_ChannelObj dmaObj4, dmaObj5;
#endif

#ifdef INSTANCE0_I2S
CSL_I2sHandle hI2s3;

/* Ping/pong buffers for serilizer 3 (I2S0) */
#ifdef ENABLE_AUDIO_CONVERSION
#pragma DATA_ALIGN(i2sDmaReadBufLeft3In, 8);
Uint32 i2sDmaReadBufLeft3In[I2S_DMA_BUF_LEN*SYS_FS_RATIO];  // left Rx read ping/pong buffer 3 input
#pragma DATA_ALIGN(i2sDmaReadBufRight3In, 8);
Uint32 i2sDmaReadBufRight3In[I2S_DMA_BUF_LEN*SYS_FS_RATIO]; // right Rx read ping/pong buffer 3 input
#endif
#pragma DATA_ALIGN(i2sDmaReadBufLeft3, 8);
Uint32 i2sDmaReadBufLeft3[I2S_DMA_BUF_LEN];  // left Rx read ping/pong buffer 3
#pragma DATA_ALIGN(i2sDmaReadBufRight3, 8);
Uint32 i2sDmaReadBufRight3[I2S_DMA_BUF_LEN]; // right Rx read ping/pong buffer 3

CSL_DMA_Handle dmaLeftRxHandle3;
CSL_DMA_Handle dmaRightRxHandle3;
CSL_DMA_ChannelObj dmaObj6, dmaObj7;
#endif

// Initializes device
CSL_Status DeviceInit(void);
#ifndef CHIP_C5517
// USB LDO Switch
void UsbLdoSwitch(Uint16 mode);
#endif
// Initializes DMA
CSL_Status DmaInit(void);

// GPIO initialization
CSL_Status GpioInit(Uint32 ioDir, Uint32 intEn, Uint32 intEdg);

// I2S and DMA initialization
CSL_Status I2sDmaInit(void);

// User defined algorithm
void UserAlgorithm(void);

// Put CPU in idle
void UserIdle(void);

// DMA ISR
void DmaIsr(void);

CSL_Status ioExpander_Setup(void);
CSL_Status ioExpander_Read(Uint16 port, Uint16 pin, Uint16 *rValue);
CSL_Status ioExpander_Write(Uint16 port, Uint16 pin, Uint16 value);

extern int *inFramePtr[];
extern int *buf_Outptr;
extern int *buf_Outptr2;
void bf_main (void);
int bf_process(void);

void main(void)
{
    CSL_Status status;

    // Enable HWA, CPU, DPORT, MPORT, XPORT, and IPORT in ICR */
    *(volatile ioport Uint16 *)(0x0001) = 0x000E;
    /* Execute idle instruction */
    asm("    idle");

    // enable all the peripherals
	CSL_SYSCTRL_REGS->PCGCR1 = 0x0000;
	CSL_SYSCTRL_REGS->PCGCR2 = 0x0000;

	// enable the CLKOUT to be the PLL
#ifdef CHIP_C5517
	CSL_SYSCTRL_REGS->CLKOUTCR = 0;
#endif

    printf("Audio Pre-processing Demo for C5517 EVM with LMB\n");
    printf("This Demo Will Continuously Get Audio Input from ");
    printf("LMB, Process the Audio Using BF+ASNR+MSS+DRC, then\n");
    printf("Send the Processed Audio to the Left Channel of the On-Board Codec (AIC3204-2)\n");
    printf("Bypass Audio Input from Mic1 of LMB to the Right Channel of the On-Board Codec\n\n");
    printf("Please Connect the Headphone to the Audio Output (Headphone) ");
    printf("Jack (P9)\n\n");

    // Initialize the audio DMA TX buffers
#ifdef ENABLE_AUDIO_CONVERSION
    memset(i2sDmaWriteBufLeftOut, 0, I2S_DMA_BUF_LEN*SYS_FS_RATIO*sizeof(Uint32));
    memset(i2sDmaWriteBufRightOut, 0, I2S_DMA_BUF_LEN*SYS_FS_RATIO*sizeof(Uint32));
    memset(i2sDmaReadBufLeftInDelay, 0, I2S_DMA_BUF_LEN*SYS_FS_RATIO*sizeof(Uint16));
    memset(i2sDmaReadBufRightInDelay, 0, I2S_DMA_BUF_LEN*SYS_FS_RATIO*sizeof(Uint16));
#else
    memset(i2sDmaWriteBufLeft, 0, I2S_DMA_BUF_LEN*sizeof(Uint32));
    memset(i2sDmaWriteBufRight, 0, I2S_DMA_BUF_LEN*sizeof(Uint32));
#endif

    // Initialize DSP
    status = DeviceInit();
    if (status != CSL_SOK)
    {
        printf("ERROR: Unable to initialize DSP\n");
        exit(EXIT_FAILURE);
    }

#ifdef CHIP_C5517
    status = ioExpander_Setup();
	if (status != CSL_SOK)
    {
        printf("ERROR: ioExpander_Setup Failed\n");
        exit(EXIT_FAILURE);
    }

    /* Configure I2C address for AIC3204-2 */
	status = ioExpander_Write(0, 2, 1);// SEL_I2C_S0 = 1
	status = ioExpander_Write(0, 3, 0);// SEL_I2C_S1 = 0
    /* Configure SPI2 mux for AIC3204-2 */
	status = ioExpander_Write(0, 5, 1);// SPI_I2S2_S0 = 1
	status = ioExpander_Write(0, 6, 1); // SPI_I2S2_S1 = 1
    /* Release AIC3204 reset */
	status = ioExpander_Write(0, 0, 0);// AIC_RST = 0
	if (status != CSL_SOK)
    {
        printf("ERROR: ioExpander_Write Failed\n");
        exit(EXIT_FAILURE);
    }
#endif

	// Initialize DMA
    status = DmaInit();
    if (status != CSL_SOK)
    {
        printf("ERROR: Unable to initialize DMA\n");
        exit(EXIT_FAILURE);
    }

#ifndef CHIP_C5517
    /* GPIO10 for AIC3204 reset */
    Uint32 gpioIoDir = (((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN10);
    status = GpioInit(gpioIoDir, 0x00000000, 0x00000000);
    if (status != CSL_SOK)
    {
        printf("ERROR: Unable to initialize GPIO\n");
        exit(1);
    }
#endif

    // Initialize I2S with DMA
    status = I2sDmaInit();
    if (status != CSL_SOK)
    {
        printf("ERROR: Unable to initialize I2S and DMA\n");
        exit(1);
    }

    /* Intialize AIC3204 codec */
    status = AIC3254_init(16000, 16000, PLL_MHZ);
    if (status != CSL_SOK)
    {
        printf("ERROR: Unable to initialize AIC3204\n");
        exit(1);
    }

    /* Intialize PCM1864 codec */
    status = PCM186x_init(16000, PLL_MHZ);
    if (status != CSL_SOK)
    {
        printf("ERROR: Unable to initialize PCM186x\n");
        exit(1);
    }

    /* Set CPUI bit in ICR */
    //*(volatile ioport Uint16 *)(0x0001) |= 0x000F;
    /* Set CPUI, DPORTI, XPORTI, and IPORTI in ICR */
    *(volatile ioport Uint16 *)(0x0001) |= 0x016F;

    /* Enable global interrupt */
    IRQ_globalEnable();

    if(!dmaRxFrameCount)
    {
    	/* Start left Rx DMA I2S2 */
    	status = DMA_start(dmaLeftRxHandle);
    	if (status != CSL_SOK)
    	{
    		printf("I2S Dma Left Rx for I2S2 Failed!!\n");
    		exit(EXIT_FAILURE);
    	}


    	/* Start right Rx DMA for I2S2 */
    	status = DMA_start(dmaRightRxHandle);
    	if (status != CSL_SOK)
    	{
    		printf("I2S Dma Right Rx for I2S2 Failed!!\n");
    		exit(EXIT_FAILURE);
    	}

#ifdef INSTANCE3_I2S
    	/* Start left Rx DMA for I2S3 */
    	status = DMA_start(dmaLeftRxHandle2);
    	if (status != CSL_SOK)
    	{
    		printf("I2S Dma Left Rx for I2S3 Failed!!\n");
    		exit(EXIT_FAILURE);
    	}


    	/* Start right Rx DMA for I2S3 */
    	status = DMA_start(dmaRightRxHandle2);
    	if (status != CSL_SOK)
    	{
    		printf("I2S Dma Right Rx for I2S3 Failed!!\n");
    		exit(EXIT_FAILURE);
    	}
#endif

#ifdef INSTANCE0_I2S
    	/* Start left Rx DMA for I2S0 */
    	status = DMA_start(dmaLeftRxHandle3);
    	if (status != CSL_SOK)
    	{
    		printf("I2S Dma Left Rx for I2S0 Failed!!\n");
    		exit(EXIT_FAILURE);
    	}


    	/* Start right Rx DMA for I2S0 */
    	status = DMA_start(dmaRightRxHandle3);
    	if (status != CSL_SOK)
    	{
    		printf("I2S Dma Right Rx for I2S0 Failed!!\n");
    		exit(EXIT_FAILURE);
    	}
#endif
    }

    if(!dmaTxFrameCount)
    {
    	/* Start left Tx DMA */
    	status = DMA_start(dmaLeftTxHandle);
    	if (status != CSL_SOK)
    	{
    		printf("I2S Dma Left Tx Failed!!\n");
    		exit(EXIT_FAILURE);
    	}

    	/* Start right Tx DMA */
    	status = DMA_start(dmaRightTxHandle);
    	if (status != CSL_SOK)
    	{
    		printf("I2S Dma Right Tx Failed!!\n");
    		exit(EXIT_FAILURE);
    	}
    }

    /* Enable I2S */
    CSL_I2S2_REGS->I2SSCTRL |= 0x8000;
#ifdef INSTANCE3_I2S
    CSL_I2S3_REGS->I2SSCTRL |= 0x8000;
#endif
#ifdef INSTANCE0_I2S
    CSL_I2S0_REGS->I2SSCTRL |= 0x8000;
#endif

#ifndef LOOPBACK_ONLY
    // setup BF, ASNR and MSS
    bf_main();
#endif

    printf("Pumping audio to the headphone output P9 started!!\n");


}

void audioLoopTask()
{
    while (1)
    {
    	// wait for the I2S DMA is done
    	SEM_pend(&SEM_i2s_dmaTransferDone, SYS_FOREVER);
        /* Perform your algorithm here */
        UserAlgorithm();

        /* Set CPU to Idle */
        //UserIdle();
    }
}
//*****************************************************************************
// DeviceInit
//*****************************************************************************
//  Description:
//      Sets up interrupt vector table,
//      Disables and clears interrupts,
//      Resets all peripherals,
//      Sets PLL output to desired MHz,
//      Disables USB LDO
//
//  Arguments:
//      none
//
//  Return:
//      CSL_Status: CSL_SOK - Device initialization successful
//                  other   - Device initialization unsuccessful
//
//*****************************************************************************
CSL_Status DeviceInit(void)
{
    Uint16 prcr;
    CSL_Status status;

    // Set up dispatch table
    status = IRQ_init(&dispatchTable, 0);
    if (status != CSL_SOK)
    {
        //printf("IRQ_init() fail, status=%d\n", status);
        return status;
    }

    // Disable global interrupts
    IRQ_globalDisable();

    // Disable all the interrupts
    IRQ_disableAll();

    // Clear any pending interrupts
    IRQ_clearAll();

    // Set the interrupt vector table address
    status = IRQ_setVecs((Uint32)&VECSTART);
    if (status != CSL_SOK)
    {
        //printf("IRQ_setVecs() fail, status=%d\n", status);
        return status;
    }

    // Reset all peripherals
    CSL_SYSCTRL_REGS->PSRCR = 0x0020;
    CSL_SYSCTRL_REGS->PRCR = 0x00BF;
    do
    {
        prcr = CSL_SYSCTRL_REGS->PRCR;
    } while (prcr != 0);

#ifndef CHIP_C5517

    // Set PLL output frequency
    // Note: desired frequency must be allowed for core voltage setting
    status = pll_sample(PLL_MHZ);
    if (status != CSL_SOK)
    {
        //printf("pll_sample() fail, status=%d\n", status);
        return status;
    }

    // Turn off USB LDO
    UsbLdoSwitch(0);
#else
    // Set PLL to 200Mhz for C5517
    status = pll_sample(200);
    if (status != CSL_SOK)
    {
        printf("pll_sample(200) fail, status=%d\n", status);
        return status;
    }
#endif

    return CSL_SOK;
}

#ifndef CHIP_C5517

//*****************************************************************************
// UsbLdoSwitch
//*****************************************************************************
//  Description: Enables/disables USB LDO
//
//  Arguments:
//      mode:   0 - disable USB LDO
//              1 - enable USB LDO
//              other - no change
//
//  Return:
//      none
//
//*****************************************************************************
void UsbLdoSwitch(Uint16 mode)
{
    Uint16 pcgcr2;

    pcgcr2 = CSL_SYSCTRL_REGS->PCGCR2; // save PCGCR2

    /* Enable the Analog Register only */
    CSL_SYSCTRL_REGS->PCGCR2 &= ~(0x0040);

#if (defined(CHIP_C5517))
    if (mode == 0)
    {
        /* Disable USB LDO (clear bit 0) */
        CSL_LDO_REGS->LDOCTRL &= 0xFFFE;
    }

    if (mode == 1)
    {
        /* Enable USB LDO (set bit 0) */
        CSL_LDO_REGS->LDOCTRL |= 0x0001;
    }
#else
    if (mode == 0)
    {
        /* Disable USB LDO (clear bit 0) */
        CSL_SAR_REGS->USBLDOCNTL &= 0xFFFE;
    }

    if (mode == 1)
    {
        /* Enable USB LDO (set bit 0) */
        CSL_SAR_REGS->USBLDOCNTL |= 0x0001;
    }
#endif

    CSL_SYSCTRL_REGS->PCGCR2 = pcgcr2; // restore PCGCR2
}

#endif//CHIP_C5517

//*****************************************************************************
// DmaInit
//*****************************************************************************
//  Description: Initializes DMA, enables DMA interrupts
//
//  Arguments:
//      none
//
//  Return:
//      CSL_Status: CSL_SOK - DMA initialization successful
//                  other   - DMA initialization unsuccessful
//
//*****************************************************************************
CSL_Status DmaInit(void)
{
    Uint16 ifrValue;
    CSL_Status status;

    // Disable DMA interrupt
    IRQ_disable(DMA_EVENT);

    // Clear DMA interrupt
    IRQ_clear(DMA_EVENT);

    // Disable DMA interrupts
    CSL_SYSCTRL_REGS->DMAIER = 0x0000;

    // Clear pending DMA interrupts
    ifrValue = CSL_SYSCTRL_REGS->DMAIFR;
    CSL_SYSCTRL_REGS->DMAIFR |= ifrValue;

    // Plug in DMA ISR
    IRQ_plug(DMA_EVENT, &DmaIsr);

    // DMA initialization
    status = DMA_init();
    if (status != CSL_SOK)
    {
        //printf("DMA_init() fail, status=%d\n", status);
        return status;
    }

    // Enable DMA interrupt
    IRQ_enable(DMA_EVENT);

    return CSL_SOK;
}

#ifndef CHIP_C5517
//*****************************************************************************
// GpioInit
//*****************************************************************************
//  Description: Initializes GPIO module
//
//  Arguments:
//      hGpio       - handle to GPIO (global)
//      gGpioObj    - GPIO parameter structure (global)
//      ioDir       - GPIO direction
//      intEn       - GPIO interrupt enables
//      intEdg      - GPIO edge
//
//  Return:
//      CSL_Status: CSL_SOK - GPIO module initialization successful
//                  other   - GPIO module initialization unsuccessful
//
//*****************************************************************************
CSL_Status GpioInit(
    Uint32 ioDir,
    Uint32 intEn,
    Uint32 intEdg
)
{
    CSL_Status status;
    CSL_GpioConfig config;

    /* Open GPIO module */
    hGpio = GPIO_open(&gGpioObj, &status);
    if ((hGpio == NULL) || (status != CSL_SOK))
    {
        return status;
    }

    /* Reset GPIO module */
    status = GPIO_reset(hGpio);
    if (status != CSL_SOK)
    {
        return status;
    }

    /* Configure GPIO module */
    config.GPIODIRL = ioDir & 0xFFFF;
    config.GPIODIRH = ioDir >> 16;
    config.GPIOINTENAL = intEn & 0xFFFF;
    config.GPIOINTENAH = intEn >> 16;
    config.GPIOINTTRIGL = intEdg & 0xFFFF;
    config.GPIOINTTRIGH = intEdg >> 16;
    status = GPIO_config(hGpio, &config);
    if (status != CSL_SOK)
    {
        return status;
    }

    return CSL_SOK;
}
#endif

//*****************************************************************************
// I2sDmaInit
//*****************************************************************************
//  Description:
//      Configures and starts I2S with DMA
//
//  Parameters:
//      hI2s                - I2S handle (global)
//      dmaLeftTxHandle     - handle for I2S left DMA (global)
//      dmaRightTxHandle    - handle for I2S right DMA (global)
//
//      CSL_Status: CSL_SOK - I2S & DMA initialization successful
//                  other   - I2S & DMA initialization unsuccessful
//
//*****************************************************************************
CSL_Status I2sDmaInit(void)
{
    I2S_Config hwConfig;
    CSL_DMA_Config dmaConfig;
    CSL_Status status;

    /* DMA engine initialization */
    /* Open the device with I2S 2 */
    /*(AIC3204-2 is connected to I2S2 on C5517 EVM) */
    hI2s = I2S_open(I2S_INSTANCE, DMA_POLLED, I2S_CHAN_STEREO);
    if(NULL == hI2s)
    {
        status = CSL_ESYS_FAIL;
        return (status);
    }
    else
    {
        printf ("I2S2 Module Instance opened successfully\n");
    }

#ifdef INSTANCE0_I2S
    // Set Pinmux for I2S0
	status = SYS_setEBSR(CSL_EBSR_FIELD_SP0MODE,
						 CSL_EBSR_SP0MODE_1);
#endif

    // Set Pinmux for I2S2 and I2S3
	status = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
						 CSL_EBSR_PPMODE_6);

#ifndef CHIP_C5517
    // Serial Port mode 1 (I2S1 and GP[11:10]).
    //*(volatile ioport Uint16*)(CSL_SYSCTRL_REGS) |=  0x0800;
    status = SYS_setEBSR(CSL_EBSR_FIELD_SP1MODE,
    						CSL_EBSR_SP1MODE_2);
#endif
	if(CSL_SOK != status)
    {
        printf("SYS_setEBSR failed\n");
        return (status);
    }

    /* Set the value for the configure structure */
    hwConfig.dataFormat     = I2S_DATAFORMAT_LJUST;
    hwConfig.dataType       = I2S_STEREO_ENABLE;
    hwConfig.loopBackMode   = I2S_LOOPBACK_DISABLE;
    hwConfig.fsPol          = I2S_FSPOL_HIGH; //I2S_FSPOL_LOW;
    hwConfig.clkPol         = I2S_RISING_EDGE;//I2S_FALLING_EDGE;
    hwConfig.datadelay      = I2S_DATADELAY_ONEBIT;
#ifdef USE_32BIT_SAMPLE
    hwConfig.datapack       = I2S_DATAPACK_DISABLE;
#else
    hwConfig.datapack       = I2S_DATAPACK_ENABLE;
#endif
    hwConfig.signext        = I2S_SIGNEXT_DISABLE;
    hwConfig.wordLen        = I2S_WORDLEN_16;
    hwConfig.i2sMode        = I2S_SLAVE;
    hwConfig.clkDiv         = I2S_CLKDIV2; // don't care for slave mode
    hwConfig.fsDiv          = I2S_FSDIV32; // don't care for slave mode
    hwConfig.FError         = I2S_FSERROR_DISABLE;
    hwConfig.OuError        = I2S_OUERROR_DISABLE;

    /* Configure hardware registers */
    status = I2S_setup(hI2s, &hwConfig);
    if(status != CSL_SOK)
    {
        return (status);
    }
    else
    {
        printf ("I2S2 Module Configured successfully\n");
    }

#ifdef INSTANCE3_I2S
    /* DMA engine initialization */
    /* Open the device with instance 3 */
    hI2s2 = I2S_open(I2S_INSTANCE_2, DMA_POLLED, I2S_CHAN_STEREO);
    if(NULL == hI2s2)
    {
        status = CSL_ESYS_FAIL;
        return (status);
    }
    else
    {
        printf ("I2S3 Module Instance opened successfully\n");
    }

    /* Set the value for the configure structure */
    hwConfig.dataFormat     = I2S_DATAFORMAT_LJUST;
    hwConfig.dataType       = I2S_STEREO_ENABLE;
    hwConfig.loopBackMode   = I2S_LOOPBACK_DISABLE;
    hwConfig.fsPol          = I2S_FSPOL_HIGH; //I2S_FSPOL_LOW;
    hwConfig.clkPol         = I2S_RISING_EDGE;//I2S_FALLING_EDGE;
    hwConfig.datadelay      = I2S_DATADELAY_ONEBIT;
    hwConfig.datapack       = I2S_DATAPACK_ENABLE; //I2S_DATAPACK_DISABLE;
    hwConfig.signext        = I2S_SIGNEXT_DISABLE;
    hwConfig.wordLen        = I2S_WORDLEN_16;
    hwConfig.i2sMode        = I2S_SLAVE;
    hwConfig.clkDiv         = I2S_CLKDIV2; // don't care for slave mode
    hwConfig.fsDiv          = I2S_FSDIV32; // don't care for slave mode
    hwConfig.FError         = I2S_FSERROR_DISABLE;
    hwConfig.OuError        = I2S_OUERROR_DISABLE;

    /* Configure hardware registers */
    status = I2S_setup(hI2s2, &hwConfig);
    if(status != CSL_SOK)
    {
        return (status);
    }
    else
    {
        printf ("I2S3 Module Configured successfully\n");
    }
#endif

#ifdef INSTANCE0_I2S
    /* DMA engine initialization */
    /* Open the device with instance 3 */
    hI2s3 = I2S_open(I2S_INSTANCE_3, DMA_POLLED, I2S_CHAN_STEREO);
    if(NULL == hI2s3)
    {
        status = CSL_ESYS_FAIL;
        return (status);
    }
    else
    {
        printf ("I2S0 Module Instance opened successfully\n");
    }

    /* Set the value for the configure structure */
    hwConfig.dataFormat     = I2S_DATAFORMAT_LJUST;
    hwConfig.dataType       = I2S_STEREO_ENABLE;
    hwConfig.loopBackMode   = I2S_LOOPBACK_DISABLE;
    hwConfig.fsPol          = I2S_FSPOL_HIGH; //I2S_FSPOL_LOW;
    hwConfig.clkPol         = I2S_RISING_EDGE;//I2S_FALLING_EDGE;
    hwConfig.datadelay      = I2S_DATADELAY_ONEBIT;
    hwConfig.datapack       = I2S_DATAPACK_ENABLE; //I2S_DATAPACK_DISABLE;
    hwConfig.signext        = I2S_SIGNEXT_DISABLE;
    hwConfig.wordLen        = I2S_WORDLEN_16;
    hwConfig.i2sMode        = I2S_SLAVE;
    hwConfig.clkDiv         = I2S_CLKDIV2; // don't care for slave mode
    hwConfig.fsDiv          = I2S_FSDIV32; // don't care for slave mode
    hwConfig.FError         = I2S_FSERROR_DISABLE;
    hwConfig.OuError        = I2S_OUERROR_DISABLE;

    /* Configure hardware registers */
    status = I2S_setup(hI2s3, &hwConfig);
    if(status != CSL_SOK)
    {
        return (status);
    }
    else
    {
        printf ("I2S0 Module Configured successfully\n");
    }
#endif

    /* Configure DMA channel 4 for I2S2 left channel write*/
    dmaConfig.pingPongMode  = CSL_DMA_PING_PONG_ENABLE;
    dmaConfig.autoMode      = CSL_DMA_AUTORELOAD_ENABLE;
    dmaConfig.burstLen      = CSL_DMA_TXBURST_1WORD;
    dmaConfig.trigger       = CSL_DMA_EVENT_TRIGGER;
    dmaConfig.dmaEvt        = CSL_DMA_EVT_I2S2_TX;

    dmaConfig.dmaInt        = CSL_DMA_INTERRUPT_ENABLE;
    dmaConfig.chanDir       = CSL_DMA_WRITE;
    dmaConfig.trfType       = CSL_DMA_TRANSFER_IO_MEMORY;
#ifdef ENABLE_AUDIO_CONVERSION
    dmaConfig.dataLen       = I2S_DMA_BUF_LEN*SYS_FS_RATIO*4; // frame buffer size in bytes
#else
    dmaConfig.dataLen       = I2S_DMA_BUF_LEN*4; // frame buffer size in bytes
#endif

#if (defined(CHIP_C5517))
    dmaConfig.destAddr      = (Uint32)&CSL_I2S2_REGS->I2STXLTL;
#else
    dmaConfig.destAddr      = (Uint32)&CSL_I2S2_REGS->I2STXLT0;
#endif
#ifdef ENABLE_AUDIO_CONVERSION
    dmaConfig.srcAddr       = (Uint32)i2sDmaWriteBufLeftOut;
#else
    dmaConfig.srcAddr       = (Uint32)i2sDmaWriteBufLeft;
#endif

    /* Open DMA ch4 for I2S2 left channel write*/
    dmaLeftTxHandle = DMA_open(DMA_CHAN_TX_L, &dmaObj0,&status);
    if (dmaLeftTxHandle == NULL)
    {
        printf("DMA_open CH4 Failed \n");
        dmaLeftTxHandle = NULL;
    }

    /* Configure DMA channel4 */
    status = DMA_config(dmaLeftTxHandle, &dmaConfig);
    if (status != CSL_SOK)
    {
        printf("DMA_config CH4 Failed \n");
        dmaLeftTxHandle = NULL;
    }

    /* Configure DMA ch5 for I2S2 right channel write*/
    dmaConfig.pingPongMode = CSL_DMA_PING_PONG_ENABLE;
    dmaConfig.autoMode     = CSL_DMA_AUTORELOAD_ENABLE;
    dmaConfig.burstLen     = CSL_DMA_TXBURST_1WORD;
    dmaConfig.trigger      = CSL_DMA_EVENT_TRIGGER;
    dmaConfig.dmaEvt        = CSL_DMA_EVT_I2S2_TX;
    dmaConfig.dmaInt       = CSL_DMA_INTERRUPT_ENABLE;
    dmaConfig.chanDir      = CSL_DMA_WRITE;
    dmaConfig.trfType      = CSL_DMA_TRANSFER_IO_MEMORY;
#ifdef ENABLE_AUDIO_CONVERSION
    dmaConfig.dataLen      = I2S_DMA_BUF_LEN*SYS_FS_RATIO*4; // frame buffer size in bytes
#else
    dmaConfig.dataLen      = I2S_DMA_BUF_LEN*4; // frame buffer size in bytes
#endif

#if (defined(CHIP_C5517))
    dmaConfig.destAddr     = (Uint32)&CSL_I2S2_REGS->I2STXRTL;
#else
    dmaConfig.destAddr     = (Uint32)&CSL_I2S2_REGS->I2STXRT0;
#endif


#ifdef ENABLE_AUDIO_CONVERSION
    dmaConfig.srcAddr      = (Uint32)i2sDmaWriteBufRightOut;
#else
    dmaConfig.srcAddr      = (Uint32)i2sDmaWriteBufRight;
#endif

    /* Open DMA ch5 for I2S2 right channel write*/
    dmaRightTxHandle = DMA_open(DMA_CHAN_TX_R, &dmaObj1,&status);
    if (dmaRightTxHandle == NULL)
    {
        printf("DMA_open CH5 Failed \n");
        dmaRightTxHandle = NULL;
    }

    /* Configure DMA channel5*/
    status = DMA_config(dmaRightTxHandle, &dmaConfig);
    if (status != CSL_SOK)
    {
        printf("DMA_config CH5 Failed \n");
        dmaRightTxHandle = NULL;
    }

    /* Configure DMA channel 6 for I2S2 left channel read */
    dmaConfig.pingPongMode  = CSL_DMA_PING_PONG_ENABLE;
    dmaConfig.autoMode      = CSL_DMA_AUTORELOAD_ENABLE;
    dmaConfig.burstLen      = CSL_DMA_TXBURST_1WORD;
    dmaConfig.trigger       = CSL_DMA_EVENT_TRIGGER;
    dmaConfig.dmaEvt        = CSL_DMA_EVT_I2S2_RX;
    dmaConfig.dmaInt        = CSL_DMA_INTERRUPT_ENABLE;
    dmaConfig.chanDir       = CSL_DMA_READ;
    dmaConfig.trfType       = CSL_DMA_TRANSFER_IO_MEMORY;
#ifdef ENABLE_AUDIO_CONVERSION
    dmaConfig.dataLen       = I2S_DMA_BUF_LEN*SYS_FS_RATIO*4; // frame buffer size in bytes
#else
    dmaConfig.dataLen       = I2S_DMA_BUF_LEN*4; // frame buffer size in bytes
#endif

#if (defined(CHIP_C5517))
    dmaConfig.srcAddr      = (Uint32)&CSL_I2S2_REGS->I2SRXLTL;
#else
    dmaConfig.srcAddr      = (Uint32)&CSL_I2S2_REGS->I2SRXLT0;
#endif

#ifdef ENABLE_AUDIO_CONVERSION
    dmaConfig.destAddr       = (Uint32)i2sDmaReadBufLeftIn;
#else
    dmaConfig.destAddr       = (Uint32)i2sDmaReadBufLeft;
#endif

    /* Open DMA ch6 for I2S2 left channel read */
    dmaLeftRxHandle = DMA_open(DMA_CHAN_RX_L, &dmaObj2,&status);
    if (dmaLeftRxHandle == NULL)
    {
        printf("DMA_open CH6 Failed \n");
        dmaLeftRxHandle = NULL;
    }

    /* Configure DMA channel6 */
    status = DMA_config(dmaLeftRxHandle, &dmaConfig);
    if (status != CSL_SOK)
    {
        printf("DMA_config CH6 Failed \n");
        dmaLeftRxHandle = NULL;
    }

    /* Configure DMA ch7 for I2S2 right channel read */
    dmaConfig.pingPongMode = CSL_DMA_PING_PONG_ENABLE;
    dmaConfig.autoMode     = CSL_DMA_AUTORELOAD_ENABLE;
    dmaConfig.burstLen     = CSL_DMA_TXBURST_1WORD;
    dmaConfig.trigger      = CSL_DMA_EVENT_TRIGGER;
    dmaConfig.dmaEvt        = CSL_DMA_EVT_I2S2_RX;
    dmaConfig.dmaInt       = CSL_DMA_INTERRUPT_ENABLE;
    dmaConfig.chanDir      = CSL_DMA_READ;
    dmaConfig.trfType      = CSL_DMA_TRANSFER_IO_MEMORY;
#ifdef ENABLE_AUDIO_CONVERSION
    dmaConfig.dataLen      = I2S_DMA_BUF_LEN*SYS_FS_RATIO*4; // frame buffer size in bytes
#else
    dmaConfig.dataLen      = I2S_DMA_BUF_LEN*4; // frame buffer size in bytes
#endif

#if (defined(CHIP_C5517))
    dmaConfig.srcAddr     = (Uint32)&CSL_I2S2_REGS->I2SRXRTL;
#else
    dmaConfig.srcAddr     = (Uint32)&CSL_I2S2_REGS->I2SRXRT0;
#endif

#ifdef ENABLE_AUDIO_CONVERSION
    dmaConfig.destAddr      = (Uint32)i2sDmaReadBufRightIn;
#else
    dmaConfig.destAddr      = (Uint32)i2sDmaReadBufRight;
#endif

    /* Open DMA ch7 for I2S2 right channel read */
    dmaRightRxHandle = DMA_open(DMA_CHAN_RX_R, &dmaObj3,&status);
    if (dmaRightRxHandle == NULL)
    {
        printf("DMA_open CH7 Failed \n");
        dmaRightRxHandle = NULL;
    }

    /* Configure DMA channel */
    status = DMA_config(dmaRightRxHandle, &dmaConfig);
    if (status != CSL_SOK)
    {
        printf("DMA_config CH7 Failed \n");
        dmaRightRxHandle = NULL;
    }

#ifdef INSTANCE3_I2S
    /* Configure DMA channel 8 for I2S3 left channel read */
    dmaConfig.pingPongMode  = CSL_DMA_PING_PONG_ENABLE;
    dmaConfig.autoMode      = CSL_DMA_AUTORELOAD_ENABLE;
    dmaConfig.burstLen      = CSL_DMA_TXBURST_1WORD;
    dmaConfig.trigger       = CSL_DMA_EVENT_TRIGGER;
    dmaConfig.dmaEvt        = CSL_DMA_EVT_I2S3_RX;
    dmaConfig.dmaInt        = CSL_DMA_INTERRUPT_ENABLE;
    dmaConfig.chanDir       = CSL_DMA_READ;
    dmaConfig.trfType       = CSL_DMA_TRANSFER_IO_MEMORY;
#ifdef ENABLE_AUDIO_CONVERSION
    dmaConfig.dataLen       = I2S_DMA_BUF_LEN*SYS_FS_RATIO*4; // frame buffer size in bytes
#else
    dmaConfig.dataLen       = I2S_DMA_BUF_LEN*4; // frame buffer size in bytes
#endif

#if (defined(CHIP_C5517))
    dmaConfig.srcAddr      = (Uint32)&CSL_I2S3_REGS->I2SRXLTL;
#else
    dmaConfig.srcAddr      = (Uint32)&CSL_I2S3_REGS->I2SRXLT0;
#endif

#ifdef ENABLE_AUDIO_CONVERSION
    dmaConfig.destAddr       = (Uint32)i2sDmaReadBufLeft2In;
#else
    dmaConfig.destAddr       = (Uint32)i2sDmaReadBufLeft2;
#endif
    /* Open DMA ch8 for I2S3 left channel read */
    dmaLeftRxHandle2 = DMA_open(DMA_CHAN_RX_L2, &dmaObj4, &status);
    if (dmaLeftRxHandle2 == NULL)
    {
        printf("DMA_open CH8 Failed \n");
        dmaLeftRxHandle2 = NULL;
    }

    /* Configure DMA channel8 */
    status = DMA_config(dmaLeftRxHandle2, &dmaConfig);
    if (status != CSL_SOK)
    {
        printf("DMA_config CH8 Failed \n");
        dmaLeftRxHandle = NULL;
    }

    /* Configure DMA ch9 for I2S3 right channel read */
    dmaConfig.pingPongMode = CSL_DMA_PING_PONG_ENABLE;
    dmaConfig.autoMode     = CSL_DMA_AUTORELOAD_ENABLE;
    dmaConfig.burstLen     = CSL_DMA_TXBURST_1WORD;
    dmaConfig.trigger      = CSL_DMA_EVENT_TRIGGER;
    dmaConfig.dmaEvt        = CSL_DMA_EVT_I2S3_RX;
    dmaConfig.dmaInt       = CSL_DMA_INTERRUPT_ENABLE;
    dmaConfig.chanDir      = CSL_DMA_READ;
    dmaConfig.trfType      = CSL_DMA_TRANSFER_IO_MEMORY;
#ifdef ENABLE_AUDIO_CONVERSION
    dmaConfig.dataLen      = I2S_DMA_BUF_LEN*SYS_FS_RATIO*4; // frame buffer size in bytes
#else
    dmaConfig.dataLen      = I2S_DMA_BUF_LEN*4; // frame buffer size in bytes
#endif

#if (defined(CHIP_C5517))
    dmaConfig.srcAddr     = (Uint32)&CSL_I2S3_REGS->I2SRXRTL;
#else
    dmaConfig.srcAddr     = (Uint32)&CSL_I2S3_REGS->I2SRXRT0;
#endif

#ifdef ENABLE_AUDIO_CONVERSION
    dmaConfig.destAddr      = (Uint32)i2sDmaReadBufRight2In;
#else
    dmaConfig.destAddr      = (Uint32)i2sDmaReadBufRight2;
#endif

    /* Open DMA ch9 for I2S3 right channel read */
    dmaRightRxHandle2 = DMA_open(DMA_CHAN_RX_R2, &dmaObj5, &status);
    if (dmaRightRxHandle2 == NULL)
    {
        printf("DMA_open CH9 Failed \n");
        dmaRightRxHandle2 = NULL;
    }

    /* Configure DMA channel */
    status = DMA_config(dmaRightRxHandle2, &dmaConfig);
    if (status != CSL_SOK)
    {
        printf("DMA_config CH9 Failed \n");
        dmaRightRxHandle2 = NULL;
    }
#endif

#ifdef INSTANCE0_I2S
    /* Configure DMA channel 8 for I2S0 left channel read */
    dmaConfig.pingPongMode  = CSL_DMA_PING_PONG_ENABLE;
    dmaConfig.autoMode      = CSL_DMA_AUTORELOAD_ENABLE;
    dmaConfig.burstLen      = CSL_DMA_TXBURST_1WORD;
    dmaConfig.trigger       = CSL_DMA_EVENT_TRIGGER;
    dmaConfig.dmaEvt        = CSL_DMA_EVT_I2S0_RX;
    dmaConfig.dmaInt        = CSL_DMA_INTERRUPT_ENABLE;
    dmaConfig.chanDir       = CSL_DMA_READ;
    dmaConfig.trfType       = CSL_DMA_TRANSFER_IO_MEMORY;
#ifdef ENABLE_AUDIO_CONVERSION
    dmaConfig.dataLen       = I2S_DMA_BUF_LEN*SYS_FS_RATIO*4; // frame buffer size in bytes
#else
    dmaConfig.dataLen       = I2S_DMA_BUF_LEN*4; // frame buffer size in bytes
#endif

#if (defined(CHIP_C5517))
    dmaConfig.srcAddr      = (Uint32)&CSL_I2S0_REGS->I2SRXLTL;
#else
    dmaConfig.srcAddr      = (Uint32)&CSL_I2S0_REGS->I2SRXLT0;
#endif

#ifdef ENABLE_AUDIO_CONVERSION
    dmaConfig.destAddr       = (Uint32)i2sDmaReadBufLeft3In;
#else
    dmaConfig.destAddr       = (Uint32)i2sDmaReadBufLeft3;
#endif

    /* Open DMA ch0 for I2S0 left channel read */
    dmaLeftRxHandle3 = DMA_open(DMA_CHAN_RX_L3, &dmaObj6, &status);
    if (dmaLeftRxHandle3 == NULL)
    {
        printf("DMA_open CH0 Failed \n");
        dmaLeftRxHandle3 = NULL;
    }

    /* Configure DMA channel0 */
    status = DMA_config(dmaLeftRxHandle3, &dmaConfig);
    if (status != CSL_SOK)
    {
        printf("DMA_config CH0 Failed \n");
        dmaLeftRxHandle3 = NULL;
    }

    /* Configure DMA ch1 for I2S0 right channel read */
    dmaConfig.pingPongMode = CSL_DMA_PING_PONG_ENABLE;
    dmaConfig.autoMode     = CSL_DMA_AUTORELOAD_ENABLE;
    dmaConfig.burstLen     = CSL_DMA_TXBURST_1WORD;
    dmaConfig.trigger      = CSL_DMA_EVENT_TRIGGER;
    dmaConfig.dmaEvt        = CSL_DMA_EVT_I2S0_RX;
    dmaConfig.dmaInt       = CSL_DMA_INTERRUPT_ENABLE;
    dmaConfig.chanDir      = CSL_DMA_READ;
    dmaConfig.trfType      = CSL_DMA_TRANSFER_IO_MEMORY;
#ifdef ENABLE_AUDIO_CONVERSION
    dmaConfig.dataLen      = I2S_DMA_BUF_LEN*SYS_FS_RATIO*4; // frame buffer size in bytes
#else
    dmaConfig.dataLen      = I2S_DMA_BUF_LEN*4; // frame buffer size in bytes
#endif

#if (defined(CHIP_C5517))
    dmaConfig.srcAddr     = (Uint32)&CSL_I2S0_REGS->I2SRXRTL;
#else
    dmaConfig.srcAddr     = (Uint32)&CSL_I2S0_REGS->I2SRXRT0;
#endif

#ifdef ENABLE_AUDIO_CONVERSION
    dmaConfig.destAddr      = (Uint32)i2sDmaReadBufRight3In;
#else
    dmaConfig.destAddr      = (Uint32)i2sDmaReadBufRight3;
#endif

    /* Open DMA ch9 for I2S0 right channel read */
    dmaRightRxHandle3 = DMA_open(DMA_CHAN_RX_R3, &dmaObj7, &status);
    if (dmaRightRxHandle3 == NULL)
    {
        printf("DMA_open CH1 Failed \n");
        dmaRightRxHandle3 = NULL;
    }

    /* Configure DMA channel */
    status = DMA_config(dmaRightRxHandle3, &dmaConfig);
    if (status != CSL_SOK)
    {
        printf("DMA_config CH1 Failed \n");
        dmaRightRxHandle3 = NULL;
    }
#endif

    return CSL_SOK;
}

// last DMA transfer type
Bool lastDmaType;

/* User defined algorithm */
void UserAlgorithm(void)
{
    Uint16 i, j, offset;
    CSL_Status status = 0;
	short *tempPtrSrcL, *tempPtrDstL, *tempPtrSrcR, *tempPtrDstR;
	linSample *tempPtr;

    if (dmaRxFrameCount >= NUM_OF_MICS)
    {
    	dmaRxFrameCount=0;

    	// get last DMA transfer type
    	lastDmaType = DMA_getLastTransferType(dmaLeftRxHandle, &status);
    	if (status != CSL_SOK)
    	{
    		printf("I2S Dma Left Rx Get Type Failed!!\n");
    		exit(EXIT_FAILURE);
    	}

    	// set offset for data copy
    	if (lastDmaType==0)
    		offset = 0;		// data is in Ping buffer
    	else
#ifdef ENABLE_AUDIO_CONVERSION
    		offset = I2S_DMA_BUF_LEN*SYS_FS_RATIO/2;   // data is in Pong buffer
#else
    		offset = I2S_DMA_BUF_LEN/2;   // data is in Pong buffer
#endif

        /* --- Insert algorithm here --- */
#ifdef LOOPBACK_ONLY
#ifdef ENABLE_AUDIO_CONVERSION
    	for (i=0; i<I2S_DMA_BUF_LEN*SYS_FS_RATIO/2; i++)
#else
    	for (i=0; i<I2S_DMA_BUF_LEN/2; i++)
#endif //ENABLE_AUDIO_CONVERSION
        {
            // i2sDmaReadBufLeft (I2S2 left) - mic1 */
            // i2sDmaReadBufRight (I2S2 right) - mic2 */
            // i2sDmaReadBufLeft2 (I2S3 left) - mic4 */
            // i2sDmaReadBufRight2 (I2S3 right) - mic3 */
            // i2sDmaReadBufLeft3 (I2S0 left) - mic5 */
            // i2sDmaReadBufRight3 (I2S0 right) - mic6 */
#ifdef INSTANCE0_I2S
#ifdef ENABLE_AUDIO_CONVERSION
            ///i2sDmaWriteBufLeftOut[offset+i]  = i2sDmaReadBufLeftIn[offset+i];
            ///i2sDmaWriteBufRightOut[offset+i] = i2sDmaReadBufRightIn[offset+i];
#else
            ///i2sDmaWriteBufLeft[offset+i]  = i2sDmaReadBufLeft[offset+i];
            ///i2sDmaWriteBufRight[offset+i] = i2sDmaReadBufRight[offset+i];
#endif //ENABLE_AUDIO_CONVERSION

    		// loopback mic3 and mic4
#ifdef ENABLE_AUDIO_CONVERSION
            ///i2sDmaWriteBufLeftOut[offset+i]  = i2sDmaReadBufLeft2In[offset+i];
            ///i2sDmaWriteBufRightOut[offset+i] = i2sDmaReadBufRight2In[offset+i];
#else
            ///i2sDmaWriteBufLeft[offset+i]  = i2sDmaReadBufLeft2[offset+i];
            ///i2sDmaWriteBufRight[offset+i] = i2sDmaReadBufRight2[offset+i];
#endif //ENABLE_AUDIO_CONVERSION

    		// loopback mic5 and mic6
#ifdef ENABLE_AUDIO_CONVERSION
            i2sDmaWriteBufLeftOut[offset+i]  = i2sDmaReadBufLeft3In[offset+i];
            i2sDmaWriteBufRightOut[offset+i] = i2sDmaReadBufRight3In[offset+i];
#else
            i2sDmaWriteBufLeft[offset+i]  = i2sDmaReadBufLeft3[offset+i];
            i2sDmaWriteBufRight[offset+i] = i2sDmaReadBufRight3[offset+i];
#endif //ENABLE_AUDIO_CONVERSION
#else
#ifdef INSTANCE3_I2S
    		// loopback mic1 and mic2
#ifdef ENABLE_AUDIO_CONVERSION
            ///i2sDmaWriteBufLeftOut[offset+i]  = i2sDmaReadBufLeftIn[offset+i];
            ///i2sDmaWriteBufRightOut[offset+i] = i2sDmaReadBufRightIn[offset+i];
#else
            ///i2sDmaWriteBufLeft[offset+i]  = i2sDmaReadBufLeft[offset+i];
            ///i2sDmaWriteBufRight[offset+i] = i2sDmaReadBufRight[offset+i];
#endif //ENABLE_AUDIO_CONVERSION

    		// loopback mic3 and mic4
#ifdef ENABLE_AUDIO_CONVERSION
            i2sDmaWriteBufLeftOut[offset+i]  = i2sDmaReadBufLeft2In[offset+i];
            i2sDmaWriteBufRightOut[offset+i] = i2sDmaReadBufRight2In[offset+i];
#else
            i2sDmaWriteBufLeft[offset+i]  = i2sDmaReadBufLeft2[offset+i];
            i2sDmaWriteBufRight[offset+i] = i2sDmaReadBufRight2[offset+i];
#endif //ENABLE_AUDIO_CONVERSION
#else
    		// loopback mic1 and mic2
#ifdef ENABLE_AUDIO_CONVERSION
            i2sDmaWriteBufLeftOut[offset+i]  = i2sDmaReadBufLeftIn[offset+i];
            i2sDmaWriteBufRightOut[offset+i] = i2sDmaReadBufRightIn[offset+i];
#else
            i2sDmaWriteBufLeft[offset+i]  = i2sDmaReadBufLeft[offset+i];
            i2sDmaWriteBufRight[offset+i] = i2sDmaReadBufRight[offset+i];
#endif //ENABLE_AUDIO_CONVERSION

#endif //INSTANCE3_I2S
#endif //INSTANCE2_I2S
        }
#else	// BF+ASNR+MSS signal processing
        // i2sDmaReadBufLeft (I2S2 left) - mic1 */
        // i2sDmaReadBufRight (I2S2 right) - mic2 */
        // i2sDmaReadBufLeft2 (I2S3 left) - mic4 */
        // i2sDmaReadBufRight2 (I2S3 right) - mic3 */
        // i2sDmaReadBufLeft3 (I2S0 left) - mic5 */
        // i2sDmaReadBufRight3 (I2S0 right) - mic6 */

#ifdef ENABLE_AUDIO_CONVERSION
#ifdef PROFILE_CYCLES
        statusInt = GPT_getCnt(hGpt, &timeCntInt1);;
#endif //PROFILE_CYCLES
    	// down sample the left channel
    	// copy the input frame into the delay buffer
    	tempPtr = (linSample *)&i2sDmaReadBufLeftIn[offset];
    	for (i=0; i<I2S_DMA_BUF_LEN*SYS_FS_RATIO; i++)
    	{
    		i2sDmaReadBufLeftInDelay[sfc_NUM_DEF_COEFS+i] = tempPtr[i];
    	}
    	// downsample from 48Khz to 16 Khz with low pass filter
    	sfcDecimationBy3((linSample *)&i2sDmaReadBufLeftInDelay[0],
    			(linSample *)&i2sDmaReadBufLeft[offset],
				sfc_def_decim_interp_filt,
				sfc_NUM_DEF_COEFS,
				I2S_DMA_BUF_LEN*SYS_FS_RATIO);
    	// copy the last sfc_NUM_DEF_COEFS-1 samples of input frame into the delay buffer
    	tempPtr = (linSample *)&i2sDmaReadBufLeftIn[offset];
    	for (i=0; i<sfc_NUM_DEF_COEFS; i++)
    	{
    		i2sDmaReadBufLeftInDelay[i] = tempPtr[(I2S_DMA_BUF_LEN*SYS_FS_RATIO-(sfc_NUM_DEF_COEFS))+i];
    	}

    	// down sample the right channel
    	// copy the input frame into the delay buffer
    	tempPtr = (linSample *)&i2sDmaReadBufRightIn[offset];
    	for (i=0; i<I2S_DMA_BUF_LEN*SYS_FS_RATIO; i++)
    	{
    		i2sDmaReadBufRightInDelay[sfc_NUM_DEF_COEFS+i] = tempPtr[i];
    	}
    	// downsample from 48Khz to 16 Khz with low pass filter
    	sfcDecimationBy3((linSample *)&i2sDmaReadBufRightInDelay[0],
    			(linSample *)&i2sDmaReadBufRight[offset],
				sfc_def_decim_interp_filt,
				sfc_NUM_DEF_COEFS,
				I2S_DMA_BUF_LEN*SYS_FS_RATIO);
    	// copy the last sfc_NUM_DEF_COEFS samples of input frame into the delay buffer
    	tempPtr = (linSample *)&i2sDmaReadBufRightIn[offset];
    	for (i=0; i<sfc_NUM_DEF_COEFS; i++)
    	{
    		i2sDmaReadBufRightInDelay[i] = tempPtr[(I2S_DMA_BUF_LEN*SYS_FS_RATIO-(sfc_NUM_DEF_COEFS))+i];
    	}
#endif //ENABLE_AUDIO_CONVERSION
    	// set up the input frame pointers
    	// set up the input frame pointers
    	inFramePtr[0] = (int *)(&i2sDmaReadBufLeft[offset]);
    	inFramePtr[1] = (int *)(&i2sDmaReadBufRight[offset]);

#ifdef INSTANCE3_I2S
#ifdef ENABLE_AUDIO_CONVERSION
    	// down sample the left channel
    	// copy the input frame into the delay buffer
    	tempPtr = (linSample *)&i2sDmaReadBufLeft2In[offset];
    	for (i=0; i<I2S_DMA_BUF_LEN*SYS_FS_RATIO; i++)
    	{
    		i2sDmaReadBufLeft2InDelay[sfc_NUM_DEF_COEFS+i] = tempPtr[i];
    	}
    	// downsample from 48Khz to 16 Khz with low pass filter
    	sfcDecimationBy3((linSample *)&i2sDmaReadBufLeft2InDelay[0],
    			(linSample *)&i2sDmaReadBufLeft2[offset],
				sfc_def_decim_interp_filt,
				sfc_NUM_DEF_COEFS,
				I2S_DMA_BUF_LEN*SYS_FS_RATIO);
    	// copy the last sfc_NUM_DEF_COEFS samples of input frame into the delay buffer
    	tempPtr = (linSample *)&i2sDmaReadBufLeft2In[offset];
    	for (i=0; i<sfc_NUM_DEF_COEFS; i++)
    	{
    		i2sDmaReadBufLeft2InDelay[i] = tempPtr[(I2S_DMA_BUF_LEN*SYS_FS_RATIO-(sfc_NUM_DEF_COEFS))+i];
    	}

    	// down sample the right channel
    	// copy the input frame into the delay buffer
    	tempPtr = (linSample *)&i2sDmaReadBufRight2In[offset];
    	for (i=0; i<I2S_DMA_BUF_LEN*SYS_FS_RATIO; i++)
    	{
    		i2sDmaReadBufRight2InDelay[sfc_NUM_DEF_COEFS+i] = tempPtr[i];
    	}
    	// downsample from 48Khz to 16 Khz with low pass filter
    	sfcDecimationBy3((linSample *)&i2sDmaReadBufRight2InDelay[0],
    			(linSample *)&i2sDmaReadBufRight2[offset],
				sfc_def_decim_interp_filt,
				sfc_NUM_DEF_COEFS,
				I2S_DMA_BUF_LEN*SYS_FS_RATIO);
    	// copy the last sfc_NUM_DEF_COEFS samples of input frame into the delay buffer
    	tempPtr = (linSample *)&i2sDmaReadBufRight2In[offset];
    	for (i=0; i<sfc_NUM_DEF_COEFS; i++)
    	{
    		i2sDmaReadBufRight2InDelay[i] = tempPtr[(I2S_DMA_BUF_LEN*SYS_FS_RATIO-(sfc_NUM_DEF_COEFS))+i];
    	}
#endif //ENABLE_AUDIO_CONVERSION
    	inFramePtr[2] = (int *)(&i2sDmaReadBufRight2[offset]);
    	inFramePtr[3] = (int *)(&i2sDmaReadBufLeft2[offset]);
#endif //INSTANCE3_I2S
#ifdef INSTANCE0_I2S
#ifdef ENABLE_AUDIO_CONVERSION
    	// downsample from 48Khz to 16 Khz by taking 1 sample out of 3
    	tempPtrSrcL = (short *)&i2sDmaReadBufLeft3In[offset];
    	tempPtrDstL = (short *)&i2sDmaReadBufLeft3[offset];
    	tempPtrSrcR = (short *)&i2sDmaReadBufRight3In[offset];
    	tempPtrDstR = (short *)&i2sDmaReadBufRight3[offset];
    	for (i=0; i<I2S_DMA_BUF_LEN; i++)
    	{
    		*tempPtrDstL = *tempPtrSrcL;
    		*tempPtrDstR = *tempPtrSrcR;
    		tempPtrDstL++;
    		tempPtrDstR++;
    		tempPtrSrcL += SYS_FS_RATIO;
    		tempPtrSrcR += SYS_FS_RATIO;
    	}
#endif //ENABLE_AUDIO_CONVERSION
    	inFramePtr[4] = (int *)(&i2sDmaReadBufLeft3[offset]);
    	inFramePtr[5] = (int *)(&i2sDmaReadBufRight3[offset]);
#endif //INSTANCE0_I2S
#ifdef PROFILE_CYCLES
    	statusInt = GPT_getCnt(hGpt, &timeCntInt2);
    	process_cycles((timeCntInt1-timeCntInt2)<<(hwConfig.preScaleDiv+1), &cyclesDownSamp);
#endif
    	// set up the processed audio output to left ear
    	buf_Outptr = (int *)(&i2sDmaWriteBufLeft[offset]);
    	// set up the processed audio output to right ear
    	buf_Outptr2 = (int *)(&i2sDmaWriteBufRight[offset]);

        // process next frame
#if 1
    	bf_process();
#else
        for (i=0;i<I2S_DMA_BUF_LEN;i++){
          buf_Outptr[i] = inFramePtr[0][i];
          buf_Outptr2[i] = inFramePtr[1][i];
        }
#endif

#ifdef ENABLE_AUDIO_CONVERSION
#ifdef PROFILE_CYCLES
        statusInt = GPT_getCnt(hGpt, &timeCntInt1);;
#endif //PROFILE_CYCLES
    	// up sample sample the processed channel
    	// copy the processed frame into the delay buffer
    	tempPtr = (linSample *)&i2sDmaWriteBufLeft[offset];
    	for (i=0; i<I2S_DMA_BUF_LEN; i++)
    	{
    		i2sDmaWriteBufLeftOutDelay[(sfc_NUM_DEF_COEFS)/SYS_FS_RATIO+i] =
    				tempPtr[i];
    	}
    	// upsample from 16Khz to 48 Khz with low pass filter
    	sfcInterpolationBy3((linSample *)&i2sDmaWriteBufLeftOutDelay[0],
    			(linSample *)&i2sDmaWriteBufLeftOut[offset],
				sfc_def_decim_interp_filt,
				sfc_NUM_DEF_COEFS, I2S_DMA_BUF_LEN);
    	// copy the last (sfc_NUM_DEF_COEFS)/SYS_FS_RATIO samples of processed frame into the delay buffer
    	tempPtr = (linSample *)&i2sDmaWriteBufLeft[offset];
    	for (i=0; i<(sfc_NUM_DEF_COEFS)/SYS_FS_RATIO; i++)
    	{
    		i2sDmaWriteBufLeftOutDelay[i] = tempPtr[(I2S_DMA_BUF_LEN-(sfc_NUM_DEF_COEFS)/SYS_FS_RATIO)+i];
    	}

    	// up sample sample the un-processed channel
    	// copy the un-processed frame into the delay buffer
    	tempPtr = (linSample *)&i2sDmaWriteBufRight[offset];
    	for (i=0; i<I2S_DMA_BUF_LEN; i++)
    	{
    		i2sDmaWriteBufRightOutDelay[(sfc_NUM_DEF_COEFS)/SYS_FS_RATIO+i] =
    				tempPtr[i];
    	}
    	// upsample from 16Khz to 48 Khz with low pass filter
    	sfcInterpolationBy3((linSample *)&i2sDmaWriteBufRightOutDelay[0],
    			(linSample *)&i2sDmaWriteBufRightOut[offset],
				sfc_def_decim_interp_filt,
				sfc_NUM_DEF_COEFS, I2S_DMA_BUF_LEN);
    	// copy the last (sfc_NUM_DEF_COEFS)/SYS_FS_RATIO samples of un-processed frame into the delay buffer
    	tempPtr = (linSample *)&i2sDmaWriteBufRight[offset];
    	for (i=0; i<(sfc_NUM_DEF_COEFS)/SYS_FS_RATIO; i++)
    	{
    		i2sDmaWriteBufRightOutDelay[i] = tempPtr[(I2S_DMA_BUF_LEN-(sfc_NUM_DEF_COEFS)/SYS_FS_RATIO)+i];
    	}
#ifdef PROFILE_CYCLES
    	statusInt = GPT_getCnt(hGpt, &timeCntInt2);
    	process_cycles((timeCntInt1-timeCntInt2)<<(hwConfig.preScaleDiv+1), &cyclesUpSamp);
#endif //PROFILE_CYCLES
#endif //ENABLE_AUDIO_CONVERSION
#endif //LOOPBACK_ONLY
    }

    /* Clear DMA frame count */
    if(dmaTxFrameCount>=NUM_OF_OUTPUT_CH)
    {
    	dmaTxFrameCount = 0;
    }

}

// Put CPU in idle
void UserIdle(void)
{
    /* Set CPUI bit in ICR */
    //*(volatile ioport Uint16 *)(0x0001) |= 0x000F;
    /* Set CPUI, DPORTI, XPORTI, and IPORTI in ICR */
    //*(volatile ioport Uint16 *)(0x0001) |= 0x016F;

    /* Execute idle instruction */
    asm("    idle");
}

volatile Uint32 dmaIntCount = 0;
volatile Uint32 dmaSemCount = 0;

// DMA ISR
interrupt void DmaIsr(void)
{
    volatile Uint16 ifrValue;

#ifdef PROFILE_CYCLES
      statusInt = GPT_getCnt(hGpt, &timeCntInt1);;
#endif
    /* Clear the DMA interrupt */
    ifrValue = CSL_SYSCTRL_REGS->DMAIFR;
    CSL_SYSCTRL_REGS->DMAIFR |= ifrValue;

    /* Check for DMA DMA_CHAN_TX_L transfer completion */
    if (ifrValue & (1<<DMA_CHAN_TX_L))
    {
        dmaTxFrameCount++;
    }

    /* Check for DMA DMA_CHAN_TX_R transfer completion */
    if (ifrValue & (1<<DMA_CHAN_TX_R))
    {
        dmaTxFrameCount++;
    }

    /* Check for DMA DMA_CHAN_RX_L transfer completion */
    if (ifrValue & (1<<DMA_CHAN_RX_L))
    {
        dmaRxFrameCount++;
    }

    /* Check for DMA DMA_CHAN_RX_R transfer completion */
    if (ifrValue & (1<<DMA_CHAN_RX_R))
    {
        dmaRxFrameCount++;
    }

#ifdef INSTANCE3_I2S
    /* Check for DMA DMA_CHAN_RX_L2 transfer completion */
    if (ifrValue & (1<<DMA_CHAN_RX_L2))
    {
        dmaRxFrameCount++;
    }

    /* Check for DMA DMA_CHAN_RX_R2 transfer completion */
    if (ifrValue & (1<<DMA_CHAN_RX_R2))
    {
        dmaRxFrameCount++;
    }
#endif

#ifdef INSTANCE0_I2S
    /* Check for DMA DMA_CHAN_RX_L3 transfer completion */
    if (ifrValue & (1<<DMA_CHAN_RX_L3))
    {
        dmaRxFrameCount++;
    }

    /* Check for DMA DMA_CHAN_RX_R3 transfer completion */
    if (ifrValue & (1<<DMA_CHAN_RX_R3))
    {
        dmaRxFrameCount++;
    }
#endif

    // wait up the audio task when both RX and TX DMAs are done
    if ((dmaRxFrameCount>=NUM_OF_MICS)&&(dmaTxFrameCount>=NUM_OF_OUTPUT_CH))
    {
    	SEM_post(&SEM_i2s_dmaTransferDone);
    	dmaSemCount++;
    }

    dmaIntCount++;
#ifdef PROFILE_CYCLES
	  statusInt = GPT_getCnt(hGpt, &timeCntInt2);
      process_cycles((timeCntInt1-timeCntInt2)<<(hwConfig.preScaleDiv+1), &cyclesDmaInt);
#endif
}
