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
*    ata_nand_io.c
**********************************************************************/
#include "registers.h"
#include "ata_nand.h"
#include "nand.h"

Uint16 NandFlashChipSelect = NAND_FLASH_CS2;
extern NandStruct *NandFlashCtrl;


/******************************************************************************
* AtaNANDDataReadyStatusCheck
******************************************************************************/
int AtaNANDDataReadyStatusCheck(void *pAtaMediaState)
{
   unsigned short NandFlashStatus;

   pAtaMediaState = pAtaMediaState; /* prevent compiler warning for unused parameter */
   NandDeselect();

   /* The NAND's R/B- pin is attached to the RDY-pin corresponding to the ChipSelect used */
   /* Use NandFlashStatus register bits [3..0] */
   NandFlashStatus = IOPORT_REG_EMIF_NAND_STATUS;
   return ((NandFlashStatus & NandFlashChipSelect));
}

/******************************************************************************
* NandConfig
******************************************************************************/
int NandConfig(Uint16 ChipSelect)
{
   /* Access EMIF registers as 16-bit registers */
   IOPORT_REG_EMIF_CNTL = 0x0000;

   /* Configure ASYNC Wait config Registers */
   IOPORT_REG_EMIF_ASYNC_WAIT_0 = 0x0080; // max_ext_wait = 0x80
   IOPORT_REG_EMIF_ASYNC_WAIT_1 = 0x00E4; // wp(all) = 0

   /* Initialize NAND as deselected */
   IOPORT_REG_EMIF_NAND_CTRL = 0;

   /* Configure as 8-bit data bus */
   switch (ChipSelect)
   {
   case NAND_FLASH_CS2:
      IOPORT_REG_EMIF_ASYNC1_CONFIG_0 = 0xFFFC;
      IOPORT_REG_EMIF_ASYNC1_CONFIG_1 = 0x3FFF;
      NandFlashCtrl = (NandStruct *)NAND_FLASH_BASE_ADDRESS_CS2;
      break;
   case NAND_FLASH_CS3:
      IOPORT_REG_EMIF_ASYNC2_CONFIG_0 = 0xFFFC;
      IOPORT_REG_EMIF_ASYNC2_CONFIG_1 = 0x3FFF;
      NandFlashCtrl = (NandStruct *)NAND_FLASH_BASE_ADDRESS_CS3;
      break;
   case NAND_FLASH_CS4:
      IOPORT_REG_EMIF_ASYNC3_CONFIG_0 = 0xFFFC;
      IOPORT_REG_EMIF_ASYNC3_CONFIG_1 = 0x3FFF;
      NandFlashCtrl = (NandStruct *)NAND_FLASH_BASE_ADDRESS_CS4;
      break;
   case NAND_FLASH_CS5:
      IOPORT_REG_EMIF_ASYNC4_CONFIG_0 = 0xFFFC;
      IOPORT_REG_EMIF_ASYNC4_CONFIG_1 = 0x3FFF;
      NandFlashCtrl = (NandStruct *)NAND_FLASH_BASE_ADDRESS_CS5;
      break;
   default:
      return 1; /* ERROR: Invalid chip-select */
   }
   NandFlashChipSelect = ChipSelect;

   /* Set EMIF to Byte access (required for data transfer to NAND) */
   IOPORT_REG_EMIF_CNTL = 0x0002;
   return 0;
}

/******************************************************************************
* NandSelect
******************************************************************************/
void NandSelect()
{
   IOPORT_REG_EMIF_NAND_CTRL |= NandFlashChipSelect; /* set the bit to select */
}

/******************************************************************************
* NandDeselect
******************************************************************************/
void NandDeselect()
{
   IOPORT_REG_EMIF_NAND_CTRL &= ~NandFlashChipSelect; /* clear the bit to deselect */
}
