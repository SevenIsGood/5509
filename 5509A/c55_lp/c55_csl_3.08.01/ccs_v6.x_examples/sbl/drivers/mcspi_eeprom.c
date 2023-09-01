/********************************************************************* 
*    Property of Texas Instruments Incorporated, Copyright 2011
*    All rights reserved
**********************************************************************
*    mcspi_eeprom.c
**********************************************************************/
#include "mcspi_eeprom.h"
#include "mcspi.h"
#include "tipack.h"
#include "user_option.h"

Uint16 commandbuffer[10];

unsigned char mcspiFlashChipErase(void)
{
	unsigned short commandbuffer[4], buffer2[4], status;    
	volatile int i, op_timeout;
	unsigned int errCode;

	/* Set up the write-enable command */
	commandbuffer[0] = MCSPI_EEPROM_WREN;
	errCode = MCSPI_READ_WRITE_N_BYTES(buffer2, 0, commandbuffer, 1, MCSPI_CMD_WRITE);
	if (errCode)
		return errCode;

   //send erase command
    commandbuffer[0] = MCSPI_EEPROM_CHER;
	errCode = MCSPI_READ_WRITE_N_BYTES(buffer2, 0, commandbuffer, 1, MCSPI_CMD_WRITE);
	if (errCode)
		return errCode;

	op_timeout = MCSPI_TIMEOUT;
	do
    {
		/* Wait for the the device to become ready */
		status = mcspi_eeprom_rdsr(&errCode);
		if (errCode)
			return errCode;
		op_timeout--;
		if (op_timeout<=0)
			return MCSPI_ERROR_TIMEOUT;
	} while( (status & 0x01) != 0x00);
    return (0);
 
}

/********************************************************************* 
* mcspi_eeprom_init
********************************************************************/
int mcspi_eeprom_init(int div)
{
   /* Use sys-clk divider of 25 for real-system (keep MCSPI-clk under 500kHz) */
   MCSPI_INIT(div);

   return 0;
}

/********************************************************************* 
* mcspi_eeprom_done
********************************************************************/
void mcspi_eeprom_done()
{
   MCSPI_RESET();
}

/********************************************************************* 
* mcspi_eeprom_read_n_words
* This function reads N words from the Dataflash.
* It functions by sending out the 64-bit command with
* the command unpacked byte by byte in commandbuffer.
* The function then triggers the CLK by calling SPI_READ_WRITE_N_BYTES
* and the results of the read are placed within
* buffer2.  Finally, buffer2 is packed back into buffer.
********************************************************************/
UInt16 mcspi_eeprom_read_n_words(UInt32 address, UInt16 *buffer, UInt16 count)
{
	Uint32 byte_address = address*2;
	unsigned short commandbuffer[4];     /* Buffer for unpacked command */
	unsigned int errCode;

	commandbuffer[0] = (MCSPI_EEPROM_READ<<8) | (byte_address >> 16);
	commandbuffer[1] = (byte_address & 0xFFFF);
	errCode = MCSPI_READ_WRITE_N_BYTES(buffer, count, commandbuffer, 2, MCSPI_CMD_READ);

	if (errCode)
		return errCode;

   return 0;
}

/********************************************************************* 
* mcspi_eeprom_rdsr
********************************************************************/
byte mcspi_eeprom_rdsr(unsigned int *err)
{
   unsigned short commandbuffer = MCSPI_EEPROM_RDSR;
	unsigned short data[2];
	unsigned int	status;

   /* Send out the RDSR command and read the RDSR results */
	status = MCSPI_READ_WRITE_N_BYTES(data, 1, &commandbuffer, 1, MCSPI_CMD_READ);
	if (status)
	{
		*err = status;
		return 0;
	}

	*err = 0;
	return data[0];
}
