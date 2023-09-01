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

/** \file cc5517ListeningMenu.c Implements the listening menu and operations it invokes.\n (C)
 *  Copyright 2017, Texas Instruments, Inc.
 */

/*----------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//#include "msp.h"

/* APIs used in testing menu */
#include "menus.h"

#include "audio_collect.h"
#include "leds.h"
#include "lcd.h"

#include "uart_term.h"
#include "Board.h"
#include "httphandler.h"
#include "appConfig.h"
#if UART_SYNC_IN_USE
#include "ti/devices/cc32xx/driverlib/utils.h"
#endif
#if USE_CONTINUOUS_UART_RECORDING
extern UART_Handle uartHandle1;
#endif
/*----------------------------------------------------------------------------
 * Defines and Typedefs
 * -------------------------------------------------------------------------*/
//Probably don't need this
typedef enum
{
    Audio_UART_Test,
    Audio_IBM_Test,

}e_ListeningModes;
/* Recognize menu state structure*/
typedef struct ListeningStateStruct
{
    int_least16_t framesProcessed;
    bool running;
    int listeningMode;

} listeningStateObject;

void SendTranscript(const char *str);

/*----------------------------------------------------------------------------
 * Globals
 * -------------------------------------------------------------------------*/

/* Handles for manipulating audio and stream collection */
extern Audio_Handle Audio;

/* Other menus used */
//Probably don't need this
extern Menu_Object MainMenu;

/* Handles for manipulating wifi semaphore */
extern Semaphore_Handle wifiSemHandle;

/* Handle for manipulating recognize semaphore */
extern Semaphore_Handle recogSemHandle;

/* Handle for manipulating main task semaphore */
extern Semaphore_Handle mainSemHandle;

/* Packet of data to modify for wifi thread */
extern IBMTestPacket packetToWifi;
/* Listening menu defined */
/* This menu is blank */
static char const * const ListeningItems[] =
{

};

/* Testing event handling function declared */
Menu_Handle ListeningMenu_processEvent(Events event);

/* Update menu instance */
Menu_Object ListeningMenu =
{
    NULL,
    ListeningItems,
    ListeningMenu_processEvent,
    0,
    0
};

/* Recognize menu state instance */
static listeningStateObject listeningState =
{
    0,
    false,
    0
};
extern uint16_t g_audioBuffer;

extern char g_charTranscript;
extern char g_charConfidenceLevel;

