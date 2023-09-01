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
/* $Id: atawrite.c,2.4+ $													    */
/*										    									*/
/* This is proprietary information, not to be published -- TI INTERNAL DATA   	*/
/* Copyright (C) 2000, Texas Instruments, Inc.  All Rights Reserved.		  	*/
/*																				*/
/* Author: Susmit Pal													  		*/
/* Date: November 6, 2002														*/
/* Modified Date: November 28, 2002												*/
/* Last Modified Date: February 11, 2003										*/
/* April 26, 2010 - added check of NULL pointers KR042610                       */
/********************************************************************************/

#include <stdlib.h>
#include "ata_.h"

extern AtaCluster start_FAT_cluster; //KR012210

AtaError ATA_create(AtaFile *pAtaFile)
{
  AtaError ret_stat = ATA_ERROR_NONE;
  if (pAtaFile==NULL) return(ATA_ERROR_INVALID_PARAM); //KR042610
  pAtaFile->Size = 0L;
  ret_stat = _AtaCreate(pAtaFile, 0, 0);
  return(ret_stat);
}

AtaError ATA_createDirectory(AtaFile *pAtaFile)
{
  AtaError ret_stat = ATA_ERROR_NONE;
  if (pAtaFile==NULL) return(ATA_ERROR_INVALID_PARAM); //KR042610
  ret_stat = _AtaCreateDirectory(pAtaFile, 0);
  return(ret_stat);
}

AtaError ATA_rename(AtaFile* pAtaFile)
{
  AtaError ret_stat = ATA_ERROR_NONE;
  if (pAtaFile==NULL) return(ATA_ERROR_INVALID_PARAM); //KR042610

  ret_stat = _AtaCreate(pAtaFile,2,0);	/* 2 is for rename, adjusts offset calculated */

  return(ret_stat);
}

AtaError ATA_delete(AtaFile *pAtaFile)
{
  int last_cluster;
  AtaError ret_stat = ATA_ERROR_NONE;

  AtaState *pAtaDrive = pAtaFile->pDrive;

  if ((pAtaFile==NULL)||(pAtaDrive==NULL)) return(ATA_ERROR_INVALID_PARAM); //KR042610

  pAtaFile->PrevBrokenCluster = pAtaFile->NextBrokenCluster = pAtaFile->Cluster = pAtaFile->StartCluster;
  pAtaFile->Filename[0] = '\xE5';
  if(_AtaCreate(pAtaFile,2,0))
  {
	return(1);
  }
/*SUSMIT - Directory Delete FAT Updation Bug Fix*/
/* if(pAtaFile->Size!=0) {*/
/* if((pAtaFile->Size!=0)||(pAtaFile->Attributes == ATA_ATTR_DIRECTORY)) {*//*SUSMIT - Directory Attribute Fix*/
 if((pAtaFile->Size!=0)||(ATA_isDir(pAtaFile))) {
/*SUSMIT - Directory Delete FAT Updation Bug Fix*/
  do
  {
/*SUSMIT - New delete change due to modification in _AtaFindNextBrokenCluster*/
/*	ret_stat = _AtaFindNextBrokenCluster(pAtaFile,1);*/
	ret_stat = _AtaFindNextBrokenCluster(pAtaFile,0);
/*SUSMIT - New delete change due to modification in _AtaFindNextBrokenCluster*/
	if(ret_stat == ATA_ERROR_EOF) ret_stat = ATA_ERROR_NONE;
	if(ret_stat) return(ret_stat);

	ret_stat = _AtaReadFatEntry(pAtaFile->NextBrokenCluster, pAtaDrive, &last_cluster, &pAtaFile->Cluster);
	if(ret_stat) return(ret_stat);

	ret_stat = _AtaWriteFatEntry(pAtaFile,1);
    if(ret_stat) return(ret_stat);
  }
  while(!last_cluster);
 }

  start_FAT_cluster=ATA_FIRST_CLUSTER; //KR012210

  return(ret_stat);
}

