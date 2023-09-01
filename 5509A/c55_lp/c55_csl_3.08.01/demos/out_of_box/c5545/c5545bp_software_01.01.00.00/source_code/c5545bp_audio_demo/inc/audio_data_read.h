/*
* audio_data_read.h
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

#ifndef _AUDIO_DATA_READ_H_
#define _AUDIO_DATA_READ_H_

#include <std.h>
#include "csl_types.h"
#include "ata.h"
#include "csl_mmcsd.h"
#include "csl_mmcsd_ataif.h"

#define WAVE_DATA_BLOCK_SIZE (32 * TX_FRAME_LEN)
#define WAVE_DATA_BLOCK_COUNT (4)
#define WAVE_DATA_BUF_SIZE (WAVE_DATA_BLOCK_COUNT * WAVE_DATA_BLOCK_SIZE)
#define WAVE_DATA_READBLK_SIZE (32 * TX_FRAME_LEN)
#define WAVE_DATA_READBLK_MIN  (512)

/* Function to intialize SD card and filesystem */
int init_media(void);

/* Function to seek in a file */
AtaError seek_file(AtaFileSize offset);
/* Function to read a file.
   This function fills wave data buffers based on the
   free space available
 */
AtaError read_file(void);

/* Function to open first wave file on the SD card */
AtaError open_first_file(void);

/* Function to find the BLE stack firmware file on SD card */
AtaError find_ble_stack_firmware(Uint8 *stackFound);

#endif  /* _AUDIO_DATA_READ_H_ */

