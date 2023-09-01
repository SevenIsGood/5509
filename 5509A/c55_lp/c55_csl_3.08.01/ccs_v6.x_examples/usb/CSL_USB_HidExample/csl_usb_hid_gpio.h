/*
 * Copyright (c) 2015, Texas Instruments Incorporated
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
 *
 */

/**
 *
 * \file   push_button_test.h
 *
 * \brief  This file contains structure, typedefs, functions and
 *         prototypes used for push button test
 *
 *****************************************************************************/

#ifndef _CSL_USB_HID_GPIO_H_
#define _CSL_USB_HID_GPIO_H_

#define UP_KEY          (1)
#define DOWN_KEY        (2)
#define RIGHT_KEY       (3)
#define LEFT_KEY        (4)
#define RIGHT_CLICK_KEY (5)
#define LEFT_CLICK_KEY  (6)
#define STOP_KEY        (7)
#define NO_KEY          (8)

extern Uint16 preKey;

/*---------Function prototypes---------*/
/**
 *  \brief  GPIO Interrupt Service Routine
 *
 *  \param  void
 *
 *  \return void
 */
interrupt void gpioISR(void);

CSL_Status GPIO_initInstance(void);
CSL_Status GPIO_closeInstance(void);

Uint16 GPIO_GetKey(void);


#endif
