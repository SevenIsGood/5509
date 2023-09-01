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
/* $Id: atalong.c,2.4+ $													    */
/*										    									*/
/* This is proprietary information, not to be published -- TI INTERNAL DATA   	*/
/* Copyright (C) 2000, Texas Instruments, Inc.  All Rights Reserved.		  	*/
/*																				*/
/* Author: Susmit Pal													  		*/
/* Date: November 6, 2002														*/
/* Modified Date: November 28, 2002												*/
/* Last Modified Date: February 11, 2003										*/
/* April 26, 2010 added error check for NULL pointers KR042610                  */
/********************************************************************************/

#include "ata.h"
#include "ata_.h"

AtaError ATA_getLongName(AtaFile *pAtaFile, char *LongName, AtaUint16 StartOffset, AtaUint16 Length)
{
  register char *cc;
    AtaError ret_stat = ATA_ERROR_NONE;
    AtaUint32 ll;AtaUint16 kk;
    short jj = 0;long lll;
    AtaState *pAtaDrive = pAtaFile->pDrive;
    AtaUint16 *_AtaWriteBuffer = pAtaDrive->_AtaWriteBuffer;
    AtaUint16 exit_flag = 0;
    AtaUint16 NumLFNEntry = 0;
    int i,j;AtaUint16 icnt = 0;
    AtaUint16 file_cksum = _Ata_lfn_checksum(pAtaFile);

    if ((pAtaFile==NULL)||(pAtaDrive==NULL)) return(ATA_ERROR_INVALID_PARAM); //KR042610

    file_cksum = file_cksum<<8;
    file_cksum &= 0xff00;

    ll = _AtaCalculatePhySectorAndOffsetFromDirEntry((AtaUint32)pAtaFile->CurrentDirEntry, (AtaUint16 *)&jj, pAtaFile, &ret_stat);
    if(ret_stat) return(ret_stat);
    ret_stat = _AtaReadSector(ll, pAtaDrive, &_AtaWriteBuffer[0], 0);
    if(ret_stat) return(ret_stat);
    kk=jj;
    jj-=16;
/*SUSMIT - Fix for UNICODE*/
/*    while(((_AtaWriteBuffer[jj+5] == 0x0F00)||(_AtaWriteBuffer[jj+5] == 0x0FFF))&&(jj>=0)&&(!exit_flag)&&(_AtaWriteBuffer[jj+6] == file_cksum)) {*/
    while((((_AtaWriteBuffer[jj+5]&0xff00) == 0x0F00)||(_AtaWriteBuffer[jj+5] == 0x0FFF))&&(jj>=0)&&(!exit_flag)&&(_AtaWriteBuffer[jj+6] == file_cksum)) {
/*SUSMIT - Fix for UNICODE*/
      if(_AtaWriteBuffer[jj]&0x0040) exit_flag = 1;
      NumLFNEntry++;
      jj-=16; }
/*SUSMIT - Code to check for corner cases*/
	if((!exit_flag)&&((NumLFNEntry)||(kk==0))) {

     ret_stat = AtaGetLFN(ll, pAtaDrive, LongName, &icnt, kk, jj, Length, NumLFNEntry);
      		NumLFNEntry = 0;
    if(ret_stat) return(ret_stat);

/*Susmit - Fix for .*/
    lll = (AtaUint32)pAtaFile->CurrentDirEntry-16-(AtaUint16)(kk%16);
    if(lll<0) lll = (AtaUint32)pAtaFile->CurrentDirEntry;
    ll = _AtaCalculatePhySectorAndOffsetFromDirEntry((AtaUint32)(lll), (AtaUint16 *)&jj, pAtaFile, &ret_stat);
/*Susmit - Fix for .*/
/*    ll = _AtaCalculatePhySectorAndOffsetFromDirEntry(((AtaUint32)pAtaFile->CurrentDirEntry-16-(AtaUint16)(kk%16)), (AtaUint16 *)&jj, pAtaFile, &ret_stat);*/
    if(ret_stat) return(ret_stat);
    ret_stat = _AtaReadSector(ll, pAtaDrive, &_AtaWriteBuffer[0], 0);
    if(ret_stat) return(ret_stat);
    jj = 256;
    kk=jj;
    jj-=16;
/*SUSMIT - Fix for UNICODE*/
/*    while(((_AtaWriteBuffer[jj+5] == 0x0F00)||(_AtaWriteBuffer[jj+5] == 0x0FFF))&&(jj>=0)&&(!exit_flag)&&(_AtaWriteBuffer[jj+6] == file_cksum)) {*/
    while((((_AtaWriteBuffer[jj+5]&0xff00) == 0x0F00)||(_AtaWriteBuffer[jj+5] == 0x0FFF))&&(jj>=0)&&(!exit_flag)&&(_AtaWriteBuffer[jj+6] == file_cksum)) {
/*SUSMIT - Fix for UNICODE*/
      if(_AtaWriteBuffer[jj]&0x0040) exit_flag = 1;
      NumLFNEntry++;
      jj-=16; }
	}
/*SUSMIT - Code to check for corner cases*/
    if(NumLFNEntry) {
     ret_stat = AtaGetLFN(ll, pAtaDrive, LongName, &icnt, kk, jj, Length, NumLFNEntry);
      		NumLFNEntry = 0;
     if(ret_stat) return(ret_stat);
 /*End of filename collection, now need to put '\0' at proper place - SUSMIT*/
/*SUSMIT - Fix for UNICODE of more than 208 characters*/
	if(!exit_flag) {
    ll = _AtaCalculatePhySectorAndOffsetFromDirEntry((AtaUint32)(lll-16), (AtaUint16 *)&jj, pAtaFile, &ret_stat);
    if(ret_stat) return(ret_stat);
    ret_stat = _AtaReadSector(ll, pAtaDrive, &_AtaWriteBuffer[0], 0);
    if(ret_stat) return(ret_stat);
    jj = 256;
    kk=jj;
    jj-=16;
/*SUSMIT - Fix for UNICODE*/
/*    while(((_AtaWriteBuffer[jj+5] == 0x0F00)||(_AtaWriteBuffer[jj+5] == 0x0FFF))&&(jj>=0)&&(!exit_flag)&&(_AtaWriteBuffer[jj+6] == file_cksum)) {*/
    while((((_AtaWriteBuffer[jj+5]&0xff00) == 0x0F00)||(_AtaWriteBuffer[jj+5] == 0x0FFF))&&(jj>=0)&&(!exit_flag)&&(_AtaWriteBuffer[jj+6] == file_cksum)) {
/*SUSMIT - Fix for UNICODE*/
      if(_AtaWriteBuffer[jj]&0x0040) exit_flag = 1;
      NumLFNEntry++;
      jj-=16; }
	}
/*SUSMIT - Code to check for corner cases*/
    if(NumLFNEntry) {
     ret_stat = AtaGetLFN(ll, pAtaDrive, LongName, &icnt, kk, jj, Length, NumLFNEntry);
      		NumLFNEntry = 0;
     if(ret_stat) return(ret_stat);
	}
/*SUSMIT - Fix for UNICODE of more than 208 characters*/
      if(icnt < Length) LongName[icnt] = '\0';
/*SUSMIT - No need to worry about zeroes if we get the full LFN, they will come automatically from LFN entries*/
    }
    else {
       cc = pAtaFile->Filename;
       i = 0;j = 0;
       while((*cc!=' ')&&(i<Length)&&(*cc!='\0')) {LongName[i++] = pAtaFile->Filename[j++];cc++;}
       cc = pAtaFile->Ext; j = 0;
       if((i<Length)&&(*cc!=' ')&&(*cc!='\0')) LongName[i++] = '.';
       while((*cc!=' ')&&(i<Length)&&(*cc!='\0')) {LongName[i++] = pAtaFile->Ext[j++];cc++;}
      if(i<Length) LongName[i] = '\0'; }
  return(ret_stat);
}

