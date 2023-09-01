/********************************************************************* 
*    Property of Texas Instruments Incorporated, Copyright 2011
*    All rights reserved
**********************************************************************
*    mcspi.h
**********************************************************************/
#ifndef MCSPI_H
#define MCSPI_H

/********************************************************************* 
* Bit-field definitions for MCSPI registers...
**********************************************************************/

/* clock regsiter */
#define MCSPI_CLK_ENABLE       0x8000
/* device config register */
/* command register */
#define MCSPI_CMD_WIRQ         0x4000
#define MCSPI_CMD_FIRQ         0x8000
#define MCSPI_CMD_FLEN         0xFFF
#define MCSPI_CMD_WRITE        0x2     /* Write command */
#define MCSPI_CMD_READ         0x1     /* Read command */
#define MCSPI_CMD_WLEN_SHIFT   0x3     /* word length shift */
#define MCSPI_CMD_8BIT_WLEN    0x7
#define MCSPI_CMD_16BIT_WLEN   0x15
#define MCSPI_CMD_CS0          0x0     /* CS0 transfer */
#define MCSPI_CMD_CS1          0x1     /* CS1 transfer */
#define MCSPI_CMD_CS2          0x2     /* CS2 transfer */
#define MCSPI_CMD_CS3          0x3     /* CS3 transfer */
#define MCSPI_CMD_CS_SHIFT     12
/* status register */
#define MCSPI_STATUS_BUSY      0x1
#define MCSPI_STATUS_WC        0x2
#define MCSPI_STATUS_FC        0x4
#define MCSPI_STATUS_AE        0x8

#define MCSPI_TIMEOUT	         30000
#define MCSPI_ERROR_TIMEOUT	     0x0004

/******************************************************
      Function prototypes
******************************************************/
void MCSPI_RESET(void);
Uint16 MCSPI_INIT(unsigned int clock_divider);
Uint16 MCSPI_READ_WRITE_N_BYTES(unsigned short *buffer, unsigned short count, 
               unsigned short *cmdbuffer, unsigned short cmdcount, unsigned short readWrite );

#endif // MCSPI_H
