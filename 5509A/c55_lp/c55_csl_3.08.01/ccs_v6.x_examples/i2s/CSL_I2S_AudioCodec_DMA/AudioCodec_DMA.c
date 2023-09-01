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
 *   Open the project "CSL_I2S_AudioCodec_DMA.pjt" and build it
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

#include "stdio.h"
#include "stdlib.h"

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
#include "IdleLoop.h"

//INSTANCE0/2_I2S defined in codec_aic3254.h

#ifdef INSTANCE0_I2S
#define I2S_INSTANCE    ( I2S_INSTANCE0 )
#define DMA_CHAN_TX_L      ( CSL_DMA_CHAN0 )
#define DMA_CHAN_TX_R      ( CSL_DMA_CHAN1 )
#define DMA_CHAN_RX_L      ( CSL_DMA_CHAN2 )
#define DMA_CHAN_RX_R      ( CSL_DMA_CHAN3 )

#define I2S0_SRGR          *(volatile ioport Uint16*)(0x2804)
#define I2S0_ICMR          *(volatile ioport Uint16*)(0x2814)
#define I2S0_CR            *(volatile ioport Uint16*)(0x2800)

#endif

#ifdef INSTANCE2_I2S
#define I2S_INSTANCE    ( I2S_INSTANCE2 )
#define DMA_CHAN_TX_L      ( CSL_DMA_CHAN4 )
#define DMA_CHAN_TX_R      ( CSL_DMA_CHAN5 )
#define DMA_CHAN_RX_L      ( CSL_DMA_CHAN6 )
#define DMA_CHAN_RX_R      ( CSL_DMA_CHAN7 )

#define I2S2_SRGR          *(volatile ioport Uint16*)(0x2A04)
#define I2S2_ICMR          *(volatile ioport Uint16*)(0x2A14)
#define I2S2_CR            *(volatile ioport Uint16*)(0x2A00)

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

/* Ping/pong buffers */
#pragma DATA_ALIGN(i2sDmaWriteBufLeft, 8);
Uint32 i2sDmaWriteBufLeft[I2S_DMA_BUF_LEN];  // left Tx write ping/pong buffer
#pragma DATA_ALIGN(i2sDmaWriteBufRight, 8);
Uint32 i2sDmaWriteBufRight[I2S_DMA_BUF_LEN]; // right Tx write ping/pong buffe
#pragma DATA_ALIGN(i2sDmaReadBufLeft, 8);
Uint32 i2sDmaReadBufLeft[I2S_DMA_BUF_LEN];  // left Rx read ping/pong buffer
#pragma DATA_ALIGN(i2sDmaReadBufRight, 8);
Uint32 i2sDmaReadBufRight[I2S_DMA_BUF_LEN]; // right Rx read ping/pong buffer
Uint16 RdBuf2WrBufTxfrDone=0;

/* Input buffer left */
#pragma DATA_SECTION(inputDataBufLeft, ".inputDataBufLeft");
Int16 inputDataBufLeft[FRAME_LEN];
#pragma DATA_SECTION(inputDataBufRight, ".inputDataBufRight");
/* Input buffer right */
Int16 inputDataBufRight[FRAME_LEN];

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
interrupt void DmaIsr(void);

CSL_Status ioExpander_Setup(void);
CSL_Status ioExpander_Read(Uint16 port, Uint16 pin, Uint16 *rValue);
CSL_Status ioExpander_Write(Uint16 port, Uint16 pin, Uint16 value);

