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
  @headerfile:    sbl_cmd.c
  Revised:        $Date: 2015-06-22 20:21:02 -0700 (Mon, 22 Jun 2015) $
  Revision:       $Revision: 44191 $

  Description:    This file contains SBL commands

**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */
//#include <xdc/std.h>

#include "sbl.h"
#include "sbl_cmd.h"
#include "sbl_tl.h"
#include "string.h"


/*********************************************************************
 * CONSTANTS
 */

#define SBL_MAX_BYTES_PER_TRANSFER   252

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static void SBL_CMD_uint32MSB(Uint32 src, Uint8 *pDst);
void SBL_CMD_uint32to16(Uint32 src, Uint16 *pDst);

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

Uint8 SBL_CMD_ping(void)
{
  return SBL_TL_sendCmd(SBL_CMD_PING, NULL, 0);
}

Uint8 SBL_CMD_download(Uint32 addr, Uint32 size)
{
  Uint16 payload[8];

  // Check input arguments
  if (size & 0x03)
  {
    // NEED TO CHECK: addr in range of device
    // CHECKING: byte count must be multiple of 4
    return SBL_FAILURE;
  }

  // Initialize payload - MSB order
  SBL_CMD_uint32to16(addr, &payload[0]);
  SBL_CMD_uint32to16(size, &payload[4]);

  return SBL_TL_sendCmd16(SBL_CMD_DOWNLOAD, payload, sizeof(payload));
}

Uint8 SBL_CMD_getStatus(void)
{
  Uint8 rsp[255];
  Uint16 rspLen = 0;

  // Send Get Status command
  if (SBL_TL_sendCmd(SBL_CMD_GET_STATUS, NULL, 0) == SBL_FAILURE)
  {
    return SBL_FAILURE;
  }

  // Get the status response from target
  SBL_TL_getRsp(rsp, sizeof(rsp), &rspLen);

  return rsp[0];
}

Uint8 SBL_CMD_sendData(Uint16 *pData, Uint16 len)
{
  // Check input arguments
  if (len > SBL_MAX_BYTES_PER_TRANSFER)
  {
    // Length of bytes excees maximum allowed per transfer
    return SBL_FAILURE;
  }

  // Send command
  return SBL_TL_sendCmd16(SBL_CMD_SEND_DATA, pData, len);
}

Uint8 SBL_CMD_reset(void)
{
  return SBL_TL_sendCmd(SBL_CMD_RESET, NULL, 0);
}

Uint8 SBL_CMD_sectorErase(Uint32 addr)
{
  Uint8 payload[4];

  // Initialize payload - MSB order
  SBL_CMD_uint32MSB(addr, &payload[0]);

  // Send command
  return SBL_TL_sendCmd(SBL_CMD_SECTOR_ERASE, payload, sizeof(payload));
}

Uint8 SBL_CMD_crc32(Uint32 addr, Uint32 size, Uint32 readRepeatCnt)
{
  Uint8 payload[12];

  // Initialize payload - MSB order
  SBL_CMD_uint32MSB(addr, &payload[0]);
  SBL_CMD_uint32MSB(size, &payload[4]);
  SBL_CMD_uint32MSB(readRepeatCnt, &payload[8]);

  return SBL_TL_sendCmd(SBL_CMD_CRC32, payload, sizeof(payload));
}

Uint8 SBL_CMD_memoryRead(Uint32 addr, Uint8 numAccess, Uint8 *readRsp)
{
  Uint8 payload[6];
  Uint16 rspLen = 0;


  // Initialize payload - MSB order
  SBL_CMD_uint32MSB(addr, &payload[0]);

  // Verify we do not exceed the max reads
  if (numAccess > 253)
  {
    numAccess = 253;
  }

  // 8 bit read access
  payload[4] = 0;
  payload[5] = numAccess;

  SBL_TL_sendCmd(SBL_CMD_MEMORY_READ, payload, sizeof(payload));

  return (SBL_TL_getRsp(readRsp, numAccess, &rspLen));
}

Uint32 SBL_CMD_getChipID(void)
{
  Uint8 rsp[4];
  Uint16 rspLen = 0;
  Uint32 chipID = 0;

  // Send Get Chip ID command
  if (SBL_TL_sendCmd(SBL_CMD_GET_CHIP_ID, NULL, 0) == SBL_FAILURE)
  {
    return SBL_FAILURE;
  }

  // Get the status response from target
  SBL_TL_getRsp(rsp, sizeof(rsp), &rspLen);

  // Reverse MSB order of response to get Chip ID
  SBL_CMD_uint32MSB(chipID, rsp);

  return chipID;
}

Uint8 SBL_CMD_bankErase(void)
{
  return SBL_TL_sendCmd(SBL_CMD_BANK_ERASE, NULL, 0);
}

Uint8 SBL_CMD_setCCFG(Uint32 fieldID, Uint32 fieldValue)
{
  Uint8 payload[8];

  // Initialize payload - MSB order
  SBL_CMD_uint32MSB(fieldID, &payload[0]);
  SBL_CMD_uint32MSB(fieldValue, &payload[4]);

  return SBL_TL_sendCmd(SBL_CMD_SET_CCFG, payload, sizeof(payload));
}

void SBL_CMD_uint32MSB(Uint32 src, Uint8 *pDst)
{
  // MSB first
  pDst[0] = (Uint8)((src >> 24) & 0xFF);
  pDst[1] = (Uint8)((src >> 16) & 0xFF);
  pDst[2] = (Uint8)((src >> 8) & 0xFF);
  pDst[3] = (Uint8)((src >> 0) & 0xFF);
}

void SBL_CMD_uint32to16(Uint32 src, Uint16 *pDst)
{
  // MSB first
  pDst[0] = (Uint8)((src >> 24) & 0xFF);
  pDst[1] = (Uint8)((src >> 16) & 0xFF);
  pDst[2] = (Uint8)((src >> 8) & 0xFF);
  pDst[3] = (Uint8)((src >> 0) & 0xFF);
}
