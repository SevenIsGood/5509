/*
* sys_init.c
*
*
* Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/ 
* 
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

// RTS includes
#include <stdio.h>
#include <stdlib.h>
// DSP/BIOS includes
#include <std.h>
#include "TIesrDemoC55cfg.h"
// CSL includes
#include "csl_types.h"
#include "csl_error.h"
#include "soc.h"
#include "csl_intc.h"
// application includes
#include "app_globals.h"
#include "pll_control.h"
#include "gpio_control.h"
#include "lcd_osd.h"
#include "codec_aic3254.h"
#include "audio_data_collection.h"
#include "user_interface.h"
#include "recognizer.h"
#include "sys_init.h"

#define I2C_OWN_ADDR            (0x2F)
#define I2C_BUS_FREQ            (400)    // I2C 400Khz

extern Int SEG0;

extern pI2cHandle    i2cHandle;

/*
 *  ======== main ========
 */
Void main(Void)
{
    Int16 p1, p2, p3, p4, p5; // for reading SAD parameters
    CSL_Status cslStatus;
    TIesrEngineStatusType tiesrStatus;
    TIesrEngineJACStatusType jacStatus;
    //MEM_Stat memStatBuf; // FL: debug
    //Bool status;

    /* Initialize DSP */
    cslStatus = DeviceInit();
    if (cslStatus != CSL_SOK)
    {
#ifdef DEBUG_LOG_PRINT
        LOG_printf(&trace, "ERROR: Unable to initialize DSP");
#endif
        exit(EXIT_FAILURE);
    }

    /* Reset Rx ping/pong buffer */
    resetRxPingPongBuf(gRxPingPongBuf, RX_PING_PONG_BUF_LEN, &gRxPingPongBufSel);

    /* Reset Rx circular buffer */
    resetRxCircBuf(gRxCircBuf, RX_CIRCBUF_LEN, &gRxCircBufInFrame, &gRxCircBufOutFrame, &gRxCircBufOvrCnt, &gRxCircBufUndCnt);

    /* Initialize the TIesr engine function object */
    TIesrEngineOpen(&tiesr);

    // FL: debug -- check used heap
    //status = MEM_stat(SEG0, &memStatBuf);

    /* Open an instance of the TIesr engine, loading the grammar */
    //tiesrInstance = (TIesr_t)malloc( memorySize * sizeof(short) );
    tiesrInstance = (TIesr_t)MEM_alloc(SEG0, TIESR_MEMORY_SIZE * sizeof(short), 0);
    if(tiesrInstance == MEM_ILLEGAL)
    {
#ifdef DEBUG_LOG_PRINT
        LOG_printf(&trace, "ERROR: Unable to allocate TIesr memory");
#endif
        exit(EXIT_FAILURE);
    }

    // FL: debug -- check used heap
    //status = MEM_stat(SEG0, &memStatBuf);

    tiesrStatus = tiesr.OpenASR(NULL, TIESR_MEMORY_SIZE, tiesrInstance); //Lali
    if(tiesrStatus != eTIesrEngineSuccess)
    {
#ifdef DEBUG_LOG_PRINT
        LOG_printf(&trace, "ERROR: Unable to open TIesr instance %d", tiesrStatus);
#endif
        exit(EXIT_FAILURE);
    }

    // FL: debug -- check used heap
    //status = MEM_stat(SEG0, &memStatBuf);

    /* Set parameters for recognition. There are many other parameters that
    could be adjusted. */
    tiesr.SetTIesrPrune(tiesrInstance, PRUNE_FACTOR);
    tiesr.SetTIesrTransiWeight(tiesrInstance, TRANSI_WEIGHT);
    tiesr.SetTIesrJacRate(tiesrInstance, JAC_RATE);

    tiesr.GetTIesrSAD(tiesrInstance, &p1, &p2, &p3, &p4, &p5);
    tiesr.SetTIesrSAD(tiesrInstance, p1, SAD_SPEECH_DB, p3, SAD_MIN_END_FRAMES, p5 );

    /* Open the JAC adaptation information. Since JAC file does not exist,
    then use an initial set of parameters */
    jacStatus = tiesr.JAC_load(NULL, tiesrInstance, NULL, NULL);
    if( jacStatus != eTIesrEngineJACSuccess )
    {
#ifdef DEBUG_LOG_PRINT
        LOG_printf(&trace, "Failed to load JAC %d", jacStatus);
#endif
        exit(EXIT_FAILURE);
    }

    gEndRecog = TRUE;

    SWI_post(&SWI_SysInit);
}

