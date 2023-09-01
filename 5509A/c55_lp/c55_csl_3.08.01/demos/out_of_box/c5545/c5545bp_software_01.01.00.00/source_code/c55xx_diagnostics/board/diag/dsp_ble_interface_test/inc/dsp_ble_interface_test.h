/*
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

/**
 *
 * \file   dsp_ble_interface_test.h
 *
 * \brief  This file contains structure, typedefs, functions and
 *         prototypes used for dsp ble interface test
 *
 *****************************************************************************/

#ifndef DSP_SPI_TEST_H_
#define DSP_SPI_TEST_H_

#include "platform_internals.h"
#include "platform_test.h"

#define C5535_EZDSP

#define CSL_TEST_FAILED         (1)
/**< Error Status to indicate Test has Failed */
#define CSL_TEST_PASSED         (0)
/**< Error Status to indicate Test has Passed */

#define	CSL_SPI_BUF_LEN			(64)
/**< SPI Buffer length used for reading and writing */

#define	SPI_CLK_DIV				(25)
/**< SPI Clock Divisor */

#define	SPI_FRAME_LENGTH		(1)
/**< SPI Frame length */

/* SPI Commands */
#define SPI_CMD_WRSR            (0x01)
#define SPI_CMD_WRITE           (0x02)
#define SPI_CMD_READ            (0x03)
#define SPI_CMD_WRDI            (0x04)
#define SPI_CMD_RDSR            (0x05)
#define SPI_CMD_WREN            (0x06)
#define SPI_CMD_ERASE           (0x20)

 /**
  * \brief This function performs dsp-BLE interface test
  *
  * \param    testArgs   [IN]   Test arguments
  *
  * \return
  * \n      TEST_PASS  - Test Passed
  * \n      TEST_FAIL  - Test Failed
  *
  */
TEST_STATUS dspBLETest(void *testArgs);

#endif
