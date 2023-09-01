/*
 * Copyright (C) 2003 Texas Instruments Incorporated
 * All Rights Reserved
 */
/*
 *---------gpt3.cmd---------
 */
MEMORY
{
    PAGE 0:

        MMR     : origin = 0000000h, length = 00000BFh 
        DARAM0  : origin = 0000100h, length = 000FDFFh
        DARAM1  : origin = 000FF00h, length = 0000100h
        CE0     : origin = 0010000h, length = 03b0000h 
        CE1     : origin = 0400000h, length = 0400000h
        CE2     : origin = 0800000h, length = 0400000h
        CE3     : origin = 0c00000h, length = 03f8000h
}	

 
SECTIONS
{
        .text    : {} > DARAM0 PAGE 0
        .cinit   : {} > DARAM0 PAGE 0 
        .stack   : {} > DARAM0 PAGE 0
        .sysstack: {} > DARAM0 PAGE 0
        .bss     : {} > DARAM0 PAGE 0
        .sysmem  : {} > DARAM0 PAGE 0 
        .cio     : {} > DARAM0 PAGE 0 
        .data    : {} > DARAM0 PAGE 0
        .const   : {} > DARAM0 PAGE 0 
        .csldata : {} > DARAM0 PAGE 0 
        
		/* interrupt vector table must be on 256 "page" boundry*/
		.vectors : {} > DARAM1 PAGE 0
}
