/*
* lcd_osd.h
*
*
* Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/ 
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/
#ifndef _LCD_OSD_H_
#define _LCD_OSD_H_

#include <std.h>
#include "csl_i2c.h"

/* Sends 2 bytes of data to the OSD9616 */
CSL_Status OSD9616_send(
    Uint16 regAddr, 
    Uint16 regData, 
    pI2cHandle hi2c
);

/* Sends multiple bytes of data to the OSD9616 */
CSL_Status OSD9616_multiSend(
    Uint16 *regData, 
    Uint16 length, 
    pI2cHandle hi2c
);

/* Initializes OSD9616 display */
CSL_Status oled_init(
    pI2cHandle hi2c
);

/* Clears LCD page */
CSL_Status clear_lcd_page(
    Uint16 pageNum, 
    pI2cHandle hi2c
);

/* Clears the LCD */
CSL_Status clear_lcd(
    pI2cHandle hi2c
);

/* Send 6 bytes representing a Character to LCD */
CSL_Status print_char(
    unsigned char a, 
    pI2cHandle hi2c
);

/* Prints a string to LCD */
CSL_Status print_string(
    char *a, 
    pI2cHandle hi2c
);

#if 0
/* Prints "Texas Instruments" 
"C5535 eZdsp stk" to LCD */
Int16 print_ti(
    pI2cHandle hi2c
);

/* Prints "Industry lowest"
"power dsp C5535" to LCD */
Int16 print_dsp(
    pI2cHandle hi2c
);

/* Prints all characters in ROM table to LCD */
Int16 print_charROM(
    pI2cHandle hi2c
);
#endif

#endif
