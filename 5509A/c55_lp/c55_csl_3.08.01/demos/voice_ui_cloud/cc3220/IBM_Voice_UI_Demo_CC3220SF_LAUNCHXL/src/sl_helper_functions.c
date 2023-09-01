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

/** \file sl_helper_functions.c Implements helper functions used for NWP interaction.\n (C)
 *  Copyright 2017, Texas Instruments, Inc.
 */



#include "sl_helper_functions.h"
extern int g_intTimeout;
extern uint32_t g_ulStatus;
int32_t InitSimplelink(uint8_t const role)
{
    int32_t retVal = -1;

    retVal = sl_Start(0, 0, 0);

    if(role == retVal)
    {
        UART_PRINT("SimpleLinkInitCallback: started in role %d\n\r", role);
        SignalEvent(APP_EVENT_STARTED);
    }
    else
    {
        UART_PRINT("SimpleLinkInitCallback: started in role %d, set the requested role %d\n\r", retVal, role);
        retVal = sl_WlanSetMode(role);
        ASSERT_ON_ERROR(retVal);
        retVal = sl_Stop(SL_STOP_TIMEOUT);
        ASSERT_ON_ERROR(retVal);
        retVal = sl_Start(0, 0, 0);
        ASSERT_ON_ERROR(retVal);
        if (role != retVal)
        {
            UART_PRINT("SimpleLinkInitCallback: error setting role %d, status=%d\n\r", role, retVal);
            SignalEvent(APP_EVENT_ERROR);
        }
        UART_PRINT("SimpleLinkInitCallback: restarted in role %d\n\r", role);
        SignalEvent(APP_EVENT_STARTED);
    }

    /* Start timer */
    g_intTimeout = ASYNC_EVT_TIMEOUT;
    retVal = StartAsyncEvtTimer();
    ASSERT_ON_ERROR(retVal);

    return retVal;
}
int32_t WlanConnect()
{
    SlWlanSecParams_t secParams = {0};
    long lRetVal = 0;

    secParams.Key = (signed char *)SECURITY_KEY;
    secParams.KeyLen = strlen(SECURITY_KEY);
    secParams.Type = SECURITY_TYPE;

    lRetVal = sl_WlanConnect((signed char *)SSID_NAME,
                           strlen((const char *)SSID_NAME), 0, &secParams, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Wait for WLAN Event
    while((!IS_CONNECTED(g_ulStatus)) || (!IS_IP_ACQUIRED(g_ulStatus)))
    {
        // wait till connects to an AP, block for 1 sec at a time
        UART_PRINT(".@");
        Task_sleep(1000);
    }

    return 0;

}