//*****************************************************************************
// ClockGatingAll
//*****************************************************************************
//  Description:
//      Clock gates all peripherals,
//      Clock gates HWA FFT,
//      Disables CLKOUT (reduce I/O power),
//      Sets XF to 0 (reduce I/O power since LED not driven)
//
//  Arguments:
//      none
//
//  Return:
//      None
//
//*****************************************************************************
void ClockGatingAll(void)
{
    Uint16 pcgcr_value;
//    Uint16 clkstop_value;
//    Uint16 status;

    // enable the MPORT and disable HWA
    *(volatile ioport Uint16 *)0x0001 = 0x020E;
    asm("   idle");

    // set PCGCR1
    pcgcr_value = 0;
    // clock gating SPI
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_SPICG, DISABLED);
    // clock gating SD/MMC
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_MMCSD0CG, DISABLED);
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_MMCSD1CG, DISABLED);

    // clock stop request for UART
    //clkstop_value = CSL_FMKT(SYS_CLKSTOP_URTCLKSTPREQ, REQ);
    // write to CLKSTOP
    //CSL_FSET(CSL_SYSCTRL_REGS->CLKSTOP, 15, 0, clkstop_value);
    // wait for acknowledge
    //while (CSL_FEXT(CSL_SYSCTRL_REGS->CLKSTOP, SYS_CLKSTOP_URTCLKSTPACK)==0);
    // clock gating UART
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_UARTCG, DISABLED);

    // clock stop request for EMIF
    //clkstop_value = CSL_FMKT(SYS_CLKSTOP_EMFCLKSTPREQ, REQ);
    // write to CLKSTOP
    //CSL_FSET(CSL_SYSCTRL_REGS->CLKSTOP, 15, 0, clkstop_value);
    // wait for acknowledge
    //while (CSL_FEXT(CSL_SYSCTRL_REGS->CLKSTOP, SYS_CLKSTOP_EMFCLKSTPACK)==0);
    // clock gating EMIF
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_EMIFCG, DISABLED);

    // clock gating I2S I2S 0
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_I2S0CG, DISABLED);
    // clock gating I2S I2S 1
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_I2S1CG, DISABLED);
    // clock gating I2S I2S 2
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_I2S2CG, DISABLED);
    // clock gating I2S I2S 3
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_I2S3CG, DISABLED);

    // clock gating DMA0
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_DMA0CG, DISABLED);

    // clock gating Timer 0
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_TMR0CG, DISABLED);
    // clock gating Timer 1
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_TMR1CG, DISABLED);
    // clock gating Timer 2
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_TMR2CG, DISABLED);

    // clock gating I2C
    pcgcr_value |= CSL_FMKT(SYS_PCGCR1_I2CCG, DISABLED);

    // write to PCGCR1
    CSL_FSET(CSL_SYSCTRL_REGS->PCGCR1, 15, 0, pcgcr_value);

    // set PCGCR2
    pcgcr_value = 0;
    // clock gating LCD controller
    pcgcr_value |= CSL_FMKT(SYS_PCGCR2_LCDCG, DISABLED);

    // clock gating SAR
    pcgcr_value |= CSL_FMKT(SYS_PCGCR2_SARCG, DISABLED);

    // clock gating DMA1
    pcgcr_value |= CSL_FMKT(SYS_PCGCR2_DMA1CG, DISABLED);
    // clock gating DMA2
    pcgcr_value |= CSL_FMKT(SYS_PCGCR2_DMA2CG, DISABLED);
    // clock gating DMA3
    pcgcr_value |= CSL_FMKT(SYS_PCGCR2_DMA3CG, DISABLED);

    // clock gating analog registers
    pcgcr_value |= CSL_FMKT(SYS_PCGCR2_ANAREGCG, DISABLED);

    // clock gating USB
    pcgcr_value |= CSL_FMKT(SYS_PCGCR2_USBCG, DISABLED);

    // write to PCGCR2
    CSL_FSET(CSL_SYSCTRL_REGS->PCGCR2, 15, 0, pcgcr_value);

    // disable the CLKOUT. It is on reset
    // set bit 2 of ST3_55 to 1
    asm("    bit(ST3, #ST3_CLKOFF) = #1");

    // turn off the XF
    // set bit 13 of ST1_55 to 0
    asm("    bit(ST1, #ST1_XF) = #0");

