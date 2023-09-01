/*
* sys_init.h
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

#ifndef _SYS_INIT_H_
#define _SYS_INIT_H_

#include <std.h>
#include "csl_types.h"

// DSP LDO setting
//#define DSP_LDO                 ( 105 )
#define DSP_LDO                 ( 130 )

// DSP PLL output MHz setting
//#define PLL_MHZ                 ( 60 )
#define PLL_MHZ                 ( 100 )
//#define PLL_MHZ                 ( 75 )

#define	CSL_SPI_BUF_LEN			(64)
/**< SPI Buffer length used for reading and writing */

#define	SPI_CLK_DIV				(25)
/**< SPI Clock Divisor */

#define	SPI_FRAME_LENGTH		(1)

void ClockGatingAll(void);
void DspLdoSwitch(Uint16 mode);
void UsbLdoSwitch(Uint16 mode);

CSL_Status DeviceInit(void);
CSL_Status DmaInit(void);
CSL_Status I2cInit(void);

CSL_Status GpioInit(
    Uint32 ioDir,
    Uint32 intEn,
    Uint32 intEdg,
    Uint32 oVal
);

/* Function to initialize RTC module */
CSL_Status init_rtc(void);

/* Function to initialize SPI module */
CSL_Status init_spi(void);

/* Function to reset BLE module */
void reset_ble(void);

Void swi_SysInitFxn(Void);

/**
 * \brief    This function used to generate time delay in milliseconds
 *
 * \param    msecs - number of milliseconds
 *
 * \return   void
 */
void C55x_delay_msec(int msecs);

#endif

