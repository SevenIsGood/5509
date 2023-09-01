/*
* ble_fw_uprgade.c
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

#include "ble_fw_upgrade.h"
#include "sbl.h"
#include "sbl_cmd.h"
#include "sys_init.h"
#include "audio_data_read.h"

Uint8 SBL_eraseFlash(Uint32 addr, Uint32 size);

/* Global Structure Declaration */
extern GPIO_Handle hGpio;

extern Uint16  imgBuf[SBL_MAX_TRANSFER];
Uint16 dataBuf[DATA_BUFF_SIZE];

Uint32 highAddr = 0;
Uint32 bytesWritten = 0;
Uint32 cmdErrCnt = 0;
Uint32 dataErrCnt = 0;
Uint32 statusErrCnt = 0;
Uint32 totalCnt = 0;
Uint8 dataBuffering = 0;
Uint32 dataBufIndex = 0;
Uint32 dataBufAddr = 0;

Uint32 getBleFirmwareSize(void);
Uint32 readBleFirmware(Uint16 *data, Uint32 length);

/**
 *
 * \brief Structure to hold the hex record information
 */
typedef struct _hexRecord
{
	Uint16 recType;
	Uint16 dataLen;
	Uint32 destAddr;
	Uint16 cheksum;
} hexRecord;

hexRecord gHexRec;

/**
 *  \brief Sends ping command to BLE
 *
 *  \return 0 - if successful, else suitable error code
 */
Uint8 spi_send_ping(void)
{
	Uint8 retVal = 0;
	Uint8 retryCount = 0;
	C55x_delay_msec(2000);

	/* Send ping packet to check the established connection with target
	 * It is observed that BLE is responding with NACK for ping command sometimes.
	 * Retrying in such cases gets proper ACK from BLE. This API tries to send ping
	 * command to BLE three times before returning error
	 */
	while(retryCount < BLE_RETRY_CNT_MAX)
	{
		retVal = SBL_CMD_ping();
		if(retVal == 0)
		{
			/* BLE responded with ACK, break the loop */
			break;
		}
		else
		{
			/* Wait for some time before retrying */
			C55x_delay_msec(20);
		}

		retryCount++;
	}

    return (retVal);
}

/**
 *  \brief Function to enable/disable BLE backdoor
 *
 *  \param bdEnable  [IN]  BLE backdoor control flag
 *                         1 - Enable backdoor
 *                         0 - Disable backdoor
 *
 *  \return 0 - if successful, else suitable error code
 */
CSL_Status ble_backdoor_ctrl(Uint8 bdEnable)
{
	CSL_Status retVal;

	retVal = GPIO_write(hGpio, CSL_GPIO_PIN10, bdEnable);

	return (retVal);
}

/* Function to read the high address in case of extended address record */
void get_high_addr(Uint16 *data)
{
	Uint16 index;

    for (index = 0; index < 4; index++)
    {
    	if((data[index] >= 0x30) && (data[index] <= 0x39))
    	{
    		data[index] = data[index] - 0x30;
    	}
    	else
    	{
    		data[index] = data[index] - 0x41 + 10;
    	}
    }

    highAddr = ((((data[0] ) & 0xF) << 12) |
            			  (((data[1] ) & 0xF) << 8)  |
                          (((data[2] ) & 0xF) << 4)  |
			              (((data[3] ) & 0xF)) );

    highAddr = (highAddr << 16);
}

/* Get the record details from record header */
void get_record_details(Uint16 *data)
{
	Uint16 index;

    for (index = 0; index < 8; index++)
    {
    	if((data[index] >= 0x30) && (data[index] <= 0x39))
    	{
    		data[index] = data[index] - 0x30;
    	}
    	else
    	{
    		data[index] = data[index] - 0x41 + 10;
    	}

    }

	gHexRec.dataLen = (((data[0]) & 0xF) << 4) | ((data[1]) & 0xF);

	gHexRec.destAddr = ((((data[2] ) & 0xF) << 12) |
            			  (((data[3] ) & 0xF) << 8)  |
                          (((data[4] ) & 0xF) << 4)  |
			              (((data[5] ) & 0xF)) );

	gHexRec.destAddr |= highAddr;

	gHexRec.recType = (((data[6] ) & 0xF) << 4) | ((data[7] ) & 0xF);

}

