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
/* $Id: ata_flush.c,2.4+ $													    */
/*										    									*/
/* This is proprietary information, not to be published -- TI INTERNAL DATA   	*/
/* Copyright (C) 2000, Texas Instruments, Inc.  All Rights Reserved.		  	*/
/*																				*/
/* Author: Susmit Pal													  		*/
/* Date: November 6, 2002														*/
/* Modified Date: November 28, 2002												*/
/* Last Modified Date: February 11, 2003										*/
/********************************************************************************/

#include "ata.h"
#include "ata_.h"

AtaError _AtaFlush(AtaState *pAtaDrive)
{
  int ii;
  AtaError kk = ATA_ERROR_NONE;

  /* Flush the current sector from the ATA device */
  if(pAtaDrive->CurrentPhySector != ATA_INVALID_SECTOR)
  {
#ifdef READNWORDSAPI
	if(!_AtaTimeoutCheck(pAtaDrive->AtaDataReadyStatusCheck,pAtaDrive->pAtaMediaState))
	  return(ATA_ERROR_TIMEOUT);
	ii = ATA_WORDS_PER_PHY_SECTOR - (pAtaDrive->CurrentWord+1);
	pAtaDrive->AtaReadNextNWords(pAtaDrive->pAtaMediaState, 0, ii);
#else
	kk = pAtaDrive->AtaBusyStatusCheck(pAtaDrive->pAtaMediaState);
	for(ii=pAtaDrive->CurrentWord+1; ii<ATA_WORDS_PER_PHY_SECTOR; ii++)
	{
	kk =  pAtaDrive->AtaReadNextWord(pAtaDrive->pAtaMediaState, &pAtaDrive->Data);
/*	if(kk) return(kk); *//*SUSMIT_2.2_TEST_DEBUG - This should be break to enable multiple ATaState on same media for simultaneous operations*/
    if(kk) break;
	}
#endif
  }
  pAtaDrive->CurrentPhySector = ATA_INVALID_SECTOR;
  pAtaDrive->CurrentWord = ATA_INVALID_WORD;
  return(ATA_ERROR_NONE);
}


