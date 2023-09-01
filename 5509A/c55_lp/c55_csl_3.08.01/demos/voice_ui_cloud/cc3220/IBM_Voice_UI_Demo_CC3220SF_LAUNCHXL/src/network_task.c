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

/** \file network_task.c Implements the wifi interaction task, and calls the HTTP handling functions.\n (C)
 *  Copyright 2017, Texas Instruments, Inc.
 */
/*----------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------*/
#include "network_task.h"
#include "httphandler.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "leds.h"
#include "lcd.h"

#if USE_PROVISIONING
#include "provisioning_task.h"
#endif

//Set IBM_NET_TEST to 1 in order to execute a test send to IBM with static data to verify API functionality
//Need to verify correct filename and other vars in httpvars.h
#define IBM_NET_TEST 0
/*----------------------------------------------------------------------------
 * Globals
 * -------------------------------------------------------------------------*/
//Semaphore to unblock processing for wifi task
Semaphore_Struct wifiSemStruct;
Semaphore_Handle wifiSemHandle;
//Semaphore to indicate end of HTTP handling to recognition module
extern Semaphore_Handle recogSemHandle;
int g_intNetworkState = NETWORK_STATE_NULL;
extern int g_intSlStatus;
uint32_t g_ulStatus;

#if READ_API_KEY_FROM_FILE
//Char array to store API Key
char APIKey[256] = {0};
#endif

//Struct with data to send to IBM
IBMTestPacket packetToWifi = {0};
/*----------------------------------------------------------------------------
 * Functions
 * --------------------------------------------------------------------------*/
/* Function to test wifi connectivity by pinging google.com */
void pingGoogleTest();

