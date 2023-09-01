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

/** \file mainTask.c A test program for demonstrating the speech recognition
 * capabilities of the CC3220 using the IBM Watson API
 * speech recognizer\n (C) Copyright 2017, Texas Instruments, Inc.
 */

/*----------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Headers for other source files in application */
#include "Board.h"
#include "menus.h"
#include "audio_collect.h"
#include "lcd.h"
#include "buttons.h"
#include "leds.h"
#include "platform.h"
#include "uart_term.h"
#include "sl_event_handlers.h"
#include "sl_helper_functions.h"
#include "network_task.h"
#if USE_PROVISIONING
#include "provisioning_task.h"
#endif

/* APIs used in the application */
#include <ti/drivers/ADC.h>
#include <ti/drivers/Timer.h>
#include <ti/drivers/SPI.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2S.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/net/wifi/simplelink.h>
/* POSIX Header files */
#include <pthread.h>

/* RTOS header files */
//#include <ti/sysbios/BIOS.h>
/*----------------------------------------------------------------------------
 * Defines
 * -------------------------------------------------------------------------*/

#define SPAWN_TASK_PRIORITY     (9)
#define TASK_STACK_SIZE         (3*1024)


/*----------------------------------------------------------------------------
 * Globals
 * -------------------------------------------------------------------------*/

/* Global handles for APIs used in the application that retain
 * state information in the application */
Audio_Handle Audio;
Buttons_Handle Buttons;

/* Ping-pong audio buffers for Audio API setup */
#if UART_SYNC_IN_USE
static int_least16_t AudioBuffer1[162];
static int_least16_t AudioBuffer2[162];
#else
static int_least16_t AudioBuffer1[160];
static int_least16_t AudioBuffer2[160];
#endif
#if USE_CONTINUOUS_UART_RECORDING
int toggleRecordingSend = 1;
#endif
/* Top level app main menu */
extern Menu_Object MainMenu;
extern Menu_Object ListeningMenu;
int g_intSleepAllowed = 1;
/* Semaphore posted by the various interrupts to inform main thread of event */
Semaphore_Struct mainSemStruct;
Semaphore_Handle mainSemHandle;
/* Semaphore posted by the network thread to unblock main thread following network operations in recognize menu*/
Semaphore_Struct recogSemStruct;
Semaphore_Handle recogSemHandle;

/* Variable that the aync events will use to signal the main thread of NWP events */
extern int g_intSlStatus;
int g_intTimeout = 0;
/* POSIX threads for the main loop as well as the async sl_task threads */
pthread_t g_main_thread = (pthread_t)NULL;
pthread_t g_network_thread = (pthread_t)NULL;
pthread_t g_spawn_thread = (pthread_t)NULL;
#if USE_PROVISIONING
pthread_t g_read_response_thread = (pthread_t)NULL;
pthread_t g_provisioning_thread = (pthread_t)NULL;
#endif
/*----------------------------------------------------------------------------
 * Functions
 * --------------------------------------------------------------------------*/

