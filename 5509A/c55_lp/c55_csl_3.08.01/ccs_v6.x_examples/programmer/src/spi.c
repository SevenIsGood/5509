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


/*********************************************************************
*    spi.c
**********************************************************************
*   TMS320C55x code for accessing SPI devices on C5505.
**********************************************************************/
#include "registers.h"
#include "sysctrl.h"
#include "spi.h"

/*******************************************************************
* void SPI_RESET(void)
* input:
*        None
* Output:
*        None
* Return:
*        None;
*Description:
*     This function is to reset the SPI module in the MD100
********************************************************************/
void vSPI_RESET(void)
{
   /* Reset SPI Module */
   PeripheralReset(PERIPH_RESET_LCD_IIS2_IIS3_UART_SPI, 0x04, 100);
}

/*******************************************************************
* void SPI_INIT(unsigned char clock_divider)
* input:
*        clock_divider     set the clock divider for SPI
* Output:
*        None
* Return:
*        None;
*Description:
*     This function is to init the SPI module in the MD100
********************************************************************/
void vSPI_INIT(unsigned char clock_divider)
{
   //reset the SPI
   vSPI_RESET();

   //SPI init
   IOPORT_REG_SPI_CLKCTR = 0;
   //Setup the clock
   IOPORT_REG_SPI_CLKDIV = clock_divider - 1;
   //CS0 active low, Data delay = 00, data shifted out on falling edge, clock inactive when low
   IOPORT_REG_SPI_DEVCS10 = 0;
   //enable the clock
   IOPORT_REG_SPI_CLKCTR = SPI_CLK_ENABLE;
}

/*************************************************************************
*void SPI_READ_WRITE_N_BYTES(unsigned short *buffer, unsigned short count,
*                    unsigned short *cmdbuffer, unsigned short cmdcount,
*                                   unsigned short readWrite )
* input:
*        buffer: read/write buffer
*        count:   read/write count
*        cmddbuffer: commandbuffer
*        cmdcount:   command length
*        readWrote:  read/write operation
* Output:
*        None
* Return:
*        None;
*Description:
*     This function is to read/write from SPI module
********************************************************************/
void SPI_READ_WRITE_N_BYTES(unsigned short *buffer, unsigned short count,
               unsigned short *cmdbuffer, unsigned short cmdcount, unsigned short readWrite )
{
   unsigned short total_size;
   unsigned short spi_cmd = 0;
   unsigned short status;
   int i;

   //enable WIRQ//this should put to FLEN field in the command 1 register;
   total_size = count + cmdcount;
   //enable WIRQ and FIRQ
   spi_cmd |= SPI_CMD_WIRQ | SPI_CMD_FIRQ;
   //Frame length = total - 1;
   spi_cmd |= (total_size - 1);
   IOPORT_REG_SPI_CMD1 = spi_cmd;
   //preconfig CMD2, 8bit per bytes, CS0
   spi_cmd = (SPI_CMD_8BIT_WLEN << SPI_CMD_WLEN_SHIFT) | (SPI_CMD_CS0 << SPI_CMD_CS_SHIFT);
   IOPORT_REG_SPI_CMD2 = spi_cmd;

   //send out the command first
   for (i=0;i<total_size;i++)
   {
      //send out the command first
      if (i <cmdcount)
         IOPORT_REG_SPI_DAT2 = (*cmdbuffer++ << 8);

      //write data out
      if ((readWrite == SPI_CMD_WRITE ) && (i >= cmdcount))
      {
         IOPORT_REG_SPI_DAT2 = (*buffer++ << 8);
      }
      //set the LSB to 0 first
      IOPORT_REG_SPI_DAT1 = 0;
      //send out write or right command
      IOPORT_REG_SPI_CMD2 |= readWrite;

      //check the status, word complete
      do
      {
         status = IOPORT_REG_SPI_STAT1;
      }
      while ((status & SPI_STATUS_WC) != SPI_STATUS_WC);

      //check the status, bus busy
      do
      {
         status = IOPORT_REG_SPI_STAT1;
      }
      while ((status & SPI_STATUS_BUSY) == SPI_STATUS_BUSY);

      //read data in
      if ((readWrite == SPI_CMD_READ) && (i>=cmdcount))
      {
         *buffer++ = IOPORT_REG_SPI_DAT1 & 0xFF;
         IOPORT_REG_SPI_DAT1 = 0;
         IOPORT_REG_SPI_DAT2 = 0;
      }
   }
}
