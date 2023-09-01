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
/* $Id: atadir_.c,2.4+ $													    */
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

AtaError _AtaGetDirEntry(AtaFile *pAtaFile)
{
  register char *cc;
  AtaUint32 ll;
  AtaUint16 jj;int i;
  AtaError ret_stat = ATA_ERROR_NONE;

  AtaState *pAtaDrive = pAtaFile->pDrive;

  /* calculate physical sector and word offset of current directory entry */
  ll = _AtaCalculatePhySectorAndOffsetFromDirEntry((AtaDirEntry)pAtaFile->CurrentDirEntry, (AtaUint16 *)&jj, pAtaFile, &ret_stat);
  if(ret_stat) return(ret_stat);
  jj = jj*2;

  cc = (char *)&pAtaFile->Filename[0];
  for(i=0;i<8;i++)
  *cc++ = _AtaReadByte(ll, pAtaDrive, jj++);
  *cc++ = '\0';
  cc = (char *)&pAtaFile->Ext[0];
  for(i=0;i<3;i++)
  *cc++ = _AtaReadByte(ll, pAtaDrive, jj++);
  *cc++ = '\0';
  pAtaFile->Attributes   = (unsigned char)_AtaReadByte(ll, pAtaDrive, jj);
#ifndef DISABLE_FAT32
  jj+=9;
  pAtaFile->StartCluster = ((AtaCluster)_AtaReadWord(ll, pAtaDrive, jj)) << 16;
  jj+=2;
#else
  jj+=11;
#endif
  pAtaFile->Time = _AtaReadWord(ll, pAtaDrive, jj);
  jj+=2;
  pAtaFile->Date = _AtaReadWord(ll, pAtaDrive, jj);
  jj+=2;
#ifndef DISABLE_FAT32
  pAtaFile->StartCluster|= (AtaCluster)_AtaReadWord(ll, pAtaDrive, jj);
#else
  pAtaFile->StartCluster = _AtaReadWord(ll, pAtaDrive, jj);
#endif
  jj+=2;
  pAtaFile->Size = _AtaReadDoubleWord(ll, pAtaDrive, jj);

  pAtaFile->PrevBrokenCluster = pAtaFile->NextBrokenCluster = pAtaFile->Cluster = pAtaFile->StartCluster;
  pAtaFile->WordInCluster = 0;

  return(pAtaDrive->Err_Status);
}


