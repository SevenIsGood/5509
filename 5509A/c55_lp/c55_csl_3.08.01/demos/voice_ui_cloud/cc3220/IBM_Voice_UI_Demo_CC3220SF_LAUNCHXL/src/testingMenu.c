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

/** \file testingMenu.c Defines the testing menu and operations it invokes.\n (C)
 *  Copyright 2017, Texas Instruments, Inc.
 */

/*----------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

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

/*----------------------------------------------------------------------------
 * Defines and Typedefs
 * -------------------------------------------------------------------------*/
typedef enum
{
    Audio_UART_Test,
    Audio_IBM_Test,

}e_TestingModes;
/* Recognize menu state structure*/
typedef struct testingStateStruct
{
    int_least16_t framesProcessed;
    bool running;
    int testmode;

} testingStateObject;

/*----------------------------------------------------------------------------
 * Globals
 * -------------------------------------------------------------------------*/

/* Handles for manipulating audio and stream collection */
extern Audio_Handle Audio;

/* Other menus used */
extern Menu_Object MainMenu;

/* Handles for manipulating wifi semaphore */
extern Semaphore_Handle wifiSemHandle;

/* Handle for manipulating recognize semaphore */
extern Semaphore_Handle recogSemHandle;

/* Packet of data to modify for wifi thread */
extern IBMTestPacket packetToWifi;
/* Testing menu defined */
static char const * const testingItems[] =
{
    "<Menu Up>",
    "Start dump",
    "Start IBM Test",
    //"C5545 Test",
};

/* Testing event handling function declared */
Menu_Handle TestingMenu_processEvent(Events event);

/* Update menu instance */
Menu_Object TestingMenu =
{
    "Testing Menu",
    testingItems,
    TestingMenu_processEvent,
    3,
    0
};

/* Recognize menu state instance */
static testingStateObject testingState =
{
    0,
    false,
    0
};
uint16_t g_audioBuffer[160*MAX_FRAMES] = {0};
uint16_t* audioBufferPointer = NULL;
char g_charTranscript[MAX_TRANSCRIPT_LENGTH] = {0};
char g_charConfidenceLevel[6] = {0};

/*----------------------------------------------------------------------------
 * Functions
 * -------------------------------------------------------------------------*/
