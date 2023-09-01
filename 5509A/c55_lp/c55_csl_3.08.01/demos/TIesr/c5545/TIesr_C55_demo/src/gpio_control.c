/*
* gpio_control.c
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
#include <std.h>
#include "csl_types.h"
#include "csl_gpio.h"

GPIO_Handle hGpio;
CSL_GpioObj gGpioObj;

//*****************************************************************************
// GpioInit
//*****************************************************************************
//  Description: Initializes GPIO module
//
//  Arguments:
//      hGpio       - handle to GPIO (global)
//      gGpioObj    - GPIO parameter structure (global)
//      ioDir       - GPIO direction
//      intEn       - GPIO interrupt enables
//      intEdg      - GPIO edge
//
//  Return:
//      CSL_Status: CSL_SOK - GPIO module initialization successful
//                  other   - GPIO module initialization unsuccessful
//
//*****************************************************************************
CSL_Status GpioInit(
    Uint32 ioDir, 
    Uint32 intEn, 
    Uint32 intEdg,
    Uint32 oVal
)
{
    CSL_GpioConfig config;
    Uint16 i;
    CSL_Status status;

    /* Open GPIO module */
    hGpio = GPIO_open(&gGpioObj, &status);
    if ((hGpio == NULL) || (status != CSL_SOK))
    {
        return status;
    }

    /* Reset GPIO module */
    status = GPIO_reset(hGpio);
    if (status != CSL_SOK)
    {
        return status;
    }

    /* Configure GPIO module */
    config.GPIODIRL = ioDir & 0xFFFF;
    config.GPIODIRH = ioDir >> 16;
    config.GPIOINTENAL = intEn & 0xFFFF;
    config.GPIOINTENAH = intEn >> 16;
    config.GPIOINTTRIGL = intEdg & 0xFFFF;
    config.GPIOINTTRIGH = intEdg >> 16;
    status = GPIO_config(hGpio, &config);
    if (status != CSL_SOK)
    {
        return status;
    }

    /* Write output values for outputs */
    for (i=0; i<CSL_GPIO_NUM_PIN; i++)
    {
        if ((ioDir>>i) & 0x1)
        {
            GPIO_write(hGpio, (CSL_GpioPinNum)i, ((oVal>>i) & 0x1));
        }
    }

    return CSL_SOK;
}
