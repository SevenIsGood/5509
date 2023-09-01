/*
* sys_state.h
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

#ifndef _SYS_STATE_H_
#define _SYS_STATE_H_

#include <std.h>
#include "csl_types.h"

#include <csl_mmcsd.h>
#include <csl_mmcsd_ataIf.h>
#include <csl_types.h>

#define SYS_STATE_BLE_FW_FOUND     (0)
#define SYS_STATE_AUDSRC_SELECT    (1)
#define SYS_STATE_IDLE             (2)
#define SYS_STATE_PLAY             (3)
#define SYS_STATE_PAUSE            (4)
#define SYS_STATE_STOP             (5)
#define SYS_STATE_BUSY             (6)
#define SYS_STATE_BLE_FW_UPGRADE   (7)

#define SYS_STATE_MAX_COUNT (8)

#define SWITCH_SW2          (1)
#define SWITCH_SW3          (2)
#define SWITCH_SW4          (3)

#define SWITCH_COUNT_MAX    (3)

#define BLE_INT_IN          (4)
#define BLE_INT_OUT         (5)

#define CMD_NONE            (0)
#define CMD_PLAY_START      (1)
#define CMD_PLAY_STOP       (2)
#define CMD_PLAY_PAUSE      (3)
#define CMD_PLAY_RESUME     (4)
#define CMD_AUDSRC_SEL      (5)
#define CMD_BLEFW_UPGRADE   (6)
#define CMD_BLEFW_SKIP      (7)

#define AUDIO_SOURCE_SD     (0)
#define AUDIO_SOURCE_LINEIN (1)

#define C5545BP_SW_VERSION  "01.01.00.00"

typedef struct _switch2CmdMapping
{
	Uint8  button;
	Uint8  command;
	Uint8  newState;
} switch2CmdMapping;

typedef struct _stateLookup
{
	Uint8  sysState;
	switch2CmdMapping sw2CmdMap [SWITCH_COUNT_MAX];
} stateLookup;

extern Uint8 gSysState;

Void updateDisplay(Void);
Void sdAccessErrDisp(Void);
Void fileAccessErrDisp(Void);
Void fileFormatErrDisp(Void);
Void fileReadStatusDisp(Void);

#endif  /* _SYS_STATE_H_ */

