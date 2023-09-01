/* --COPYRIGHT--,BSD
 * Copyright (c) 2014-2016, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/
/**************************************************************************************************
  @headerfile:    sbl_tl.c
  Revised:        $Date: 2014-04-16 15:24:18 -0700 (Wed, 16 Apr 2014) $
  Revision:       $Revision: 38209 $

  Description:    This file contains transport layer code for SBL on MSP432P4xx

**************************************************************************************************/

/*********************************************************************
 * INCLUDES
 */

#include <string.h>

#include "sbl.h"
#include "sbl_tl.h"

#include "csl_spi.h"
#include <stdio.h>

/*********************************************************************
 * CONSTANTS
 */

enum {
  SBL_HDR_LEN_IDX = 0,
  SBL_HDR_CKS_IDX,
  SBL_HDR_SIZE
};

#define SBL_ACK_SIZE            2

#define DEVICE_ACK              0xCC
#define DEVICE_NACK             0x33

#define SBL_UART_BR             115200

#define	SPI_CLK_DIV				(100)
/**< SPI Clock Divisor */

#define	SPI_FRAME_LENGTH		(1)
/**< SPI Frame length */

#define SPI_RESPONSE_RETRY_CNT  (0xFFFF)  //Too many retries, just to ensure BLE firmware upgrade does not fail

const Uint8 NACK[] =          { 0x00, 0x33 };
const Uint8 ACK[] =           { 0x00, 0xcc };
const Uint8 BAUD[] =          { 0x55, 0x55 };

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

extern CSL_SpiHandle hSpi;
SPI_OperMode opMode;
SPI_Config		hwConfig;
/*********************************************************************
 * LOCAL FUNCTIONS
 */

static Uint8 SBL_TL_CKS(Uint8 cmd, Uint8 *pData, Uint16 len);

static Uint8 SBL_TL_sendACK(Uint8 ack);

static Uint8 SBL_TL_getRspACK(void);

void C55x_delay_msec(int msecs);

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/**
 * @fn      SBL_TL_open
 *
 * @brief   Open device port for communication with the target device. Currently
 *          only supports UART
 *
 * @param   pType - SBL_DEV_INTERFACE_[UART,SPI]
 * @param   pID - local serial interface ID (i.e. CC2650_UART0)
 *
 * @return  Uint8 - SBL_SUCCESS, SBL_FAILURE
 */
Uint8 SBL_TL_open(Uint8 pType, Uint8 pID)
{
	Int16 			retVal;

	/*Open Spi and get an handler*/
     SPI_init();
     hSpi = SPI_open(SPI_CS_NUM_2, SPI_POLLING_MODE);
     if(hSpi == NULL)
     {
   	  //printf("Error initializing SPI\n");
     }
     /** Set the hardware configuration 							*/
     	hwConfig.spiClkDiv	= SPI_CLK_DIV;
     	hwConfig.wLen		= SPI_WORD_LENGTH_8;
     	hwConfig.frLen		= SPI_FRAME_LENGTH;
     	hwConfig.wcEnable	= SPI_WORD_IRQ_DISABLE;
     	hwConfig.fcEnable	= SPI_FRAME_IRQ_DISABLE;
     	hwConfig.csNum		= SPI_CS_NUM_2;
     	hwConfig.dataDelay	= SPI_DATA_DLY_0;
     	hwConfig.csPol		= SPI_CSP_ACTIVE_LOW;
     	hwConfig.clkPol		= SPI_CLKP_LOW_AT_IDLE;
     	hwConfig.clkPh		= SPI_CLK_PH_FALL_EDGE;

     	retVal = SPI_config(hSpi, &hwConfig);
     	if(retVal != 0)
     	{
     		return SBL_FAILURE;
     	}

     	return SBL_SUCCESS;

}

