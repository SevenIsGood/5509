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
/* $Id: ata_fileinit.c,2.4+ $												    */
/*										    									*/
/* This is proprietary information, not to be published -- TI INTERNAL DATA   	*/
/* Copyright (C) 2000, Texas Instruments, Inc.  All Rights Reserved.		  	*/
/*																				*/
/* Author: Susmit Pal													  		*/
/* Date: November 6, 2002														*/
/* Modified Date: November 28, 2002												*/
/* Last Modified Date: February 11, 2003										*/
/* April 21, 2010 - added NULL check of pointers KR042110                       */
/********************************************************************************/

#include "ata.h"
#include "ata_.h"

extern  AtaCluster start_FAT_cluster; //KR012210

AtaError ATA_fileInit(AtaState *pAtaDrive, AtaFile *pAtaFile)
{
  AtaError kk = ATA_ERROR_NONE;
  pAtaFile->pDrive = pAtaDrive;

  if ((pAtaDrive==NULL)||(pAtaFile==NULL)) return(ATA_ERROR_INVALID_PARAM); //KR042110

  start_FAT_cluster=ATA_FIRST_CLUSTER; //KR012210

  kk = ATA_cdRoot(pAtaFile);
  if(kk) return(kk);
/*  return(AtaFindFirst(pAtaFile));	*/	/*This is not a bug fix, this is a workaround so that ATA_fileInit doesn't return error for a formatted MMC - Susmit*/
  kk=ATA_findFirst(pAtaFile);
  if((kk==ATA_ERROR_NONE)||(kk==ATA_ERROR_FILE_NOT_FOUND)) return(ATA_ERROR_NONE);
  else return(kk);
}

