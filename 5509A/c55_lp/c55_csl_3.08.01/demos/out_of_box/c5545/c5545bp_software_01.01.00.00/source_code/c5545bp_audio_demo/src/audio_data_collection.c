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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// DSP/BIOS includes
#include <std.h>
#include "c55AudioDemocfg.h"
// CSL includes
#include "csl_types.h"
#include "csl_i2s.h"
#include "csl_dma.h"
#include "csl_gpio.h"
// application includes
#include "app_globals.h"
#include "pll_control.h"
#include "codec_config.h"
#include "sys_init.h"
#include "sys_state.h"
#include "user_interface.h"
#include "audio_data_collection.h"
#include "audio_data_read.h"
#include "audio_utils.h"
#include "wave_parser.h"

#define ENABLE_STEREO_PROCESSING

CSL_I2sHandle hI2s;
CSL_DMA_ChannelObj dmaObjTxL;
CSL_DMA_ChannelObj dmaObjTxR;
CSL_DMA_ChannelObj dmaObjRx;
CSL_DMA_ChannelObj dmaObjRxR;
CSL_DMA_Handle hDmaRx;
CSL_DMA_Handle hDmaRxR;
CSL_DMA_Handle hDmaTxL;
CSL_DMA_Handle hDmaTxR;

Bool gPlayActive = 0;
Bool gAudioBuffering = 0;
extern Uint8 gWaveFileFound;

#pragma DATA_ALIGN(gRxPingPongBuf, 4);
// Rx ping/pong buffer
Uint16 gRxPingPongBuf[RX_PING_PONG_BUF_LEN];
Uint16 gRxRPingPongBuf[RX_PING_PONG_BUF_LEN];
Uint16 gSilenceBuf[TX_FRAME_LEN] = {0};
//Uint32 gPongBuf[RX_PING_PONG_BUF_LEN];

CSL_DMA_Config dmaTxLConfig;
CSL_DMA_Config dmaTxRConfig;
CSL_DMA_Config dmaRxLConfig;
CSL_DMA_Config dmaRxRConfig;

#pragma DATA_ALIGN(gTxBuf, 4);
Uint16 gTxBuf[RX_PING_PONG_BUF_LEN] = {
        0x0000, 0x10b4, 0x2120, 0x30fb, 0x3fff, 0x4dea, 0x5a81, 0x658b,
        0x6ed8, 0x763f, 0x7ba1, 0x7ee5, 0x7ffd, 0x7ee5, 0x7ba1, 0x76ef,
        0x6ed8, 0x658b, 0x5a81, 0x4dea, 0x3fff, 0x30fb, 0x2120, 0x10b4,
        0x0000, 0xef4c, 0xdee0, 0xcf06, 0xc002, 0xb216, 0xa57f, 0x9a75,
        0x9128, 0x89c1, 0x845f, 0x811b, 0x8002, 0x811b, 0x845f, 0x89c1,
        0x9128, 0x9a76, 0xa57f, 0xb216, 0xc002, 0xcf06, 0xdee0, 0xef4c
    };
Int16 gRxPingPongBufSel;

#pragma DATA_ALIGN(gRxCircBuf, 4);
// Rx circular buffer
Int16 gRxCircBuf[RX_CIRCBUF_LEN];

#pragma DATA_ALIGN(gRxRCircBuf, 4);
// Rx circular buffer
Int16 gRxRCircBuf[RX_CIRCBUF_LEN];

Uint16 gRxCircBufInFrame = 0;   // circular buffer input frame
Uint16 gRxCircBufOutFrame = 0;  // circular buffer output frame
Uint32 gRxCircBufOvrCnt = 0;    // circular buffer overflow count
Uint32 gRxCircBufUndCnt = 0;    // circular buffer underflow count

#pragma DATA_ALIGN(gTxCircBuf, 4);
// Tx circular buffer
Int16 gTxCircBuf[TX_CIRCBUF_LEN];

#ifdef ENABLE_STEREO_PROCESSING
#pragma DATA_ALIGN(gTxRCircBuf, 4);
Int16 gTxRCircBuf[TX_CIRCBUF_LEN];
#endif

Uint16 gTxCircBufInFrame = 0;   // circular buffer input frame
Uint16 gTxCircBufOutFrame = 0;  // circular buffer output frame
Uint32 gTxCircBufOvrCnt = 0;    // circular buffer overflow count
Uint32 gTxCircBufUndCnt = 0;    // circular buffer underflow count

#define ENABLE_CIRC_BUF
#define ENABLE_EQ
#define ENABLE_SRC

#ifdef ENABLE_EQ

#include "eq_ti.h"
#include "eq.h"

extern IEQ_Handle  eqleft, eqright;
extern Int gain[5];
#endif

#ifdef ENABLE_SRC
#include "src_ti.h"
#include "src_ti_prvt.h"
#include "src.h"

extern ISRC_Handle srcLeft, srcRight;
extern int srcOutL[TX_FRAME_LEN];
extern int srcOutR[TX_FRAME_LEN];
extern int srcInL[TX_FRAME_LEN];
extern int srcInR[TX_FRAME_LEN];

