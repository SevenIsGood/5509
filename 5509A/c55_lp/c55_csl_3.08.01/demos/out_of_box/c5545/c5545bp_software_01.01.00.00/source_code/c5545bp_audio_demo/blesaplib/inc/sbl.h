/* --COPYRIGHT--,BSD
 * Copyright (c) 2014, Texas Instruments Incorporated
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
  Filename:       sbl.h
  Revised:        $Date: 2015-06-22 20:21:02 -0700 (Mon, 22 Jun 2015) $
  Revision:       $Revision: 44191 $

  Description:    This file contains top level SBL API for MSP432P4xx

**************************************************************************************************/

#ifndef SBL_H
#define SBL_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */
//akhil:added
#include <stdint.h>
#include <tistdtypes.h>

//end
//! \brief External Device SBL default interface
#define SBL_DEV_INTERFACE_UART          0 //!< External device SBL set for UART
#define SBL_DEV_INTERFACE_SPI           1 //!< External device SBL set for SPI
#define SBL_DEV_INTERFACE_UNDEFINED     9 //!< External device SBL undefined

//! \brief SBL Image location types
#define SBL_IMAGE_TYPE_INT              0 //!< Image located in internal flash
#define SBL_IMAGE_TYPE_EXT              1 //!< Image located in external flash

//! \brief Response codes for SBL
#define SBL_SUCCESS                     0
#define SBL_DEV_ACK                     1
#define SBL_DEV_NACK                    2
#define SBL_FAILURE                     3

//! \brief Target and Device specific defines
#define SBL_MAX_TRANSFER                252
#define SBL_PAGE_SIZE                   4096

/*********************************************************************
 * TYPEDEFS
 */

typedef struct
{
  Uint8       targetInterface;          //!< SBL_DEV_INTERFACE_[UART,SPI]
  Uint8       localInterfaceID;         //!< Device Interface (i.e. CC2650_UART0, CC2650_SPI0, etc.)
  Uint32      resetPinID;               //!< Local PIN ID connected to target RST PIN
  Uint32      blPinID;                  //!< Local PIN ID connected to target BL PIN
} SBL_Params;

typedef struct
{
  Uint8       imgType;                //!< SBL_IMAGE_TYPE_[INT,EXT]
  Uint32      imgInfoLocAddr;         //!< Address of image info header
  Uint32      imgLocAddr;             //!< Local address of image to use for SBL
  Uint32      imgTargetAddr;          //!< Target address to write image
} SBL_Image;

/*********************************************************************
*  EXTERNAL VARIABLES
*/
extern const SBL_Params SBL_defaultParams;

/*********************************************************************
 * FUNCTIONS
 */

extern void SBL_initParams(SBL_Params *params);

extern Uint8 SBL_open(SBL_Params *params);

extern Uint8 SBL_openTarget( void );

extern void SBL_closeTarget( void );

extern Uint8 SBL_resetTarget(Uint32 rstPinID, Uint32 blPinID);

extern Uint8 SBL_eraseFlash(Uint32 addr, Uint32 size);

extern Uint8 SBL_writeImage(SBL_Image *image);

extern Uint8 SBL_close(void);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* SBL_H */
