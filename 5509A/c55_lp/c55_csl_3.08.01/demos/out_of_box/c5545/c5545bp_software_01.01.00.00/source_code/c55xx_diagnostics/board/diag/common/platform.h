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
 * \file   platform.h
 *
 * \brief  This file contains structure, typedefs, functions and
 *         prototypes used for system initialization
 *
 *****************************************************************************/

#ifndef PLATFORM_UTILS_H_
#define PLATFORM_UTILS_H_

#define Platform_STATUS        Int32 /** Platform API return type */
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
 *  \brief UART Object structure
 *
 *  This structure maintains information of UART object
 */
typedef struct
{
    /** Instance id of UART     */
    Uint16 insId;
    /** UART register overlay structure pointer     */
	CSL_UartRegsOvly uartRegs;
    /** System control register overlay structure pointer                    */
    CSL_SysRegsOvly sysAddr;
    /**Operation mode of UART interrupt/polled*/
    CSL_UartOpmode opmode;
    /**Trigger level of fifo*/
    Uint16 trigLevel;
    /** ISR  dispatch table for storing call back function pointers*/
    Uint32 UART_isrDispatchTable[7];
}PLATFORM_UART_object;

/** Handle to the UART object structure                                       */
typedef PLATFORM_UART_object  *UartHandle;

/**
 * \brief    This function used to generate time delay in milliseconds
 *
 * \param    numOfmsec - number of milliseconds
 *
 * \return   void
 */
void C55x_delay_msec(int numOfmsec);

/**
 *  \brief  Function to calculate the clock at which system is running
 *
 *  \param    void
 *
 *  \return   System clock value in Hz
 */
Uint32 C55x_getSysClk(void);

/**
 *
 * \brief This function initialises the UART to transfer data to serial console
 *
 * \param    void
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
Platform_STATUS uart_initialisation(void);

/**
 *
 * \brief This function used to set the uart parameters
 *
 * \param    args   [IN]   UartSetup arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
Platform_STATUS platform_uart_set_params(CSL_UartSetup *args);

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
WRITE_info C55x_msgWriteConfigure (WRITE_info write_type);

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
READ_info C55x_msgReadConfigure (READ_info rdype);

/**
 *
 * \brief This function perform's Printf or echo to UART
 *
 * \param    fmt - character pointer
 *
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 */
Int32 C55x_msgWrite(const char *fmt, ...);

/**
 *
 * \brief This function perform's scanf or read from UART
 *
 * \param    data     - character pointer
 * \param	 length	  - charcter length
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
Platform_STATUS C55x_msgRead(Uint8 *data, Uint32 length);

#endif