#endif

extern AtaUint16   gWaveFileData[WAVE_DATA_BLOCK_COUNT][WAVE_DATA_BLOCK_SIZE];
extern Uint32      gWaveDataRdBlkOffset;
extern Uint32      gWaveDataWrBlkOffset;
extern Int8        gFreeDataBlocks;
extern volatile Uint8 gInFileRead;
extern Uint8 gAudioSrc;
Uint32      gWaveDataRdOffset = 0;

extern pI2cHandle    i2cHandle;
extern Uint8 gUpdateDisp;
extern Bool gSysInitDone;
extern Uint8 gSysState;
extern waveHeader gWaveHdr;

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
    CSL_Status status;
    Uint16 length;

    /* Open I2S instance 2 */
    hI2s = I2S_open(I2S_INSTANCE2, DMA_POLLED, I2S_CHAN_STEREO);
    if (hI2s == NULL)
    {
        status = CSL_ESYS_FAIL;
        //LOG_printf(&trace, "I2S_open() fail, status=%d", status);
        return status;
    }

    for(length = 0; length < TX_FRAME_LEN; length++)
    {
    	gSilenceBuf [length] = 0;
    }

    /* Set the value for the configure structure */
    hwConfig.dataFormat     = I2S_DATAFORMAT_LJUST;
    hwConfig.dataType       = I2S_STEREO_ENABLE;
    hwConfig.loopBackMode   = I2S_LOOPBACK_DISABLE;
    hwConfig.fsPol          = I2S_FSPOL_LOW;
    hwConfig.clkPol         = I2S_RISING_EDGE;
    hwConfig.datadelay      = I2S_DATADELAY_ONEBIT;
    hwConfig.datapack       = I2S_DATAPACK_ENABLE;
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

    /* Open DMA1 ch0 for I2S2 channel read */
    hDmaRx = DMA_open(CSL_DMA_CHAN4, &dmaObjRx, &status);
    if (status != CSL_SOK)
    {
        //LOG_printf(&trace, "DMA_open() fail, status=%d", status);
        return status;
    }

    hDmaTxL = DMA_open(CSL_DMA_CHAN5, &dmaObjTxL, &status);
    if (status != CSL_SOK)
    {
        //LOG_printf(&trace, "DMA_open() fail, status=%d", status);
        return status;
    }

    hDmaTxR = DMA_open(CSL_DMA_CHAN6, &dmaObjTxR, &status);
    if (status != CSL_SOK)
    {
        //LOG_printf(&trace, "DMA_open() fail, status=%d", status);
        return status;
    }

    hDmaRxR = DMA_open(CSL_DMA_CHAN7, &dmaObjRxR, &status);
    if (status != CSL_SOK)
    {
        //LOG_printf(&trace, "DMA_open() fail, status=%d", status);
        return status;
    }

    /* Configure DMA 1 channel 0 for I2S2 left channel read */
    if (gAudioSrc == AUDIO_SOURCE_LINEIN)
    {
		dmaRxLConfig.pingPongMode  = CSL_DMA_PING_PONG_DISABLE;
		dmaRxLConfig.autoMode      = CSL_DMA_AUTORELOAD_DISABLE;
		dmaRxLConfig.dmaInt        = CSL_DMA_INTERRUPT_DISABLE;
		dmaRxLConfig.dataLen       = RX_FRAME_LEN_LINEIN*2; // two frames
	}
	else
	{
		dmaRxLConfig.pingPongMode  = CSL_DMA_PING_PONG_ENABLE;
		dmaRxLConfig.autoMode      = CSL_DMA_AUTORELOAD_ENABLE;
		dmaRxLConfig.dmaInt        = CSL_DMA_INTERRUPT_ENABLE;
		dmaRxLConfig.dataLen       = RX_FRAME_LEN*2*2; // two frames
	}

    dmaRxLConfig.burstLen      = CSL_DMA_TXBURST_1WORD;
    dmaRxLConfig.trigger       = CSL_DMA_EVENT_TRIGGER;
    dmaRxLConfig.dmaEvt        = CSL_DMA_EVT_I2S2_RX;
    dmaRxLConfig.chanDir       = CSL_DMA_READ;
    dmaRxLConfig.trfType       = CSL_DMA_TRANSFER_IO_MEMORY;
    dmaRxLConfig.srcAddr       = (Uint32)&CSL_I2S2_REGS->I2SRXLT0;
    dmaRxLConfig.destAddr      = (Uint32)gRxPingPongBuf;

    /* Configure DMA channel */
    status = DMA_config(hDmaRx, &dmaRxLConfig);
    if (status != CSL_SOK)
    {
        //LOG_printf(&trace, "DMA_config() fail, status=%d", status);
        return status;
    }

    dmaRxRConfig.pingPongMode  = CSL_DMA_PING_PONG_DISABLE;
    dmaRxRConfig.autoMode      = CSL_DMA_AUTORELOAD_DISABLE;
    dmaRxRConfig.burstLen      = CSL_DMA_TXBURST_1WORD;
    dmaRxRConfig.trigger       = CSL_DMA_EVENT_TRIGGER;
    dmaRxRConfig.dmaEvt        = CSL_DMA_EVT_I2S2_RX;
    dmaRxRConfig.dmaInt        = CSL_DMA_INTERRUPT_ENABLE;
    dmaRxRConfig.chanDir       = CSL_DMA_READ;
    dmaRxRConfig.trfType       = CSL_DMA_TRANSFER_IO_MEMORY;
    dmaRxRConfig.dataLen       = RX_FRAME_LEN_LINEIN*2; // two frames
 	dmaRxRConfig.srcAddr       = (Uint32)&CSL_I2S2_REGS->I2SRXRT0;
    dmaRxRConfig.destAddr      = (Uint32)gRxRPingPongBuf;

    /* Configure DMA channel */
    status = DMA_config(hDmaRxR, &dmaRxRConfig);
    if (status != CSL_SOK)
    {
        //LOG_printf(&trace, "DMA_config() fail, status=%d", status);
        return status;
    }

    /* Configure DMA 1 channel 1 for I2S2 left channel write */
    dmaTxLConfig.pingPongMode  = CSL_DMA_PING_PONG_DISABLE;
    dmaTxLConfig.autoMode      = CSL_DMA_AUTORELOAD_DISABLE;
    dmaTxLConfig.burstLen      = CSL_DMA_TXBURST_1WORD;
    dmaTxLConfig.trigger       = CSL_DMA_EVENT_TRIGGER;
    dmaTxLConfig.dmaEvt        = CSL_DMA_EVT_I2S2_TX;
    dmaTxLConfig.dmaInt        = CSL_DMA_INTERRUPT_DISABLE;
    dmaTxLConfig.chanDir       = CSL_DMA_WRITE;
    dmaTxLConfig.trfType       = CSL_DMA_TRANSFER_IO_MEMORY;
    dmaTxLConfig.dataLen       = TX_FRAME_LEN * 2; // one frame
    dmaTxLConfig.srcAddr       = (Uint32)gSilenceBuf;
    dmaTxLConfig.destAddr      = (Uint32)&CSL_I2S2_REGS->I2STXLT0;

    /* Configure DMA channel */
    status = DMA_config(hDmaTxL, &dmaTxLConfig);
    if (status != CSL_SOK)
    {
        //LOG_printf(&trace, "DMA_config() fail, status=%d", status);
        return status;
    }

    /* Configure DMA 1 channel 3 for I2S2 right channel write */
    dmaTxRConfig.pingPongMode  = CSL_DMA_PING_PONG_DISABLE;
    dmaTxRConfig.autoMode      = CSL_DMA_AUTORELOAD_DISABLE;
    dmaTxRConfig.burstLen      = CSL_DMA_TXBURST_1WORD;
    dmaTxRConfig.trigger       = CSL_DMA_EVENT_TRIGGER;
    dmaTxRConfig.dmaEvt        = CSL_DMA_EVT_I2S2_TX;
    dmaTxRConfig.dmaInt        = CSL_DMA_INTERRUPT_ENABLE;
    dmaTxRConfig.chanDir       = CSL_DMA_WRITE;
    dmaTxRConfig.trfType       = CSL_DMA_TRANSFER_IO_MEMORY;
    dmaTxRConfig.dataLen       = TX_FRAME_LEN * 2; // one frame
    dmaTxRConfig.srcAddr       = (Uint32)gSilenceBuf;
    dmaTxRConfig.destAddr      = (Uint32)&CSL_I2S2_REGS->I2STXRT0;

    /* Configure DMA channel */
    status = DMA_config(hDmaTxR, &dmaTxRConfig);
    if (status != CSL_SOK)
    {
        //LOG_printf(&trace, "DMA_config() fail, status=%d", status);
        return status;
    }

	if (gAudioSrc != AUDIO_SOURCE_LINEIN)
	{
		/* Start I2S Rx DMA */
		status = DMA_start(hDmaRx);
		if (status != CSL_SOK)
		{
			//LOG_printf(&trace, "DMA_start() fail, status=%d", status);
			return status;
		}
	}

