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

/*! \file i2c_dsp_master_msp_slave.h
*
* \brief  This file contains structure, typedefs, functions and
*         prototypes used for i2c dsp master msp slave test
*/

#ifndef _I2C_DSP_MASTER_MSP_SLAVE_H_
#define _I2C_DSP_MASTER_MSP_SLAVE_H_

#include "platform_internals.h"
#include "platform_test.h"

#define I2C_MST_DATA_BLOCK_SIZE    (8u)
#define I2C_MST_DATA_BLOCK_COUNT   (2u)

#define I2C_MST_DATA_SIZE         ((I2C_MST_DATA_BLOCK_SIZE) * (I2C_MST_DATA_BLOCK_COUNT))

#define I2C_MST_SLAVE_ADDRESS      (0x38)

#define I2C_MST_SYS_CLK          	(100)
#define I2C_MST_BUS_FREQ         	(100)

/**
 * \brief This function performs i2c communication from dsp to msp
 *
 * \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
TEST_STATUS i2cDspMastermspSlaveTest(void *testArgs);

#endif /* _I2C_DSP_MASTER_MSP_SLAVE_H_ */

