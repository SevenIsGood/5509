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
  Filename:       sbl_cmd.h
  Revised:        $Date: 2015-06-22 20:21:02 -0700 (Mon, 22 Jun 2015) $
  Revision:       $Revision: 44191 $

  Description:    This file contains SBL commands

**************************************************************************************************/

#ifndef SBL_CMD_H
#define SBL_CMD_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
*  EXTERNAL VARIABLES
*/

/*********************************************************************
 * CONSTANTS
 */

//!< \brief Serial Bootloader Command IDs for CC26xx
#define SBL_CMD_PING                    0x20
#define SBL_CMD_DOWNLOAD                0x21
#define SBL_CMD_GET_STATUS              0x23
#define SBL_CMD_SEND_DATA               0x24
#define SBL_CMD_RESET                   0x25
#define SBL_CMD_SECTOR_ERASE	        0x26
#define SBL_CMD_CRC32                   0x27
#define SBL_CMD_GET_CHIP_ID             0x28
#define SBL_CMD_MEMORY_READ             0x2A
#define SBL_CMD_MEMORY_WRITE            0x2B    // currently not supported
#define SBL_CMD_BANK_ERASE              0x2C
#define SBL_CMD_SET_CCFG                0x2D

//!< \brief Serial Bootloader Response IDs for CC26xx
#define SBL_CMD_RET_SUCCESS             0x40
#define SBL_CMD_RET_UNKNOWN_CMD         0x41
#define SBL_CMD_RET_INVALID_CMD         0x42
#define SBL_CMD_RET_INVALID_ADR         0x43
#define SBL_CMD_RET_FLASH_FAIL          0x44

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */

extern Uint8 SBL_CMD_ping(void);

extern Uint8 SBL_CMD_download(Uint32 addr, Uint32 size);

extern Uint8 SBL_CMD_getStatus(void);

extern Uint8 SBL_CMD_sendData(Uint16 *pData, Uint16 len);

extern Uint8 SBL_CMD_reset(void);

extern Uint8 SBL_CMD_sectorErase(Uint32 addr);

extern Uint8 SBL_CMD_crc32(Uint32 addr, Uint32 size, Uint32 readRepeatCnt);

extern Uint8 SBL_CMD_memoryRead(Uint32 addr, Uint8 numAccess, Uint8 *readRsp);

extern Uint32 SBL_CMD_getChipID(void);

extern Uint8 SBL_CMD_bankErase(void);

extern Uint8 SBL_CMD_setCCFG(Uint32 fieldID, Uint32 fieldValue);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* #define SBL_CMD_H */