#ifndef ENABLE_CIRC_BUF
    /* Start I2S Tx DMA */
    status = DMA_start(hDmaTxL);
    if (status != CSL_SOK)
    {
        //LOG_printf(&trace, "DMA_start() fail, status=%d", status);
        return status;
    }
#endif

    /* Enable I2S */
    I2S_transEnable(hI2s, TRUE);

#if 0  // for debug
    for (i = 0; i <= 48; i++)
    {
    	i = i % 48;
        while((CSL_I2S_I2SINTFL_XMITSTFL_MASK & hI2s->hwRegs->I2SINTFL) == 0);  // Wait for transmit interrupt to be pending
        hI2s->hwRegs->I2STXLT0 = gTxBuf[i++];            // 16 bit left channel transmit audio data

       // while((CSL_I2S_I2SINTFL_XMITSTFL_MASK & hI2s->hwRegs->I2SINTFL) == 0);  // Wait for transmit interrupt to be pending
        hI2s->hwRegs->I2STXRT0 = gTxBuf[i];            // 16 bit left channel transmit audio data
    }
#endif

    return CSL_SOK;
}

/* Resets Rx ping/pong buffer */
void resetRxPingPongBuf(
    Uint16 *rxPingPongBuf,
    Uint16 rxPingPongBufLen,
    Int16 *pRxPingPongBufSel
)
{
    memset(rxPingPongBuf, 0, sizeof(Uint16)*rxPingPongBufLen);
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

Uint32 dmaIsrCnt=0; // debug
volatile Uint32 dmaIsrCntTx=0; // debug
Uint32 dmaIsrCntRx=0; // debug
Uint32 bufSel=0; // debug
Uint32 rxBufSel=1; // debug

static handle_txintr_linein(void)
{
	dmaTxLConfig.dataLen  = TX_FRAME_LEN * 2;
	dmaTxLConfig.srcAddr  = (Uint32)&gTxCircBuf[gTxCircBufOutFrame * TX_FRAME_LEN];
	dmaTxLConfig.destAddr = (Uint32)&CSL_I2S2_REGS->I2STXLT0;

	/* Configure DMA channel */
	DMA_config(hDmaTxL, &dmaTxLConfig);


	dmaTxRConfig.dataLen  = TX_FRAME_LEN * 2;
	dmaTxRConfig.srcAddr  = (Uint32)&gTxRCircBuf[gTxCircBufOutFrame * TX_FRAME_LEN];
	dmaTxRConfig.destAddr = (Uint32)&CSL_I2S2_REGS->I2STXRT0;

	/* Configure DMA channel */
	DMA_config(hDmaTxR, &dmaTxRConfig);

	/* Start I2S Rx DMA */
	DMA_start(hDmaTxL);
	/* Start I2S Rx DMA */
	DMA_start(hDmaTxR);

	bufSel ^= 1;
	gTxCircBufOutFrame++;

	if (gTxCircBufOutFrame >= TX_CIRCBUF_NUM_FRAMES)
	{
		gTxCircBufOutFrame = 0;
	}
}

static handle_txintr_sd(void)
{
	dmaTxLConfig.dataLen  = TX_FRAME_LEN * 2;
	dmaTxLConfig.destAddr = (Uint32)&CSL_I2S2_REGS->I2STXLT0;

	if(gAudioBuffering)
	{
		dmaTxLConfig.srcAddr = (Uint32)gSilenceBuf;
	}
	else
	{
		dmaTxLConfig.srcAddr = (Uint32)&gTxCircBuf[gTxCircBufOutFrame * TX_FRAME_LEN];
	}

	/* Configure DMA channel */
	DMA_config(hDmaTxL, &dmaTxLConfig);

	dmaTxRConfig.dataLen  = TX_FRAME_LEN * 2;
	dmaTxRConfig.destAddr = (Uint32)&CSL_I2S2_REGS->I2STXRT0;
	if(gAudioBuffering)
	{
		dmaTxRConfig.srcAddr   = (Uint32)gSilenceBuf;
	}
	else
	{
		if(gWaveHdr.numChannels == 2)
		{
			dmaTxRConfig.srcAddr = (Uint32)&gTxRCircBuf[gTxCircBufOutFrame * TX_FRAME_LEN];
		}
		else
		{
			dmaTxRConfig.srcAddr = (Uint32)&gTxCircBuf[gTxCircBufOutFrame * TX_FRAME_LEN];
		}
	}

	/* Configure DMA channel */
	DMA_config(hDmaTxR, &dmaTxRConfig);

	/* Start I2S Rx DMA */
	DMA_start(hDmaTxL);
	/* Start I2S Rx DMA */
	DMA_start(hDmaTxR);

	bufSel ^= 1;
	if(!gAudioBuffering)
		gTxCircBufOutFrame++;

	if (gTxCircBufOutFrame >= TX_CIRCBUF_NUM_FRAMES)
	{
		gTxCircBufOutFrame = 0;
	}
}

void DMA_Isr(void)
{
    Uint16 ifrValue;

    dmaIsrCnt++;

    /* Read the IFR register of DMA */
    ifrValue = CSL_SYSCTRL_REGS->DMAIFR;
    CSL_SYSCTRL_REGS->DMAIFR = ifrValue;

	/* DMA Rx interrupt while receiving MIC data for TIesr processing */
    if ((1 << hDmaRx->chanNum) & ifrValue)
    {
		dmaIsrCntRx++;
        SWI_post(&SWI_AudioMicDataCollection);
    }

	/* DMA Rx interrupt while receiving LINEIN data for loopback */
    if ((1 << hDmaRxR->chanNum) & ifrValue)
    {
    	dmaIsrCntRx++;

    	dmaRxLConfig.destAddr = (Uint32)&gRxPingPongBuf[rxBufSel * RX_FRAME_LEN_LINEIN];
    	dmaRxRConfig.destAddr = (Uint32)&gRxRPingPongBuf[rxBufSel * RX_FRAME_LEN_LINEIN];

		/* Configure DMA channel */
		DMA_config(hDmaRx, &dmaRxLConfig);
		/* Configure DMA channel */
		DMA_config(hDmaRxR, &dmaRxRConfig);

		DMA_start(hDmaRx);
		DMA_start(hDmaRxR);

		rxBufSel ^= 1;
    }

    if ((1 << hDmaTxR->chanNum) & ifrValue)
    {
    	dmaIsrCntTx++;

		if (gAudioSrc == AUDIO_SOURCE_LINEIN)
		{
			handle_txintr_linein();
		}
		else
		{
			handle_txintr_sd();
		}
    }

	if (gAudioSrc == AUDIO_SOURCE_LINEIN)
	{
	    if ((1 << hDmaRxR->chanNum) & ifrValue)
	    {
	        SWI_post(&SWI_AudioDataCollection);
	    }
	}
	else
	{
	    if ((1 << hDmaTxR->chanNum) & ifrValue)
	    {
			SWI_post(&SWI_AudioDataProcess);

	        if(!gInFileRead)
	        	SWI_post(&SWI_readAudioData);
	    }
	}
}

//Uint32 swiAudioDataCollectionCnt=0; // debug
/*
 *  ======== swi_AudioMicDataCollectionFxn ========
 */
Void swi_AudioMicDataCollectionFxn(Void)
{
    Int16 rxCbNumFrames;
    Uint16 offset, i;

    //swiAudioDataCollectionCnt++; // debug

    if (gUcStates.recognizerActive == 1)
    {
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
            gRxCircBuf[gRxCircBufInFrame*RX_FRAME_LEN + i] = (Int16)(gRxPingPongBuf[offset+i]);
        }
        gRxCircBufInFrame++;
        if (gRxCircBufInFrame >= RX_CIRCBUF_NUM_FRAMES)
        {
            gRxCircBufInFrame = 0;
        }

        SWI_inc(&SWI_Recognizer);
    }
}

