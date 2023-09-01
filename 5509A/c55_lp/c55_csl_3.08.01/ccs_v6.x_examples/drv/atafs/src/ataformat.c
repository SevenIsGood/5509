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
/* $Id: ataformat.c,2.4+ $													    */
/*										    									*/
/* This is proprietary information, not to be published -- TI INTERNAL DATA   	*/
/* Copyright (C) 2000, Texas Instruments, Inc.  All Rights Reserved.		  	*/
/*																				*/
/* Author: Susmit Pal													  		*/
/* Date: November 6, 2002														*/
/* Modified Date: November 28, 2002												*/
/* Last Modified Date: February 11, 2003										*/
/* April 28 2010 added check for NULL point KR042810                            */
/********************************************************************************/
/*-----------------2002-08-20 3:25PM----------------
 * A low-level format function (which we need) would
 * examine the information returned by the media
 * geometry descriptors.  The low-level drivers likely
 * need to be modified to pass that information back
 * to ATA_format().
 * --------------------------------------------------*/

#include "ata.h"
#include "ata_.h"


AtaError ATA_format(AtaState *pDrive, int flag)
{
	AtaSector testsector, testsector1;
	int x; AtaError ret_stat = ATA_ERROR_NONE;
    AtaUint16 *_AtaWriteBuffer = pDrive->_AtaWriteBuffer;
    AtaUint16 _AtaWriteBuffer1[256];
/*SUSMIT - FAT32 format fix*/
  AtaSector PhySector;
  AtaUint16 fOffset, fAlign;
  AtaCluster write_cluster, RootCluster;
  int LastCluster = 0;
  unsigned short FatNo;
/*SUSMIT - FAT32 format fix*/

    if (pDrive==NULL) return(ATA_ERROR_INVALID_PARAM); //KR042810

	for(x=0; x< 256; x++) _AtaWriteBuffer1[x] = 0;
/*Removed this on order to reduce memory, we won't remember boot records
so, our format won't be able to fix disks with Bad MBR/BR but will
be able to format*/
#if 0
	switch(pDrive->FatType) {
      case ATA_FAT_TYPE_12:
        /*write MBR*/
        ret_stat = _AtaReadSector((AtaSector)0, pDrive, &_AtaWriteBuffer[0], 0);
        if(ret_stat) return(ret_stat);
        ret_stat = pDrive->AtaWriteSector((AtaSector)0, pDrive->pAtaMediaState, &MBR_8MB_FAT12_MMC[0], 0);
        if(ret_stat) return(ret_stat);
        /*Write BR*/
        ret_stat = _AtaReadSector((AtaSector)pDrive->BootPhySector, pDrive, &_AtaWriteBuffer[0], 0);
        if(ret_stat) return(ret_stat);
        ret_stat = pDrive->AtaWriteSector((AtaSector)pDrive->BootPhySector, pDrive->pAtaMediaState, &BR_8MB_FAT12_MMC[0], 0);
        if(ret_stat) return(ret_stat);
        break;
      case ATA_FAT_TYPE_16:
        /*Write MBR*/
        ret_stat = _AtaReadSector((AtaSector)0, pDrive, &_AtaWriteBuffer[0], 0);
        if(ret_stat) return(ret_stat);
        ret_stat = pDrive->AtaWriteSector((AtaSector)0, pDrive->pAtaMediaState, &MBR_32MB_FAT16_MMC[0], 0);
        if(ret_stat) return(ret_stat);
        /*Write BR*/
        ret_stat = _AtaReadSector((AtaSector)pDrive->BootPhySector, pDrive, &_AtaWriteBuffer[0], 0);
        if(ret_stat) return(ret_stat);
        ret_stat = pDrive->AtaWriteSector((AtaSector)pDrive->BootPhySector, pDrive->pAtaMediaState, &BR_32MB_FAT16_MMC[0], 0);
        if(ret_stat) return(ret_stat);
        break;
      case ATA_FAT_TYPE_32:
        /*Write MBR*/
        ret_stat = _AtaReadSector((AtaSector)0, pDrive, &_AtaWriteBuffer[0], 0);
        if(ret_stat) return(ret_stat);
        ret_stat = pDrive->AtaWriteSector((AtaSector)0, pDrive->pAtaMediaState, &MBR_32MB_FAT16_MMC[0], 0);
        if(ret_stat) return(ret_stat);
        /*Write BR*/
        ret_stat = _AtaReadSector((AtaSector)pDrive->BootPhySector, pDrive, &_AtaWriteBuffer[0], 0);
        if(ret_stat) return(ret_stat);
        ret_stat = pDrive->AtaWriteSector((AtaSector)pDrive->BootPhySector, pDrive->pAtaMediaState, &BR_32MB_FAT16_MMC[0], 0);
        if(ret_stat) return(ret_stat);
        break;
      default: return(ATA_ERROR_BAD_MEDIA);
    }
#endif
	/*Clear FAT*/
	if(pDrive->FatType == ATA_FAT_TYPE_32) {
/*SUSMIT - FAT32 format fix*/
    RootCluster = _AtaReadDoubleWord(pDrive->BootPhySector, pDrive, 44);			/* Fat Size/Sector Count  byte 36 - 39 */
	ret_stat = _AtaReadSector((AtaSector)pDrive->RootDirSector, pDrive, &_AtaWriteBuffer[0], 0);
	if(ret_stat) return(ret_stat);
	_AtaWriteBuffer1[0] = 0xFFF8;
	_AtaWriteBuffer1[1] = 0xFFFF;
	_AtaWriteBuffer1[2] = 0xFFF8;
	_AtaWriteBuffer1[3] = 0xFFFF;
	_AtaWriteBuffer1[4] = 0xFFF8;
	_AtaWriteBuffer1[5] = 0xFFFF;
	ret_stat = pDrive->AtaWriteSector((AtaSector)pDrive->RootDirSector, pDrive->pAtaMediaState, &_AtaWriteBuffer1[0], 0);
	if(ret_stat) return(ret_stat);
    ret_stat = pDrive->AtaWriteSectorFlush(pDrive->pAtaMediaState);
    if(ret_stat) return(ret_stat);
	for(FatNo=0; FatNo<pDrive->NumberOfFats; FatNo++)
	{
      PhySector = _AtaCalculatePhySectorAndOffsetOfFatEntry(RootCluster, pDrive, &fOffset, &fAlign);
		PhySector +=
		(
		  (unsigned long)FatNo *
		  pDrive->LogSectorsPerFat *
		  (unsigned long)(pDrive->WordsPerLogSector/ATA_WORDS_PER_PHY_SECTOR)
		);
	  ret_stat = _AtaReadFatEntryAtPhySector(PhySector, pDrive, fOffset, fAlign, &LastCluster, &write_cluster);
	  if(ret_stat) return(ret_stat);
      ret_stat = _AtaWriteFatEntryAtPhySector(PhySector, pDrive, 0x0FFFFFFF, fOffset, fAlign, 1,1);
	  if(ret_stat) return(ret_stat);
	}
/*SUSMIT - FAT32 format fix*/
	}
	else {
	testsector=pDrive->FirstFatSector;
	ret_stat = _AtaReadSector((AtaSector)testsector, pDrive, &_AtaWriteBuffer[0], 0);
	if(ret_stat) return(ret_stat);
	_AtaWriteBuffer1[0] = 0xFFF8;
	_AtaWriteBuffer1[1] = 0xFFFF;
	ret_stat = pDrive->AtaWriteSector((AtaSector)testsector++, pDrive->pAtaMediaState, &_AtaWriteBuffer1[0], 0);
	if(ret_stat) return(ret_stat);
    ret_stat = pDrive->AtaWriteSectorFlush(pDrive->pAtaMediaState);
    if(ret_stat) return(ret_stat);
	_AtaWriteBuffer1[0] = 0;
	_AtaWriteBuffer1[1] = 0;
	testsector1 = pDrive->FirstFatSector+pDrive->LogSectorsPerFat;
	while(testsector<testsector1) {
      ret_stat = _AtaReadSector((AtaSector)testsector, pDrive, &_AtaWriteBuffer[0], 0);
      if(ret_stat) return(ret_stat);
      ret_stat = pDrive->AtaWriteSector((AtaSector)testsector++, pDrive->pAtaMediaState, &_AtaWriteBuffer1[0], 0);
      if(ret_stat) return(ret_stat);
      ret_stat = pDrive->AtaWriteSectorFlush(pDrive->pAtaMediaState);
      if(ret_stat) return(ret_stat);
    }
    ret_stat = _AtaReadSector((AtaSector)testsector, pDrive, &_AtaWriteBuffer[0], 0);
	if(ret_stat) return(ret_stat);
	_AtaWriteBuffer1[0] = 0xFFF8;
	_AtaWriteBuffer1[1] = 0xFFFF;
	ret_stat = pDrive->AtaWriteSector((AtaSector)testsector++, pDrive->pAtaMediaState, &_AtaWriteBuffer1[0], 0);
	if(ret_stat) return(ret_stat);
    ret_stat = pDrive->AtaWriteSectorFlush(pDrive->pAtaMediaState);
    if(ret_stat) return(ret_stat);
	_AtaWriteBuffer1[0] = 0;
	_AtaWriteBuffer1[1] = 0;
	testsector1 = pDrive->FirstDataSector;
	while(testsector<testsector1) {
      ret_stat = _AtaReadSector((AtaSector)testsector, pDrive, &_AtaWriteBuffer[0], 0);
      if(ret_stat) return(ret_stat);
      ret_stat = pDrive->AtaWriteSector((AtaSector)testsector++, pDrive->pAtaMediaState, &_AtaWriteBuffer1[0], 0);
      if(ret_stat) return(ret_stat);
      ret_stat = pDrive->AtaWriteSectorFlush(pDrive->pAtaMediaState);
      if(ret_stat) return(ret_stat);
    } }
	/*only for full format*/
	if (flag) {
	testsector = pDrive->FirstDataSector;
	while(testsector<=pDrive->TotalSectors) {
      ret_stat = _AtaReadSector((AtaSector)testsector, pDrive, &_AtaWriteBuffer[0], 0);
      if(ret_stat) return(ret_stat);
      ret_stat = pDrive->AtaWriteSector((AtaSector)testsector++, pDrive->pAtaMediaState, &_AtaWriteBuffer1[0], 0);
      if(ret_stat) return(ret_stat);
      ret_stat = pDrive->AtaWriteSectorFlush(pDrive->pAtaMediaState);
      if(ret_stat) return(ret_stat);
    } }
/* SUSMIT - 25 FEB 2003 - NMP*/
/*	ret_stat = ATA_systemInit(pDrive);*/
	ret_stat = ATA_systemInit(pDrive,0);
/* SUSMIT - 25 FEB 2003 - NMP*/
	return(ret_stat);
}
