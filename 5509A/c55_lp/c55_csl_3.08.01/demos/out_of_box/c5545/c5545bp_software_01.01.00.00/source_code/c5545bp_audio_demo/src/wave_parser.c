/*
* wave_parser.c
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
#include "wave_parser.h"

void swapBytes(Uint16 *input, Uint32 length);

/* Function to parse the wave header and extract the
   information of audio file
 */
void extractWaveInfo(waveHeader *header, Uint16 *data)
{
	memcpy(&header->chunkID, &data[CHUNKID_OFFSET], 2);
	swapBytes((Uint16 *)&header->chunkID, 2);

	memcpy(&header->chunkSize, &data[CHUNKSIZE_OFFSET], 2);
	SWAP_WORDS(header->chunkSize);

	memcpy(&header->format, &data[FORMAT_OFFSET], 2);
	swapBytes((Uint16 *)&header->format, 2);

	memcpy(&header->subchunk1ID, &data[SUBCHUNK1ID_OFFSET], 2);
	swapBytes((Uint16 *)&header->subchunk1ID, 2);
	memcpy(&header->subchunk1Size, &data[SUBCHUNK1SIZE_OFFSET], 2);
	SWAP_WORDS(header->subchunk1Size);

	header->audioFormat = data[AUDIOFORMAT_OFFSET];
	header->numChannels = data[NUMCHANNELS_OFFSET];

	memcpy(&header->sampleRate, &data[SAMPLERATE_OFFSET], 2);
	SWAP_WORDS(header->sampleRate);
	memcpy(&header->byteRate, &data[BYTERATE_OFFSET], 2);
	SWAP_WORDS(header->byteRate);

	header->blockAlign    = data[BLOCKALIGN_OFFSET];
	header->bitsPerSample = data[BITSPERSAMPLE_OFFSET];

	memcpy(&header->subchunk2ID, &data[SUBCHUNK2ID_OFFSET], 2);
	swapBytes((Uint16 *)&header->subchunk2ID, 2);
	memcpy(&header->subchunk2Size, &data[SUBCHUNK2SIZE_OFFSET], 2);
	SWAP_WORDS(header->subchunk2Size);
}

/* Function to check wave header to confirm
   that wave file can be played by the system.
   Modify this function to enable/disable features
   of wave file that are supported by the system.

   Returns 0 if wave header info is compatible with system
   supported features else returns 1
 */
int checkWaveHeader(waveHeader *header)
{
	if((header->chunkID != WAVE_CHUNKID) ||
	   (header->format != WAVE_FORMAT))
	{
		return 1;
	}

//	if((header->audioFormat != 1) ||
//	   (header->numChannels != 1))
	if(header->audioFormat != 1)
	{
		return 1;
	}

	if((header->sampleRate > 48000) ||
	   (header->bitsPerSample != 16))
	{
		return 1;
	}

	return 0;
}