/*
 *  ======== swi_AudioDataCollectionFxn ========
 */
Void swi_AudioDataCollectionFxn(Void)
{
    Int16 rxCbNumFrames;
    Uint16 offset, i;

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
	offset = gRxPingPongBufSel * RX_FRAME_LEN_LINEIN;
	gRxPingPongBufSel ^= 1; /* toggle ping/pong flag */

	/* Copy current frame from ping or pong buffer into Rx circular buffer */
	for (i=0; i<RX_FRAME_LEN_LINEIN; i++)
	{
		/* Left channel 32 to 16 bit conversion */
		gRxCircBuf[gRxCircBufInFrame*RX_FRAME_LEN_LINEIN + i]  = gRxPingPongBuf[offset+i];
		gRxRCircBuf[gRxCircBufInFrame*RX_FRAME_LEN_LINEIN + i] = gRxRPingPongBuf[offset+i];
	}
	gRxCircBufInFrame++;
	if (gRxCircBufInFrame >= RX_CIRCBUF_NUM_FRAMES)
	{
		gRxCircBufInFrame = 0;
	}

	SWI_post(&SWI_AudioDataProcess);
}

Uint32 audioDataProcessCnt = 0;

#ifdef ENABLE_STEREO_PROCESSING

static process_audio_data_linein(void)
{
	int i;

	for (i=0; i<TX_FRAME_LEN; i++)
	{
		/* Left channel 32 to 16 bit conversion */
		gTxCircBuf[(gTxCircBufInFrame * TX_FRAME_LEN) + i] = gRxCircBuf[(gRxCircBufOutFrame * RX_FRAME_LEN_LINEIN) + i];
		gTxRCircBuf[(gTxCircBufInFrame * TX_FRAME_LEN) + i] = gRxRCircBuf[(gRxCircBufOutFrame * RX_FRAME_LEN_LINEIN) + i];
	}

#ifdef ENABLE_EQ
    EQ_setGains(eqleft, (int *)gain);
	EQ_setGains(eqright, (int *)gain);

    EQ_apply(eqleft, (Int *)&gTxCircBuf[(gTxCircBufInFrame * TX_FRAME_LEN)],
            (Int *)&gTxCircBuf[(gTxCircBufInFrame * TX_FRAME_LEN)]);
	EQ_apply(eqright, (Int *)&gTxRCircBuf[(gTxCircBufInFrame * TX_FRAME_LEN)],
	         (Int *)&gTxRCircBuf[(gTxCircBufInFrame * TX_FRAME_LEN)]);
#endif

	gTxCircBufInFrame++;
	if (gTxCircBufInFrame >= TX_CIRCBUF_NUM_FRAMES)
	{
		gTxCircBufInFrame = 0;
	}

	gRxCircBufOutFrame++;
	if (gRxCircBufOutFrame >= RX_CIRCBUF_NUM_FRAMES)
	{
		gRxCircBufOutFrame = 0;
	}

	if(!gPlayActive)
	{
		DMA_start(hDmaTxL);
		DMA_start(hDmaTxR);
		gPlayActive = 1;
	}
}