void * mainLoopThread( void *pvParameters )
{
    int RetVal = 0;
    uintptr_t                  key;
    pthread_attr_t      pAttrs;
    struct sched_param  priParam;
#if USE_PROVISIONING
    pthread_attr_t      pAttrs2;
    struct sched_param  priParam2;
#endif
    //int iRetVal = 0;
    /* Start at the main level menu */
    Menu_Handle currentMenu = &ListeningMenu;

    /* Flag to stop processing */
    bool done;
    /* Semaphore setup */
    Semaphore_Params semParams;
    Semaphore_Params_init(&semParams);
    Semaphore_construct(&mainSemStruct,0,&semParams);
    mainSemHandle = Semaphore_handle(&mainSemStruct);
    Semaphore_Params semParams2;
    Semaphore_Params_init(&semParams2);
    Semaphore_construct(&recogSemStruct,0,&semParams2);
    recogSemHandle = Semaphore_handle(&recogSemStruct);
#if USE_PROVISIONING
    sem_init(&Provisioning_ControlBlock.connectionAsyncEvent, 0, 0);
    sem_init(&Provisioning_ControlBlock.provisioningDoneSignal, 0, 0);
    sem_init(&Provisioning_ControlBlock.provisioningConnDoneToOtaServerSignal,
             0, 0);
#endif
    /* Set up CC3220 */
    //Board_initGeneral(); Handled in main()

    Timer_init();

    ADC_init();
    I2S_init();
    UART_init();
#if USE_I2S_AUDIO_IN&&CONFIG_AUDIO_BOOSTERPACK
    I2C_init();
#endif
    /* Hold off all interrupts */
    key = HwiP_disable();

    /* Prepare to use Launchpad LEDs */
    Leds_setup();

    /* Prepare to use Launchpad buttons */
    Buttons = Buttons_setup();

    /* Prepare to use Sharp display */
    LCD_setup();

    /* Prepare to collect audio ping-pong buffers */
#if UART_SYNC_IN_USE
    Audio = Audio_setupCollect( AudioBuffer1, AudioBuffer2, 162, 8000);
#else
    Audio = Audio_setupCollect( AudioBuffer1, AudioBuffer2, 160, 8000);
#endif
    /* Once setup is done, interrupts can be allowed */
    HwiP_restore(key);

    /* Display the main menu */
    LCD_showMenu(currentMenu);

    /* Ensure LEDs are off */
    Leds_off1();
    Leds_setColor2(0, 0, 0);
#if USE_PROVISIONING
    /* Spawn provisioning task */
    pthread_attr_init(&pAttrs2);
    priParam2.sched_priority = 1;
    RetVal = pthread_attr_setschedparam(&pAttrs2, &priParam2);
    RetVal = pthread_attr_setstacksize(&pAttrs2, TASK_STACK_SIZE);
    RetVal = pthread_create(&g_provisioning_thread, &pAttrs2, provisioningTask,
    NULL);
#endif
    /* Spawn network task */
    pthread_attr_init(&pAttrs);
    priParam.sched_priority = 2;
    RetVal = pthread_attr_setschedparam(&pAttrs, &priParam);
    RetVal |= pthread_attr_setstacksize(&pAttrs, TASK_STACK_SIZE+1024);
    RetVal = pthread_create(&g_network_thread, &pAttrs, networkTaskThread, NULL);
#if USE_CONTINUOUS_UART_RECORDING
#if USE_PROVISIONING
    //sem_wait(&Provisioning_ControlBlock.provisioningConnDoneToOtaServerSignal);
#endif
    Semaphore_post(mainSemHandle);
#endif
    /* Event processing loop */
    done = false;
    while(!done)
    {
        /* TIRTOS - Block here until event needs to be processed */
        Semaphore_pend(mainSemHandle, BIOS_WAIT_FOREVER);

        /* Check if buttons events have been set and handle them first. */
        if(Buttons_getButton1(Buttons))
        {
            currentMenu = currentMenu->processEvent(EVENT_BUTTON1);
            Buttons_clearButton1(Buttons);
        }

        if(Buttons_getButton2(Buttons))
        {
            currentMenu = currentMenu->processEvent(EVENT_BUTTON2);
            Buttons_clearButton2(Buttons);
        }
#if USE_CONTINUOUS_UART_RECORDING
        /* Check if flag to change recording state has been set */
        if(toggleRecordingSend>0){
            //This event is currently only implemented for the c5517 listening menu, for use in the headless demo
            currentMenu = currentMenu->processEvent(EVENT_TOGGLE_RECORDING);
            toggleRecordingSend--;
        }
#endif
        /* Check if audio collection has a buffer pending with valid data. */
        if(Audio_getActive(Audio))
        {
            currentMenu = currentMenu->processEvent(EVENT_AUDIOFRAME);
            Audio_resetActive(Audio);
        }

        /* Main menu flagging to quit the loop */
        if(currentMenu == NULL)
        {
            done = true;
        }
    }

    /* Shutdown everything */
    Audio_shutdownCollect(Audio);
    Buttons_shutdown(Buttons);
    Leds_shutdown();
    LCD_shutdown();
    return(0);
}
void * mainThread( void *pvParameters )
{
    uint32_t RetVal ;
    pthread_attr_t      pAttrs;
    pthread_attr_t      pAttrs_spawn;
    struct sched_param  priParam;
    struct timespec     ts = {0};

    //Board_initGPIO();
    GPIO_init();
    SPI_init();

 /* init the platform code..*/
#if !USE_PROVISIONING
    Platform_TimerInit(&AsyncEvtTimerIntHandler);
#endif


    /* init Terminal, and print App name */
    InitTerm();
    /* initilize the realtime clock */
    clock_settime(CLOCK_REALTIME, &ts);
    displayBanner();


    //create the sl_Task
    pthread_attr_init(&pAttrs_spawn);
    priParam.sched_priority = SPAWN_TASK_PRIORITY;
    RetVal = pthread_attr_setschedparam(&pAttrs_spawn, &priParam);
    RetVal |= pthread_attr_setstacksize(&pAttrs_spawn, TASK_STACK_SIZE);

    RetVal = pthread_create(&g_spawn_thread, &pAttrs_spawn, sl_Task, NULL);

    if(RetVal)
    {
        while(1);
    }

    //Create main task
    pthread_attr_init(&pAttrs);
    priParam.sched_priority = 1;
    RetVal = pthread_attr_setschedparam(&pAttrs, &priParam);
    RetVal |= pthread_attr_setstacksize(&pAttrs, TASK_STACK_SIZE);

    if(RetVal)
    {
        while(1);    /* error handling */
    }

    RetVal = pthread_create(&g_main_thread, &pAttrs, mainLoopThread, NULL);

    if(RetVal)
    {
        while(1);
    }

    return(0);
}
