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


#ifndef ATA_SD_H
#define ATA_SD_H

#define  SERIAL_ID_LEN			16
#define ATA_WORDS_PER_PHY_SECTOR  0x100
/********************************************************************
***					SD card state structure				               ***
********************************************************************/
typedef struct AtaSDState
{
int sdNumber;                       //sd slot
unsigned int data_lines;            //1 data or 4 data lines
int c55x_sd_ctl_addr;               //address
int card_number;                    //card number
unsigned int FDIV;
unsigned int CDIV;
unsigned short fifo_threshold;      //fifo_level
int  *buffer;                       //internal buffer
void *ptr;                          //internal buffer
}AtaSDState;

/********************************************************************
****		SD ATA interface function prototypes			      	  ****
********************************************************************/
AtaError SD_systemInit(AtaState *pAtaDrive);

AtaError AtaSDInitState(void *pAtaMediaState);

AtaError AtaSDReset(void *pAtaMediaState);

AtaError AtaSDBusyStatusCheck(void *pAtaMediaState);

AtaUint16 AtaSDGetSerialID(void *pAtaMediaState, AtaSerialID *pID);

AtaError AtaSDIssueReadCommand( AtaSector PhySector,  void *pAtaMediaState,  AtaUint16 SectorCount);

int AtaSDCommandReadyStatusCheck(void *pAtaMediaState);

AtaError AtaSDDataReadyStatusCheck(void *pAtaMediaState);

AtaError AtaSDReadNextWord(void *pAtaMediaState, AtaUint16 *pWord);

AtaError AtaSDReadNextNWords(void *pAtaMediaState, AtaUint16 *Data, AtaUint16 Words);

AtaError AtaSDWriteSector( AtaSector PhySector,  void *pAtaMediaState,  AtaUint16 *pWord,  int ByteSwap);

AtaError AtaSDWriteSectorFlush(void *pAtaMediaState);

// the following function is for init only
AtaUint16 AtaSDReadWord(AtaSector PhySector, AtaState *pAtaDrive, AtaUint16 ByteOffset);
int AtaSDTimeoutCheck(AtaError (fpStatusCheck)(void *ptr), void *ptr);
AtaUint32 AtaSDReadDoubleWord(AtaSector PhySector, AtaState *pAtaDrive, AtaUint16 ByteOffset);
char AtaSDReadByte(AtaSector PhySector, AtaState *pAtaDrive, AtaUint16 ByteOffset);
AtaError AtaSDFlush(AtaState *pAtaDrive);

#endif

