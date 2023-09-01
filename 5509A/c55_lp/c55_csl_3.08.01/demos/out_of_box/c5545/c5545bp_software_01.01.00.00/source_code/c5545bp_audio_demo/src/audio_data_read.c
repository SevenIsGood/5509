/*
* audio_data_read.c
*
*
* Copyright (C) 2016 Texas Instruments Incorporated - http://www.ti.com/
*
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csl_intc.h"
#include "audio_data_read.h"
#include "audio_data_collection.h"
#include "c55AudioDemocfg.h"
#include "sys_state.h"
#include "wave_parser.h"
#include "audio_utils.h"

AtaState    gstrAtaDrive;
AtaState    *gpstrAtaDrive = &gstrAtaDrive;

AtaFile     strAtaFile;
AtaFile     *pAtaFile = &strAtaFile;

#pragma DATA_ALIGN(AtaWrBuf ,4);
AtaUint16   AtaWrBuf[256];
AtaMMCState  gstrAtaMMCState;
AtaMMCState  *gpstrAtaMMCState = &gstrAtaMMCState;

#pragma DATA_SECTION(gWaveFileData, ".waveData");
#ifdef ENABLE_FIFO_BUFFERING
AtaUint16   gWaveFileData[WAVE_DATA_BUF_SIZE];
#else
AtaUint16   gWaveFileData[WAVE_DATA_BLOCK_COUNT][WAVE_DATA_BLOCK_SIZE];
#endif
AtaUint16   gWaveHdrData[30];
Uint32      gWaveDataRdBlkOffset = 0;
Uint32      gWaveDataWrBlkOffset = 0;
Int8        gFreeDataBlocks = WAVE_DATA_BLOCK_COUNT;

Uint32 bytesRead = 0;

/* CSL MMCSD Data structures */
CSL_MMCControllerObj 	pMmcsdContObj;
CSL_MmcsdHandle 		mmcsdHandle;
CSL_MMCCardObj			mmcCardObj;
CSL_MMCCardIdObj 		sdCardIdObj;
CSL_MMCCardCsdObj 		sdCardCsdObj;

CSL_DMA_Handle        dmaWrHandle;
CSL_DMA_Handle        dmaRdHandle;
CSL_DMA_ChannelObj    dmaWrChanObj;
CSL_DMA_ChannelObj    dmaRdChanObj;

Uint8 gWaveFileFound;
Uint8 gBleFWFound = 0;
Uint8 gFileName[10];

Uint32 gBleFWAppSize = 0;
Uint32 gBleFWStackSize = 0;

waveHeader gWaveHdr;

AtaError _AtaRead_b(AtaFile *pAtaFile, AtaUint16 *Data, AtaUint16 bytes);

/* Swap bytes in a given buffer */
void swapBytes(Uint16 *input, Uint32 length)
{
    Uint32 index;
    Uint16 byte;

    for (index = 0; index < length; index++)
    {
        byte = (input[index] & 0xFF) << 8;
        input[index] = (input[index] & 0xFF00) >> 8;

        input[index] |= byte;
    }
}

/* converts a word buffer to bytes */
void byteshift(Uint16 *input, Uint32 length)
{
    Uint32 index;

    for (index = 0; index < length; index++)
    {
        input[index] = input[index] >> 8;
    }
}

/* Function to read a file.
   This function fills wave data buffers based on the
   free space available
 */
