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
/* $Id: ata_labels.c,2.4+ $													    */
/*										    									*/
/* This is proprietary information, not to be published -- TI INTERNAL DATA   	*/
/* Copyright (C) 2000, Texas Instruments, Inc.  All Rights Reserved.		  	*/
/*																				*/
/* Author: Susmit Pal													  		*/
/* Date: November 6, 2002														*/
/* Modified Date: November 28, 2002												*/
/* Last Modified Date: February 11, 2003										*/
/********************************************************************************/
/*Moving some functions here for use in ROM - Susmit*/
#include "ata.h"
#include "ata_.h"

AtaError _AtaFindVol(AtaFile *pAtaFile)
{
  AtaState *pAtaDrive = pAtaFile->pDrive;
  AtaError ret_stat = ATA_ERROR_NONE;
  while(1)
  {
	/* -----------------9/8/99 2:46PM--------------------
	Check to see if we are out of directory entry locations
	--------------------------------------------------*/
/*SUSMIT - Max File Creation Error code Bug FIX*/
/*	if(pAtaFile->CurrentDirSector == pAtaDrive->FirstDataSector)*/
	if((pAtaFile->CurrentDirSector == pAtaDrive->FirstDataSector)||(pAtaFile->CurrentDirSector == pAtaDrive->RootDirSector))
/*SUSMIT - Max File Creation Error code Bug FIX*/
	{
	  if((pAtaFile->CurrentDirEntry >= pAtaDrive->RootDirEntries) && !(pAtaDrive->FatType == ATA_FAT_TYPE_32))
	  {
		ret_stat = ATA_ERROR_FILE_NOT_FOUND;
		break;
	  }
	}
	else
	{
	  /* -----------------9/8/99 2:52PM--------------------
	  If the directory is a sub-directory, the length of the
	   directory entry table is determined by the FAT.  A
	   test must be placed here to see if we are about to run
	   over the directory entry table into a file.
	   --------------------------------------------------*/
	}

	/* -----------------9/8/99 3:28PM--------------------
	Read directory entry
	 --------------------------------------------------*/
	ret_stat = _AtaGetDirEntry(pAtaFile);
	if(ret_stat) return(ret_stat);

	if(pAtaFile->Filename[0] == '\0')
	{
	return(ATA_ERROR_EOF);
	}
	/* -----------------9/8/99 3:28PM--------------------
	Determine validity of the directory entry
	 --------------------------------------------------*/
/*	if(pAtaFile->Attributes & ATA_ATTR_VOLUME)*//*SUSMIT ARCHIVE+VOL*/
	if(pAtaFile->Attributes == 0x28)
	{
	return(ATA_ERROR_NONE);
	  /*-----------------1/19/00 4:03PM-------------------
	   * Volume label
	   * --------------------------------------------------*/
	}
	pAtaFile->CurrentDirEntry++;
  }

  return(ret_stat);
}

