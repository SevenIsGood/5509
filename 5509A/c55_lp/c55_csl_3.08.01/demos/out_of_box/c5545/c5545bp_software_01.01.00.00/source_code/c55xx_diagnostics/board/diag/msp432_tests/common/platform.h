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
 
/*! \file platform.h
*
*   \brief Defines the data types used by platform APIs
*
*/ 

#ifndef PLATFORM_H_
#define PLATFORM_H_

#include "stdint.h"

#define Platform_EINVALID     -3   /**< Error code for invalid parameters */
#define Platform_EUNSUPPORTED -2   /**< Error code for unsupported feature */
#define Platform_EFAIL        -1   /**< General failure code */
#define Platform_EOK           0   /**< General success code */

/**
 * @brief Define how platform_write should behave.
 *    These write types can be set in the init structure
 */
typedef enum {
    PLATFORM_WRITE_UART,
    /** <Write to the UART */
    PLATFORM_WRITE_PRINTF,
    /** <printf mapped output -- CCS console */
    PLATFORM_WRITE_ALL
    /** <write all - default configuration */
} WRITE_info;

/**
 * @brief Define how platform_read should behave.
 *    These write types can be set in the init structure
 */
typedef enum {
    PLATFORM_READ_UART,
    /** <Read from the UART */
    PLATFORM_READ_SCANF
    /** <Read from scanf -- CCS console */
} READ_info;

/**
 * \brief    This function used to generate time delay in milliseconds
 *
 * \param    numOfmsec - number of milliseconds
 *
 * \return   void
 */
void delay_msec(int numOfmsec);

int32_t platform_write(const char *fmt, ...);

uint8_t platform_read(uint8_t *ch);

/**
 *
 * \brief This function Connfigures the input source for C55x_msgWrite
 *
 * \param    write_type - writes to uart console or ccs console
 *
 * \return
 * \n      0 - echo to UART
 * \n      1 - printf
 */
WRITE_info platformWriteConfigure (WRITE_info	wtype);

/**
 *
 * \brief This function Connfigures the input source for C55x_msgRead
 *
 * \param    rdype - read from uart console or ccs console
 *
 * \return
 * \n      0 - echo to UART
 * \n      1 - printf
 */
READ_info platformReadConfigure (READ_info	rdype);

#endif /* PLATFORM_H_ */