AtaError read_file(void)
{
	Uint16 words2Read;
	AtaError err;

#ifdef ENABLE_FIFO_BUFFERING
	/* Calculate data size to read */
	if((gWaveDataRdBlkOffset == 0) && (gWaveDataWrBlkOffset == 0))
	{
		words2Read = WAVE_DATA_BUF_SIZE/2;
	}
	else if(gWaveDataWrBlkOffset > gWaveDataRdBlkOffset)
	{
		words2Read = ((WAVE_DATA_BUF_SIZE - gWaveDataWrBlkOffset) > WAVE_DATA_READBLK_SIZE) ? WAVE_DATA_READBLK_SIZE : (WAVE_DATA_BUF_SIZE - gWaveDataWrBlkOffset);
	}
	else  //(gWaveDataWrBlkOffset < gWaveDataRdBlkOffset)
	{
		words2Read = gWaveDataRdBlkOffset - gWaveDataWrBlkOffset;

		if(words2Read < WAVE_DATA_READBLK_MIN)
		{
			return(ATA_ERROR_NONE);
		}
	}

	err = ATA_read(pAtaFile, &gWaveFileData[gWaveDataWrBlkOffset], words2Read);
	if (err != ATA_ERROR_NONE)
	{
		return(err);
	}

	swapBytes(&gWaveFileData[gWaveDataWrBlkOffset], words2Read);
	gWaveDataWrBlkOffset += words2Read;
	bytesRead += 2*words2Read;

	if(gWaveDataWrBlkOffset >= WAVE_DATA_BUF_SIZE)
	{
		gWaveDataWrBlkOffset = 0;
	}

#else
	if(gFreeDataBlocks == 0)
	{
		return 0;
	}

	words2Read = WAVE_DATA_BLOCK_SIZE;

	err = ATA_read(pAtaFile, &gWaveFileData[gWaveDataWrBlkOffset][0], words2Read);
	if (err != ATA_ERROR_NONE)
	{
		return(err);
	}

	gFreeDataBlocks--;
	swapBytes(&gWaveFileData[gWaveDataWrBlkOffset][0], words2Read);
	bytesRead += 2*words2Read;

	gWaveDataWrBlkOffset++;
	if(gWaveDataWrBlkOffset >= WAVE_DATA_BLOCK_COUNT)
	{
		gWaveDataWrBlkOffset = 0;
	}

#endif

	return(err);
}

/* Function to seek in a file */
AtaError seek_file(AtaFileSize offset)
{
	return (ATA_seek(pAtaFile, offset));
}

/* Function to read wave header from a wave audio file.
   Function assumes file pointer is at start of the file */
static AtaError read_wave_header(void)
{
	Uint16 words2Read = 22;
	AtaError err;

	err = ATA_read(pAtaFile, gWaveHdrData, words2Read);
	if (err != ATA_ERROR_NONE)
	{
		return(err);
	}

	swapBytes(gWaveHdrData, words2Read);

	return(err);
}

/* Function to open first wave file on the SD card */
AtaError open_first_file(void)
{
	AtaError error;

	gWaveFileFound = 0;

	error = ATA_findFirst(pAtaFile);
	if(error != ATA_ERROR_NONE)
	{
		return (error);
	}

	do
	{
		if((!strcmp(pAtaFile->Ext, "WAV")) ||
		   (!strcmp(pAtaFile->Ext, "wav")))
		{
			read_wave_header();
			extractWaveInfo(&gWaveHdr, gWaveHdrData);
			if(!checkWaveHeader(&gWaveHdr))
			{
				strcpy((char*)gFileName, pAtaFile->Filename);
				gWaveFileFound = 1;
				error = ATA_ERROR_NONE;
			}
			else
			{
				error = ATA_findNext(pAtaFile);
				if(error != ATA_ERROR_NONE)
				{
					return (error);
				}
			}
		}
		else
		{
			error = ATA_findNext(pAtaFile);
			if(error != ATA_ERROR_NONE)
			{
				return (error);
			}
		}
	} while(!gWaveFileFound);

	if(error == ATA_ERROR_NONE)
	{
		gFreeDataBlocks = 4;
		gWaveDataRdBlkOffset = 0;
		gWaveDataWrBlkOffset = 0;
		/* Set SRC input sampling rate */
		src_set_samplerate(gWaveHdr.sampleRate);
		seek_file(22);
		read_file();
		read_file();
	}

	return (error);
}

