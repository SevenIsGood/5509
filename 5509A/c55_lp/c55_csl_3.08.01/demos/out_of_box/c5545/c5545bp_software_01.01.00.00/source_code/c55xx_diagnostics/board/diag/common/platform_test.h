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
  * \file   platform_test.h
  *
  * \brief  This file contains structure, typedefs, functions and
  *         prototypes used for audio line in loopback test
  *
 *****************************************************************************/

#ifndef _PLATFORM_TEST_H_
#define _PLATFORM_TEST_H_

#include "platform_internals.h"
#include "platform.h"

typedef Int16 TEST_STATUS;

typedef Int16 RET_STATUS;

#define PLATFORM_TEST_AUTO_ALL     (0)
#define PLATFORM_TEST_START        (1)
#define PLATFORM_AUTO_TEST_COUNT   (14)
#define PLATFORM_TEST_COUNT        (16)

/* Platform test return codes */
#define TEST_PASS     (0)
#define TEST_FAIL     (-1)

#ifdef USE_SERIAL_CONSOLE
#define SD_BOOT
#endif

#define ENABLE_USER_INTERVENTION

#ifdef ENABLE_USER_INTERVENTION
#define USE_USER_INPUT
#endif


/**
 *  \brief    Initializes platform test modules
 *
 *  \return    - Platform_EOK on Success or error code
 */
Platform_STATUS initPlatform(void);

/**
 *  \brief structure parameters for GPIO Handle
 */
typedef struct{

    /** \brief BaseAddress of GPIO Registers */
    CSL_GpioRegsOvly baseAddr;

    /** \brief This is the maximum number of pins supported by this
    *   instance of GPIO
    */
    Uint8 numPins;

}ledTestArgs_t;

/**
 *  \brief UART Setup structure
 *
 *  This structure contains the UART setup parameters.
 *  Using this structure individual parameters can be passed
 */
typedef struct
{
   /** Input clock in Hz*/
    Uint32              clkInput;
   /**Baud rate  */
    Uint32              baud;
   /**Word length selection
      Valid values - 5,6,7, or 8bits*/
    Uint16              wordLength;
   /**Number of stop bits to be generated
      Valid values - 0 or 1 */
    Uint16              stopBits;
   /** Parity generation enable/disable*/
    Uint16              parity;
   /** Fifo control register */
    Uint16              fifoControl;
    /**Looppback mode enable/disable*/
    Uint16              loopBackEnable;
	/** AFE enable/disable*/
	Uint16              afeEnable;
	/** RTS enable/disable*/
	Uint16              rtsEnable;


} uartTestArgs_t;

/**
 * \brief structure for card specific information.
 *
 */
typedef struct {
	Uint16                rca;
	/**< MMC card: User assigned relative card address (RCA).
	     SD card: Relative card address (RCA) published by the card          */
	Uint16                ST0;
	/**< Last read ST0 register value                                        */
	Uint16                ST1;
	/**< Last read ST1 register value                                        */
	Uint16                cardIndex;
	/**< MMC module assigned card index for card                             */
	Uint32                maxXfrRate;
	/**< Maximum transfer rate                                               */
	Uint32                readAccessTime;
	/**< TAAC - exp * mantissa                                               */
	Uint32 cardCapacity;
	/**< Total memory available on card
	   Note: For CSD Version 1.0 (CSL_MMCCardCsdObj.csdStruct = 0) capacity
			 will be in bytes
			 For CSD Version 2.0 (CSL_MMCCardCsdObj.csdStruct = 1) capacity
			 will be in KBytes                                               */
	Uint32                blockLength;
	/**< Block length of card                                                */
	Uint32                totalSectors;
	/**< Total number of sectors on card                                     */
	Uint32                lastAddrRead;
	/**< Last address read from memory card                                  */
	Uint32                lastAddrWritten;
	/**< Last Address written to memory card                                 */
	CSL_CardType          cardType;
	/**< Type of the card - MMC or SD                                        */
	CSL_MMCCardIdObj      *cid;
	/**< Manufacturers Card ID                                               */
	CSL_MMCCardCsdObj     *csd;
	/**< Card specific data                                                  */
	CSL_MMCCardXCsdObj    *xcsd;
	/**< Extended CSD                                                        */
	Bool                  sdHcDetected;
	/**< Flag to indicate detection of SD HC card                            */
	Bool                  mmcHcDetected;
	/**< Flag to indicate detection of MMC HC card                           */
	Bool                  cardAtaFsOpen;
	/**< Flag to indicate whether ATAFS open on card                         */
	Uint16                cardMscStatus;
	/**< MSC status for card                                                 */
} sdCardTestArgs_t;

/**
 *  \brief    Diagnostic test main function
 *
 *  \return   Platform_STATUS
 */
Platform_STATUS diag_main (void);

typedef struct diagTestTable_s
{
	const char *testName;
	TEST_STATUS (*testFunction)(void *);
	void  *args;
	Uint8  pass;
	Uint8 fail;
}diagTestTable_t;

#endif
