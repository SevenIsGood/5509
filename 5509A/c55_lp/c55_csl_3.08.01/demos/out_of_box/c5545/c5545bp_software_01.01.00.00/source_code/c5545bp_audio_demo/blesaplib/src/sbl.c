/* --COPYRIGHT--,BSD
 * Copyright (c) 2014-2016, Texas Instruments Incorporated
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
 * --/COPYRIGHT--*/
/**************************************************************************************************
  @headerfile:    sbl.c
  Revised:        $Date: 2015-06-22 20:21:02 -0700 (Mon, 22 Jun 2015) $
  Revision:       $Revision: 44191 $

  Description:    This file contains top level SBL API for MSP432P4xx

**************************************************************************************************/


/*********************************************************************
 * INCLUDES
 */

//#include <xdc/std.h>
#include <stdbool.h>

#include "sbl.h"
#include "sbl_image.h"
#include "sbl_cmd.h"
#include "sbl_tl.h"

#include "board.h"
#include "csl_gpio.h"
#include "csl_spi.h"
#include <stdio.h>

Uint32 GetImageSize();
Uint8 Read_Image_Data(Uint8 *);
Uint16 *FWAddr;

extern GPIO_Handle     hGpio;
Uint16  imgBuf[SBL_MAX_TRANSFER];
/*********************************************************************
 * CONSTANTS
 */

// Indexes for pin configurations in PIN_Config array
#define RST_PIN_IDX             0
#define BL_PIN_IDX              1

// Imperically found delay count for 1 ms
#define SBL_DELAY_1_MS          4800

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

//! \brief PIN Config for reset and bl signals without PIN IDs
#if GPIO
static GPIO_PinConfig sblPinsCfg[] =
{
    GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH,
    GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_HIGH
};
#endif
static Uint32 rstPIN = 0;
static Uint32 blPIN = 0;


/*********************************************************************
 * LOCAL FUNCTIONS
 */

static Uint8 SBL_lastSectorProtected( void );
 void SBL_utilDelay(Uint32 ms);

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @fn      SBL_initParams
 *
 * @brief   Initializes SBL parameters
 *
 * @param   params - SBL parameter structure to be set to SBL default values
 *
 * @return  None.
 */
void SBL_initParams(SBL_Params *params)
{
   SBL_Params SBL_defaultParams;
  *params = SBL_defaultParams;
}

/**
 * @fn      SBL_open
 *
 * @brief   Opens the SBL port for writing images
 *
 * @param   params - SBL parameters to initialize the port with
 *
 * @return  Uint8 - SBL_SUCCESS, SBL_FAILURE
 */
Uint8 SBL_open(SBL_Params *params)
{
  // Assign PIN IDs to reset and bl
  rstPIN = params->resetPinID;
  blPIN = params->blPinID;

  // Open SBL Transport Layer
  return SBL_TL_open(params->targetInterface, params->localInterfaceID);
}

/**
 * @fn      SBL_eraseFlash
 *
 * @brief   Erases specific flash sectors of target device
 *
 * @param   addr - address of image to be erased
 * @param   size - total amount of bytes to erase starting from addr
 *
 * @return  Uint8 - SBL_SUCCESS, SBL_FAILURE
 */
Uint8 SBL_eraseFlash(Uint32 addr, Uint32 size)
{
  Uint16 i;
  Uint16 numPages = 0;
  Uint32 numBytes = size;
  Uint32 firstPageAddr = addr;


  // Calculate number of flash pages that need to be erased
  if (addr % SBL_PAGE_SIZE)
  {
    // Starting address is not page aligned, add page to erase total and
    // remove bytes in the starting page from total
    numPages++;
    numBytes -= (SBL_PAGE_SIZE - (addr % SBL_PAGE_SIZE));

    // Reset first page address to beginning of page that contains addr
    firstPageAddr -= (addr % SBL_PAGE_SIZE);
  }

  // Calculate pages from remaining byte total
  numPages += (numBytes / SBL_PAGE_SIZE);
  numPages += (numBytes % SBL_PAGE_SIZE) ? 1 : 0;

  for(i = 0; i < numPages; i++)
  {
    // Erase page
    SBL_CMD_sectorErase(firstPageAddr);

    // Update page address to next page
    firstPageAddr += SBL_PAGE_SIZE;
  }

  return SBL_SUCCESS;
}