/* Convert ASCII to hex */
void ascii2HexBytes(Uint16 *data, Uint32 length)
{
    Uint32 index;

    for (index = 0; index < length; index++)
    {
    	if((data[index] >= 0x30) && (data[index] <= 0x39))
    	{
    		data[index] = data[index] - 0x30;
    	}
    	else
    	{
    		data[index] = data[index] - 0x41 + 10;
    	}

    }

    for (index = 0; index < length/2; index++)
    {
    	data[index] = (((data[2*index]) & 0xF) << 4) | ((data[2*index + 1]) & 0xF);
    }
}

/* Function to send the data to BLE */
Uint8 download_data(Uint16 *data, Uint32 destAddr, Uint16 dataLen)
{
	Uint8 retVal = 0;
	Uint8 retryCount = 0;

	while(retryCount < BLE_RETRY_CNT_MAX)
	{
		// Send data to target
		retVal = SBL_CMD_download(destAddr, dataLen);
		if(retVal == 0)
		{
			/* BLE responded with ACK, break the loop */
			break;
		}
		else
		{
			/* Wait for some time before retrying */
			C55x_delay_msec(10);
		}

		retryCount++;
	}

	if(retryCount >= BLE_RETRY_CNT_MAX)
	{
		return (BLE_FWUP_STATUS_FAIL_CMD);
	}

	retVal = SBL_CMD_sendData(data, dataLen);
	if(retVal)
	{
		return (BLE_FWUP_STATUS_FAIL_DATA);
	}

	retVal = SBL_CMD_getStatus();
	if(retVal != SBL_CMD_RET_SUCCESS)
	{
		statusErrCnt++;  // Ignoring status errors. Should be OK as along as previous two commands successful
	}

	return BLE_FWUP_STATUS_SUCESS;
}

/* Reset BLE firmware upgrade state variables */
static void reset_state(void)
{
    bytesWritten  = 0;
    cmdErrCnt     = 0;
    dataErrCnt    = 0;
    statusErrCnt  = 0;
    totalCnt      = 0;
    dataBuffering = 0;
    dataBufIndex  = 0;
    dataBufAddr   = 0;
    highAddr      = 0;
}

