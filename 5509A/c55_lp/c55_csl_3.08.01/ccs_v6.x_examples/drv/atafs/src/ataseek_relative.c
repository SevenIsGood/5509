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
/* $Id: ataseek_relative.c,2.4+ $											    */
/*										    									*/
/* This is proprietary information, not to be published -- TI INTERNAL DATA   	*/
/* Copyright (C) 2000, Texas Instruments, Inc.  All Rights Reserved.		  	*/
/*																				*/
/* Author: Susmit Pal													  		*/
/* Date: November 6, 2002														*/
/* Modified Date: November 28, 2002												*/
/* Last Modified Date: February 11, 2003										*/
/* April 26, 2010 - added check for NULL pointer KR042610                       */
/********************************************************************************/
#include "ata_.h"

AtaError ATA_seek_relative(AtaFile *pAtaFile, long OffsetFromCurrent)
{
  AtaState *pAtaDrive = pAtaFile->pDrive;

  AtaUint32 ll;
  AtaError ret_stat = ATA_ERROR_NONE;
  AtaUint32 OffsetFromStart = (pAtaFile->CurrentByte + OffsetFromCurrent*2)/2;

  if (pAtaFile==NULL) return(ATA_ERROR_INVALID_PARAM); //KR042610

  if(OffsetFromStart > pAtaFile->CurrentByte) return(ATA_ERROR_EOF);
  if((signed long)(OffsetFromCurrent*2 + (signed long)(pAtaFile->CurrentByte))<0L) return(ATA_ERROR_EOF);
/*SUSMIT - ATA_seek absolute seek bug fix*/
  pAtaFile->CurrentByte += OffsetFromCurrent*2;  /*Relative Seek*//*SUSMIT - CurrentByte Updation*/
/*  pAtaFile->CurrentByte = OffsetFromStart*2;*//*Absolute Seek*/
/*SUSMIT - ATA_seek absolute seek bug fix*/
  pAtaFile->NextBrokenCluster = pAtaFile->Cluster = pAtaFile->StartCluster;
  pAtaFile->WordInCluster = 0;
  ll = 0L;
  while(ll<OffsetFromStart)
  {
	ll += (AtaUint32)(pAtaDrive->WordsPerCluster);
	if(ll > OffsetFromStart)
	{
	  ll -= (AtaUint32)(pAtaDrive->WordsPerCluster);
	  pAtaFile->WordInCluster = (AtaUint32)(OffsetFromStart-ll);
	  /* ll = OffsetFromStart; */
	  break;
	}
	if(pAtaFile->Cluster == pAtaFile->NextBrokenCluster)
	{
/*SUSMIT - SEEK TO END & APPEND BUG FIX*/
	if(pAtaFile->StartCluster == pAtaFile->StartDirCluster) {
	  ret_stat = _AtaFindNextBrokenCluster(pAtaFile,1);
	  if(ret_stat) return(ret_stat); }
    else { if(ll!=OffsetFromStart) {
	  ret_stat = _AtaFindNextBrokenCluster(pAtaFile,1);
	  if(ret_stat) return(ret_stat); }
	 else {pAtaFile->WordInCluster = pAtaDrive->WordsPerCluster;} }
/*SUSMIT - SEEK TO END & APPEND BUG FIX*/
	}
	else
	{
	  pAtaFile->Cluster++;
	}
  }
  if(pAtaFile->Size)
  if(pAtaFile->CurrentByte > pAtaFile->Size) {
  	ret_stat = ATA_ERROR_EOF;
  	pAtaFile->WordInCluster -= (pAtaFile->CurrentByte - pAtaFile->Size);
  	pAtaFile->CurrentByte = pAtaFile->Size;}/*New Seek beyond EOF check - Susmit*/
  return(ret_stat);
}