#if 0
    // set all GPIO pins to be output and low to save power
    // set the GPIO pin 0 - 15 to output, set SYS_GPIO_DIR0 (0x1C06) bit 0 and 15 to 1
    *(volatile ioport Uint16 *)(0x1C06) = 0xFFFF;
    // set the GPIO pin 16 - 31 to output, set SYS_GPIO_DIR1 (0x1C07) bit 0 and 15 to 1
    *(volatile ioport Uint16 *)(0x1C07) = 0xFFFF;

    // set the GPIO 0 - 15 to 0, set SYS_GPIO_DATAOUT0 (0x1C0A) bit 0 and 15 to 0
    *(volatile ioport Uint16 *)(0x1C0A) = 0x0000;
    // set the GPIO 16 - 31 to 0, set SYS_GPIO_DATAOUT1 (0x1C0B) bit 0 and 15 to 0
    *(volatile ioport Uint16 *)(0x1C0B) = 0x0000;
#endif
}

//*****************************************************************************
// DspLdoSwitch
//*****************************************************************************
//  Description: Sets DSP LDO output to desired voltage
//
//  Arguments:
//      mode    105 - set DSP LDO to 1.05V
//              130 - set DSP LDO to 1.3V
//              other - no change
//
//  Return:
//      none
//
//*****************************************************************************
void DspLdoSwitch(Uint16 mode)
{
    Uint16 pcgcr2;

    pcgcr2 = CSL_SYSCTRL_REGS->PCGCR2; // save PCGCR2

    /* Enable the Analog Register only */
    CSL_SYSCTRL_REGS->PCGCR2 &= ~(0x0040);

    if (mode == 130)
    {
        /* Set DSP LDO to 1.3 V (clear bit 1) */
        CSL_SAR_REGS->USBLDOCNTL &= 0xFFFD;
    }
    else if (mode == 105)
    {
        /* Set DSP LDO to 1.05 V (set bit 1) */
        CSL_SAR_REGS->USBLDOCNTL |= 0x0002;
    }

    CSL_SYSCTRL_REGS->PCGCR2 = pcgcr2; // restore PCGCR2
}

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

    if (mode == 0)
    {
        /* Disable USB LDO (clear bit 0) */
        CSL_SAR_REGS->USBLDOCNTL &= 0xFFFE;
    }
    else if (mode == 1)
    {
        /* Enable USB LDO (set bit 0) */
        CSL_SAR_REGS->USBLDOCNTL |= 0x0001;
    }

    CSL_SYSCTRL_REGS->PCGCR2 = pcgcr2; // restore PCGCR2
}

//*****************************************************************************
// DeviceInit
//*****************************************************************************
//  Description:
//      Clears pending interrupts,
//      Resets all peripherals,
//      Clock gates all peripherals,
//      Sets DSP LDO output to desired voltage,
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

    /* Clear any pending interrupts */
    IRQ_clearAll();

    /* Enable all peripherals */
    CSL_SYSCTRL_REGS->PCGCR1 = 0x0000;
    CSL_SYSCTRL_REGS->PCGCR2 = 0x0000;

    /* Reset all peripherals */
    CSL_SYSCTRL_REGS->PSRCR = 0x0020;
    CSL_SYSCTRL_REGS->PRCR = 0x00BF;
    do
    {
        prcr = CSL_SYSCTRL_REGS->PRCR;
    } while (prcr != 0);

    /* Clock gate all peripherals */
    ClockGatingAll();

    /* Set DSP LDO to desired output voltage */
    DspLdoSwitch(DSP_LDO);

    /* Set PLL output frequency */
    /* Note: desired frequency must be allowed for core voltage setting */
    status = pll_sample(PLL_MHZ);
    if (status != CSL_SOK)
    {
        //LOG_printf(&trace, "pll_sample() fail, status=%d", status);
        return status;
    }

    /* Turn off USB LDO */
    UsbLdoSwitch(0);

    return CSL_SOK;
}

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

    /* Disable DMA interrupt */
    IRQ_disable(DMA_EVENT);

    /* Clear DMA interrupt */
    IRQ_clear(DMA_EVENT);

    /* Disable DMA interrupts */
    CSL_SYSCTRL_REGS->DMAIER = 0x0000;

    /* Clear pending DMA interrupts */
    ifrValue = CSL_SYSCTRL_REGS->DMAIFR;
    CSL_SYSCTRL_REGS->DMAIFR |= ifrValue;

    /* Initialize DMA */
    status = DMA_init();
    if (status != CSL_SOK)
    {
        //LOG_printf(&trace, "DMA_init() fail, status=%d", status);
        return status;
    }

    /* Enable DMA interrupt */
    IRQ_enable(DMA_EVENT);

    return CSL_SOK;
}

