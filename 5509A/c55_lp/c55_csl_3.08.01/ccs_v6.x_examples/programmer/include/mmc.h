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


#ifndef MMC_DOT_H
#define MMC_DOT_H

#define   MFG_ID_SANDISK	2

/*****************************************************************************
***				 Commands and responses	 								            ***
*****************************************************************************/

#define CMD0	0
#define GO_IDLE_STATE                     (CMD0 | RSPFMT_NONE)				//Response: R-
#define CMD1	1
#define SEND_OP_COND                      (CMD1 | RSPFMT_R3 )				//Response: R3
#define CMD2	2
#define ALL_SEND_CID                      (CMD2 | RSPFMT_R2)            //Response: R2
#define CMD3	3
#define SET_RELATIVE_ADDR                 (CMD3 | RSPFMT_R1)            //Response: R1
#define CMD4	4
#define SET_DSR                           (CMD4 | RSPFMT_NONE)				//Response: R-
#define CMD7	7
#define SELECT_CARD                       (CMD7 | RSPFMT_R1)            //Response: R1

#define DESELECT_CARD                     (CMD7			)                 //Response: none
#define CMD9	9
#define SEND_CSD                          (CMD9 | RSPFMT_R2)            //Response: R2
#define CMD10   10
#define SEND_CID                          (CMD10 | RSPFMT_R2)           //Response: R2
#define CMD11   11
#define READ_DAT_UNTIL_STOP               (CMD11 | RSPFMT_R1)           //Response: R1
#define CMD12   12
#define STOP_TRANSMISSION                 (CMD12 | RSPFMT_R1)           //Response: R1
#define CMD13   13
#define SEND_STATUS                       (CMD13 | RSPFMT_R1)           //Response: R1
#define CMD15   15
#define GO_INACTIVE_STATE                 (CMD15 | RSPFMT_NONE)         //Response: R-
#define CMD16   16
#define SET_BLOCKLEN                      (CMD16 | RSPFMT_R1)           //Response: R1
#define CMD17   17
#define READ_SINGLE_BLOCK                 (CMD17 | RSPFMT_R1)           //Response: R1
#define CMD18   18
#define READ_MULTIPLE_BLOCK               (CMD18 | RSPFMT_R1)           //Response: R1
#define CMD20   20
#define WRITE_DAT_UNTIL_STOP              (CMD20 | RSPFMT_R1 | BSYEXP)  //Response: R1b
#define CMD24   24
#define WRITE_BLOCK					 	      (CMD24 | RSPFMT_R1 )				//Response: R1
#define CMD25   25
#define WRITE_MULTIPLE_BLOCK              (CMD25 | RSPFMT_R1 |	BSYEXP)  //Response: R1b
#define CMD26   26
#define PROGRAM_CID                       (CMD26 | RSPFMT_R1 |	BSYEXP)  //Response: R1b
#define CMD27   27
#define PROGRAM_CSD                       (CMD27 | RSPFMT_R1 |	BSYEXP)  //Response: R1b
#define CMD28   28
#define SET_WRITE_PROT                    (CMD28 | RSPFMT_R1 |	BSYEXP)  //Response: R1b
#define CMD29   29
#define CLR_WRITE_PROT                    (CMD29 | RSPFMT_R1 |	BSYEXP)  //Response: R1b
#define CMD30   30
#define SEND_WRITE_PROT                   (CMD30 | RSPFMT_R1)           //Response: R1
#define CMD32   32
#define TAG_SECTOR_START                  (CMD32 | RSPFMT_R1)           //Response: R1
#define CMD33   33
#define TAG_SECTOR_END                    (CMD33 | RSPFMT_R1)           //Response: R1
#define CMD34   34
#define UNTAG_SECTOR                      (CMD34 | RSPFMT_R1)           //Response: R1
#define CMD35   35
#define TAG_ERASE_GROUP_START             (CMD35 | RSPFMT_R1)           //Response: R1
#define CMD36   36
#define TAG_ERASE_GROUP_END               (CMD36 | RSPFMT_R1)           //Response: R1
#define CMD37   37
#define UNTAG_ERASE_GROUP                 (CMD37 | RSPFMT_R1)           //Response: R1
#define CMD38   38
#define ERASE                             (CMD38 | RSPFMT_R1 | BSYEXP)  //Response: R1b
#define CMD39   39
#define FAST_IO                           (CMD39 | RSPFMT_R1)           //Response: R4
#define CMD40   40
#define GO_IRQ_STATE                      (CMD40 | RSPFMT_R1)           //Response: R5

//SUSMIT_LOCK_UNLOCK
#define CMD42	42
#define CARD_LOCK_UNLOCK_PASSWD           (CMD42 | RSPFMT_R1)
//SUSMIT_LOCK_UNLOCK

#define CMD59   59
#define CRC_ON_OFF                        (CMD59 | RSPFMT_R1)

/*****************************************************************************
****	The following are the interal MMC card information structure
*****************************************************************************/
//define MMC card status
typedef struct card_status
{
	unsigned short busy;
	CARD_STATE	 state;
	unsigned short erase_clrd;
	unsigned short erase_partial;
	unsigned short error_flags;
}CARD_STATUS;

//MMC interal buffer
typedef struct mmc_internals
{
C55XX_MMC Mmc;
CARD_STATUS	CardStatus;
CARD_CID CardCid;
CARD_CSD CardCsd;
}MMC_internals;

/****************************************************************
**	The following are the MMC low level function
****************************************************************/
AtaError MMC_cardLockUnlockPasswd(void *pAtaMediaState, int lock, char *password);
void Mmc_Enable(MMC_SD_CONTROLLER controller);
int			 MMC_Init(void *pAtaMediaState);
AtaError _MMC_ReadNextSectorWords(void *pAtaMediaState, unsigned short *word);
int			 MMC_SelectCard(void *pAtaMediaState, unsigned short rca);
AtaError			 _MMC_WriteNextWord(void *pAtaMediaState, unsigned short *buffer);
void MMC_Clear_Rsp(void *pAtaMediaState);
int MMC_Set_Blocklen(void *pAtaMediaState, unsigned short block_len );
AtaError MMC_FIFOReset(void *pAtaMediaState);
AtaError MMC_FIFOReceive(void *pAtaMediaState);
AtaError MMC_FIFOTransmit(void *pAtaMediaState);
AtaError MMC_SetDataSize(unsigned short numblks, void *pAtaMediaState);
unsigned short MMC_SendCommand(unsigned short cmd,unsigned short argh,unsigned short argl,void *pAtaMediaState);
AtaError MMC_SetTriger(void *pAtaMediaState);

#endif

