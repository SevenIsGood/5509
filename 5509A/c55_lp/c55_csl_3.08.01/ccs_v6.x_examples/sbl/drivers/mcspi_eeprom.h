/********************************************************************* 
*    Property of Texas Instruments Incorporated, Copyright 2011
*    All rights reserved
**********************************************************************
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


/************* Mid Level Media Drivers ****************
 ******************************************************/

int mcspi_eeprom_init(int div);
void mcspi_eeprom_done();
UInt16 mcspi_eeprom_read_n_words(UInt32 address, UInt16 *buffer, UInt16 count);
UInt16 mcspi_eeprom_write_n_words(UInt32 address, UInt16 *buffer, UInt16 count);

/************* Low Level Media Drivers ****************
 ******************************************************/

/* This function reads the status register of the dataflash */
byte mcspi_eeprom_rdsr(unsigned int *err);

unsigned char mcspiFlashChipErase(void);


#endif // MCSPI_EEPROM_H
