/*
 * Copyright (C) 2003 Texas Instruments Incorporated
 * All Rights Reserved
 */
/*
 *---------i2c2.cmd---------
 *
 */

MEMORY
{
    PAGE 0:

        MMR     : origin = 0000000h, length = 00000c0h 
        VECS    : origin = 0000100h, length = 0000100h
        
        DARAM0  : origin = 0000200h, length = 0003E00h
        DARAM1  : origin = 0004000h, length = 0004000h
        DARAM2  : origin = 0008000h, length = 0004000h
        DARAM3  : origin = 000c000h, length = 0004000h
        
        CE0     : origin = 0050000h, length = 03b0000h 
        CE1     : origin = 0400000h, length = 0400000h
        CE2     : origin = 0800000h, length = 0400000h
        CE3     : origin = 0c00000h, length = 03f8000h
}       
 
SECTIONS
{
        .vectors : {} > VECS   PAGE 0         /* interrupt vector table */
        .cinit   : {} > DARAM0 PAGE 0
        .text    : {} > DARAM1 PAGE 0
        .stack   : {} > DARAM0 PAGE 0
        .sysstack: {} > DARAM0 PAGE 0
        .sysmem  : {} > DARAM2 PAGE 0
        .data    : {} > DARAM1 PAGE 0
        .bss     : {} > DARAM2 PAGE 0
        .const   : {} > DARAM2 PAGE 0
		.cio     : {} > DARAM2 PAGE 0
        .csldata:  {} > DARAM0 PAGE 0		
}