/* Function to parse the hex file and send the data to BLE */
Uint8 parse_hex_file(Uint8 erase)
{
    Uint32 imgSize = 0;
    Uint32 index, count = 0;
    Uint32 bytesRead = 0;
    Uint8 retVal;
	Uint16 recHeader;
	Uint16 recLen;

    reset_state();

    /* Not earasing last block of flash to avoid BLE backdoor disable due
     * to failure in BLE firmware upgrade
     */
	imgSize = (Uint32)124*1024;

	if(erase)
	{
		// Erase the flash
		SBL_eraseFlash(0, imgSize);
	}

	while(1)
	{
		/* Loop till we find the record header */
		recHeader = 0;
		while(recHeader != 0x3A)
		{
			bytesRead = readBleFirmware(&recHeader, 1);
			if(!bytesRead)
			{
				return (BLE_FWUP_STATUS_FAIL_FILE_READ);
			}
		}

		/* Found start of record. Read the record up to data */
		bytesRead = readBleFirmware((Uint16*)imgBuf, 8);
		if(!bytesRead)
		{
			return (BLE_FWUP_STATUS_FAIL_FILE_READ);
		}

		get_record_details((Uint16*)imgBuf);

		if(gHexRec.recType == HEX_FILE_REC_DATA)
		{
			totalCnt++;

			/* Data download command needs data length to be multiples of 4 */
			if((!dataBuffering) && (gHexRec.dataLen & 0x3))
			{
				/* Data length is not multiple of 4. Need to buffer the data till
				* we read enough data to use with download command
				*/
				dataBufIndex = 0;
				dataBuffering = 1;
				dataBufAddr = gHexRec.destAddr;
			}

			if(dataBuffering)
		    {
				if(dataBufIndex == 0)
				{
					/* First block during data buffering. Can be read without further checks */
					bytesRead = readBleFirmware(&dataBuf[dataBufIndex], 2*gHexRec.dataLen);
					if(!bytesRead)
					{
						return (BLE_FWUP_STATUS_FAIL_FILE_READ);
					}
					ascii2HexBytes(&dataBuf[dataBufIndex], 2*gHexRec.dataLen);
					dataBufIndex += gHexRec.dataLen;
			    }
			  	else
			  	{
					/* Not first data block of data buffering.
					 * Check whether the data to be read is contiguous or not with previously read block
					 */
					if((dataBufAddr + dataBufIndex) != gHexRec.destAddr)
					{
						/* Not contiguous block of data, flush the existing data with padding.
						 * Normally memory is allocated in contiguous blocks except for odd bytes.
						 * It should be OK to do paadding to fill th holes in case of odd bytes
						 */
						dataBufIndex += (4 - (dataBufIndex % 4));
						retVal = download_data((Uint16*)dataBuf, dataBufAddr, dataBufIndex);
						if(retVal != BLE_FWUP_STATUS_SUCESS)
						{
							return (retVal);
						}

						dataBuffering = 0;

						/* Now check if the current block of data can be written */
						if(gHexRec.dataLen & 0x3)
						{
							/* Data length is not multiple of 4. Need to buffer the data till
							 * we read enough data to use with download command
							 */
							dataBufIndex = 0;
							dataBuffering = 1;
							dataBufAddr = gHexRec.destAddr;
							bytesRead = readBleFirmware(&dataBuf[dataBufIndex], 2*gHexRec.dataLen);
							if(!bytesRead)
							{
								return (BLE_FWUP_STATUS_FAIL_FILE_READ);
							}
							ascii2HexBytes(&dataBuf[dataBufIndex], 2*gHexRec.dataLen);
							dataBufIndex += gHexRec.dataLen;
					    }
					  	else
					  	{
							/* Write current block of data */
							bytesRead = readBleFirmware((Uint16*)imgBuf, 2*gHexRec.dataLen);
							if(!bytesRead)
							{
								return (BLE_FWUP_STATUS_FAIL_FILE_READ);
							}
							ascii2HexBytes((Uint16*)imgBuf, 2*gHexRec.dataLen);

							retVal = download_data(imgBuf, gHexRec.destAddr, gHexRec.dataLen);
							if(retVal != BLE_FWUP_STATUS_SUCESS)
							{
								return (retVal);
							}
					    }
				    }
				  	else
				  	{
						/* Current block is in contiguos memory location to buffered data.
						 * Buffer it based on the data buffer size or write some part of it
						 */
						if((dataBufIndex + 2*gHexRec.dataLen) < DATA_BUFF_SIZE)
						{
							bytesRead = readBleFirmware(&dataBuf[dataBufIndex], 2*gHexRec.dataLen);
							if(!bytesRead)
							{
								return (BLE_FWUP_STATUS_FAIL_FILE_READ);
							}
							ascii2HexBytes(&dataBuf[dataBufIndex], 2*gHexRec.dataLen);
							dataBufIndex += gHexRec.dataLen;
						}
						else
						{
							/* Not enough buffer free. Write some data */
							recLen = 4 * (dataBufIndex/4);

							retVal = download_data((Uint16*)dataBuf, dataBufAddr, recLen);
							if(retVal != BLE_FWUP_STATUS_SUCESS)
							{
								return (retVal);
							}

							dataBufAddr += recLen;

							/* Move the remaining bytes to start of buffer */
							count = 0;
							for(index = recLen; index < dataBufIndex; index++)
							{
								dataBuf[count++] = dataBuf[index];
						    }
						  	dataBufIndex = count;

						  	bytesRead = readBleFirmware(&dataBuf[dataBufIndex], 2*gHexRec.dataLen);
							if(!bytesRead)
							{
								return (BLE_FWUP_STATUS_FAIL_FILE_READ);
							}

						  	ascii2HexBytes(&dataBuf[dataBufIndex], 2*gHexRec.dataLen);
						  	dataBufIndex += gHexRec.dataLen;
					  	}

					  	if(!(gHexRec.dataLen & 0x3))
					  	{
							/* Data length is multiple of 4. Write the data */
							retVal = download_data((Uint16*)dataBuf, dataBufAddr, dataBufIndex);
							if(retVal != BLE_FWUP_STATUS_SUCESS)
							{
								return (retVal);
							}
						  	dataBuffering = 0;
					  	}
					  	else
					  	{
							/* Nothing to do, just continue to buffer the data */
					  	}
				  	}
			  	}
		  	}
		  	else
		  	{
				bytesRead = readBleFirmware((Uint16*)imgBuf, 2*gHexRec.dataLen);
				if(!bytesRead)
				{
					return (BLE_FWUP_STATUS_FAIL_FILE_READ);
				}

				ascii2HexBytes((Uint16*)imgBuf, 2*gHexRec.dataLen);
				retVal = download_data(imgBuf, gHexRec.destAddr, gHexRec.dataLen);
				if(retVal != BLE_FWUP_STATUS_SUCESS)
				{
					return (retVal);
				}
		  	}
	  	}
	  	else if (gHexRec.recType == HEX_FILE_REC_EXT_ADDR)
	  	{
			bytesRead = readBleFirmware((Uint16*)imgBuf, 4);
			if(!bytesRead)
			{
				return (BLE_FWUP_STATUS_FAIL_FILE_READ);
			}

			get_high_addr((Uint16*)imgBuf);
	  	}
	  	else if (gHexRec.recType == HEX_FILE_REC_EOF)
	  	{
			// Reached end of file
			bytesWritten += gHexRec.dataLen;
			break;
	  	}
	  	else
	  	{
			return (BLE_FWUP_STATUS_FAIL_REC);
	  	}

	  	// Update bytes written and image address of next read
	  	bytesWritten += gHexRec.dataLen;

	  	if(bytesWritten >= imgSize)
	  	{
			gHexRec.recType = 0;
			break;
	  	}
	}

	return (BLE_FWUP_STATUS_SUCESS);
}

