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

/** \file leds.c Contains functions for managing LED display of the two
 * LEDs on the Launchpad.\n (C) Copyright 2017, Texas Instruments, Inc.
 */

/*----------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdbool.h>

#include "board.h"
#include "leds.h"
#include <ti/drivers/GPIO.h>

/*----------------------------------------------------------------------------
 * Globals
 * -------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------
 * Functions
 * --------------------------------------------------------------------------*/
/* Set up LED ports and timer for color LED */
void Leds_setup()
{

    //Nothing to do here when using TIRTOS - all setup done in GPIO_init()
}

/*--------------------------------------------------------------------------*/
/* Turn on LED1 */
void Leds_on1()
{
#if !ENABLE_C5545_PIN_SETTINGS
    GPIO_write(CC3220SF_LAUNCHXL_GPIO_LED_D7,1);
#endif
}

/*--------------------------------------------------------------------------*/
/* Turn off LED1 */
void Leds_off1()
{
#if !ENABLE_C5545_PIN_SETTINGS
    GPIO_write(CC3220SF_LAUNCHXL_GPIO_LED_D7,0);
#endif
}
/*--------------------------------------------------------------------------*/

/* Turn on LED2 */
void Leds_on2()
{
#if !ENABLE_C5545_PIN_SETTINGS
    GPIO_write(CC3220SF_LAUNCHXL_GPIO_LED_D6,1);
#endif
}

/*--------------------------------------------------------------------------*/
/* Turn off LED2 */
void Leds_off2()
{
#if !ENABLE_C5545_PIN_SETTINGS
    GPIO_write(CC3220SF_LAUNCHXL_GPIO_LED_D6,0);
#endif
}
/*--------------------------------------------------------------------------*/

/* Turn on LED3 */
void Leds_on3()
{
#if !ENABLE_C5545_PIN_SETTINGS
    GPIO_write(CC3220SF_LAUNCHXL_GPIO_LED_D5,1);
#endif
}

/*--------------------------------------------------------------------------*/
/* Turn off LED3 */
void Leds_off3()
{
#if !ENABLE_C5545_PIN_SETTINGS
    GPIO_write(CC3220SF_LAUNCHXL_GPIO_LED_D5,0);
#endif
}

/*--------------------------------------------------------------------------*/
/* Set color of LED2 */
void Leds_setColor2(uint_least8_t red, uint_least8_t green, uint_least8_t blue)
{
    /*If all colors are off, then turn off both LEDs */
    if(!(red||green||blue)){
        Leds_off2();
        Leds_off3();
    }
    /*If red and green are fully on, then turn on both LEDs */
    else if((red==255&&green==255)){
        Leds_on2();
        Leds_on3();
    }
    /*If only red is fully on, then turn on the yellow LED, and turn off the green LED */
    else if(red==255){
        Leds_on2();
        Leds_off3();
    }
    /*If only green is fully on, then turn on the green LED, and turn off the yellow LED */
    else if(green==255){
        Leds_on3();
        Leds_off2();
    }
}

void Leds_shutdown()
{
    Leds_off1();
    Leds_off2();
    Leds_off3();
}



