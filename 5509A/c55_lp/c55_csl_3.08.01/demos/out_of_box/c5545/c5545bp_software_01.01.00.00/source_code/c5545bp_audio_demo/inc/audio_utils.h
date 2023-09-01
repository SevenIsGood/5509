/*
* audio_utils.h
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

#ifndef _AUDIO_UTILS_H_
#define _AUDIO_UTILS_H_

// RTS includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// CSL includes
#include "csl_types.h"
#include "csl_error.h"
#include "soc.h"

// application includes
#include "app_globals.h"
#include "sys_init.h"
#include "sys_state.h"
#include "audio_data_collection.h"

// Equalizer includes
#include "eq_ti.h"
#include "eq.h"

// SRC includes
#include "src_ti.h"
#include "src.h"
#include "src_ti_prvt.h"

#define NUM_OF_BANDS   5
#define MAX_HEAP_SECTIONS 6

Int16 eq_config(Uint8 audSrc);
Void SRC_delete(ISRC_Handle handle);
Int16 src_config(void);
void src_delete(void);
void src_set_samplerate(Uint32 rate);


#endif /* _AUDIO_UTILS_H_ */