/**
 * \brief Function to upgrade BLE firmware
 *
 * \return - 0 for success, else suitable error code
 *
 */
Uint16 bleFWUpgrade(void)
{
    CSL_Status status = 0;
    Uint8 retVal;
    Uint8 stackFound;

	// Enable BLE backdoor
    status = ble_backdoor_ctrl(1);
    if(status != 0)
    {
     	return (BLE_FWUP_STATUS_ERROR);
    }

	// Reset BLE module so that backdoor state is enetered
    reset_ble();

	//Send ping packet to check the established connection with target
    retVal = spi_send_ping();
    if(retVal != 0)
    {
		// Ping failed after retries. Cannot proceed with upgrade. Return error code
    	return (BLE_FWUP_STATUS_FAIL_PING);
    }

	// Parse the BLE firmware file. BLE flash will be earsed now
    retVal = parse_hex_file(1);
    if(retVal != BLE_FWUP_STATUS_SUCESS)
    {
		// Some error while sending the firmware to BLE. Cannot proceed. Reset BLE and return
		reset_ble();
    	return (retVal);
    }

    /* Check for stack */
    find_ble_stack_firmware(&stackFound);
    if(stackFound == 1)
    {
    	/* Write the stack */
    	retVal = parse_hex_file(0);
		if(retVal != BLE_FWUP_STATUS_SUCESS)
		{
			// Some error while sending the firmware to BLE. Cannot proceed. Reset BLE and return
			reset_ble();
			return (retVal);
		}
    }

    status = ble_backdoor_ctrl(0);
    if(status != 0)
    {
     	return (BLE_FWUP_STATUS_ERROR);
    }

    reset_ble();

    return (BLE_FWUP_STATUS_SUCESS);
}

