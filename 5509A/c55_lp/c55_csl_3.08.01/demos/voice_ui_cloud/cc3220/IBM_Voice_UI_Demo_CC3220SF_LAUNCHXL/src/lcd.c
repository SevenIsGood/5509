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

/** \file display.c Contains functions for displaying menus and recognition
 * results on the LCD.\n (C) Copyright 2017, Texas Instruments, Inc.
 */

/*----------------------------------------------------------------------------
 * Includes
 * --------------------------------------------------------------------------*/
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "grlib.h"

#include "lcd.h"
#include "Sharp96x96.h"

#include "menus.h"
#include "board.h"
/*----------------------------------------------------------------------------
 * Globals
 * -------------------------------------------------------------------------*/

/* Sharp display context */
Graphics_Context g_sContext;
#define TIMERTARGET 1
void vcom_cycle_timer_isr_callback(Timer_Handle handle);
Timer_Handle vcomCycleTimerHandle;
bool g_allowLCDVcomToggle;

/*----------------------------------------------------------------------------
 * Functions
 * --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* Set up the Sharp LCD */
void LCD_setup()
{
#if !ENABLE_C5545_PIN_SETTINGS
    /* Initialize GPIO, and SPI EUSCI_B0 for LCD display */
    Sharp96x96_LCDInit();

    /* Set up initial graphics context for LCD */
    Graphics_initContext(&g_sContext, &g_sharp96x96LCD);
    Graphics_setForegroundColor(&g_sContext, ClrBlack);
    Graphics_setBackgroundColor(&g_sContext, ClrWhite);
    Graphics_setFont(&g_sContext, &g_sFontFixed6x8);
    g_allowLCDVcomToggle = true;

    /* TIRTOS - Setup the vcom cycle timer with the TIMERA2A hardware timer */
    /* Nominal 29Hz VCOM cycle frequency */
    Timer_Params params;
    Timer_Params_init(&params);
    params.periodUnits = Timer_PERIOD_HZ;
    params.period = 29;
    params.timerMode = Timer_CONTINUOUS_CALLBACK;
    params.timerCallback = vcom_cycle_timer_isr_callback;
    vcomCycleTimerHandle = Timer_open(CC3220SF_LAUNCHXL_TIMER1, &params);

    /* Start timer immediately */
    Timer_start(vcomCycleTimerHandle);
#endif
}

/*-------------------------------------------------------------------------*/
/* Shutdown the display */
void LCD_shutdown()
{
#if !ENABLE_C5545_PIN_SETTINGS
    Timer_stop(vcomCycleTimerHandle);
#endif
}

/*--------------------------------------------------------------------------*/
/* Clear the display */
static void LCD_clear()
{
    /* Clear display; must disable timer interrupt during write */
    /* TIRTOS - Set a flag to prevent the VCOM cycle interrupt callback from sending the toggle vcom command. No interface to simply disable the timer interrupt */

#if !ENABLE_C5545_PIN_SETTINGS
    g_allowLCDVcomToggle = false;
    Graphics_clearDisplay(&g_sContext);
    g_allowLCDVcomToggle = true;
#endif
}

/*--------------------------------------------------------------------------*/
/* Flush the display buffer to the display */
static void LCD_flush()
{
#if !ENABLE_C5545_PIN_SETTINGS
    /* Flush display buffer; must disable timer interrupt during write */
    g_allowLCDVcomToggle = false;
    Graphics_flushBuffer(&g_sContext);
    g_allowLCDVcomToggle = true;
#endif
}

/*--------------------------------------------------------------------------*/
/* Clear the display and show the full menu */
void LCD_showMenu(Menu_Handle menu)
{
#if !ENABLE_C5545_PIN_SETTINGS
    uint_fast8_t fontHeight;
    uint_fast16_t displayWidth;
    uint_fast16_t height = 0;
    uint_fast8_t item;

    /* Parameters needed to draw menu with given font */
    displayWidth = Graphics_getDisplayWidth(&g_sContext);
    fontHeight = Graphics_getFontHeight(g_sContext.font);

    /* Remove any prior graphics */
    LCD_clear();

    /* Write header to buffer if it exists */
    if( menu->header != NULL)
    {
        Graphics_drawStringCentered(&g_sContext, (int8_t *)(menu->header),
            GRAPHICS_AUTO_STRING_LENGTH, displayWidth/2, fontHeight/2,
            GRAPHICS_TRANSPARENT_TEXT);
        height += fontHeight + 1;
    }

    /* Write menu items to buffer */
    for(item = 0; item < menu->numItems; item++)
    {
        if( item == menu->itemSelected)
        {
            /* Selected item in reverse text */
            Graphics_setForegroundColor(&g_sContext, ClrWhite);
            Graphics_setBackgroundColor(&g_sContext, ClrBlack);

            Graphics_drawString(&g_sContext, (int8_t *)(menu->items[item]),
                GRAPHICS_AUTO_STRING_LENGTH, 1, height,
                GRAPHICS_OPAQUE_TEXT);

            /* Return to normal text style */
            Graphics_setForegroundColor(&g_sContext, ClrBlack);
            Graphics_setBackgroundColor(&g_sContext, ClrWhite);

            height += fontHeight + 1;
        }

        else
        {
            /* Non-selected menu items */
            Graphics_drawString(&g_sContext, (int8_t *)(menu->items[item]),
                GRAPHICS_AUTO_STRING_LENGTH, 1, height,
                GRAPHICS_TRANSPARENT_TEXT);

             height += fontHeight + 1;
        }
    }

    /* Output menu to display*/
    LCD_flush();
#endif
}