AtaError AtaGetLFN(AtaSector ll, AtaState *pAtaDrive, char *LongName, AtaUint16 *icnt, AtaUint16 kk, short jj, AtaUint16 Length, AtaUint16 NumLFNEntry)
{
      AtaUint16 ii,i;char holder;/*SUSMIT - Fix for UNICODE*/
      for(ii=0;ii<NumLFNEntry;ii++) {
        kk-=16;
      		jj=kk*2;
	  		jj+=1;
      		for(i=0;(i<5)&&(*icnt<Length);i++)
      			{
/*SUSMIT - Fix for UNICODE*/
				holder = _AtaReadByte(ll, pAtaDrive, jj);
				holder |= _AtaReadByte(ll, pAtaDrive, jj+1)<<8;
        		LongName[(*icnt)++] = holder;
        		jj+=2;
      			}
	  		jj+=3;
      		for(i=0;(i<6)&&(*icnt<Length);i++)
      			{
/*SUSMIT - Fix for UNICODE*/
        		LongName[(*icnt)++] = _AtaReadWord(ll, pAtaDrive, jj);
        		jj+=2;
      			}
	  		jj+=2;
	  		if (*icnt<Length)
/*SUSMIT - Fix for UNICODE*/
        		LongName[(*icnt)++] = _AtaReadWord(ll, pAtaDrive, jj);
	  		jj+=2;
	  		if (*icnt<Length)
/*SUSMIT - Fix for UNICODE*/
        		LongName[(*icnt)++] = _AtaReadWord(ll, pAtaDrive, jj);
      }
	return(pAtaDrive->Err_Status);
}