Menu_Handle TestingMenu_processEvent(Events event)
{
    int_least16_t *audioFrame;

    switch(event)
    {
        case EVENT_BUTTON1:
        {
        if (!testingState.running)
        {
            /* Go to next menu item if update not running */
            Menus_nextItem(&TestingMenu);
            //UART_PRINT("Menu item selected: %i\n\r",TestingMenu.itemSelected);
            switch (TestingMenu.itemSelected)
            {
            case 0:
                UART_PRINT("Selected <Menu Up>\n\r");
                break;
            case 1:
                UART_PRINT("Selected UART Dump\n\r");
                break;
            case 2:
                UART_PRINT("Selected IBM Test\n\r");
                break;
            }
        }
        }
        break;


        case EVENT_BUTTON2:
        {
            if(testingState.running)
            {
                /* End recognition, and show message stating so */
                Audio_stopCollect16KHz(Audio);
                char buffer[13];
                testingState.running = false;
                Leds_off1();
                Leds_setColor2(0, 0, 0);

                /* Set selected item to go to main menu */
                LCD_updateSelectedItem(&TestingMenu,
                     TestingMenu.itemSelected, 0);
                TestingMenu.itemSelected = 0;
                if(testingState.testmode==Audio_UART_Test){
                    Leds_on1();
                    LCD_showMessage(">> Dumping");
                    MessageFLen((char *)g_audioBuffer,320*testingState.framesProcessed);
                    Leds_off1();
                    sprintf(buffer,"Dumped %i",testingState.framesProcessed);
                    LCD_showMessage(buffer);
                    testingState.framesProcessed = 0;
                }
                else{
                    LCD_showMessage(">> Sending");
                    packetToWifi.bufferPointer =(char *) g_audioBuffer;
                    packetToWifi.bytesInBuffer = 160*2*testingState.framesProcessed;
                    packetToWifi.postOnCompletion = 1;
                    Leds_off2();
                    Leds_off3();
                    Leds_on1();
                    testingState.framesProcessed = 0;
                    Semaphore_post(wifiSemHandle);
                    Semaphore_pend(recogSemHandle,BIOS_WAIT_FOREVER);
                    LCD_showMenu(&TestingMenu);
                    LCD_showMessageLine(g_charTranscript, 45);
                    LCD_showMessageLine(g_charConfidenceLevel,70);

                }
            }

            else if(TestingMenu.itemSelected == 0)
            {
                /* Go back to main menu */
                LCD_showMenu(&MainMenu);
                UART_PRINT("Returned to Main Menu\n\r");
                return &MainMenu;
            }

            else if(TestingMenu.itemSelected == 1)
            {
                /* Start audio collection into global buffer, to dump over UART to PC once stopped */
                Audio_startCollect16KHz(Audio);
                testingState.running = true;
                testingState.testmode = Audio_UART_Test;
                testingState.framesProcessed = 0;

                LCD_showMessage(">> Dumping");
                UART_PRINT("Starting audio recording!\n\r");
                Leds_on3();
            }
            else if(TestingMenu.itemSelected == 2)
            {
                /* Start audio collection into global buffer, to send over wifi to IBM for processing once stopped */
                Audio_startCollect16KHz(Audio);
                testingState.running = true;
                testingState.testmode = Audio_IBM_Test;
                testingState.framesProcessed = 0;
                LCD_showMessage(">> Recording");
                UART_PRINT("Starting audio recording!\n\r");
                Leds_on3();
            }
            else{
                /* Display a test message onto the middle of the LCD */
                LCD_showMessageLine("Testing Message Print, see how this works with a long string",44);
            }
        }
        break;


        case EVENT_AUDIOFRAME:
        {
            if(testingState.running)
            {
#if UART_SYNC_IN_USE
                /* First check if UART needs reset due to data corruption */
            if (Audio_getUARTResetRequired(Audio))
            {
                //Wait 2.6ms to ensure UART burst is complete, then restart UART interface
                UtilsDelay(17333);
                Audio_stopCollect16KHz(Audio);
                Audio_startCollect16KHz(Audio);
            }
            else
            {
                /* Dump next frame of voice data. Display status
                 * based on result of processing. */
                audioFrame = Audio_getActiveBuffer(Audio);
                if (testingState.framesProcessed < MAX_FRAMES)
                {
                    audioBufferPointer = g_audioBuffer
                            + 160 * testingState.framesProcessed;
                    memcpy(audioBufferPointer, (audioFrame+2), 320);
                    testingState.framesProcessed++;
                    //If buffer is full, then warn user
                    if(testingState.framesProcessed==MAX_FRAMES){
                        Leds_on2();
                        LCD_showMessage("Buffer full!");
                        UART_PRINT("Buffer full! Press SW2 to send audio data\n\r");
                    }
                }
                else if (testingState.framesProcessed == MAX_FRAMES)
                {
                    //Warn user that buffer is maxed out
                    //Leds_on2();
                }
            }

#else
            /* Dump next frame of voice data. Display status
             * based on result of processing. */
            audioFrame = Audio_getActiveBuffer(Audio);
            if(testingState.framesProcessed<MAX_FRAMES)
            {
                audioBufferPointer = g_audioBuffer+160*testingState.framesProcessed;
                memcpy(audioBufferPointer,audioFrame,320);
                testingState.framesProcessed++;
            }
            else if(testingState.framesProcessed==MAX_FRAMES)
            {
                //Warn user that buffer is maxed out
                Leds_on2();
            }
#endif
        }

            else
            {
                /* Should not get an audio frame while not recognizing */
                Audio_stopCollect(Audio);
                testingState.framesProcessed = 0;
                Leds_off1();
                Leds_setColor2(0, 0, 0);

                LCD_showMessage("Dumping Error");
            }
        }
        break;
    }

    return &TestingMenu;
}
