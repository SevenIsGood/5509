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

/** \file httpvars.h Defines the constants and enums used by httphandler.c .\n (C)
 *  Copyright 2017, Texas Instruments, Inc.
 */

#ifndef SRC_HTTPVARS_H_
#define SRC_HTTPVARS_H_

//First define has smart formatting enabled. Second does not.
//Smart formatting means saying something like "one, two, three" will get transcribed as
//"123" rather than "one two three"
#define POST_REQUEST_URI    "/speech-to-text/api/v1/recognize?smart_formatting=true&max_alternatives=1"
//#define POST_REQUEST_URI    "/speech-to-text/api/v1/recognize?max_alternatives=1"

#define HOST_NAME           "stream.watsonplatform.net" //"<host name>"
#define HOST_PORT           443

#define USE_PROXY           0
#define PROXY_IP            <proxy_ip>
#define PROXY_PORT          <proxy_port>

//#define READ_SIZE           1450
#define MAX_BUFF_SIZE       1460

#define FILE_NAME           "audio-file.flac"
#define CONTENT_TYPE        "audio/flac"
#define CONTENT_TYPE_RAW    "audio/l16;rate=16000;channels=1"
#define CONTENT_TYPE_RAW_2CH "audio/l16;rate=16000;channels=2"

#define READ_API_KEY_FROM_FILE 1
#define API_KEY_FILENAME    "ibm_watson_key.txt"
#define AUTHENTICATION      "Basic Thisisanexamplekeythatdoesnotwork=="
#define ROOT_CA_CERT        "geotrustglobalca.der"

#define DEVICE_YEAR           2017
#define DEVICE_MONTH          7
#define DEVICE_DATE           24

#define HTTP_RETRY_LIMIT      10
// Application specific status/error codes
typedef enum{
 /* Choosing this number to avoid overlap with host-driver's error codes */
    DEVICE_NOT_IN_STATION_MODE = -0x7D0,
    DEVICE_START_FAILED = DEVICE_NOT_IN_STATION_MODE - 1,
    INVALID_HEX_STRING = DEVICE_START_FAILED - 1,
    TCP_RECV_ERROR = INVALID_HEX_STRING - 1,
    TCP_SEND_ERROR = TCP_RECV_ERROR - 1,
    FILE_NOT_FOUND_ERROR = TCP_SEND_ERROR - 1,
    INVALID_SERVER_RESPONSE = FILE_NOT_FOUND_ERROR - 1,
    FORMAT_NOT_SUPPORTED = INVALID_SERVER_RESPONSE - 1,
    FILE_OPEN_FAILED = FORMAT_NOT_SUPPORTED - 1,
    FILE_WRITE_ERROR = FILE_OPEN_FAILED - 1,
    INVALID_FILE = FILE_WRITE_ERROR - 1,
    SERVER_CONNECTION_FAILED = INVALID_FILE - 1,
    GET_HOST_IP_FAILED = SERVER_CONNECTION_FAILED  - 1,

    STATUS_CODE_MAX = -0xBB8
}e_AppStatusCodes;




#endif /* SRC_HTTPVARS_H_ */
