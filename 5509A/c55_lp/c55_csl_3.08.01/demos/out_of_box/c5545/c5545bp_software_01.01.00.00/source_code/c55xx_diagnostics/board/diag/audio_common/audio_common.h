/*
 * Copyright (c) 2016, Texas Instruments Incorporated
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
 * \file   audio_common.h
 *
 * \brief  This file contains structure, typedefs, functions and
 *         prototypes used for audio line in loopback test
 *
 *****************************************************************************/

#ifndef _AUDIO_COMMON_H_
#define _AUDIO_COMMON_H_

#include "platform_internals.h"
#include "platform_test.h"
#include "evm5515.h"

#define AIC3206_I2C_ADDR 0x18

#define Rcv 0x08
#define Xmit 0x20

extern CSL_I2sHandle   hI2s;

static CSL_GpioObj     GpioObj;
static CSL_GpioObj     *gpioHandle;

extern volatile Uint16  sw3Pressed;

/* Reference the start of the interrupt vector table */
/* This symbol is defined in file vectors.asm       */
extern void VECSTART(void);

/**
 *  \brief  GPIO Interrupt Service Routine
 *
 *  \param testArgs   [IN]   Test arguments
 *
 *  \return none
 */
interrupt void gpioISR(void);


TEST_STATUS gpio_interrupt_initiliastion(void);

/**
 * \brief This function used to Enable and initalize the I2S module
 *
 * \param void
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
TEST_STATUS initialise_i2s_interface(void);

/**
 * \brief This function Reads incoming I2S left channel word and writes it
 *      to the location of "data".
 *
 * \param   *data - Pointer to location if I2S data destination
 *
 * \return void
 *
 */
void I2S_readLeft(Int16* data);

/**
 * \brief This function used to Writes I2S left channel word
 *
 * \param  data -I2S left data
 *
 * \return void
 *
 */
void I2S_writeLeft(Int16 data);

/**
 * \brief This function Reads incoming I2S right channel word and writes it
 *      to the location of "data".
 *
 * \param   *data - Pointer to location if I2S data destination
 *
 * \return void
 *
 */
void I2S_readRight(Int16* data);

/**
 * \brief This function used to Writes I2S right channel word
 *
 * \param  data -I2S right data
 *
 * \return void
 *
 */
void I2S_writeRight(Int16 data);

/**
 * \brief This function used to Enable and initalize the I2C module
 *		   The I2C clk is set to run at 100 KHz
 *
 * \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
TEST_STATUS initialise_i2c_interface(void *testArgs);

/**
 * \brief This function used to write into the audio codec registers
 *
 * \param  regnum - register number
 *         regVal - register data
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
TEST_STATUS AIC3206_write(Uint16 regnum, Uint16 regval);

#endif
