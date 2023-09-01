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
/* $Id: ataread.c,2.4+ $													    */
/*										    									*/
/* This is proprietary information, not to be published -- TI INTERNAL DATA   	*/
/* Copyright (C) 2000, Texas Instruments, Inc.  All Rights Reserved.		  	*/
/*																				*/
/* Author: Susmit Pal													  		*/
/* Date: November 6, 2002														*/
/* Modified Date: November 28, 2002												*/
/* Last Modified Date: February 11, 2003										*/
/* Mar 7, 2010 - added ATA_No_Swap control variable                             */
/* April 26, 2010 - added check for NULL pointer KR042610                       */
/********************************************************************************/

#include "ata_.h"

extern AtaUint16 ATA_No_Swap;

AtaError ATA_read(AtaFile *pAtaFile, AtaUint16 *Data, AtaUint16 Words)
{
  //int jj;
  AtaError ret_stat = ATA_ERROR_NONE;

  if (pAtaFile==NULL) return(ATA_ERROR_INVALID_PARAM); //KR042610

  pAtaFile->CurrentByte += Words*2;  /*SUSMIT - CurrentByte Updation*/
  ret_stat = _AtaRead(pAtaFile, Data, Words);

////////////////////
// no swap KR030710
////////////////////
#if 0
  for(jj=0; jj < Words; jj++)
  {
	Data[jj] = ((unsigned short)(Data[jj] >> 8) & 0x00FF)|((unsigned short)(Data[jj]<< 8) & 0xFF00);
  }
#endif


  if(pAtaFile->CurrentByte > pAtaFile->Size) ret_stat = ATA_ERROR_EOF;
  return(ret_stat);
}

AtaError ATA_readLittleEndian(AtaFile *pAtaFile, AtaUint16 *Data, AtaUint16 Words)
{
  AtaError ret_stat = ATA_ERROR_NONE;

  if (pAtaFile==NULL) return(ATA_ERROR_INVALID_PARAM); //KR042610

  pAtaFile->CurrentByte += Words*2;  /*SUSMIT - CurrentByte Updation*/

  ret_stat = _AtaRead(pAtaFile, Data, Words);

  if(pAtaFile->CurrentByte > pAtaFile->Size) ret_stat = ATA_ERROR_EOF;
  return(ret_stat);
}

AtaError ATA_sleep(AtaState *pAtaDrive)
{

  if (pAtaDrive==NULL) return(ATA_ERROR_INVALID_PARAM); //KR042610

  /* Flush the current sector from the ATA device */
  return(_AtaFlush(pAtaDrive));
}
