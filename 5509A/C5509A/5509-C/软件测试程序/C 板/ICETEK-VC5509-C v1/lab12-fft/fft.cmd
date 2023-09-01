/*
 *  Copyright 2001 by Texas Instruments Incorporated.
 *  All rights reserved. Property of Texas Instruments Incorporated.
 *  Restricted rights to use, duplicate or disclose this code are
 *  granted through contract.
 */
/*
 *  ======== volume.cmd ========
 *
 */
-w
-stack 500
-sysstack 500
-l rts55.lib

MEMORY {
   DATA:       origin = 0xe000,        len = 0x10000
   PROG:       origin = 0x200,         len = 0x5e00
   VECT:       origin = 0xd000,        len = 0x100
}

SECTIONS
{
    .vectors: {} > VECT
    .trcinit: {} > PROG
    .gblinit: {} > PROG
     frt:     {} > PROG
    .text:    {} > PROG
    .cinit:   {} > PROG
    .pinit:   {} > PROG
    .sysinit: {} > PROG
    .bss:     {} > DATA
    .far:     {} > DATA
    .const:   {} > DATA
    .switch:  {} > DATA
    .sysmem:  {} > DATA
    .cio:     {} > DATA
    .MEM$obj: {} > DATA
    .sysheap: {} > DATA
    .stack:   {} > DATA
    .sysstack {} > DATA
    .twiddle32 {} > DATA
}
