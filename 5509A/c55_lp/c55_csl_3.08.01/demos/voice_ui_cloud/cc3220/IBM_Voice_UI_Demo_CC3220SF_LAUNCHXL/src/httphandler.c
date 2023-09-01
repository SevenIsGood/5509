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

/** \file httphandler.c Implements the HTTP handling, and invokes IBM Watson API with HTTPCli calls.\n (C)
 *  Copyright 2017, Texas Instruments, Inc.
 */
/*----------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------*/

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "httphandler.h"
#include "lcd.h"
#include "network_task.h"
/*----------------------------------------------------------------------------
 * Globals
 * -------------------------------------------------------------------------*/
unsigned char g_buff[MAX_BUFF_SIZE+1];
unsigned long  g_ulDestinationIP; // IP address of destination server
extern char g_charTranscript;
extern char g_charConfidenceLevel;
extern int g_intNetworkState;
extern int g_intServerResponded;
extern char APIKey;
/*----------------------------------------------------------------------------
 * Functions
 * --------------------------------------------------------------------------*/
//Helper functions to get the confidence and transcript from the JSON response received back from IBM Watson's recognize API
// Function to take in a JSON string and a float (0-1.0) and update the value of the float with min(currentMinConfidence,value of "confidence" in JSON)
int getConfidenceFromJSONString(char ** jsonData, float *currentMinConfidence);
//Function to take in a JSON string, a target string, and size of the string and copy the string data of the "transcript" object to the target
int getTranscriptFromJSONString(char ** jsonData, char * currentTranscript,
                                int spaceLeftInTranscript);

