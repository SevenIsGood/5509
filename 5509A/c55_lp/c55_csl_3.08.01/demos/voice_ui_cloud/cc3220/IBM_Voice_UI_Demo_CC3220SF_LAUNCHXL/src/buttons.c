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

/** \file buttons.c Contains functions for managing and getting results for
 * the two buttons on the Launchpad the Launchpad.\n (C) Copyright 2017,
 * Texas Instruments, Inc.
 */

/*----------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdbool.h>

#include "Board.h"

#include "buttons_private.h"

/*----------------------------------------------------------------------------
 * Globals
 * -------------------------------------------------------------------------*/
//Global buttons state struct
static Buttons_Object ButtonsGlobal;
//Callbacks for button debouncing
void switch3_isr_callback(uint_least8_t index);
void switch2_isr_callback(uint_least8_t index);
void switch3_timer_isr_callback(Timer_Handle handle);
void switch2_timer_isr_callback(Timer_Handle handle);
//Controls whether the processor is allowed to enter sleep
extern int g_intSleepAllowed;
//Handles to access switch control structures
Timer_Handle    switch3TimerHandle;
Timer_Handle    switch2TimerHandle;
//Semaphore handles to interact with semaphore blocking main loop
extern Semaphore_Struct mainSemStruct;
extern Semaphore_Handle mainSemHandle;
/*----------------------------------------------------------------------------
 * Functions
 * --------------------------------------------------------------------------*/

Buttons_Handle Buttons_setup()
{

    /* install Switch 3 callback */
#if !ENABLE_C5545_PIN_SETTINGS
    GPIO_setCallback(CC3220SF_LAUNCHXL_GPIO_SW3, switch3_isr_callback);
#endif

    /* install Switch 2 callback */
    GPIO_setCallback(CC3220SF_LAUNCHXL_GPIO_SW2, switch2_isr_callback);

    /* TIRTOS - Setup the switch 3 timer with the TIMERA3A hardware timer */
    Timer_Params    params;
    Timer_Params_init(&params);
    params.periodUnits = Timer_PERIOD_US;
    params.period = 15300;
    params.timerMode = Timer_ONESHOT_CALLBACK;
    params.timerCallback = switch3_timer_isr_callback;
    switch3TimerHandle = Timer_open(CC3220SF_LAUNCHXL_TIMER2, &params);

    /* TIRTOS- Setup the switch 2 timer with the TIMERA3B hardware timer */
    params.timerCallback = switch2_timer_isr_callback;
    switch2TimerHandle = Timer_open(CC3220SF_LAUNCHXL_TIMER3, &params);

    /* Setup buttons structure */
    ButtonsGlobal.button1Debounce = false;
    ButtonsGlobal.button1Pressed = false;
    ButtonsGlobal.button2Debounce = false;
    ButtonsGlobal.button2Pressed = false;

    /* TIRTOS - Enable interrupts. Note that the timer interrupts will be enabled automatically when Timer_start() is called*/
#if !ENABLE_C5545_PIN_SETTINGS
    GPIO_enableInt(CC3220SF_LAUNCHXL_GPIO_SW3);
#endif
    GPIO_enableInt(CC3220SF_LAUNCHXL_GPIO_SW2);

    return &ButtonsGlobal;
}

/*--------------------------------------------------------------------------*/
/* Find out if button1 event set */
bool Buttons_getButton1(Buttons_Handle buttons)
{
    return buttons->button1Pressed;
}

/*--------------------------------------------------------------------------*/
/* Find out if button2 event set */
bool Buttons_getButton2(Buttons_Handle buttons)
{
    return buttons->button2Pressed;
}

/*--------------------------------------------------------------------------*/
/* Reset button1 event */
void Buttons_clearButton1(Buttons_Handle buttons)
{
    buttons->button1Pressed = false;
}

/*--------------------------------------------------------------------------*/
/* Reset button2 event */
void Buttons_clearButton2(Buttons_Handle buttons)
{
    buttons->button2Pressed = false;
}

/*--------------------------------------------------------------------------*/
/* Shutdown use of the buttons API */
void Buttons_shutdown(Buttons_Handle buttons)
{
    /* Disable button press and timer A3 interrupts */
    /* TIRTOS - Only need to turn off the GPIO interrupts, Timer interrupts handled by Timer_stop() */
#if !ENABLE_C5545_PIN_SETTINGS
    GPIO_disableInt(CC3220SF_LAUNCHXL_GPIO_SW3);
#endif
    GPIO_disableInt(CC3220SF_LAUNCHXL_GPIO_SW2);
    Timer_stop(switch3TimerHandle);
    Timer_stop(switch2TimerHandle);
    Timer_close(switch3TimerHandle);
    Timer_close(switch2TimerHandle);

    buttons->button1Debounce = false;
    buttons->button1Pressed = false;
    buttons->button2Debounce = false;
    buttons->button2Pressed = false;
}

/*--------------------------------------------------------------------------*/
/* GPIO interrupt ISR for switch 3 */
void switch3_isr_callback(uint_least8_t index)
{
    /* button1 negative edge detected, no pending event, and debounce not ongoing */
    if( !ButtonsGlobal.button1Pressed &&!ButtonsGlobal.button1Debounce)
    {
        /* TIRTOS - All timer config handled by timer drivers Timer_open() and at Timer_start()*/
        Timer_start(switch3TimerHandle);
        ButtonsGlobal.button1Debounce = true;
    }
}
/*--------------------------------------------------------------------------*/
/* GPIO interrupt ISR for switch 2 */
void switch2_isr_callback(uint_least8_t index)
{
    /* button2 negative edge detected, no pending event, and debounce not ongoing */
    if( !ButtonsGlobal.button2Pressed && !ButtonsGlobal.button2Debounce)
    {
        /* TIRTOS - All timer config handled by timer drivers at Timer_open() and at Timer_start()*/
        Timer_start(switch2TimerHandle);
        ButtonsGlobal.button2Debounce = true;
    }
}
/*--------------------------------------------------------------------------*/
/* Timer A3 timeout interrupt ISR */
void switch3_timer_isr_callback(Timer_Handle handle)
{
    /* Debounce timeout for switch3 */
    ButtonsGlobal.button1Debounce = false;

    /* Set switch3 event if button still pressed */
#if !ENABLE_C5545_PIN_SETTINGS
    if(GPIO_read(CC3220SF_LAUNCHXL_GPIO_SW3))
    {
        ButtonsGlobal.button1Pressed = true;
        Semaphore_post(mainSemHandle);
    }
#endif
}
void switch2_timer_isr_callback(Timer_Handle handle)
{
    /*Debounce timeout for switch 2 - check to see if it is still pressed */
    ButtonsGlobal.button2Debounce = false;

    /* Set button2 event if button still pressed */
    if(GPIO_read(CC3220SF_LAUNCHXL_GPIO_SW2))
    {
        ButtonsGlobal.button2Pressed = true;
        Semaphore_post(mainSemHandle);
    }
}