/* Initializes I2C module */
CSL_Status I2cInit(void)
{
    CSL_I2cSetup i2cSetup;
    CSL_Status status = CSL_SOK;

    /* Initialize I2C */
    status = I2C_init(CSL_I2C0);
    if (status != CSL_SOK)
    {
        return status;
    }
    /* I2C setup */
    i2cSetup.addrMode    = CSL_I2C_ADDR_7BIT;
    i2cSetup.bitCount    = CSL_I2C_BC_8BITS;
    i2cSetup.loopBack    = CSL_I2C_LOOPBACK_DISABLE;
    i2cSetup.freeMode    = CSL_I2C_FREEMODE_DISABLE;
    i2cSetup.repeatMode  = CSL_I2C_REPEATMODE_DISABLE;
    i2cSetup.ownAddr     = I2C_OWN_ADDR;
    i2cSetup.sysInputClk = appGetSysClk()/(1000000);
    i2cSetup.i2cBusFreq  = I2C_BUS_FREQ;
    
    status = I2C_setup(&i2cSetup);
    if(status != CSL_SOK)
    {
        return status;
    }

    return status;
}

//Uint32 swiSysInitFxnCnt=0; // debug
/*
 *  ======== swi_SysInitFxn ========
 */
Void swi_SysInitFxn(Void)
{
    Uint32 gpioIoDir;
    Uint32 gpioOVal;
    CSL_Status cslStatus;
    Int16 status;

    //swiSysInitFxnCnt++; // debug

    /* Initialize EBSR */
#ifdef PF_C5515_EVM
    /* Configure EBSR */
    /* SP1 Mode 2 (GP[11:6]) */
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_SP1MODE, MODE2); // need GPIO10 for AIC3204 reset on C5515 EVM
#endif

    /* Initialize GPIO */
