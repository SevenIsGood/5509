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


#ifndef _CODEC_PCM186x_H_
#define _CODEC_PCM186x_H_

// Including profiling code
///#define PROFILE_CYCLES

// Loopback only
///#define LOOPBACK_ONLY

#define USE_48KHZ_RATE
#ifdef USE_48KHZ_RATE
#define ENABLE_AUDIO_CONVERSION
#define SYS_FS_RATIO	3
#endif

// Number of Mics Used
///#define NUM_OF_MICS		2
#define NUM_OF_MICS		4

// Number of output channels
#define NUM_OF_OUTPUT_CH	2

// micr gain in dB (0 dB to 32 dB)
#define MIC_GAIN_DB		32

#if NUM_OF_MICS==4
// Use I2S 3 for getting mic3 & mic4 from LMB
#define INSTANCE0_I2S
#endif

#define UART_RX_BUF_SIZE 256

#include "tistdtypes.h"

#include "cslr_gpio.h"

#include "csl_i2c.h"
#include "csl_gpio.h"

CSL_Status PCM186x_init(Uint16 sampRateRec, Uint32 i2cInClk);
CSL_Status PCM186x_Write(Uint16 regAddr, Uint16 regData, pI2cHandle hi2c);
CSL_Status PCM186x_Write2(Uint16 regAddr, Uint16 regData, pI2cHandle hi2c);
CSL_Status PCM186x_Read(Uint16 regAddr, Uint16 *Data, pI2cHandle  hi2c);

#endif /* _CODEC_PCM186x_H_ */
