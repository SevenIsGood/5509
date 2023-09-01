/* --COPYRIGHT--,BSD
 * Copyright (c) 2017, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/

/** \file audio_collect_private.h Defines the audio ping-pong buffer object,
 * which contains the state information to implement a ping-pong
 * buffer for audio data, starting and stopping audio data collection, and
 * the interrupt handler for the DMA interrupt that declares a buffer of
 * data is available for processing.\n (C) Copyright 2017, Texas Instruments,
 * Inc.
 */

#ifndef AUDIO_COLLECT_PRIVATE_H_
#define AUDIO_COLLECT_PRIVATE_H_

/*---------------------------------------------------------------------------
 * Includes
 * -------------------------------------------------------------------------*/
#include <stdbool.h>
#include <stdint.h>

#include "audio_collect.h"

/*---------------------------------------------------------------------------
 * Defines and Typedefs
 * -------------------------------------------------------------------------*/
/* The Audio object structure, containing the Audio instance information */
typedef struct Audio_Struct
{
    /* Ping-pong audio buffers */
    int_least16_t *audioBuffer1;
    int_least16_t *audioBuffer2;

    /* Size of both audio buffers */
    int_fast32_t bufferSize;

    /* audio sample rate in Hz */
    int_fast32_t sampleRate;

    /* Flag indicating DMA is filling audioBuffer1 */
    bool fillingBuffer1;

    /* Flag indicating DMA has a filled buffer, and is filling the other */
    bool bufferActive;

    /* Flag indicating that the channel has not processed data in real time */
    bool overflow;

    /* Flag indicating that the ADC is already being sampled with an active timer */
    bool samplingActive;

    /* Flag indicating that the UART audio stream is corrupt and the UART interface needs a reset */
    bool uartResetRequired;

    /* Flag indicating that incoming UART data should be ignored and discarded */
    bool uartIgnoreData;
} Audio_Object;



#endif /* AUDIO_COLLECT_PRIVATE_H_ */
