/*******************************************************************************
*  Filename:       bsp_spi.c
*  Revised:        $Date$
*  Revision:       $Revision$
*
*  Description:    Layer added on top of RTOS driver for backward
*                  compatibility with non RTOS SPI driver.
*
*  Copyright (C) 2015-2016 Texas Instruments Incorporated - http://www.ti.com/
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
*    documentation and/or other materials provided with the distribution.
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
*******************************************************************************/
#ifdef TI_DRIVERS_SPI_INCLUDED

/*******************************************************************************
 * INCLUDES
 */
#include <ti/sysbios/family/arm/cc26xx/Power.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/drivers/spi/SPICC26XXDMA.h>
#include <ti/drivers/dma/UDMACC26XX.h>

#include <driverlib/ssi.h>

#include "Board.h"
#include "bsp_spi.h"
#include "string.h"

/*******************************************************************************
 * GLOBAL variables
 */

/*******************************************************************************
 * LOCAL variables
 */
static SPI_Handle spiHandle = NULL;
static SPI_Params spiParams;

uint32_t txCbCnt,rxCbCnt = 0;
uint8_t gSpiTxBuf[DATA_BUF_SIZE];
uint8_t gSpiRxBuf[DATA_BUF_SIZE];
volatile uint8_t spiReadPending = 0;
extern int gNotifyApp;
extern uint8_t gNotifyVal;

/*******************************************************************************
 * @fn          bspSpiCallback
 *
 * @brief       SPI callback function
 *
 * @param       handle - SPI handle
 * @param       transaction - SPI transfer handle
 *
 */
void bspSpiCallback (SPI_Handle handle, SPI_Transaction *transaction)
{
	if(transaction->rxBuf)
	{
		if(spiReadPending)
		{
			spiReadPending = 0;

			//Read the play status from DSP.
			// Currently only play status is communicated from DSP
			if(gSpiRxBuf[1] == 0x3)
			{
				gNotifyApp = 1;
				gNotifyVal = gSpiRxBuf[3];
			}
		}

		//rxCbCnt++;  //debug
	}
	else
	{
		//txCbCnt++;  //debug
	}
}

/*******************************************************************************
 * @fn          bspSpiWrite
 *
 * @brief       Write to an SPI device
 *
 * @param       buf - pointer to data buffer
 * @param       len - number of bytes to write
 *
 * @return      '0' if success, -1 if failed
 */
int bspSpiWrite(const uint8_t *buf, size_t len)
{
  SPI_Transaction spiTransaction;

  spiTransaction.count  = len;
  //spiTransaction.txBuf  = (void*)buf;
  spiTransaction.txBuf  = (void*)gSpiTxBuf;
  spiTransaction.arg    = spiHandle;
  //spiTransaction.rxBuf  = NULL;
  spiTransaction.rxBuf  = (void*)gSpiRxBuf;

  return SPI_transfer(spiHandle, &spiTransaction) ? 0 : -1;
}


/*******************************************************************************
 * @fn          bspSpiRead
 *
 * @brief       Read from an SPI device
 *
 * @param       buf - pointer to data buffer
 * @param       len - number of bytes to write
 *
 * @return      '0' if success, -1 if failed
 */
int bspSpiRead(uint8_t *buf, size_t len)
{
  SPI_Transaction spiTransaction;

  spiTransaction.count  = len;
  spiTransaction.txBuf  = NULL;
  spiTransaction.arg    = spiHandle;
  //spiTransaction.rxBuf  = NULL;
  spiTransaction.rxBuf  = (void*)gSpiRxBuf;

  spiReadPending = 1;
  return SPI_transfer(spiHandle, &spiTransaction) ? 0 : -1;
}


/*******************************************************************************
 * @fn          bspSpiWriteRead
 *
 * @brief       Write to and read from an SPI device in the same transaction
 *
 * @param       buf - pointer to data buffer
 * @param       wlen - number of bytes to write
 * @param       rlen - number of bytes to read
 *
 * @return      '0' if success, -1 if failed
 */
int bspSpiWriteRead(uint8_t *buf, uint8_t wlen, uint8_t rlen)
{
  SPI_Transaction masterTransaction;
  bool success;

  masterTransaction.count  = wlen + rlen;
  masterTransaction.txBuf  = buf;
  masterTransaction.arg    = NULL;
  masterTransaction.rxBuf  = buf;

  success = SPI_transfer(spiHandle, &masterTransaction);
  if (success)
  {
    memcpy(buf,buf+wlen,rlen);
  }

  return success ? 0 : -1;
}

/*******************************************************************************
 * @fn          bspSpiOpen
 *
 * @brief       Open the RTOS SPI driver
 *
 * @param       none
 *
 * @return      none
 */
void bspSpiOpen(void)
{
#if 0
  if (hSpiPin != NULL)
  {
    // Remove IO configuration of SPI lines
    PIN_close(hSpiPin);

    hSpiPin = NULL;
  }
#endif
  if (spiHandle == NULL)
  {
    /*  Configure SPI as master, 1 mHz bit rate*/
    SPI_Params_init(&spiParams);
    //spiParams.bitRate = 1000000;
    //spiParams.mode         = SPI_MASTER;
    //spiParams.transferMode = SPI_MODE_BLOCKING;
    spiParams.bitRate = 4000000;
    spiParams.mode = SPI_SLAVE;
    spiParams.transferMode=SPI_MODE_CALLBACK;
    spiParams.transferCallbackFxn = bspSpiCallback;

    /* Attempt to open SPI. */
    spiHandle = SPI_open(Board_SPI0, &spiParams);
  }
#if 0
    if (spiHandle == NULL)
    {
      Task_exit();
    }
  }

  nUsers++;
#endif
}

/*******************************************************************************
 * @fn          bspSpiClose
 *
 * @brief       Close the RTOS SPI driver
 *
 * @return      none
 */
void bspSpiClose(void)
{
#if 0
  nUsers--;

  if (nUsers > 0)
  {
    // Don't close the driver if still in use
    return;
  }
#endif
  if (spiHandle != NULL)
  {
    // Close the RTOS driver
    SPI_close(spiHandle);
    spiHandle = NULL;
  }
#if 0
  if (hSpiPin == NULL)
  {
    // Configure SPI lines as IO and set them according to BoardSpiInitTable
    hSpiPin = PIN_open(&pinState, BoardSpiInitTable);
  }
#endif
 // nUsers = 0;
}


/*******************************************************************************
 * @fn          bspSpiFlush
 *
 * @brief       Get rid of garbage from the slave
 *
 * @param       none
 *
 * @return      none
 */
void bspSpiFlush(void)
{
    /* make sure SPI hardware module is done  */
    while(SSIBusy(((SPICC26XX_HWAttrs*)spiHandle->hwAttrs)->baseAddr))
    { };
}

#endif