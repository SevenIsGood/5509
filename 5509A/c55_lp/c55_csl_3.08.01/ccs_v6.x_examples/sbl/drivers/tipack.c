/********************************************************************* 
*    Property of Texas Instruments Incorporated, Copyright 2008
*    All rights reserved
**********************************************************************
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
