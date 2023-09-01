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
*    mcspi_eeprom.h
**********************************************************************/
#ifndef MCSPI_EEPROM_H
#define MCSPI_EEPROM_H

#include "machine.h"

#define MCSPI_EEPROM_PAGE_SIZE    0x20

/************* SPI EEPROM COMMANDS *****************/
#define MCSPI_EEPROM_READ       0x03  /* Read command */
#define MCSPI_EEPROM_WREN       0x06  /* Enable Write Operation */
#define MCSPI_EEPROM_WRDI       0x04  /* Disable Write Operation */
#define MCSPI_EEPROM_RDSR       0x05  /* Read Status Register */
#define MCSPI_EEPROM_WRSR       0x01  /* Write Status Register */
#define MCSPI_EEPROM_WRITE      0x02  /* Write command */
#define MCSPI_EEPROM_CHER		0xC7	/* chip erase command */
#define MCSPI_EEPROM_READID     0x90  /* Read device ID */
#define MCSPI_EEPROM_WRSREN     0x50  /* enable Write Status Register */

/************* Mid Level Media Drivers ****************
 ******************************************************/

int mcspi_eeprom_init(int div);
void mcspi_eeprom_done();
UInt16 mcspi_eeprom_read_n_words(UInt32 address, UInt16 *buffer, UInt16 count);
UInt16 mcspi_eeprom_write_n_words(UInt32 address, UInt16 *buffer, UInt16 count);

/************* Low Level Media Drivers ****************
 ******************************************************/

/* This function reads the status register of the dataflash */
byte mcspi_eeprom_rdsr();
byte mcspi_eeprom_wrsr(unsigned int data);
byte mcspi_read(UInt32 address);
void mcspi_write(UInt16 data, UInt32 address);
void mcspi_read_id(UInt16 *id);

unsigned char mcspiFlashChipErase(void);


#endif // MCSPI_EEPROM_H
