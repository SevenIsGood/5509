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
/* $Id: atatell.c,2.4+ $													    */
/*										    									*/
/* This is proprietary information, not to be published -- TI INTERNAL DATA   	*/
/* Copyright (C) 2000, Texas Instruments, Inc.  All Rights Reserved.		  	*/
/*																				*/
/* Author: Susmit Pal													  		*/
/* Date: November 6, 2002														*/
/* Modified Date: November 28, 2002												*/
/* Last Modified Date: February 11, 2003										*/
/* April 27, 2010 - addded check for NULL pointer KR042710                      */
/********************************************************************************/

#include "ata.h"
#include "ata_.h"

/*unsigned long AtaTell(AtaFile *pAtaFile)*/
AtaError ATA_tell(AtaFile *pAtaFile, AtaFileSize *pOffsetInFile)
{
  AtaState *Ata = pAtaFile->pDrive;
  AtaUint32 savedNextBrokenCluster, targetCluster;
  AtaUint16 targetWordInCluster;
  AtaError ret_stat = ATA_ERROR_NONE;

  if (pAtaFile==NULL) return(ATA_ERROR_INVALID_PARAM); //KR042710

  /* Save off current NextBrokenCluster, WordInCluster, and Cluster values */
  savedNextBrokenCluster = pAtaFile->NextBrokenCluster;
  targetCluster = pAtaFile->Cluster;
  targetWordInCluster = pAtaFile->WordInCluster;

  /* Set pointers back to the beginning of the file */
  pAtaFile->NextBrokenCluster = pAtaFile->Cluster = pAtaFile->StartCluster;
  pAtaFile->WordInCluster = 0;
  *pOffsetInFile = 0;

  /* Start searching for the target cluster, adding up the words */
  while(pAtaFile->Cluster < targetCluster)
  {
	*pOffsetInFile += (AtaUint32)(Ata->WordsPerCluster);
	if(pAtaFile->Cluster == pAtaFile->NextBrokenCluster)
	{
	  ret_stat = _AtaFindNextBrokenCluster(pAtaFile, 1);
	  if(ret_stat)
	  {
/*		return((AtaUint32)-1L);*/
/*		return(ATA_ERROR_BAD_MEDIA);*/  /*Susmit - Check this*/
		return(ret_stat);
	  }
	}
	else
	{
	  pAtaFile->Cluster++;
	}
  }

  /* Add the target word offset within the cluster */
  *pOffsetInFile += (AtaUint32)targetWordInCluster;

  /* Restore current NextBrokenCluster, WordInCluster, and Cluster values */
  pAtaFile->NextBrokenCluster = savedNextBrokenCluster;
  pAtaFile->Cluster = targetCluster;
  pAtaFile->WordInCluster = targetWordInCluster;

  return(ATA_ERROR_NONE);
}

