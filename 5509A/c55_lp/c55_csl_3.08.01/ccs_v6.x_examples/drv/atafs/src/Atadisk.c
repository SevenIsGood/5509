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
/* $Id: atadisk.c,2.4+ $													    */
/*										    									*/
/* This is proprietary information, not to be published -- TI INTERNAL DATA   	*/
/* Copyright (C) 2000, Texas Instruments, Inc.  All Rights Reserved.		  	*/
/*																				*/
/* Author: Susmit Pal													  		*/
/* Date: November 6, 2002														*/
/* Modified Date: November 28, 2002												*/
/* Last Modified Date: February 11, 2003										*/
/* April 26,2010 - added check for NULL pointer KR042610                        */
/********************************************************************************/

#include "ata.h"
#include "ata_.h"

AtaUint32 ATA_diskSize(AtaState *pAtaState,AtaError *ret_stat)
{  /*returns in KB.*/
/*  return((pAtaState->TotalSectors * pAtaState->WordsPerLogSector)/512);*/ /*Overflows - Susmit*/
/*	return((pAtaState->TotalSectors / 512)*pAtaState->WordsPerLogSector);*/ /*SUSMIT - Original Implementation*/
  if (pAtaState==NULL) *ret_stat = ATA_ERROR_INVALID_PARAM; //KR042610
  else                 *ret_stat = ATA_ERROR_NONE;
	return(((pAtaState->TotalSectors)*((pAtaState->WordsPerLogSector*2)/512))/2);
}

#if 0
AtaUint32 ATA_diskUsed(AtaState *pAtaState, AtaError *ret_stat)
{  /*returns in KB.*/
  int i,j;
  AtaUint32 size=0;
  AtaUint32 tempsize;
  AtaSector firstFat=pAtaState->FirstFatSector;
  AtaUint16 *_AtaWriteBuffer = pAtaState->_AtaWriteBuffer;

  if (pAtaState==NULL) *ret_stat = ATA_ERROR_INVALID_PARAM; //KR042610
  else                 *ret_stat = ATA_ERROR_NONE;
  for(i=0;i<pAtaState->LogSectorsPerFat;i++) {
  *ret_stat = _AtaReadSector(firstFat++, pAtaState, &_AtaWriteBuffer[0], 0);
  tempsize=0;
  for(j=0;j<ATA_WORDS_PER_PHY_SECTOR;j++)
  if(_AtaWriteBuffer[j]!=0) tempsize++;
  size+=tempsize; }
/*  return ((size*pAtaState->WordsPerCluster)/512);*/ /*SUSMIT - Original Implementation*/
/*  return (size*(pAtaState->WordsPerCluster/512));*//*SUSMIT - Fails for disks with cluster size of 512 bytes*/
  return ((size*(pAtaState->WordsPerCluster/256))/2);
}
#endif

AtaUint32 ATA_diskUsed(AtaState *pAtaState, AtaError *ret_stat)
{  /*returns in KB.*/
  int i,j, LastCluster = 0;
  AtaUint32 size=0;
  AtaUint32 tempsize, fatmarker = 0;
  AtaCluster NumClusters = 0, tempCluster = 0;AtaSector PhySector = 0;
  AtaUint16 FatEntryOffset = 0, FatEntryAlignment = 0;
  AtaSector firstFat=pAtaState->FirstFatSector;
  AtaUint16 *_AtaWriteBuffer = pAtaState->_AtaWriteBuffer;
  if (pAtaState==NULL) *ret_stat = ATA_ERROR_INVALID_PARAM; //KR042610
  else                 *ret_stat = ATA_ERROR_NONE;

  *ret_stat = ATA_ERROR_NONE;
  switch(pAtaState->FatType) {
  	case ATA_FAT_TYPE_12:
		  NumClusters = (AtaCluster)((pAtaState->TotalSectors-pAtaState->RootDirSector)/pAtaState->LogSectorsPerCluster);
		  for(i=0;i<NumClusters;i++) {
			PhySector = _AtaCalculatePhySectorAndOffsetOfFatEntry((AtaCluster)i, pAtaState, &FatEntryOffset, &FatEntryAlignment);
			*ret_stat = _AtaReadFatEntryAtPhySector(PhySector, pAtaState, FatEntryOffset, FatEntryAlignment, &LastCluster, &tempCluster);
		  	if(tempCluster!=0) size++; }
		  break;
  	case ATA_FAT_TYPE_16:
		  for(i=0;i<pAtaState->LogSectorsPerFat;i++) {
		  *ret_stat = _AtaReadSector(firstFat++, pAtaState, &_AtaWriteBuffer[0], 0);
		  tempsize=0;
		  for(j=0;j<ATA_WORDS_PER_PHY_SECTOR;j++)
		  if(_AtaWriteBuffer[j]!=0) tempsize++;
		  size+=tempsize; }
		  break;
  	case ATA_FAT_TYPE_32:
		  for(i=0;i<pAtaState->LogSectorsPerFat;i++) {
		  *ret_stat = _AtaReadSector(firstFat++, pAtaState, &_AtaWriteBuffer[0], 0);
		  tempsize=0;
		  for(j=0;j<ATA_WORDS_PER_PHY_SECTOR;j+=2) {
		  	fatmarker = (AtaUint32)_AtaWriteBuffer[j];
		  	fatmarker = (fatmarker&0x0000ffff) << 16;
		  	fatmarker |= (AtaUint32)_AtaWriteBuffer[j+1];
		  	if(fatmarker!=0) tempsize++; }
		  size+=tempsize; }
		  break;
  	default:
		  break;
  }
/*  return ((size*pAtaState->WordsPerCluster)/512);*/ /*SUSMIT - Original Implementation*/
/*  return (size*(pAtaState->WordsPerCluster/512));*//*SUSMIT - Fails for disks with cluster size of 512 bytes*/
  return ((size*(pAtaState->WordsPerCluster/256))/2);
}

