/*
* recognizer.h
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
#ifndef _RECOGNIZER_H_
#define _RECOGNIZER_H_

#include <std.h>
#include "Tiesr_engine_api_sireco.h"

#define TIESR_MEMORY_SIZE ( 2668 )

extern TIesrEngineSIRECOType tiesr;
extern TIesr_t tiesrInstance;

/*--------------------------------
 Parameters used within the recognition program to setup the
 TIesrEngine instance.  A lot more parameters could be set to tune the
 recognizer performance.
 --------------------------------*/
#define JAC_RATE            ( 10 )
#define PRUNE_FACTOR        (-6 )
#define TRANSI_WEIGHT       ( 0 )

/* SAD end of utterance parameter 15 = 300ms */
#define SAD_MIN_END_FRAMES  ( 30 )
/* SAD miniumum speech detection value in dB */
#define SAD_SPEECH_DB       ( 60 )

#define _SIL_IDX            ( 0 )
#define _FILL_IDX           ( 5 )

extern Bool gEndRecog;

Void swi_RecognizerFxn(Void);

typedef struct
{
	Uint8 string[20];
	Uint8 command;
} voiceCmd;

#endif
