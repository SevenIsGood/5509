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
/* $Id: atalong_create.c,2.4+ $												    */
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
/*SUSMIT - LFN APIs for creation & renaming.*/

#include "ata.h"
#include "ata_.h"
#include <string.h>

AtaError ATA_renameLong(AtaFile *pAtaFile, char *longnamenew)
{
    AtaError ret_stat = ATA_ERROR_NONE;
    AtaFile temp_file, new_file;
    AtaFile *pfile = &new_file;
    AtaUint32 ll;
    unsigned short jj = 0;
    AtaUint16 dirflag;
/*SUSMIT - New LFN Fix*/
	AtaUint16 deleteflag = 0;
/*SUSMIT - New LFN Fix*/
    AtaState *pAtaDrive = pAtaFile->pDrive;
    AtaUint16 *_AtaWriteBuffer = pAtaDrive->_AtaWriteBuffer;
    AtaUint32 direntrycounter, dircounterstart = 0;
    AtaUint16 entriespersector = pAtaDrive->WordsPerLogSector/16;
    AtaUint16 file_cksum = _Ata_lfn_checksum(pAtaFile);

    if (pAtaFile==NULL) return(ATA_ERROR_INVALID_PARAM); //KR042610

    file_cksum = file_cksum<<8;
    file_cksum &= 0xff00;
    dirflag = ATA_isDir(pAtaFile);

    memmove(&temp_file, pAtaFile, sizeof(AtaFile));

  	if(pAtaFile->CurrentDirEntry>20) {dircounterstart = (AtaUint32)(pAtaFile->CurrentDirEntry-20);
/*SUSMIT - This magic number 20 comes from the limitation that an LFN can contain max 255
characters & all the entries are contiguous*/
  	dircounterstart-=(AtaUint32)(dircounterstart%entriespersector);}
  for(direntrycounter = dircounterstart; direntrycounter <= pAtaFile->CurrentDirEntry; direntrycounter += entriespersector) {
    ll = _AtaCalculatePhySectorAndOffsetFromDirEntry((AtaUint32)direntrycounter, (AtaUint16 *)&jj, pAtaFile, &ret_stat);
    if(ret_stat) return(ret_stat);
    ret_stat = _AtaReadSector(ll, pAtaDrive, &_AtaWriteBuffer[0], 0);
    if(ret_stat) return(ret_stat);
    while(jj<256) {
    if((((_AtaWriteBuffer[jj+5]&0xff00) == 0x0F00)||(_AtaWriteBuffer[jj+5] == 0x0FFF))&&(_AtaWriteBuffer[jj+6] == file_cksum))
/*SUSMIT - New LFN Fix*/
     { if(((_AtaWriteBuffer[jj]&0x0040)==0x0040)&&((_AtaWriteBuffer[jj]&0x000f)==(pAtaFile->CurrentDirEntry-(direntrycounter+jj/entriespersector)))) deleteflag = 1;
/*SUSMIT - New LFN deletion modification to save characters*/
/*     	if(deleteflag){ _AtaWriteBuffer[jj] = '\xE5'; }*/
     	if(deleteflag){ _AtaWriteBuffer[jj] = (_AtaWriteBuffer[jj]&0xff00)|0x00E5; }
/*SUSMIT - New LFN deletion modification to save characters*/
      }
    jj+=16;
/*SUSMIT - New LFN Fix*/
    }
    ret_stat = _AtaFlush(pAtaDrive);
    if(ret_stat) return(ret_stat);
    ret_stat = pAtaDrive->AtaWriteSector(ll, pAtaDrive->pAtaMediaState, &_AtaWriteBuffer[0], 0);
    if(ret_stat) return(ret_stat);
    ret_stat = pAtaDrive->AtaWriteSectorFlush(pAtaDrive->pAtaMediaState);
    if(ret_stat) return(ret_stat);
  }

    pAtaFile->PrevBrokenCluster = pAtaFile->NextBrokenCluster = pAtaFile->Cluster = pAtaFile->StartCluster;
    pAtaFile->Filename[0] = '\xE5';
    if(_AtaCreate(pAtaFile,2,0))
    {
        return(1);
    }
    ret_stat = ATA_setLongFileName(pAtaFile, longnamenew);
    if(ret_stat) return(ret_stat);
    memmove(pfile->Filename, pAtaFile->Filename, 9);
    memmove(pfile->Ext, pAtaFile->Ext, 4);
    memmove(pAtaFile, &temp_file, sizeof(AtaFile));
    memmove(pAtaFile->Filename, pfile->Filename, 9);
    memmove(pAtaFile->Ext, pfile->Ext, 4);
    if(dirflag) pAtaFile->Attributes|= ATA_ATTR_DIRECTORY;
    ret_stat = _AtaCreateLFNEntry(pAtaFile, longnamenew, 1, dirflag, 1);
    if(ret_stat) return(ret_stat);
    if(!dirflag) {ret_stat = ATA_close(pAtaFile);}
    return(ret_stat);
}