void main(void)
{
    CSL_Status status;

	CSL_SYSCTRL_REGS->PCGCR1 = 0x0000;
	CSL_SYSCTRL_REGS->PCGCR2 = 0x0000;

    printf("I2S Idle Loop Audio Playback Example\n");
    printf("This Example will continuously Pump PC lineout to the ");
    printf("Audio Output using the Codec\n\n");

    printf("Please connect the Headphone to the Audio Output (Headphone) ");
    printf("Jack\n\n");

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


#ifdef INSTANCE0_I2S
    status = ioExpander_Write(0, 2, 0);// SEL_I2C_S0 = 0
    status = ioExpander_Write(0, 3, 0);// SEL_I2C_S1 = 0
    /* Set AIC_MCBSP_MODE to I2S (disable McBSP) */
    status = ioExpander_Write(0, 1, 1);// AIC_MCBSP_MODE = 0

    status = ioExpander_Write(1, 0, 1);  /* Enable SEL_MMC0_I2S */
    /* Release AIC3204 reset */
    status = ioExpander_Write(0, 0, 0);// AIC_RST = 0
	if (status != CSL_SOK)
    {
        printf("ERROR: ioExpander_Write Failed\n");
        exit(EXIT_FAILURE);
    }
#else

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
    status = AIC3254_init(48000, 48000, PLL_MHZ);
    if (status != CSL_SOK)
    {
        printf("ERROR: Unable to initialize AIC3204\n");
        exit(1);
    }



    /* Set CPUI bit in ICR */
    //*(volatile ioport Uint16 *)(0x0001) |= 0x000F;
    /* Set CPUI, DPORTI, XPORTI, and IPORTI in ICR */
    *(volatile ioport Uint16 *)(0x0001) |= 0x016F;

    /* Enable global interrupt */
    IRQ_globalEnable();

    /* Enable I2S */
#ifdef INSTANCE0_I2S
    CSL_I2S0_REGS->I2SSCTRL |= 0x8000;
#else
    CSL_I2S2_REGS->I2SSCTRL |= 0x8000;
#endif
    printf("Pumping PC lineout to the HP Output Started!!\n");

    while (1)
    {
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
    /* Open the device with instance 0/2 */
    /*(AIC3204-1 is connected to I2S0 on C5517 EVM) */
    /*(AIC3204-2 is connected to I2S2 on C5517 EVM) */
    hI2s = I2S_open(I2S_INSTANCE, DMA_POLLED, I2S_CHAN_STEREO);
    if(NULL == hI2s)
    {
        status = CSL_ESYS_FAIL;
        return (status);
    }
    else
    {
        printf ("I2S Module Instance opened successfully\n");
    }


    if (I2S_INSTANCE0 == I2S_INSTANCE)
    {
    	status = SYS_setEBSR(CSL_EBSR_FIELD_SP0MODE,
							 CSL_EBSR_SP0MODE_1);
    }
	else
	{
		status = SYS_setEBSR(CSL_EBSR_FIELD_PPMODE,
							 CSL_EBSR_PPMODE_6);
	}

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
    hwConfig.fsPol          = I2S_FSPOL_LOW;
    hwConfig.clkPol         = I2S_RISING_EDGE;//I2S_FALLING_EDGE;
    hwConfig.datadelay      = I2S_DATADELAY_ONEBIT;
    hwConfig.datapack       = I2S_DATAPACK_DISABLE;
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
        printf ("I2S Module Configured successfully\n");
    }

    /* Configure DMA channel 0 for I2S left channel write*/
    dmaConfig.pingPongMode  = CSL_DMA_PING_PONG_DISABLE;
    dmaConfig.autoMode      = CSL_DMA_AUTORELOAD_ENABLE;
    dmaConfig.burstLen      = CSL_DMA_TXBURST_1WORD;
    dmaConfig.trigger       = CSL_DMA_EVENT_TRIGGER;
#ifdef INSTANCE0_I2S
    dmaConfig.dmaEvt        = CSL_DMA_EVT_I2S0_TX;
#endif
#ifdef INSTANCE2_I2S
    dmaConfig.dmaEvt        = CSL_DMA_EVT_I2S2_TX;
#endif

    dmaConfig.dmaInt        = CSL_DMA_INTERRUPT_ENABLE;
    dmaConfig.chanDir       = CSL_DMA_WRITE;
    dmaConfig.trfType       = CSL_DMA_TRANSFER_IO_MEMORY;
    dmaConfig.dataLen       = NUM_SAMP_PER_MS*NUM_MS_PER_FRAME*4*2; // two frames

#ifdef INSTANCE0_I2S
#if (defined(CHIP_C5517))
    dmaConfig.destAddr      = (Uint32)&CSL_I2S0_REGS->I2STXLTL;
#else
    dmaConfig.destAddr      = (Uint32)&CSL_I2S0_REGS->I2STXLT0;
#endif
#endif
#ifdef INSTANCE2_I2S
#if (defined(CHIP_C5517))
    dmaConfig.destAddr      = (Uint32)&CSL_I2S2_REGS->I2STXLTL;
#else
    dmaConfig.destAddr      = (Uint32)&CSL_I2S2_REGS->I2STXLT0;
#endif
#endif
    dmaConfig.srcAddr       = (Uint32)i2sDmaWriteBufLeft;

    /* Open DMA ch0 for I2S left channel write*/
    dmaLeftTxHandle = DMA_open(DMA_CHAN_TX_L, &dmaObj0,&status);
    if (dmaLeftTxHandle == NULL)
    {
        printf("DMA_open CH0 Failed \n");
        dmaLeftTxHandle = NULL;
    }

    /* Configure DMA channe0 */
    status = DMA_config(dmaLeftTxHandle, &dmaConfig);
    if (status != CSL_SOK)
    {
        printf("DMA_config CH0 Failed \n");
        dmaLeftTxHandle = NULL;
    }

    /* Configure DMA ch1 for I2S right channel write*/
    dmaConfig.pingPongMode = CSL_DMA_PING_PONG_DISABLE;
    dmaConfig.autoMode     = CSL_DMA_AUTORELOAD_ENABLE;
    dmaConfig.burstLen     = CSL_DMA_TXBURST_1WORD;
    dmaConfig.trigger      = CSL_DMA_EVENT_TRIGGER;
#ifdef INSTANCE0_I2S
    dmaConfig.dmaEvt        = CSL_DMA_EVT_I2S0_TX;
#endif
#ifdef INSTANCE2_I2S
    dmaConfig.dmaEvt        = CSL_DMA_EVT_I2S2_TX;
#endif
    dmaConfig.dmaInt       = CSL_DMA_INTERRUPT_ENABLE;
    dmaConfig.chanDir      = CSL_DMA_WRITE;
    dmaConfig.trfType      = CSL_DMA_TRANSFER_IO_MEMORY;
    dmaConfig.dataLen      = NUM_SAMP_PER_MS*NUM_MS_PER_FRAME*4*2;  // two frames

#ifdef INSTANCE0_I2S
#if (defined(CHIP_C5517))
    dmaConfig.destAddr     = (Uint32)&CSL_I2S0_REGS->I2STXRTL;
#else
    dmaConfig.destAddr     = (Uint32)&CSL_I2S0_REGS->I2STXRT0;
#endif
#endif
#ifdef INSTANCE2_I2S
#if (defined(CHIP_C5517))
    dmaConfig.destAddr     = (Uint32)&CSL_I2S2_REGS->I2STXRTL;
#else
    dmaConfig.destAddr     = (Uint32)&CSL_I2S2_REGS->I2STXRT0;
#endif
#endif


    dmaConfig.srcAddr      = (Uint32)i2sDmaWriteBufRight;

    /* Open DMA ch1 for I2S right channel write*/
    dmaRightTxHandle = DMA_open(DMA_CHAN_TX_R, &dmaObj1,&status);
    if (dmaRightTxHandle == NULL)
    {
        printf("DMA_open CH1 Failed \n");
        dmaRightTxHandle = NULL;
    }

    /* Configure DMA channel1*/
    status = DMA_config(dmaRightTxHandle, &dmaConfig);
    if (status != CSL_SOK)
    {
        printf("DMA_config CH1 Failed \n");
        dmaRightTxHandle = NULL;
    }

    /* Configure DMA channel 2 for I2S left channel read */
    dmaConfig.pingPongMode  = CSL_DMA_PING_PONG_DISABLE;
    dmaConfig.autoMode      = CSL_DMA_AUTORELOAD_ENABLE;
    dmaConfig.burstLen      = CSL_DMA_TXBURST_1WORD;
    dmaConfig.trigger       = CSL_DMA_EVENT_TRIGGER;
#ifdef INSTANCE0_I2S
    dmaConfig.dmaEvt        = CSL_DMA_EVT_I2S0_RX;
#endif
#ifdef INSTANCE2_I2S
    dmaConfig.dmaEvt        = CSL_DMA_EVT_I2S2_RX;
#endif
    dmaConfig.dmaInt        = CSL_DMA_INTERRUPT_ENABLE;
    dmaConfig.chanDir       = CSL_DMA_READ;
    dmaConfig.trfType       = CSL_DMA_TRANSFER_IO_MEMORY;
    dmaConfig.dataLen       = NUM_SAMP_PER_MS*NUM_MS_PER_FRAME*4*2; // two frames

#ifdef INSTANCE0_I2S
#if (defined(CHIP_C5517))
    dmaConfig.srcAddr      = (Uint32)&CSL_I2S0_REGS->I2SRXLTL;
#else
    dmaConfig.srcAddr      = (Uint32)&CSL_I2S0_REGS->I2SRXLT0;
#endif
#endif
#ifdef INSTANCE2_I2S
#if (defined(CHIP_C5517))
    dmaConfig.srcAddr      = (Uint32)&CSL_I2S2_REGS->I2SRXLTL;
#else
    dmaConfig.srcAddr      = (Uint32)&CSL_I2S2_REGS->I2SRXLT0;
#endif
#endif

    dmaConfig.destAddr       = (Uint32)i2sDmaReadBufLeft;

    /* Open DMA ch2 for I2S left channel read */
    dmaLeftRxHandle = DMA_open(DMA_CHAN_RX_L, &dmaObj2,&status);
    if (dmaLeftRxHandle == NULL)
    {
        printf("DMA_open CH2 Failed \n");
        dmaLeftRxHandle = NULL;
    }

    /* Configure DMA channel2 */
    status = DMA_config(dmaLeftRxHandle, &dmaConfig);
    if (status != CSL_SOK)
    {
        printf("DMA_config CH2 Failed \n");
        dmaLeftRxHandle = NULL;
    }

    /* Configure DMA ch3 for I2S right channel read */
    dmaConfig.pingPongMode = CSL_DMA_PING_PONG_DISABLE;
    dmaConfig.autoMode     = CSL_DMA_AUTORELOAD_ENABLE;
    dmaConfig.burstLen     = CSL_DMA_TXBURST_1WORD;
    dmaConfig.trigger      = CSL_DMA_EVENT_TRIGGER;
#ifdef INSTANCE0_I2S
    dmaConfig.dmaEvt        = CSL_DMA_EVT_I2S0_RX;
#endif

#ifdef INSTANCE2_I2S
    dmaConfig.dmaEvt        = CSL_DMA_EVT_I2S2_RX;
#endif
    dmaConfig.dmaInt       = CSL_DMA_INTERRUPT_ENABLE;
    dmaConfig.chanDir      = CSL_DMA_READ;
    dmaConfig.trfType      = CSL_DMA_TRANSFER_IO_MEMORY;
    dmaConfig.dataLen      = NUM_SAMP_PER_MS*NUM_MS_PER_FRAME*4*2;  // two frames

#ifdef INSTANCE0_I2S
#if (defined(CHIP_C5517))
    dmaConfig.srcAddr     = (Uint32)&CSL_I2S0_REGS->I2SRXRTL;
#else
    dmaConfig.srcAddr     = (Uint32)&CSL_I2S0_REGS->I2SRXRT0;
#endif
#endif

#ifdef INSTANCE2_I2S
#if (defined(CHIP_C5517))
    dmaConfig.srcAddr     = (Uint32)&CSL_I2S2_REGS->I2SRXRTL;
#else
    dmaConfig.srcAddr     = (Uint32)&CSL_I2S2_REGS->I2SRXRT0;
#endif
#endif

    dmaConfig.destAddr      = (Uint32)i2sDmaReadBufRight;

    /* Open DMA ch3 for I2S right channel read */
    dmaRightRxHandle = DMA_open(DMA_CHAN_RX_R, &dmaObj3,&status);
    if (dmaRightRxHandle == NULL)
    {
        printf("DMA_open CH3 Failed \n");
        dmaRightRxHandle = NULL;
    }

    /* Configure DMA channel */
    status = DMA_config(dmaRightRxHandle, &dmaConfig);
    if (status != CSL_SOK)
    {
        printf("DMA_config CH3 Failed \n");
        dmaRightRxHandle = NULL;
    }

    return CSL_SOK;
}