/* Function to open first wave file on the SD card */
static AtaError find_ble_firmware(void)
{
	AtaError error;

	gBleFWFound = 0;

	error = ATA_findFirst(pAtaFile);
	if(error != ATA_ERROR_NONE)
	{
		return (error);
	}

	do
	{
		if((!strcmp(pAtaFile->Ext, "hex")) ||
		   (!strcmp(pAtaFile->Ext, "HEX")))
		{
			if((!strncmp(pAtaFile->Filename, "bleApp", 6)) ||
			   (!strncmp(pAtaFile->Filename, "BLEAPP", 6)))
			{
				gBleFWFound = 1;
				gBleFWAppSize = pAtaFile->Size;
			}
			else
			{
				error = ATA_findNext(pAtaFile);
				if(error != ATA_ERROR_NONE)
				{
					return (error);
				}
			}
		}
		else
		{
			error = ATA_findNext(pAtaFile);
			if(error != ATA_ERROR_NONE)
			{
				return (error);
			}
		}
	} while(!gBleFWFound);

	return (error);
}

/* Function to find the BLE stack firmware file on SD card */
AtaError find_ble_stack_firmware(Uint8 *stackFound)
{
	AtaError error;

	*stackFound = 0;

	error = ATA_findFirst(pAtaFile);
	if(error != ATA_ERROR_NONE)
	{
		return (error);
	}

	do
	{
		if((!strcmp(pAtaFile->Ext, "hex")) ||
		   (!strcmp(pAtaFile->Ext, "HEX")))
		{
			if((!strcmp(pAtaFile->Filename, "bleStack")) ||
			   (!strcmp(pAtaFile->Filename, "BLESTACK")))
			{
				*stackFound = 1;
				gBleFWStackSize = pAtaFile->Size;
			}
			else
			{
				error = ATA_findNext(pAtaFile);
				if(error != ATA_ERROR_NONE)
				{
					return (error);
				}
			}
		}
		else
		{
			error = ATA_findNext(pAtaFile);
			if(error != ATA_ERROR_NONE)
			{
				return (error);
			}
		}
	} while(!*stackFound);

	return (error);
}

/* Function to open next wave file on the SD card */
static AtaError open_next_file(void)
{
	AtaError error = ATA_ERROR_NONE;

	gWaveFileFound = 0;

	do
	{
		error = ATA_findNext(pAtaFile);
		if(error != ATA_ERROR_NONE)
		{
			return (error);
		}

		if((!strcmp(pAtaFile->Ext, "WAV")) ||
		   (!strcmp(pAtaFile->Ext, "wav")))
		{
			read_wave_header();
			extractWaveInfo(&gWaveHdr, gWaveHdrData);
			if(!checkWaveHeader(&gWaveHdr))
			{
				strcpy((char*)gFileName, pAtaFile->Filename);
				gWaveFileFound = 1;
				error = ATA_ERROR_NONE;
			}
		}

	} while(!gWaveFileFound);

	return (error);
}

/**
 *  \brief  MMCSD call back function
 *
 *   This function is called from the MMCSD read and write APIs
 *   after starting the DMA data transfer
 *
 *  \param  none
 *
 *  \return none
 */
void mmcsdTransferDoneCallBack(void *mmcsdHandle)
{
	/* Wait for the MMCSD interrupt */
	SEM_pend(&SEM_mmcDone, SYS_FOREVER);
}

