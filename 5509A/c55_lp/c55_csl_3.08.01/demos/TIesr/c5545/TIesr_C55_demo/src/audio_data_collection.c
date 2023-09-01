/*
* audio_data_collection.c
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
#include <string.h>
// DSP/BIOS includes
#include <std.h>
#include "TIesrDemoC55cfg.h"
// CSL includes
#include "csl_types.h"
#include "csl_i2s.h"
#include "csl_dma.h"
#include "csl_gpio.h"
// application includes
#include "app_globals.h"
#include "pll_control.h"
#include "codec_aic3254.h"
#include "sys_init.h"
#include "user_interface.h"
#include "audio_data_collection.h"

CSL_I2sHandle hI2s;
CSL_DMA_ChannelObj dmaObj0;
CSL_DMA_Handle hDmaRx;

// Rx ping/pong buffer
Uint32 gRxPingPongBuf[RX_PING_PONG_BUF_LEN];
Int16 gRxPingPongBufSel;

// Rx circular buffer
Int16 gRxCircBuf[RX_CIRCBUF_LEN];
Uint16 gRxCircBufInFrame;   // circular buffer input frame
Uint16 gRxCircBufOutFrame;  // circular buffer output frame
Uint32 gRxCircBufOvrCnt;    // circular buffer overflow count
Uint32 gRxCircBufUndCnt;    // circular buffer underflow count


//*****************************************************************************
// AudioCodecGpioInit
//*****************************************************************************
//  Description:
//      Configures EBSR so GPIO10 routed to SP1,
//      Configures GPIO10 as output for AIC3204 reset on C5515 EVM,
//      Initializes AIC3204 codec
//
//  Arguments:
//      none
//
//  Return:
//      CSL_Status: CSL_SOK - Audio codec initialization successful
//                  other   - Audio codec initialization unsuccessful
//
//*****************************************************************************
CSL_Status AudioCodecInit(
    pI2cHandle hi2c
)
{
    CSL_Status status;

    /* Configure AIC3204 codec */
    status = AIC3254_init(SAMP_RATE, SAMP_RATE, hi2c);
    if (status != CSL_SOK)
    {
        //LOG_printf(&trace, "AIC3254_init() fail, status=%d", status);
        return status;
    }

    return CSL_SOK;
}

//*****************************************************************************
// I2sDmaInit
//*****************************************************************************
//  Description:
//      Configures and starts I2S with DMA
//
//  Parameters:
//      hI2s                - I2S handle (global)
//      dmaLeftRxHandle     - handle for I2S left DMA (global)
//      dmaRightRxHandle    - handle for I2S right DMA (global)
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

    /* Open I2S instance 2 (AIC3204 is connected to I2S2 on C5515 EVM) */
    hI2s = I2S_open(I2S_INSTANCE2, DMA_POLLED, I2S_CHAN_STEREO);
    if (hI2s == NULL)
    {
        status = CSL_ESYS_FAIL;
        //LOG_printf(&trace, "I2S_open() fail, status=%d", status);
        return status;
    }

    /* Set the value for the configure structure */
    hwConfig.dataFormat     = I2S_DATAFORMAT_LJUST;
    hwConfig.dataType       = I2S_STEREO_ENABLE;
    hwConfig.loopBackMode   = I2S_LOOPBACK_DISABLE;
    hwConfig.fsPol          = I2S_FSPOL_LOW;
    hwConfig.clkPol         = I2S_FALLING_EDGE;
    hwConfig.datadelay      = I2S_DATADELAY_ONEBIT;
    hwConfig.datapack       = I2S_DATAPACK_DISABLE;
    hwConfig.signext        = I2S_SIGNEXT_DISABLE;
    hwConfig.wordLen        = I2S_WORDLEN_16;
    hwConfig.i2sMode        = I2S_SLAVE;
    hwConfig.clkDiv         = I2S_CLKDIV2; // don't care for slave mode
    hwConfig.fsDiv          = I2S_FSDIV32; // don't care for slave mode
    hwConfig.FError         = I2S_FSERROR_DISABLE;
    hwConfig.OuError        = I2S_OUERROR_DISABLE;

    /* Configure I2S hardware registers */
    status = I2S_setup(hI2s, &hwConfig);
    if(status != CSL_SOK)
    {
        //LOG_printf(&trace, "I2S_setup() fail, status=%d", status);
        return status;
    }

    /* Configure DMA 1 channel 0 for I2S2 left channel read */
    dmaConfig.pingPongMode  = CSL_DMA_PING_PONG_ENABLE;
    dmaConfig.autoMode      = CSL_DMA_AUTORELOAD_ENABLE;
    dmaConfig.burstLen      = CSL_DMA_TXBURST_1WORD;
    dmaConfig.trigger       = CSL_DMA_EVENT_TRIGGER;
    dmaConfig.dmaEvt        = CSL_DMA_EVT_I2S2_RX;
    dmaConfig.dmaInt        = CSL_DMA_INTERRUPT_ENABLE;
    dmaConfig.chanDir       = CSL_DMA_READ;
    dmaConfig.trfType       = CSL_DMA_TRANSFER_IO_MEMORY;
    dmaConfig.dataLen       = RX_FRAME_LEN*4*2; // two frames
    dmaConfig.srcAddr       = (Uint32)&CSL_I2S2_REGS->I2SRXLT0;
    dmaConfig.destAddr      = (Uint32)gRxPingPongBuf;

    /* Open DMA1 ch0 for I2S2 channel read */
    hDmaRx = DMA_open(CSL_DMA_CHAN4, &dmaObj0, &status);
    if (status != CSL_SOK)
    {
        //LOG_printf(&trace, "DMA_open() fail, status=%d", status);
        return status;
    }

    /* Configure DMA channel */
    status = DMA_config(hDmaRx, &dmaConfig);
    if (status != CSL_SOK)
    {
        //LOG_printf(&trace, "DMA_config() fail, status=%d", status);
        return status;
    }

    /* Start I2S Rx DMA */
    status = DMA_start(hDmaRx);
    if (status != CSL_SOK)
    {
        //LOG_printf(&trace, "DMA_start() fail, status=%d", status);
        return status;
    }

    /* Enable I2S */
    I2S_transEnable(hI2s, TRUE);

    return CSL_SOK;
}