#ifdef PF_C5515_EVM
    /* Set GPIO10 as output for AIC3204 reset */
    gpioIoDir = (((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN10);
    /* AIC3204 not in reset */
    gpioOVal = (((Uint32)1)<<CSL_GPIO_PIN10);
#elif defined (PF_C5535_EZDSP)
    /* Set GPIO14-17 as outputs for LEDs */
    gpioIoDir = (((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN14);
    gpioIoDir |= (((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN15);
    gpioIoDir |= (((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN16);
    gpioIoDir |= (((Uint32)CSL_GPIO_DIR_OUTPUT)<<CSL_GPIO_PIN17);
    /* LEDs turned off */
    gpioOVal = (((Uint32)1)<<CSL_GPIO_PIN14);
    gpioOVal |= (((Uint32)1)<<CSL_GPIO_PIN15);
    gpioOVal |= (((Uint32)1)<<CSL_GPIO_PIN16);
    gpioOVal |= (((Uint32)1)<<CSL_GPIO_PIN17);
#else
    #warn "Unsupported platform"
#endif

    /* Configure GPIOs */
    status = GpioInit(gpioIoDir, 0x00000000, 0x00000000, gpioOVal);
    if (status != CSL_SOK)
    {
#ifdef DEBUG_LOG_PRINT
        LOG_printf(&trace, "ERROR: Unable to initialize GPIO");
#endif
        exit(EXIT_FAILURE);
    }

    /* Initialize DMA */
    cslStatus = DmaInit();
    if (cslStatus != CSL_SOK)
    {
#ifdef DEBUG_LOG_PRINT
        LOG_printf(&trace, "ERROR: Unable to initialize DMA");
#endif
        exit(EXIT_FAILURE);
    }

    /* Initialize I2C */
    cslStatus = I2cInit();
    if (cslStatus != CSL_SOK)
    {
#ifdef DEBUG_LOG_PRINT
        LOG_printf(&trace, "ERROR: Unable to initialize I2C");
#endif
        exit(EXIT_FAILURE);
    }

#ifdef PF_C5535_EZDSP
    /* Initialize OLED LCD */
    cslStatus = oled_init(i2cHandle);
    if (cslStatus != CSL_SOK)
    {
#ifdef DEBUG_LOG_PRINT
        LOG_printf(&trace, "ERROR: Unable to initialize OLED LCD");
#endif
        exit(EXIT_FAILURE);
    }
#endif

    /* Initialize audio codec */
    cslStatus = AudioCodecInit(i2cHandle);
    if (cslStatus != CSL_SOK)
    {
#ifdef DEBUG_LOG_PRINT
        LOG_printf(&trace, "ERROR: Unable to initialize audio codec");
#endif
        exit(EXIT_FAILURE);
    }

    /* Initialize I2S with DMA */
    cslStatus = I2sDmaInit();
    if (cslStatus != CSL_SOK)
    {
#ifdef DEBUG_LOG_PRINT
        LOG_printf(&trace, "ERROR: Unable to initialize I2S with DMA");
#endif
        exit(EXIT_FAILURE);
    }

    /* Initialize user interface */
    status = userInterfaceInit(UI_DEF_SAMP_FREQ, &cslStatus);
    if (status != UI_SOK)
    {
#ifdef DEBUG_LOG_PRINT
        LOG_printf(&trace, "ERROR: Unable to initialize user input");
#endif
        exit(EXIT_FAILURE);
    }

#if 1 // FL: disturbs DSP/BIOS CPU load graph
    /* Starts user interface */
    status = userInterfaceStart(&cslStatus);
    if (status != UI_SOK)
    {
#ifdef DEBUG_LOG_PRINT
        LOG_printf(&trace, "ERROR: Unable to start user input");
#endif
        exit(EXIT_FAILURE);
    }

#else
    gUcStates.recognizerActive=TRUE; // init recognizer active since no user input
#endif

    /* PP Mode 1 (SPI, GPIO[17:12], UART, and I2S2) */
    CSL_FINST(CSL_SYSCTRL_REGS->EBSR, SYS_EBSR_PPMODE, MODE1);

    /* Configure PCGCR1 & PCGCR2 */
#ifdef PF_C5515_EVM // don't need I2C for C5515 EVM since no OLED LCD
    CSL_FINS(CSL_SYSCTRL_REGS->PCGCR1, SYS_PCGCR1_I2CCG, CSL_SYS_PCGCR1_I2CCG_DISABLED);
#endif
    CSL_FINS(CSL_SYSCTRL_REGS->PCGCR1, SYS_PCGCR1_DMA0CG, CSL_SYS_PCGCR1_DMA0CG_DISABLED);
    CSL_FINS(CSL_SYSCTRL_REGS->PCGCR2, SYS_PCGCR2_DMA2CG, CSL_SYS_PCGCR2_DMA2CG_DISABLED);
    CSL_FINS(CSL_SYSCTRL_REGS->PCGCR2, SYS_PCGCR2_DMA3CG, CSL_SYS_PCGCR2_DMA3CG_DISABLED);

    /* Set CPUI bit in ICR */
    //*(volatile ioport Uint16 *)(0x0001) = (0x000E | 1<<0);
    /* Set CPUI, DPORTI, IPORTI, XPORT and HWAI in ICR */
    *(volatile ioport Uint16 *)(0x0001) = (0x000E | 1<<0 | 1<<5 | 1<<6 | 1<<8 | 1<<9);

    CSL_CPU_REGS->ST1_55 |= (1<<CSL_CPU_ST1_55_XF_SHIFT); // set XF -- indicate system init. complete, recognizer inactive
}

void userIdle(void)
{
    /* Execute idle instruction */
    asm("    idle");
}
