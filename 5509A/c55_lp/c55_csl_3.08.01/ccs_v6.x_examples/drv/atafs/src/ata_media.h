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


/******************************************************************************/
/* $Id: ata_media.h,v $                                                       */
/*                                                                            */
/* This is proprietary information, not to be published -- TI INTERNAL DATA   */
/* Copyright (C) 2000, Texas Instruments, Inc.  All Rights Reserved.          */
/*                                                                            */
/* Author: Jason Kridner                                                      */
/* Date: December 16, 2000                                                    */
/******************************************************************************/

#ifndef ATA_MEDIA_H
#define ATA_MEDIA_H 1

#include "ata.h"
#define ATA_WORDS_PER_PHY_SECTOR 0x100
/* -----------------12/16/2000 10:11AM---------------
All of these functions can be non-nestable (non-reentrant).  It is the
 responsibility of the system software to make sure these functions are not
 nested.
 --------------------------------------------------*/

/* -----------------12/16/2000 10:12AM---------------
This function should put the media back into its idle state with no
 sectors open and ready to accept commands.  The high-level API will
 attempt not to call this function until any open sector is read to
 the end.  If an error condition exists, this function will be called to
 clear the error condition.
 --------------------------------------------------*/
AtaError AtaMediaReset(void *AtaMediaState);

/* -----------------12/16/2000 11:21AM---------------
This function should return 0 to the high-level API if the media is
 ready to accept commands.  This allows the high-level API to yeild
 to other processes if the media is not ready to accept commands.
 --------------------------------------------------*/
int AtaMediaBusyStatusCheck(void *AtaMediaState);

/* -----------------12/16/2000 10:12AM---------------
This function is used to fetch the ATA GetIdentify data structure.  This
 data structure is defined in the ATA specification.  It contains the
 serial number and physical description of the media.  Today, only the
 serial number is fetched, but future high-level API functions will use
 this data for formatting the media.  On NAND devices, the very first
 sector on the media is reserved for this data and all subsequent sectors
 are shifted accordingly.  This data is always 512 16-bit words. All high-
 level APIs will call AtaMediaIssueGetIdentifyCommand once then call
 AtaMediaReadNextWord 512 times to fetch this data.
 --------------------------------------------------*/
/* -----------------3/27/2001 10:15PM----------------
Stephen changed this because each media returns a different structure.
There still needs to be a mechanism somewhere to get at more information
in this structure, but it has not been solved.  -- JDK
 --------------------------------------------------*/
AtaUint16 AtaMediaGetSerialID(void *pAtaMediaState, AtaSerialID *pID);

/* -----------------12/16/2000 10:12AM---------------
This function is used to fetch all normal data from the media.  A sector
 is always assumed to be 256 16-bit words by the high-level API.
 --------------------------------------------------*/
AtaError AtaMediaIssueReadCommand
(
  AtaSector PhySector,
  void *AtaMediaState,
  AtaUint16 SectorCount
);

/* -----------------12/16/2000 11:21AM---------------
This function should return 0 to the high-level API if the media is
 NOT ready to provide data.  This allows the high-level API to yeild
 to other processes if the media is not ready to provide data.

AtaMediaDataReadyStatusCheck is only guaranteed to be called before the
 first call to AtaMediaReadNextWord following a AtaMediaIssue*Command,
 but may be called at any point.  It should always report if there is
 data available to be read by AtaMediaReadNextWord.
 --------------------------------------------------*/
int AtaMediaDataReadyStatusCheck(void *AtaMediaState);

/* -----------------12/16/2000 11:20AM---------------
This function should always return data.  It is the high-level API's
 reponsibility to verify that data will be ready by using the
 AtaMediaDataReadyStatusCheck function.  Once that function returns
 a non-zero value, any subsequent calls to AtaMediaReadNextWord should
 provide valid data.  This function will be called 256 times when it
 follows a AtaMediaIssueReadCommand function call.  It will be called
 512 times when it follows a AtaMediaIssueGetIdentifyCommand function
 call.  Any caching should occur below this function.  Because most
 media only have a delay in fetching the first word, and not subsequent
 words, no caching is generally required.  In the case the DMA is used
 to fetch data, an intermediate buffer below this function will be
 required.

AtaMediaDataReadyStatusCheck is only guaranteed to be called before the
 first call to AtaMediaReadNextWord following a AtaMediaIssue*Command,
 but may be called at any point.  It should always report if there is
 data available to be read by AtaMediaReadNextWord.
 --------------------------------------------------*/
AtaError AtaMediaReadNextWord(void *pAtaMediaState, AtaUint16 *pWord, AtaUint16 Count);

/* -----------------12/16/2000 7:39PM----------------
This function writes one (1) sector of data to the media.  It is
 possible to cache data being written to the media because the high-
 level API guarantees that AtaMediaWriteSectorFlush will be called
 before any data is read back from the media.  AtaMediaWriteSectorFlush
 is also called when the media is left idle for a long period of time
 and before power-down.  This function should not be called while read
 data is pending for AtaMediaReadNextWord.
 --------------------------------------------------*/
AtaError AtaMediaWriteSector
(
  AtaSector PhySector,
  void *AtaMediaState,
  AtaUint16 *pWord,
  int ByteSwap
);

/* -----------------12/16/2000 7:42PM----------------
This function commits any data which is in a write cache to be written
 to the media.  In the case that write data is not cached, this should
 simply be a stub function.
 --------------------------------------------------*/
AtaError AtaMediaWriteSectorFlush(void *AtaMediaState);

/* -----------------12/16/2000 7:41PM----------------
Put any state information required for the media here.
 --------------------------------------------------*/
typedef struct AtaMediaStateStruct
{
  /* -----------------12/16/2000 10:12AM---------------
  For example,
  AtaUint16 PortNo;
  AtaUint16 *pSectorBuffer;
   --------------------------------------------------*/
	void *ptr;
} AtaMediaState;

/* -----------------12/16/2000 10:13AM---------------
If there is a requirement for a special function to initialize the
AtaMediaState structure, place that function below.  This function
will need to be called independantly from the ATA high-level API
functions
 --------------------------------------------------*/
/* -----------------12/16/2000 10:13AM---------------
void AtaInitAtaMediaState
(
  AtaMediaState *AtaMediaState1,
  AtaUint16 *pSectorBuffer
);
 --------------------------------------------------*/

#endif