static process_audio_data_sd(void)
{
	int i, index;

#ifdef ENABLE_SRC
	SRC_TI_setOutput(srcLeft, (Int *)&gTxCircBuf[(gTxCircBufInFrame * TX_FRAME_LEN)]);

	if(gWaveHdr.numChannels == 2)
		SRC_TI_setOutput(srcRight, (Int *)&gTxRCircBuf[(gTxCircBufInFrame * TX_FRAME_LEN)]);

	while (!SRC_TI_isOutputBufferFull(srcLeft))
	{
		if (SRC_TI_isInputBufferEmpty(srcLeft))
		{
			index = 0;
			if(gWaveHdr.numChannels == 2)
			{
				for (i = 0; i < (gWaveHdr.numChannels * TX_FRAME_LEN); i++)
				{
					srcInL[index] = gWaveFileData[gWaveDataRdBlkOffset][gWaveDataRdOffset + i++];
					srcInR[index++] = gWaveFileData[gWaveDataRdBlkOffset][gWaveDataRdOffset + i];
				}
			}
			else
			{
				for (i = 0; i < TX_FRAME_LEN; i++)
				{
					srcInL[i] = gWaveFileData[gWaveDataRdBlkOffset][gWaveDataRdOffset + i];
				}
			}

			gWaveDataRdOffset += i;

			if(gWaveDataRdOffset >= WAVE_DATA_BLOCK_SIZE)
			{
				gWaveDataRdOffset = 0;
				gWaveDataRdBlkOffset++;
				gFreeDataBlocks++;
			}

			if(gWaveDataRdBlkOffset >= WAVE_DATA_BLOCK_COUNT)
			{
				gWaveDataRdBlkOffset = 0;
			}

			SRC_TI_setInput(srcLeft, (Int *)srcInL);
			if(gWaveHdr.numChannels == 2)
				SRC_TI_setInput(srcRight, (Int *)srcInR);
		}

		SRC_TI_apply(srcLeft);
		if(gWaveHdr.numChannels == 2)
			SRC_TI_apply(srcRight);
	}
#else
	index = 0;
	for (i = 0; i < (gWaveHdr.numChannels * TX_FRAME_LEN); i++)
	{
		gTxCircBuf[(gTxCircBufInFrame * TX_FRAME_LEN) + index] = gWaveFileData[gWaveDataRdBlkOffset][gWaveDataRdOffset + i];

		if(gWaveHdr.numChannels == 2)
		{
			gTxRCircBuf[(gTxCircBufInFrame * TX_FRAME_LEN) + index] = gWaveFileData[gWaveDataRdBlkOffset][gWaveDataRdOffset + (i + 1)];
			i++;
		}

		index++;
	}

	gWaveDataRdOffset += i;

	if(gWaveDataRdOffset >= WAVE_DATA_BLOCK_SIZE)
	{
		gWaveDataRdOffset = 0;
		gWaveDataRdBlkOffset++;
		gFreeDataBlocks++;
	}

	if(gWaveDataRdBlkOffset >= WAVE_DATA_BLOCK_COUNT)
	{
		gWaveDataRdBlkOffset = 0;
	}
#endif

#ifdef ENABLE_EQ
    EQ_setGains(eqleft, (int *)gain);

    if(gWaveHdr.numChannels == 2)
    	EQ_setGains(eqright, (int *)gain);

    EQ_apply(eqleft, (Int *)&gTxCircBuf[(gTxCircBufInFrame * TX_FRAME_LEN)], (Int *)&gTxCircBuf[(gTxCircBufInFrame * TX_FRAME_LEN)]);

    if(gWaveHdr.numChannels == 2)
    	EQ_apply(eqright, (Int *)&gTxRCircBuf[(gTxCircBufInFrame * TX_FRAME_LEN)], (Int *)&gTxRCircBuf[(gTxCircBufInFrame * TX_FRAME_LEN)]);

#endif

	gTxCircBufInFrame++;
	if (gTxCircBufInFrame >= TX_CIRCBUF_NUM_FRAMES)
	{
		gTxCircBufInFrame = 0;
	}

	gRxCircBufOutFrame++;
	if (gRxCircBufOutFrame >= RX_CIRCBUF_NUM_FRAMES)
	{
		gRxCircBufOutFrame = 0;
	}

	//if(gAudioBuffering && (gTxCircBufInFrame >= TX_CIRCBUF_NUM_FRAMES/2))
	if(gAudioBuffering && (dmaIsrCntTx >= 20))
	{
		gAudioBuffering = 0;
	}
}

