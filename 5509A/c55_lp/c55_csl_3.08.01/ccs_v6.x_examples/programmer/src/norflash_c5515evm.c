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


/*
 *  Flash Setup
 *
 */

#include "evm5515_emif.h"
#include "evm5515.h"
#include "norflash_c5515evm.h"

static Uint32 device_flash_pages = DEV_INTEL_128_TOP_PAGES;
static Uint32 device_flash_page_length = 0x20000;
static Uint32 device_flash_page_len_pow2 = 17;

/* INTEL_MICRON */
static Int32 device_page_alignment = 0;


/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  norflash_init( )                                                        *
 *                                                                          *
 *      Setup Flash                                                         *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 norflash_init( )
{
	Int16 i;

	/* Reset Counter value */
	SYS_PRCNTR = 0x020;
	/* Reset all Modules */
	SYS_PRCNTRLR = 0x00fb;
	for(i=0; i<50; i++);

	/* Config EMIF - ASYNC-Wait Config Regsiters */
	EMIF_AWCCR1 = 0x0080;
	EMIF_AWCCR2  = 0x00E4;
	return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _FLASH_unlockBlocks( )                                                  *
 *                                                                          *
 *      Flash unlock blocks ( For Intel/Micron Flash )                          *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 _FLASH_unlockBlocks( Uint32 startAddr, Uint32 len )
{
    Uint32 endAddr = startAddr + len - 1;
    Uint32 tempAddr;
    volatile Uint16 *blockPtr;
    for ( tempAddr = startAddr ; tempAddr <= endAddr ; tempAddr = _FLASH_nextPage( tempAddr ) )
            {
		blockPtr = ( volatile Uint16* )tempAddr;
		// Block lock setup
		*blockPtr = FLASH_CONFIG_BLOCK_LOCK_BITS;
		// Unlock block
		*blockPtr = FLASH_CLEAR_LOCK_BITS;
		// set to read mode
		*blockPtr = FLASH_READ_ARRAY;
    }
    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _FLASH_waitWhileBusy( )                                                 *
 *                                                                          *
 *      Wait for the Busy bit                                               *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 _FLASH_waitWhileBusy( Uint32 timeout )
{
    /* Read Status Register */
    FLASH_BASE_PTR16 = FLASH_READ_STATUS;
    /*
     * Wait on the busy bit in the status register
     *
     * The variable timeout, a misnomer b/c it counts the number of
     * loop iterations to check the status, is used to avoid infinite loops!
     */
    while ( timeout-- > 0 )
    {
        /*
         *  The most important bit in the status is the busy bit.
         *  When asserted the operation is done.
         */
        if ( ( FLASH_BASE_PTR16 & 0x0080 )
            || ( FLASH_BASE_PTR16 & 0x8000 ) )
         {
            /*
             * Check for any errors
             */
            if ( ( ( FLASH_BASE_PTR16 & 0x007F ) != 0 )
                || ( ( FLASH_BASE_PTR16 & 0x7F00 ) != 0 ) )
            {
                /* Bad, operation finished w/ errors */
                FLASH_BASE_PTR16 = FLASH_READ_ARRAY;
                return -1;
            }
            else
            {
                /* Good, operation is finished w/o errors */
                FLASH_BASE_PTR16 = FLASH_READ_ARRAY;
                return 0;
            }
        }
    }
    /* Timeout occured */
    FLASH_BASE_PTR16 = FLASH_READ_ARRAY;

    return 1;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _FLASH_checksum( start, length )                                        *
 *      Compute the checksum from ( start ) to ( start + length ).  The     *
 *      checksum is a 32-bit cumulative sum of each 8-bit byte in the       *
 *      range.                                                              *
 *                                                                          *
 *      start  <- starting address                                          *
 *      length <- length in bytes                                           *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Uint32 norflash_checksum( Uint32 start, Uint32 length )
{
    Uint32 i;
    Uint32 checksum32 = 0;
    Uint8* ptr = ( Uint8* )start;

    /*
     *  Set to Read Mode
     */
    FLASH_BASE_PTR16 = FLASH_READ_ARRAY;

    /*
     *  Compute Checksum by adding each byte up
     */
    for ( i = 0 ; i < length ; i++ )
        checksum32 += *ptr++;

    return checksum32;
}

Uint32 erase_timeout = 0x08000000;

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  norflash_erase( start, length )                                         *
 *      Erase Flash containing address ( start ) to ( start + length ).     *
 *      Flash can only erase entire blocks containing the range.            *
 *                                                                          *
 *      start  <- starting address                                          *
 *      length <- length in bytes                                           *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int16 norflash_erase( Uint32 start, Uint32 length )
{
    Uint32 addr;
    Uint32 end;
    volatile Uint16 *addr16;

    end = start + length - 1;                   // Calculate end of range
    start &= 0xFFFFFFFE;                        // Align to 16-bit words

    /*
     *  Erase each Flash block in the range
     */
    _FLASH_unlockBlocks(start, length);
    for ( addr = start ; addr <= end ; addr = _FLASH_nextPage( addr ) )
    {
        addr16 = ( volatile Uint16* )addr;
        *addr16 = FLASH_READ_ARRAY;          // Reset Flash
        *addr16 = FLASH_BLOCK_ERASE;         // Erase block command
        *addr16 = FLASH_ERASE_PROGRAM_RESUME;// Erase block confirm

        if ( _FLASH_waitWhileBusy( erase_timeout ) )
        {
            *addr16 = FLASH_READ_ARRAY;
            return -1;                      // Timeout/Error occured
        }
    }
    *addr16 = FLASH_READ_ARRAY;
    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _FLASH_getId( )                                                         *
 *                                                                          *
 *      Who am I?  Intel/Micron based Flash or AMD/Spansion based Flash     *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Uint16 _FLASH_getId( Uint16* id)
{
    Uint32 addr = FLASH_BASE;

    volatile Uint16* addr16  = ( Uint16* )addr;
    volatile Uint16* pmfgid  = ( Uint16* )addr;
    volatile Uint16* pdevid1 = ( Uint16* )( addr + 0x1 );

    ///volatile Uint16* flash_ctl555 = ( Uint16* )( addr + 0x555 );
    ///volatile Uint16* flash_ctl2AA = ( Uint16* )( addr + 0x2AA );

    *addr16 = FLASH_READ_ARRAY;         // Read Mode
    *addr16 = FLASH_READ_ID;            // Read ID Mode

    *id++ = *pmfgid;          // Read MFG_ID
    *id++ = *pdevid1;         // Read DEV_ID

    *addr16 = FLASH_READ_ARRAY;
    return 0;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _FLASH_getPageLength( )                                                 *
 *                                                                          *
 *      What is the page Length?                                            *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Int32 _FLASH_getPageLength( )
{
    return device_flash_page_length;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _FLASH_nextPage( )                                                      *
 *                                                                          *
 *      Get Next Page of Flash.  This is only used b/c the Intel Flash being*
 *      contains 4 smaller pages in the TOP or BOTTOM page.  This breaks    *
 *      the assumption that each page is the same size, thus we need to     *
 *      compute what the next page is.                                      *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Uint32 _FLASH_nextPage( Uint32 addr )
{
    Uint32 flash_offset;        // Offset in Flash
    Uint32 flash_major_page;    // Full size page
    Uint32 flash_minor_page;    // Quarter size page

    /* Go to first page */
    if ( addr < FLASH_BASE )
        return FLASH_BASE;

    /* Last Page, no where left to go */
    if ( addr >= FLASH_END_OR_RANGE )
        return FLASH_END_OR_RANGE;

    /* Compute offset and page */
    flash_offset = addr - FLASH_BASE;
    flash_major_page = flash_offset >> device_flash_page_len_pow2;
    flash_minor_page = flash_offset >> ( device_flash_page_len_pow2 - 2 );
    flash_minor_page &= 3;

    /* TOP side page alignment */
    if ( device_page_alignment == FLASH_DEVICE_TOP_PAGES )
    {
        if ( flash_major_page >= ( device_flash_pages - 1 ) )
        {
            addr = FLASH_BASE;
            addr += flash_major_page << device_flash_page_len_pow2;
            addr += ( flash_minor_page + 1 ) << ( device_flash_page_len_pow2 - 2 );
        }
        else
        {
            addr = FLASH_BASE;
            addr += ( flash_major_page + 1 ) << device_flash_page_len_pow2;
        }
    }
    /* BOTTOM side page alignment */
    else if ( device_page_alignment == FLASH_DEVICE_BOTTOM_PAGES )
    {
        if ( flash_major_page < 1 )
        {
            addr = FLASH_BASE;
            addr += ( flash_minor_page + 1 ) << ( device_flash_page_len_pow2 - 2 );
        }
        else
        {
            addr = FLASH_BASE;
            addr += ( flash_major_page + 1 ) << device_flash_page_len_pow2;
        }
    }
    else
    {
        addr = FLASH_BASE;
        addr += ( flash_major_page + 1 ) << device_flash_page_len_pow2;
    }
    return addr;
}

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  norflash_read( src, dst, length )                                         *
 *      Read from Flash address ( src ) to the data at non-Flash address    *
 *      ( dst ) for ( length ) bytes.                                       *
 *                                                                          *
 *      src     <- source address                                           *
 *      dest    <- destination address                                      *
 *      length  <- length in bytes                                          *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Uint16 norflash_read( Uint32 src, Uint16* dst, Uint16 length )
{
    Uint32 i;
    Uint16* psrc16 = ( Uint16* )(src+NOR_FLASH_BASE_ADDRESS_CS2);
    Uint16* pdst16 = ( Uint16* )dst;

    /*
     *  Set to Read Mode
     */
    FLASH_BASE_PTR16 = FLASH_READ_ARRAY;

    /*
     *  Read Data to Buffer
     */
    for ( i = 0 ; i < length ; i++ )
        *pdst16++ = *psrc16++;

    return 0;
}

Uint32 write_timeout = 0x00010000;

/* ------------------------------------------------------------------------ *
 *                                                                          *
 *  _FLASH_write( src, dst, length )                                        *
 *      Write to Flash address [dst] the data at a non-Flash address [src]  *
 *      for [length] #bytes.                                                *
 *                                                                          *
 *      src     <- source address                                           *
 *      dest    <- destination address                                      *
 *      length  <- length in bytes                                          *
 *                                                                          *
 * ------------------------------------------------------------------------ */
Uint16 norflash_write( Uint32 src, Uint16* dst, Uint16 length )
{
    Uint32 i;
    Uint16* psrc16;
    Uint16* pdst16;

    /*
     *  Align to 8 or 16 bits
     */
    psrc16 = ( Uint16* )dst;
    pdst16 = ( Uint16* )(src+NOR_FLASH_BASE_ADDRESS_CS2);
    length &= 0xFFFFFFFF;

    for ( i = 0 ; i < length ; i++)
    {
        /* Program one 16-bit word */
        *pdst16 = FLASH_WORD_PROGRAM;
        *pdst16 = *psrc16++;

        /* Wait for operation to complete */
        if ( _FLASH_waitWhileBusy( write_timeout ) )
            return -1;

        pdst16++;
    }
    return 0;
}