/* Function to intialize SD card and filesystem */
int init_media(void)
{
	CSL_Status     status;
	Uint16         clockDiv;
	Uint16         rca;
    AtaError      ata_error;

	/* Get the clock divider value for the current CPU frequency */
	clockDiv = 1;

	/* Open Dma channel for MMCSD write */
	dmaWrHandle = DMA_open(CSL_DMA_CHAN0, &dmaWrChanObj, &status);
    if((dmaWrHandle == NULL) || (status != CSL_SOK))
    {
        printf("DMA_open for MMCSD Write Failed!\n");
        return(status);
    }

	/* Open Dma channel for MMCSD read */
	dmaRdHandle = DMA_open(CSL_DMA_CHAN1, &dmaRdChanObj, &status);
    if((dmaRdHandle == NULL) || (status != CSL_SOK))
    {
        printf("DMA_open for MMCSD Read Failed!\n");
        return(status);
    }

	/* Open MMCSD CSL module */
	mmcsdHandle = MMC_open(&pMmcsdContObj, CSL_MMCSD0_INST,
			               CSL_MMCSD_OPMODE_DMA, &status);
	if(mmcsdHandle == NULL)
	{
		printf("MMC_open Failed\n");
		return (status);
	}

	/* Set the DMA handle for MMC access */
	status = MMC_setDmaHandle(mmcsdHandle, dmaWrHandle, dmaRdHandle);
	if(status != CSL_SOK)
	{
		printf("MMC_setDmaHandle for MMCSD Failed\n");
		return(status);
	}

	/* Reset the SD card */
	status = MMC_sendGoIdle(mmcsdHandle);
	if(status != CSL_SOK)
	{
		printf("MMC_sendGoIdle Failed\n");
		return (status);
	}

	/* Check for the card */
    status = MMC_selectCard(mmcsdHandle, &mmcCardObj);
	if((status == CSL_ESYS_BADHANDLE) ||
	   (status == CSL_ESYS_INVPARAMS))
	{
		printf("MMC_selectCard Failed\n");
		return (status);
	}

	/* Verify whether the SD card is detected or not */
	if(mmcCardObj.cardType == CSL_SD_CARD)
	{
#if 0
		printf("SD Card detected\n");

		/* Check if the card is high capacity card */
		if(mmcsdHandle->cardObj->sdHcDetected == TRUE)
		{
			printf("SD card is High Capacity Card\n");
			printf("Memory Access will use Block Addressing\n");
		}
		else
		{
			printf("SD card is Standard Capacity Card\n");
			printf("Memory Access will use Byte Addressing\n");
		}
#endif
	}
	else
	{
		if(mmcCardObj.cardType == CSL_CARD_NONE)
		{
			printf("No Card detected\n");
		}
		else
		{
			printf("SD Card not detected\n");
		}
		printf("Please Insert SD Card\n");
		return(CSL_ESYS_FAIL);
	}

	/* Set the init clock */
    status = MMC_sendOpCond(mmcsdHandle, 70);
	if(status != CSL_SOK)
	{
		printf("MMC_sendOpCond Failed\n");
		return (status);
	}

	/* Send the card identification Data */
	status = SD_sendAllCID(mmcsdHandle, &sdCardIdObj);
	if(status != CSL_SOK)
	{
		printf("SD_sendAllCID Failed\n");
		return (status);
	}

	/* Set the Relative Card Address */
	status = SD_sendRca(mmcsdHandle, &mmcCardObj, &rca);
	if(status != CSL_SOK)
	{
		printf("SD_sendRca Failed\n");
		return (status);
	}

	/* Read the SD Card Specific Data */
	status = SD_getCardCsd(mmcsdHandle, &sdCardCsdObj);
	if(status != CSL_SOK)
	{
		printf("SD_getCardCsd Failed\n");
		return (status);
	}

	/* Set clock for read-write access */
    status = MMC_sendOpCond(mmcsdHandle, clockDiv);
	if(status != CSL_SOK)
	{
		printf("MMC_sendOpCond Failed\n");
		return (status);
	}

	/* Set block length for the memory card
	 * For high capacity cards setting the block length will have
	 * no effect
	 */
	status = MMC_setBlockLength(mmcsdHandle, CSL_MMCSD_BLOCK_LENGTH);
	if(status != CSL_SOK)
	{
		printf("MMC_setBlockLength Failed\n");
		return(status);
	}

	status = MMC_setDataTransferCallback(mmcsdHandle, mmcsdTransferDoneCallBack);
	if(status != CSL_SOK)
	{
		printf("MMC_setDataTransferCallback Failed\n");
		return(status);
	}

	/* Enable MMCSD interrupts */
	IRQ_enable(PROG0_EVENT);

    ata_error = ATA_ERROR_NONE;

	/* Call init function initialize ATA state structure */
    gpstrAtaDrive->AtaInitAtaMediaState = (AtaError (*)(void *))MMC_initState;
	gpstrAtaMMCState->hMmcSd = mmcsdHandle;
    gpstrAtaDrive->pAtaMediaState = gpstrAtaMMCState;
    gpstrAtaDrive->AtaInitAtaMediaState(gpstrAtaDrive);

	/* Set the temp write buffer */
    gpstrAtaDrive->_AtaWriteBuffer = AtaWrBuf;

	/* For partitioned disk, 'diskType' should be 0
	   and for unpartiotioned disk, it should be 1
	 */
	ata_error = ATA_systemInit(gpstrAtaDrive, 0);
	if(ata_error != ATA_ERROR_NONE)
	{
		printf("ATA_systemInit Failed\n");
		printf("Format the SD card\n");
		return(ata_error);
	}

	/* Find the first file available */
    ata_error =  ATA_fileInit(gpstrAtaDrive, pAtaFile);
    if(ata_error) {
        printf("ATA_fileInit error (0x%x)\n", ata_error);
        return(ata_error);
    }

#ifdef DISABLE_BLE_FW_UPGRADE
	open_first_file();
	if(gWaveFileFound == 0)
	{
		fileFormatErrDisp();
		exit(1);
	}
#else
    find_ble_firmware();
#endif

	return (status);
}