/**
 * @fn      SBL_close
 *
 * @brief   Closes SBL port
 *
 * @param   None.
 *
 * @return  Uint8 - SBL_SUCCESS, SBL_FAILURE
 */
Uint8 SBL_close(void)
{
  // Close SBL Transport Layer
  SBL_TL_close();

  return SBL_SUCCESS;
}

/**
 * @fn      SBL_closeTarget
 *
 * @brief   Exit SBL
 *
 * @param   None.
 *
 * @return  None.
 */
void SBL_closeTarget( void )
{
  // Set BL PIN high and then set Reset PIN low to exit SBL
  GPIO_write(hGpio,(CSL_GpioPinNum )rstPIN, Board_LED_OFF);

  SBL_utilDelay(15);

  // Release Reset PIN while keeping BL pin low
  GPIO_write(hGpio,(CSL_GpioPinNum )rstPIN, Board_LED_ON);

  // Delay to be tuned
  SBL_utilDelay(150);

  SBL_utilDelay(150);
}

/**
 * @fn      SBL_openTarget
 *
 * @brief   Forces target device into SBL
 *
 * @param   None.
 *
 * @return  Uint8 - SBL_SUCCESS, SBL_FAILURE
 */
Uint8 SBL_openTarget( void )
{
  // Set BL PIN low and then set Reset PIN low to enter SBL
  GPIO_write(hGpio,(CSL_GpioPinNum )blPIN, Board_LED_OFF);
  GPIO_write(hGpio,(CSL_GpioPinNum )rstPIN, Board_LED_OFF);

  SBL_utilDelay(15);

  // Release Reset PIN while keeping BL pin low
  GPIO_write(hGpio,(CSL_GpioPinNum )rstPIN, Board_LED_ON);

  GPIO_write(hGpio,(CSL_GpioPinNum )blPIN, Board_LED_ON);

  // Delay to be tuned
  SBL_utilDelay(150);

  return SBL_FAILURE;
}

/**
 * @fn      SBL_resetTarget
 *
 * @brief   Forces target device to boot from flash image instead of SBL. This
 *          function can be called before SBL_open() or after SBL_open()
 *
 * @param   rstPinID - Board Pin ID of reset PIN
 * @param   blPinID  - Board Pin ID of boot loader PIN
 *
 * @return  Uint8 - SBL_SUCCESS, SBL_FAILURE
 */
Uint8 SBL_resetTarget(Uint32 rstPinID, Uint32 blPinID)
{
 	GPIO_write(hGpio,(CSL_GpioPinNum )blPIN, Board_LED_ON);

    // Set reset PIN low
    GPIO_write(hGpio,(CSL_GpioPinNum )rstPIN, Board_LED_OFF);

    // Release Reset PIN
    GPIO_write(hGpio,(CSL_GpioPinNum )rstPIN, Board_LED_ON);

    return SBL_SUCCESS;
}

/**
 * @fn      SBL_lastSectorProtected
 *
 * @brief   Verifies if the last sector is protected. Last sectors holds
 *          the CCFG configuration
 *
 * @param   None.
 *
 * @return  Uint8 - SBL_SUCCESS, SBL_FAILURE
 */
static Uint8 SBL_lastSectorProtected( void )
{
  Uint8 rsp[4];
  SBL_CMD_memoryRead(0x1FFF0,4,rsp);

  if (rsp[3] == 0x7F)
  {
      // Protected
      return SBL_SUCCESS;
  }
  else
  {
      // Not Protected
      return SBL_FAILURE;
  }
}

/**
 * @fn      SBL_utilDelay
 *
 * @brief   Simple for loop to burn cycles while target device is resetting
 *
 * @param   ms - Milliseconds to delay
 *
 * @return  None.
 */
void SBL_utilDelay(Uint32 ms)
{
  volatile Uint32 i,j; // Force compiler to not optimize loops

  for (i = 0; i < ms; i++)
  {
    // Delay one millisecond
    for (j = SBL_DELAY_1_MS; j > 0; j--);
  }
}