/*
 *  ======== swi_AudioDataProcessFxn ========
 */
Void swi_AudioDataProcessFxn(Void)
{
	audioDataProcessCnt++;

	if (gAudioSrc == AUDIO_SOURCE_LINEIN)
	{
		process_audio_data_linein();
	}
	else
	{
		process_audio_data_sd();
	}
}
#else
/*
 *  ======== swi_AudioDataProcessFxn ========
 */
Void swi_AudioDataProcessFxn(Void)
{
	int i;

	audioDataProcessCnt++;

	if (gAudioSrc == AUDIO_SOURCE_LINEIN)
	{
		for (i=0; i<TX_FRAME_LEN; i++)
		{
			/* Left channel 32 to 16 bit conversion */
			gTxCircBuf[(gTxCircBufInFrame * TX_FRAME_LEN) + i] = gRxCircBuf[(gRxCircBufOutFrame * RX_FRAME_LEN_LINEIN) + i];
		}
	}
	else
	{

#ifdef ENABLE_SRC
		SRC_TI_setOutput(srcLeft, (Int *)&gTxCircBuf[(gTxCircBufInFrame * TX_FRAME_LEN)]);
		while (!SRC_TI_isOutputBufferFull(srcLeft))
		{
			if (SRC_TI_isInputBufferEmpty(srcLeft))
			{
				for (i = 0; i < (gWaveHdr.numChannels * TX_FRAME_LEN); i++)
				{
					srcInL[i] = gWaveFileData[gWaveDataRdBlkOffset][gWaveDataRdOffset + (gWaveHdr.numChannels * i)];
				}

				gWaveDataRdOffset += i;

				if(gWaveDataRdOffset >= WAVE_DATA_BLOCK_SIZE)
				{
					gWaveDataRdOffset = 0;
					gWaveDataRdBlkOffset++;
					gFreeDataBlocks++;
				}

				if(gWaveDataRdBlkOffset >= WAVE_DATA_BLOCK_COUNT)
				{
					gWaveDataRdBlkOffset = 0;
				}

				SRC_TI_setInput(srcLeft, (Int *)srcInL);
			}

			SRC_TI_apply(srcLeft);
		}
#else
		for (i=0; i<TX_FRAME_LEN; i++)
		{
			for (i = 0; i < (gWaveHdr.numChannels * TX_FRAME_LEN); i++)
			{
				gTxCircBuf[(gTxCircBufInFrame * TX_FRAME_LEN) + i] = gWaveFileData[gWaveDataRdBlkOffset][gWaveDataRdOffset + (gWaveHdr.numChannels * i)];
			}
		}

		gWaveDataRdOffset += i;

		if(gWaveDataRdOffset >= WAVE_DATA_BLOCK_SIZE)
		{
			gWaveDataRdOffset = 0;
			gWaveDataRdBlkOffset++;
			gFreeDataBlocks++;
		}

		if(gWaveDataRdBlkOffset >= WAVE_DATA_BLOCK_COUNT)
		{
			gWaveDataRdBlkOffset = 0;
		}
#endif
	}

#ifdef ENABLE_EQ
    EQ_setGains(eqleft, (int *)gain);
    EQ_apply(eqleft, (Int *)&gTxCircBuf[(gTxCircBufInFrame * TX_FRAME_LEN)], (Int *)&gTxCircBuf[(gTxCircBufInFrame * TX_FRAME_LEN)]);
#endif

	gTxCircBufInFrame++;
	if (gTxCircBufInFrame >= TX_CIRCBUF_NUM_FRAMES)
	{
		gTxCircBufInFrame = 0;
	}

	gRxCircBufOutFrame++;
	if (gRxCircBufOutFrame >= RX_CIRCBUF_NUM_FRAMES)
	{
		gRxCircBufOutFrame = 0;
	}

	if(!gPlayActive)
	{
		DMA_start(hDmaTxL);
		DMA_start(hDmaTxR);
		gPlayActive = 1;
	}
}
#endif

