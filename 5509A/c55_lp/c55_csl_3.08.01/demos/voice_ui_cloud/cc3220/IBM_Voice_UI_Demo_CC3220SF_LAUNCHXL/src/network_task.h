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

/** \file network_task.h Defines enums and main wifi thread.\n (C)
 *  Copyright 2017, Texas Instruments, Inc.
 */

#ifndef SRC_NETWORK_TASK_H_
#define SRC_NETWORK_TASK_H_
/* Simplelink includes */
#include <ti/drivers/net/wifi/simplelink.h>
/* Other API includes */
#include "Board.h"
#include "uart_term.h"
#include "sl_event_handlers.h"
#include "sl_helper_functions.h"
#include "appConfig.h"
#include "platform.h"
// HTTP Client lib
#include "ti/net/http/httpcli.h"
#include "ti/net/common.h"

typedef enum
{
    NETWORK_STATE_NULL,
    NETWORK_STATE_SPAWNED,
    NETWORK_STATE_STARTED,
    NETWORK_STATE_CONNECTING,
    NETWORK_STATE_CONNECTED,
    NETWORK_STATE_WAIT_FOR_COMMAND,
    NETWORK_STATE_SOCKET_CONNECTING,
    NETWORK_STATE_SOCKET_CONNECTED,
    NETWORK_STATE_SOCKET_FAILED,
    NETWORK_STATE_WAITING_FOR_DATA,
    NETWORK_STATE_PERFORMING_FLAC_SEND,
    NETWORK_STATE_TRANSMITTING_DATA,
    NETWORK_STATE_TERMINATE_MESSAGE,
    NETWORK_STATE_READING_RESPONSE,
    NETWORK_STATE_CLOSING_SOCKET,
}e_NetworkState;
void * networkTaskThread( void *pvParameters );



#endif /* SRC_NETWORK_TASK_H_ */
