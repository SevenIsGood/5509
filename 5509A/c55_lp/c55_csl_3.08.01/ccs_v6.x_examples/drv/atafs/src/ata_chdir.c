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
/* $Id: ata_chdir.c,2.4+ $													    */
/*										    									*/
/* This is proprietary information, not to be published -- TI INTERNAL DATA   	*/
/* Copyright (C) 2000, Texas Instruments, Inc.  All Rights Reserved.		  	*/
/*																				*/
/* Author: Susmit Pal													  		*/
/* Date: November 6, 2002														*/
/* Modified Date: November 28, 2002												*/
/* Last Modified Date: February 11, 2003										*/
/* April 26, 2010 - added NULL check of pointers KR042610                       */
/********************************************************************************/

#include "ata.h"
#include "ata_.h"

AtaError ATA_cdRoot(AtaFile *pAtaFile)
{
  AtaState *pAtaDrive = pAtaFile->pDrive;

  if ((pAtaFile==NULL)||(pAtaDrive==NULL)) return(ATA_ERROR_INVALID_PARAM); //KR042610

  /*-----------------12/13/00 1:30PM------------------
   * In FAT32 case RootDirSector is set to FirstDataSector
   * in AtaSystemInit
   * --------------------------------------------------*/
  pAtaFile->CurrentDirSector = pAtaDrive->RootDirSector;
  pAtaFile->StartDirCluster = 0;
  if(pAtaDrive->FatType == ATA_FAT_TYPE_32)
#ifdef ATA_HDD
  pAtaFile->StartCluster = pAtaFile->StartDirCluster = _AtaReadDoubleWord(pAtaDrive->BootPhySector, pAtaDrive, 44);			/* Fat Size/Sector Count  byte 36 - 39 */
#else /* ATA_HDD */
  pAtaFile->StartDirCluster = _AtaReadDoubleWord(pAtaDrive->BootPhySector, pAtaDrive, 44);			/* Fat Size/Sector Count  byte 36 - 39 */
#endif /* ATA_HDD */
  /*FIX for ROOT DIR of FAT32 disk browse - Susmit*/
  /*SUSMIT - Actually the StartDirCluster for root dir is zero for FAT12/16 but not for FAT32*/
  pAtaFile->CurrentDirSectorTag = 0;
  pAtaFile->CurrentDirEntry = 0;
/*  pAtaFile->CurrentDirCluster = 0;*/   /*SUSMIT_BUGFIX-Directory Browsing BUgfix*/
  return(pAtaDrive->Err_Status);
}

AtaError ATA_cd(AtaFile *pAtaFile)
{

  if (pAtaFile==NULL) return(ATA_ERROR_INVALID_PARAM); // KR042610

/*  AtaState *pAtaDrive = pAtaFile->pDrive;*//*SUSMIT_BUGFIX-Directory Browsing BUgfix*/
  if(pAtaFile->StartCluster)
  {
 /*SUSMIT_BUGFIX-Directory Browsing BUgfix*/
/*	pAtaFile->CurrentDirSector =
	  pAtaDrive->FirstDataSector +
	  (AtaSector)((AtaSector)(pAtaFile->StartCluster-2) * (AtaSector)pAtaDrive->LogSectorsPerCluster);*/
	pAtaFile->CurrentDirSectorTag = 0xFFFFFFFF; // Mistral: It was: -1L;
	pAtaFile->CurrentDirEntry = 0;
  }
  else
  {
 /*SUSMIT_BUGFIX-Directory Browsing BUgfix*/
/*	pAtaFile->CurrentDirSector = pAtaDrive->RootDirSector;*/
	return ATA_cdRoot(pAtaFile);
  }
 /*SUSMIT_BUGFIX-Directory Browsing BUgfix*/
/*  pAtaFile->CurrentDirCluster = pAtaFile->StartCluster;*/
  pAtaFile->StartDirCluster = pAtaFile->StartCluster;
  return(ATA_ERROR_NONE);
}