/*-------------------------------------------------------------------------*/
void * networkTaskThread( void *pvParameters ){
    int iRetVal = 0;
    g_ulStatus = 0;
    bool done = false;
    /* Setup network semaphore */
    Semaphore_Params semParams;
    Semaphore_Params_init(&semParams);
    Semaphore_construct(&wifiSemStruct,0,&semParams);
    wifiSemHandle = Semaphore_handle(&wifiSemStruct);

    /* Perform setup of NWP*/
#if !USE_PROVISIONING
    iRetVal = InitSimplelink(ROLE_STA);
    if(iRetVal<0){
        UART_PRINT("Fatal Error in InitSimplelink \n\r");
        LOOP_FOREVER();
    }
    /* Load Watson API Key into application if needed */
#if READ_API_KEY_FROM_FILE
    SlFsFileInfo_t fileInfo;
    int fd;
    iRetVal = sl_FsGetInfo(API_KEY_FILENAME,NULL,&fileInfo);
    if(iRetVal<0){
        UART_PRINT("Error #%i reading API Key file! Please double-check file exists with filename %s \n\r",iRetVal,API_KEY_FILENAME);
        while(1);
    }
    fd = sl_FsOpen(API_KEY_FILENAME, SL_FS_READ,0);
    if(fd<0){
        UART_PRINT("Error #%i reading API Key file! Please double-check file exists with filename %s \n\r",fd,API_KEY_FILENAME);
        while(1);
    }
    iRetVal = sl_FsRead(fd,0,(_u8*)APIKey,fileInfo.Len);
    if(iRetVal<0){
        UART_PRINT("Error #%i reading API Key file! Please double-check file exists with filename %s \n\r",iRetVal,API_KEY_FILENAME);
        while(1);
    }
    if(iRetVal!=fileInfo.Len){
        UART_PRINT("Did not complete file read!\n\r");
    }
    iRetVal = sl_FsClose(fd,0,0,0);
    if(iRetVal<0){
        UART_PRINT("Error #%i reading API Key file! Please double-check file exists with filename %s \n\r",fd,API_KEY_FILENAME);
        while(1);
    }
    else{
        UART_PRINT("IBM Watson API Key:\n\r%s\n\rloaded successfully!\n\r",APIKey);
    }
#endif
    /*Connect to AP */
    iRetVal = WlanConnect();
#else
    sem_wait(&Provisioning_ControlBlock.provisioningConnDoneToOtaServerSignal);
#endif
    UART_PRINT("Ready to perform network task \n\r");
    /* Ping to test connection */
    //pingGoogleTest();

    /* IBM transcription test */
    /* Declare the HTTP objects needed for the HTTP client library */
    HTTPCli_Struct httpClient;
    TLS_Handle     tlsHandle;
    HTTPCli_Params httpCliParams;
    HTTPCli_Params_init(&httpCliParams);
    tlsHandle = TLS_create(TLS_METHOD_CLIENT_TLSV1_2);
    if(tlsHandle == NULL){
        UART_PRINT("Failed to create TLS context\n\r");
        while(1){
            //spin forever
        }
    }
    iRetVal = TLS_setCertFile(tlsHandle,TLS_CERT_TYPE_CA,TLS_CERT_FORMAT_DER,"geotrustglobalca.der");
    if(iRetVal!=0){
        UART_PRINT("Failed to set TLS cert file\n\r");
        while(1){

        }
    }
    httpCliParams.tls = tlsHandle;
    /* Set the security parameters of the device so that TLS server cert verification can be performed */
    SlDateTime_t dateTime;
    dateTime.tm_day = DEVICE_DATE;
    dateTime.tm_mon = DEVICE_MONTH;
    dateTime.tm_year = DEVICE_YEAR;
    dateTime.tm_hour = 0;
    dateTime.tm_min = 0;
    dateTime.tm_sec = 0;
    sl_DeviceSet(SL_DEVICE_GENERAL, SL_DEVICE_GENERAL_DATE_TIME, sizeof(SlDateTime_t), (uint8_t *)(&dateTime));
    Task_sleep(1000);

#if IBM_NET_TEST
    iRetVal = ConnectToHTTPServer(&httpClient,&httpCliParams);
    if(iRetVal < 0)
    {
        LOOP_FOREVER();
    }
    UART_PRINT("\n\r");
    UART_PRINT("HTTP Post Begin:\n\r");
    iRetVal = HTTPPostMethod(&httpClient,HTTP_POST_FLACFILE, &packetToWifi);
    if(iRetVal < 0)
    {
        UART_PRINT("HTTP Post failed.\n\r");
    }
    DisconnectFromHTTPServer(&httpClient);
    UART_PRINT("HTTP Post End:\n\r");
#endif
    while(!done){
        //Block until there is a packet of audio data for the wifi task to send
        Semaphore_pend(wifiSemHandle, BIOS_WAIT_FOREVER);
        iRetVal = ConnectToHTTPServer(&httpClient,&httpCliParams);
        if(iRetVal < 0)
        {
            UART_PRINT("Something went wrong with HTTP socket connection.\n\r");
            LOOP_FOREVER();
        }
#if ENABLE_DEBUG_OUTPUT
        UART_PRINT("\n\r");
        UART_PRINT("HTTP Post Begin:\n\r");
#endif
#if USE_2CH_AUDIO
        iRetVal = HTTPPostMethod(&httpClient,HTTP_POST_RAW_2CH, &packetToWifi);
#else
        iRetVal = HTTPPostMethod(&httpClient,HTTP_POST_RAW, &packetToWifi);
#endif
        if(iRetVal < 0)
        {
            UART_PRINT("HTTP Post failed.\n\r");
        }
#if ENABLE_DEBUG_OUTPUT
        UART_PRINT("HTTP Post End:\n\r");
#endif
        LCD_showMessage("Done Sending");
        Leds_off1();
        DisconnectFromHTTPServer(&httpClient);
        if(packetToWifi.postOnCompletion){
            Semaphore_post(recogSemHandle);
        }
    }
    return(0);
}
void pingGoogleTest(){
    /* Ping to test connection */
    int iRetVal = 0;
    SlNetAppPingCommand_t pingParams = {0};
    SlNetAppPingReport_t pingReport = {0};
    pingParams.PingIntervalTime = PING_INTERVAL;
    pingParams.PingSize = PING_PKT_SIZE;
    pingParams.PingRequestTimeout = PING_TIMEOUT;
    pingParams.TotalNumberOfAttempts = NO_OF_ATTEMPTS;
    pingParams.Flags = 0;
    pingParams.Ip = 0x8080808;
    iRetVal = sl_NetAppPing((SlNetAppPingCommand_t*)&pingParams,\
                             SL_AF_INET, (SlNetAppPingReport_t*)&pingReport,\
                             SimpleLinkPingReport);
    ASSERT_ON_ERROR(iRetVal);
    UART_PRINT("Pinging 8.8.8.8 ...!");
    UART_PRINT("\n\r");
}
