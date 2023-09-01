/********************************************************************* 
*    Property of Texas Instruments Incorporated, Copyright 2008
*    All rights reserved
**********************************************************************
*    tipack.h
**********************************************************************/
#ifndef TIPACK_H
#define TIPACK_H

#include "machine.h"

/******************************************************************************
* TI_unpack
* This routine takes packed data in 'data' and unpacks
* it into unsigned chars in 'buffer'.  This routine is
* central to solve the problems of packed/unpacked 8-bit
* values on the 16-bit, non-byte addressable DSP.
******************************************************************************/
void TI_unpack(unsigned char *buffer, const void *data, unsigned int count,
               unsigned int offset);

/******************************************************************************
* TI_pack
* This routine takes unpacked data in unsigned chars
* and packs them into UInt16s.
******************************************************************************/
void TI_pack(UInt16 *data, const unsigned char *buffer, unsigned int count);

/******************************************************************************
* TI_unpack_words
* This routine takes packed data in 'data' and unpacks it into 'buffer'.
* This routine only handles an even number of bytes.
* 'packedWords' will be read from 'data'.
* 'buffer' will be written with (2 * 'packedWords') values.
******************************************************************************/
void TI_unpack_words(UInt16 *buffer, UInt16 *data, unsigned int packedWords);

/******************************************************************************
* TI_pack_words
* This routine takes unpacked data in 'buffer' packs it into 'data'.
* This routine only handles an even number of bytes.
* (2 * 'packedWords') will be read from 'buffer'.
* 'data' will be written with 'packedWords' values.
******************************************************************************/
void TI_pack_words(UInt16 *data, UInt16 *buffer, unsigned int packedWords);

#endif /* TIPACK_H */