/* User defined algorithm */
void UserAlgorithm(void)
{
    Uint16 i;
    Uint16 status=0;
    if(!dmaRxFrameCount)
    {
    	/* Start left Rx DMA */
    	status = DMA_start(dmaLeftRxHandle);
    	if (status != CSL_SOK)
    	{
    		printf("I2S Dma Left Rx Failed!!\n");
    		exit(EXIT_FAILURE);
    	}


    	/* Start right Rx DMA */
    	status = DMA_start(dmaRightRxHandle);
    	if (status != CSL_SOK)
    	{
    		printf("I2S Dma Right Rx Failed!!\n");
    		exit(EXIT_FAILURE);
    	}

    }


    if (dmaRxFrameCount >= 2)
    {
    	dmaRxFrameCount=0;
    	DMA_stop(dmaLeftRxHandle);
    	DMA_stop(dmaRightRxHandle);

    	for (i=0; i<I2S_DMA_BUF_LEN; i++)
        {

            i2sDmaWriteBufLeft[i]  = i2sDmaReadBufLeft[i];
            i2sDmaWriteBufRight[i] = i2sDmaReadBufRight[i];
        }

        /* --- Insert algorithm here --- */

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


    /* Clear DMA frame count */
    if(dmaTxFrameCount>=2)
    {
    	dmaTxFrameCount = 0;
    	DMA_stop(dmaLeftTxHandle);
    	DMA_stop(dmaRightTxHandle);
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
// DMA ISR
interrupt void DmaIsr(void)
{
    volatile Uint16 ifrValue;

    /* Clear the DMA interrupt */
    ifrValue = CSL_SYSCTRL_REGS->DMAIFR;
    CSL_SYSCTRL_REGS->DMAIFR |= ifrValue;

    /* Check for DMA1 CH0 transfer completion */
#ifdef INSTANCE0_I2S
    if (ifrValue & 1)
    {
        dmaTxFrameCount++;
    }
#endif

#ifdef INSTANCE2_I2S
    if (ifrValue & (1<<4))
    {
        dmaTxFrameCount++;
    }
#endif


    /* Check for DMA1 CH1 transfer completion */
#ifdef INSTANCE0_I2S
    if (ifrValue & (1<<1))
    {
        dmaTxFrameCount++;
    }
#endif

#ifdef INSTANCE2_I2S
    if (ifrValue & (1<<5))
    {
        dmaTxFrameCount++;
    }
#endif

    /* Check for DMA1 CH2 transfer completion */
#ifdef INSTANCE0_I2S
    if (ifrValue & (1<<2))
    {
        dmaRxFrameCount++;
    }
#endif
#ifdef INSTANCE2_I2S
    if (ifrValue & (1<<6))
    {
        dmaRxFrameCount++;
    }
#endif

    /* Check for DMA1 CH3 transfer completion */
#ifdef INSTANCE0_I2S
    if (ifrValue & (1<<3))
    {
        dmaRxFrameCount++;
    }
#endif
#ifdef INSTANCE2_I2S

    if (ifrValue & (1<<7))
    {
        dmaRxFrameCount++;
    }
#endif

    dmaIntCount++;
}