/*--------------------------------------------------------------------------*/
/* Switch the selected item to a new item */
void LCD_updateSelectedItem(Menu_Handle menu, int_least8_t oldItem,
    int_least8_t newItem)
{
#if !ENABLE_C5545_PIN_SETTINGS
    uint_fast16_t height = 0;
    uint_fast16_t heightStep;
    uint_fast16_t heightBase = 0;

    /* Parameters needed to draw menu with given font */
    heightStep = Graphics_getFontHeight(g_sContext.font) + 1;
    if(menu->header != NULL)
    {
        heightBase = heightStep;
    }

    /* Write the old menu item as normal text */
    height = heightBase + (heightStep * oldItem);

    Graphics_drawString(&g_sContext, (int8_t *)(menu->items[oldItem]),
        GRAPHICS_AUTO_STRING_LENGTH, 1, height,
        GRAPHICS_OPAQUE_TEXT);

    /* Write the new menu item as reverse text */
    Graphics_setForegroundColor(&g_sContext, ClrWhite);
    Graphics_setBackgroundColor(&g_sContext, ClrBlack);

    height = heightBase + (heightStep * newItem);

    Graphics_drawString(&g_sContext, (int8_t *)(menu->items[newItem]),
        GRAPHICS_AUTO_STRING_LENGTH, 1, height,
        GRAPHICS_OPAQUE_TEXT);

    /* Return to normal text style */
    Graphics_setForegroundColor(&g_sContext, ClrBlack);
    Graphics_setBackgroundColor(&g_sContext, ClrWhite);

    /* Flush the changes */
    /* Output menu; must disable timer interrupt during write */
    LCD_flush();
#endif
}

/*--------------------------------------------------------------------------*/
/* Display a message at the bottom of the display. Menus must not be in this
 * area of the display. If message is NULL, the message area is cleared. */
void LCD_showMessage(char const *message)
{
#if !ENABLE_C5545_PIN_SETTINGS
    uint_fast16_t displayHeight;
    Graphics_Rectangle rect;

    /* Parameters needed to draw menu with given font */
    displayHeight = Graphics_getDisplayHeight(&g_sContext);

    /* Clear message area */
    rect.xMin = 0;
    rect.xMax = Graphics_getDisplayWidth(&g_sContext) - 1;
    rect.yMin = displayHeight - Graphics_getFontHeight(g_sContext.font);
    rect.yMax = displayHeight - 1;

    Graphics_setForegroundColor(&g_sContext, ClrWhite);
    Graphics_fillRectangle(&g_sContext, &rect);
    Graphics_setForegroundColor(&g_sContext, ClrBlack);

    /* Write new message to bottom of display */
    if( message != NULL)
    {
        Graphics_drawString(&g_sContext, (int8_t *)(message),
            GRAPHICS_AUTO_STRING_LENGTH, 1, rect.yMin,
            GRAPHICS_TRANSPARENT_TEXT);
    }

    /* Flush the changes */
    /* Output menu; must disable timer interrupt during write */
    LCD_flush();
#endif
}

/*--------------------------------------------------------------------------*/
/* Display a message at a specific line of the display. Menus must not be in this
 * area of the display. If message is NULL, the message area is cleared. */
void LCD_showMessageLine(char const *message, int line)
{
#if !ENABLE_C5545_PIN_SETTINGS
    uint_fast16_t displayHeight;
    Graphics_Rectangle rect;
    char* toPrint = (char *) message;

    /* Parameters needed to draw menu with given font */
    displayHeight = Graphics_getDisplayHeight(&g_sContext);

    /* Clear message area */
    rect.xMin = 0;
    rect.xMax = Graphics_getDisplayWidth(&g_sContext) - 1;
    rect.yMin = line;
    rect.yMax = line + Graphics_getFontHeight(g_sContext.font);
    if(rect.yMin<0){
        rect.yMin = 0;
        rect.yMax = Graphics_getFontHeight(g_sContext.font);
    }
    else if(rect.yMax>(displayHeight-1)){
        rect.yMin = displayHeight - Graphics_getFontHeight(g_sContext.font);
        rect.yMax = displayHeight - 1;
    }

    /* Write new message starting from designated line of display */
    if( toPrint != NULL)
    {
        bool messageEnd = false;
        while(((rect.yMax<displayHeight)&&!messageEnd)){
            if(strlen(toPrint)>16){
                Graphics_setForegroundColor(&g_sContext, ClrWhite);
                Graphics_fillRectangle(&g_sContext, &rect);
                Graphics_setForegroundColor(&g_sContext, ClrBlack);
                Graphics_drawString(&g_sContext, (int8_t *)(toPrint),
                                    15, 1, rect.yMin,
                                    GRAPHICS_TRANSPARENT_TEXT);
                toPrint += 15;
                rect.yMin += Graphics_getFontHeight(g_sContext.font);
                rect.yMax += Graphics_getFontHeight(g_sContext.font);
            }
            else{
                Graphics_drawString(&g_sContext, (int8_t *)(toPrint),
                                    GRAPHICS_AUTO_STRING_LENGTH, 1, rect.yMin,
                                    GRAPHICS_TRANSPARENT_TEXT);
                messageEnd = true;
            }
        }
    }

    /* Flush the changes */
    /* Output menu; must disable timer interrupt during write */
    LCD_flush();
#endif
}

/*--------------------------------------------------------------------------*/
/* Interrupt handler for software LCD VCOM toggle */
void vcom_cycle_timer_isr_callback(Timer_Handle handle)
{
#if !ENABLE_C5545_PIN_SETTINGS
    /* Toggle VCOM signal at approximately 29 Hz rate */
    /* Only if the LCD isn't busy */
    if(g_allowLCDVcomToggle){
        Sharp96x96_SendToggleVCOMCommand();
    }
#endif
}

