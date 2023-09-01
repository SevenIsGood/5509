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
*    tipack.c
**********************************************************************/
#include "tipack.h"

/******************************************************************************
* TI_unpack
* This routine takes packed data in 'data' and unpacks
* it into unsigned chars in 'buffer'.  This routine is
* central to solve the problems of packed/unpacked 8-bit
* values on the 16-bit, non-byte addressable DSP.
******************************************************************************/
void TI_unpack(unsigned char *buffer, const void *data, unsigned int count,
               unsigned int offset)
{
    unsigned int i;

    for(i=0; i<count; i++)
    {
        buffer[i] = (unsigned char)
        (
            *((unsigned char *)data+((offset+i)/BYTES_PER_CHAR)) >>  /* unsigned char is smallest alloc unit */
#ifdef LITTLE_ENDIAN
            (((offset+i)%BYTES_PER_CHAR) * 8)
#else
            ((BYTES_PER_CHAR - ((offset+i)%BYTES_PER_CHAR) - 1) * 8)
#endif
        ) & 0xFF;
    }
}

/******************************************************************************
* TI_pack
* This routine takes unpacked data in unsigned chars
* and packs them into UInt16s.
******************************************************************************/
void TI_pack(UInt16 *data, const unsigned char *buffer, unsigned int count)
{
    unsigned int i;

    for(i=0; i<(count/2); i++)
    {
        data[i] = 0;
    }

    for(i=0; i<count; i++)
    {
        data[i/2] |=
        (
            /* UInt16s may be directly allocated on all our target processors */
            ((UInt16) (buffer[i] & 0xFF)) <<
#ifdef LITTLE_ENDIAN
            ((i%2) * 8)
#else
            ((1 - (i%2)) * 8)
#endif
        );
    }
}


/******************************************************************************
* TI_unpack_words
* This routine takes packed data in 'data' and unpacks it into 'buffer'.
* This routine only handles an even number of bytes.
* 'packedWords' will be read from 'data'.
* 'buffer' will be written with (2 * 'packedWords') values.
******************************************************************************/
void TI_unpack_words(UInt16 *buffer, UInt16 *data, unsigned int packedWords)
{
   unsigned int i;

   for (i=0; i<packedWords; i++)
   {
      *buffer++ = *data >> 8;
      *buffer++ = *data & 0xFF;
      *data++;
   }
}

/******************************************************************************
* TI_pack_words
* This routine takes unpacked data in 'buffer' packs it into 'data'.
* This routine only handles an even number of bytes.
* (2 * 'packedWords') will be read from 'buffer'.
* 'data' will be written with 'packedWords' values.
******************************************************************************/
void TI_pack_words(UInt16 *data, UInt16 *buffer, unsigned int packedWords)
{
   unsigned int i;

   for (i=0; i<packedWords; i++)
   {
      *data = (*buffer++) << 8;
      *data |= (*buffer++) & 0xFF;
      *data++;
   }
}
