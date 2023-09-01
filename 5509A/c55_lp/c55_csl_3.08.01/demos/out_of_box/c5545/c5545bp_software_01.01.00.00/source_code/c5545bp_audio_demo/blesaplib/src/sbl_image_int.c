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
  @headerfile:    sbl_image_ext.c
  Revised:        $Date: 2015-06-22 20:01:12 -0700 (Mon, 22 Jun 2015) $
  Revision:       $Revision: 44190 $

  Description:    This file contains APIs for accessing SBL image for NP when
                  image is contained in internal flash

**************************************************************************************************/


/*********************************************************************
 * INCLUDES
 */

//#include <xdc/std.h>
#include <stdbool.h>

#include "sbl.h"
#include "sbl_image.h"

#include "snp_image.h"
#include <stdio.h>
#include <string.h>

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

static Uint8 flashIsOpen = 0;
extern char filename[20]="file.txt";
char data[4];
Uint32 *bootloder_enable_register;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

void flashRead(Uint32 addr, Uint16 len, Uint8 *pBuf)
{
   Uint8  *flashPtr;
   Uint16 flashIndex;

   flashPtr = (Uint8 *)addr;

   for(flashIndex = 0; flashIndex <= len; flashIndex++)
   {
     /* data fit into buffer */
     pBuf[flashIndex] = *flashPtr++;
   }
}

Uint8 SBL_IMG_open( void )
{
  if (( SNP_Code_sections != 0 ) &&
      ( SNP_Code_length != 0 ) )
  {
    flashIsOpen = 1;
    return SBL_SUCCESS;
  }

  return SBL_FAILURE;
}

bool SBL_IMG_isValid( Uint32 hdrAddr )
{
    if((*bootloder_enable_register)==0xC5FF02C5)
    {
        return true;
    }

  return false;
}

Uint32 SBL_IMG_getSize( Uint32 hdrAddr )
{
  // Verify the flash has been opened
  if ( flashIsOpen )
  {
    // Length in NP_Code_length number of bytes
    return (SNP_Code_length);
  }
  return 0;
}

Uint32 get_imagesize()
{
	FILE *fp;
	int size = 0;
	fp = fopen(filename, "r");
	if (fp == NULL)
		return SBL_FAILURE;
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	//printf(" size of file: %d\n", size);
	fclose(fp);
	return size;
}

Uint32 SBL_IMG_getOffset( Uint32 hdrAddr )
{
  return 0;
}

Uint8 SBL_IMG_read( Uint32 addr, Uint8 *pBuf, Uint16 len)
{
  flashRead(addr, len, pBuf);
  return SBL_SUCCESS;
}

void SBL_IMG_close( void )
{
  flashIsOpen = 0;
}

