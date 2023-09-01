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
 * \file   usb_test.h
 *
 * \brief  This file contains structure, typedefs, functions and
 *         prototypes used for usb test
 *
 *****************************************************************************/

#ifndef _USB_TEST_H_
#define _USB_TEST_H_

#include "platform_internals.h"
#include "platform_test.h"

#include "csl_error.h"
#include "csl_intc.h"
#include "csl_usb.h"
#include "csl_hid.h"
#include "csl_hidAux.h"
#include "soc.h"
#include "csl_usbAux.h"
#include "tistdtypes.h"

#define STRING_DESCR_SZ         (7)  /**< string descriptor size  */

#define HID_REPORT_SIZE_BYTES   (3)  /**< report size in bytes */

#define HID_REPORT_SIZE_WORDS   ((HID_REPORT_SIZE_BYTES + 1)/2 )

#define CSL_USB_MAX_CURRENT     (50)
#define HID_REPORT_ID           (1)        /**< HID report ID          */
#define DEV_VID                 (0x04B3 )  /**< Vendor ID (idVendor)   */
#define DEV_PID                 (0x310B )  /**< Product ID (idProduct) */
#define IF_NUM_HID              (3)
#define EP_HID_MAXP             (3)        /**< maximum packet size for HID
                                                endpoint */
#define UP_KEY          (1)
#define DOWN_KEY        (2)
#define RIGHT_KEY       (3)
#define LEFT_KEY        (4)
#define RIGHT_CLICK_KEY (5)
#define LEFT_CLICK_KEY  (6)
//#define STOP_KEY        (7)
#define NO_KEY          (8)
#define STOP_KEY        (2)

extern Uint16 preKey;

/*---------Function prototypes---------*/
/**
 *  \brief  GPIO Interrupt Service Routine
 *
 *  \param  void
 *
 *  \return void
 */
interrupt void gpioPinISR(void);

CSL_Status GPIO_initInstance(void);
CSL_Status GPIO_closeInstance(void);

Uint16 GPIO_GetKey(void);


CSL_Status SAR_initInstance(void);
CSL_Status SAR_closeInstance(void);

Uint16 SAR_GetKey(void);

#define SW6_MIN_VALUE  0x367
#define SW6_MAX_VALUE  0x374

#define SW7_MIN_VALUE  0x24F
#define SW7_MAX_VALUE  0x258

#define SW8_MIN_VALUE  0x30B
#define SW8_MAX_VALUE  0x316

#define SW9_MIN_VALUE  0x230
#define SW9_MAX_VALUE  0x238

#ifdef C5535_EZDSP
#define SW10_MIN_VALUE 0x290
#define SW10_MAX_VALUE 0x2BF
#else
#define SW10_MIN_VALUE 0x20F
#define SW10_MAX_VALUE 0x216
#endif

#define SW11_MIN_VALUE 0x1D3
#define SW11_MAX_VALUE 0x1D8

#ifdef C5535_EZDSP
#define SW12_MIN_VALUE 0x1F0
#define SW12_MAX_VALUE 0x21F
#else
#define SW12_MIN_VALUE 0x163
#define SW12_MAX_VALUE 0x167
#endif

#define SW13_VALUE     0x000

#define SW14_MIN_VALUE 0x2BC
#define SW14_MAX_VALUE 0x2C6

#define SW15_MIN_VALUE 0xD5
#define SW15_MAX_VALUE 0xD8

#define NoKey_MIN_VALUE 0x3FB
#define NoKey_MAX_VALUE 0x3FF

#define ERROR_STATUS    (0x10)

#define NO_OF_ATTEMPTS  (10)

/**< This macro indicates the no of times the key values must be read for
     sending a single mouse command */

/**
 * \brief This function performs uart test
 *
 * \param    testArgs   [IN]   Test arguments
 *
 * \return
 * \n      TEST_PASS  - Test Passed
 * \n      TEST_FAIL  - Test Failed
 *
 */
TEST_STATUS usbTest(void *testArgs);

#endif
