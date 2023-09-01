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

/** \file httphandler.h Defines the HTTP handling functions.\n (C)
 *  Copyright 2017, Texas Instruments, Inc.
 */

#ifndef SRC_HTTPHANDLER_H_
#define SRC_HTTPHANDLER_H_

/*----------------------------------------------------------------------------
 * Includes
 * -------------------------------------------------------------------------*/
#include "httpvars.h"
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


/*----------------------------------------------------------------------------
 * Defines and Typedefs
 * -------------------------------------------------------------------------*/
typedef enum
{
    HTTP_POST_FLACFILE,
    HTTP_POST_RAW,
    HTTP_POST_RAW_2CH,

}e_HTTPPostSettings;
typedef struct IBMTestPacket
{
    char* bufferPointer;
    int bytesInBuffer;
    int lineToStartPrint;
    bool postOnCompletion;
} IBMTestPacket;
/*----------------------------------------------------------------------------
 * Functions
 * --------------------------------------------------------------------------*/
int FlushHTTPResponse(HTTPCli_Handle httpClient);
int ParseJSONData(char *ptr);
int readResponse(HTTPCli_Handle httpClient);
int HTTPPostMethod(HTTPCli_Handle httpClient, int postSettings, IBMTestPacket* dataToPost);
int ConnectToHTTPServer(HTTPCli_Handle httpClient, HTTPCli_Params *params);
void DisconnectFromHTTPServer(HTTPCli_Handle httpClient);


#endif /* SRC_HTTPHANDLER_H_ */