/* Resets Tx and Rx circular buffer indexes */
Void resetCircBuf(Void)
{
	gRxCircBufInFrame  = 0;
	gRxCircBufOutFrame = 0;
	gRxCircBufOvrCnt   = 0;
	gRxCircBufUndCnt   = 0;

	gTxCircBufInFrame  = 0;
	gTxCircBufOutFrame = 0;
	gTxCircBufOvrCnt   = 0;
	gTxCircBufUndCnt   = 0;

}

/* Function to stop the DMA */
Void stopDma(Void)
{
	if (gAudioSrc == AUDIO_SOURCE_LINEIN)
	{
		DMA_stop(hDmaRx);
		DMA_stop(hDmaRxR);
	}

	DMA_stop(hDmaTxL);
	DMA_stop(hDmaTxR);
}

/* Function to start DMA */
Void startDma(Void)
{
	if (gAudioSrc == AUDIO_SOURCE_LINEIN)
	{
		dmaRxLConfig.destAddr = (Uint32)gRxPingPongBuf;
		dmaRxRConfig.destAddr = (Uint32)gRxRPingPongBuf;

		/* Configure Rx DMA channels */
		DMA_config(hDmaRx, &dmaRxLConfig);
		DMA_config(hDmaRxR, &dmaRxRConfig);

		rxBufSel = 1;

	    dmaTxLConfig.srcAddr = (Uint32)gRxPingPongBuf;
	    dmaTxRConfig.srcAddr = (Uint32)gRxPingPongBuf;

		/* Configure Tx DMA channels */
		DMA_config(hDmaTxL, &dmaTxLConfig);
		DMA_config(hDmaTxR, &dmaTxRConfig);

		DMA_start(hDmaRx);
		DMA_start(hDmaRxR);
	}
	else
	{
		/* Send silence on Tx line till the output of SRC is available  */
	    dmaTxLConfig.srcAddr = (Uint32)gSilenceBuf;
	    dmaTxRConfig.srcAddr = (Uint32)gSilenceBuf;

		/* Configure Tx DMA channels */
		DMA_config(hDmaTxL, &dmaTxLConfig);
		DMA_config(hDmaTxR, &dmaTxRConfig);

		DMA_start(hDmaTxL);
		DMA_start(hDmaTxR);
		gPlayActive = 1;
	}
}