Uint16 SPI_sendCommand (CSL_SpiHandle hSpi,
						Uint16	*writeBuffer,
						Uint16	bufLen,
						Uint32	timeout)
{
	Uint16 	getWLen;
	volatile Uint16 	bufIndex;
	Uint16 	spiStatusReg;
	volatile Uint16 	spiBusyStatus;
	volatile Uint16 	spiWcStaus;
	Uint16     readData = 0;
	Uint16     writeData = 0;
	Uint16     ackRcvd = 0;

	bufIndex = 0;
	if((NULL == writeBuffer) || (0 == bufLen))
	{
		return (readData);
	}

	/* Read the word length form the register */
#if (defined(CHIP_C5517))
	getWLen = CSL_FEXT(CSL_SPI_REGS->SPICMDU, SPI_SPICMDU_CLEN) + 1;
#else
	getWLen = CSL_FEXT(CSL_SPI_REGS->SPICMD2, SPI_SPICMD2_CLEN) + 1;
#endif

	if(getWLen >= SPI_MAX_WORD_LEN)
	{
		return (readData);
	}

	writeData = writeBuffer[bufIndex];

	/* Write Word length set by the user */
	//while(bufIndex < bufLen)
	while(!ackRcvd)
	{
		/* Write to registers more then 16 bit word length */
		if(getWLen == 16)
		{
#if (defined(CHIP_C5517))
			CSL_SPI_REGS->SPIDRU = writeData;
			CSL_SPI_REGS->SPIDRL = 0x0000;
#else
			CSL_SPI_REGS->SPIDR2 = writeData;
			CSL_SPI_REGS->SPIDR1 = 0x0000;
#endif
			bufIndex ++;
		}
		/* Write to register less then or equal to 16 bit word length */
		else if(getWLen == 8)
		{
#if (defined(CHIP_C5517))
			CSL_SPI_REGS->SPIDRU = (Uint16)(writeData << 0x08);
			CSL_SPI_REGS->SPIDRL = 0x0000;
#else
			CSL_SPI_REGS->SPIDR2 = (Uint16)(writeData << 0x08);
			CSL_SPI_REGS->SPIDR1 = 0x0000;

			bufIndex++;
#endif
		}

		/* Set command for Writting to registers */
#if (defined(CHIP_C5517))
		CSL_FINS(CSL_SPI_REGS->SPICMDU, SPI_SPICMDU_CMD, SPI_WRITE_CMD);
#else
		CSL_FINS(CSL_SPI_REGS->SPICMD2, SPI_SPICMD2_CMD, SPI_WRITE_CMD);
#endif

	    do { /* Check for Word Complete status */
#if (defined(CHIP_C5517))
	        spiStatusReg=CSL_FEXT(CSL_SPI_REGS->SPISTATL,SPI_SPISTATL_CC);
#else
	        spiStatusReg=CSL_FEXT(CSL_SPI_REGS->SPISTAT1,SPI_SPISTAT1_CC);
#endif
	    }while((spiStatusReg & 0x01) == 0x0);

	    do { /* Check for busy status */
#if (defined(CHIP_C5517))
	        spiStatusReg=CSL_FEXT(CSL_SPI_REGS->SPISTATL,SPI_SPISTATL_BSY);
#else
	        spiStatusReg=CSL_FEXT(CSL_SPI_REGS->SPISTAT1,SPI_SPISTAT1_BSY);
#endif
	    }while((spiStatusReg & 0x01) == 0x01);

		if(bufIndex > bufLen)
		{
			while(1)
			{
				readData = 0;
				SPI_read(hSpi, &readData, 1);
				if((readData == 0xCC) || (readData == 0x33))
				{
					ackRcvd = 1;
					break;
				}

				timeout--;
				if(!timeout)
				{
					return (readData);
				}
			}
			writeData = 0;
		}
		else
		{
			writeData = writeBuffer[bufIndex];
		}
	}

	return (readData);
}

CSL_Status SPI_readResponse (CSL_SpiHandle hSpi, Uint16 *readBuf, Uint16 length)
{
	CSL_Status status;

	while(1)
	{
		status = SPI_read(hSpi, readBuf, length);
		if((readBuf[0] != 0) || (readBuf[1] != 0))
		{
			break;
		}
	}

	return (status);
}

/**
 * @fn      SBL_TL_getRspACK
 *
 * @brief   Get ACK/NACK response from the target device
 *
 * @param   None.
 *
 * @return  Uint8 - SBL_DEV_ACK, SBL_DEV_NACK, or SBL_FAILURE if neither ACK/NACK
 */
Uint8 SBL_TL_getRspACK(void)
{
	Uint16 rsp[SBL_ACK_SIZE];

	SPI_readResponse(hSpi,rsp,sizeof(rsp));

	if (!(memcmp(rsp, ACK, sizeof(rsp))))
	{
		return SBL_DEV_ACK;
	}
	else if (!(memcmp(rsp, NACK, sizeof(rsp))))
  	{
		return SBL_DEV_NACK;
	}
	else
	{
		return SBL_FAILURE;
	}
}

/**
 * @fn      SBL_TL_getRsp
 *
 * @brief   Get response message from the target device
 *
 * @param   pData - pointer to byte array to store data
 * @param   maxSize - size of byte array pointed to by pData
 * @param   len - will be set to the length of data written to pData
 *
 * @return  Uint8 - SBL_SUCCESS or SBL_FAILURE
 */