/* Resets Rx ping/pong buffer */
void resetRxPingPongBuf(
    Uint32 *rxPingPongBuf,
    Uint16 rxPingPongBufLen, 
    Int16 *pRxPingPongBufSel
)
{
    memset(rxPingPongBuf, 0, sizeof(Uint32)*rxPingPongBufLen);
    *pRxPingPongBufSel = 0;
}

/* Resets Rx circular buffer */
void resetRxCircBuf(
    Int16 *rxCircBuf, 
    Uint16 rxCircBufLen,
    Uint16 *pRxCircBufInFrame, 
    Uint16 *pRxCircBufOutFrame, 
    Uint32 *pRxCircBufOvrCnt,
    Uint32 *pRxCircBufUndCnt
)
{
    memset(rxCircBuf, 0, rxCircBufLen);
    *pRxCircBufOutFrame = 0;
    *pRxCircBufInFrame = *pRxCircBufOutFrame + 1;
    *pRxCircBufOvrCnt = 0;
    *pRxCircBufUndCnt = 0;
}

//Uint32 dmaIsrCnt=0; // debug
void DMA_Isr(void)
{
    Uint16 ifrValue;

    //dmaIsrCnt++;

    /* Read the IFR register of DMA */
    ifrValue = CSL_SYSCTRL_REGS->DMAIFR;
    CSL_SYSCTRL_REGS->DMAIFR = ifrValue;

    if ((1 << hDmaRx->chanNum) & ifrValue)
    {
        SWI_post(&SWI_AudioDataCollection);
    }
}

//Uint32 swiAudioDataCollectionCnt=0; // debug
/*
 *  ======== swi_AudioDataCollectionFxn ========
 */
Void swi_AudioDataCollectionFxn(Void)
{
    Int16 rxCbNumFrames;
    Uint16 offset, i;

    //swiAudioDataCollectionCnt++; // debug

    //CSL_CPU_REGS->ST1_55 ^= (1<<CSL_CPU_ST1_55_XF_SHIFT); // toggle XF -- debug

#ifdef C5535_EZDSP
     if (gUcStates.recognizerActive == 1)
     {
#endif
        CSL_CPU_REGS->ST1_55 ^= (1<<CSL_CPU_ST1_55_XF_SHIFT); // toggle XF -- indicate recognizer active

        /* Check Rx circular buffer overflow */
        rxCbNumFrames = gRxCircBufInFrame - gRxCircBufOutFrame;
        if (rxCbNumFrames < 0)
        {
            rxCbNumFrames += RX_CIRCBUF_NUM_FRAMES;
        }
        if (rxCbNumFrames > (RX_CIRCBUF_NUM_FRAMES-1))
        {
            gRxCircBufOvrCnt++;
#ifdef DEBUG_LOG_PRINT
            LOG_printf(&trace, "ERROR: Rx circular buffer overflow");
#endif
        }

        /* Determine which frame to use ping or pong */
        offset = gRxPingPongBufSel * RX_FRAME_LEN;
        gRxPingPongBufSel ^= 1; /* toggle ping/pong flag */

        /* Copy current frame from ping or pong buffer into Rx circular buffer */
        for (i=0; i<RX_FRAME_LEN; i++)
        {
            /* Left channel 32 to 16 bit conversion */
            gRxCircBuf[gRxCircBufInFrame*RX_FRAME_LEN + i] = (Int16)(gRxPingPongBuf[offset+i]>>16);
        }
        gRxCircBufInFrame++;
        if (gRxCircBufInFrame >= RX_CIRCBUF_NUM_FRAMES)
        {
            gRxCircBufInFrame = 0;
        }

        SWI_inc(&SWI_Recognizer);
		
#ifdef C5535_EZDSP
      }
#endif
}
