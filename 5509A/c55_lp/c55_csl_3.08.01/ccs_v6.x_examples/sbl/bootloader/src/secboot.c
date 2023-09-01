/********************************************************************* 
*    Property of Texas Instruments Incorporated, Copyright 2007
*    All rights reserved
**********************************************************************
*    secboot.c
**********************************************************************
* Secure Bootloader for the C5505.  The bootloader performs a
* secure bootload from whichever media device was detected by the
* secure bootloader shell.  The shell passes in a pointer to the
* driver that will be needed to read a word from the media, and also
* the key that was selected by the shell to decrypt with.  From this
* point forward, the bootloader performs an anonymous secure boot
* from the media using the safer decryption algorithm as adapted to
* the C5xx DSPs.  When the bootload is finished, the entry point is
* returned to the shell so that the shell can finish up any other
* media/security items and branch to the loaded code.  An entry 
* point value of 0 indicates a failure.
* The shell will also notify this routine if re-authoring should
* be performed, or if the image is insecure.
**********************************************************************/
#include "tistdtypes.h"
#include "machine.h"
#include "soc.h"
#include "tipack.h"
#include "secboot.h"
#include "user_option.h"

extern Uint16	emifAddrGPIOFlag;

/* Local Function Prototypes */
void RegisterConfiguration(UInt16 *Data);
UInt32 InsecureBoot(fpReadNWords read_n_words, UInt32 address);


/******************************************************************************
* RegisterConfiguration
******************************************************************************/
void RegisterConfiguration(UInt16 *Data)
{
   ioport UInt16 *ioptr;
   UInt16 DelayCycles;
   UInt16 EmifCtrl;

   if (Data[0] != 0xFFFF)
   {
      /* Get register to write to */
      ioptr = (UInt16 *)Data[0];
      /* Check for EMIF register (0x10XX) */
      if ((Data[0] & 0xFF00) == 0x1000)
      {
         /* Save original state of EMIF_CTRL */
		 EmifCtrl = CSL_SYSCTRL_REGS->ESCR;
         /* Force 16-bit access to EMIF registers */
		 CSL_SYSCTRL_REGS->ESCR = 0x0000;
         /* Write data to register */
         *ioptr = Data[1];
         /* Restore original state of EMIF_CTRL */
         CSL_SYSCTRL_REGS->ESCR = EmifCtrl;
      }
      else
      {
         /* Write data to register */
         *ioptr = Data[1];
      }
   }
   else
   {
      /* Delay by specified number of cycles */
      for (DelayCycles=Data[1]; DelayCycles!=0; DelayCycles--)
      {
         asm("\tnop");
      }
   }
}

/******************************************************************************
* InsecureBoot
******************************************************************************/
UInt32 InsecureBoot(fpReadNWords read_n_words, UInt32 address)
{
#ifdef USE_32WORD_BLOCK
   UInt16 in_block[64];   /* Block to decrypt */
#else
   UInt16 in_block[4];   /* Block to decrypt */
#endif
   UInt32 entry_point;
   UInt16 count;
   UInt16 size;
   UInt32 addr;

   /****************************************************************
   * Entry Point
   ****************************************************************/
   if (read_n_words(address, &in_block[0], 2))
	return 0;
   address += 2;
   entry_point = ((unsigned long)in_block[0] << 16);
   entry_point |= ((unsigned long)in_block[1]);

   /****************************************************************
   * Register Configuration
   ****************************************************************/
   /* Read the register configuration count */
   if (read_n_words(address, &count, 1))
	return 0;
   address += 1;

   /* Handle register configuration */
   for (; count!=0; count--)
   {
      /* Read the register address and data */
      if (read_n_words(address, &in_block[0], 2))
		return 0;
      address += 2;

      /* Process this register configuration */
      RegisterConfiguration(in_block);
   }

   /****************************************************************
   * Sections (to copy)
   ****************************************************************/
   /* Read the first section's size */
   if (read_n_words(address, &size, 1))
	return 0;
   address += 1;

   while (size)
   {
      /* Read the destination address (2 words) and the first 2 data words */
      /* NOTE: The 2 destination address words do not count as part of "size" */
      if (read_n_words(address, &in_block[0], 4))
		return 0;
      address += 4;
      addr = ((unsigned long)in_block[0] << 16);
      addr |= ((unsigned long)in_block[1]);

      /* Process the 2 data words */
      count = 2; // words to copy
      if (count > size)
         count = size;

  	  // set the emifAddrGPIOFlag if needed
	  if (emifAddrGPIOFlag==1)
	  {
	    // if the destination address is in EMIF address space
		if (addr>=0x28000)
		{
			// set the EBSR[5:0] to 0 (set those pins from GPIO to EMIF address)
			CSL_SYSCTRL_REGS->EBSR &= ~(0x003F);
			// set emifAddrGPIOFlag to 0
			emifAddrGPIOFlag = 0;
		}
	  }

      memory_write_n_words(addr, &in_block[2], count);
      addr += count;
      size -= count;

      while (size)
      {
#ifdef USE_32WORD_BLOCK
		if (size>=32)
		{
	         /* Read the next 32 data words */
	         read_n_words(address, &in_block[0], 32);
	         address += 32;
	
	         /* Process the 32 data words */
	         count = 32; // words to copy
	         if (count > size)
	            count = size;
	         memory_write_n_words(addr, &in_block[0], count);
	         addr += count;
	         size -= count;
		} else if (size>=16)
		{
	         /* Read the next 16 data words */
	         read_n_words(address, &in_block[0], 16);
	         address += 16;
	
	         /* Process the 16 data words */
	         count = 16; // words to copy
	         if (count > size)
	            count = size;
	         memory_write_n_words(addr, &in_block[0], count);
	         addr += count;
	         size -= count;
		} else if (size>=8)
		{
	         /* Read the next 8 data words */
	         read_n_words(address, &in_block[0], 8);
	         address += 8;
	
	         /* Process the 8 data words */
	         count = 8; // words to copy
	         if (count > size)
	            count = size;
	         memory_write_n_words(addr, &in_block[0], count);
	         addr += count;
	         size -= count;
		} else
		{
	         /* Read the next 4 data words */
	         read_n_words(address, &in_block[0], 4);
	         address += 4;
	
	         /* Process the 4 data words */
	         count = 4; // words to copy
	         if (count > size)
	            count = size;
	         memory_write_n_words(addr, &in_block[0], count);
	         addr += count;
	         size -= count;
		}
#else
         /* Read the next 4 data words */
         if (read_n_words(address, &in_block[0], 4))
			return 0;
         address += 4;

         /* Process the 4 data words */
         count = 4; // words to copy
         if (count > size)
            count = size;
         memory_write_n_words(addr, &in_block[0], count);
         addr += count;
         size -= count;
#endif
      }

      /* Read the next section's size */
      if (read_n_words(address, &size, 1))
		return 0;
      address += 1;
   }

   return (entry_point);
}


/******************************************************************************
* secBoot
* This function handles receiving most of the boot-image, and reauthoring
* if necessary.
* A return value of 0 indicates an error occurred.
* A non-zero return value is the entry-point for the received application.
******************************************************************************/
UInt32 secBoot(fpReadNWords read_n_words, fpWriteNWords write_n_words,
                UInt32 address, UInt16 *seed, UInt16 flags)
{
   UInt32        entry_point;

   /* Check for Insecure boot image */
   if (flags == SECBOOT_FLAGS_INSECURE)
   {
      entry_point = InsecureBoot(read_n_words, address);
      return (entry_point);
   }

   return (0);
}
