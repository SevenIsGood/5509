/*
* audio_data_collection.h
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
#ifndef _AUDIO_DATA_COLLECTION_H_
#define _AUDIO_DATA_COLLECTION_H_

#include <std.h>
#include "csl_types.h"
#include "csl_i2c.h"
#include "csl_i2s.h"
#include "csl_dma.h"

#include "sample_rate.h"

#define SAMP_RATE                   ( SAMP_RATE_8KHZ )
#define NUM_SAMP_PER_MS             ( SAMPS_PER_MSEC_8KHZ )  // samples per msec
#define NUM_MS_PER_FRAME            ( 20 )  // msec per frame

#define RX_FRAME_LEN                ( NUM_MS_PER_FRAME * NUM_SAMP_PER_MS )  // frame length in samples

// Ping/pong buffer length (one frame for ping, one frame for pong)
#define RX_PING_PONG_BUF_LEN        ( RX_FRAME_LEN * 2 )

#define RX_CIRCBUF_NUM_FRAMES       ( 10 )  // 10 frames in Rx circular buffer
#define RX_CIRCBUF_LEN              ( RX_FRAME_LEN * RX_CIRCBUF_NUM_FRAMES)  // Rx circular buffer length

extern CSL_I2sHandle hI2s;
extern CSL_DMA_ChannelObj dmaObj0;
extern CSL_DMA_Handle hDmaRx;

// Rx ping/pong buffer
extern Uint32 gRxPingPongBuf[RX_PING_PONG_BUF_LEN];
extern Int16 gRxPingPongBufSel;

// Rx circular buffer
extern Int16 gRxCircBuf[RX_CIRCBUF_LEN];
extern Uint16 gRxCircBufInFrame;   // circular buffer input frame
extern Uint16 gRxCircBufOutFrame;  // circular buffer output frame
extern Uint32 gRxCircBufOvrCnt;    // circular buffer overflow count
extern Uint32 gRxCircBufUndCnt;    // circular buffer underflow count

CSL_Status AudioCodecInit(
    pI2cHandle hi2c
);
CSL_Status I2sDmaInit(void);

/* Resets Rx ping/pong buffer */
void resetRxPingPongBuf(
    Uint32 *rxPingPongBuf,
    Uint16 rxPingPongBufLen, 
    Int16 *pRxPingPongBufSel
);

/* Resets Rx circular buffer */
void resetRxCircBuf(
    Int16 *rxCircBuf, 
    Uint16 rxCircBufLen,
    Uint16 *pRxCircBufInFrame, 
    Uint16 *pRxCircBufOutFrame, 
    Uint32 *pRxCircBufOvrCnt,
    Uint32 *pRxCircBufUndCnt
);

Void swi_AudioDataCollectionFxn(Void);
void DMA_Isr(void);

#endif
