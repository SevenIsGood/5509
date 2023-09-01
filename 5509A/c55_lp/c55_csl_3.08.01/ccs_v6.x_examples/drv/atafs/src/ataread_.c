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
/* $Id: ataread_.c,2.4+ $														*/
/*																				*/
/* This is proprietary information, not to be published -- TI INTERNAL DATA	   */
/* Copyright (C) 2000, Texas Instruments, Inc.  All Rights Reserved.			  */
/*																				*/
/* Author: Susmit Pal															  */
/* Date: November 6, 2002														*/
/* Modified Date: November 28, 2002												*/
/* Last Modified Date: February 11, 2003										*/
/* Mar 7, 2010 - added ATA_No_Swap control variable                             */
/********************************************************************************/

#include "ata_.h"

extern AtaUint16 ATA_No_Swap;

AtaError _AtaRead(AtaFile *pAtaFile, AtaUint16 *Data, AtaUint16 Words)
{
  AtaUint16 WordCount, OffsetInSector;
  AtaSector PhySector;
  AtaState *pAtaDrive = pAtaFile->pDrive;
  AtaError ret_stat = ATA_ERROR_NONE; AtaUint16 kk;

#ifdef DISABLE_ALWAYS_CALCULATE_SECTOR
  int CalculateSector = 1;
#endif

  for(WordCount=0; WordCount<Words; WordCount++, OffsetInSector++, pAtaFile->WordInCluster++)
  {
	if(pAtaFile->WordInCluster >= pAtaDrive->WordsPerCluster)
	{
	  pAtaFile->WordInCluster = 0;
	  if(pAtaFile->Cluster == pAtaFile->NextBrokenCluster)
	  {
#ifdef DISABLE_ALWAYS_CALCULATE_SECTOR
		CalculateSector = 1;
#endif
		ret_stat = _AtaFindNextBrokenCluster(pAtaFile,1);
		 if(ret_stat)  return(ret_stat);
	  }
	  else
	  {
		pAtaFile->Cluster++;
#ifdef DISABLE_ALWAYS_CALCULATE_SECTOR
		PhySector++;
		OffsetInSector = 0;
#endif
	  }
	}

	/* set up the first sector in the cluster */
#ifdef DISABLE_ALWAYS_CALCULATE_SECTOR
	else if(OffsetInSector >= ATA_WORDS_PER_PHY_SECTOR)
	{
	  PhySector++;
	  OffsetInSector = 0;
	}

	if(CalculateSector)
	{
	  /* calculate physical sector and word offset of current cluster */
	  CalculateSector = 0;
	  PhySector = _AtaCalculatePhySectorAndOffsetFromCluster(pAtaFile,&OffsetInSector);
	}
#else
	PhySector = _AtaCalculatePhySectorAndOffsetFromCluster(pAtaFile,&OffsetInSector);
#endif

	ATA_No_Swap=1; // KR030710
	kk = _AtaReadWord(PhySector, pAtaDrive, OffsetInSector*2);
	ATA_No_Swap=0; // KR030710
	Data[WordCount] = (AtaUint16)kk;
	ret_stat = pAtaDrive->Err_Status;
  }

  return(ret_stat);
}

