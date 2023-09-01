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
/* $Id: ataread_calc.c,2.4+ $												    */
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


AtaSector _AtaCalculatePhySectorAndOffsetFromCluster(AtaFile *pAtaFile, AtaUint16 *OffsetInSector)
{
  AtaState *pAtaDrive = pAtaFile->pDrive;
  AtaSector PhySector;

  AtaUint16 WordInCluster = pAtaFile->WordInCluster;

  *OffsetInSector = WordInCluster % ATA_WORDS_PER_PHY_SECTOR;

  PhySector =
	pAtaDrive->FirstDataSector +
	((AtaSector)(pAtaFile->Cluster-2) * (AtaSector)pAtaDrive->LogSectorsPerCluster) +
	(AtaSector)(WordInCluster / ATA_WORDS_PER_PHY_SECTOR);

  return(PhySector);
}

AtaSector _AtaCalculatePhySectorAndOffsetOfFatEntry
(
  AtaCluster Cluster,
  AtaState *pAtaDrive,
  AtaUint16 *pFatEntryOffset,
  AtaUint16 *pFatEntryAlignment
)
{
  AtaSector mm;

#ifndef DISABLE_FAT32
  if(pAtaDrive->FatType == ATA_FAT_TYPE_32)
  {
	*pFatEntryAlignment = 0;
	mm = (AtaSector)((Cluster)<<1);
  }
  else
#endif
  if(pAtaDrive->FatType == ATA_FAT_TYPE_16)
  {
	*pFatEntryAlignment = 0;
	mm = (AtaSector)(Cluster);
  } /* end of ATA_FAT_TYPE_16 */
#ifndef DISABLE_FAT12
  else if(pAtaDrive->FatType == ATA_FAT_TYPE_12)
  {
	*pFatEntryAlignment = Cluster&0x03;
	mm = (AtaSector)((Cluster*3)>>2);
  } /* end of ATA_FAT_TYPE_12 */
#endif
  else
  {
	/* -----------------04/14/2000 10:31AM---------------
	Bad FAT type
	 --------------------------------------------------*/
/*	while(1);*//*SUSMIT - It must not come here, if it comes, it means something is corrupt*/
	return(ATA_ERROR_UNSUPPORTED); /*Should be something like this - Susmit*/
  }

  *pFatEntryOffset = (AtaUint16)(mm % ATA_WORDS_PER_PHY_SECTOR);
  mm /= ATA_WORDS_PER_PHY_SECTOR;
  mm += pAtaDrive->FirstFatSector;
  return(mm);
}