int g_intServerResponded = 0;
#if USE_CONTINUOUS_UART_RECORDING
extern int toggleRecordingSend;
#endif
/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/
Menu_Handle ListeningMenu_processEvent(Events event)
{
    int_least16_t *audioFrame;

    switch(event)
    {
        case EVENT_BUTTON1:
        {
        //Do nothing, no option to select, no menu to escape to
        }
        break;


        case EVENT_BUTTON2:
        {
            //Only start listening if we were not already listening - ignore this event if so
            if(!listeningState.running)
            {
                /* Start audio collection into global buffer, to send over wifi to IBM for processing once stopped */
                Audio_startCollect16KHz(Audio);
                listeningState.running = true;
                listeningState.listeningMode = Audio_IBM_Test;
                listeningState.framesProcessed = 0;
                LCD_showMessage(">> Recording");
                UART_PRINT("Starting audio recording!\n\r");
                Leds_on1();
            }
        }
        break;


        case EVENT_AUDIOFRAME:
        {
            if(listeningState.running)
            {
#if UART_SYNC_IN_USE
                /* First check if UART needs reset due to data corruption */
            if (Audio_getUARTResetRequired(Audio))
            {
#if !USE_CONTINUOUS_UART_RECORDING
                //Wait 2.6ms to ensure UART burst is complete, then restart UART interface
                UtilsDelay(35000);
                Audio_stopCollect16KHz(Audio);
                Audio_startCollect16KHz(Audio);
#else
                //Wait 2.6ms to ensure UART burst is complete, then flush FIFO and restart recording
                UtilsDelay(35000);
                Audio_stopCollect16KHz(Audio);
                Audio_flushFIFO(Audio);
                Audio_startCollect16KHz(Audio);
#endif
            }
            else
            {
                /* Dump next frame of voice data. Display status
                 * based on result of processing. */
                uint16_t* pointerToAudioBuffer;
                audioFrame = Audio_getActiveBuffer(Audio);
                if (listeningState.framesProcessed < MAX_FRAMES)
                {
                    pointerToAudioBuffer = &g_audioBuffer
                            + 160 * listeningState.framesProcessed;
                    memcpy(pointerToAudioBuffer, (audioFrame+2), 320);
                    listeningState.framesProcessed++;
                    //If buffer is full, then warn user
                    if(listeningState.framesProcessed==MAX_FRAMES){
                        Leds_on2();
                        LCD_showMessage("Buffer full!");
                        UART_PRINT("Buffer full! Sending audio data\n\r");
                    }
                }
                else if (listeningState.framesProcessed == MAX_FRAMES)
                {
                    //Buffer is full, so we are done recording data
                    Audio_stopCollect16KHz(Audio);
                    listeningState.running = false;
                    Leds_off1();
                    Leds_setColor2(0, 0, 0);
                    LCD_showMessage(">> Sending");
                    packetToWifi.bufferPointer = (char *) &g_audioBuffer;
                    packetToWifi.bytesInBuffer = 160 * 2
                            * listeningState.framesProcessed;
                    packetToWifi.postOnCompletion = 1;
                    Leds_off2();
                    Leds_off3();
                    Leds_on1();
                    listeningState.framesProcessed = 0;
                    Semaphore_post(wifiSemHandle);
                    Semaphore_pend(recogSemHandle, BIOS_WAIT_FOREVER);
                    LCD_showMenu(&ListeningMenu);
                    if(g_intServerResponded){
                        LCD_showMessageLine(&g_charTranscript, 45);
                        LCD_showMessageLine(&g_charConfidenceLevel, 70);
                        SendTranscript(&g_charTranscript);
                    }
                    else{
                        LCD_showMessageLine("(No Response)", 45);
                        SendTranscript("(No Response)");
                    }
#if USE_CONTINUOUS_UART_RECORDING
                    toggleRecordingSend++;
                    Semaphore_post(mainSemHandle);
#endif
                }
            }

#else //If UART sync is not in use
            /* Dump next frame of voice data. Display status
             * based on result of processing. */
            uint16_t* pointerToAudioBuffer;
            audioFrame = Audio_getActiveBuffer(Audio);
            if(listeningState.framesProcessed<MAX_FRAMES)
            {
                pointerToAudioBuffer = &g_audioBuffer+160*listeningState.framesProcessed;
                memcpy(pointerToAudioBuffer,audioFrame,320);
                listeningState.framesProcessed++;
            }
            else if(listeningState.framesProcessed==MAX_FRAMES)
            {
                //Buffer is full, so we are done recording data
                Audio_stopCollect16KHz(Audio);
                listeningState.running = false;
                Leds_off1();
                Leds_setColor2(0, 0, 0);
                LCD_showMessage(">> Sending");
                packetToWifi.bufferPointer =(char *) &g_audioBuffer;
                packetToWifi.bytesInBuffer = 160*2*listeningState.framesProcessed;
                packetToWifi.postOnCompletion = 1;
                Leds_off2();
                Leds_off3();
                Leds_on1();
                listeningState.framesProcessed = 0;
                Semaphore_post(wifiSemHandle);
                Semaphore_pend(recogSemHandle,BIOS_WAIT_FOREVER);
                LCD_showMenu(&ListeningMenu);
                if(g_intServerResponded)
                {
                    LCD_showMessageLine(&g_charTranscript, 45);
                    LCD_showMessageLine(&g_charConfidenceLevel, 70);
                    SendTranscript(&g_charTranscript);
                }
                else
                {
                    LCD_showMessageLine("(No Response)", 45);
                    SendTranscript("(No Response)");
                }
#if USE_CONTINUOUS_UART_RECORDING
                toggleRecordingSend++;
                Semaphore_post(mainSemHandle);
#endif

            }
#endif
        }

            else
            {
                /* Should not get an audio frame while not recognizing */
                Audio_stopCollect(Audio);
                listeningState.framesProcessed = 0;
                Leds_off1();
                Leds_setColor2(0, 0, 0);

                LCD_showMessage("Dumping Error");
            }
        }
        break;
        case EVENT_TOGGLE_RECORDING:
        {
            //If device wasn't listening on UART previously, start UART recording
            if(!listeningState.running)
            {
                /* Start audio collection into global buffer, to send over wifi to IBM for processing once stopped */
                Audio_startCollect16KHz(Audio);
                listeningState.running = true;
                listeningState.listeningMode = Audio_IBM_Test;
                listeningState.framesProcessed = 0;
                LCD_showMessage(">> Recording");
                UART_PRINT("Starting audio recording!\n\r");
                Leds_on1();

            }
            //If the device was listening on UART, then stop UART recording, invoke IBM Watson API, and then go back to listening state
            else
            {
                UART_PRINT("UART Timeout! Sending audio data\n\r");
                //Stop audio collection
                Audio_stopCollect16KHz(Audio);
                //char buffer[13];
                listeningState.running = false;
                Leds_off1();
                Leds_setColor2(0, 0, 0);
                LCD_showMessage(">> Sending");
                packetToWifi.bufferPointer = (char *) &g_audioBuffer;
                packetToWifi.bytesInBuffer = 160 * 2
                * listeningState.framesProcessed;
                packetToWifi.postOnCompletion = 1;
                Leds_off2();
                Leds_off3();
                Leds_on1();
                listeningState.framesProcessed = 0;
                Semaphore_post(wifiSemHandle);
                Semaphore_pend(recogSemHandle, BIOS_WAIT_FOREVER);
                LCD_showMenu(&ListeningMenu);
                if(g_intServerResponded)
                {
                    LCD_showMessageLine(&g_charTranscript, 45);
                    LCD_showMessageLine(&g_charConfidenceLevel, 70);
                    SendTranscript(&g_charTranscript);
                }
                else
                {
                    LCD_showMessageLine("(No Response)", 45);
                    SendTranscript("(No Response)");
                }
#if USE_CONTINUOUS_UART_RECORDING
                toggleRecordingSend++;
                Semaphore_post(mainSemHandle);
#endif

            }
        }
        break;
    }

    return &ListeningMenu;
}
//Private function to send the transcript received from IBM back to C55X device
//Must have already executed InitTerm() if not in continuous UART recording mode
//Must have already executed Audio_setupCollect() if in continuous UART recording mode
void SendTranscript(const char *str){
#if USE_UART_AUDIO_IN && !USE_UART0
    //Open UART interface
    char outputBuffer[256] = {0};
    strcpy(outputBuffer,str);
#if !USE_CONTINUOUS_UART_RECORDING
    //Open the UART1 interface (which should be closed)
    UART_Params uartParams;
    UART_Handle uartWriteHandle;
    UART_Params_init(&uartParams);
    uartParams.writeDataMode = UART_DATA_BINARY;
    uartParams.readDataMode = UART_DATA_BINARY;
    uartParams.readReturnMode = UART_RETURN_FULL;
    uartParams.readEcho = UART_ECHO_OFF;
    uartParams.baudRate = UART_BAUDRATE;
    uartParams.readMode = UART_MODE_BLOCKING;
    uartParams.writeMode = UART_MODE_BLOCKING;
    uartWriteHandle = UART_open(Board_UART1, &uartParams);
    //Write string, then close UART interface
    UART_write(uartWriteHandle, outputBuffer, 256); //Make sure to send ending null character too
    UART_close(uartWriteHandle);
#else
    //Write string to UART1, which should be already opened by Audio_setupCollect()
    UART_write(uartHandle1, outputBuffer, 256); //Make sure to send ending null character too
#endif
#endif
}
