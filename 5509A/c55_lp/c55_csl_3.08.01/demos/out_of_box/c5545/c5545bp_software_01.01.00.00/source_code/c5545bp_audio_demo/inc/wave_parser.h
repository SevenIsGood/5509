/*
* wave_parser.h
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

#ifndef _WAVE_PARSER_H_
#define _WAVE_PARSER_H_

#include "csl_types.h"

#include <stdio.h>
#include <stdlib.h>

/* Standard wave header structure */
typedef struct _waveHeader
{
	Uint32 chunkID;
	Uint32 chunkSize;
	Uint32 format;
	Uint32 subchunk1ID;
	Uint32 subchunk1Size;
	Uint16 audioFormat;
	Uint16 numChannels;
	Uint32 sampleRate;
	Uint32 byteRate;
	Uint16 blockAlign;
	Uint16 bitsPerSample;
	Uint32 subchunk2ID;
	Uint32 subchunk2Size;
} waveHeader;

/* Word offsets of wave info in 44 bytes wave header */
#define CHUNKID_OFFSET          (0)
#define CHUNKSIZE_OFFSET        (2)
#define FORMAT_OFFSET           (4)
#define SUBCHUNK1ID_OFFSET      (6)
#define SUBCHUNK1SIZE_OFFSET    (8)
#define AUDIOFORMAT_OFFSET      (10)
#define NUMCHANNELS_OFFSET      (11)
#define SAMPLERATE_OFFSET       (12)
#define BYTERATE_OFFSET         (14)
#define BLOCKALIGN_OFFSET       (16)
#define BITSPERSAMPLE_OFFSET    (17)
#define SUBCHUNK2ID_OFFSET      (18)
#define SUBCHUNK2SIZE_OFFSET    (20)

#define WAVE_CHUNKID    (0x52494646)
#define WAVE_FORMAT     (0x57415645)


#define SWAP_WORDS(x) (x = (x >> 16) | (x << 16))

void extractWaveInfo(waveHeader *header, Uint16 *data);
int checkWaveHeader(waveHeader *header);

#endif /* _WAVE_PARSER_H_ */