/*-------------------------------------------------------------------------*/
//*****************************************************************************
//
//! \brief Flush response body.
//!
//! \param[in]  httpClient - Pointer to HTTP Client instance
//!
//! \return 0 on success else error code on failure
//!
//*****************************************************************************
int FlushHTTPResponse(HTTPCli_Handle httpClient)
{
    const char *ids[2] = {
                            HTTPCli_FIELD_NAME_CONNECTION, /* App will get connection header value. all others will skip by lib */
                            NULL
                         };
    char buf[128];
    int id;
    int len = 1;
    bool moreFlag = 0;
    char ** prevRespFilelds = NULL;


    /* Store previosly store array if any */
    prevRespFilelds = HTTPCli_setResponseFields(httpClient, ids);

    /* Read response headers */
    while ((id = HTTPCli_getResponseField(httpClient, buf, sizeof(buf), &moreFlag))
            != HTTPCli_FIELD_ID_END)
    {

        if(id == 0)
        {
            if(!strncmp(buf, "close", sizeof("close")))
            {
                UART_PRINT("Connection terminated by server\n\r");
            }
        }

    }

    /* Restore previosuly store array if any */
    HTTPCli_setResponseFields(httpClient, (const char **)prevRespFilelds);

    while(1)
    {
        /* Read response data/body */
        /* Note:
                moreFlag will be set to 1 by HTTPCli_readResponseBody() call, if more
                data is available Or in other words content length > length of buffer.
                The remaining data will be read in subsequent call to HTTPCli_readResponseBody().
                Please refer HTTP Client Libary API documenation @ref HTTPCli_readResponseBody
                for more information.
        */
        HTTPCli_readResponseBody(httpClient, buf, sizeof(buf) - 1, &moreFlag);
        ASSERT_ON_ERROR(len);

        if ((len - 2) >= 0 && buf[len - 2] == '\r' && buf [len - 1] == '\n'){
            break;
        }

        if(!moreFlag)
        {
            /* There no more data. break the loop. */
            break;
        }
    }
    return 0;
}
//*****************************************************************************
//
//! \brief Handler for parsing JSON data
//!
//! \param[in]  ptr - Pointer to http response body data
//!
//! \return 0 on success else error code on failure
//!
//*****************************************************************************
int ParseJSONData(char *ptr)
{
    long lRetVal = 0;
    char* globalTranscriptPointer = &g_charTranscript;
    char* globalConfidenceLevelPointer = &g_charConfidenceLevel;
    //float to keep track of the highest confidence score observed by the parser
    float currentHighestConfidence = 1;

    char * jsonData = ptr;
    char * currentTranscriptPosition = globalTranscriptPointer;
    UART_PRINT("%s\r\n",jsonData);
    bool done = false;
    bool errorOccured = false;
    int spaceLeft = MAX_TRANSCRIPT_LENGTH-1;
    memset(globalTranscriptPointer,0,MAX_TRANSCRIPT_LENGTH);
    int iRetVal = 0;
    while (!done)
    {
        iRetVal = getConfidenceFromJSONString(&jsonData,
                                              &currentHighestConfidence);
        if (iRetVal<0)
        {
            done = true;
            break;
        }
        iRetVal = getTranscriptFromJSONString(&jsonData, currentTranscriptPosition, spaceLeft);
        if(iRetVal==spaceLeft){
            done = true;
            currentTranscriptPosition += iRetVal;
            spaceLeft -= iRetVal;
        }
        else if(iRetVal==-1){
            done = true;
        }
        else if(iRetVal==-2){
            done = true;
            errorOccured = true;
        }
        else{
            currentTranscriptPosition += iRetVal;
            spaceLeft -= iRetVal;
        }
    }
    currentTranscriptPosition[MAX_TRANSCRIPT_LENGTH-spaceLeft-1] = '\0';
    if (currentTranscriptPosition > globalTranscriptPointer && !errorOccured) //Check for errors
    {
        UART_PRINT("transcript: %s\n\r", &g_charTranscript);
        LCD_showMessageLine(&g_charTranscript, 45);
        char confidenceString[7];
        //convert the confidence into a percentage
        currentHighestConfidence = currentHighestConfidence * 100;
        sprintf(confidenceString, "%.1f", currentHighestConfidence);
        confidenceString[strlen(confidenceString) + 1] = '\0';
        confidenceString[strlen(confidenceString)] = '%';
        memcpy(globalConfidenceLevelPointer, confidenceString,
               strlen(confidenceString));
        globalConfidenceLevelPointer[strlen(confidenceString)] = '\0';
        UART_PRINT("confidence level: %s\n\r", &g_charConfidenceLevel);
        g_intServerResponded = 1;
    }
    else
    {
        UART_PRINT("Server could not identify any speech, try again.\n\r");
        g_intServerResponded = 0;
    }

    return lRetVal;
}
/*!
    \brief This function read respose from server and dump on console

    \param[in]      httpClient - HTTP Client object

    \return         0 on success else -ve

    \note

    \warning
*/
int readResponse(HTTPCli_Handle httpClient)
{
    long lRetVal = 0;
    int bytesRead = 0;
    int id = 0;
    bool bufferIsDynamicallyAllocated = false;
    unsigned long len = MAX_BUFF_SIZE;
    int json = 0;
    char *dataBuffer=NULL;
    bool moreFlags = 1;
    const char *ids[15] = {
                                        "X-Note",
                            "X-Backside-Transport",
                            HTTPCli_FIELD_NAME_CONNECTION,
                            HTTPCli_FIELD_NAME_TRANSFER_ENCODING,
                            "Session-Name",
                            "Content-Disposition",
                            HTTPCli_FIELD_NAME_DATE,
                            HTTPCli_FIELD_NAME_VIA,
                            HTTPCli_FIELD_NAME_CONTENT_TYPE,
                            HTTPCli_FIELD_NAME_SERVER,
                            "Set-Cookie",
                            "X-Client-IP",
                            "X-Global-Transaction-ID",
                            "X-DP-Watson-Trans-ID",
                                     NULL
                         };

    /* Read HTTP POST request status code */
    lRetVal = HTTPCli_getResponseStatus(httpClient);
    if(lRetVal > 0)
    {
        switch(lRetVal)
        {
        case 100:
        {
#if ENABLE_DEBUG_OUTPUT
            UART_PRINT("HTTP Status 100\n\r");
#endif
            /*
            Set response header fields to filter response headers. All
            other than set by this call we be skipped by library.
            */
            HTTPCli_setResponseFields(httpClient, (const char **)ids);

            /* Read filter response header and take appropriate action. */
            /* Note:
                1. id will be same as index of fileds in filter array setted
                in previous HTTPCli_setResponseFields() call.

                2. moreFlags will be set to 1 by HTTPCli_getResponseField(), if  field
                value could not be completely read. A subsequent call to
                HTTPCli_getResponseField() will read remaining field value and will
                return HTTPCli_FIELD_ID_DUMMY. Please refer HTTP Client Libary API
                documenation @ref HTTPCli_getResponseField for more information.
            */
            while((id = HTTPCli_getResponseField(httpClient, (char *)g_buff, sizeof(g_buff), &moreFlags))
                    != HTTPCli_FIELD_ID_END)
            {
                switch(id)
                {
                case 0:
                {
#if ENABLE_DEBUG_OUTPUT
                    UART_PRINT("X-Note");
                    UART_PRINT(":");
                    UART_PRINT("%s\n\r",g_buff);
#endif
                }
                break;
                default:
                {
                    UART_PRINT("Wrong filter id\n\r");
                    lRetVal = -1;
                    goto end;
                }
                }
            }
            bytesRead = 0;
            if(len > sizeof(g_buff))
            {
#if ENABLE_DEBUG_OUTPUT
                UART_PRINT("Using dynamically allocated buffer\n\r");
#endif
                dataBuffer = (char *) malloc(len);
                bufferIsDynamicallyAllocated = true;
                if(dataBuffer)
                {
                    UART_PRINT("Failed to allocate memory\n\r");
                    lRetVal = -1;
                    goto end;
                }
            }
            else
            {
#if ENABLE_DEBUG_OUTPUT
                UART_PRINT("Using global buffer\n\r");
#endif
                dataBuffer = (char *)g_buff;
            }

            /* Read response data/body */
            /* Note:
                moreFlag will be set to 1 by HTTPCli_readResponseBody() call, if more
                data is available Or in other words content length > length of buffer.
                The remaining data will be read in subsequent call to HTTPCli_readResponseBody().
                Please refer HTTP Client Libary API documenation @ref HTTPCli_readResponseBody
                for more information

            */
#if ENABLE_DEBUG_OUTPUT
            UART_PRINT("Passed step 1\n\r");
#endif
            bytesRead = HTTPCli_readResponseBody(httpClient, (char *)dataBuffer, len, &moreFlags);
#if ENABLE_DEBUG_OUTPUT
            UART_PRINT("Passed readResponseBody\n\r");
#endif
            if(bytesRead < 0)
            {
                UART_PRINT("Failed to receive response body\n\r");
                lRetVal = bytesRead;
                goto end;
            }
            else if( bytesRead >= 0 && moreFlags)
            {
                UART_PRINT("Response body is longer than MAX_BUFF_SIZE\n\r");
                lRetVal = bytesRead;
                goto end;
            }
            dataBuffer[bytesRead] = '\0';

            if(json)
            {
                /* Parse JSON data */
                lRetVal = ParseJSONData(dataBuffer);
                if(lRetVal < 0)
                {
                    goto end;
                }
            }
            else
            {
                /* treating data as a plain text */
            }
        }
        break;

        case 200:
        {
#if ENABLE_DEBUG_OUTPUT
            UART_PRINT("HTTP Status 200\n\r");
#endif
            /*
                 Set response header fields to filter response headers. All
                  other than set by this call we be skipped by library.
             */
            HTTPCli_setResponseFields(httpClient, (const char **)ids);

            /* Read filter response header and take appropriate action. */
            /* Note:
                    1. id will be same as index of fileds in filter array setted
                    in previous HTTPCli_setResponseFields() call.

                    2. moreFlags will be set to 1 by HTTPCli_getResponseField(), if  field
                    value could not be completely read. A subsequent call to
                    HTTPCli_getResponseField() will read remaining field value and will
                    return HTTPCli_FIELD_ID_DUMMY. Please refer HTTP Client Libary API
                    documenation @ref HTTPCli_getResponseField for more information.
             */
            while((id = HTTPCli_getResponseField(httpClient, (char *)g_buff, sizeof(g_buff), &moreFlags))
                    != HTTPCli_FIELD_ID_END)
            {

                switch(id)
                {
                case 0: /* X-Note */
                {
#if ENABLE_DEBUG_OUTPUT
                    UART_PRINT("X-Note");
                    UART_PRINT(":");
                    UART_PRINT("%s\n\r", g_buff);
#endif
                }
                break;
                case 1: /* X-Backside-Transport */
                {
#if ENABLE_DEBUG_OUTPUT
                    UART_PRINT("X-Backside-Transport");
                    UART_PRINT(":");
                    UART_PRINT("%s\n\r", g_buff);
#endif
                }
                break;
                case 2: /* HTTPCli_FIELD_NAME_CONNECTION */
                {
#if ENABLE_DEBUG_OUTPUT
                    UART_PRINT(HTTPCli_FIELD_NAME_CONNECTION);
                    UART_PRINT(":");
                    UART_PRINT("%s\n\r", g_buff);
#endif
                }
                break;
                case 3: /* HTTPCli_FIELD_NAME_TRANSFER_ENCODING */
                {
#if ENABLE_DEBUG_OUTPUT
                    UART_PRINT(HTTPCli_FIELD_NAME_TRANSFER_ENCODING);
                    UART_PRINT(":");
                    UART_PRINT("%s\n\r", g_buff);
#endif
                }
                break;
                case 4: /* "Session-Name" */
                {
#if ENABLE_DEBUG_OUTPUT
                    UART_PRINT("Session-Name");
                    UART_PRINT(":");
                    UART_PRINT("%s\n\r", g_buff);
#endif
                }
                break;
                case 5: /* "Content-Disposition" */
                {
#if ENABLE_DEBUG_OUTPUT
                    UART_PRINT("Content-Disposition");
                    UART_PRINT(":");
                    UART_PRINT("%s\n\r", g_buff);
#endif
                }
                break;
                case 6: /* HTTPCli_FIELD_NAME_DATE */
                {
#if ENABLE_DEBUG_OUTPUT
                    UART_PRINT(HTTPCli_FIELD_NAME_DATE);
                    UART_PRINT(":");
                    UART_PRINT("%s\n\r", g_buff);
#endif
                }
                break;
                case 7: /* HTTPCli_FIELD_NAME_VIA */
                {
#if ENABLE_DEBUG_OUTPUT
                    UART_PRINT(HTTPCli_FIELD_NAME_VIA);
                    UART_PRINT(":");
                    UART_PRINT("%s\n\r", g_buff);
#endif
                }
                break;
                case 8: /* HTTPCli_FIELD_NAME_CONTENT_TYPE */
                {
#if ENABLE_DEBUG_OUTPUT
                    UART_PRINT(HTTPCli_FIELD_NAME_CONTENT_TYPE);
                    UART_PRINT(":");
                    UART_PRINT("%s\n\r", g_buff);
#endif
                    if(!strncmp((const char *)g_buff, "application/json",
                            sizeof("application/json")))
                    {
                        json = 1;
                    }
                    else
                    {
                        /* Note:
                                Developers are advised to use appropriate
                                content handler. In this example all content
                                type other than json are treated as plain text.
                         */
                        json = 0;
                    }
                }
                break;
                case 9: /* HTTPCli_FIELD_NAME_SERVER */
                {
#if ENABLE_DEBUG_OUTPUT
                    UART_PRINT(HTTPCli_FIELD_NAME_SERVER);
                    UART_PRINT(":");
                    UART_PRINT("%s\n\r", g_buff);
#endif
                }
                break;
                case 10: /* "Set-Cookie" */
                {
#if ENABLE_DEBUG_OUTPUT
                    UART_PRINT("Set-Cookie");
                    UART_PRINT(":");
                    UART_PRINT("%s\n\r", g_buff);
#endif
                }
                break;
                case 11: /* "X-Client-IP" */
                {
#if ENABLE_DEBUG_OUTPUT
                    UART_PRINT("X-Client-IP");
                    UART_PRINT(":");
                    UART_PRINT("%s\n\r", g_buff);
#endif
                }
                break;
                case 12: /* "X-Global-Transaction-ID" */
                {
#if ENABLE_DEBUG_OUTPUT
                    UART_PRINT("X-Global-Transaction-ID");
                    UART_PRINT(":");
                    UART_PRINT("%s\n\r", g_buff);
#endif
                }
                break;
                case 13: /* "X-DP-Watson-Trans-ID" */
                {
#if ENABLE_DEBUG_OUTPUT
                    UART_PRINT("X-DP-Watson-Trans-ID");
                    UART_PRINT(":");
                    UART_PRINT("%s\n\r", g_buff);
#endif
                }
                break;
                default:
                {
                    UART_PRINT("Wrong filter id\n\r");
                    lRetVal = -1;
                    goto end;
                }
                }
            }
            bytesRead = 0;
            dataBuffer = (char *)g_buff;

            /* Read response data/body */
            /* Note:
                    moreFlag will be set to 1 by HTTPCli_readResponseBody() call, if more
                    data is available Or in other words content length > length of buffer.
                    The remaining data will be read in subsequent call to HTTPCli_readResponseBody().
                    Please refer HTTP Client Libary API documenation @ref HTTPCli_readResponseBody
                    for more information

             */
            bytesRead = HTTPCli_readResponseBody(httpClient, (char *)dataBuffer, len, &moreFlags);
            while(moreFlags){
                if(bytesRead>0){ //If there was no error or no timeout, increment pointer by bytes read
                    dataBuffer += bytesRead;
                }
                bytesRead = HTTPCli_readResponseBody(httpClient, (char *)dataBuffer, len, &moreFlags);
            }
            if(bytesRead < 0 && dataBuffer==(char *)g_buff)
            {
                UART_PRINT("Failed to receive response body\n\r");
                lRetVal = bytesRead;
                goto end;
            }
            //else if( bytesRead >= len)
            else if( (dataBuffer - (char *)g_buff) >= len)
            {
                UART_PRINT("Response body is longer than MAX_BUFF_SIZE\n\r");
                lRetVal = bytesRead;
                goto end;
            }
            dataBuffer[bytesRead] = '\0';
            dataBuffer = (char *)g_buff;
            if(json)
            {
                /* Parse JSON data */
                lRetVal = ParseJSONData(dataBuffer);
                if(lRetVal < 0)
                {
                    UART_PRINT("%s\n\r", dataBuffer);
                    goto end;
                }
            }
            else
            {
                /* treating data as a plain text */
            }

        }
        break;

        case 404:
            UART_PRINT("File not found. \r\n");
            /* Handle response body as per requirement.
                  Note:
                    Developers are advised to take appropriate action for HTTP
                    return status code else flush the response body.
                    In this example we are flushing response body in default
                    case for all other than 200 HTTP Status code.
             */
        default:
            /* Note:
              Need to flush received buffer explicitly as library will not do
              for next request. Application is responsible for reading all
              data.
             */
            FlushHTTPResponse(httpClient);
            break;
        }
    }
    else
    {
        if(lRetVal!=-104){
            UART_PRINT("Failed to receive data from server. Error code %i\r\n",lRetVal);
        }
        goto end;
    }

    lRetVal = 0;

end:
#if ENABLE_DEBUG_OUTPUT
    UART_PRINT("Got to the end of read response\n\r");
#endif
    if(dataBuffer != NULL&&bufferIsDynamicallyAllocated)
    {
        free(dataBuffer);
        UART_PRINT("Successfully freed memory\n\r");
        }
#if ENABLE_DEBUG_OUTPUT
    UART_PRINT("About to return from read response\n\r");
#endif
    return lRetVal;
}
//*****************************************************************************
//
//! \brief HTTP POST Demonstration
//!
//! \param[in]  httpClient - Pointer to http client
//!
//! \return 0 on success else error code on failure
//!
//*****************************************************************************
int HTTPPostMethod(HTTPCli_Handle httpClient, int postSettings, IBMTestPacket* packetToPost)
{
    unsigned char *pucFileBuffer = NULL;        // Data read or to be written
    unsigned long chunkSize = MAX_BUFF_SIZE;
    long pFileHandle;               // Pointer to file handle
    SlFsFileInfo_t pFsFileInfo;
    char *FileRead = FILE_NAME;             // File to be read
    int fileSize = 0;
    char s_fileSize[15];
    int uiFileSize;
    int i;

    bool moreFlags = 0;
    long lRetVal = 0;

    pucFileBuffer = malloc(chunkSize);
        if(NULL == pucFileBuffer)
        {
            UART_PRINT("Can't Allocate Resources\r\n");
            LOOP_FOREVER();
        }

        memset(pucFileBuffer,'\0',chunkSize);
        memset(s_fileSize,'\0',15);
        /* Read file size if we are posting a file */
        if(postSettings==HTTP_POST_FLACFILE){
            /* open audio file */
            pFileHandle = sl_FsOpen((unsigned char *)FileRead,SL_FS_READ, \
                NULL);
            if(lRetVal < 0)
            {
                free(pucFileBuffer);
                ERR_PRINT(lRetVal);
                LOOP_FOREVER();
            }

            lRetVal = sl_FsGetInfo((unsigned char *)FileRead, NULL, &pFsFileInfo);
            if(lRetVal < 0)
            {
                lRetVal = sl_FsClose(pFileHandle,0,0,0);
                free(pucFileBuffer);
                ERR_PRINT(lRetVal);
                LOOP_FOREVER();
            }

            uiFileSize = (&pFsFileInfo)->Len;
            sprintf(s_fileSize, "%d", uiFileSize);
        }
        /* Get stream size from packet otherwise */
        else if((postSettings==HTTP_POST_RAW)||(postSettings==HTTP_POST_RAW_2CH)){
            uiFileSize = packetToPost->bytesInBuffer;
            sprintf(s_fileSize, "%d", packetToPost->bytesInBuffer);
        }
        HTTPCli_Field fields[7] = {
#if READ_API_KEY_FROM_FILE
                                   {HTTPCli_FIELD_NAME_AUTHORIZATION, &APIKey},
#else
                                    {HTTPCli_FIELD_NAME_AUTHORIZATION, AUTHENTICATION},
#endif
                                    {HTTPCli_FIELD_NAME_HOST, HOST_NAME},
                                {HTTPCli_FIELD_NAME_ACCEPT, "*/*"},
                                {HTTPCli_FIELD_NAME_CONTENT_TYPE, CONTENT_TYPE},
                                {HTTPCli_FIELD_NAME_CONTENT_LENGTH, s_fileSize},
                                {HTTPCli_FIELD_NAME_EXPECT, "100-continue"},
                                    {NULL,NULL}
                            };
//        if(READ_API_KEY_FROM_FILE){
//            fields[0].value = APIKey;
//        }
        if(postSettings==HTTP_POST_RAW){
            fields[3].value = CONTENT_TYPE_RAW;
        }
        else if(postSettings==HTTP_POST_RAW_2CH){
            fields[3].value = CONTENT_TYPE_RAW_2CH;
        }
    /* Set request header fields to be sent for HTTP request. */
        HTTPCli_setRequestFields(httpClient, fields);

        /* Send POST method request. */
        lRetVal = HTTPCli_sendRequest(httpClient, HTTPCli_METHOD_POST, POST_REQUEST_URI, moreFlags);
        if(lRetVal < 0)
        {
            UART_PRINT("Failed to send HTTP POST request header.\n\r");
            return lRetVal;
        }
        if(postSettings==HTTP_POST_FLACFILE){
        for (i = 0; i < uiFileSize; i = i + fileSize){

            fileSize = sl_FsRead(pFileHandle, i,  pucFileBuffer, chunkSize);
            if(lRetVal < 0)
            {
                lRetVal = sl_FsClose(pFileHandle,0,0,0);
                free(pucFileBuffer);
                ERR_PRINT(lRetVal);
                LOOP_FOREVER();
            }

            /* Send POST data/body */
            lRetVal = HTTPCli_sendRequestBody(httpClient, (const char *)pucFileBuffer, fileSize);

            if(lRetVal < 0)
            {
                UART_PRINT("Failed to send HTTP POST request body.\n\r");
                    return lRetVal;
            }
        }
        }
        else if((postSettings==HTTP_POST_RAW)||(postSettings==HTTP_POST_RAW_2CH)){
            lRetVal = HTTPCli_sendRequestBody(httpClient, packetToPost->bufferPointer, packetToPost->bytesInBuffer);

            if(lRetVal < 0)
            {
                UART_PRINT("Failed to send HTTP POST request body.\n\r");
                return lRetVal;
            }
        }

        free(pucFileBuffer);
        if(postSettings==HTTP_POST_FLACFILE){
        lRetVal = sl_FsClose(pFileHandle,0,0,0);
        }
        lRetVal = readResponse(httpClient);
        if(lRetVal < 0)
        {
            UART_PRINT("Failed to read response.\n\r");
            return lRetVal;
        }
#if ENABLE_DEBUG_OUTPUT
        UART_PRINT("First Response Received.Waiting for second response...\n\r");
#endif
        int failcounter = 0;
        lRetVal = readResponse(httpClient);
        while(lRetVal<0&&failcounter<HTTP_RETRY_LIMIT){
            //UART_PRINT("Failed to read response.\n\r");
            failcounter++;
            lRetVal = readResponse(httpClient);
        }
        return lRetVal;
}
//*****************************************************************************
//
//! Function to connect to HTTP server
//!
//! \param  httpClient - Pointer to HTTP Client instance
//!
//! \return Error-code or SUCCESS
//!
//*****************************************************************************
int ConnectToHTTPServer(HTTPCli_Handle httpClient, HTTPCli_Params *params)
{
    long lRetVal = -1;
    struct SlSockAddrIn_t addr;


#if USE_PROXY
    struct sockaddr_in paddr;
    paddr.sin_family = AF_INET;
    paddr.sin_port = htons(PROXY_PORT);
    paddr.sin_addr.s_addr = sl_Htonl(PROXY_IP);
    HTTPCli_setProxy((struct sockaddr *)&paddr);
#endif

    /* Resolve HOST NAME/IP */
    lRetVal = sl_NetAppDnsGetHostByName((signed char *)HOST_NAME,
                                          strlen((const char *)HOST_NAME),
                                          &g_ulDestinationIP,SL_AF_INET);
    if(lRetVal < 0)
    {
        ASSERT_ON_ERROR(GET_HOST_IP_FAILED);
    }

    /* Set up the input parameters for HTTP Connection */
    addr.sin_family = SL_AF_INET;
    addr.sin_port = sl_Htons(HOST_PORT);
    addr.sin_addr.s_addr = sl_Htonl(g_ulDestinationIP);

    /* Testing HTTPCli open call: handle, address params only */
    HTTPCli_construct(httpClient);
    lRetVal = HTTPCli_connect(httpClient, (struct SlSockAddr_t *)&addr, HTTPCli_TYPE_TLS, params);
    if (lRetVal < 0)
    {
        UART_PRINT("Connection to server failed. error(%d)\n\r", lRetVal);
        ASSERT_ON_ERROR(SERVER_CONNECTION_FAILED);
    }
    else
    {
        UART_PRINT("Connection to server created successfully\r\n");
        g_intNetworkState = NETWORK_STATE_CONNECTED;
    }

    return 0;
}
//*****************************************************************************
//
//! Function to disconnect from HTTP server
//!
//! \param  httpClient - Pointer to HTTP Client instance
//!
//! \return Error-code or SUCCESS
//!
//*****************************************************************************
void DisconnectFromHTTPServer(HTTPCli_Handle httpClient){

    HTTPCli_disconnect(httpClient);
}
//*****************************************************************************
//
//! Helper function to update the current confidence level with the next occurrence following the string "confidence":
//!
//! \param  jsondata - Pointer to the jsonData char array
//! \param  currentMinConfidence - Pointer to the current minimum confidence level float
//!
//! \return Error-code or SUCCESS
//!
//*****************************************************************************
int getConfidenceFromJSONString(char ** jsonData, float *currentMinConfidence)
{
    char *currentPosition;
    currentPosition = strstr(*jsonData, "\"confidence\":");
    if (currentPosition == NULL)
    {
        return -1; //Did not find any confidence value in the JSON string
    }
    else
    {
        currentPosition += 13; //Increment pointer past the "confidence": string
        float temp = atof(currentPosition);
        if (*currentMinConfidence > temp)
        {
            *currentMinConfidence = temp;

        }
        *jsonData += 13;
    }
    return 0;
}
//*****************************************************************************
//
//! Helper function to update the current transcript with the next transcript data following the string "transcript":
//!
//! \param  jsonData - Pointer to the jsonData char array
//! \param  currentTranscript - Pointer to the memory location to write transcript
//! \param  spaceLeftInTranscript - Int with number of bytes of space in currentTranscript
//!
//! \return Error-code or SUCCESS
//!
//*****************************************************************************
int getTranscriptFromJSONString(char ** jsonData, char * currentTranscript,
                                int spaceLeftInTranscript)
{
    char * currentPosition;
    char * startPosition;
    char * endPosition;
    int size;
    currentPosition = strstr(*jsonData, "\"transcript\":");
    if (currentPosition == NULL)
    {
        return -1; //Did not find any confidence value in the JSON string
    }
    else
    {
        currentPosition += 13; //Increment pointer past the "transcript": string
        startPosition = strstr(currentPosition, "\""); //find opening quote
        endPosition = strstr(startPosition + 1, "\""); //find ending quote
        if (startPosition == NULL || endPosition == NULL)
        {
            return -2; //JSON string is malformed or cutoff
        }
        else
        {
            size = endPosition - startPosition - 1; //We don't want the quotes
            if (size < spaceLeftInTranscript)
            {
                memcpy(currentTranscript, (startPosition + 1), size);
                *jsonData = endPosition;
                return size;
            }
            else
            {
                memcpy(currentTranscript, (startPosition + 1),
                       spaceLeftInTranscript);
                *jsonData = endPosition;
                return spaceLeftInTranscript;
            }
        }
    }
}