volatile Uint8 gInFileRead = 0;
/* Function to read audio data from SD card */
void swi_ReadAudioDataFxn(void)
{
	AtaError err;

	gInFileRead = 1;

	err = read_file();
	if (err == ATA_ERROR_EOF)
	{
		gSysState = SYS_STATE_BUSY;
		pausePlay();
		fileReadStatusDisp();
		err = open_next_file();
		if (gWaveFileFound != 1)
		{
			src_delete();
			open_first_file();
			if(gWaveFileFound == 0)
			{
				fileAccessErrDisp();
				exit(1);
			}
			src_config();
			src_set_samplerate(gWaveHdr.sampleRate);
		}
		else
		{
			src_delete();
			gWaveDataRdBlkOffset = 0;
			gWaveDataWrBlkOffset = 0;
			gFreeDataBlocks = 4;
			src_config();
			src_set_samplerate(gWaveHdr.sampleRate);
			seek_file(22);
			read_file();
			read_file();
		}
		gSysState = SYS_STATE_PLAY;
		updateDisplay();
		resumePlay();
	}
	else if(err != ATA_ERROR_NONE)
	{
		stopPlay();
		fileAccessErrDisp();
		exit(1);
	}

	gInFileRead = 0;
}

/* MMCSD ISR which will be invoked at the end of each data transfer */
void MMCSD_Isr(void)
{
	SEM_post(&SEM_mmcDone);
}

/* Function to read BLE firmware file size in bytes */
Uint32 getBleFirmwareSize(void)
{
	return (gBleFWAppSize);
}

/* Function to read BLE firmware data from SD card.
   Returns the actual number of bytes read from the file */
Uint32 readBleFirmware(Uint16 *data, Uint32 length)
{
	Uint32 words2Read;
	AtaError err;
	Uint32 index;

	if(gSysState == SYS_STATE_BLE_FW_UPGRADE)
	{
		words2Read = length;

		for (index = 0; index < words2Read; index++)
		{
			err = _AtaRead_b(pAtaFile, &data[index], 1);
			if (err != ATA_ERROR_NONE)
			{
				return(0);
			}
		}

		byteshift(data, words2Read);
	}

	return (words2Read);
}