/* Function to start playback */
Void startPlay(Void)
{
	resetCircBuf();

	if (gAudioSrc == AUDIO_SOURCE_SD)
	{
		src_config();
		src_set_samplerate(gWaveHdr.sampleRate);
		//SWI_post(&SWI_AudioDataProcess);
		gAudioBuffering = 1;
		dmaIsrCntTx = 0;
	}

	gPlayActive = 0;
	startDma();

	if (gAudioSrc == AUDIO_SOURCE_LINEIN)
	{
		I2S_transEnable(hI2s, TRUE);
	}
}

/* Function to stop playback */
Void stopPlay(Void)
{
	if (gAudioSrc == AUDIO_SOURCE_LINEIN)
	{
		I2S_transEnable(hI2s, FALSE);
	}
	stopDma();

	/* Clear the Tx registers to avoid noise as I2S will be
	   active for TIesr operation */
	CSL_I2S2_REGS->I2STXLT0 = 0;
	CSL_I2S2_REGS->I2STXLT1 = 0;
	CSL_I2S2_REGS->I2STXRT0 = 0;
	CSL_I2S2_REGS->I2STXRT1 = 0;

	gPlayActive = 0;
	resetCircBuf();

	if (gAudioSrc == AUDIO_SOURCE_SD)
	{
		src_delete();
		gFreeDataBlocks = 4;
		gWaveDataRdBlkOffset = 0;
		gWaveDataWrBlkOffset = 0;
		gWaveDataRdOffset = 0;
		seek_file(22);
		read_file();
		read_file();
	}
}

/* Function to pause playback */
Void pausePlay(Void)
{
	if (gAudioSrc == AUDIO_SOURCE_LINEIN)
	{
		I2S_transEnable(hI2s, FALSE);
	}

	stopDma();

	/* Clear the Tx registers to avoid noise as I2S will be
	   active for TIesr operation */
	CSL_I2S2_REGS->I2STXLT0 = 0;
	CSL_I2S2_REGS->I2STXLT1 = 0;
	CSL_I2S2_REGS->I2STXRT0 = 0;
	CSL_I2S2_REGS->I2STXRT1 = 0;

	gPlayActive = 0;
	resetCircBuf();
}

/* Function to resume playback */
Void resumePlay(Void)
{
	gPlayActive = 0;
	startDma();

	if (gAudioSrc == AUDIO_SOURCE_LINEIN)
	{
		I2S_transEnable(hI2s, TRUE);
	}
}

int init_media(void);

/* Configures audio codec and audio processing modules based
   on the audio source selection
 */
CSL_Status selectAudioSource(Uint8 audSrc)
{
	CSL_Status status = 0;

	gUpdateDisp = 0;
	gAudioSrc   = audSrc;
	gSysState   = SYS_STATE_BUSY;

	updateDisplay();

	if(audSrc == AUDIO_SOURCE_SD)
	{
		/* Configure audio codec */
		status = codecConfigPlayback(SAMP_RATE_SD, 1, AIC_AMIC);
		if (status != CSL_SOK)
		{
			return status;
		}

		gUcStates.recognizerActive = 1;
		gUcStates.recognizerActiveUpd = TRUE;

#if 0
		/* Configure SRC module */
		if(src_config())
		{
			exit(1);
		}
#endif

		/* Configure equalizer module */
		if(eq_config(audSrc))
		{
			exit(1);
		}

		/* Initialize filesystem */
		//if(init_media())
		if(open_first_file())
		{
			if(gWaveFileFound == 0)
			{
				fileFormatErrDisp();
				exit(1);
			}
			else
			{
				sdAccessErrDisp();
				exit(1);
			}
		}
	}
	else
	{
		/* Configure audio codec */
		status = codecConfigLoopback();
		if (status != CSL_SOK)
		{
			exit(1);
		}

		/* Configure equalizer module */
		if(eq_config(audSrc))
		{
			exit(1);
		}
	}

    /* Initialize I2S with DMA */
    status = I2sDmaInit();
    if (status != CSL_SOK)
    {
        exit(EXIT_FAILURE);
    }

	/* Delay for codec to get ready */
    C55x_delay_msec(2000);

    gSysState = SYS_STATE_IDLE;
    updateDisplay();
    gSysInitDone = 1;

	/* In case of audio selection from SD card, it is not possible to
	   update display due to high CPU load of TIesr.
	   Display update is disable in case of SD
	 */
    if(audSrc != AUDIO_SOURCE_SD)
    	gUpdateDisp = 1;

    return 0;
}
