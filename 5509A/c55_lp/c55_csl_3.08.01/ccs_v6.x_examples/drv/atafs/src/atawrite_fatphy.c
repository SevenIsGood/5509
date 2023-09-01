/* ============================================================================
 * Copyright (c) 2008-2012 Texas Instruments Incorporated.
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


/********************************************************************************/
/* $Id: atawrite_fatphy.c,2.4+ $											    */
/*										    									*/
/* This is proprietary information, not to be published -- TI INTERNAL DATA   	*/
/* Copyright (C) 2000, Texas Instruments, Inc.  All Rights Reserved.		  	*/
/*																				*/
/* Author: Susmit Pal													  		*/
/* Date: November 6, 2002														*/
/* Modified Date: November 28, 2002												*/
/* Last Modified Date: February 11, 2003										*/
/********************************************************************************/

#include "ata_.h"


AtaError _AtaWriteFatEntryAtPhySector(
  AtaSector PhySector,
  AtaState *pAtaDrive,
  AtaCluster FatEntry,
  AtaUint16 FatEntryOffset,
  AtaUint16 FatEntryAlignment,
  int FlushSector,
/*Susmit - Scandisk bug fix*/
  unsigned long eofflag
/*Susmit - Scandisk bug fix*/
)
{
  AtaError ret_stat = ATA_ERROR_NONE;
  AtaUint16 *_AtaWriteBuffer = pAtaDrive->_AtaWriteBuffer;
  /*-----------------2002-08-20 6:19PM----------------
   * pAtaDrive->_AtaWriteCurrentPhySector wouldn't have
   * to be saved so often if the error checking was
   * done less frequently.
   * --------------------------------------------------*/
  AtaSector _AtaWriteCurrentPhySector = pAtaDrive->_AtaWriteCurrentPhySector;

  /* -----------------9/3/99 2:18PM--------------------
  Read sector with desired FAT entries
  --------------------------------------------------*/
  if(_AtaWriteCurrentPhySector != PhySector)
  {
	if(_AtaWriteCurrentPhySector != ATA_INVALID_SECTOR)
	{
	  ret_stat = _AtaFlush(pAtaDrive);
	  if (ret_stat) return(ret_stat);
	  ret_stat = pAtaDrive->AtaWriteSector(_AtaWriteCurrentPhySector, pAtaDrive->pAtaMediaState, &_AtaWriteBuffer[0], 0);
	  if (ret_stat) return(ret_stat);
	  ret_stat = pAtaDrive->AtaWriteSectorFlush(pAtaDrive->pAtaMediaState);
	  if (ret_stat) return(ret_stat);
	}
    pAtaDrive->_AtaWriteCurrentPhySector = _AtaWriteCurrentPhySector = PhySector;
	ret_stat = _AtaReadSector(_AtaWriteCurrentPhySector, pAtaDrive, &_AtaWriteBuffer[0], 0);
    if(ret_stat) return(ret_stat);
  }

#ifndef DISABLE_FAT32
  if(pAtaDrive->FatType == ATA_FAT_TYPE_32)
  {
/*Susmit - Scandisk bug fix*/
	if(eofflag) {
/*Susmit - Scandisk bug fix*/
	_AtaWriteBuffer[FatEntryOffset++] = (FatEntry & 0xFFFF);
	_AtaWriteBuffer[FatEntryOffset] = ((FatEntry >> 16) & 0x0FFF) | (_AtaWriteBuffer[FatEntryOffset] & 0xF000);
/*Susmit - Scandisk bug fix*/
	}
/*Susmit - Scandisk bug fix*/
  }
  else if(pAtaDrive->FatType == ATA_FAT_TYPE_16)
#else
  if(pAtaDrive->FatType == ATA_FAT_TYPE_16)
#endif
  {
/*Susmit - Scandisk bug fix*/
	if(eofflag) _AtaWriteBuffer[FatEntryOffset] = FatEntry;
/*Susmit - Scandisk bug fix*/
  }									 /* end of ATA_FAT_TYPE_16 */
#ifndef DISABLE_FAT12
  else if(pAtaDrive->FatType == ATA_FAT_TYPE_12)
  {
	switch(FatEntryAlignment)
	{
	  case 0:
	  {
		/* In lower 3/4 of word */
		if(eofflag)
		_AtaWriteBuffer[FatEntryOffset] = (_AtaWriteBuffer[FatEntryOffset] & 0xF000) | (FatEntry & 0x0FFF);
		break;
	  }
	  case 1:
	  {
		/* First nibble in first word */
		if(eofflag)
		_AtaWriteBuffer[FatEntryOffset] = (_AtaWriteBuffer[FatEntryOffset] & 0x0FFF) | ((FatEntry & 0x000F) << 12);
		FatEntryOffset++;
		if(FatEntryOffset >= ATA_WORDS_PER_PHY_SECTOR)
		{
		  FatEntryOffset = 0;
		  ret_stat = _AtaFlush(pAtaDrive);
          if (ret_stat) return(ret_stat);
		  ret_stat = pAtaDrive->AtaWriteSector(_AtaWriteCurrentPhySector, pAtaDrive->pAtaMediaState, &_AtaWriteBuffer[0], 0);
          if (ret_stat) return(ret_stat);
		  ret_stat = pAtaDrive->AtaWriteSectorFlush(pAtaDrive->pAtaMediaState);
          if (ret_stat) return(ret_stat);
/*SUSMIT - New Fix for FAT12*/
/*          pAtaDrive->_AtaWriteCurrentPhySector = _AtaWriteCurrentPhySector++;*/
          pAtaDrive->_AtaWriteCurrentPhySector = ++_AtaWriteCurrentPhySector;
/*SUSMIT - New Fix for FAT12*/
		  ret_stat = _AtaReadSector(_AtaWriteCurrentPhySector, pAtaDrive, &_AtaWriteBuffer[0], 0);
		  if(ret_stat) return(ret_stat);
		}
		if(eofflag)
		_AtaWriteBuffer[FatEntryOffset] = (_AtaWriteBuffer[FatEntryOffset] & 0xFF00) | ((FatEntry & 0x0FF0) >> 4);
		break;
	  }
	  case 2:
	  {
		/* First byte in first word */
		if(eofflag)
		_AtaWriteBuffer[FatEntryOffset] = (_AtaWriteBuffer[FatEntryOffset] & 0x00FF) | ((FatEntry & 0x00FF) << 8);
		FatEntryOffset++;
		if(FatEntryOffset >= ATA_WORDS_PER_PHY_SECTOR)
		{
		  FatEntryOffset = 0;
		  ret_stat = _AtaFlush(pAtaDrive);
          if (ret_stat) return(ret_stat);
		  ret_stat = pAtaDrive->AtaWriteSector(_AtaWriteCurrentPhySector, pAtaDrive->pAtaMediaState, &_AtaWriteBuffer[0], 0);
          if (ret_stat) return(ret_stat);
		  ret_stat = pAtaDrive->AtaWriteSectorFlush(pAtaDrive->pAtaMediaState);
          if (ret_stat) return(ret_stat);
/*SUSMIT - New Fix for FAT12*/
/*          pAtaDrive->_AtaWriteCurrentPhySector = _AtaWriteCurrentPhySector++;*/
          pAtaDrive->_AtaWriteCurrentPhySector = ++_AtaWriteCurrentPhySector;
/*SUSMIT - New Fix for FAT12*/
		  ret_stat = _AtaReadSector(_AtaWriteCurrentPhySector, pAtaDrive, &_AtaWriteBuffer[0], 0);
		  if(ret_stat) return(ret_stat);
		}
		if(eofflag)
		_AtaWriteBuffer[FatEntryOffset] = (_AtaWriteBuffer[FatEntryOffset] & 0xFFF0) | ((FatEntry & 0x0F00) >> 8);
		break;
	  }
	  case 3:
	  {
		/* In upper 3/4 of word */
		if(eofflag)
		_AtaWriteBuffer[FatEntryOffset] = (_AtaWriteBuffer[FatEntryOffset] & 0x000F) | ((FatEntry & 0x0FFF) << 4);
		break;
	  }
	  default:
		break;
	}
  }									 /* end of ATA_FAT_TYPE_12 */
#endif
  else
  {
	while(1);
  }

  if(FlushSector)
  {
	ret_stat = _AtaFlush(pAtaDrive);
    if (ret_stat) return(ret_stat);
	ret_stat = pAtaDrive->AtaWriteSector(_AtaWriteCurrentPhySector, pAtaDrive->pAtaMediaState, &_AtaWriteBuffer[0], 0);
    if (ret_stat) return(ret_stat);
	ret_stat = pAtaDrive->AtaWriteSectorFlush(pAtaDrive->pAtaMediaState);
    if (ret_stat) return(ret_stat);
    pAtaDrive->_AtaWriteCurrentPhySector = _AtaWriteCurrentPhySector = ATA_INVALID_SECTOR;
  }

  return(ret_stat);
}

