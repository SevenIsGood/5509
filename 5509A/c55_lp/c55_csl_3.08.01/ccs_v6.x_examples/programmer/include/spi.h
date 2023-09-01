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
*    spi.h
**********************************************************************/
#ifndef SPI_H
#define SPI_H

/*********************************************************************
* Bit-field definitions for SPI registers...
**********************************************************************/

/* clock regsiter */
#define SPI_CLK_ENABLE       0x8000
/* device config register */
/* command register */
#define SPI_CMD_WIRQ         0x4000
#define SPI_CMD_FIRQ         0x8000
#define SPI_CMD_FLEN         0xFFF
#define SPI_CMD_WRITE        0x2     /* Write command */
#define SPI_CMD_READ         0x1     /* Read command */
#define SPI_CMD_WLEN_SHIFT   0x3     /* word length shift */
#define SPI_CMD_8BIT_WLEN    0x7
#define SPI_CMD_16BIT_WLEN   0x15
#define SPI_CMD_CS0          0x0     /* CS0 transfer */
#define SPI_CMD_CS1          0x1     /* CS1 transfer */
#define SPI_CMD_CS2          0x2     /* CS2 transfer */
#define SPI_CMD_CS3          0x3     /* CS3 transfer */
#define SPI_CMD_CS_SHIFT     12
/* status register */
#define SPI_STATUS_BUSY      0x1
#define SPI_STATUS_WC        0x2
#define SPI_STATUS_FC        0x4
#define SPI_STATUS_AE        0x8


/******************************************************
      Function prototypes
******************************************************/
void SPI_RESET(void);
void SPI_INIT(unsigned char clock_divider);
void SPI_READ_WRITE_N_BYTES(unsigned short *buffer, unsigned short count,
               unsigned short *cmdbuffer, unsigned short cmdcount, unsigned short readWrite );

#endif // SPI_H