Uint8 SBL_TL_getRsp(Uint8 *pData, Uint16 maxSize, Uint16 *len)
{
	Uint16 hdr[SBL_HDR_SIZE];

	// Read Response header
	SPI_read(hSpi,hdr,sizeof(hdr));


  // Check if length of incoming response is too long
  if (maxSize < (hdr[SBL_HDR_LEN_IDX] - sizeof(hdr)))
  {
    return SBL_FAILURE;
  }

  // Read Response Payload
  SPI_read(hSpi, (Uint16*)pData, hdr[SBL_HDR_LEN_IDX] - sizeof(hdr));

  // Verify Checksum
  if (hdr[SBL_HDR_CKS_IDX] != SBL_TL_CKS(0, pData, hdr[SBL_HDR_LEN_IDX] - sizeof(hdr)))
  {
    return SBL_FAILURE;
  }

  // Set length parameter to length of payload data
  *len = hdr[SBL_HDR_LEN_IDX];

  // Respond with ACK
  return SBL_TL_sendACK(DEVICE_ACK);
}

/**
 * @fn      SBL_TL_sendCmd
 *
 * @brief   Sends a SBL command to target device
 *
 * @param   cmd - command ID
 * @param   pData - pointer to command payload
 * @param   len - length of command payload
 *
 * @return  Uint8 - SBL_SUCCESS
 */
Uint8 SBL_TL_sendCmd(Uint8 cmd, Uint8 *pData, Uint16 len)
{
	Uint16 hdr[SBL_HDR_SIZE + 1]; // Header + CMD byte
	Uint8 ackRsp;

	memset(hdr, 0, sizeof(hdr));

	// Initalize Header
  hdr[SBL_HDR_LEN_IDX] = len + sizeof(hdr);             // Length
  hdr[SBL_HDR_CKS_IDX] = SBL_TL_CKS(cmd, pData, len);   // Checksum
  ////printf("CHeck sum is %d\n", hdr[SBL_HDR_CKS_IDX]);
  hdr[2] = cmd;                                         // Command

  // Send Packet
  if (len)
  {
	  SPI_write(hSpi, hdr, sizeof(hdr));
	  C55x_delay_msec(5);
	  ackRsp = SPI_sendCommand(hSpi, (Uint16*)pData, len, SPI_RESPONSE_RETRY_CNT);
  }
  else
  {
	  ackRsp = SPI_sendCommand(hSpi, hdr, sizeof(hdr), SPI_RESPONSE_RETRY_CNT);
  }

 return ackRsp ==0xcc ? SBL_SUCCESS : SBL_FAILURE;
}

Uint8 SBL_TL_sendCmd16(Uint8 cmd, Uint16 *pData, Uint16 len)
{
	Uint16 hdr[SBL_HDR_SIZE + 1]; // Header + CMD byte
	Uint8 ackRsp;

	memset(hdr, 0, sizeof(hdr));

	// Initalize Header
  hdr[SBL_HDR_LEN_IDX] = len + sizeof(hdr);             // Length
  hdr[SBL_HDR_CKS_IDX] = SBL_TL_CKS(cmd, (Uint8*)pData, len);   // Checksum
  hdr[2] = cmd;                                         // Command

  // Send Packet
  if (len)
  {
	  SPI_write(hSpi, hdr, sizeof(hdr));
	  C55x_delay_msec(5);
	  ackRsp = SPI_sendCommand(hSpi, (Uint16*)pData, len, SPI_RESPONSE_RETRY_CNT);
  }
  else
  {
	  ackRsp = SPI_sendCommand(hSpi, hdr, sizeof(hdr), SPI_RESPONSE_RETRY_CNT);
  }

 return ackRsp ==0xcc ? SBL_SUCCESS : SBL_FAILURE;

}

/**
 * @fn      SBL_TL_sendACK
 *
 * @brief   Send ACK/NACK response to the target device
 *
 * @param   ack - ACK/NACK value
 *
 * @return  Uint8 - SBL_SUCCESS
 */
Uint8 SBL_TL_sendACK(Uint8 ack)
{
	Uint16 rsp[2];

  // Initalize response
  rsp[0] = 0x00; // Must write zero byte first for response
  rsp[1] = ack;

  SPI_write(hSpi,rsp,sizeof(rsp));

  return SBL_SUCCESS;
}


/**
 * @fn      SBL_TL_close
 *
 * @brief   Close interface to target device
 *
 * @param   None.
 *
 * @return  None.
 */
void SBL_TL_close(void)
{
	SPI_close(hSpi);
}

/**
 * @fn      SBL_TL_CKS
 *
 * @brief   Calculates checksum over a byte array
 *
 * @param   cmd - optional command byte from sending messages
 * @param   pData - pointer to byte array to store data
 * @param   len - length of byte array pointed to by pData
 *
 * @return  Uint8 - checksum
 */
Uint8 SBL_TL_CKS(Uint8 cmd, Uint8 *pData, Uint16 len)
{
  Uint8 checksum = cmd;
  Uint16 i;

  for(i = 0; i < len; i++)
  {
    checksum += pData[i];
  }

  return checksum;
}

/**
 * @fn      SBL_TL_flushRxBuffer
 *
 * @brief   Flushes receive buffer
 *
 * @param   None.
 *
 * @return  None.
 */
void SBL_TL_flushRxBuffer( void )
{

}

